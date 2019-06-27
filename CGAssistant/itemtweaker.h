#ifndef ITEMTWEAKER_H
#define ITEMTWEAKER_H

#pragma once

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>

#include "player.h"

class CItemTweakerModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    //impl
    explicit CItemTweakerModel(QObject *parent = Q_NULLPTR);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    //util
    CItemTweaker *TweakerFromIndex(const QModelIndex &index) const;
    void appendRow(CItemTweakerPtr &item);
    bool swapRow(int position, int destination);
    bool removeRow(int position);
    bool removeRows(int position, int count);
signals:
    void syncList(CItemTweakerList list);
private:
    CItemTweakerList m_List;
};

#endif // ITEMTWEAKER_H
