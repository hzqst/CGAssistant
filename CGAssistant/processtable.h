#ifndef PROCESSTABLE_H
#define PROCESSTABLE_H

#pragma once

//Event table

#include <QAbstractTableModel>
#include <QList>
#include "psworker.h"

class CProcessTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    //impl
    explicit CProcessTableModel(QObject *parent = Q_NULLPTR, CProcessWorker *work = NULL);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    //util
    CProcessItem *ItemFromIndex(const QModelIndex &index) const ;
    void appendRow(const CProcessItemPtr &item);
    bool removeRow(int position, const QModelIndex &parent = QModelIndex());
    bool removeRows(int position, int count, const QModelIndex &parent = QModelIndex());
private:
    CProcessItemList m_List;
    QStringList m_HeaderString;
public:
    CProcessWorker *m_worker;
};

#endif // PROCESSTABLE_H
