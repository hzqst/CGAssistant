#include "mapform.h"
#include "ui_mapform.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

extern CGA::CGAInterface *g_CGAInterface;

MapForm::MapForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapForm)
{
    ui->setupUi(this);

    connect(ui->widget_paintmap, &MyPaintMap::updateMousePosition, this, &MapForm::UpdateMousePosition);
    connect(ui->widget_paintmap, &MyPaintMap::runNavigatorScript, this, &MapForm::RunNavigatorScript);
    connect(ui->widget_paintmap, &MyPaintMap::stopNavigatorScript, this, &MapForm::StopNavigatorScript);
}

MapForm::~MapForm()
{
    delete ui;
}

void MapForm::keyPressEvent(QKeyEvent *event)
{
    if(!m_collision)
        return;

    ui->widget_paintmap->keyPressEvent(event);
}

void MapForm::UpdateMousePosition(int x, int y)
{
    ui->label_xy_2->setText(tr("mouse click (%1, %2)").arg(x).arg(y));
}

void MapForm::RunNavigatorScript(int x, int y, int enter, QString *result)
{
    runNavigatorScript(x, y, enter, result);
}

void MapForm::StopNavigatorScript()
{
    stopNavigatorScript();
}

void MapForm::OnReportNavigatorFinish(int exitCode)
{
    if(exitCode != 0)
        ui->widget_paintmap->ShowNavError(QObject::tr("Navigation Failed"));
    ui->widget_paintmap->LoadNavPath(nullptr);
}

void MapForm::OnReportNavigatorPath(QString json)
{
    QJsonParseError err;
    auto doc = QJsonDocument::fromJson(json.toLocal8Bit(), &err);

    if(err.error != QJsonParseError::NoError)
        return;

    if(!doc.isArray())
        return;

    auto arr = doc.array();

    QSharedPointer<QVector<uint32_t>> navpath(new QVector<uint32_t>);
    for(auto itor = arr.begin(); itor != arr.end(); ++itor)
    {
        if(itor->isArray())
        {
            auto sub = itor->toArray();
            if(sub.count() == 2)
            {
                int x = sub.at(0).toInt();
                int y = sub.at(1).toInt();
                navpath->push_back((x & 0xFFFF) | ((y & 0xFFFF) << 16));
            }
        }
    }


    if(navpath->empty())
        ui->widget_paintmap->ShowNavError(QObject::tr("Navigation Failed"));

    ui->widget_paintmap->LoadNavPath(navpath);
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

void MapForm::OnNotifyGetMapInfo(QString name, int index1, int index2, int index3, int x, int y, int worldstatus, int gamestatus)
{
    ui->widget_paintmap->m_bShowCrosshair = ui->checkBox_showcrosshair->isChecked();

    ui->widget_paintmap->LoadMapInfo(name, x, y, index3, worldstatus, gamestatus);
    ui->label_mapname->setText(name);
    ui->label_xy->setText(tr("player at (%1, %2)").arg(x).arg(y));
    ui->label_index->setText(tr("map index : %1").arg(index3));
}

void MapForm::OnNotifyRefreshMapRegion(int xbase, int ybase, int xtop, int ytop, int index3)
{
    if(m_collision && index3 == m_collision->mapindex)
        ui->widget_paintmap->RepaintCollisionPixels(xbase, ybase, xtop, ytop);
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
