#ifndef QMLVIEW_H
#define QMLVIEW_H

#include <QQuickView>

class QmlView
{
public:
    QmlView();

    inline void show()
    {
        view.show();
    }

private:
    // TODO: view must be static !!
    QQuickView view;
};

class Widget : public QmlView
{
public:
    Widget() = default;
};

class LoginScreen : public QmlView
{
public:
    LoginScreen() = default;
};

#endif
