#include "mapform.h"
#include "ui_mapform.h"

extern CGA::CGAInterface *g_CGAInterface;

MapForm::MapForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapForm)
{
    ui->setupUi(this);
}

MapForm::~MapForm()
{
    delete ui;
}

void MapForm::OnNotifyGetMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units)
{
    ui->widget_paintmap->m_bShowCrosshair = ui->checkBox_showcrosshair->isChecked();
    if(coll && coll->xsize && coll->ysize)
    {
        m_collision = coll;
        ui->widget_paintmap->LoadMapCellInfo(coll, obj, units);
    }
}

void MapForm::OnNotifyGetMapInfo(QString name, int index1, int index2, int index3, int x, int y, int gameStatus, int battleStatus)
{
    ui->widget_paintmap->m_bShowCrosshair = ui->checkBox_showcrosshair->isChecked();
    ui->widget_paintmap->LoadMapInfo(name, x, y, gameStatus, battleStatus);
    ui->label_mapname->setText(name);
    ui->label_xy->setText(QString("x:%1 y:%2").arg(x).arg(y));
    ui->label_index->setText(QString("index:%1 %2 %3").arg(index1).arg(index2).arg(index3));
}

void MapForm::OnCloseWindow()
{

}

void MapForm::on_pushButton_loadmap_clicked()
{
    if(m_collision)
    {
        RequestDownloadMap(m_collision->xsize, m_collision->ysize);
    }
}
