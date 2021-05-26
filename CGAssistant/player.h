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
        unitid = 0;
        usingpunchclock = false;
        default_petid = -1;
        serverindex = -1;
    }

    QString name;
    QString job;
    int level;
    int gold;
    int hp, maxhp;
    int mp, maxmp;
    int xp, maxxp;
    int default_petid;
    int unitid;
    int punchclock;
    bool usingpunchclock;
    int serverindex;
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
        loyality = 0;
        default_battle = false;
    }

    int id;
    int level;
    int hp, maxhp;
    int mp, maxmp;
    int xp, maxxp;
    int flags, battle_flags;
    int loyality;
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
        available = false;
    }
    int level, cost, flags;
    bool available;
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
        type = -1;
        count = -1;
        pos = -1;
        assessed = false;
    }
    int itemid;
    int type;
    int count;
    int pos;
    bool assessed;
    QString name;
    QString attr;
    QString info;
}CGA_ItemInfo_t;

typedef QList<CGA_ItemInfo_t> CGA_ItemList_t;

class CItemDropper
{
public:
    virtual ~CItemDropper(){

    }
    virtual bool ShouldDrop(const CGA_ItemInfo_t &item) = 0;
    virtual void GetName(QString &str) = 0;
};

class CItemTweaker
{
public:
    virtual ~CItemTweaker(){

    }
    virtual bool ShouldTweak(const CGA_ItemInfo_t &item) = 0;
    virtual void GetName(QString &str) = 0;
    virtual int GetMaxCount() { return m_maxcount; }
protected:
    int m_maxcount;
};

typedef QSharedPointer<CItemDropper> CItemDropperPtr;
typedef QList<CItemDropperPtr> CItemDropperList;

typedef QSharedPointer<CItemTweaker> CItemTweakerPtr;
typedef QList<CItemTweakerPtr> CItemTweakerList;

typedef struct CGA_MapCellData_s
{
    CGA_MapCellData_s()
    {
        xbottom = 0;
        ybottom = 0;
        xsize = 0;
        ysize = 0;
    }
    int xbottom;
    int ybottom;
    int xsize;
    int ysize;
    std::vector<short> cells;
    int mapindex;
}CGA_MapCellData_t;

typedef struct CGA_MapUnit_s
{
    CGA_MapUnit_s()
    {
        valid = 0;
        type = 0;
        unit_id = 0;
        model_id = 0;
        xpos = 0;
        ypos = 0;
        item_count = 0;
        injury = 0;
        level = 0;
        flags = 0;
    }
    int valid;
    int type;
    int unit_id;
    int model_id;
    int xpos;
    int ypos;
    int item_count;
    int injury;
    int level;
    int flags;
    QString unit_name;
    QString nick_name;
    QString title_name;
    QString item_name;
}CGA_MapUnit_t;

typedef QVector<CGA_MapUnit_t> CGA_MapUnits_t;

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
    void NotifyPlayerMenuCallback(CGA::cga_player_menu_items_t items);
    void NotifyUnitMenuCallback(CGA::cga_unit_menu_items_t items);
    void NotifyNPCDialogCallback(CGA::cga_npc_dialog_t dlg);
    void NotifyDownloadMapCallback(CGA::cga_download_map_t down);
    void NotifyChatMsgCallback(CGA::cga_chat_msg_t msg);
    void NotifyConnectionStateCallback(CGA::cga_conn_state_t msg);

    bool NeedHPSupply(CGA::cga_player_info_t &pl);
    bool NeedMPSupply(CGA::cga_player_info_t &pl);
    bool NeedPetSupply(CGA::cga_pets_info_t &pets);
private:
    bool OnTweakDropItem(QSharedPointer<CGA_ItemList_t> items);
    bool OnEatItem(QSharedPointer<CGA_ItemList_t> items);

public slots:
    void OnQueueGetPlayerInfo();
    void OnQueueGetItemInfo();
    void OnQueueGetMapInfo();
    void OnQueueDownloadMap();
    void OnSyncItemDroppers(CItemDropperList list);
    void OnSyncItemTweakers(CItemTweakerList list);
    void OnQueueDropItem(int itempos, int itemid);
    void OnNotifyAttachProcessOk(quint32 ProcessId, quint32 ThreadId, quint32 port, quint32 hWnd);
    void OnNotifyNPCDialog(QSharedPointer<CGA_NPCDialog_t> dlg);
    void OnNotifyPlayerMenu(QSharedPointer<CGA::cga_player_menu_items_t> menu);
    void OnNotifyUnitMenu(QSharedPointer<CGA::cga_unit_menu_items_t> menu);
    void OnSetAutoSupply(int state);
    void OnSetMoveSpeed(int value);
    void OnSetWorkDelay(int value);
    void OnSetUseFood(int state);
    void OnSetUseMed(int state);
    void OnSetPetFood(int state);
    void OnSetPetMed(int state);
    void OnSetWorkAcc(int value);
    void OnSetNoSwitchAnim(int state);
    void OnSetShowHPMP(int state);
    void OnDownloadMap(int xsize, int ysize);
    void OnTabChanged(int tabindex);
    void OnSetUseFoodAt(QString str);
    void OnSetUseMedAt(QString str);
    void OnSetPetFoodAt(QString str);
    void OnSetPetMedAt(QString str);
signals:
    void NotifyGetInfoFailed(bool bIsConnected, bool bIsInGame);
    void NotifyGetPlayerInfo(QSharedPointer<CGA_PlayerInfo_t> player);
    void NotifyGetPetsInfo(QSharedPointer<CGA_PetList_t> pets);
    void NotifyGetSkillsInfo(QSharedPointer<CGA_SkillList_t> skills);
    void NotifyGetItemsInfo(QSharedPointer<CGA_ItemList_t> items);
    void NotifyGetMapCellInfo(QSharedPointer<CGA_MapCellData_t> coll, QSharedPointer<CGA_MapCellData_t> obj, QSharedPointer<CGA_MapUnits_t> units);
    void NotifyGetMapInfo(QString name, int index1, int index2, int index3, int x, int y, int worldstatus, int gamestatus);
    void NotifyNPCDialog(QSharedPointer<CGA_NPCDialog_t> dlg);
    void NotifyPlayerMenu(QSharedPointer<CGA::cga_player_menu_items_t> menu);
    void NotifyUnitMenu(QSharedPointer<CGA::cga_unit_menu_items_t> menu);
    void NotifyRefreshMapRegion(int xbase, int ybase, int xtop, int ytop, int index3);
    void NotifyChatMsg(int unitid, QString msg, int size, int color);
    void NotifyConnectionState(int state, QString msg);
private:
    CItemDropperList m_ItemDroppers;
    CItemTweakerList m_ItemTweakers;
    QString m_NurseMessage;
    int m_NurseNPCId;
    bool m_bUseFood;
    bool m_bUseMed;
    bool m_bPetFood;
    bool m_bPetMed;
    bool m_UseFoodAtPercentage;
    bool m_UseMedAtPercentage;
    bool m_PetFoodAtPercentage;
    bool m_PetMedAtPercentage;
    int m_UseFoodAt;
    int m_UseMedAt;
    int m_PetFoodAt;
    int m_PetMedAt;
    bool m_LastFreqMovePosSaved;
    int m_LastFreqMovePos[2];
    bool m_bAutoSupply;
    int m_MoveSpeed;
    int m_WorkDelay;
    int m_WorkAcc;
    bool m_bNoSwitchAnim;
    bool m_bHighSpeedBattle;
    bool m_bShowHPMP;
    QSharedPointer<CGA_PlayerInfo_t> m_player;
    QSharedPointer<CGA_PetList_t> m_pets;

    int m_DownloadMapX;
    int m_DownloadMapY;
    int m_DownloadMapXSize;
    int m_DownloadMapYSize;
    bool m_IsDownloadingMap;
    int m_tabindex;

    uint64_t m_LastUseItem;
    int m_LastUseItemTargetHP;
    int m_LastUseItemTargetMaxHP;
    int m_LastUseItemTargetMP;
    int m_LastUseItemTargetMaxMP;
    uint64_t m_LastUseItemPlayerSelect;
};

#endif // PLAYER_H
