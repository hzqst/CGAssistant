#ifndef MAPFORM_H
#define MAPFORM_H

#include <QWidget>
#include "player.h"

namespace Ui {
class MapForm;
}

class MapForm : public QWidget
{
    Q_OBJECT

public:
    explicit MapForm(QWidget *parent = nullptr);
    ~MapForm();
    virtual void keyPressEvent(QKeyEvent *event);
public slots:
    void OnNotifyRefreshMapRegion(int xbase, int ybase, int xtop, int ytop, int index3);
    void OnNotifyGetMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units);
    void OnNotifyGetMapInfo(QString name, int index1, int index2, int index3, int x, int y, int worldstatus, int gamestatus);
    void OnReportNavigatorPath(QString json);
    void OnReportNavigatorFinish(int exitCode);
    void OnCloseWindow();
    void RunNavigatorScript(int x, int y, int enter, QString *result);
    void StopNavigatorScript();
    void UpdateMousePosition(int x, int y);

private slots:
    void on_pushButton_loadmap_clicked();

signals:
    void RequestDownloadMap(int xsize, int ysize);
    void runNavigatorScript(int x, int y, int enter, QString *result);
    void stopNavigatorScript();
private:
    Ui::MapForm *ui;
    QSharedPointer<CGA_MapCellData_t> m_collision;
};

#endif // MAPFORM_H
