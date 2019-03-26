#ifndef MYLISTVIEW_H
#define MYLISTVIEW_H

#include <QListView>

class MyListView : public QListView
{
    Q_OBJECT
public:
    MyListView(QWidget *parent = NULL);
    virtual void keyPressEvent(QKeyEvent *keyevent);
signals:
    void NotifyDeletePressed(int row);
};

#endif // MYLISTVIEW_H
