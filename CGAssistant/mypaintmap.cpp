#include "mypaintmap.h"
#include <QPainter>
#include <QBitmap>

MyPaintMap::MyPaintMap(QWidget *parent) : QWidget (parent)
{
    m_playerx = 0;
    m_playery = 0;
    m_playermapindex = 0;
    m_select_unitid = 0;
    m_bShowCrosshair = false;
    m_bMoveCamera = false;
    m_pixelwidth = 0;
    m_pixelheight = 0;
}

#define blockSize 4

void MyPaintMap::keyPressEvent(QKeyEvent *event)
{
    if(!m_collision)
        return;

    if(event->key() == 'S')
    {
        stopNavigatorScript();
    }
    else if(event->key() == 'C')
    {
        m_movecamera_x = m_playerx;
        m_movecamera_y = m_playery;
    }
    else if(event->key() == 'R')
    {
        m_bMoveCamera = false;
    }
}

void MyPaintMap::mousePressEvent(QMouseEvent *event)
{
    if(!m_collision)
        return;

    if(event->button() == Qt::LeftButton)
    {
        auto widgetSize = size();
        if(m_bMoveCamera)
        {
            int basex = m_movecamera_x * blockSize - widgetSize.width()/2;
            int basey = m_movecamera_y * blockSize - widgetSize.height()/2;

            int mapX = (event->x() + basex) / blockSize;
            int mapY = (event->y() + basey) / blockSize;

            float mindist = 9999;

            int select_unit = 0;
            int select_unitx = 0;
            int select_unity = 0;
            QString select_unit_name;

            for(int i = 0; i < m_units->size(); ++i)
            {
                const auto &unit = m_units->at(i);
                if(unit.valid && unit.type == 1 && unit.model_id != 0 && (unit.flags & 4096) != 0)
                {
                    int x = unit.xpos;
                    int y = unit.ypos;

                    float d = sqrt((x - mapX) * (x - mapX) + (y - mapY) * (y - mapY));
                    if(d < 3.0f && d < mindist)
                    {
                        mindist = d;
                        select_unit = unit.unit_id;
                        select_unitx = unit.xpos;
                        select_unity = unit.ypos;
                        select_unit_name = unit.unit_name + QObject::tr(" (%1, %2)").arg(unit.xpos).arg(unit.ypos);
                    }
                }
                else if(unit.valid && unit.type == 2 && unit.model_id != 0 && (unit.flags & 1024) != 0)
                {
                    int x = unit.xpos;
                    int y = unit.ypos;

                    float d = sqrt((x - mapX) * (x - mapX) + (y - mapY) * (y - mapY));
                    if(d < 3.0f && d < mindist)
                    {
                        mindist = d;
                        select_unit = unit.unit_id;
                        select_unitx = unit.xpos;
                        select_unity = unit.ypos;
                        select_unit_name = unit.item_name + QObject::tr(" (%1, %2)").arg(unit.xpos).arg(unit.ypos);
                    }
                }
            }

            if(select_unit)
            {
                m_select_unitid = select_unit;
                m_select_unitx = select_unitx;
                m_select_unity = select_unity;
                m_select_unit_name = select_unit_name;
                update();
            }
            else
            {
                updateMousePosition(mapX, mapY);
            }
        }
        else
        {
            int mapX = event->x() * m_collision->xsize / widgetSize.width();
            int mapY = event->y() * m_collision->ysize / widgetSize.height();

            float mindist = 9999;

            int select_unit = 0;
            int select_unitx = 0;
            int select_unity = 0;
            QString select_unit_name;

            for(int i = 0; i < m_units->size(); ++i)
            {
                const auto &unit = m_units->at(i);
                if(unit.valid && unit.type == 1 && unit.model_id != 0 && (unit.flags & 4096) != 0)
                {
                    int x = unit.xpos;
                    int y = unit.ypos;

                    auto d = sqrt((x - mapX) * (x - mapX) + (y - mapY) * (y - mapY));
                    if(d < 3.0 && d < mindist)
                    {
                        mindist = d;
                        select_unit = unit.unit_id;
                        select_unitx = unit.xpos;
                        select_unity = unit.ypos;
                        select_unit_name = unit.unit_name + QObject::tr(" (%1, %2)").arg(unit.xpos).arg(unit.ypos);
                    }
                }
                else if(unit.valid && unit.type == 2 && unit.model_id != 0 && (unit.flags & 1024) != 0)
                {
                    int x = unit.xpos;
                    int y = unit.ypos;

                    auto d = sqrt((x - mapX) * (x - mapX) + (y - mapY) * (y - mapY));
                    if(d < 3.0 && d < mindist)
                    {
                        mindist = d;
                        select_unit = unit.unit_id;
                        select_unitx = unit.xpos;
                        select_unity = unit.ypos;
                        select_unit_name = unit.item_name + QObject::tr(" (%1, %2)").arg(unit.xpos).arg(unit.ypos);
                    }
                }
            }

            if(select_unit)
            {
                m_select_unitid = select_unit;
                m_select_unitx = select_unitx;
                m_select_unity = select_unity;
                m_select_unit_name = select_unit_name;
                update();
            }
            else
            {
                updateMousePosition(mapX, mapY);
            }
        }
    }

    else if(event->button() == Qt::RightButton)
    {
        auto widgetSize = size();
        if(m_bMoveCamera)
        {
            int basex = m_movecamera_x * blockSize - widgetSize.width()/2;
            int basey = m_movecamera_y * blockSize - widgetSize.height()/2;

            int mapX = (event->x() + basex) / blockSize;
            int mapY = (event->y() + basey) / blockSize;

            if(mapX >= 0 && mapX < m_collision->xsize && mapY >= 0 && mapY < m_collision->ysize && m_collision->cells.at( (size_t)(mapX + mapY * m_collision->xsize) ) != 1)
            {
                int enter = 0;
                if(mapX < m_objects->xsize && mapY < m_objects->ysize){
                    enter = (m_objects->cells.at( (size_t)(mapX + mapY * m_objects->xsize) ) & 0xFF);
                }
                m_nav_x = mapX;
                m_nav_y = mapY;
                m_naverror = QString();

                runNavigatorScript(mapX, mapY, enter, &m_naverror);
            }
        }
        else
        {
            int mapX = event->x() * m_collision->xsize / widgetSize.width();
            int mapY = event->y() * m_collision->ysize / widgetSize.height();

            if(mapX >= 0 && mapX < m_collision->xsize && mapY >= 0 && mapY < m_collision->ysize && m_collision->cells.at( (size_t)(mapX + mapY * m_collision->xsize) ) != 1)
            {
                int enter = 0;
                if(mapX < m_objects->xsize && mapY < m_objects->ysize){
                   enter = (m_objects->cells.at( (size_t)(mapX + mapY * m_objects->xsize) ) & 0xFF);
                }
                m_nav_x = mapX;
                m_nav_y = mapY;
                m_naverror = QString();
                runNavigatorScript(mapX, mapY, enter, &m_naverror);
            }
        }

    }

    else if(event->button() == Qt::MidButton)
    {
        auto widgetSize = size();
        if(m_bMoveCamera)
        {
            int basex = m_movecamera_x * blockSize - widgetSize.width()/2;
            int basey = m_movecamera_y * blockSize - widgetSize.height()/2;

            int mapX = (event->x() + basex) / blockSize;
            int mapY = (event->y() + basey) / blockSize;

            if(mapX >= 0 && mapX < m_collision->xsize && mapY >= 0 && mapY < m_collision->ysize)
            {
                m_movecamera_x = mapX;
                m_movecamera_y = mapY;
            }
        }
        else
        {
            int mapX = event->x() * m_collision->xsize / widgetSize.width();
            int mapY = event->y() * m_collision->ysize / widgetSize.height();

            if(mapX >= 0 && mapX < m_collision->xsize && mapY >= 0 && mapY < m_collision->ysize)
            {
                m_movecamera_x = mapX;
                m_movecamera_y = mapY;
                m_bMoveCamera = true;
            }
        }
    }
}

void MyPaintMap::paintEvent(QPaintEvent *event){
    auto widgetSize = size();

    QImage image((uchar*)m_pixels.get(), m_pixelwidth, m_pixelheight, QImage::Format_ARGB32);
    QImage image2((uchar*)m_pixels2.get(), m_pixelwidth, m_pixelheight, QImage::Format_ARGB32);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRectF renderRect(0, 0, widgetSize.width(), widgetSize.height());
    if(m_bMoveCamera)
    {
        int basex = m_movecamera_x * blockSize - widgetSize.width()/2;
        int basey = m_movecamera_y * blockSize - widgetSize.height()/2;
        QRectF sourceRect(basex, basey, widgetSize.width(), widgetSize.height());

        painter.drawImage(renderRect, image, sourceRect);
        painter.drawImage(renderRect, image2, sourceRect);

        if(m_collision)
        {
            if(m_bShowCrosshair)
            {
                painter.setPen(Qt::red);
                int x = m_playerx * blockSize - basex + blockSize / 2;
                int y = m_playery * blockSize - basey + blockSize / 2;
                painter.drawLine(0, y, widgetSize.width(), y);
                painter.drawLine(x, 0, x, widgetSize.height());
            }
            else
            {
                painter.setPen(Qt::red);
                int x = m_playerx * blockSize - basex + blockSize / 2;
                int y = m_playery * blockSize - basey + blockSize / 2;
                painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));
                painter.drawRect(x - 2, y - 2, 4, 4);
            }

            if(m_select_unitid)
            {
                painter.setPen(Qt::red);
                int x = m_select_unitx * blockSize - basex + blockSize / 2;
                int y = m_select_unity * blockSize - basey + blockSize / 2;
                painter.setBrush(QBrush(Qt::NoBrush));
                painter.drawEllipse(x - 4, y - 4, 8, 8);

                painter.setPen(Qt::white);
                painter.drawText(x + 10, y + 4, m_select_unit_name);

                painter.setPen(Qt::red);
                painter.drawText(x + 9, y + 3, m_select_unit_name);
            }

            if(m_naverror.size())
            {
                int x = m_nav_x * blockSize - basex + blockSize / 2;
                int y = m_nav_y * blockSize - basey + blockSize / 2;

                painter.setPen(Qt::white);
                painter.drawText(x + 10, y + 4, m_naverror);

                painter.setPen(Qt::red);
                painter.drawText(x + 9, y + 3, m_naverror);
            }

            if(m_navpath && m_navpath->size() >= 2)
            {
                painter.setPen(Qt::green);

                for(int j = 1;j < m_navpath->size(); ++j)
                {
                    int prevx = (m_navpath->at(j - 1) & 0xFFFF) * blockSize - basex + blockSize / 2;
                    int prevy = ((m_navpath->at(j - 1) >> 16) & 0xFFFF) * blockSize - basey + blockSize / 2;
                    int x = (m_navpath->at(j) & 0xFFFF) * blockSize - basex + blockSize / 2;
                    int y = ((m_navpath->at(j) >> 16) & 0xFFFF) * blockSize - basey + blockSize / 2;
                    painter.drawLine(prevx, prevy, x, y);
                }

                int endx = m_navpath->at(m_navpath->size() - 1) & 0xFFFF;
                int endy = (m_navpath->at(m_navpath->size() - 1) >> 16) & 0xFFFF;
                int x = endx * blockSize - basex + blockSize / 2;
                int y = endy * blockSize - basey + blockSize / 2;

                QString pathend = tr("(%1, %2)").arg(endx).arg(endy);

                painter.setPen(Qt::white);
                painter.drawText(x + 10, y + 4, pathend);

                painter.setPen(Qt::red);
                painter.drawText(x + 9, y + 3, pathend);
            }
        }
        painter.setBrush(QBrush(Qt::NoBrush));
        painter.setPen( hasFocus() ? Qt::red : Qt::black);
        painter.drawRect(renderRect);
    }
    else {
        painter.drawImage(renderRect, image);
        painter.drawImage(renderRect, image2);

        if(m_collision)
        {
            auto newBlockWidth = blockSize * widgetSize.width() / m_pixelwidth;
            auto newBlockHeight = blockSize * widgetSize.height() / m_pixelheight;
            if(m_bShowCrosshair)
            {
                painter.setPen(Qt::red);
                int x = m_playerx * widgetSize.width() / m_collision->xsize + newBlockWidth / 2 + 1;
                int y = m_playery * widgetSize.height() / m_collision->ysize + newBlockHeight / 2 + 1;
                painter.drawLine(0, y, widgetSize.width(), y);
                painter.drawLine(x, 0, x, widgetSize.height());
            }
            else
            {
                painter.setPen(Qt::red);
                int x = m_playerx * widgetSize.width() / m_collision->xsize + newBlockWidth / 2 + 1;
                int y = m_playery * widgetSize.height() / m_collision->ysize + newBlockHeight / 2 + 1;
                painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));
                painter.drawRect(x - 2, y - 2, 4, 4);
            }

            if(m_select_unitid)
            {
                painter.setPen(Qt::red);
                int x = m_select_unitx * widgetSize.width() / m_collision->xsize + newBlockWidth / 2 + 1;
                int y = m_select_unity * widgetSize.height() / m_collision->ysize + newBlockHeight / 2 + 1;
                painter.setBrush(QBrush(Qt::NoBrush));
                painter.drawEllipse(x - 4, y - 4, 8, 8);

                painter.setPen(Qt::white);
                painter.drawText(x + 10, y + 4, m_select_unit_name);

                painter.setPen(Qt::red);
                painter.drawText(x + 9, y + 3, m_select_unit_name);
            }

            if(m_naverror.size())
            {
                int x = m_nav_x * widgetSize.width() / m_collision->xsize + newBlockWidth / 2 + 1;
                int y = m_nav_y * widgetSize.height() / m_collision->ysize + newBlockHeight / 2 + 1;

                painter.setPen(Qt::white);
                painter.drawText(x + 10, y + 4, m_naverror);

                painter.setPen(Qt::red);
                painter.drawText(x + 9, y + 3, m_naverror);
            }

            if(m_navpath && m_navpath->size() >= 2)
            {
                painter.setPen(Qt::green);

                for(int j = 1;j < m_navpath->size(); ++j)
                {
                    int prevx = (m_navpath->at(j - 1) & 0xFFFF) * widgetSize.width() / m_collision->xsize + newBlockWidth / 2 + 1;
                    int prevy = ((m_navpath->at(j - 1) >> 16) & 0xFFFF) * widgetSize.height() / m_collision->ysize + newBlockHeight / 2 + 1;
                    int x = (m_navpath->at(j) & 0xFFFF) * widgetSize.width() / m_collision->xsize + newBlockWidth / 2 + 1;
                    int y = ((m_navpath->at(j) >> 16) & 0xFFFF) * widgetSize.height() / m_collision->ysize + newBlockHeight / 2 + 1;
                    painter.drawLine(prevx, prevy, x, y);
                }

                int endx = m_navpath->at(m_navpath->size() - 1) & 0xFFFF;
                int endy = (m_navpath->at(m_navpath->size() - 1) >> 16) & 0xFFFF;
                int x = endx * widgetSize.width() / m_collision->xsize + newBlockWidth / 2 + 1;
                int y = endy * widgetSize.height() / m_collision->ysize + newBlockHeight / 2 + 1;

                QString pathend = tr("(%1, %2)").arg(endx).arg(endy);

                painter.setPen(Qt::white);
                painter.drawText(x + 10, y + 4, pathend);

                painter.setPen(Qt::red);
                painter.drawText(x + 9, y + 3, pathend);
            }
        }
        painter.setBrush(QBrush(Qt::NoBrush));
        painter.setPen( hasFocus() ? Qt::red : Qt::black);
        painter.drawRect(renderRect);
    }

    painter.end();
}

void MyPaintMap::LoadNavPath(QSharedPointer<QVector<uint32_t>> navpath)
{
    m_navpath = navpath;
    update();
}

void MyPaintMap::ShowNavError(QString err)
{
    m_naverror = err;
    update();
}

void MyPaintMap::LoadMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units)
{
    m_collision = coll;
    m_objects = obj;
    m_units = units;

    if(m_collision)
     {
         if(m_pixelwidth != m_collision->xsize * blockSize || m_pixelheight != m_collision->ysize * blockSize)
         {
             m_pixelwidth = m_collision->xsize * blockSize;
             m_pixelheight = m_collision->ysize * blockSize;
             m_pixels = QSharedPointer<QRgb>(new QRgb[m_pixelwidth * m_pixelheight]);
             m_pixels2 = QSharedPointer<QRgb>(new QRgb[m_pixelwidth * m_pixelheight]);
         }

         if(m_mapindex != m_collision->mapindex)
         {
             m_select_unitid = 0;
             m_bMoveCamera = false;
            RepaintCollisionPixels(0, 0, m_collision->xsize, m_collision->ysize);
         }

         m_mapindex = m_collision->mapindex;
     }

    for(uint i = 0; i < m_pixelheight * m_pixelwidth ; ++i)
        m_pixels2.get()[i] = qRgba(255, 255, 255, 0);

    if(m_collision && m_objects && m_collision->xsize && m_collision->ysize)
    {
        for(int y = 0;y < m_collision->ysize; ++y)
        {
            for(int x = 0;x < m_collision->xsize; ++x)
            {
                if(x < m_objects->xsize && y < m_objects->ysize)
                {
                    auto cellObject = m_objects->cells.at( (size_t)(x + y * m_objects->xsize) );
                    if(cellObject & 0xff){
                        if((cellObject & 0xff) == 3)
                        {
                            for(int yy = y * blockSize; yy < y * blockSize + blockSize; ++yy)
                                for(int xx = x * blockSize; xx < x * blockSize + blockSize; ++xx)
                                    m_pixels2.get()[xx + yy * m_pixelwidth] = qRgb(0, 50, 200);
                        }
                        else
                        {
                            for(int yy = y * blockSize; yy < y * blockSize + blockSize; ++yy)
                                for(int xx = x * blockSize; xx < x * blockSize + blockSize; ++xx)
                                    m_pixels2.get()[xx + yy * m_pixelwidth] = qRgb(0, 200, 0);
                        }
                    }
                }
            }
        }

        for(int i = 0; i < m_units->size(); ++i)
        {
            const auto &unit = m_units->at(i);
            if(unit.valid && unit.type == 1 && unit.model_id != 0 && (unit.flags & 4096) != 0)
            {
                int x = unit.xpos;
                int y = unit.ypos;

                if(x < m_collision->xsize && y < m_collision->ysize)
                {
                    for(int yy = y * blockSize; yy < y * blockSize + blockSize; ++yy)
                        for(int xx = x * blockSize; xx < x * blockSize + blockSize; ++xx)
                            m_pixels2.get()[xx + yy * m_pixelwidth] = qRgb(255, 150, 50);
                }
            }
            else if(unit.valid && unit.type == 2 && unit.model_id != 0 && (unit.flags & 1024) != 0)
            {
                int x = unit.xpos;
                int y = unit.ypos;

                if(x < m_collision->xsize && y < m_collision->ysize)
                {
                    for(int yy = y * blockSize; yy < y * blockSize + blockSize; ++yy)
                        for(int xx = x * blockSize; xx < x * blockSize + blockSize; ++xx)
                            m_pixels2.get()[xx + yy * m_pixelwidth] = qRgb(255, 100, 255);
                }
            }
        }
    }
}

void MyPaintMap::RepaintCollisionPixels(int xbase, int ybase, int xtop, int ytop)
{
    if(xbase >= 0 && ybase >= 0 && xtop <= m_collision->xsize && ytop <= m_collision->ysize)
    {
        if(m_collision && m_collision->xsize * blockSize == m_pixelwidth && m_collision->ysize * blockSize == m_pixelheight)
        {
            for(int y = ybase;y < ytop; ++y)
            {
                for(int x = xbase;x < m_collision->xsize; ++x)
                {
                    auto cellWall = m_collision->cells.at( (size_t)(x + y * m_collision->xsize) );
                    if(cellWall == 1)
                    {
                        for(int yy = y * blockSize; yy < y * blockSize + blockSize; ++yy)
                            for(int xx = x * blockSize; xx < x * blockSize + blockSize; ++xx)
                                m_pixels.get()[xx + yy * m_pixelwidth] = qRgb(100, 100, 100);
                    }
                    else
                    {
                        for(int yy = y * blockSize; yy < y * blockSize + blockSize; ++yy)
                            for(int xx = x * blockSize; xx < x * blockSize + blockSize; ++xx)
                                m_pixels.get()[xx + yy * m_pixelwidth] = qRgb(255, 255, 255);
                    }
                }
            }

            update();
        }
    }
}

void MyPaintMap::LoadMapInfo(QString name, int playerx, int playery, int mapindex, int worldStatus, int gameStatus)
{
    m_playerx = playerx;
    m_playery = playery;
    m_playermapindex = mapindex;

    update();
}
