#include "mylistview.h"
#include <QKeyEvent>

MyListView::MyListView(QWidget *parent) : QListView(parent)
{

}

void MyListView::keyPressEvent(QKeyEvent *keyevent)
{
    if(keyevent->key() == Qt::Key_Delete)
    {
        if(currentIndex().isValid())
            NotifyDeletePressed(currentIndex().row());
    }
}
