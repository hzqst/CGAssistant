#ifndef CHATFORM_H
#define CHATFORM_H

#include <QWidget>
#include <QSqlDatabase>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "player.h"

namespace Ui {
class ChatForm;
}

class ChatForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChatForm(QWidget *parent = nullptr);
    ~ChatForm();

public slots:
    void OnNotifyGetPlayerInfo(QSharedPointer<CGA_PlayerInfo_t> player);
    void OnNotifyChatMsg(int unitid, QString msg, int size, int color);
    void UpdateFromDatabase(QString playername, int flags);
    bool ParseChatSettings(const QJsonValue &val);
    void SaveChatSettings(QJsonObject &obj);

private slots:
    void on_lineEdit_returnPressed();

    void on_checkBox_displaytime_stateChanged(int arg1);

    void on_checkBox_sysmsg_stateChanged(int arg1);

    void on_checkBox_mychat_stateChanged(int arg1);

    void on_checkBox_playerchat_stateChanged(int arg1);

    void on_checkBox_npcmsg_stateChanged(int arg1);

    void on_checkBox_displayname_stateChanged(int arg1);

private:
    Ui::ChatForm *ui;
    QSqlDatabase db;
    QSharedPointer<CGA_PlayerInfo_t> m_player;
    QString m_dbplayername;
};

#endif // CHATFORM_H
