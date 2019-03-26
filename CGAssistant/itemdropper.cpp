#include <QDateTime>
#include "itemdropper.h"

CItemDropperModel::CItemDropperModel(QObject *parent) : QAbstractTableModel(parent)
{
}

bool CItemDropperModel::swapRow(int position, int destination)
{
    bool success = false;

    if(position >= 0 && position < m_List.size() && destination >= 0 && destination < m_List.size() )
    {
        CItemDropperPtr ptr = m_List[destination];
        m_List[destination] = m_List[position];
        m_List[position] = ptr;
        dataChanged(index(position, 0), index(position, columnCount()));
        dataChanged(index(destination, 0), index(destination, columnCount()));

        success = true;
        syncList(m_List);
    }

    return success;
}

void CItemDropperModel::appendRow(CItemDropperPtr &item)
{
    int row = rowCount();

    beginInsertRows(QModelIndex(), row, row);
    m_List.append(item);
    endInsertRows();

    syncList(m_List);
}

bool CItemDropperModel::removeRow(int position)
{
    bool success = false;

    if(position >= 0 && position < m_List.size())
    {
        beginRemoveRows(QModelIndex(), position, position);
        m_List.erase(m_List.begin() + position);
        endRemoveRows();
        success = true;

        syncList(m_List);
    }

    return success;
}

bool CItemDropperModel::removeRows(int position, int count)
{
    bool success = false;

    if(count > 0 && position >= 0 && position + count - 1 < m_List.size())
    {
        beginRemoveRows(QModelIndex(), position, position + count - 1);
        m_List.erase(m_List.begin() + position, m_List.begin() + position + count);
        endRemoveRows();
        success = true;

        syncList(m_List);
    }

    return success;
}

Qt::ItemFlags CItemDropperModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

int CItemDropperModel::rowCount(const QModelIndex &parent) const
{
    return (int)m_List.size();
}

int CItemDropperModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QModelIndex CItemDropperModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0 || row >= m_List.size())
        return QModelIndex();

    return createIndex(row, column, m_List[row].data());
}

QModelIndex CItemDropperModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

CItemDropper *CItemDropperModel::DropperFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        return static_cast<CItemDropper *>(index.internalPointer());
    } else {
        return NULL;
    }
}

QVariant CItemDropperModel::data(const QModelIndex &index, int role) const
{
    CItemDropper *item = DropperFromIndex(index);
    if(item)
    {
        if (role == Qt::DisplayRole)
        {
            switch(index.column())
            {
            case 0:
            {
                QString str;
                item->GetName(str);
                return str;
            }
            }
        }
        else if (role == Qt::DecorationRole)
        {

        }
        else if (role == Qt::TextAlignmentRole)
        {
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        }
    }
    return QVariant();
}

QVariant CItemDropperModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}
