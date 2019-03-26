#include <QTimer>
#include <QTime>
#include "battle.h"

extern CGA::CGAInterface *g_CGAInterface;
extern QString s_BattleCondType[BattleCond_Type_Max];
extern QString s_BattleCondRelationNumber[BattleCond_NumRel_Max];
extern QString s_BattleCondRelationString[BattleCond_StrRel_Max];
extern QString s_BattleTargetString[BattleTarget_Max];
extern QString s_BattleTargetSelectString[BattleTarget_Select_Max];

static int GetPetPosition(int playerPos)
{
    if(playerPos >= 0 && playerPos <= 4)
        return playerPos + 5;
    if(playerPos >= 5 && playerPos <= 9)
        return playerPos - 5;
    return -1;
}

CBattleCondition_EnemyCount::CBattleCondition_EnemyCount(int relation, int value)
{
    m_relation = relation;
    m_value = value;
}

void CBattleCondition_EnemyCount::GetConditionName(QString &str)
{
    str = QObject::tr("EnemyCount %1%2").arg(s_BattleCondRelationNumber[m_relation], QString::number(m_value));
}

bool CBattleCondition_EnemyCount::Check(CGA_BattleContext_t &context)
{
    switch (m_relation)
    {
    case BattleCond_NumRel_EGT:
        return context.m_iEnemyCount >= m_value;
    case BattleCond_NumRel_GT:
        return context.m_iEnemyCount > m_value;
    case BattleCond_NumRel_ELT:
        return context.m_iEnemyCount <= m_value;
    case BattleCond_NumRel_LT:
        return context.m_iEnemyCount < m_value;
    case BattleCond_NumRel_EQ:
        return context.m_iEnemyCount == m_value;
    case BattleCond_NumRel_NEQ:
        return context.m_iEnemyCount != m_value;
    }

    return false;
}

CBattleCondition_PlayerHp::CBattleCondition_PlayerHp(int relation, int value, bool percentage)
{
    m_relation = relation;
    m_value = value;
    m_percentage = percentage;
}

void CBattleCondition_PlayerHp::GetConditionName(QString &str)
{
    str = QObject::tr("PlayerHP %1%2").arg(s_BattleCondRelationNumber[m_relation], QString::number(m_value));
    if(m_percentage)
        str += QLatin1String("%");
}
bool CBattleCondition_PlayerHp::Check(CGA_BattleContext_t &context)
{
    if(context.m_iPlayerPosition < 0 || context.m_iPlayerPosition > 20)
        return false;
    int curv = context.m_UnitGroup[context.m_iPlayerPosition].curhp;
    int maxv = context.m_UnitGroup[context.m_iPlayerPosition].maxhp;
    if(m_percentage){
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            return curv * 100 / maxv >= m_value;
        case BattleCond_NumRel_GT:
            return curv * 100 / maxv > m_value;
        case BattleCond_NumRel_ELT:
            return curv * 100 / maxv <= m_value;
        case BattleCond_NumRel_LT:
            return curv * 100 / maxv < m_value;
        case BattleCond_NumRel_EQ:
            return curv * 100 / maxv == m_value;
        case BattleCond_NumRel_NEQ:
            return curv * 100 / maxv != m_value;
        }
    } else {
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            return curv >= m_value;
        case BattleCond_NumRel_GT:
            return curv > m_value;
        case BattleCond_NumRel_ELT:
            return curv <= m_value;
        case BattleCond_NumRel_LT:
            return curv < m_value;
        case BattleCond_NumRel_EQ:
            return curv == m_value;
        case BattleCond_NumRel_NEQ:
            return curv != m_value;
        }
    }

    return false;
}

CBattleCondition_PlayerMp::CBattleCondition_PlayerMp(int relation, int value, bool percentage)
{
    m_relation = relation;
    m_value = value;
    m_percentage = percentage;
}
void CBattleCondition_PlayerMp::GetConditionName(QString &str)
{
    str = QObject::tr("PlayerMP %1%2").arg(s_BattleCondRelationNumber[m_relation], QString::number(m_value));
    if(m_percentage)
        str += QLatin1String("%");
}
bool CBattleCondition_PlayerMp::Check(CGA_BattleContext_t &context)
{
    if(context.m_iPlayerPosition < 0 || context.m_iPlayerPosition > 20)
        return false;
    int curv = context.m_UnitGroup[context.m_iPlayerPosition].curmp;
    int maxv = context.m_UnitGroup[context.m_iPlayerPosition].maxmp;
    if(m_percentage){
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            return curv * 100 / maxv >= m_value;
        case BattleCond_NumRel_GT:
            return curv * 100 / maxv > m_value;
        case BattleCond_NumRel_ELT:
            return curv * 100 / maxv <= m_value;
        case BattleCond_NumRel_LT:
            return curv * 100 / maxv < m_value;
        case BattleCond_NumRel_EQ:
            return curv * 100 / maxv == m_value;
        case BattleCond_NumRel_NEQ:
            return curv * 100 / maxv != m_value;
        }
    } else {
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            return curv >= m_value;
        case BattleCond_NumRel_GT:
            return curv > m_value;
        case BattleCond_NumRel_ELT:
            return curv <= m_value;
        case BattleCond_NumRel_LT:
            return curv < m_value;
        case BattleCond_NumRel_EQ:
            return curv == m_value;
        case BattleCond_NumRel_NEQ:
            return curv != m_value;
        }
    }
    return false;
}

CBattleCondition_PetHp::CBattleCondition_PetHp(int relation, int value, bool percentage)
{
    m_relation = relation;
    m_value = value;
    m_percentage = percentage;
}

void CBattleCondition_PetHp::GetConditionName(QString &str)
{
    str = QObject::tr("PetHP %1%2").arg(s_BattleCondRelationNumber[m_relation], QString::number(m_value));
    if(m_percentage)
        str += QLatin1String("%");
}

bool CBattleCondition_PetHp::Check(CGA_BattleContext_t &context)
{
    if(context.m_iPetPosition < 0 || context.m_iPetPosition > 20)
        return false;
    int curv = context.m_UnitGroup[context.m_iPetPosition].curhp;
    int maxv = context.m_UnitGroup[context.m_iPetPosition].maxhp;
    if(m_percentage){
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            return curv * 100 / maxv >= m_value;
        case BattleCond_NumRel_GT:
            return curv * 100 / maxv > m_value;
        case BattleCond_NumRel_ELT:
            return curv * 100 / maxv <= m_value;
        case BattleCond_NumRel_LT:
            return curv * 100 / maxv < m_value;
        case BattleCond_NumRel_EQ:
            return curv * 100 / maxv == m_value;
        case BattleCond_NumRel_NEQ:
            return curv * 100 / maxv != m_value;
        }
    } else {
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            return curv >= m_value;
        case BattleCond_NumRel_GT:
            return curv > m_value;
        case BattleCond_NumRel_ELT:
            return curv <= m_value;
        case BattleCond_NumRel_LT:
            return curv < m_value;
        case BattleCond_NumRel_EQ:
            return curv == m_value;
        case BattleCond_NumRel_NEQ:
            return curv != m_value;
        }
    }
    return false;
}

CBattleCondition_PetMp::CBattleCondition_PetMp(int relation, int value, bool percentage)
{
    m_relation = relation;
    m_value = value;
    m_percentage = percentage;
}
void CBattleCondition_PetMp::GetConditionName(QString &str)
{
    str = QObject::tr("PetMP %1%2").arg(s_BattleCondRelationNumber[m_relation], QString::number(m_value));
    if(m_percentage)
        str += QLatin1String("%");
}
bool CBattleCondition_PetMp::Check(CGA_BattleContext_t &context)
{
    if(context.m_iPetPosition < 0 || context.m_iPetPosition > 20)
        return false;
    int curv = context.m_UnitGroup[context.m_iPetPosition].curmp;
    int maxv = context.m_UnitGroup[context.m_iPetPosition].maxmp;
    if(m_percentage){
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            return curv * 100 / maxv >= m_value;
        case BattleCond_NumRel_GT:
            return curv * 100 / maxv > m_value;
        case BattleCond_NumRel_ELT:
            return curv * 100 / maxv <= m_value;
        case BattleCond_NumRel_LT:
            return curv * 100 / maxv < m_value;
        case BattleCond_NumRel_EQ:
            return curv * 100 / maxv == m_value;
        case BattleCond_NumRel_NEQ:
            return curv * 100 / maxv != m_value;
        }
    } else {
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            return curv >= m_value;
        case BattleCond_NumRel_GT:
            return curv > m_value;
        case BattleCond_NumRel_ELT:
            return curv <= m_value;
        case BattleCond_NumRel_LT:
            return curv < m_value;
        case BattleCond_NumRel_EQ:
            return curv == m_value;
        case BattleCond_NumRel_NEQ:
            return curv != m_value;
        }
    }
    return false;
}

CBattleCondition_TeammateHp::CBattleCondition_TeammateHp(int relation, int value, bool percentage)
{
    m_relation = relation;
    m_value = value;
    m_percentage = percentage;
}
void CBattleCondition_TeammateHp::GetConditionName(QString &str)
{
    str = QObject::tr("TeammateHP %1%2").arg(s_BattleCondRelationNumber[m_relation], QString::number(m_value));
    if(m_percentage)
        str += QLatin1String("%");
}
bool CBattleCondition_TeammateHp::Check(CGA_BattleContext_t &context)
{
    for(int i = 0;i < 0xA; ++i)
    {
        if(!context.m_UnitGroup[i].exist)
            continue;
        int curv = context.m_UnitGroup[i].curhp;
        int maxv = context.m_UnitGroup[i].maxhp;
        if(m_percentage){
            switch (m_relation)
            {
            case BattleCond_NumRel_EGT:
                if(curv * 100 / maxv >= m_value)
                    return true;
            case BattleCond_NumRel_GT:
                if(curv * 100 / maxv > m_value)
                    return true;
            case BattleCond_NumRel_ELT:
                if(curv * 100 / maxv <= m_value)
                    return true;;
            case BattleCond_NumRel_LT:
                if(curv * 100 / maxv < m_value)
                    return true;
            case BattleCond_NumRel_EQ:
                if(curv * 100 / maxv == m_value)
                    return true;
            case BattleCond_NumRel_NEQ:
                if(curv * 100 / maxv != m_value)
                    return true;
            }
        }
        else
        {
            switch (m_relation)
            {
            case BattleCond_NumRel_EGT:
                if( curv >= m_value)
                    return true;
            case BattleCond_NumRel_GT:
                if( curv > m_value)
                    return true;
            case BattleCond_NumRel_ELT:
                if( curv <= m_value)
                    return true;
            case BattleCond_NumRel_LT:
                if( curv < m_value)
                    return true;
            case BattleCond_NumRel_EQ:
                if( curv == m_value)
                    return true;
            case BattleCond_NumRel_NEQ:
                if( curv != m_value)
                    return true;
            }
        }
    }
    return false;
}

CBattleCondition_EnemyUnit::CBattleCondition_EnemyUnit(int relation, QString &unitName)
{
    m_relation = relation;
    m_UnitName = unitName;
}

void CBattleCondition_EnemyUnit::GetConditionName(QString &str)
{
    str = QObject::tr("Enemy %1%2").arg( s_BattleCondRelationString[m_relation], m_UnitName);
}
bool CBattleCondition_EnemyUnit::Check(CGA_BattleContext_t &context)
{
    for(int i = 0xA;i < 20; ++i)
    {
        if(!context.m_UnitGroup[i].exist)
            continue;
        if(context.m_UnitGroup[i].name == m_UnitName){
            if(m_relation == BattleCond_StrRel_CONTAIN)
                return true;
        }
    }
    return (m_relation == BattleCond_StrRel_NOT_CONTAIN) ? true : false;
}


void CBattleAction_PlayerAttack::GetActionName(QString &str)
{
    str = QObject::tr("Attack");
}
bool CBattleAction_PlayerAttack::DoAction(int target, CGA_BattleContext_t &context)
{
    bool result = false;
    g_CGAInterface->BattleNormalAttack(target, result);
    return result;
}

void CBattleAction_PlayerDefense::GetActionName(QString &str)
{
    str = QObject::tr("Defense");
}
bool CBattleAction_PlayerDefense::DoAction(int target, CGA_BattleContext_t &context)
{
    bool result = false;
    g_CGAInterface->BattleDefense(result);
    return result;
}

void CBattleAction_PlayerEscape::GetActionName(QString &str)
{
    str = QObject::tr("Escape");
}

bool CBattleAction_PlayerEscape::DoAction(int target, CGA_BattleContext_t &context)
{
    bool result = false;
    g_CGAInterface->BattleEscape(result);
    return result;
}

void CBattleAction_PlayerExchangePosition::GetActionName(QString &str)
{
    str = QObject::tr("Exchange Position");
}

bool CBattleAction_PlayerExchangePosition::DoAction(int target, CGA_BattleContext_t &context)
{
    bool result = false;
    g_CGAInterface->BattleExchangePosition(result);
    return result;
}

CBattleAction_PlayerChangePet::CBattleAction_PlayerChangePet(int type)
{
    m_Type = type;
}

CBattleAction_PlayerChangePet::CBattleAction_PlayerChangePet(QString &petname)
{
    m_Type = 0;
    m_PetName = petname;
}

void CBattleAction_PlayerChangePet::GetActionName(QString &str)
{
    QString type;
    switch(m_Type){
    case 0:
        str = QObject::tr("Call pet %1").arg(m_PetName);
        break;
    case 1:
        str = QObject::tr("Recall pet");
        break;
    case 2:
        str = QObject::tr("Call pet with highest level");
        break;
    case 3:
        str = QObject::tr("Call pet with highest health");
        break;
    }
}

bool CBattleAction_PlayerChangePet::DoAction(int target, CGA_BattleContext_t &context)
{
    int petid;
    bool found = false;
    switch(m_Type)
    {
    case 0:
    {
        for(int i = 0 ;i < context.m_Pets->size(); ++i ){
            const CGA_PetInfo_t &pet = context.m_Pets->at(i);
            if(pet.id == context.m_iPetId)
                continue;
            if(pet.hp == 0)
                continue;
            if((pet.battle_flags & 3) == 0)
                continue;
            if(pet.showname == m_PetName){
                petid = pet.id;
                found = true;
                break;
            }
        }
        break;
    }
    case 1:
    {
        petid = -1;
        found = true;
        break;
    }
    case 2:
    {
        int highest = 0;
        for(int i = 0 ;i < context.m_Pets->size(); ++i ){
            const CGA_PetInfo_t &pet = context.m_Pets->at(i);
            if(pet.id == context.m_iPetId)
                continue;
            if(pet.hp == 0)
                continue;
            if((pet.battle_flags & 3) == 0)
                continue;
            if(pet.level > highest){
                highest = pet.level;
                petid = pet.id;
                found = true;
            }
        }
        break;
    }
    case 3:
    {
        int highest = 0;
        for(int i = 0 ;i < context.m_Pets->size(); ++i ){
            const CGA_PetInfo_t &pet = context.m_Pets->at(i);
            if(pet.id == context.m_iPetId)
                continue;
            if(pet.hp == 0)
                continue;
            if((pet.battle_flags & 3) == 0)
                continue;
            if(pet.level > highest){
                highest = pet.level;
                petid = pet.id;
                found = true;
            }
        }
        break;
    }
    }

    bool result = false;
    if(found)
    {
        g_CGAInterface->BattleChangePet(petid, result);
    }
    return result;
}

CBattleAction_PlayerSkillAttack::CBattleAction_PlayerSkillAttack(QString &skillName, int skillLevel)
{
    m_SkillName = skillName;
    m_SkillLevel = skillLevel;
}

void CBattleAction_PlayerSkillAttack::GetActionName(QString &str)
{
    str = m_SkillName;
    if(m_SkillLevel >= 1 && m_SkillLevel <= 10)
        str += QObject::tr(" Lv %1").arg(QString::number(m_SkillLevel));
    else
        str += QObject::tr(" Lv Max");
}

bool CBattleAction_PlayerSkillAttack::DoAction(int target, CGA_BattleContext_t &context)
{
    int skill_pos, skill_level;

    bool result = false;
    if(context.m_bIsSkillPerformed == false && GetSkill(context, skill_pos, skill_level)){
        FixTarget(context, skill_pos, skill_level, target);
        g_CGAInterface->BattleSkillAttack(skill_pos, skill_level, target, result);
        //qDebug("BattleSkillAttack %d %d %d", skill_pos, skill_level, target);
    }

    if(!result){
        g_CGAInterface->BattleNormalAttack(target, result);
        //qDebug("BattleNormalAttack %d %d", skill_pos, target);
    }
    return result;
}

bool CBattleAction_PlayerSkillAttack::GetSkill(CGA_BattleContext_t &context, int &skillpos, int &skilllevel)
{
    if(!context.m_PlayerSkills.data())
        return false;
    for(int i = 0;i < context.m_PlayerSkills->size(); ++i)
    {
        const CGA_SkillInfo_t &skill = context.m_PlayerSkills->at(i);
        if(m_SkillName == skill.name)
        {
            skillpos = i;
            skilllevel = skill.lv;
            if(m_SkillLevel >= 1 && skilllevel > m_SkillLevel)
                skilllevel = m_SkillLevel;
            int playerMp = context.m_UnitGroup[context.m_iPlayerPosition].curmp;
            const CGA_SubSkills_t &subsks = skill.subskills;
            for(int j = subsks.size() - 1; j >= 0; --j){
                if(skilllevel >= subsks.at(j).level && playerMp >= subsks.at(j).cost){
                    skilllevel = j;
                    //qDebug("j = %d", skilllevel);
                    return true;
                }
            }
            return false;
        }
    }

    return false;
}

void CBattleAction_PlayerSkillAttack::FixTarget(CGA_BattleContext_t &context, int skill_pos, int skill_level, int &target)
{
    if(!context.m_PlayerSkills.data())
        return;

    const CGA_SkillInfo_t &skill = context.m_PlayerSkills->at(skill_pos);
    int flags = skill.subskills.at(skill_level).flags;

    if(flags & FL_SKILL_SINGLE)
        return;
    if(flags & FL_SKILL_MULTI){
        target = target + 20;
        return;
    }
    if(flags & FL_SKILL_ALL){
        target = (target >= 10 && target <= 19) ? 41 : 40;
        return;
    }
    if(flags & FL_SKILL_BOOM){
        target = 42;
        return;
    }
    target = -1;
}

CBattleAction_PlayerUseItem::CBattleAction_PlayerUseItem(QString &itemName)
{
    m_ItemName = itemName;
    m_ItemId = 0;
    if(itemName[0] == '#')
    {
        bool bValue = false;
        int value = itemName.mid(1).toInt(&bValue);
        if(bValue)
            m_ItemId = value;
    }
}

CBattleAction_PlayerUseItem::CBattleAction_PlayerUseItem(int itemId)
{
    m_ItemId = itemId;
}

void CBattleAction_PlayerUseItem::GetActionName(QString &str)
{
    str = QObject::tr("Use item %1").arg(m_ItemName);
}

bool CBattleAction_PlayerUseItem::DoAction(int target, CGA_BattleContext_t &context)
{
    //TODO
    int itempos;;
    bool result = false;
    if(GetItemPosition(context, itempos))
        g_CGAInterface->BattleUseItem(itempos, target, result);
    return result;
}

bool CBattleAction_PlayerUseItem::GetItemPosition(CGA_BattleContext_t &context, int &itempos)
{
    if(!context.m_Items.data())
        return false;

    for(int i = 0;i < context.m_Items->size(); ++i)
    {
        const CGA_ItemInfo_t &item = context.m_Items->at(i);
        if(item.name == m_ItemName || (m_ItemId > 0 && item.itemid == m_ItemId)){
            itempos = item.pos;
            return true;
        }
    }

    return false;
}

CBattleAction_PetSkillAttack::CBattleAction_PetSkillAttack(QString &skillName)
{
    m_SkillName = skillName;
}

void CBattleAction_PetSkillAttack::GetActionName(QString &str)
{
    str = m_SkillName;
}

bool CBattleAction_PetSkillAttack::DoAction(int target, CGA_BattleContext_t &context)
{
    int skillpos = -1;

    bool result = false;
    if(GetSkill(context, skillpos))
        g_CGAInterface->BattlePetSkillAttack(skillpos, target, result);

    qDebug("BattlePetSkillAttack %d %d\n", skillpos, target);

    return result;
}

bool CBattleAction_PetSkillAttack::GetSkill(CGA_BattleContext_t &context, int &skillpos)
{
    if(context.m_iPetId == -1)
        return false;

    if(!context.m_Pets.data())
        return false;

    for(int i = 0 ;i < context.m_Pets->size(); ++i ){
        const CGA_PetInfo_t &pet = context.m_Pets->at(i);
        if(pet.id == context.m_iPetId){
            const CGA_PetSkillList_t &petskills = pet.skills;

            for(int j = 0; j < petskills.size(); ++j){
                if(petskills.at(j).name == m_SkillName && context.m_UnitGroup[context.m_iPetPosition].curmp >= petskills.at(j).cost){
                    skillpos = j;
                    return true;
                }
            }

            const char szAttack[] = {-71,-91,-69,-9};
            QString attack = QString::fromLocal8Bit(szAttack);
            for(int j = 0; j < petskills.size(); ++j){
                if(petskills.at(j).name == attack){//Attack
                    skillpos = j;
                    return true;
                }
            }

            break;
        }
    }

    return false;
}

void CBattleAction_PetSkillAttack::FixTarget(CGA_BattleContext_t &context, int skillpos, int &target)
{
    if(!context.m_Pets.data())
        return;

    for(int i = 0 ;i < context.m_Pets->size(); ++i )
    {
        const CGA_PetInfo_t &pet = context.m_Pets->at(i);
        if(i == context.m_iPetId)
        {
            const CGA_PetSkillList_t &petskills = pet.skills;
            for(int j = 0; j < petskills.size(); ++j){
                int flags = petskills.at(j).flags;

                if(flags & FL_SKILL_SINGLE)
                    return;
                if(flags & FL_SKILL_MULTI){
                    target = target + 20;
                    return;
                }
                if(flags & FL_SKILL_ALL){
                    target = (target >= 10 && target <= 19) ? 41 : 40;
                    return;
                }
                if(flags & FL_SKILL_BOOM){
                    target = 42;
                    return;
                }
                target = -1;

                return;
            }
        }
    }
}

void CBattleTarget_Self::GetTargetName(QString &str)
{
    str += s_BattleTargetString[BattleTarget_Self];
}

int CBattleTarget_Self::GetTarget(CGA_BattleContext_t &context)
{
    return context.m_iPlayerPosition;
}

void CBattleTarget_Pet::GetTargetName(QString &str)
{
    str += s_BattleTargetString[BattleTarget_Pet];
}

int CBattleTarget_Pet::GetTarget(CGA_BattleContext_t &context)
{
    return context.m_iPetPosition;
}

CBattleTarget_Enemy::CBattleTarget_Enemy(int select)
{
    m_Select = select;
}

void CBattleTarget_Enemy::GetTargetName(QString &str)
{
    str += s_BattleTargetString[BattleTarget_Enemy];
    str += QLatin1String(", ");
    str += s_BattleTargetSelectString[m_Select];
}

bool compareFront(const CGA_BattleUnit_t & a, const CGA_BattleUnit_t & b){
    return a.pos > b.pos;
}

bool compareBack(const CGA_BattleUnit_t & a, const CGA_BattleUnit_t & b){
    return a.pos > b.pos;
}

bool compareLowHP(const CGA_BattleUnit_t & a, const CGA_BattleUnit_t & b){
    return a.curhp < b.curhp;
}

bool compareHighHP(const CGA_BattleUnit_t & a, const CGA_BattleUnit_t & b){
    return a.curhp > b.curhp;
}

bool compareLowLv(const CGA_BattleUnit_t & a, const CGA_BattleUnit_t & b){
    return a.level < b.level;
}

bool compareHighLv(const CGA_BattleUnit_t & a, const CGA_BattleUnit_t & b){
    return a.level > b.level;
}

int CBattleTarget_Enemy::GetTarget(CGA_BattleContext_t &context)
{
    QList<CGA_BattleUnit_t> newGroup;

    for(int i = 0;i < 20; ++i)
    {
        const CGA_BattleUnit_t &unit = context.m_UnitGroup[i];
        if(unit.exist && unit.pos >= 0xA)
            newGroup.append(unit);
    }

    if(!newGroup.size())
        return -1;

    switch(m_Select)
    {
    case BattleTarget_Select_Random:
        return newGroup[qrand() % newGroup.size()].pos;
    case BattleTarget_Select_Front:
        std::sort(newGroup.begin(), newGroup.end(), compareFront );
        break;
    case BattleTarget_Select_Back:
        std::sort(newGroup.begin(), newGroup.end(), compareBack );
        break;
    case BattleTarget_Select_LowHP:
        std::sort(newGroup.begin(), newGroup.end(), compareLowHP );
        break;
    case BattleTarget_Select_HighHP:
        std::sort(newGroup.begin(), newGroup.end(), compareHighHP );
        break;
    case BattleTarget_Select_LowLv:
        std::sort(newGroup.begin(), newGroup.end(), compareLowLv );
        break;
    case BattleTarget_Select_HighLv:
        std::sort(newGroup.begin(), newGroup.end(), compareHighLv );
        break;
    case BattleTarget_Select_Goatfarm:
    {
        const char cname_a[] = {-69,-16,-47,-26,-59,-93,-51,-73,-71,-42};
        QString name_a = QString::fromLocal8Bit(cname_a);
        if(context.m_UnitGroup[0x10].exist && context.m_UnitGroup[0x10].name == name_a){
            return context.m_UnitGroup[0x10].pos;
        } else if(context.m_UnitGroup[0x11].exist && context.m_UnitGroup[0x11].name == name_a){
            return context.m_UnitGroup[0x11].pos;
        } else {
            const char cname_b[] = {-59,-93,-51,-73,-71,-42};
            QString name_b = QString::fromLocal8Bit(cname_b);
            if(context.m_UnitGroup[0x10].exist && context.m_UnitGroup[0x10].name == name_b)
                return context.m_UnitGroup[0x10].pos;
            else if(context.m_UnitGroup[0x11].exist && context.m_UnitGroup[0x11].name == name_b)
                return context.m_UnitGroup[0x11].pos;
        }
        return newGroup[qrand() % newGroup.size()].pos;
    }
    case BattleTarget_Select_Boomerang:
        if(context.m_iFrontCount > context.m_iBackCount)
            std::sort(newGroup.begin(), newGroup.end(), compareFront );
        else
            std::sort(newGroup.begin(), newGroup.end(), compareBack );
        break;
    }

    return newGroup[0].pos;
}

CBattleTarget_Teammate::CBattleTarget_Teammate(int select)
{
    m_Select = select;
}

void CBattleTarget_Teammate::GetTargetName(QString &str)
{
    str += s_BattleTargetString[BattleTarget_Teammate];
    str += QLatin1String(", ");
    str += s_BattleTargetSelectString[m_Select];
}

int CBattleTarget_Teammate::GetTarget(CGA_BattleContext_t &context)
{
    QList<CGA_BattleUnit_t> newGroup;

    for(int i = 0;i < 20; ++i)
    {
        const CGA_BattleUnit_t &unit = context.m_UnitGroup[i];
        if(unit.exist && unit.pos < 0xA)
            newGroup.append(unit);
    }

    if(!newGroup.size())
        return -1;

    switch(m_Select)
    {
    case BattleTarget_Select_Random:
        return newGroup[qrand() % newGroup.size()].pos;
    case BattleTarget_Select_Front:
        std::sort(newGroup.begin(), newGroup.end(), compareFront );
        break;
    case BattleTarget_Select_Back:
        std::sort(newGroup.begin(), newGroup.end(), compareBack );
        break;
    case BattleTarget_Select_LowHP:
        std::sort(newGroup.begin(), newGroup.end(), compareLowHP );
        break;
    case BattleTarget_Select_HighHP:
        std::sort(newGroup.begin(), newGroup.end(), compareHighHP );
        break;
    case BattleTarget_Select_LowLv:
        std::sort(newGroup.begin(), newGroup.end(), compareLowLv );
        break;
    case BattleTarget_Select_HighLv:
        std::sort(newGroup.begin(), newGroup.end(), compareHighLv );
        break;
    case BattleTarget_Select_Goatfarm:
        //TODO:
        break;
    case BattleTarget_Select_Boomerang:
        //TODO:
        break;
    }

    return newGroup[0].pos;
}

CBattleSetting::CBattleSetting(CBattleCondition *cond, CBattleAction *playerAction, CBattleTarget *playerTarget, CBattleAction *petAction, CBattleTarget *petTarget)
{
    m_conditions = cond;
    m_playerAction = playerAction;
    m_playerTarget = playerTarget;
    m_petAction = petAction;
    m_petTarget = petTarget;
}

CBattleSetting::~CBattleSetting()
{
    if(m_conditions)
        delete m_conditions;
    if(m_playerAction)
        delete m_playerAction;
    if(m_playerTarget)
        delete m_playerTarget;
    if(m_petAction)
        delete m_petAction;
    if(m_petTarget)
        delete m_petTarget;
}

bool CBattleSetting::DoAction(CGA_BattleContext_t &context)
{
    if(!m_conditions->Check(context))
        return false;

    bool bIsPet = (context.m_iPlayerStatus == 4 && context.m_iPetPosition >= 0 && context.m_iPetId != -1) ? true : false;

    //qDebug("isPet=%d", bIsPet ? 1 : 0);

    int target = -1;
    if(!bIsPet && m_playerAction)
    {
        if(m_playerTarget)
            target = m_playerTarget->GetTarget(context);

        if(m_playerAction->DoAction(target, context))
            return true;
    }

    if(bIsPet && m_petAction)
    {
        if(m_petTarget)
            target = m_petTarget->GetTarget(context);

        if(m_petAction->DoAction(target, context))
            return true;
    }

    return false;
}

void CBattleSetting::GetConditionName(QString &str)
{
    m_conditions->GetConditionName(str);
}

void CBattleSetting::GetPlayerActionName(QString &str)
{
    if(m_playerAction)
        m_playerAction->GetActionName(str);
}

void CBattleSetting::GetPlayerTargetName(QString &str)
{
    if(m_playerTarget)
        m_playerTarget->GetTargetName(str);
}

void CBattleSetting::GetPetActionName(QString &str)
{
    if(m_petAction)
        m_petAction->GetActionName(str);
}

void CBattleSetting::GetPetTargetName(QString &str)
{
    if(m_petTarget)
        m_petTarget->GetTargetName(str);
}

int CBattleSetting::GetConditionTypeId()
{
    return m_conditions->GetConditionTypeId();
}

int CBattleSetting::GetConditionRelId()
{
    return m_conditions->GetConditionRelId();
}

void CBattleSetting::GetConditionValue(QString &str)
{
    m_conditions->GetConditionValue(str);
}

int CBattleSetting::GetPlayerActionTypeId()
{
    if(m_playerAction)
        return m_playerAction->GetActionTypeId();
    return -1;
}

QString CBattleSetting::GetPlayerSkillName()
{
    if(m_playerAction && m_playerAction->GetActionTypeId() == BattlePlayerAction_Skill)
    {
        auto sk = dynamic_cast<CBattleAction_PlayerSkillAttack *>(m_playerAction);
        return sk->GetSkillName();
    }
    return QString();
}

int CBattleSetting::GetPlayerSkillLevel()
{
    if(m_playerAction && m_playerAction->GetActionTypeId() == BattlePlayerAction_Skill)
    {
        auto sk = dynamic_cast<CBattleAction_PlayerSkillAttack *>(m_playerAction);
        return sk->GetSkillLevel();
    }
    return -1;
}

int CBattleSetting::GetPlayerTargetTypeId()
{
    if(m_playerTarget)
        return m_playerTarget->GetTargetTypeId();
    return -1;
}

int CBattleSetting::GetPlayerTargetSelectId()
{
    if(m_playerTarget)
        return m_playerTarget->GetTargetSelectId();
    return -1;
}

int CBattleSetting::GetPetActionTypeId()
{
    if(m_petAction)
        return m_petAction->GetActionTypeId();
    return -1;
}

QString CBattleSetting::GetPetSkillName()
{
    if(m_petAction && m_petAction->GetActionTypeId() == BattlePetAction_Skill)
    {
        auto sk = dynamic_cast<CBattleAction_PetSkillAttack *>(m_petAction);
        return sk->GetSkillName();
    }
    return QString();
}

int CBattleSetting::GetPetTargetTypeId()
{
    if(m_petTarget)
        return m_petTarget->GetTargetTypeId();
    return -1;
}

int CBattleSetting::GetPetTargetSelectId()
{
    if(m_petTarget)
        return m_petTarget->GetTargetSelectId();
    return -1;
}

bool CBattleSetting::HasPlayerAction()
{
    return (m_playerAction) ? true : false;
}

bool CBattleSetting::HasPetAction()
{
    return (m_petAction) ? true : false;
}

CBattleWorker::CBattleWorker()
{
    m_bAutoBattle = false;
    m_bHighSpeed = false;
    m_bFirstRoundNoDelay = false;
    m_bLevelOneProtect = false;
    m_bShowHPMP = false;
    m_iDelayFrom = 0;
    m_iDelayTo = 0;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(OnLockCountdown()));
    timer->start(500);

    connect(this, &CBattleWorker::NotifyBattleAction, this, &CBattleWorker::OnNotifyBattleAction, Qt::ConnectionType::QueuedConnection);
}

void CBattleWorker::OnLockCountdown()
{
    if(!m_bLockCountdown)
        return;

    int gameStatus = 0;
    if(!g_CGAInterface->GetGameStatus(gameStatus) && gameStatus != 10)
        return;

    g_CGAInterface->SetBattleEndTick(1000*30);
}

#if 0
const int s_XYToUnitPos[4][5] =
{
    {0xE, 0xC, 0xA, 0xB, 0xD},
    {0x13, 0x11, 0xF, 0x10, 0x12},
    {0x9, 0x7, 0x5, 0x6, 0x8},
    {0x4, 0x2, 0x0, 0x1, 0x3},
};

const int s_UnitPosToXY[][2] =
{
    {3, 2}, {3, 3}, {3, 1}, {3, 4}, {3, 0},
    {2, 2}, {2, 3}, {2, 1}, {2, 4}, {2, 0},
    {0, 2}, {0, 3}, {0, 1}, {0, 4}, {0, 0},
    {1, 2}, {1, 3}, {1, 1}, {1, 4}, {1, 0},
};
#endif

void CBattleWorker::GetBattleUnits()
{
    m_BattleContext.m_iFrontCount = 0;
    m_BattleContext.m_iBackCount = 0;
    m_BattleContext.m_iEnemyCount = 0;
    m_BattleContext.m_iTeammateCount = 0;
    m_BattleContext.m_iPetPosition = -1;

    CGA_BattleUnitGroup_t &group = m_BattleContext.m_UnitGroup;

    for(int i = 0;i < 20; ++i)
        group[i].exist = false;

    CGA::cga_battle_units_t us;
    if(g_CGAInterface->GetBattleUnits(us))
    {
        for(size_t i = 0;i < us.size(); ++i)
        {
            const CGA::cga_battle_unit_t &u = us.at(i);
            group[u.pos].exist = true;
            group[u.pos].name = QString::fromStdString(u.name);
            group[u.pos].curhp =u.curhp;
            group[u.pos].maxhp =u.maxhp;
            group[u.pos].curmp =u.curmp;
            group[u.pos].maxmp =u.maxmp;
            group[u.pos].level =u.level;
            group[u.pos].pos = u.pos;

            if(u.pos >= 0xF && u.pos <= 0x13)
                m_BattleContext.m_iFrontCount ++;
            else if(u.pos >= 0xA && u.pos <= 0xE)
                m_BattleContext.m_iBackCount ++;

            if(u.pos >= 0xA)
                m_BattleContext.m_iEnemyCount ++;
            else if(u.pos != m_BattleContext.m_iPlayerPosition)
                m_BattleContext.m_iTeammateCount ++;

            if(u.pos == GetPetPosition(m_BattleContext.m_iPlayerPosition))
                m_BattleContext.m_iPetPosition = u.pos;
        }
    }
}

void CBattleWorker::OnPerformanceBattle()
{
    for(int i = 0;i < m_SettingList.size(); ++i)
    {
        const CBattleSettingPtr &ptr = m_SettingList.at(i);
        if(ptr->DoAction(m_BattleContext))
            return;
    }
}

void CBattleWorker::OnNotifyGetSkillsInfo(QSharedPointer<CGA_SkillList_t> skills)
{
    m_BattleContext.m_PlayerSkills = skills;
}

void CBattleWorker::OnNotifyGetPetsInfo(QSharedPointer<CGA_PetList_t> pets)
{
    m_BattleContext.m_Pets = pets;
}

void CBattleWorker::OnNotifyGetItemsInfo(QSharedPointer<CGA_ItemList_t> items)
{
    m_BattleContext.m_Items = items;
}

bool CBattleWorker::CheckLevelOneProtect()
{
    if(!m_bLevelOneProtect)
        return false;

    for(int i = 0xA;i < 20; ++i)
    {
        if(m_BattleContext.m_UnitGroup[i].exist && m_BattleContext.m_UnitGroup[i].level == 1)
            return true;
    }
    return false;
}

void CBattleWorker::OnNotifyBattleAction(int flags)
{
    int ingame = 0;

    if(!g_CGAInterface->IsConnected() || !g_CGAInterface->IsInGame(ingame) || !ingame)
        return;

    int gameStatus = 0;
    if(!g_CGAInterface->GetGameStatus(gameStatus) && gameStatus != 10)
        return;

    if(!g_CGAInterface->GetBattlePlayerStatus(m_BattleContext.m_iPlayerStatus) || !m_BattleContext.m_iPlayerStatus)
        return;

    if(m_BattleContext.m_iPlayerStatus != 1 && m_BattleContext.m_iPlayerStatus != 4)
        return;

    m_BattleContext.m_bIsPlayer = (flags & FL_BATTLE_ACTION_ISPLAYER) ? true : false;
    m_BattleContext.m_bIsDouble = (flags & FL_BATTLE_ACTION_ISDOUBLE) ? true : false;
    m_BattleContext.m_bIsSkillPerformed = (flags & FL_BATTLE_ACTION_ISSKILLPERFORMED) ? true : false;

    g_CGAInterface->BattleSetHighSpeedEnabled(m_bHighSpeed);
    g_CGAInterface->GetBattleRoundCount(m_BattleContext.m_iRoundCount);
    g_CGAInterface->GetBattlePlayerPosition(m_BattleContext.m_iPlayerPosition);
    g_CGAInterface->GetBattlePetId(m_BattleContext.m_iPetId);

    //qDebug("petid = %d m_bIsSkillPerformed = %d\n", m_BattleContext.m_iPetId, m_BattleContext.m_bIsSkillPerformed ? 1 : 0);

    GetBattleUnits();

    if(m_bAutoBattle)
    {
        if(CheckLevelOneProtect()){
            //qDebug("Found Lv1 enemy, stopped.");
            return;
        }

        if(((m_BattleContext.m_iRoundCount == 0 && !m_bFirstRoundNoDelay) || m_BattleContext.m_iRoundCount > 0) && m_BattleContext.m_iLastRound != m_BattleContext.m_iRoundCount)
        {
            int randDelay = qrand() * (m_iDelayTo - m_iDelayFrom) / RAND_MAX + m_iDelayFrom;
            if(randDelay < 100)
                randDelay = 100;
            if(randDelay > 10000)
                randDelay = 10000;
            QTimer::singleShot( randDelay, this, SLOT(OnPerformanceBattle()) );
        }
        else
        {
            OnPerformanceBattle();
        }

        if(m_BattleContext.m_iLastRound != m_BattleContext.m_iRoundCount){
            m_BattleContext.m_bIsSkillPerformed = false;
            m_BattleContext.m_iLastRound = m_BattleContext.m_iRoundCount;
        }
    }
}

void CBattleWorker::OnNotifyAttachProcessOk(quint32 ProcessId, quint32 port, quint32 hWnd)
{
    g_CGAInterface->RegisterBattleActionNotify(std::bind(&CBattleWorker::NotifyBattleAction, this, std::placeholders::_1));
}

void CBattleWorker::OnSetAutoBattle(int state)
{
    m_bAutoBattle = state ? true : false;
}

void CBattleWorker::OnSetHighSpeed(int state)
{
    m_bHighSpeed = state ? true : false;

    g_CGAInterface->BattleSetHighSpeedEnabled(m_bHighSpeed);
}

void CBattleWorker::OnSetFRND(int state)
{
    m_bFirstRoundNoDelay = state ? true : false;
}

void CBattleWorker::OnSetLv1Protect(int state)
{
    m_bLevelOneProtect = state ? true : false;
}

void CBattleWorker::OnSetLockCountdown(int state)
{
    m_bLockCountdown = state ? true : false;
}


void CBattleWorker::OnSetShowHPMP(int state)
{
    m_bShowHPMP = state ? true : false;
    g_CGAInterface->BattleSetShowHPMPEnabled(m_bShowHPMP);
}

void CBattleWorker::OnSetDelayFrom(int val)
{
    m_iDelayFrom = val;
}

void CBattleWorker::OnSetDelayTo(int val)
{
    m_iDelayTo = val;
}

void CBattleWorker::OnSyncList(CBattleSettingList list)
{
    m_SettingList = list;
}
