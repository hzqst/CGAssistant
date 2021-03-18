#include "itemform.h"
#include "ui_itemform.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>

ItemForm::ItemForm(CPlayerWorker *worker, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ItemForm),  m_worker(worker)
{
    ui->setupUi(this);

    m_model_Item = new QStandardItemModel(ui->tableView);
    ui->tableView->setModel(m_model_Item);

    for(int i = 0;i < 8; ++i)
        for(int j = 0;j < 5; ++j)
            m_model_Item->setItem(i, j, new QStandardItem());

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);

    m_model_Drop = new CItemDropperModel(ui->listView_drop);
    ui->listView_drop->setModel(m_model_Drop);

    m_model_Tweaker = new CItemTweakerModel(ui->listView_tweak);
    ui->listView_tweak->setModel(m_model_Tweaker);

    connect(this, &ItemForm::QueueDropItem, m_worker, &CPlayerWorker::OnQueueDropItem, Qt::QueuedConnection);
    connect(m_model_Drop, SIGNAL(syncList(CItemDropperList)), m_worker, SLOT(OnSyncItemDroppers(CItemDropperList)), Qt::ConnectionType::QueuedConnection);
    connect(m_model_Tweaker, SIGNAL(syncList(CItemTweakerList)), m_worker, SLOT(OnSyncItemTweakers(CItemTweakerList)), Qt::ConnectionType::QueuedConnection);
    connect(m_worker, &CPlayerWorker::NotifyGetInfoFailed, this, &ItemForm::OnNotifyGetInfoFailed, Qt::QueuedConnection);
    connect(m_worker, &CPlayerWorker::NotifyGetItemsInfo, this, &ItemForm::OnNotifyGetItemsInfo, Qt::ConnectionType::QueuedConnection);
    connect(ui->listView_drop, &MyListView::NotifyDeletePressed, this, &ItemForm::OnItemDropperDeletePressed);
    connect(ui->listView_tweak, &MyListView::NotifyDeletePressed, this, &ItemForm::OnItemTweakerDeletePressed);

    m_item_Menu = new QMenu(this);
}

ItemForm::~ItemForm()
{
    delete ui;
}

void ItemForm::OnCloseWindow()
{

}

void ItemForm::GetRowColumnFromItemPos(int itempos, int &row, int &col)
{
    row = itempos / 5;
    col = itempos % 5;
}

void ItemForm::OnNotifyGetInfoFailed(bool bIsConnected, bool bIsInGame)
{
    int row, col;
    for(int i = 0;i < 40; ++i)
    {
        GetRowColumnFromItemPos(i, row, col);
        QStandardItem *qItem = m_model_Item->item(row, col);
        if(qItem)
        {
            if(!qItem->text().isEmpty())
                qItem->setText(QLatin1String(""));
            qItem->setData(QVariant(false), Qt::UserRole + 10083);
        }
    }
}

bool ItemForm::GetItemTips(int itemid, QString &str)
{
    QHash<int, QString>::const_iterator itor = m_IdMap.constFind(itemid);
    if(itor != m_IdMap.constEnd())
    {
        str = itor.value();
        return true;
    }

    return false;
}

void ItemForm::OnNotifyGetItemsInfo(QSharedPointer<CGA_ItemList_t> items)
{
    int row, col;
    for(int i = 0;i < 40; ++i)
    {
        GetRowColumnFromItemPos(i, row, col);
        QStandardItem *qItem = m_model_Item->item(row, col);
        if(qItem)
        {
            qItem->setText(QLatin1String(""));
            qItem->setData(QVariant(false), Qt::UserRole + 10083);
        }
    }
    for(int i = 0;i < items->size(); ++i)
    {
        const CGA_ItemInfo_t &item = items->at(i);
        if(item.pos < 8)
            continue;
        GetRowColumnFromItemPos(item.pos - 8, row, col);
        QStandardItem *qItem = m_model_Item->item(row, col);
        if(qItem)
        {
            QString str, tips;
            if(item.count > 0)
                str = QString("%1 x %2\n#%3 @%4").arg(item.name, QString::number(item.count), QString::number(item.itemid), QString::number(item.type));
            else
                str = QString("%1\n#%2 @%3").arg(item.name, QString::number(item.itemid), QString::number(item.type));

            if(GetItemTips(item.itemid, tips) && !tips.isEmpty())
                str += QString("\n(%1)").arg(tips);

            qItem->setText(str);
            qItem->setData(QVariant(true), Qt::UserRole + 10083);
            qItem->setData(QVariant(item.pos), Qt::UserRole + 10084);
            qItem->setData(QVariant(item.itemid), Qt::UserRole + 10085);
            qItem->setData(QVariant(item.name), Qt::UserRole + 10086);
        }
    }
}

class CItemNameDropper : public CItemDropper
{
public:
    CItemNameDropper(const QString &name)
    {
        m_name = name;
    }

    virtual bool ShouldDrop(const CGA_ItemInfo_t &item)
    {
        if(0 == item.name.compare(m_name, Qt::CaseInsensitive))
            return true;
        return false;
    }

    virtual void GetName(QString &str)
    {
        str = m_name;
    }

private:
    QString m_name;
};

class CItemIdDropper : public CItemDropper
{
public:
    CItemIdDropper(int itemid)
    {
        m_itemid = itemid;
    }

    virtual bool ShouldDrop(const CGA_ItemInfo_t &item)
    {
        if(item.itemid == m_itemid)
            return true;
        return false;
    }

    virtual void GetName(QString &str)
    {
        str = QString("#%1").arg(m_itemid);
    }

private:
    int m_itemid;
};

class CItemNameTweaker : public CItemTweaker
{
public:
    CItemNameTweaker(const QString &name, int maxcount)
    {
        m_name = name;
        m_maxcount = maxcount;
    }

    virtual bool ShouldTweak(const CGA_ItemInfo_t &item)
    {
        if(0 == item.name.compare(m_name, Qt::CaseInsensitive) && item.assessed && item.count < m_maxcount)
            return true;
        return false;
    }

    virtual void GetName(QString &str)
    {
        str = QString("%1|%2").arg(m_name).arg(m_maxcount);
    }

private:
    QString m_name;
};

class CItemIdTweaker : public CItemTweaker
{
public:
    CItemIdTweaker(int itemid, int maxcount)
    {
        m_itemid = itemid;
        m_maxcount = maxcount;
    }

    virtual bool ShouldTweak(const CGA_ItemInfo_t &item)
    {
        if(item.itemid == m_itemid && item.assessed && item.count < m_maxcount)
            return true;

        return false;
    }

    virtual void GetName(QString &str)
    {
        str = QString("#%1|%2").arg(m_itemid).arg(m_maxcount);
    }

private:
    int m_itemid;
};

void ItemForm::AddItemDropper(const QString &str)
{
    if(str.isEmpty())
        return;
    if(str.at(0) == '#'){
        bool bValue = false;
        int value = str.mid(1).toInt(&bValue);
        if(value)
        {
            CItemDropperPtr ptr(new CItemIdDropper(value));
            m_model_Drop->appendRow(ptr);
            ui->lineEdit_drop->clear();
            return;
        }
    }

    CItemDropperPtr ptr(new CItemNameDropper(str));
    m_model_Drop->appendRow(ptr);
    ui->lineEdit_drop->clear();
}

void ItemForm::AddItemTweaker(const QString &str)
{
    if(str.isEmpty())
        return;

    auto strip = str.indexOf("|");
    if(strip <= 0)
        return;

    bool bValue = false;
    int maxcount = str.mid(strip+1).toInt(&bValue);
    if(!(bValue && maxcount > 0))
        return;

    if(str.at(0) == '#'){
        int value = str.mid(1, strip-1).toInt(&bValue);
        if(bValue && value > 0)
        {
            CItemTweakerPtr ptr(new CItemIdTweaker(value, maxcount));
            m_model_Tweaker->appendRow(ptr);
            ui->lineEdit_tweak->clear();
            return;
        }
    }

    CItemTweakerPtr ptr(new CItemNameTweaker(str.mid(0, strip), maxcount));
    m_model_Tweaker->appendRow(ptr);
    ui->lineEdit_tweak->clear();
}

void ItemForm::on_lineEdit_drop_returnPressed()
{
    AddItemDropper(ui->lineEdit_drop->text());
}

void ItemForm::OnItemDropperDeletePressed(int row)
{
    m_model_Drop->removeRow(row);
}

void ItemForm::OnItemTweakerDeletePressed(int row)
{
    m_model_Tweaker->removeRow(row);
}

void ItemForm::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex modelIndex = ui->tableView->indexAt(pos);
    if(modelIndex.isValid())
    {
        auto qItem = m_model_Item->item(modelIndex.row(), modelIndex.column());
        if(qItem && qItem->data(Qt::UserRole + 10083).toBool() == true) {
            m_item_Menu->clear();
            {
                auto pAction = m_item_Menu->addAction(tr("Drop %1").arg(qItem->data(Qt::UserRole + 10086).toString()));
                pAction->setProperty("itempos", qItem->data(Qt::UserRole + 10084).toInt());
                pAction->setProperty("itemid", qItem->data(Qt::UserRole + 10085).toInt());
                connect(pAction, SIGNAL(triggered(bool)), this, SLOT(OnDropItemAction()));
            }
            {
                auto pAction = m_item_Menu->addAction(tr("Add to auto-drop..."));
                pAction->setProperty("itemid", qItem->data(Qt::UserRole + 10085).toInt());
                connect(pAction, SIGNAL(triggered(bool)), this, SLOT(OnAddAutoDropAction()));
            }
            {
                auto pAction = m_item_Menu->addAction(tr("Add to auto-tweak..."));
                pAction->setProperty("itemid", qItem->data(Qt::UserRole + 10085).toInt());
                pAction->setProperty("itemname", qItem->data(Qt::UserRole + 10086).toString());
                connect(pAction, SIGNAL(triggered(bool)), this, SLOT(OnAddAutoTweakAction()));
            }
            m_item_Menu->exec(QCursor::pos());
        }
    }
}

void ItemForm::OnDropItemAction()
{
    auto pAction = qobject_cast<QAction*>(sender());
    QueueDropItem(pAction->property("itempos").toInt(), pAction->property("itemid").toInt());
}

void ItemForm::OnAddAutoDropAction()
{
    auto pAction = qobject_cast<QAction*>(sender());
    AddItemDropper(QString("#%1").arg(pAction->property("itemid").toInt()));
}

void ItemForm::OnAddAutoTweakAction()
{
    auto pAction = qobject_cast<QAction*>(sender());
    ui->lineEdit_tweak->setText(QString("%1|").arg(pAction->property("itemname").toString()));
}

bool ItemForm::ParseItemIdMap(const QJsonValue &val)
{
    if(!val.isObject())
        return false;

    m_IdMap.clear();
    const QJsonObject obj = val.toObject();
    QJsonObject::const_iterator itor = obj.constBegin();
    while(itor != obj.constEnd())
    {
        if((*itor).isString())
        {
            bool ok = false;
            int id = itor.key().toInt(&ok);
            QString name = (*itor).toString();
            if(ok && !name.isEmpty())
                m_IdMap.insert(id, name);
        }
    }
    return true;
}

bool ItemForm::ParseItemDropper(const QJsonValue &val)
{
    if(!val.isArray())
        return false;
    const QJsonArray arr = val.toArray();
    QJsonArray::const_iterator itor = arr.constBegin();
    m_model_Drop->removeRows(0, m_model_Drop->rowCount());
    while(itor != arr.constEnd())
    {
        if((*itor).isString())
        {
            QString line = (*itor).toString();
            if(!line.isEmpty())
                AddItemDropper(line);
        }
        ++itor;
    }
    return true;
}

void ItemForm::SaveItemDropper(QJsonArray &arr)
{
    int rowCount = m_model_Drop->rowCount();
    for(int i = 0;i < rowCount; ++i)
    {
        CItemDropper *dropper = m_model_Drop->DropperFromIndex(m_model_Drop->index(i, 0));
        if(dropper)
        {
            QString str;
            dropper->GetName(str);
            arr.insert(arr.end(), str);
        }
    }
}

bool ItemForm::ParseItemTweaker(const QJsonValue &val)
{
    if(!val.isArray())
        return false;
    const QJsonArray arr = val.toArray();
    QJsonArray::const_iterator itor = arr.constBegin();
    m_model_Tweaker->removeRows(0, m_model_Tweaker->rowCount());
    while(itor != arr.constEnd())
    {
        if((*itor).isString())
        {
            QString line = (*itor).toString();
            if(!line.isEmpty())
                AddItemTweaker(line);
        }
        ++itor;
    }
    return true;
}

void ItemForm::SaveItemTweaker(QJsonArray &arr)
{
    int rowCount = m_model_Tweaker->rowCount();
    for(int i = 0;i < rowCount; ++i)
    {
        CItemTweaker *tweaker = m_model_Tweaker->TweakerFromIndex(m_model_Tweaker->index(i, 0));
        if(tweaker)
        {
            QString str;
            tweaker->GetName(str);
            arr.insert(arr.end(), str);
        }
    }
}

void ItemForm::SaveItemIdMap(QJsonObject &obj)
{
    QHash<int, QString>::const_iterator itor = m_IdMap.constBegin();
    while(itor != m_IdMap.constEnd())
    {
        obj.insert(QString::number(itor.key()), itor.value());
        ++itor;
    }
}

void ItemForm::on_lineEdit_tweak_returnPressed()
{
     AddItemTweaker(ui->lineEdit_tweak->text());
}
