#ifndef PLAYERFORM_H
#define PLAYERFORM_H

#include <QWidget>
#include <QThread>
#include <QStandardItemModel>
#include "player.h"

namespace Ui {
class PlayerForm;
}

class PlayerForm : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerForm(CPlayerWorker *worker, QWidget *parent = 0);
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
    void SaveSettings(QByteArray &data);
public slots:
    void OnCloseWindow();
    void OnNotifyGetInfoFailed(bool bIsConnected, bool bIsInGame);
    void OnNotifyGetPlayerInfo(QSharedPointer<CGA_PlayerInfo_t> player);
    void OnNotifyGetPetsInfo(QSharedPointer<CGA_PetList_t> pets);
    void OnNotifyGetSkillsInfo(QSharedPointer<CGA_SkillList_t> pets);
    void OnNotifyGetMapInfo(QString name, int x, int y, int worldStatus, int gameStatus);
signals:
    bool ParseItemIdMap(const QJsonValue &val);
    bool ParseItemDropper(const QJsonValue &val);
    bool ParseBattleSettings(const QJsonValue &val);
    void SaveItemDropper(QJsonArray &arr);
    void SaveItemIdMap(QJsonObject &obj);
    void SaveBattleSettings(QJsonObject &obj);
private:
    Ui::PlayerForm *ui;

    CPlayerWorker *m_worker;
    QStandardItemModel *m_model_Pet;
    QStandardItemModel *m_model_Skill;
};

#endif // PLAYERFORM_H
