#ifndef PLAYERFORM_H
#define PLAYERFORM_H

#include <QWidget>
#include <QThread>
#include <QStandardItemModel>
#include "player.h"
#include "battle.h"

#include "qhttpclient.hpp"
#include "qhttpclientrequest.hpp"
#include "qhttpclientresponse.hpp"

namespace Ui {
class PlayerForm;
}

class PlayerForm : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerForm(CPlayerWorker *worker,CBattleWorker *bworker, QWidget *parent = 0);
    ~PlayerForm();
private:
    void ClearPlayerInfo();
    void ClearMapInfo(bool bIsConnected, bool bIsInGame);
    QString FormatPunchClock(int val);
private slots:
    void OnSetMoveSpeed(int value);
    void OnSetWorkDelay(int value);
    void OnSetWorkAcc(int value);
    void on_pushButton_save_clicked();
    void on_pushButton_load_clicked();
    bool ParsePlayerSettings(const QJsonValue &val);
    bool ParseSettings(const QByteArray &data, QJsonDocument &doc);
    void SaveSettings(QJsonDocument &doc);

public slots:
    void OnCloseWindow();
    void OnNotifyBattleAction(int flags);
    void OnNotifyChatMsg(int unitid, QString msg, int size, int color);
    void OnNotifyGetInfoFailed(bool bIsConnected, bool bIsInGame);
    void OnNotifyGetPlayerInfo(QSharedPointer<CGA_PlayerInfo_t> player);
    void OnNotifyGetPetsInfo(QSharedPointer<CGA_PetList_t> pets);
    void OnNotifyGetSkillsInfo(QSharedPointer<CGA_SkillList_t> pets);
    void OnNotifyGetMapInfo(QString name, int index1, int index2, int index3, int x, int y, int worldStatus, int gameStatus);
    void OnNotifyFillLoadSettings(QString path);
    void OnHttpGetSettings(QJsonDocument *doc);
    void OnHttpLoadSettings(QString query, QByteArray postdata, QJsonDocument* doc);
signals:
    bool ParseItemIdMap(const QJsonValue &val);
    bool ParseItemDropper(const QJsonValue &val);
    bool ParseItemTweaker(const QJsonValue &val);
    bool ParseBattleSettings(const QJsonValue &val);
    bool ParseChatSettings(const QJsonValue &val);
    void SaveItemTweaker(QJsonArray &arr);
    void SaveItemDropper(QJsonArray &arr);
    void SaveItemIdMap(QJsonObject &obj);
    void SaveChatSettings(QJsonObject &obj);
    void SaveBattleSettings(QJsonObject &obj);
private:
    Ui::PlayerForm *ui;

    CPlayerWorker *m_worker;
    QStandardItemModel *m_model_Pet;
    QStandardItemModel *m_model_Skill;
    int m_ServerIndex;
    qhttp::client::QHttpClient *m_HttpClient;
};

#endif // PLAYERFORM_H
