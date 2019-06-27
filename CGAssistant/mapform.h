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

public slots:
    void OnNotifyGetMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units);
    void OnNotifyGetMapInfo(QString name, int index1, int index2, int index3, int x, int y, int gameStatus, int battleStatus);
    void OnCloseWindow();

private slots:
    void on_pushButton_loadmap_clicked();

signals:
    void RequestDownloadMap(int xsize, int ysize);

private:
    Ui::MapForm *ui;
    QSharedPointer<CGA_MapCellData_t> m_collision;

};

#endif // MAPFORM_H
