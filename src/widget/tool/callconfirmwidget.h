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


#ifndef CALLCONFIRMWIDGET_H
#define CALLCONFIRMWIDGET_H

#include <QWidget>
#include <QBrush>

class CallConfirmWidget final : public QWidget
{
    Q_OBJECT
public:
    explicit CallConfirmWidget(QWidget* Anchor, quint32 friendId,
                               QWidget* parent);

signals:
    void accepted();
    void rejected();

protected:
    void paintEvent(QPaintEvent* event) final;
    void moveEvent(QMoveEvent* event) final;
    void resizeEvent(QResizeEvent* event) final;
    bool eventFilter(QObject* watched, QEvent* event) final;

private:
    void closeHelper();
    void reposition();

private:
    QWidget* anchor;
    QPoint anchorPoint;
    const quint32 mFriendId;

    QBrush brush;

    const int rectW, rectH;
    const int spikeW, spikeH;
    const int roundedFactor;
    const qreal rectRatio;
};

#endif // CALLCONFIRMWIDGET_H
