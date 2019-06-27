#include <QDateTime>
#include "itemtweaker.h"

CItemTweakerModel::CItemTweakerModel(QObject *parent) : QAbstractTableModel(parent)
{
}

bool CItemTweakerModel::swapRow(int position, int destination)
{
    bool success = false;

    if(position >= 0 && position < m_List.size() && destination >= 0 && destination < m_List.size() )
    {
        CItemTweakerPtr ptr = m_List[destination];
        m_List[destination] = m_List[position];
        m_List[position] = ptr;
        dataChanged(index(position, 0), index(position, columnCount()));
        dataChanged(index(destination, 0), index(destination, columnCount()));

        success = true;
        syncList(m_List);
    }

    return success;
}

void CItemTweakerModel::appendRow(CItemTweakerPtr &item)
{
    int row = rowCount();

    beginInsertRows(QModelIndex(), row, row);
    m_List.append(item);
    endInsertRows();

    syncList(m_List);
}

bool CItemTweakerModel::removeRow(int position)
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

bool CItemTweakerModel::removeRows(int position, int count)
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

Qt::ItemFlags CItemTweakerModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

int CItemTweakerModel::rowCount(const QModelIndex &parent) const
{
    return (int)m_List.size();
}

int CItemTweakerModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QModelIndex CItemTweakerModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0 || row >= m_List.size())
        return QModelIndex();

    return createIndex(row, column, m_List[row].data());
}

QModelIndex CItemTweakerModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

CItemTweaker *CItemTweakerModel::TweakerFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        return static_cast<CItemTweaker *>(index.internalPointer());
    } else {
        return NULL;
    }
}

QVariant CItemTweakerModel::data(const QModelIndex &index, int role) const
{
    CItemTweaker *item = TweakerFromIndex(index);
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

QVariant CItemTweakerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}
