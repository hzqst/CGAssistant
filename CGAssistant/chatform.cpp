#include "chatform.h"
#include "ui_chatform.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QDateTime>

extern CGA::CGAInterface *g_CGAInterface;

ChatForm::ChatForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatForm)
{
    ui->setupUi(this);
}

ChatForm::~ChatForm()
{
    delete ui;
}

void ChatForm::on_lineEdit_returnPressed()
{
    auto saystring = ui->lineEdit->text()./*toLocal8Bit().*/toStdString();
    int ingame = 0;
    if(g_CGAInterface->IsInGame(ingame) && ingame){
        g_CGAInterface->SayWords(saystring, 0, 3, 1);
    }

    ui->lineEdit->setText("");
}

void ChatForm::UpdateFromDatabase(QString playername, int flags)
{
    return;

    if(playername.isEmpty())
        return;

    if(!db.isOpen())
    {
        m_dbplayername = playername;

        playername = playername.replace(QString("\\"),QString("%5C"));
        playername = playername.replace(QString("/"),QString("%2F"));
        playername = playername.replace(QString(":"),QString("%3A"));
        playername = playername.replace(QString("*"),QString("%2A"));
        playername = playername.replace(QString("?"),QString("%3F"));
        playername = playername.replace(QString("\""),QString("%22"));
        playername = playername.replace(QString("<"),QString("%3C"));
        playername = playername.replace(QString(">"),QString("%3E"));
        playername = playername.replace(QString("|"),QString("%7C"));

        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(tr("%1\\chatdb\\%2.db").arg(QApplication::applicationDirPath()).arg(playername));
        if (!db.open())
        {
            ui->textEdit_chat->setText(tr("Error: Failed to connect sqlite.\nChat database will not work properly."));
            ui->textEdit_chat->append(db.lastError().text());
            return;            
        }
        else
        {
            QSqlQuery query(db);
            if(!query.exec("create table if not exists msg_table(id int primary key,name varchar(64), msg varchar(256), size int, color int, type int, uploadtime int)"))
            {
                ui->textEdit_chat->setText(tr("Error: Failed to create table.\nChat database will not work properly."));
                ui->textEdit_chat->append(query.lastError().text());
                return;
            }
            if(!query.exec("create index if not exists type_index on msg_table(type)"))
            {
                ui->textEdit_chat->setText(tr("Error: Failed to create index for type.\nChat database will not work properly."));
                ui->textEdit_chat->append(query.lastError().text());
                return;
            }
            if(!query.exec("create index if not exists type_index on msg_table(uploadtime)"))
            {
                ui->textEdit_chat->setText(tr("Error: Failed to create index for uploadtime.\nChat database will not work properly."));
                ui->textEdit_chat->append(query.lastError().text());
                return;
            }
        }
    } else if(0 != m_dbplayername.compare(playername)){
        m_dbplayername = playername;
        db.close();
        UpdateFromDatabase(playername, flags);
        return;
    }

    if(flags & 1)
    {
        QStringList orsql;
        orsql.append(QString("%1").arg(-1));
        if(ui->checkBox_mychat->isChecked())
            orsql.append(QString("%1").arg(1));
        if(ui->checkBox_playerchat->isChecked())
            orsql.append(QString("%1").arg(2));
        if(ui->checkBox_npcmsg->isChecked())
            orsql.append(QString("%1").arg(3));
        if(ui->checkBox_sysmsg->isChecked())
            orsql.append(QString("%1").arg(0));

        QDateTime time = QDateTime::currentDateTime();

        time = time.addDays(-1);

        QString sql = QString("select * from msg_table where type in (%1) and uploadtime >= %2 order by uploadtime asc").arg(orsql.join(",")).arg(
                    ui->checkBox_today->isChecked()?
                    time.toTime_t() : 0);

        bool displayTime = ui->checkBox_displaytime->isChecked();
        bool displayName = ui->checkBox_displayname->isChecked();

        QSqlQuery query(db);
        if(query.exec(sql))
        {
            QString all;
            while(query.next())
            {
                QString name = query.value("name").toString();
                QString msg = query.value("msg").toString();
                int type = query.value("type").toInt();
                uint32_t time = query.value("uploadtime").toUInt();
                msg = msg.replace("\\c", ",");
                if(displayTime)
                {
                    all.append(QString("[%1] ").arg(QDateTime::fromTime_t(time).toString("yyyy-MM-dd hh:mm:ss")));
                }

                if(displayName)
                {
                    if(type == 0)
                    {
                        all.append(QString("<font color=\"#FF0000\">%1</font><br>").arg(msg));
                    }
                    else if(type == 1)
                    {
                        all.append(QString("<font color=\"#0000FF\">%1: %2</font><br>").arg(name).arg(msg));
                    }
                    else
                    {
                        all.append(QString("%1: %2<br>").arg(name).arg(msg));
                    }
                }
                else
                {
                    if(type == 0)
                    {
                        all.append(QString("<font color=\"#FF0000\">%1</font><br>").arg(msg));
                    }
                    else if(type == 1)
                    {
                        all.append(QString("<font color=\"#0000FF\">%2</font><br>").arg(msg));
                    }
                    else
                    {
                        all.append(QString("%1<br>").arg(msg));
                    }
                }
            }

            ui->textEdit_chat->setText(all);
            ui->textEdit_chat->moveCursor(QTextCursor::End);
        }
    }
}

void ChatForm::OnNotifyGetPlayerInfo(QSharedPointer<CGA_PlayerInfo_t> player)
{
    m_player = player;
    if(0 != m_dbplayername.compare(m_player->name)){
       UpdateFromDatabase(m_player->name, 1);
    }
}

void ChatForm::OnNotifyChatMsg(int unitid, QString msg, int size, int color)
{
    if(m_player && !m_player->name.isEmpty())
    {
        UpdateFromDatabase(m_player->name, 0);

        int type = -1;
        QString name;
        if(unitid > 0){
            if(m_player->unitid == unitid)
            {
                type = 1;
                name = m_player->name;
            }
            else
            {
                CGA::cga_map_units_t units;
                if(g_CGAInterface->GetMapUnits(units))
                {
                    for(const auto &u :units){
                        if(u.valid == 2 && ((u.flags & 256) || (u.flags & 4096)) && u.unit_id == unitid){
                            type = (u.flags & 256) ? 2 : 3;
                            name = QString::fromStdString(u.unit_name);
                            break;
                        }
                    }
                }
            }
        } else {
            type = 0;
        }

        if(type == -1)
            return;

        if(type >= 1 && type <= 3)
        {
            auto header = QString("%1: ").arg(name);
            if(msg.indexOf(header) == 0)
            {
                msg = msg.mid(header.length());
            }
            else {
                auto header2 = QString("[GP]%1: ").arg(name);
                if(msg.indexOf(header2) == 0)
                {
                    msg = msg.mid(header2.length());
                }
            }
        }

        QSqlQuery query(db);

        query.prepare("insert into msg_table (name,msg,size,color,type,uploadtime) values (?,?,?,?,?,?)");

        QDateTime time = QDateTime::currentDateTime();

        query.addBindValue(QVariant(name));
        query.addBindValue(QVariant(msg));
        query.addBindValue(QVariant(size));
        query.addBindValue(QVariant(color));
        query.addBindValue(QVariant(type));
        query.addBindValue(QVariant(time.toTime_t()));

        query.exec();

        UpdateFromDatabase(m_player->name, 1);
    }
}

void ChatForm::on_checkBox_displaytime_stateChanged(int arg1)
{
    if(m_player)
        UpdateFromDatabase(m_player->name, 1);
}

void ChatForm::on_checkBox_sysmsg_stateChanged(int arg1)
{
    if(m_player)
        UpdateFromDatabase(m_player->name, 1);
}

void ChatForm::on_checkBox_mychat_stateChanged(int arg1)
{
    if(m_player)
        UpdateFromDatabase(m_player->name, 1);
}

void ChatForm::on_checkBox_playerchat_stateChanged(int arg1)
{
    if(m_player)
        UpdateFromDatabase(m_player->name, 1);
}

void ChatForm::on_checkBox_npcmsg_stateChanged(int arg1)
{
    if(m_player)
        UpdateFromDatabase(m_player->name, 1);
}

void ChatForm::on_checkBox_displayname_stateChanged(int arg1)
{
    if(m_player)
        UpdateFromDatabase(m_player->name, 1);
}

bool ChatForm::ParseChatSettings(const QJsonValue &val)
{
    if(!val.isObject())
        return false;

    auto obj = val.toObject();
    ui->checkBox_displaytime->setChecked(obj.take("displaytime").toBool());
    ui->checkBox_displayname->setChecked(obj.take("displayname").toBool());
    ui->checkBox_sysmsg->setChecked(obj.take("sysmsg").toBool());
    ui->checkBox_mychat->setChecked(obj.take("mychat").toBool());
    ui->checkBox_playerchat->setChecked(obj.take("playerchat").toBool());
    ui->checkBox_npcmsg->setChecked(obj.take("npcmsg").toBool());
    ui->checkBox_today->setChecked(obj.take("todayonly").toBool());

    return true;
}

void ChatForm::SaveChatSettings(QJsonObject &obj)
{
    obj.insert("displaytime", ui->checkBox_displaytime->isChecked());
    obj.insert("displayname", ui->checkBox_displayname->isChecked());
    obj.insert("sysmsg", ui->checkBox_sysmsg->isChecked());
    obj.insert("mychat", ui->checkBox_mychat->isChecked());
    obj.insert("playerchat", ui->checkBox_playerchat->isChecked());
    obj.insert("npcmsg", ui->checkBox_npcmsg->isChecked());
    obj.insert("todayonly", ui->checkBox_today->isChecked());
}
