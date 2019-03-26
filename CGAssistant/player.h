#ifndef PLAYER_H
#define PLAYER_H

#include <QList>
#include <QVector>
#include <QString>
#include <QSharedPointer>
#include "../CGALib/gameinterface.h"

typedef struct CGA_PlayerInfo_s
{
    CGA_PlayerInfo_s()
    {
        level = 0;
        gold = 0;
        hp = 0;
        maxhp = 0;
        mp = 0;
        maxmp = 0;
        xp = 0;
        maxxp = 0;
        punchclock = 0;
        usingpunchclock = false;
        default_petid = -1;
    }

    QString name;
    QString job;
    int level;
    int gold;
    int hp, maxhp;
    int mp, maxmp;
    int xp, maxxp;
    int default_petid;
    int punchclock;
    bool usingpunchclock;
}CGA_PlayerInfo_t;

typedef struct CGA_PetSkillInfo_s
{
    QString name;
    QString info;
    int cost;
    int flags;
    int pos;
}CGA_PetSkillInfo_t;

typedef QVector<CGA_PetSkillInfo_t> CGA_PetSkillList_t;

typedef struct CGA_PetInfo_s
{
    CGA_PetInfo_s()
    {
        id = -1;
        level = 0;
        hp = 0;
        maxhp = 0;
        mp = 0;
        maxmp = 0;
        xp = 0;
        maxxp = 0;
        flags = 0;
        battle_flags = 0;
        default_battle = false;
    }

    int id;
    int level;
    int hp, maxhp;
    int mp, maxmp;
    int xp, maxxp;
    int flags, battle_flags;
    bool default_battle;
    QString name, realname, showname;
    CGA_PetSkillList_t skills;
}CGA_PetInfo_t;

typedef struct CGA_SubSkill_s
{
    CGA_SubSkill_s()
    {
        level = 0;
        cost = 0;
        flags = 0;
    }
    int level, cost, flags;
    QString name;
}CGA_SubSkill_t;

typedef QVector<CGA_SubSkill_t> CGA_SubSkills_t;

typedef struct CGA_SkillInfo_s
{
    CGA_SkillInfo_s()
    {
        id = 0;
        pos = 0;
        lv = 0;
        maxlv = 0;
        xp = 0;
        maxxp = 0;
    }

    int id;
    int pos;
    int lv, maxlv;
    int xp, maxxp;
    QString name;
    CGA_SubSkills_t subskills;
}CGA_SkillInfo_t;

typedef QList<CGA_PetInfo_t> CGA_PetList_t;
typedef QList<CGA_SkillInfo_t> CGA_SkillList_t;

typedef struct CGA_ItemInfo_s
{
    CGA_ItemInfo_s()
    {
        itemid = -1;
        count = -1;
        pos = -1;
    }
    int itemid;
    int count;
    int pos;
    QString name;
}CGA_ItemInfo_t;

typedef QList<CGA_ItemInfo_t> CGA_ItemList_t;

class CItemDropper
{
public:
    virtual bool ShouldDrop(const CGA_ItemInfo_t &item) = 0;
    virtual void GetName(QString &str) = 0;
};

typedef QSharedPointer<CItemDropper> CItemDropperPtr;
typedef QList<CItemDropperPtr> CItemDropperList;

typedef struct CGA_NPCDialog_s
{
    CGA_NPCDialog_s()
    {
        type = 0;
        options = 0;
        dialog_id = 0;
        npc_id = 0;
    }
    int type;
    int options;
    int dialog_id;
    int npc_id;
    QString message;
}CGA_NPCDialog_t;

class CPlayerWorker : public QObject
{
    Q_OBJECT
public:
    explicit CPlayerWorker(QObject *parent = NULL);

private:
    void NotifyNPCDialogCallback(CGA::cga_npc_dialog_t dlg);
    bool NeedHPSupply(CGA::cga_player_info_t &pl);
    bool NeedMPSupply(CGA::cga_player_info_t &pl);
    bool NeedPetSupply(CGA::cga_pets_info_t &pets);
private slots:
    void OnTweakDropItem(QSharedPointer<CGA_ItemList_t> items);

public slots:
    void OnQueueGetPlayerInfo();
    void OnQueueGetItemInfo();
    void OnQueueGetMapInfo();
    void OnQueueFreqMove();
    void OnSyncItemDroppers(CItemDropperList list);
    void OnQueueDropItem(int itempos, int itemid);
    void OnNotifyAttachProcessOk(quint32 ProcessId, quint32 port, quint32 hWnd);
    void OnNotifyNPCDialog(QSharedPointer<CGA_NPCDialog_t> dlg);
    void OnSetAutoSupply(int state);
    void OnSetMoveSpeed(int value);
    void OnSetWorkDelay(int value);
    void OnSetFreqMove(int state);
    void OnSetWorkAcc(int value);
signals:
    void NotifyGetInfoFailed(bool bIsConnected, bool bIsInGame);
    void NotifyGetPlayerInfo(QSharedPointer<CGA_PlayerInfo_t> player);
    void NotifyGetPetsInfo(QSharedPointer<CGA_PetList_t> pets);
    void NotifyGetSkillsInfo(QSharedPointer<CGA_SkillList_t> skills);
    void NotifyGetItemsInfo(QSharedPointer<CGA_ItemList_t> items);
    void NotifyGetMapInfo(QString name, int x, int y, int gameStatus, int battleStatus);
    void NotifyNPCDialog(QSharedPointer<CGA_NPCDialog_t> dlg);
private:
    CItemDropperList m_ItemDroppers;
    QString m_NurseMessage;
    int m_NurseNPCId;
    bool m_bFreqMove;
    bool m_LastFreqMovePosSaved;
    int m_LastFreqMovePos[2];
    bool m_bAutoSupply;
    int m_MoveSpeed;
    int m_WorkDelay;
    int m_WorkAcc;
};

#endif // PLAYER_H
