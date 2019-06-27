#include "player.h"
#include <QTimer>
extern CGA::CGAInterface *g_CGAInterface;

Q_DECLARE_METATYPE(QSharedPointer<CGA_PlayerInfo_t>)
Q_DECLARE_METATYPE(QSharedPointer<CGA_PetList_t>)
Q_DECLARE_METATYPE(QSharedPointer<CGA_SkillList_t>)
Q_DECLARE_METATYPE(QSharedPointer<CGA_ItemList_t>)
Q_DECLARE_METATYPE(QSharedPointer<CGA_NPCDialog_t>)
Q_DECLARE_METATYPE(QSharedPointer<CGA_MapCellData_t>)
Q_DECLARE_METATYPE(QSharedPointer<CGA_MapUnits_t>)
Q_DECLARE_METATYPE(CItemDropperList)
Q_DECLARE_METATYPE(CItemTweakerList)

CPlayerWorker::CPlayerWorker(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QSharedPointer<CGA_PlayerInfo_t>>("QSharedPointer<CGA_PlayerInfo_t>");
    qRegisterMetaType<QSharedPointer<CGA_PetList_t>>("QSharedPointer<CGA_PetList_t>");
    qRegisterMetaType<QSharedPointer<CGA_SkillList_t>>("QSharedPointer<CGA_SkillList_t>");
    qRegisterMetaType<QSharedPointer<CGA_ItemList_t>>("QSharedPointer<CGA_ItemList_t>");
    qRegisterMetaType<QSharedPointer<CGA_NPCDialog_t>>("QSharedPointer<CGA_NPCDialog_t>");
    qRegisterMetaType<QSharedPointer<CGA_MapCellData_t>>("QSharedPointer<CGA_MapCellData_t>");
    qRegisterMetaType<QSharedPointer<CGA_MapUnits_t>>("QSharedPointer<CGA_MapUnits_t>");
    qRegisterMetaType<CItemDropperList>("CItemDropperList");
    qRegisterMetaType<CItemTweakerList>("CItemTweakerList");

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(OnQueueGetPlayerInfo()));
    timer->start(1000);

    QTimer *timer2 = new QTimer(this);
    connect(timer2, SIGNAL(timeout()), this, SLOT(OnQueueGetItemInfo()));
    timer2->start(500);

    QTimer *timer3 = new QTimer(this);
    connect(timer3, SIGNAL(timeout()), this, SLOT(OnQueueGetMapInfo()));
    timer3->start(100);

    QTimer *timer4 = new QTimer(this);
    connect(timer4, SIGNAL(timeout()), this, SLOT(OnQueueFreqMove()));
    timer4->start(100);

    QTimer *timer5 = new QTimer(this);
    connect(timer5, SIGNAL(timeout()), this, SLOT(OnQueueDownloadMap()));
    timer5->start(500);

    m_bFreqMove = false;
    m_bAutoSupply = false;
    m_NurseNPCId = 0;
    m_MoveSpeed = 100;
    m_WorkAcc = 100;
    m_WorkDelay = 6500;
    m_bNoSwitchAnim = false;
    m_IsDownloadingMap = false;
    m_tabindex = 0;

    const ushort nurse_message[] = {35201,22238,22797,21527,65311,0};

    m_NurseMessage = QString::fromUtf16(nurse_message);

    connect(this, &CPlayerWorker::NotifyNPCDialog, this, &CPlayerWorker::OnNotifyNPCDialog, Qt::QueuedConnection);
}

void CPlayerWorker::OnSyncItemDroppers(CItemDropperList list)
{
    m_ItemDroppers = list;
}

void CPlayerWorker::OnSyncItemTweakers(CItemTweakerList list)
{
    //qDebug("tweaker");
    m_ItemTweakers = list;
}

void CPlayerWorker::OnQueueDropItem(int itempos, int itemid)
{
    bool valid = false;
    CGA::cga_item_info_t info;
    if(g_CGAInterface->IsItemValid(itempos, valid) && valid &&
            g_CGAInterface->GetItemInfo(itempos, info) && (itemid == info.itemid || itemid == -1))
    {
        bool result = false;
        g_CGAInterface->DropItem(itempos, result);
    }
}

void CPlayerWorker::OnTweakDropItem(QSharedPointer<CGA_ItemList_t> items)
{
    for(int i = 0;i < items->size(); ++i)
    {
        const CGA_ItemInfo_t &item = items->at(i);
        for(int j = 0; j < m_ItemDroppers.size(); ++j)
        {
            if(m_ItemDroppers[j]->ShouldDrop(item))
            {
                //Drop it now
                bool result = false;
                if(g_CGAInterface->DropItem(item.pos, result))
                    return;
            }
        }
        for(int j = 0; j < m_ItemTweakers.size(); ++j)
        {
            if(m_ItemTweakers[j]->ShouldTweak(item))
            {
                 for(int k = 0; k < items->size(); ++k)
                {
                    if(k != i && items->at(k).itemid == item.itemid && items->at(k).count < m_ItemTweakers[j]->GetMaxCount())
                    {
                        //tweak it now
                        //qDebug("tweak now %d %d", items->at(k).pos, item.pos);
                        bool result = false;
                        if(g_CGAInterface->MoveItem(items->at(k).pos, item.pos, -1, result))//std::min(m_ItemTweakers[j]->GetMaxCount() - item.count, items->at(k).count)
                            return;
                    }
                }
            }
        }
    }
}

void CPlayerWorker::OnDownloadMap(int xsize, int ysize)
{
    m_DownloadMapX = 0;
    m_DownloadMapY = 0;
    m_DownloadMapXSize = xsize;
    m_DownloadMapYSize = ysize;
    m_IsDownloadingMap = true;
}

void CPlayerWorker::OnQueueDownloadMap()
{
    if(!m_IsDownloadingMap)
        return;

    int ingame = 0;
    bool connected = g_CGAInterface->IsConnected();
    if(!connected || !g_CGAInterface->IsInGame(ingame) || !ingame)
        return;

    g_CGAInterface->RequestDownloadMap(m_DownloadMapX, m_DownloadMapY, m_DownloadMapX+24, m_DownloadMapY+24);

    m_DownloadMapX += 24;

    if(m_DownloadMapX > m_DownloadMapXSize){
        m_DownloadMapX = 0;
        m_DownloadMapY += 24;
    }

    if(m_DownloadMapY > m_DownloadMapYSize){
        m_IsDownloadingMap = false;
    }
}

void CPlayerWorker::OnQueueFreqMove()
{
    int ingame = 0;
    bool connected = g_CGAInterface->IsConnected();
    if(!connected || !g_CGAInterface->IsInGame(ingame) || !ingame)
        return;
    if(m_bFreqMove)
    {
        if(!m_LastFreqMovePosSaved){
            g_CGAInterface->GetMapXY(m_LastFreqMovePos[0], m_LastFreqMovePos[1]);
            m_LastFreqMovePosSaved = true;
        }

        bool bMoved = false;
        int x, y;
        if(g_CGAInterface->GetMapXY(x, y)){
            if(m_LastFreqMovePos[0] > x)
                g_CGAInterface->ForceMove(0, false, bMoved);
            else
                g_CGAInterface->ForceMove(4, false, bMoved);
        }
    }
}

void CPlayerWorker::OnQueueGetMapInfo()
{
    int ingame = 0;
    bool connected = g_CGAInterface->IsConnected();
    if(!connected || !g_CGAInterface->IsInGame(ingame) || !ingame)
    {
        NotifyGetInfoFailed(connected, ingame ? true : false);
        return;
    }

    std::string name;
    float x = 0, y = 0;
    int worldStatus = 0, gameStatus = 0;
    int index1 = 0, index2 = 0, index3 = 0;
    g_CGAInterface->GetMapName(name);
    g_CGAInterface->GetMapXYFloat(x, y);
    g_CGAInterface->GetMapIndex(index1, index2, index3);
    g_CGAInterface->GetWorldStatus(worldStatus);
    g_CGAInterface->GetGameStatus(gameStatus);

    NotifyGetMapInfo(QString::fromStdString(name), index1, index2, index3, (int)(x / 64.0f), (int)(y / 64.0f), worldStatus, gameStatus);
}

void CPlayerWorker::OnQueueGetItemInfo()
{
    int ingame = 0;

    QSharedPointer<CGA_ItemList_t> items(new CGA_ItemList_t);

    if(g_CGAInterface->IsConnected() && g_CGAInterface->IsInGame(ingame) && ingame)
    {
        CGA::cga_items_info_t itemsinfo;
        if(g_CGAInterface->GetItemsInfo(itemsinfo))
        {
            for(size_t i = 0; i < itemsinfo.size(); ++i)
            {
                const CGA::cga_item_info_t &iteminfo = itemsinfo.at(i);
                CGA_ItemInfo_t item;
                item.name = QString::fromStdString(iteminfo.name);
                item.itemid = iteminfo.itemid;
                item.count = iteminfo.count;
                item.pos = iteminfo.pos;
                items->append(item);
            }
        }

        if(!items->empty())
            OnTweakDropItem(items);

        NotifyGetItemsInfo(items);
    }
}

void CPlayerWorker::OnQueueGetPlayerInfo()
{
    int ingame = 0;

    QSharedPointer<CGA_PlayerInfo_t> player(new CGA_PlayerInfo_t);
    QSharedPointer<CGA_PetList_t> pets(new CGA_PetList_t);
    QSharedPointer<CGA_SkillList_t> skills(new CGA_SkillList_t);
    QSharedPointer<CGA_MapCellData_t> mapcollision(new CGA_MapCellData_t);
    QSharedPointer<CGA_MapCellData_t> mapobject(new CGA_MapCellData_t);
    QSharedPointer<CGA_MapUnits_t> mapunits(new CGA_MapUnits_t);

    if(g_CGAInterface->IsConnected() && g_CGAInterface->IsInGame(ingame) && ingame)
    {
        //syncronize value
        g_CGAInterface->SetMoveSpeed(m_MoveSpeed);
        g_CGAInterface->SetWorkDelay(m_WorkDelay);
        g_CGAInterface->SetWorkAcceleration(m_WorkAcc);
        g_CGAInterface->SetNoSwitchAnim(m_bNoSwitchAnim);

        CGA::cga_player_info_t info;
        if(g_CGAInterface->GetPlayerInfo(info))
        {
            player->name = QString::fromStdString(info.name);
            player->job = QString::fromStdString(info.job);
            player->level = info.level;
            player->gold = info.gold;
            player->hp = info.hp;
            player->maxhp = info.maxhp;
            player->mp = info.mp;
            player->maxmp = info.maxmp;
            player->xp = info.xp;
            player->maxxp = info.maxxp;
            player->default_petid = info.petid;
            player->punchclock = info.punchclock;
            player->usingpunchclock = info.usingpunchclock;
        }

        CGA::cga_pets_info_t petsinfo;
        if(g_CGAInterface->GetPetsInfo(petsinfo))
        {
            for(size_t i = 0;i < petsinfo.size(); ++i)
            {
                CGA_PetInfo_t pet;
                const CGA::cga_pet_info_t &petinfo = petsinfo.at(i);
                pet.id = petinfo.index;
                pet.level = petinfo.level;
                pet.hp = petinfo.hp;
                pet.maxhp = petinfo.maxhp;
                pet.mp = petinfo.mp;
                pet.maxmp = petinfo.maxmp;
                pet.xp = petinfo.xp;
                pet.maxxp = petinfo.maxxp;
                pet.flags = petinfo.flags;
                pet.battle_flags = petinfo.battle_flags;
                pet.loyality = petinfo.loyality;
                pet.name = QString::fromStdString(petinfo.name);
                pet.realname = QString::fromStdString(petinfo.realname);
                pet.showname = pet.name.isEmpty() ? pet.realname : pet.name;
                if((int)i == player->default_petid)
                    pet.default_battle = true;

                CGA::cga_pet_skills_info_t skillsinfo;
                if(g_CGAInterface->GetPetSkillsInfo(pet.id, skillsinfo))
                {
                    for(size_t j = 0;j < skillsinfo.size(); ++j)
                    {
                        CGA_PetSkillInfo_t skill;
                        const CGA::cga_pet_skill_info_t &skinfo = skillsinfo.at(j);
                        skill.pos = (int)skinfo.index;
                        skill.name = QString::fromStdString(skinfo.name);
                        skill.info = QString::fromStdString(skinfo.info);
                        skill.cost = skinfo.cost;
                        skill.flags = skinfo.flags;
                        pet.skills.append(skill);
                    }
                }
                pets->push_back(pet);
            }
        }

        CGA::cga_skills_info_t skillsinfo;
        if(g_CGAInterface->GetSkillsInfo(skillsinfo))
        {
            for(size_t i = 0;i < skillsinfo.size(); ++i)
            {
                const CGA::cga_skill_info_t &skinfo = skillsinfo.at(i);
                CGA_SkillInfo_t skill;
                skill.id = skinfo.index;
                skill.name = QString::fromStdString(skinfo.name);
                skill.lv = skinfo.lv;
                skill.maxlv = skinfo.maxlv;
                skill.xp = skinfo.xp;
                skill.maxxp = skinfo.maxxp;
                skill.pos = skinfo.pos;

                CGA::cga_subskills_info_t subskillsinfo;
                if(g_CGAInterface->GetSubSkillsInfo(skill.id, subskillsinfo))
                {
                    for(size_t j = 0;j < subskillsinfo.size(); ++j)
                    {
                        CGA_SubSkill_t subsk;
                        const CGA::cga_subskill_info_t & subskinfo = subskillsinfo.at(j);
                        subsk.name =  QString::fromStdString(subskinfo.name);
                        subsk.level = subskinfo.level;
                        subsk.cost = subskinfo.cost;
                        subsk.flags = subskinfo.flags;
                        subsk.available = subskinfo.available;
                        skill.subskills.append(subsk);
                    }
                }
                skills->append(skill);
            }
        }

        int st;
        if(g_CGAInterface->GetGameStatus(st) && st == 3 && m_tabindex == 5)
        {
            CGA::cga_map_cells_t cells;
            if(g_CGAInterface->GetMapCollisionTable(true, cells))
            {
                mapcollision->xbottom = cells.x_bottom;
                mapcollision->ybottom = cells.y_bottom;
                mapcollision->xsize = cells.x_size;
                mapcollision->ysize = cells.y_size;
                mapcollision->cells = cells.cell;
            }

            if(g_CGAInterface->GetMapObjectTable(true, cells))
            {
                mapobject->xbottom = cells.x_bottom;
                mapobject->ybottom = cells.y_bottom;
                mapobject->xsize = cells.x_size;
                mapobject->ysize = cells.y_size;
                mapobject->cells = cells.cell;
            }

            CGA::cga_map_units_t units;
            if(g_CGAInterface->GetMapUnits(units))
            {
                mapunits->resize((int)units.size());
                for(int i = 0;i < mapunits->size(); ++i){
                    (*mapunits)[i].valid = units[i].valid;
                    (*mapunits)[i].type = units[i].type;
                    (*mapunits)[i].unit_id = units[i].unit_id;
                    (*mapunits)[i].model_id = units[i].model_id;
                    (*mapunits)[i].xpos = units[i].xpos;
                    (*mapunits)[i].ypos = units[i].ypos;
                    (*mapunits)[i].item_count = units[i].item_count;
                    (*mapunits)[i].injury = units[i].injury;
                    (*mapunits)[i].level = units[i].level;
                    (*mapunits)[i].unit_name = QString::fromStdString(units[i].unit_name);
                    (*mapunits)[i].nick_name = QString::fromStdString(units[i].nick_name);
                    (*mapunits)[i].title_name = QString::fromStdString(units[i].title_name);
                    (*mapunits)[i].item_name = QString::fromStdString(units[i].item_name);
                }
            }
        }
    }

    NotifyGetPlayerInfo(player);
    NotifyGetPetsInfo(pets);
    NotifyGetSkillsInfo(skills);
    NotifyGetMapCellInfo(mapcollision, mapobject, mapunits);
}

void CPlayerWorker::NotifyNPCDialogCallback(CGA::cga_npc_dialog_t npcdlg)
{
    QSharedPointer<CGA_NPCDialog_t> dlg(new CGA_NPCDialog_t);

    dlg->type = npcdlg.type;
    dlg->options = npcdlg.options;
    dlg->dialog_id = npcdlg.dialog_id;
    dlg->npc_id = npcdlg.npc_id;
    dlg->message = QString::fromStdString(npcdlg.message);

    NotifyNPCDialog(dlg);
}

bool CPlayerWorker::NeedHPSupply(CGA::cga_player_info_t &pl)
{
    return (pl.hp < pl.maxhp) ? true : false;
}

bool CPlayerWorker::NeedMPSupply(CGA::cga_player_info_t &pl)
{
    return (pl.mp < pl.maxmp) ? true : false;
}

bool CPlayerWorker::NeedPetSupply(CGA::cga_pets_info_t &pets)
{
    for(size_t i = 0;i < pets.size(); ++i)
    {
        if(pets.at(i).hp < pets.at(i).maxhp || pets.at(i).mp < pets.at(i).maxmp)
            return true;
    }
    return false;
}

void CPlayerWorker::OnNotifyNPCDialog(QSharedPointer<CGA_NPCDialog_t> dlg)
{
    if(m_bAutoSupply)
    {
#if 0
        QString s = QString("%1 = {").arg(dlg->message);
        for(int i = 0;i < dlg->message.length(); ++i){
            if(i != 0)
                s += ",";
            s += QString::number((uint)dlg->message.at(i).unicode());
        }
        s += "}";
        qDebug(s.toLatin1().data());
        qDebug("dlg->type=%d", dlg->type);
#endif

        if(dlg->type == 2 && dlg->message.indexOf(m_NurseMessage) >= 0)
        {
            m_NurseNPCId = dlg->npc_id;
            bool result = false;
            CGA::cga_player_info_t playerinfo;
            CGA::cga_pets_info_t petsinfo;
            if(g_CGAInterface->GetPlayerInfo(playerinfo))
            {
                bool bNeedHP = NeedHPSupply(playerinfo);
                bool bNeedMP = NeedMPSupply(playerinfo);
                if(bNeedHP && (!bNeedMP || playerinfo.gold < playerinfo.maxmp-playerinfo.mp))
                {
                    g_CGAInterface->ClickNPCDialog(0, 2, result);
                }
                else if(bNeedMP && playerinfo.gold >= playerinfo.maxmp-playerinfo.mp)
                {
                    g_CGAInterface->ClickNPCDialog(0, 0, result);
                }
            }
            if(!result && g_CGAInterface->GetPetsInfo(petsinfo))
            {
                if(NeedPetSupply(petsinfo))
                {
                    g_CGAInterface->ClickNPCDialog(0, 4, result);
                }
            }
        }
        else if (dlg->npc_id == m_NurseNPCId && dlg->type == 0)
        {
            if (dlg->options == 12)
            {
                bool result = false;
                g_CGAInterface->ClickNPCDialog(4, -1, result);
            }
            if (dlg->options == 1)
            {
                bool result = false;
                g_CGAInterface->ClickNPCDialog(1, -1, result);
            }
        }
    }
}

void CPlayerWorker::OnSetNoSwitchAnim(int state)
{
    m_bNoSwitchAnim = state ? true : false;
}

void CPlayerWorker::OnSetAutoSupply(int state)
{
    m_bAutoSupply = state ? true : false;
}

void CPlayerWorker::OnSetFreqMove(int state)
{
    m_bFreqMove = state ? true : false;
    m_LastFreqMovePosSaved = false;
}

void CPlayerWorker::OnSetMoveSpeed(int value)
{
    m_MoveSpeed = value;
}

void CPlayerWorker::OnSetWorkDelay(int value)
{
    m_WorkDelay = value;
}

void CPlayerWorker::OnSetWorkAcc(int value)
{
    m_WorkAcc = value;
}

void CPlayerWorker::OnNotifyAttachProcessOk(quint32 ProcessId, quint32 port, quint32 hWnd)
{
    g_CGAInterface->RegisterNPCDialogNotify(std::bind(&CPlayerWorker::NotifyNPCDialogCallback, this, std::placeholders::_1));
}

void CPlayerWorker::OnTabChanged(int tabindex)
{
    m_tabindex = tabindex;
}
