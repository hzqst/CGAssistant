#ifndef AUTOBATTLE_H
#define AUTOBATTLE_H

#include <QString>
#include <QObject>
#include <QSharedPointer>
#include <QList>
#include "player.h"

#define BattleCond_Type_EnemyCount 0
#define BattleCond_Type_PlayerHp 1
#define BattleCond_Type_PlayerMp 2
#define BattleCond_Type_PetHp 3
#define BattleCond_Type_PetMp 4
#define BattleCond_Type_TeammateHp 5
#define BattleCond_Type_EnemyUnit 6
#define BattleCond_Type_Max 7

#define BattleCond_NumRel_EGT 0
#define BattleCond_NumRel_GT 1
#define BattleCond_NumRel_ELT 2
#define BattleCond_NumRel_LT 3
#define BattleCond_NumRel_EQ 4
#define BattleCond_NumRel_NEQ 5
#define BattleCond_NumRel_Max 6

#define BattleCond_StrRel_CONTAIN 0
#define BattleCond_StrRel_NOT_CONTAIN 1
#define BattleCond_StrRel_Max 2

#define BattlePlayerAction_Ignore 0
#define BattlePlayerAction_Attack 1
#define BattlePlayerAction_Defense 2
#define BattlePlayerAction_Escape 3
#define BattlePlayerAction_ExchangePosition 4
#define BattlePlayerAction_ChangePet 5
#define BattlePlayerAction_UseItem 6
#define BattlePlayerAction_Skill 100
#define BattlePlayerAction_Max 7

#define BattleTarget_Enemy 0
#define BattleTarget_Teammate 1
#define BattleTarget_Self 2
#define BattleTarget_Pet 3
#define BattleTarget_Max 4

#define BattleTarget_Select_Random 0
#define BattleTarget_Select_Front 1
#define BattleTarget_Select_Back 2
#define BattleTarget_Select_LowHP 3
#define BattleTarget_Select_HighHP 4
#define BattleTarget_Select_LowLv 5
#define BattleTarget_Select_HighLv 6
#define BattleTarget_Select_Goatfarm 7
#define BattleTarget_Select_Boomerang 8
#define BattleTarget_Select_Max 9

#define BattlePetAction_Ignore 0
#define BattlePetAction_Skill 100
#define BattlePetAction_Max 1

typedef struct CGA_BattleContext_s CGA_BattleContext_t;

class CBattleCondition
{
public:
    virtual void GetConditionValue(QString &str) = 0;
    virtual int GetConditionRelId() = 0;
    virtual int GetConditionTypeId() = 0;
    virtual void GetConditionName(QString &str) = 0;
    virtual bool Check(CGA_BattleContext_t &context) = 0;
protected:
};

class CBattleAction
{
public:
    virtual int GetActionTypeId() = 0;
    virtual void GetActionName(QString &str) = 0;
    virtual bool DoAction(int target, CGA_BattleContext_t &context) = 0;
protected:

};

//Target

class CBattleTarget
{
public:
    virtual int GetTargetTypeId() = 0;
    virtual int GetTargetSelectId() = 0;
    virtual void GetTargetName(QString &str) = 0;
    virtual int GetTarget(CGA_BattleContext_t &context) = 0;
};

class CBattleCondition_EnemyCount : public CBattleCondition
{
public:
    CBattleCondition_EnemyCount(int relation, int value);
    virtual void GetConditionValue(QString &str) {
        str = QString::number(m_value);
    }
    virtual int GetConditionRelId() { return m_relation; }
    virtual int GetConditionTypeId() {return BattleCond_Type_EnemyCount;}
    virtual void GetConditionName(QString &str);
    virtual bool Check(CGA_BattleContext_t &context);
protected:
    int m_relation;
    int m_value;
};

class CBattleCondition_PlayerHp : public CBattleCondition
{
public:
    CBattleCondition_PlayerHp(int relation, int value, bool percentage);
    virtual void GetConditionValue(QString &str) {
        str = QString::number(m_value);
        if(m_percentage)
           str += QLatin1String("%");
    }
    virtual int GetConditionRelId() { return m_relation; }
    virtual int GetConditionTypeId() {return BattleCond_Type_PlayerHp;}
    virtual void GetConditionName(QString &str);
    virtual bool Check(CGA_BattleContext_t &context);
protected:
    int m_relation;
    int m_value;
    bool m_percentage;
};

class CBattleCondition_PlayerMp : public CBattleCondition
{
public:
    CBattleCondition_PlayerMp(int relation, int value, bool percentage);
    virtual void GetConditionValue(QString &str) {
        str = QString::number(m_value);
        if(m_percentage)
           str += QLatin1String("%");
    }
    virtual int GetConditionRelId() { return m_relation; }
    virtual int GetConditionTypeId() {return BattleCond_Type_PlayerMp;}
    virtual void GetConditionName(QString &str);
    virtual bool Check(CGA_BattleContext_t &context);
protected:
    int m_relation;
    int m_value;
    bool m_percentage;
};

class CBattleCondition_PetHp : public CBattleCondition
{
public:
    CBattleCondition_PetHp(int relation, int value, bool percentage);
    virtual void GetConditionValue(QString &str) {
        str = QString::number(m_value);
        if(m_percentage)
           str += QLatin1String("%");
    }
    virtual int GetConditionRelId() { return m_relation; }
    virtual int GetConditionTypeId() {return BattleCond_Type_PetHp;}
    virtual void GetConditionName(QString &str);
    virtual bool Check(CGA_BattleContext_t &context);
protected:
    int m_relation;
    int m_value;
    bool m_percentage;
};

class CBattleCondition_PetMp : public CBattleCondition
{
public:
    CBattleCondition_PetMp(int relation, int value, bool percentage);
    virtual void GetConditionValue(QString &str) {
        str = QString::number(m_value);
        if(m_percentage)
           str += QLatin1String("%");
    }
    virtual int GetConditionRelId() { return m_relation; }
    virtual int GetConditionTypeId() {return BattleCond_Type_PetMp;}
    virtual void GetConditionName(QString &str);
    virtual bool Check(CGA_BattleContext_t &context);
protected:
    int m_relation;
    int m_value;
    bool m_percentage;
};

class CBattleCondition_TeammateHp : public CBattleCondition
{
public:
    CBattleCondition_TeammateHp(int relation, int value, bool percentage);
    virtual void GetConditionValue(QString &str) {
        str = QString::number(m_value);
        if(m_percentage)
           str += QLatin1String("%");
    }
    virtual int GetConditionRelId() { return m_relation; }
    virtual int GetConditionTypeId() {return BattleCond_Type_TeammateHp;}
    virtual void GetConditionName(QString &str);
    virtual bool Check(CGA_BattleContext_t &context);
protected:
    int m_relation;
    int m_value;
    bool m_percentage;
};

class CBattleCondition_EnemyUnit : public CBattleCondition
{
public:
    CBattleCondition_EnemyUnit(int relation, QString &unitName);
    virtual void GetConditionValue(QString &str) {
        str = m_UnitName;
    }
    virtual int GetConditionRelId() { return m_relation; }
    virtual int GetConditionTypeId() {return BattleCond_Type_EnemyUnit;}
    virtual void GetConditionName(QString &str);
    virtual bool Check(CGA_BattleContext_t &context);

protected:
    int m_relation;
    QString m_UnitName;
};

class CBattleAction_PlayerAttack : public CBattleAction
{
public:
    virtual int GetActionTypeId() {return BattlePlayerAction_Attack;}
    virtual void GetActionName(QString &str);
    virtual bool DoAction(int target, CGA_BattleContext_t &context);
};

class CBattleAction_PlayerDefense : public CBattleAction
{
public:
    virtual int GetActionTypeId() {return BattlePlayerAction_Defense;}
    virtual void GetActionName(QString &str);
    virtual bool DoAction(int target, CGA_BattleContext_t &context);
};

class CBattleAction_PlayerEscape : public CBattleAction
{
public:
    virtual int GetActionTypeId() {return BattlePlayerAction_Escape;}
    virtual void GetActionName(QString &str);
    virtual bool DoAction(int target, CGA_BattleContext_t &context);
};

class CBattleAction_PlayerExchangePosition : public CBattleAction
{
public:
    virtual int GetActionTypeId() {return BattlePlayerAction_ExchangePosition;}
    virtual void GetActionName(QString &str);
    virtual bool DoAction(int target, CGA_BattleContext_t &context);
};

class CBattleAction_PlayerChangePet : public CBattleAction
{
public:
    CBattleAction_PlayerChangePet(int type);
    CBattleAction_PlayerChangePet(QString &petname);
    virtual int GetActionTypeId() {return BattlePlayerAction_ChangePet;}
    virtual void GetActionName(QString &str);
    virtual bool DoAction(int target, CGA_BattleContext_t &context);
protected:
    QString m_PetName;
    int m_Type;//0 = pet, 1=recall, 2=most_level, 3=most_health
};

class CBattleAction_PlayerSkillAttack : public CBattleAction
{
public:
    CBattleAction_PlayerSkillAttack(QString &skillName, int skillLevel);
     virtual int GetActionTypeId() {return BattlePlayerAction_Skill;}
    virtual void GetActionName(QString &str);
    virtual bool DoAction(int target, CGA_BattleContext_t &context);
    virtual QString GetSkillName(){return m_SkillName; }
     virtual int GetSkillLevel(){return m_SkillLevel; }
private:
    bool GetSkill(CGA_BattleContext_t &context, int &skillpos, int &skilllevel);
    void FixTarget(CGA_BattleContext_t &context, int skillpos, int skilllevel, int &target);
protected:
    QString m_SkillName;
    int m_SkillLevel;//0=max
};

class CBattleAction_PlayerUseItem : public CBattleAction
{
public:
    CBattleAction_PlayerUseItem(QString &itemName);
    CBattleAction_PlayerUseItem(int itemId);
    virtual int GetActionTypeId() {return BattlePlayerAction_UseItem;}
    virtual void GetActionName(QString &str);
    virtual bool DoAction(int target, CGA_BattleContext_t &context);
private:
    bool GetItemPosition(CGA_BattleContext_t &context, int &itempos);
protected:
    QString m_ItemName;//item name or #id
    int m_ItemId;
};

class CBattleAction_PetSkillAttack : public CBattleAction
{
public:
    CBattleAction_PetSkillAttack(QString &skillName);

    virtual int GetActionTypeId() {return BattlePetAction_Skill;}
    virtual void GetActionName(QString &str);
    virtual bool DoAction(int target, CGA_BattleContext_t &context);
    virtual QString GetSkillName(){return m_SkillName; }
private:
    bool GetSkill(CGA_BattleContext_t &context, int &skillpos);
    void FixTarget(CGA_BattleContext_t &context, int skillpos, int &target);
protected:
    QString m_SkillName;
};

class CBattleTarget_Self : public CBattleTarget
{
public:
    virtual int GetTargetSelectId() { return -1; }
    virtual int GetTargetTypeId() {return BattleTarget_Self;}
    virtual void GetTargetName(QString &str);
    virtual int GetTarget(CGA_BattleContext_t &context);
};

class CBattleTarget_Pet : public CBattleTarget
{
public:
    virtual int GetTargetSelectId() { return -1; }
    virtual int GetTargetTypeId() {return BattleTarget_Pet;}
    virtual void GetTargetName(QString &str);
    virtual int GetTarget(CGA_BattleContext_t &context);
};

class CBattleTarget_Enemy : public CBattleTarget
{
public:
    CBattleTarget_Enemy(int select);
    virtual int GetTargetSelectId() { return m_Select; }
    virtual int GetTargetTypeId() {return BattleTarget_Enemy;}
    virtual void GetTargetName(QString &str);
    virtual int GetTarget(CGA_BattleContext_t &context);
protected:
    int m_Select;
};

class CBattleTarget_Teammate : public CBattleTarget
{
public:
    CBattleTarget_Teammate(int select);
    virtual int GetTargetSelectId() { return m_Select; }
    virtual int GetTargetTypeId() {return BattleTarget_Teammate;}
    virtual void GetTargetName(QString &str);
    virtual int GetTarget(CGA_BattleContext_t &context);
protected:
    int m_Select;
};

class CBattleSetting
{
public:
    CBattleSetting(CBattleCondition *cond, CBattleAction *playerAction, CBattleTarget *playerTarget, CBattleAction *petAction, CBattleTarget *petTarget);
    ~CBattleSetting();

    virtual bool DoAction(CGA_BattleContext_t &context);
    virtual void GetConditionName(QString &str);
    virtual int GetConditionRelId();
    virtual void GetConditionValue(QString &str);
    virtual void GetPlayerActionName(QString &str);
    virtual void GetPlayerTargetName(QString &str);
    virtual void GetPetActionName(QString &str);
    virtual void GetPetTargetName(QString &str);
    virtual int GetConditionTypeId();
    virtual int GetPlayerActionTypeId();
    virtual QString GetPlayerSkillName();
    virtual int GetPlayerSkillLevel();
    virtual int GetPlayerTargetTypeId();
    virtual int GetPlayerTargetSelectId();
    virtual int GetPetActionTypeId();
    virtual QString GetPetSkillName();
    virtual int GetPetTargetTypeId();
     virtual int GetPetTargetSelectId();
    virtual bool HasPlayerAction();
    virtual bool HasPetAction();
private:
    CBattleCondition *m_conditions;
    CBattleAction *m_playerAction;
    CBattleTarget *m_playerTarget;
    CBattleAction *m_petAction;
    CBattleTarget *m_petTarget;
};

typedef QSharedPointer<CBattleSetting> CBattleSettingPtr;
typedef QList<CBattleSettingPtr> CBattleSettingList;

typedef struct CGA_BattleUnit_s
{
    CGA_BattleUnit_s()
    {
        exist = false;
        level = 0;
        curhp = 0;
        maxhp = 0;
        curmp = 0;
        maxmp = 0;
        pos = 0;        
    }

    bool exist;
    QString name;
    int level;
    int curhp;
    int maxhp;
    int curmp;
    int maxmp;
    int pos;
}CGA_BattleUnit_t;

typedef CGA_BattleUnit_t CGA_BattleUnitGroup_t[20];

typedef struct CGA_BattleContext_s
{
    CGA_BattleContext_s()
    {
        m_iPlayerStatus = 0;
        m_iPlayerPosition = 0;
        m_iRoundCount = 0;
        m_iLastRound = 0;
        m_iPetPosition = 0;
        m_iEnemyCount = 0;
        m_iTeammateCount = 0;
        m_iFrontCount = 0;
        m_iBackCount = 0;
    }

    bool m_bIsPlayer;
    bool m_bIsDouble;
    bool m_bIsSkillPerformed;
    int m_iPlayerStatus;
    int m_iPlayerPosition;
    int m_iRoundCount;
    int m_iLastRound;
    int m_iPetId;
    int m_iPetPosition;
    int m_iEnemyCount;
    int m_iTeammateCount;
    int m_iFrontCount;
    int m_iBackCount;

    QSharedPointer<CGA_SkillList_t> m_PlayerSkills;
    QSharedPointer<CGA_PetList_t> m_Pets;
    QSharedPointer<CGA_ItemList_t> m_Items;
    CGA_BattleUnitGroup_t m_UnitGroup;
}CGA_BattleContext_t;

class CBattleWorker : public QObject
{
    Q_OBJECT
public:
    CBattleWorker();
private:
    bool CheckLevelOneProtect();
private slots:
    void GetBattleUnits();
    void OnPerformanceBattle();
    void OnNotifyBattleAction(int flags);
    void OnLockCountdown();
public slots:
    void OnNotifyGetSkillsInfo(QSharedPointer<CGA_SkillList_t> skills);
    void OnNotifyGetPetsInfo(QSharedPointer<CGA_PetList_t> pets);
    void OnNotifyGetItemsInfo(QSharedPointer<CGA_ItemList_t> items);
    void OnNotifyAttachProcessOk(quint32 ProcessId, quint32 port, quint32 hWnd);
    void OnSetAutoBattle(int state);
    void OnSetHighSpeed(int state);
    void OnSetFRND(int state);
    void OnSetLv1Protect(int state);
    void OnSetLockCountdown(int state);
    void OnSetShowHPMP(int state);
    void OnSetDelayFrom(int val);
    void OnSetDelayTo(int val);
    void OnSyncList(CBattleSettingList list);
signals:
    void NotifyBattleAction(int flags);
public:
    bool m_bAutoBattle;
    bool m_bHighSpeed;
    bool m_bFirstRoundNoDelay;
    bool m_bLevelOneProtect;
    bool m_bLockCountdown;
    bool m_bShowHPMP;
    int m_iDelayFrom;
    int m_iDelayTo;
private:
    CGA_BattleContext_t m_BattleContext;
    CBattleSettingList m_SettingList;
};

#endif // AUTOBATTLE_H
