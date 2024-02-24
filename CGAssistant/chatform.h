#ifndef CHATFORM_H
#define CHATFORM_H

#include <QWidget>
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
    void OnNotifyFillChatSettings(int blockchatmsgs);
    void OnNotifyFillStaticSettings(int freezetime, int chatmaxlines);
    void OnNotifyGetPlayerInfo(QSharedPointer<CGA_PlayerInfo_t> player);
    void OnNotifyChatMsg(int unitid, QString msg, int size, int color);
    bool ParseChatSettings(const QJsonValue &val);
    void SaveChatSettings(QJsonObject &obj);

private slots:
    void on_lineEdit_returnPressed();

    void on_checkBox_BlockAllChatMsgs_stateChanged(int state);

    void OnAutoChat();
private:
    Ui::ChatForm *ui;
    QSharedPointer<CGA_PlayerInfo_t> m_player;
    int m_ChatMaxLines;
};

#endif // CHATFORM_H
