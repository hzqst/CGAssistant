#include "chatform.h"
#include "ui_chatform.h"

#include <QTimer>
#include <QDateTime>

extern CGA::CGAInterface *g_CGAInterface;

ChatForm::ChatForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatForm)
{
    ui->setupUi(this);

    m_ChatMaxLines = 100;
    ui->textEdit_chat->document()->setMaximumBlockCount(m_ChatMaxLines);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(OnAutoChat()));
    timer->start(1000);
}

ChatForm::~ChatForm()
{
    delete ui;
}

void ChatForm::OnAutoChat()
{
    if(g_CGAInterface->IsConnected())
    {
        g_CGAInterface->SetBlockAllChatMsg(true);
    }
}

void ChatForm::on_lineEdit_returnPressed()
{
    auto saystring = ui->lineEdit->text().toStdString();
    int ingame = 0;
    if(g_CGAInterface->IsInGame(ingame) && ingame){
        g_CGAInterface->SayWords(saystring, 0, 3, 1);
    }

    ui->lineEdit->setText("");
}

void ChatForm::OnNotifyGetPlayerInfo(QSharedPointer<CGA_PlayerInfo_t> player)
{
    m_player = player;
}

void ChatForm::OnNotifyFillChatSettings(bool blockallchatmsgs)
{
    ui->checkBox_BlockAllChatMsgs->setChecked(blockallchatmsgs);
}

void ChatForm::OnNotifyFillStaticSettings(int freezetime, int chatmaxlines)
{
    m_ChatMaxLines = chatmaxlines;
    ui->textEdit_chat->document()->setMaximumBlockCount(m_ChatMaxLines);
}

void ChatForm::OnNotifyChatMsg(int unitid, QString msg, int size, int color)
{
    if(m_player && !m_player->name.isEmpty())
    {
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

        QString line;

        msg = msg.replace("\\c", ",");
        line.append(QString("[%1] ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));

        if(type == 0)
        {
            line.append(QString("<font color=\"#FF0000\">%1</font><br>").arg(msg));
        }
        else if(type == 1)
        {
            line.append(QString("<font color=\"#0000FF\">%1: %2</font><br>").arg(name).arg(msg));
        }
        else
        {
            line.append(QString("%1: %2<br>").arg(name).arg(msg));
        }

        QTextCursor txtcur = ui->textEdit_chat->textCursor();

        /*if(m_ChatMaxLines > 0){
            if (ui->textEdit_chat->document()->lineCount() > m_ChatMaxLines)
            {
                txtcur.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
                txtcur.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
                txtcur.removeSelectedText();
            }
        }*/

        txtcur.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        txtcur.insertHtml(line);
        txtcur.insertBlock();
    }
}

bool ChatForm::ParseChatSettings(const QJsonValue &val)
{
    if(!val.isObject())
        return false;

    auto obj = val.toObject();

    if(obj.contains("blockallchatmsgs"))
        ui->checkBox_BlockAllChatMsgs->setChecked(obj.take("blockallchatmsgs").toBool());

    return true;
}

void ChatForm::SaveChatSettings(QJsonObject &obj)
{
    obj.insert("blockallchatmsgs", ui->checkBox_BlockAllChatMsgs->isChecked());
}

void ChatForm::on_checkBox_BlockAllChatMsgs_stateChanged(int state)
{
    if(g_CGAInterface->IsConnected())
    {
        g_CGAInterface->SetBlockAllChatMsg(state ? true : false);
    }
}

