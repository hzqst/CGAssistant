#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MyWebView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    virtual void closeEvent(QCloseEvent *event);
    virtual void changeEvent(QEvent * event);
private:
    Ui::MainWindow *ui;
    QThread m_playerWorkerThread;
    QThread m_processWorkerThread;
    QThread m_battleWorkerThread;
    MyWebView *m_webview;

signals:
    void NotifyCloseWindow();
    void NotifyChangeWindow(Qt::WindowStates);
};

#endif // MAINWINDOW_H
