#include "mypaintmap.h"
#include <QPainter>
#include <QBitmap>

MyPaintMap::MyPaintMap(QWidget *parent) : QWidget (parent)
{
    m_playerx = 0;
    m_playery = 0;
    m_bShowCrosshair = false;
}

void MyPaintMap::paintEvent(QPaintEvent *event){
    auto widgetSize = size();

    if(m_collision && m_objects && m_collision->xsize && m_collision->ysize)
    {
        auto blockSize = 8;
        QPixmap pix((m_collision->xsize + 1) * blockSize, (m_collision->ysize + 1) * blockSize);
        QPainter painter(&pix);

        QBrush brushwh(Qt::white);
        QBrush brushgray(Qt::gray);
        QBrush brushred(Qt::red);
        QBrush brushblue(Qt::blue);
        QBrush brushgreen(Qt::green);
        QBrush brushdy(Qt::darkMagenta);
        painter.fillRect(pix.rect(), brushwh);

        for(int y = 0;y < m_collision->ysize; ++y)
        {
            for(int x = 0;x < m_collision->xsize; ++x)
            {
               auto cell = m_collision->cells.at( (size_t)(x + y * m_collision->xsize) );
               if(cell == 1)
                    painter.fillRect(x*blockSize, y*blockSize, blockSize, blockSize, brushgray);
            }
        }

        for(int y = 0;y < m_objects->ysize; ++y)
        {
            for(int x = 0;x < m_objects->xsize; ++x)
            {
               auto cell = m_objects->cells.at( (size_t)(x + y * m_objects->xsize) );
               if(cell & 0xff){
                   if((cell & 0xff) == 3)
                        painter.fillRect(x*blockSize, y*blockSize, blockSize, blockSize, brushblue);
                   else
                       painter.fillRect(x*blockSize, y*blockSize, blockSize, blockSize, brushgreen);

                   // qDebug("object %X at %d %d", cell, x, y);
               }

               if(x == m_playerx - m_collision->xbottom && y == m_playery - m_collision->ybottom)
                   painter.fillRect(x*blockSize, y*blockSize, blockSize, blockSize, brushred);
            }
        }

        for(int i = 0; i < m_units->size(); ++i)
        {
            const auto &unit = m_units->at(i);
            if(unit.valid && unit.type == 1 && unit.model_id != 0 && unit.unit_name.length())
            {
                int x = unit.xpos;
                int y = unit.ypos;
                painter.fillRect(x*blockSize, y*blockSize, blockSize, blockSize, brushdy);
            }
        }

        if(m_bShowCrosshair)
        {
            QPen redpen(Qt::red, blockSize);
            painter.setPen(redpen);
            painter.drawLine(0, m_playery*blockSize + blockSize/2, pix.width(), m_playery*blockSize + blockSize/2);
            painter.drawLine(m_playerx*blockSize + blockSize/2, 0, m_playerx*blockSize + blockSize/2, pix.height());
        }

        painter.end();
        painter.begin(this);
        painter.drawPixmap(0, 0, widgetSize.width()-1, widgetSize.height()-1, pix);
        painter.setPen(Qt::black);
        painter.drawRect(0, 0, widgetSize.width()-1, widgetSize.height()-1);
    }
};

void MyPaintMap::LoadMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units)
{
    m_collision = coll;
    m_objects = obj;
    m_units = units;
    update();
}

void MyPaintMap::LoadMapInfo(QString name, int x, int y, int gameStatus, int battleStatus)
{
    m_playerx = x;
    m_playery = y;
    update();
}
