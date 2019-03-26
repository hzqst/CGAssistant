#ifndef BATTLESETTING_H
#define BATTLESETTING_H

#pragma once

#include <QAbstractTableModel>
#include <QList>
#include "battle.h"

class CBattleSetting;

class CBattleSettingModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    //impl
    explicit CBattleSettingModel(QObject *parent = Q_NULLPTR);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    //util
    CBattleSetting *BattleSettingFromIndex(const QModelIndex &index) const;
    void appendRow(CBattleSettingPtr &item);
    bool swapRow(int position, int destination);
    bool removeRow(int position);
    bool removeRows(int position, int count);
signals:
    void syncList(CBattleSettingList list);
private:
    QStringList m_HeaderString;
    CBattleSettingList m_List;
};

#endif // BATTLESETTINGTABLE_H
