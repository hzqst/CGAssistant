#include <QDateTime>
#include "battlesetting.h"

CBattleSettingModel::CBattleSettingModel(QObject *parent) : QAbstractTableModel(parent)
{
    m_HeaderString.append(tr("Condition"));
    m_HeaderString.append(tr("Condition2"));
    m_HeaderString.append(tr("Player Action"));
    m_HeaderString.append(tr("Player Target"));
    m_HeaderString.append(tr("Pet Action"));
    m_HeaderString.append(tr("Pet Target"));
    //m_HeaderString.append(tr("Pet Action 2"));
    //m_HeaderString.append(tr("Pet Target 2"));
}

bool CBattleSettingModel::swapRow(int position, int destination)
{
    bool success = false;

    if(position >= 0 && position < m_List.size() && destination >= 0 && destination < m_List.size() )
    {
        CBattleSettingPtr ptr = m_List[destination];
        m_List[destination] = m_List[position];
        m_List[position] = ptr;
        dataChanged(index(position, 0), index(position, columnCount()));
        dataChanged(index(destination, 0), index(destination, columnCount()));

        success = true;
        syncList(m_List);
    }

    return success;
}

void CBattleSettingModel::appendRow(CBattleSettingPtr &item)
{
    int row = rowCount();

    beginInsertRows(QModelIndex(), row, row);
    m_List.append(item);
    endInsertRows();

    syncList(m_List);
}

bool CBattleSettingModel::removeRow(int position)
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

bool CBattleSettingModel::removeRows(int position, int count)
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

Qt::ItemFlags CBattleSettingModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

int CBattleSettingModel::rowCount(const QModelIndex &parent) const
{
    return (int)m_List.size();
}

int CBattleSettingModel::columnCount(const QModelIndex &parent) const
{
    return m_HeaderString.size();
}

QModelIndex CBattleSettingModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0 || row >= m_List.size())
        return QModelIndex();

    return createIndex(row, column, m_List[row].data());
}

QModelIndex CBattleSettingModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

CBattleSetting *CBattleSettingModel::BattleSettingFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        return static_cast<CBattleSetting *>(index.internalPointer());
    } else {
        return NULL;
    }
}

QVariant CBattleSettingModel::data(const QModelIndex &index, int role) const
{
    CBattleSetting *item = BattleSettingFromIndex(index);
    if(item)
    {
        if (role == Qt::DisplayRole)
        {
            switch(index.column())
            {
            case 0:
            {
                QString str;
                item->GetConditionName(str);
                return str;
            }
            case 1:
            {
                QString str;
                item->GetCondition2Name(str);
                return str;
            }
            case 2:
            {
                QString str;
                item->GetPlayerActionName(str, false);
                return str;
            }
            case 3:
            {
                QString str;
                item->GetPlayerTargetName(str);
                return str;
            }
            case 4:
            {
                QString str;
                item->GetPetActionName(str, false);
                return str;
            }
            case 5:
            {
                QString str;
                item->GetPetTargetName(str);
                return str;
            }
           /* case 6:
            {
                QString str;
                item->GetPetAction2Name(str);
                return str;
            }
            case 7:
            {
                QString str;
                item->GetPetTarget2Name(str);
                return str;
            }*/
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

QVariant CBattleSettingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section >= 0 && section < m_HeaderString.size())
        return m_HeaderString[section];

    return QVariant();
}
