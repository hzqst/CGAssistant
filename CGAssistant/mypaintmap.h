#ifndef MYPAINTMAP_H
#define MYPAINTMAP_H

#include <QWidget>
#include <QPaintEvent>
#include "player.h"

class MyPaintMap : public QWidget
{
public:
        MyPaintMap(QWidget *parent);

        void LoadMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units);
        void LoadMapInfo(QString name, int x, int y, int gameStatus, int battleStatus);
protected:
        virtual void paintEvent(QPaintEvent *event);

        QSharedPointer<CGA_MapCellData_t> m_collision, m_objects;
        QSharedPointer<CGA_MapUnits_t> m_units;
        int m_playerx, m_playery;
public:
        bool m_bShowCrosshair;
};

#endif // MYPAINTMAP_H
