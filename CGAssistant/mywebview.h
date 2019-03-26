#ifndef MYWEBVIEW_H
#define MYWEBVIEW_H

#include <QString>
#include <QWebEngineView>

class MyWebView : public QWebEngineView {
    Q_OBJECT

public:
    MyWebView(QWidget *parent = 0);

protected:
    void contextMenuEvent(QContextMenuEvent *event);
};

#endif // MYWEBVIEW_H
