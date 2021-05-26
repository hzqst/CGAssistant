#ifndef AUTOBATTLEFORM_H
#define AUTOBATTLEFORM_H

#include <QWidget>
#include <QThread>

#include "battle.h"
#include "battlesetting.h"

namespace Ui {
class AutoBattleForm;
}

class CAutoBattleWorker;

class AutoBattleForm : public QWidget
{
    Q_OBJECT

public:
    explicit AutoBattleForm(CBattleWorker *worker, CPlayerWorker *pworker, QWidget *parent = 0);
    ~AutoBattleForm();
    void SyncAutoBattleWorker();
private slots:
    void OnCloseWindow();
    void on_horizontalSlider_delayFrom_valueChanged(int value);
    void on_horizontalSlider_delayTo_valueChanged(int value);
    void on_comboBox_condition_type_currentIndexChanged(int index);    
    void on_comboBox_condition2_type_currentIndexChanged(int index);
    void on_comboBox_playerAction_currentIndexChanged(int index);
    void on_comboBox_playerTarget_currentIndexChanged(int index);
    void on_comboBox_petAction_currentIndexChanged(int index);
    void on_comboBox_petTarget_currentIndexChanged(int index);
    void on_pushButton_add_clicked();
    void on_pushButton_del_clicked();
    void on_pushButton_moveUp_clicked();
    void on_pushButton_moveDown_clicked();
    void on_comboBox_petAction_2_currentIndexChanged(int index);
    void on_comboBox_petTarget_2_currentIndexChanged(int index);

public slots:
    void OnNotifyGetSkillsInfo(QSharedPointer<CGA_SkillList_t> skills);
    void OnNotifyGetPetsInfo(QSharedPointer<CGA_PetList_t> pets);
    void OnNotifyGetItemsInfo(QSharedPointer<CGA_ItemList_t> items);
    bool ParseBattleSettings(const QJsonValue &val);
    void SaveBattleSettings(QJsonObject &obj);
private:
    CBattleWorker *m_worker;
    Ui::AutoBattleForm *ui;
    CBattleSettingModel *m_model;
    QSharedPointer<CGA_ItemList_t> m_ItemList;
    QSharedPointer<CGA_PetList_t> m_PetList;
};

#endif // AUTOBATTLEFORM_H
