#include <QDateTime>
#include "processtable.h"

CProcessTableModel::CProcessTableModel(QObject *parent, CProcessWorker *work) : QAbstractTableModel(parent), m_worker(work)
{
    m_HeaderString.append(tr("PID"));
    m_HeaderString.append(tr("Window Title"));
    m_HeaderString.append(tr("Attached"));
}

void CProcessTableModel::appendRow(const CProcessItemPtr &item)
{
    int row = rowCount();

    beginInsertRows(QModelIndex(), row, row);

    m_List.append(item);

    endInsertRows();
}

bool CProcessTableModel::removeRow(int position, const QModelIndex &parent)
{
    bool success = false;

    if(position >= 0 && position < m_List.size())
    {
        beginRemoveRows(parent, position, position);
        m_List.erase(m_List.begin() + position);
        success = true;
        endRemoveRows();
    }

    return success;
}

bool CProcessTableModel::removeRows(int position, int count, const QModelIndex &parent)
{
    bool success = false;

    if(count > 0 && position >= 0 && position + count - 1 < m_List.size())
    {
        beginRemoveRows(parent, position, position + count - 1);
        m_List.erase(m_List.begin() + position, m_List.begin() + position + count);
        success = true;
        endRemoveRows();
    }

    return success;
}

Qt::ItemFlags CProcessTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

int CProcessTableModel::rowCount(const QModelIndex &parent) const
{
    return (int)m_List.size();
}

int CProcessTableModel::columnCount(const QModelIndex &parent) const
{
    return m_HeaderString.size();
}

QModelIndex CProcessTableModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0 || row >= m_List.size())
        return QModelIndex();

    return createIndex(row, column, (void *)m_List[row].data());
}

QModelIndex CProcessTableModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

CProcessItem *CProcessTableModel::ItemFromIndex(const QModelIndex &index) const
{
        if (index.isValid()) {
                return static_cast<CProcessItem *>(index.internalPointer());
        } else {
                return NULL;
        }
}

QVariant CProcessTableModel::data(const QModelIndex &index, int role) const
{
    const CProcessItem *process = ItemFromIndex(index);
    if(process)
    {
        if (role == Qt::DisplayRole)
        {
            switch(index.column())
            {
            case 0: return QString::number(process->m_ProcessId);
            case 1: return process->m_Title;
            case 2: {
                if(process->m_bAttached && process->m_hWnd == m_worker->GetAttachedHwnd())
                    return QString("yes(*)");
                if(process->m_bAttached)
                    return QString("yes");
            }
            }
        }
        else if (role == Qt::DecorationRole)
        {
            //if(index.column() == 1)
            //    return *ev->GetUniqueProcess()->m_Icon;
        }
        else if (role == Qt::TextAlignmentRole)
        {
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        }
    }
    return QVariant();
}

QVariant CProcessTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section >= 0 && section < m_HeaderString.size())
          return m_HeaderString[section];

    return QVariant();
}
