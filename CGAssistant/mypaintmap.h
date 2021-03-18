#ifndef MYPAINTMAP_H
#define MYPAINTMAP_H

#include <QWidget>
#include <QPaintEvent>
#include <QMenu>
#include "player.h"

class MyPaintMap : public QWidget
{
    Q_OBJECT

public:
        MyPaintMap(QWidget *parent);

        void LoadMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units);
        void LoadMapInfo(QString name, int x, int y, int mapindex, int worldStatus, int gameStatus);
        void LoadNavPath(QSharedPointer<QVector<uint32_t>> navpath);
        void RepaintCollisionPixels(int xbase, int ybase, int xtop, int ytop);
        void ShowNavError(QString err);
        virtual void keyPressEvent(QKeyEvent *event);
signals:
        void updateMousePosition(int x, int y);
        void runNavigatorScript(int x, int y, int enter, QString *result);
        void stopNavigatorScript();
protected:
        virtual void paintEvent(QPaintEvent *event);
        virtual void mousePressEvent(QMouseEvent *event);

        QSharedPointer<CGA_MapCellData_t> m_collision, m_objects;
        QSharedPointer<CGA_MapUnits_t> m_units;
        QSharedPointer<QVector<uint32_t>> m_navpath;
        QString m_naverror;
        int m_nav_x;
        int m_nav_y;
        int m_movecamera_x;
        int m_movecamera_y;
        int m_mapindex;
        int m_playerx, m_playery, m_playermapindex;
        int m_select_unitid;
        int m_select_unitx;
        int m_select_unity;
        QString m_select_unit_name;
public:
        bool m_bShowCrosshair;
        bool m_bMoveCamera;
private:
        QSharedPointer<QRgb> m_pixels;
        QSharedPointer<QRgb> m_pixels2;
        size_t m_pixelwidth;
        size_t m_pixelheight;
};

#endif // MYPAINTMAP_H
