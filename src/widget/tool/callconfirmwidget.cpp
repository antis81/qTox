/*
    Copyright © 2015 by The qTox Project Contributors

    This file is part of qTox, a Qt-based graphical interface for Tox.

    qTox is libre software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    qTox is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with qTox.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "callconfirmwidget.h"

#include "core/core.h"
#include "core/coreav.h"
#include "widget/widget.h"

#include <assert.h>

#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPolygon>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QPushButton>
#include <QRect>

/**
 * @class CallConfirmWidget
 * @brief This is a widget with dialog buttons to accept/reject a call
 *
 * It tracks the position of another widget called the anchor
 * and looks like a bubble at the bottom of that widget.
 *
 * @var const QWidget* CallConfirmWidget::anchor
 * @brief The widget we're going to be tracking
 *
 * @var const Friend& CallConfirmWidget::f
 * @brief The friend on whose chat form we should appear
 *
 * @var const int CallConfirmWidget::roundedFactor
 * @brief By how much are the corners of the main rect rounded
 *
 * @var const qreal CallConfirmWidget::rectRatio
 * @brief Used to correct the rounding factors on non-square rects
 */

CallConfirmWidget::CallConfirmWidget(QWidget *Anchor, quint32 friendId,
                                     QWidget* parent)
    : QWidget(parent)
    , anchor(Anchor)
    , mFriendId(friendId)
    , brush(QColor(65,65,65))
    , rectW{120}
    , rectH{85}
    , spikeW{30}
    , spikeH{15}
    , roundedFactor{20}
    , rectRatio(static_cast<qreal>(rectH)/static_cast<qreal>(rectW))
{
    setWindowFlags(Qt::SubWindow);
    setAttribute(Qt::WA_DeleteOnClose);

    QPalette palette;
    palette.setColor(QPalette::WindowText, Qt::white);
    setPalette(palette);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *callLabel = new QLabel(QObject::tr("Incoming call..."), this);
    callLabel->setStyleSheet("color: white");
    callLabel->setWordWrap(true);
    callLabel->setAlignment(Qt::AlignHCenter);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    QPushButton *accept = new QPushButton(this), *reject = new QPushButton(this);
    accept->setFlat(true);
    reject->setFlat(true);
    accept->setStyleSheet("QPushButton{border:none;}");
    reject->setStyleSheet("QPushButton{border:none;}");
    accept->setIcon(QIcon(":/ui/acceptCall/acceptCall.svg"));
    reject->setIcon(QIcon(":/ui/rejectCall/rejectCall.svg"));
    accept->setIconSize(accept->size());
    reject->setIconSize(reject->size());

    buttonBox->addButton(accept, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(reject, QDialogButtonBox::RejectRole);

    connect(buttonBox, &QDialogButtonBox::accepted, this, [this]()
    {
        closeHelper();
        emit accepted();
    });
    connect(buttonBox, &QDialogButtonBox::rejected, this, [this]()
    {
        closeHelper();
        emit rejected();
    });

    const CoreAV* av = Core::getInstance()->getAv();
    connect(av, &CoreAV::avEnd, this, [this](quint32 friendId)
    {
        if (friendId == this->mFriendId)
            this->deleteLater();
    });

    layout->setMargin(12);
    layout->addSpacing(spikeH);
    layout->addWidget(callLabel);
    layout->addWidget(buttonBox);

    setFixedSize(rectW,rectH+spikeH);

    anchor->installEventFilter(this);
}

void CallConfirmWidget::closeHelper()
{
    if (this->testAttribute(Qt::WA_DeleteOnClose))
    {
        this->setAttribute(Qt::WA_DeleteOnClose, false);
        this->deleteLater();
    }
}

/**
 * @brief Recalculate our positions to track the anchor.
 */
void CallConfirmWidget::reposition()
{
    const QWidget* pw = parentWidget();
    const QSize anchorSize = anchor->size();
    const QPoint anchorPos = anchor->mapToGlobal(QPoint());

    QPoint newPos = pw ? pw->mapFromGlobal(anchorPos) : anchorPos;
    newPos.rx() -= (width() - anchorSize.width()) / 2;
    newPos.ry() += anchorSize.height();

    if (pw)
    {
        newPos.rx() = qBound(0, newPos.x(), pw->width() - width());
        newPos.ry() = qBound(0, newPos.y(), pw->height() - height());
    }

    // store the anchor center coordinates relative to the widget
    anchorPoint = anchorPos;
    anchorPoint += QPoint(anchorSize.width(), anchorSize.height()) / 2;

    move(newPos);
}

void CallConfirmWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);

    QRect mainRect(0, spikeH, rectW, rectH);
    painter.drawRoundRect(mainRect, roundedFactor * rectRatio, roundedFactor);

    int anchorX = mapFromGlobal(anchorPoint).x();
    QPolygon spikePoly({{anchorX - spikeW / 2, spikeH},
                        {anchorX, 0},
                        {anchorX + spikeW / 2, spikeH}});
    painter.drawPolygon(spikePoly);
}

void CallConfirmWidget::moveEvent(QMoveEvent* event)
{
    if (event->pos() != event->oldPos())
    {
        reposition();
    }

    QWidget::moveEvent(event);
}

void CallConfirmWidget::resizeEvent(QResizeEvent* event)
{
    if (event->size() != event->oldSize())
    {
        reposition();
    }

    QWidget::resizeEvent(event);
}

bool CallConfirmWidget::eventFilter(QObject* watched, QEvent* event)
{
    QEvent::Type t = event->type();
    if (t == QEvent::Move || t == QEvent::Resize)
    {
        reposition();
        return true;
    }

    if (t == QEvent::Hide)
    {
        hide();
        return true;
    }

    if (t == QEvent::Show)
    {
        show();
        return true;
    }

    return QObject::eventFilter(watched, event);
}
