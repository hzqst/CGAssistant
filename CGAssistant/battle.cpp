#include <QTimer>
#include <QDateTime>
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

CBattleCondition_Ignore::CBattleCondition_Ignore()
{

}

void CBattleCondition_Ignore::GetConditionName(QString &str)
{
    str = QObject::tr("Ignore");
}

bool CBattleCondition_Ignore::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    return true;
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

bool CBattleCondition_EnemyCount::Check(CGA_BattleContext_t &context, int &conditionTarget)
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

CBattleCondition_TeammateCount::CBattleCondition_TeammateCount(int relation, int value)
{
    m_relation = relation;
    m_value = value;
}

void CBattleCondition_TeammateCount::GetConditionName(QString &str)
{
    str = QObject::tr("TeammateCount %1%2").arg(s_BattleCondRelationNumber[m_relation], QString::number(m_value));
}

bool CBattleCondition_TeammateCount::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    switch (m_relation)
    {
    case BattleCond_NumRel_EGT:
        return context.m_iTeammateCount >= m_value;
    case BattleCond_NumRel_GT:
        return context.m_iTeammateCount > m_value;
    case BattleCond_NumRel_ELT:
        return context.m_iTeammateCount <= m_value;
    case BattleCond_NumRel_LT:
        return context.m_iTeammateCount < m_value;
    case BattleCond_NumRel_EQ:
        return context.m_iTeammateCount == m_value;
    case BattleCond_NumRel_NEQ:
        return context.m_iTeammateCount != m_value;
    }

    return false;
}

CBattleCondition_EnemySingleRowCount::CBattleCondition_EnemySingleRowCount(int relation, int value)
{
    m_relation = relation;
    m_value = value;
}

void CBattleCondition_EnemySingleRowCount::GetConditionName(QString &str)
{
    str = QObject::tr("Enemy SingleRow Count %1%2").arg(s_BattleCondRelationNumber[m_relation], QString::number(m_value));
}

bool CBattleCondition_EnemySingleRowCount::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    switch (m_relation)
    {
    case BattleCond_NumRel_EGT:
        if( context.m_iFrontCount >= m_value)
            return true;
        if( context.m_iBackCount >= m_value)
            return true;
        break;
    case BattleCond_NumRel_GT:
        if( context.m_iFrontCount > m_value)
            return true;
        if( context.m_iBackCount > m_value)
            return true;
        break;
    case BattleCond_NumRel_ELT:
        if( context.m_iFrontCount <= m_value)
            return true;
        if( context.m_iBackCount <= m_value)
            return true;
        break;
    case BattleCond_NumRel_LT:
        if( context.m_iFrontCount < m_value)
            return true;
        if( context.m_iBackCount < m_value)
            return true;
        break;
    case BattleCond_NumRel_EQ:
        if( context.m_iFrontCount == m_value)
            return true;
        if( context.m_iBackCount == m_value)
            return true;
        break;
    case BattleCond_NumRel_NEQ:
        if( context.m_iFrontCount != m_value)
            return true;
        if( context.m_iBackCount != m_value)
            return true;
        break;
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

bool CBattleCondition_PlayerHp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    if(context.m_iPlayerPosition < 0 || context.m_iPlayerPosition > 20)
        return false;

    int curv = context.m_UnitGroup[context.m_iPlayerPosition].curhp;
    int maxv = context.m_UnitGroup[context.m_iPlayerPosition].maxhp;

    if(m_percentage && maxv > 0){
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            if(curv * 100 / maxv >= m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_GT:
            if(curv * 100 / maxv > m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_ELT:
            if(curv * 100 / maxv <= m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_LT:
            if(curv * 100 / maxv < m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_EQ:
            if(curv * 100 / maxv == m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_NEQ:
            if(curv * 100 / maxv != m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        }
    } else {
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            if(curv >= m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_GT:
            if(curv > m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_ELT:
            if(curv <= m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_LT:
            if(curv < m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_EQ:
            if(curv == m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_NEQ:
            if(curv != m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
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
bool CBattleCondition_PlayerMp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    if(context.m_iPlayerPosition < 0 || context.m_iPlayerPosition > 20)
        return false;
    int curv = context.m_UnitGroup[context.m_iPlayerPosition].curmp;
    int maxv = context.m_UnitGroup[context.m_iPlayerPosition].maxmp;
    if(m_percentage && maxv > 0){
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            if(curv * 100 / maxv >= m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_GT:
            if(curv * 100 / maxv > m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_ELT:
            if(curv * 100 / maxv <= m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_LT:
            if(curv * 100 / maxv < m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_EQ:
            if(curv * 100 / maxv == m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_NEQ:
            if(curv * 100 / maxv != m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        }
    } else {
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            if(curv >= m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_GT:
            if(curv > m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_ELT:
            if(curv <= m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_LT:
            if(curv < m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_EQ:
            if(curv == m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_NEQ:
            if(curv != m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
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

bool CBattleCondition_PetHp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    if(context.m_iPetPosition < 0 || context.m_iPetPosition > 20)
        return false;
    int curv = context.m_UnitGroup[context.m_iPetPosition].curhp;
    int maxv = context.m_UnitGroup[context.m_iPetPosition].maxhp;
    if(m_percentage && maxv > 0){
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            if(curv * 100 / maxv >= m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_GT:
            if(curv * 100 / maxv > m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_ELT:
            if(curv * 100 / maxv <= m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_LT:
            if(curv * 100 / maxv < m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_EQ:
            if(curv * 100 / maxv == m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_NEQ:
            if(curv * 100 / maxv != m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        }
    } else {
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            if(curv >= m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_GT:
            if(curv > m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_ELT:
            if(curv <= m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_LT:
            if(curv < m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_EQ:
            if(curv == m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_NEQ:
            if(curv != m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
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
bool CBattleCondition_PetMp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    if(context.m_iPetPosition < 0 || context.m_iPetPosition > 20)
        return false;
    int curv = context.m_UnitGroup[context.m_iPetPosition].curmp;
    int maxv = context.m_UnitGroup[context.m_iPetPosition].maxmp;
    if(m_percentage && maxv > 0){
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            if(curv * 100 / maxv >= m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_GT:
            if(curv * 100 / maxv > m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_ELT:
            if(curv * 100 / maxv <= m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_LT:
            if(curv * 100 / maxv < m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_EQ:
            if(curv * 100 / maxv == m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_NEQ:
            if(curv * 100 / maxv != m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        }
    } else {
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            if(curv >= m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_GT:
            if(curv > m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_ELT:
            if(curv <= m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_LT:
            if(curv < m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_EQ:
            if(curv == m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
        case BattleCond_NumRel_NEQ:
            if(curv != m_value)
            {
                conditionTarget = context.m_iPlayerPosition;
                return true;
            }
            break;
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

bool CBattleCondition_TeammateHp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    for(int i = 0;i < 0xA; ++i)
    {
        if(!context.m_UnitGroup[i].exist)
            continue;
        int curv = context.m_UnitGroup[i].curhp;
        int maxv = context.m_UnitGroup[i].maxhp;
        //qDebug("curhp%d maxhp%d v%d r%d p%d", curv, maxv, m_value, m_relation, m_percentage?1:0);
        if(m_percentage && maxv > 0){
            switch (m_relation)
            {
            case BattleCond_NumRel_EGT:
                if(curv * 100 / maxv >= m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_GT:
                if(curv * 100 / maxv > m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_ELT:
                if(curv * 100 / maxv <= m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_LT:
                if(curv * 100 / maxv < m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_EQ:
                if(curv * 100 / maxv == m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_NEQ:
                if(curv * 100 / maxv != m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            }
        }
        else
        {
            switch (m_relation)
            {
            case BattleCond_NumRel_EGT:
                if( curv >= m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_GT:
                if( curv > m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_ELT:
                if( curv <= m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_LT:
                if( curv < m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_EQ:
                if( curv == m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_NEQ:
                if( curv != m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            }
        }
    }
    return false;
}

CBattleCondition_TeammateMp::CBattleCondition_TeammateMp(int relation, int value, bool percentage)
{
    m_relation = relation;
    m_value = value;
    m_percentage = percentage;
}

void CBattleCondition_TeammateMp::GetConditionName(QString &str)
{
    str = QObject::tr("TeammateMP %1%2").arg(s_BattleCondRelationNumber[m_relation], QString::number(m_value));
    if(m_percentage)
        str += QLatin1String("%");
}

bool CBattleCondition_TeammateMp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    for(int i = 0;i < 0xA; ++i)
    {
        if(!context.m_UnitGroup[i].exist)
            continue;
        int curv = context.m_UnitGroup[i].curmp;
        int maxv = context.m_UnitGroup[i].maxmp;
        //qDebug("curhp%d maxhp%d v%d r%d p%d", curv, maxv, m_value, m_relation, m_percentage?1:0);
        if(m_percentage && maxv > 0){
            switch (m_relation)
            {
            case BattleCond_NumRel_EGT:
                if(curv * 100 / maxv >= m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_GT:
                if(curv * 100 / maxv > m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_ELT:
                if(curv * 100 / maxv <= m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_LT:
                if(curv * 100 / maxv < m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_EQ:
                if(curv * 100 / maxv == m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_NEQ:
                if(curv * 100 / maxv != m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            }
        }
        else
        {
            switch (m_relation)
            {
            case BattleCond_NumRel_EGT:
                if( curv >= m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_GT:
                if( curv > m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_ELT:
                if( curv <= m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_LT:
                if( curv < m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_EQ:
                if( curv == m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_NEQ:
                if( curv != m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            }
        }
    }
    return false;
}


CBattleCondition_EnemyMultiTargetHp::CBattleCondition_EnemyMultiTargetHp(int relation, int value, bool percentage)
{
    m_relation = relation;
    m_value = value;
    m_percentage = percentage;
}

void CBattleCondition_EnemyMultiTargetHp::GetConditionName(QString &str)
{
    str = QObject::tr("Enemy MultiTarget HP %1%2").arg(s_BattleCondRelationNumber[m_relation], QString::number(m_value));
}

bool CBattleCondition_EnemyMultiTargetHp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    for(int i = 0xA;i < 20; ++i)
    {
        if(!context.m_UnitGroup[i].exist)
            continue;
        int curv = context.m_UnitGroup[i].curhp;
        int maxv = context.m_UnitGroup[i].maxhp;
        //qDebug("curhp%d maxhp%d v%d r%d p%d", curv, maxv, m_value, m_relation, m_percentage?1:0);
        if(m_percentage && maxv > 0){
            switch (m_relation)
            {
            case BattleCond_NumRel_EGT:
                if(curv * 100 / maxv >= m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_GT:
                if(curv * 100 / maxv > m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_ELT:
                if(curv * 100 / maxv <= m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_LT:
                if(curv * 100 / maxv < m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_EQ:
                if(curv * 100 / maxv == m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_NEQ:
                if(curv * 100 / maxv != m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            }
        }
        else
        {
            switch (m_relation)
            {
            case BattleCond_NumRel_EGT:
                if( curv >= m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_GT:
                if( curv > m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_ELT:
                if( curv <= m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_LT:
                if( curv < m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_EQ:
                if( curv == m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_NEQ:
                if( curv != m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            }
        }
    }
    return false;
}

CBattleCondition_TeammateMultiTargetHp::CBattleCondition_TeammateMultiTargetHp(int relation, int value, bool percentage)
{
    m_relation = relation;
    m_value = value;
    m_percentage = percentage;
}

void CBattleCondition_TeammateMultiTargetHp::GetConditionName(QString &str)
{
    str = QObject::tr("Teammate MultiTarget HP %1%2").arg(s_BattleCondRelationNumber[m_relation], QString::number(m_value));
    if(m_percentage)
        str += QLatin1String("%");
}

bool CBattleCondition_TeammateMultiTargetHp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    for(int i = 0;i < 0xA; ++i)
    {
        if(!context.m_UnitGroup[i].exist)
            continue;
        int curv = context.m_UnitGroup[i].multi_hp;
        int maxv = context.m_UnitGroup[i].multi_maxhp;
        qDebug("CBattleCondition_TeammateMultiTargetHp curhp%d maxhp%d v%d r%d p%d", curv, maxv, m_value, m_relation, m_percentage?1:0);
        if(m_percentage && maxv > 0){
            switch (m_relation)
            {
            case BattleCond_NumRel_EGT:
                if(curv * 100 / maxv >= m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_GT:
                if(curv * 100 / maxv > m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_ELT:
                if(curv * 100 / maxv <= m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_LT:
                if(curv * 100 / maxv < m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_EQ:
                if(curv * 100 / maxv == m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_NEQ:
                if(curv * 100 / maxv != m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            }
        }
        else
        {
            switch (m_relation)
            {
            case BattleCond_NumRel_EGT:
                if( curv >= m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_GT:
                if( curv > m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_ELT:
                if( curv <= m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_LT:
                if( curv < m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_EQ:
                if( curv == m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            case BattleCond_NumRel_NEQ:
                if( curv != m_value){
                    conditionTarget = i;
                    return true;
                }
                break;
            }
        }
    }
    return false;
}

CBattleCondition_EnemyAllHp::CBattleCondition_EnemyAllHp(int relation, int value, bool percentage)
{
    m_relation = relation;
    m_value = value;
    m_percentage = percentage;
}

void CBattleCondition_EnemyAllHp::GetConditionName(QString &str)
{
    str = QObject::tr("Enemy All HP %1%2").arg(s_BattleCondRelationNumber[m_relation], QString::number(m_value));
}

bool CBattleCondition_EnemyAllHp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    int curv = 0, maxv = 0;
    for(int i = 0xA; i < 20; ++i)
    {
        if(!context.m_UnitGroup[i].exist)
            continue;
        curv += context.m_UnitGroup[i].curhp;
        maxv += context.m_UnitGroup[i].maxhp;
    }

    if(m_percentage && maxv > 0){
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            if(curv * 100 / maxv >= m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_GT:
            if(curv * 100 / maxv > m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_ELT:
            if(curv * 100 / maxv <= m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_LT:
            if(curv * 100 / maxv < m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_EQ:
            if(curv * 100 / maxv == m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_NEQ:
            if(curv * 100 / maxv != m_value){
                return true;
            }
            break;
        }
    }
    else
    {
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            if( curv >= m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_GT:
            if( curv > m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_ELT:
            if( curv <= m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_LT:
            if( curv < m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_EQ:
            if( curv == m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_NEQ:
            if( curv != m_value){
                return true;
            }
            break;
        }
    }

    return false;
}

CBattleCondition_TeammateAllHp::CBattleCondition_TeammateAllHp(int relation, int value, bool percentage)
{
    m_relation = relation;
    m_value = value;
    m_percentage = percentage;
}

void CBattleCondition_TeammateAllHp::GetConditionName(QString &str)
{
    str = QObject::tr("Teammate All HP %1%2").arg(s_BattleCondRelationNumber[m_relation], QString::number(m_value));
    if(m_percentage)
        str += QLatin1String("%");
}

bool CBattleCondition_TeammateAllHp::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    int curv = 0, maxv = 0;
    for(int i = 0;i < 0xA; ++i)
    {
        if(!context.m_UnitGroup[i].exist)
            continue;
        curv += context.m_UnitGroup[i].curhp;
        maxv += context.m_UnitGroup[i].maxhp;
    }

    if(m_percentage && maxv > 0){
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            if(curv * 100 / maxv >= m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_GT:
            if(curv * 100 / maxv > m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_ELT:
            if(curv * 100 / maxv <= m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_LT:
            if(curv * 100 / maxv < m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_EQ:
            if(curv * 100 / maxv == m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_NEQ:
            if(curv * 100 / maxv != m_value){
                return true;
            }
            break;
        }
    }
    else
    {
        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            if( curv >= m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_GT:
            if( curv > m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_ELT:
            if( curv <= m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_LT:
            if( curv < m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_EQ:
            if( curv == m_value){
                return true;
            }
            break;
        case BattleCond_NumRel_NEQ:
            if( curv != m_value){
                return true;
            }
            break;
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

bool CBattleCondition_EnemyUnit::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    for(int i = 0xA;i < 20; ++i)
    {
        if(!context.m_UnitGroup[i].exist)
            continue;
        if(context.m_UnitGroup[i].name == m_UnitName){
            if(m_relation == BattleCond_StrRel_CONTAIN){
                conditionTarget = i;
                return true;
            }
        }
    }
    return (m_relation == BattleCond_StrRel_NOT_CONTAIN) ? true : false;
}

CBattleCondition_Round::CBattleCondition_Round(int relation, int value)
{
    m_relation = relation;
    m_value = value;
}

void CBattleCondition_Round::GetConditionName(QString &str)
{
    str = QObject::tr("Round %1%2").arg(s_BattleCondRelationNumber[m_relation], QString::number(m_value));
}

bool CBattleCondition_Round::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    switch (m_relation)
    {
    case BattleCond_NumRel_EGT:
        return context.m_iRoundCount + 1 >= m_value;
    case BattleCond_NumRel_GT:
        return context.m_iRoundCount + 1 > m_value;
    case BattleCond_NumRel_ELT:
        return context.m_iRoundCount + 1 <= m_value;
    case BattleCond_NumRel_LT:
        return context.m_iRoundCount + 1 < m_value;
    case BattleCond_NumRel_EQ:
        return context.m_iRoundCount + 1 == m_value;
    case BattleCond_NumRel_NEQ:
        return context.m_iRoundCount + 1 != m_value;
    }

    return false;
}

CBattleCondition_DoubleAction::CBattleCondition_DoubleAction()
{

}

void CBattleCondition_DoubleAction::GetConditionName(QString &str)
{
    str = QObject::tr("DoubleAction");
}

bool CBattleCondition_DoubleAction::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    if(context.m_bIsDouble)
        return true;

    return false;
}

CBattleCondition_TeammateDebuff::CBattleCondition_TeammateDebuff(int relation, int value)
{
    m_relation = relation;
    m_value = value;
}

void CBattleCondition_TeammateDebuff::GetConditionName(QString &str)
{
    using namespace CGA;
    QString debuff;
    if(m_value == FL_DEBUFF_ANY){
        debuff += QObject::tr("Any");
    } else {
        if(m_value & FL_DEBUFF_SLEEP){
           if(!debuff.isEmpty())
               debuff += " ";
           debuff += QObject::tr("Sleep");
        }
        if(m_value & FL_DEBUFF_MEDUSA){
            if(!debuff.isEmpty())
                debuff += " ";
           debuff += QObject::tr("Medusa");
        }
        if(m_value & FL_DEBUFF_DRUNK){
            if(!debuff.isEmpty())
                debuff += " ";
           debuff += QObject::tr("Drunk");
        }
        if(m_value & FL_DEBUFF_CHAOS){
            if(!debuff.isEmpty())
                debuff += " ";
           debuff += QObject::tr("Chaos");
        }
        if(m_value & FL_DEBUFF_FORGET){
            if(!debuff.isEmpty())
                debuff += " ";
           debuff += QObject::tr("Forget");
        }
        if(m_value & FL_DEBUFF_POISON){
            if(!debuff.isEmpty())
                debuff += " ";
           debuff += QObject::tr("Poison");
        }
    }

    str = QObject::tr("Teammate Debuff %1 %2").arg(s_BattleCondRelationString[m_relation], debuff);
}

bool CBattleCondition_TeammateDebuff::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    switch (m_relation)
    {
    case BattleCond_StrRel_CONTAIN:{
        for(int i = 0;i < 10; ++i){
            if(context.m_UnitGroup[i].exist)
            {
                if(context.m_UnitGroup[i].flags & m_value)
                {
                    conditionTarget = i;
                    return true;
               }
            }
        }
        return false;
    }
    case BattleCond_StrRel_NOT_CONTAIN:{
        for(int i = 0;i < 10; ++i){
            if(context.m_UnitGroup[i].exist)
            {
                if(context.m_UnitGroup[i].flags & m_value)
                  return false;
            }
        }
        return true;
    }
    }

    return false;
}

CBattleCondition_EnemyLevel::CBattleCondition_EnemyLevel(int relation, int value)
{
    m_relation = relation;
    m_value = value;
}

void CBattleCondition_EnemyLevel::GetConditionName(QString &str)
{
    str = QObject::tr("Enemy Level %1%2").arg(s_BattleCondRelationNumber[m_relation], QString::number(m_value));
}

bool CBattleCondition_EnemyLevel::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    for(int i = 0xA;i < 20; ++i)
    {
        if(!context.m_UnitGroup[i].exist)
            continue;
        int curv = context.m_UnitGroup[i].level;

        switch (m_relation)
        {
        case BattleCond_NumRel_EGT:
            if( curv >= m_value){
                conditionTarget = i;
                return true;
            }
            break;
        case BattleCond_NumRel_GT:
            if( curv > m_value){
                conditionTarget = i;
                return true;
            }
            break;
        case BattleCond_NumRel_ELT:
            if( curv <= m_value){
                conditionTarget = i;
                return true;
            }
            break;
        case BattleCond_NumRel_LT:
            if( curv < m_value){
                conditionTarget = i;
                return true;
            }
            break;
        case BattleCond_NumRel_EQ:
            if( curv == m_value){
                conditionTarget = i;
                return true;
            }
            break;
        case BattleCond_NumRel_NEQ:
            if( curv != m_value){
                conditionTarget = i;
                return true;
            }
            break;
        }
    }
    return false;
}

CBattleCondition_EnemyAvgLevel::CBattleCondition_EnemyAvgLevel(int relation, int value)
{
    m_relation = relation;
    m_value = value;
}

void CBattleCondition_EnemyAvgLevel::GetConditionName(QString &str)
{
    str = QObject::tr("Enemy Average Level %1%2").arg(s_BattleCondRelationNumber[m_relation], QString::number(m_value));
}

bool CBattleCondition_EnemyAvgLevel::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    int level = 0, count = 0;
    for(int i = 0xA;i < 20; ++i)
    {
        if(!context.m_UnitGroup[i].exist)
            continue;
        level += context.m_UnitGroup[i].level;
        count += 1;
    }

    float avg = (float)level / count;

    switch (m_relation)
    {
    case BattleCond_NumRel_EGT:
        if( avg >= (float)m_value){
            return true;
        }
        break;
    case BattleCond_NumRel_GT:
        if( avg > (float)m_value){
            return true;
        }
        break;
    case BattleCond_NumRel_ELT:
        if( avg <= (float)m_value){
            return true;
        }
        break;
    case BattleCond_NumRel_LT:
        if( avg < (float)m_value){
            return true;
        }
        break;
    case BattleCond_NumRel_EQ:
        if( avg == (float)m_value){
            return true;
        }
        break;
    case BattleCond_NumRel_NEQ:
        if( avg != (float)m_value){
            return true;
        }
        break;
    }

    return false;
}

CBattleCondition_InventoryItem::CBattleCondition_InventoryItem(int relation, QString &itemName)
{
    m_relation = relation;
    m_ItemName = itemName;
}

void CBattleCondition_InventoryItem::GetConditionName(QString &str)
{
    str = QObject::tr("Inventory %1%2").arg( s_BattleCondRelationString[m_relation], m_ItemName);
}

bool CBattleCondition_InventoryItem::Check(CGA_BattleContext_t &context, int &conditionTarget)
{
    for(int i = 0;i < context.m_Items->size(); ++i)
    {
        if(context.m_Items->at(i).name == m_ItemName){
            if(m_relation == BattleCond_StrRel_CONTAIN){
                return true;
            }
            else if(m_relation == BattleCond_StrRel_NOT_CONTAIN)
            {
                return false;
            }
        }
        else if(m_ItemName[0] == '@'){
            bool ok = false;
            auto val = m_ItemName.mid(1).toInt(&ok);
            if(ok && context.m_Items->at(i).type == val){
                if(m_relation == BattleCond_StrRel_CONTAIN){
                    return true;
                }
                else if(m_relation == BattleCond_StrRel_NOT_CONTAIN)
                {
                    return false;
                }
            }
        }
        else if(m_ItemName[0] == '#'){
            bool ok = false;
            auto val = m_ItemName.mid(1).toInt(&ok);
            if(ok && context.m_Items->at(i).itemid == val){
                if(m_relation == BattleCond_StrRel_CONTAIN){
                    return true;
                }
                else if(m_relation == BattleCond_StrRel_NOT_CONTAIN)
                {
                    return false;
                }
            }
        }
    }
    return (m_relation == BattleCond_StrRel_NOT_CONTAIN) ? true : false;
}

void CBattleAction_PlayerAttack::GetActionName(QString &str, bool config)
{
    str = QObject::tr("Attack");
}

bool CBattleAction_PlayerAttack::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
    bool result = false;
    g_CGAInterface->BattleNormalAttack(target, result);
    return result;
}

void CBattleAction_PlayerGuard::GetActionName(QString &str, bool config)
{
    str = QObject::tr("Guard");
}

bool CBattleAction_PlayerGuard::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
    bool result = false;
    g_CGAInterface->BattleGuard(result);
    return result;
}

void CBattleAction_PlayerEscape::GetActionName(QString &str, bool config)
{
    str = QObject::tr("Escape");
}

bool CBattleAction_PlayerEscape::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
    bool result = false;
    g_CGAInterface->BattleEscape(result);
    context.m_bIsPlayerEscaped = true;
    return result;
}

void CBattleAction_PlayerExchangePosition::GetActionName(QString &str, bool config)
{
    str = QObject::tr("Exchange Position");
}

bool CBattleAction_PlayerExchangePosition::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
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

void CBattleAction_PlayerChangePet::GetActionName(QString &str, bool config)
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
    case 4:
        str = QObject::tr("Call pet with highest mana");
        break;
    case 5:
        str = QObject::tr("Call pet with highest loyalty");
        break;
    case 6:
        str = QObject::tr("Call pet with lowest level");
        break;
    case 7:
        str = QObject::tr("Call pet with lowest health");
        break;
    case 8:
        str = QObject::tr("Call pet with lowest mana");
        break;
    case 9:
        str = QObject::tr("Call pet with lowest loyalty");
        break;
    }
}

bool CBattleAction_PlayerChangePet::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
    int petid = -1;
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
            if(pet.hp > highest){
                highest = pet.hp;
                petid = pet.id;
                found = true;
            }
        }
        break;
    }
    case 4:
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
            if(pet.mp > highest){
                highest = pet.mp;
                petid = pet.id;
                found = true;
            }
        }
        break;
    }
    case 5:
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
            if(pet.loyality > highest){
                highest = pet.loyality;
                petid = pet.id;
                found = true;
            }
        }
        break;
    }
    case 6:
    {
        int lowest = 9999;
        for(int i = 0 ;i < context.m_Pets->size(); ++i ){
            const CGA_PetInfo_t &pet = context.m_Pets->at(i);
            if(pet.id == context.m_iPetId)
                continue;
            if(pet.hp == 0)
                continue;
            if((pet.battle_flags & 3) == 0)
                continue;
            if(pet.level < lowest){
                lowest = pet.level;
                petid = pet.id;
                found = true;
            }
        }
        break;
    }
    case 7:
    {
        int lowest = 9999;
        for(int i = 0 ;i < context.m_Pets->size(); ++i ){
            const CGA_PetInfo_t &pet = context.m_Pets->at(i);
            if(pet.id == context.m_iPetId)
                continue;
            if(pet.hp == 0)
                continue;
            if((pet.battle_flags & 3) == 0)
                continue;
            if(pet.hp < lowest){
                lowest = pet.hp;
                petid = pet.id;
                found = true;
            }
        }
        break;
    }
    case 8:
    {
        int lowest = 9999;
        for(int i = 0 ;i < context.m_Pets->size(); ++i ){
            const CGA_PetInfo_t &pet = context.m_Pets->at(i);
            if(pet.id == context.m_iPetId)
                continue;
            if(pet.hp == 0)
                continue;
            if((pet.battle_flags & 3) == 0)
                continue;
            if(pet.mp < lowest){
                lowest = pet.mp;
                petid = pet.id;
                found = true;
            }
        }
        break;
    }
    case 9:
    {
        int lowest = 9999;
        for(int i = 0 ;i < context.m_Pets->size(); ++i ){
            const CGA_PetInfo_t &pet = context.m_Pets->at(i);
            if(pet.id == context.m_iPetId)
                continue;
            if(pet.hp == 0)
                continue;
            if((pet.battle_flags & 3) == 0)
                continue;
            if(pet.loyality < lowest){
                lowest = pet.loyality;
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

void CBattleAction_PlayerSkillAttack::GetActionName(QString &str, bool config)
{
    str = m_SkillName;
    if(m_SkillLevel >= 1 && m_SkillLevel <= 10)
        str += QObject::tr(" Lv %1").arg(QString::number(m_SkillLevel));
    else
        str += QObject::tr(" Lv Max");
}

int CBattleAction_PlayerSkillAttack::GetTargetFlags(CGA_BattleContext_t &context)
{
    if(!context.m_PlayerSkills.data())
        return 0;

    int skill_pos, skill_level;
    if(!GetSkill(context, skill_pos, skill_level))
        return 0;

    for(int i = 0;i < context.m_PlayerSkills->size(); ++i)
    {
        const CGA_SkillInfo_t &skill = context.m_PlayerSkills->at(i);
        if(skill_pos == skill.id)
        {
            int flags = skill.subskills.at(skill_level).flags;
            return flags;
        }
    }
    return 0;
}

bool CBattleAction_PlayerSkillAttack::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
    int skill_pos, skill_level;

    bool result = false;
    if(context.m_bIsSkillPerformed == false && GetSkill(context, skill_pos, skill_level)){
        qDebug("BattleSkillAttack %d %d %d", skill_pos, skill_level, target);
        FixTarget(context, skill_pos, skill_level, target);
        g_CGAInterface->BattleSkillAttack(skill_pos, skill_level, target, context.m_bIsPlayerForceAction ? true : false, result);
    }

    if(!result){
        qDebug("BattleNormalAttack %d %d", skill_pos, target);
        g_CGAInterface->BattleNormalAttack(defaultTarget, result);
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
            skillpos = skill.id;
            skilllevel = skill.lv;
            if(m_SkillLevel >= 1 && skilllevel > m_SkillLevel)
                skilllevel = m_SkillLevel;
            int playerMp = context.m_UnitGroup[context.m_iPlayerPosition].curmp;
            const CGA_SubSkills_t &subsks = skill.subskills;
            for(int j = subsks.size() - 1; j >= 0; --j){
                if(skilllevel >= subsks.at(j).level &&
                        subsks.at(j).available &&
                       playerMp >= subsks.at(j).cost){
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

    for(int i = 0;i < context.m_PlayerSkills->size(); ++i)
    {
        const CGA_SkillInfo_t &skill = context.m_PlayerSkills->at(i);
        if(skill_pos == skill.id)
        {
            int flags = skill.subskills.at(skill_level).flags;

            if(!(flags & FL_SKILL_SELECT_TARGET)){
                target = context.m_iPlayerPosition;
                return;
            }
            if(flags & FL_SKILL_SINGLE){
                return;
            }
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
            target = 255;
            return;
        }
    }
}

CBattleAction_PlayerUseItem::CBattleAction_PlayerUseItem(QString &itemName)
{
    m_ItemId = 0;
    m_ItemType = 0;
    if(itemName[0] == '#')
    {
        bool bValue = false;
        int value = itemName.mid(1).toInt(&bValue);
        if(bValue)
            m_ItemId = value;
    }
    else if(itemName[0] == '@')
    {
        bool bValue = false;
        int value = itemName.mid(1).toInt(&bValue);
        if(bValue)
            m_ItemType = value;
    }
    else
    {
        m_ItemName = itemName;
    }
}

void CBattleAction_PlayerUseItem::GetActionName(QString &str, bool config)
{
    if(!config){
        if(m_ItemId > 0)
        {
            str = QObject::tr("Use item id #%1").arg(m_ItemId);
        }
        else if(m_ItemType > 0)
        {
            str = QObject::tr("Use item type @%1").arg(m_ItemType);
        }
        else
        {
            str = QObject::tr("Use item %1").arg(m_ItemName);
        }
    } else {
        if(m_ItemId > 0)
        {
            str = QString("#%1").arg(m_ItemId);
        }
        else if(m_ItemType > 0)
        {
            str = QString("@%1").arg(m_ItemType);
        }
        else
        {
            str = m_ItemName;
        }
    }
}

bool CBattleAction_PlayerUseItem::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
    int itempos = -1;
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
        if(!m_ItemName.isEmpty() && item.name == m_ItemName && item.assessed){
            itempos = item.pos;
            return true;
        }
        if(m_ItemId > 0 && item.itemid == m_ItemId && item.assessed){
            itempos = item.pos;
            return true;
        }
        if(m_ItemType > 0 && item.type == m_ItemType && item.assessed){
            itempos = item.pos;
            return true;
        }
    }

    return false;
}

CBattleAction_PlayerLogBack::CBattleAction_PlayerLogBack()
{

}

void CBattleAction_PlayerLogBack::GetActionName(QString &str, bool config)
{
    str = QObject::tr("LogBack");
}

bool CBattleAction_PlayerLogBack::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
    return g_CGAInterface->LogBack();
}

CBattleAction_PlayerRebirth::CBattleAction_PlayerRebirth()
{

}

void CBattleAction_PlayerRebirth::GetActionName(QString &str, bool config)
{
    str = QObject::tr("Rebirth");
}

bool CBattleAction_PlayerRebirth::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
    bool result = false;
    if(g_CGAInterface->BattleRebirth(result))
        return result;

    return false;
}

CBattleAction_PlayerDoNothing::CBattleAction_PlayerDoNothing()
{

}

void CBattleAction_PlayerDoNothing::GetActionName(QString &str, bool config)
{
    str = QObject::tr("Do Nothing");
}

bool CBattleAction_PlayerDoNothing::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
    bool result = false;
    if(g_CGAInterface->BattleDoNothing(result))
        return result;

    return false;
}

CBattleAction_PetSkillAttack::CBattleAction_PetSkillAttack(QString &skillName)
{
    m_SkillName = skillName;
}

void CBattleAction_PetSkillAttack::GetActionName(QString &str, bool config)
{
    str = m_SkillName;
}

int CBattleAction_PetSkillAttack::GetTargetFlags(CGA_BattleContext_t &context)
{
    if(context.m_iPetId == -1)
        return 0;

    if(!context.m_Pets.data())
        return 0;

    for(int i = 0 ;i < context.m_Pets->size(); ++i ){
        const CGA_PetInfo_t &pet = context.m_Pets->at(i);
        if(pet.id == context.m_iPetId){
            const CGA_PetSkillList_t &petskills = pet.skills;

            for(int j = 0; j < petskills.size(); ++j){
                if(petskills.at(j).name == m_SkillName){

                    return petskills.at(j).flags;
                }
            }
            break;
        }
    }

    return 0;
}

bool CBattleAction_PetSkillAttack::DoAction(int target, int defaultTarget, CGA_BattleContext_t &context)
{
    int skillpos = -1;

    bool result = false;
    bool bUseDefaultTarget = false;

    auto donothing = QObject::tr("Do Nothing");
    if(donothing == m_SkillName)
    {
        g_CGAInterface->BattleDoNothing(result);
        return result;
    }

    if(GetSkill(context, skillpos, bUseDefaultTarget))
    {
        //qDebug("target1=%d", target);
        target = bUseDefaultTarget ? defaultTarget : target;
        //qDebug("target2=%d", target);
        FixTarget(context, skillpos, target);
        //qDebug("target3=%d", target);
        if(context.m_bIsPetDoubleAction && !context.m_bIsPlayerEscaped){
            g_CGAInterface->BattlePetSkillAttack(skillpos, target, true, result);
            g_CGAInterface->BattlePetSkillAttack(skillpos, target, true, result);
        } else {
            g_CGAInterface->BattlePetSkillAttack(skillpos, target, false, result);
        }
    }

    qDebug("BattlePetSkillAttack %d %d", skillpos, target);

    return result;
}

bool CBattleAction_PetSkillAttack::GetSkill(CGA_BattleContext_t &context, int &skillpos, bool &bUseDefaultTarget)
{
    if(context.m_iPetId == -1)
        return false;

    if(!context.m_Pets.data())
        return false;

    const unsigned char szAttack[] = {0xB9, 0xA5, 0xBB, 0xF7, 0};
    QString attack = QString::fromLocal8Bit((const char *)szAttack);

    const unsigned char szGuard[] = {0xB7, 0xC0, 0xD3, 0xF9, 0};
    QString guard = QString::fromLocal8Bit((const char *)szGuard);

    for(int i = 0 ;i < context.m_Pets->size(); ++i ){
        const CGA_PetInfo_t &pet = context.m_Pets->at(i);
        if(pet.id == context.m_iPetId){
            const CGA_PetSkillList_t &petskills = pet.skills;

            for(int j = 0; j < petskills.size(); ++j){
                //char *ppp = petskills.at(j).name.toLocal8Bit().data();

                if(
                        petskills.at(j).name == m_SkillName &&
                        context.m_UnitGroup[context.m_iPetPosition].curmp >= petskills.at(j).cost &&
                        (context.m_iPetSkillAllowBit & (1 << petskills.at(j).pos))
                        ){
                    skillpos = petskills.at(j).pos;
                    return true;
                }

            }

            for(int j = 0; j < petskills.size(); ++j){
                if(
                        petskills.at(j).name == attack &&
                        (context.m_iPetSkillAllowBit & (1 << petskills.at(j).pos))
                        ){

                    //qDebug("find attack");
                    skillpos = petskills.at(j).pos;
                    bUseDefaultTarget = true;
                    return true;
                }
            }

            for(int j = 0; j < petskills.size(); ++j){
                if(
                        petskills.at(j).name == guard &&
                        (context.m_iPetSkillAllowBit & (1 << petskills.at(j).pos))
                        ){

                    //qDebug("find guard");
                    skillpos = petskills.at(j).pos;
                    bUseDefaultTarget = true;
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
        if(pet.id == context.m_iPetId)
        {
            const CGA_PetSkillList_t &petskills = pet.skills;
            for(int j = 0; j < petskills.size(); ++j){
                if(petskills.at(j).pos == skillpos){
                    int flags = petskills.at(j).flags;
                    //qDebug("skill name = %s", petskills.at(j).name.toLocal8Bit().data());
                    //qDebug("skill flags = %X", flags);
                    if(!(flags & FL_SKILL_SELECT_TARGET)){
                        target = context.m_iPetPosition;
                        return;
                    }
                    if(flags & FL_SKILL_SINGLE)
                        return;
                    if(flags & FL_SKILL_MULTI){
                        //qDebug("multi skill");
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
                    target = 255;
                    return;
                }
            }
        }
    }
}

void CBattleTarget_Self::GetTargetName(QString &str)
{
    str += s_BattleTargetString[BattleTarget_Self];
}

int CBattleTarget_Self::GetTarget(int unitpos, int flags, CGA_BattleContext_t &context)
{
    return context.m_iPlayerPosition;
}

void CBattleTarget_Pet::GetTargetName(QString &str)
{
    str += s_BattleTargetString[BattleTarget_Pet];
}

int CBattleTarget_Pet::GetTarget(int unitpos, int flags, CGA_BattleContext_t &context)
{
    return context.m_iPetPosition;
}

void CBattleTarget_Condition::GetTargetName(QString &str)
{
    str += s_BattleTargetString[BattleTarget_Condition];
}

int CBattleTarget_Condition::GetTarget(int unitpos, int flags, CGA_BattleContext_t &context)
{
    return context.m_iConditionTarget;
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
    return a.pos < b.pos;
}

bool compareLowHP(const CGA_BattleUnit_t & a, const CGA_BattleUnit_t & b){
    return a.curhp < b.curhp;
}

bool compareHighHP(const CGA_BattleUnit_t & a, const CGA_BattleUnit_t & b){
    return a.curhp > b.curhp;
}

bool compareLowHPPercent(const CGA_BattleUnit_t & a, const CGA_BattleUnit_t & b){
    return (float)a.curhp / (float)a.maxhp < (float)b.curhp / (float)b.maxhp;
}

bool compareHighHPPercent(const CGA_BattleUnit_t & a, const CGA_BattleUnit_t & b){
    return a.curhp > b.curhp;
}

bool compareLowLv(const CGA_BattleUnit_t & a, const CGA_BattleUnit_t & b){
    return a.level < b.level;
}

bool compareHighLv(const CGA_BattleUnit_t & a, const CGA_BattleUnit_t & b){
    return a.level > b.level;
}

bool compareSingleDebuff(const CGA_BattleUnit_t & a, const CGA_BattleUnit_t & b){
    return a.flags > b.flags;
}

bool compareMultiDebuff(const CGA_BattleUnit_t & a, const CGA_BattleUnit_t & b){
    return a.debuff > b.debuff;
}

bool compareMultiMagic(const CGA_BattleUnit_t & a, const CGA_BattleUnit_t & b){
    return a.multi_hp > b.multi_hp;
}

int CBattleTarget_Enemy::GetTarget(int unitpos, int flags, CGA_BattleContext_t &context)
{
    QList<CGA_BattleUnit_t> newGroup;

    if(context.m_iPlayerPosition >= 0xA)
    {
        for(int i = 0;i < 20; ++i)
        {
            const CGA_BattleUnit_t &unit = context.m_UnitGroup[i];
            if(unit.exist && unit.pos < 0xA){

                if((flags & FL_SKILL_FRONT_ONLY) && unit.isback && context.m_UnitGroup[unitpos].isback &&
                        ( (unitpos == context.m_iPlayerPosition && (context.m_iWeaponAllowBit & 0x80) ) || (unitpos == context.m_iPetPosition) )
                        )
                {
                    continue;
                }

                newGroup.append(unit);
            }
        }
    }
    else
    {
        for(int i = 0;i < 20; ++i)
        {
            const CGA_BattleUnit_t &unit = context.m_UnitGroup[i];
            if(unit.exist && unit.pos >= 0xA){

                //qDebug("flags %X isback %d unitpos %d", flags, unit.isback ? 1 : 0, unit.pos);
                if((flags & FL_SKILL_FRONT_ONLY) && unit.isback && context.m_UnitGroup[unitpos].isback &&
                        ( (unitpos == context.m_iPlayerPosition && (context.m_iWeaponAllowBit & 0x80) ) || (unitpos == context.m_iPetPosition) )
                        )
                {
                    //qDebug("u%d ignored", unit.pos);
                    continue;
                }

                newGroup.append(unit);
            }
        }
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
    case BattleTarget_Select_LowHPPercent:
        std::sort(newGroup.begin(), newGroup.end(), compareLowHPPercent );
        break;
    case BattleTarget_Select_HighHPPercent:
        std::sort(newGroup.begin(), newGroup.end(), compareHighHPPercent );
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
    case BattleTarget_Select_LessUnitRow:
        if(context.m_iBackCount > context.m_iFrontCount)
            std::sort(newGroup.begin(), newGroup.end(), compareFront );
        else
            std::sort(newGroup.begin(), newGroup.end(), compareBack );
        break;
    case BattleTarget_Select_MultiMagic:
        std::sort(newGroup.begin(), newGroup.end(), compareMultiMagic );
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

int CBattleTarget_Teammate::GetTarget(int unitpos, int flags, CGA_BattleContext_t &context)
{
    QList<CGA_BattleUnit_t> newGroup;

    if(context.m_iPlayerPosition >= 0xA)
    {
        for(int i = 0;i < 20; ++i)
        {
            const CGA_BattleUnit_t &unit = context.m_UnitGroup[i];
            if(unit.exist && unit.pos >= 0xA)
                newGroup.append(unit);
        }
    }
    else
    {
        for(int i = 0;i < 20; ++i)
        {
            const CGA_BattleUnit_t &unit = context.m_UnitGroup[i];
            if(unit.exist && unit.pos < 0xA)
                newGroup.append(unit);
        }
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
    case BattleTarget_Select_LowHPPercent:
        std::sort(newGroup.begin(), newGroup.end(), compareLowHPPercent );
        break;
    case BattleTarget_Select_HighHPPercent:
        std::sort(newGroup.begin(), newGroup.end(), compareHighHPPercent );
        break;
    case BattleTarget_Select_LowLv:
        std::sort(newGroup.begin(), newGroup.end(), compareLowLv );
        break;
    case BattleTarget_Select_HighLv:
        std::sort(newGroup.begin(), newGroup.end(), compareHighLv );
        break;
    case BattleTarget_Select_SingleDebuff:
        std::sort(newGroup.begin(), newGroup.end(), compareSingleDebuff );
        break;
    case BattleTarget_Select_MultiDebuff:
        std::sort(newGroup.begin(), newGroup.end(), compareMultiDebuff );
        break;
    }

    return newGroup[0].pos;
}

CBattleSetting::CBattleSetting(CBattleCondition *cond, CBattleCondition *cond2,
                               CBattleAction *playerAction, CBattleTarget *playerTarget,
                               CBattleAction *petAction, CBattleTarget *petTarget/*,
                               CBattleAction *petAction2, CBattleTarget *petTarget2*/)
{
    m_condition = cond;
    m_condition2 = cond2;
    m_playerAction = playerAction;
    m_playerTarget = playerTarget;
    m_petAction = petAction;
    m_petTarget = petTarget;
    /*m_petAction2 = petAction2;
    m_petTarget2 = petTarget2;*/
    m_defaultTarget = new CBattleTarget_Enemy(BattleTarget_Select_Random);
}

CBattleSetting::~CBattleSetting()
{
    if(m_condition)
        delete m_condition;
    if(m_condition2)
        delete m_condition2;
    if(m_playerAction)
        delete m_playerAction;
    if(m_playerTarget)
        delete m_playerTarget;
    if(m_petAction)
        delete m_petAction;
    if(m_petTarget)
        delete m_petTarget;
    if(m_defaultTarget)
        delete m_defaultTarget;
}

bool CBattleSetting::DoAction(CGA_BattleContext_t &context)
{
    int conditionTarget = -1, condition2Target = -1;

    //qDebug("checking condition %d %d", GetConditionTypeId(), GetCondition2TypeId());

    if(!m_condition && !m_condition2)
        return false;

    if(m_condition && !m_condition->Check(context, conditionTarget))
        return false;

    //qDebug("checking condition 1 pass");

    if(m_condition2 && !m_condition2->Check(context, condition2Target))
        return false;

    //qDebug("checking condition 2 pass");

    if(conditionTarget != -1){
        context.m_iConditionTarget = conditionTarget;
        //qDebug("m_iConditionTarget = %d", conditionTarget);
    } else if(condition2Target != -1){
        context.m_iConditionTarget = condition2Target;
        //qDebug("m_iConditionTarget = %d", condition2Target);
    } else {
        context.m_iConditionTarget = -1;
    }

    bool bHasPet = context.m_iPetPosition >= 0 && context.m_iPetId != -1;
    bool bIsPetAction = (context.m_iPlayerStatus == 4) ? true : false;

    if(!bIsPetAction)
    {
        if(context.m_bIsPetDoubleAction && bHasPet)
        {
            if(m_petAction)
            {
                int flags = 0;
                int target = -1;
                int defaultTarget = -1;
                int defaultFlags = FL_SKILL_SINGLE | FL_SKILL_TO_PET | FL_SKILL_TO_TEAMMATE | FL_SKILL_TO_ENEMY | FL_SKILL_FRONT_ONLY | FL_SKILL_SELECT_TARGET;

                qDebug("m_petAction");

                flags = m_petAction->GetTargetFlags(context);

                if(m_petTarget)
                    target = m_petTarget->GetTarget(context.m_iPetPosition, flags, context);

                if(m_defaultTarget)
                    defaultTarget = m_defaultTarget->GetTarget(context.m_iPetPosition, defaultFlags, context);

                if(m_petAction->DoAction(target, defaultTarget, context)){
                    qDebug("m_petAction ok");
                }
            }
            /*{
                int flags = 0;
                int target = -1;
                int defaultTarget = -1;
                int defaultFlags = FL_SKILL_SINGLE | FL_SKILL_TO_PET | FL_SKILL_TO_TEAMMATE | FL_SKILL_TO_ENEMY | FL_SKILL_FRONT_ONLY | FL_SKILL_SELECT_TARGET;

                qDebug("m_petAction2");

                context.m_bIsPetDoubleAction = true;

                flags = m_petAction2->GetTargetFlags(context);

                if(m_petTarget2)
                    target = m_petTarget2->GetTarget(context.m_iPetPosition, flags, context);

                if(m_defaultTarget)
                    defaultTarget = m_defaultTarget->GetTarget(context.m_iPetPosition, defaultFlags, context);

                if(m_petAction2->DoAction(target, defaultTarget, context)){
                    qDebug("m_petAction2 ok");
                }
            }*/
        }

        if(m_playerAction)
        {
            int flags = 0;
            int target = -1;
            int defaultTarget = -1;
            int defaultFlags = FL_SKILL_SINGLE | FL_SKILL_TO_PET | FL_SKILL_TO_TEAMMATE | FL_SKILL_TO_ENEMY | FL_SKILL_FRONT_ONLY | FL_SKILL_SELECT_TARGET;

            flags = m_playerAction->GetTargetFlags(context);

            if(m_playerTarget)
                target = m_playerTarget->GetTarget(context.m_iPlayerPosition, flags, context);

            if(m_defaultTarget)
                defaultTarget = m_defaultTarget->GetTarget(context.m_iPlayerPosition, defaultFlags, context);

            //qDebug("m_playerAction DoAction");

            if(m_playerAction->DoAction(target, defaultTarget, context)){
                context.m_bIsPlayerActionPerformed = true;
                return true;
            } else {
                bool result = false;
                if(g_CGAInterface->BattleDoNothing(result) && result)
                {
                    context.m_bIsPlayerActionPerformed = true;
                    return true;
                }
            }
            qDebug("m_playerAction failed to DoAction");
        }
        else
        {
            qDebug("m_playerAction not present, try next");
            return false;
        }
    }

    if(bIsPetAction && m_petAction)
    {
        if(context.m_bIsPlayerForceAction && m_playerAction && !context.m_bIsPlayerActionPerformed)
        {
            int flags = 0;
            int target = -1;
            int defaultTarget = -1;
            int defaultFlags = FL_SKILL_SINGLE | FL_SKILL_TO_PET | FL_SKILL_TO_TEAMMATE | FL_SKILL_TO_ENEMY | FL_SKILL_FRONT_ONLY | FL_SKILL_SELECT_TARGET;

            flags = m_playerAction->GetTargetFlags(context);

            if(m_playerTarget)
                target = m_playerTarget->GetTarget(context.m_iPlayerPosition, flags, context);

            if(m_defaultTarget)
                defaultTarget = m_defaultTarget->GetTarget(context.m_iPlayerPosition, defaultFlags, context);

            //qDebug("m_playerAction DoAction");

            if(m_playerAction->DoAction(target, defaultTarget, context)){
                context.m_bIsPlayerActionPerformed = true;
            } else {
                bool result = false;
                if(g_CGAInterface->BattleDoNothing(result) && result)
                {
                    context.m_bIsPlayerActionPerformed = true;
                }
            }
        }

        int flags = 0;
        int target = -1;
        int defaultTarget = -1;
        int defaultFlags = FL_SKILL_SINGLE | FL_SKILL_TO_PET | FL_SKILL_TO_TEAMMATE | FL_SKILL_TO_ENEMY | FL_SKILL_FRONT_ONLY | FL_SKILL_SELECT_TARGET;

        qDebug("m_petAction");

        context.m_bIsPetDoubleAction = false;

        flags = m_petAction->GetTargetFlags(context);

        if(m_petTarget)
            target = m_petTarget->GetTarget(context.m_iPetPosition, flags, context);

        if(m_defaultTarget)
            defaultTarget = m_defaultTarget->GetTarget(context.m_iPetPosition, defaultFlags, context);

        if(m_petAction->DoAction(target, defaultTarget, context)){
            qDebug("m_petAction ok");
            return true;
        } else {
            bool result = false;
            if(g_CGAInterface->BattleDoNothing(result) && result)
                return true;
        }
    }

    qDebug("action failed");
    return false;
}

void CBattleSetting::GetPlayerActionName(QString &str, bool config)
{
    if(m_playerAction){
        m_playerAction->GetActionName(str, config);
    }
}

void CBattleSetting::GetPlayerTargetName(QString &str)
{
    if(m_playerTarget)
        m_playerTarget->GetTargetName(str);
}

void CBattleSetting::GetPetActionName(QString &str, bool config)
{
    if(m_petAction)
        m_petAction->GetActionName(str, config);
}

void CBattleSetting::GetPetTargetName(QString &str)
{
    if(m_petTarget)
        m_petTarget->GetTargetName(str);
}

/*void CBattleSetting::GetPetAction2Name(QString &str)
{
    if(m_petAction2)
        m_petAction2->GetActionName(str);
}

void CBattleSetting::GetPetTarget2Name(QString &str)
{
    if(m_petTarget2)
        m_petTarget2->GetTargetName(str);
}*/

void CBattleSetting::GetConditionName(QString &str)
{
    if(m_condition)
        m_condition->GetConditionName(str);
}

int CBattleSetting::GetConditionTypeId()
{
    if (m_condition)
        return m_condition->GetConditionTypeId();

    return -1;
}

int CBattleSetting::GetConditionRelId()
{
    if (m_condition)
        return m_condition->GetConditionRelId();

    return -1;
}

void CBattleSetting::GetConditionValue(QString &str)
{
    if(m_condition)
        m_condition->GetConditionValue(str);
}

void CBattleSetting::GetCondition2Name(QString &str)
{
    if(m_condition2)
        m_condition2->GetConditionName(str);
}

int CBattleSetting::GetCondition2TypeId()
{
    if(m_condition2)
        return m_condition2->GetConditionTypeId();

    return -1;
}

int CBattleSetting::GetCondition2RelId()
{
    if(m_condition2)
        return m_condition2->GetConditionRelId();

    return -1;
}

void CBattleSetting::GetCondition2Value(QString &str)
{
    if(m_condition2)
        m_condition2->GetConditionValue(str);
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

/*int CBattleSetting::GetPetAction2TypeId()
{
    if(m_petAction2)
        return m_petAction2->GetActionTypeId();
    return -1;
}

QString CBattleSetting::GetPetSkill2Name()
{
    if(m_petAction && m_petAction->GetActionTypeId() == BattlePetAction_Skill)
    {
        auto sk = dynamic_cast<CBattleAction_PetSkillAttack *>(m_petAction2);
        return sk->GetSkillName();
    }
    return QString();
}

int CBattleSetting::GetPetTarget2TypeId()
{
    if(m_petTarget2)
        return m_petTarget2->GetTargetTypeId();
    return -1;
}

int CBattleSetting::GetPetTarget2SelectId()
{
    if(m_petTarget2)
        return m_petTarget2->GetTargetSelectId();
    return -1;
}*/

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
    m_bBOSSProtect = false;
    m_bNoSwitchAnim = false;
    m_bPetDoubleAction = false;
    m_bPlayerForceAction = false;
    m_iDelayFrom = 0;
    m_iDelayTo = 0;
    m_LastWarpMap202 = 0;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(OnLockCountdown()));
    timer->start(500);

    connect(this, &CBattleWorker::NotifyBattleAction, this, &CBattleWorker::OnNotifyBattleAction, Qt::ConnectionType::QueuedConnection);
}

void CBattleWorker::OnLockCountdown()
{
    int worldStatus = 0;
    int gameStatus = 0;

    g_CGAInterface->BattleSetHighSpeedEnabled(m_bHighSpeed);

    if(g_CGAInterface->GetGameStatus(gameStatus) && gameStatus == 202)
    {
        auto timestamp = QDateTime::currentDateTime().toTime_t();
        if(m_LastWarpMap202 == 0)
        {
            m_LastWarpMap202 = timestamp;
        }
        else if(timestamp - m_LastWarpMap202 >= 5)
        {
            g_CGAInterface->FixMapWarpStuck(0);
            m_LastWarpMap202 = timestamp + 8;
            qDebug("fix warp");
        }
    }
    else {
        m_LastWarpMap202 = 0;
    }

    if (g_CGAInterface->GetWorldStatus(worldStatus) && worldStatus != 10)
	{
		m_BattleContext.m_iLastRound = -1;
        m_BattleContext.m_bRoundZeroNotified = false;
        m_BattleContext.m_bIsPlayerActionPerformed = false;
        m_BattleContext.m_bIsPlayerEscaped = false;
		return;
	}

    if(m_bLockCountdown)
    {
        g_CGAInterface->SetBattleEndTick(1000*30);
    }
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

    for(int i = 0;i < 20; ++i){
        group[i].exist = false;
        group[i].isback = false;
        group[i].debuff = 0;
    }

    CGA::cga_battle_units_t us;
    if(g_CGAInterface->GetBattleUnits(us))
    {
        for(size_t i = 0;i < us.size(); ++i)
        {
            const CGA::cga_battle_unit_t &u = us.at(i);

            group[u.pos].exist = true;
            group[u.pos].name = QString::fromStdString(u.name);
            group[u.pos].curhp = u.curhp;
            group[u.pos].maxhp = u.maxhp;
            group[u.pos].curmp = u.curmp;
            group[u.pos].maxmp = u.maxmp;
            group[u.pos].level = u.level;
            group[u.pos].flags = u.flags;
            group[u.pos].pos = u.pos;

            group[u.pos].debuff = 0;
            group[u.pos].multi_hp = 0;
            group[u.pos].multi_maxhp = 0;

            //if(u.flags != 0)
            //    qDebug("pos %d flags %X", u.pos, u.flags);

            if(u.pos >= 0xF && u.pos <= 0x13)
                m_BattleContext.m_iFrontCount ++;
            else if(u.pos >= 0xA && u.pos <= 0xE)
                m_BattleContext.m_iBackCount ++;

            if(u.pos >= 0xA)
                m_BattleContext.m_iEnemyCount ++;
            else// if(u.pos != m_BattleContext.m_iPlayerPosition && u.pos != GetPetPosition(m_BattleContext.m_iPlayerPosition))
                m_BattleContext.m_iTeammateCount ++;

            if(u.pos == GetPetPosition(m_BattleContext.m_iPlayerPosition))
                m_BattleContext.m_iPetPosition = u.pos;
        }
        for(size_t i = 0;i < 20; ++i)
        {
            if(group[i].exist)
            {
                if(i >= 0 && i <= 4 && group[i+5].exist){
                   group[i].isback = true;
                }
                else if(i >= 10 && i <= 14 && group[5+i].exist){
                   group[i].isback = true;
                }
                if((group[i].flags & FL_DEBUFF_ANY))
                    group[i].debuff ++;

                group[i].multi_hp += group[i].curhp;
                group[i].multi_maxhp += group[i].maxhp;

                if(i == 0){
                    if(group[1].exist && (group[1].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[2].exist && (group[2].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[5].exist && (group[5].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[1].exist)
                        group[i].multi_hp += group[1].curhp;
                    if(group[2].exist)
                        group[i].multi_hp += group[2].curhp;
                    if(group[5].exist)
                        group[i].multi_hp += group[5].curhp;

                    if(group[1].exist)
                        group[i].multi_maxhp += group[1].maxhp;
                    if(group[2].exist)
                        group[i].multi_maxhp += group[2].maxhp;
                    if(group[5].exist)
                        group[i].multi_maxhp += group[5].maxhp;
                }
                else if(i == 1){
                    if(group[0].exist && (group[0].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[3].exist && (group[3].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[6].exist && (group[6].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[0].exist)
                        group[i].multi_hp += group[0].curhp;
                    if(group[3].exist)
                        group[i].multi_hp += group[3].curhp;
                    if(group[6].exist)
                        group[i].multi_hp += group[6].curhp;

                    if(group[0].exist)
                        group[i].multi_maxhp += group[0].maxhp;
                    if(group[3].exist)
                        group[i].multi_maxhp += group[3].maxhp;
                    if(group[6].exist)
                        group[i].multi_maxhp += group[6].maxhp;
                }
                else if(i == 2){
                    if(group[0].exist && (group[0].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[4].exist && (group[4].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[7].exist && (group[7].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[0].exist)
                        group[i].multi_hp += group[0].curhp;
                    if(group[4].exist)
                        group[i].multi_hp += group[4].curhp;
                    if(group[7].exist)
                        group[i].multi_hp += group[7].curhp;

                    if(group[0].exist)
                        group[i].multi_maxhp += group[0].maxhp;
                    if(group[4].exist)
                        group[i].multi_maxhp += group[4].maxhp;
                    if(group[7].exist)
                        group[i].multi_maxhp += group[7].maxhp;
                }
                else if(i == 3){
                    if(group[1].exist && (group[1].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[8].exist && (group[8].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[1].exist)
                        group[i].multi_hp += group[1].curhp;
                    if(group[8].exist)
                        group[i].multi_hp += group[8].curhp;

                    if(group[1].exist)
                        group[i].multi_maxhp += group[1].maxhp;
                    if(group[8].exist)
                        group[i].multi_maxhp += group[8].maxhp;
                }
                else if(i == 4){
                    if(group[2].exist && (group[2].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[9].exist && (group[9].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[2].exist)
                        group[i].multi_hp += group[2].curhp;
                    if(group[9].exist)
                        group[i].multi_hp += group[9].curhp;

                    if(group[2].exist)
                        group[i].multi_maxhp += group[2].maxhp;
                    if(group[9].exist)
                        group[i].multi_maxhp += group[9].maxhp;
                }
                else if(i == 5){
                    if(group[0].exist && (group[0].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[6].exist && (group[6].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[7].exist && (group[7].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[0].exist)
                        group[i].multi_hp += group[0].curhp;
                    if(group[6].exist)
                        group[i].multi_hp += group[6].curhp;
                    if(group[7].exist)
                        group[i].multi_hp += group[7].curhp;

                    if(group[0].exist)
                        group[i].multi_maxhp += group[0].maxhp;
                    if(group[6].exist)
                        group[i].multi_maxhp += group[6].maxhp;
                    if(group[7].exist)
                        group[i].multi_maxhp += group[7].maxhp;
                }
                else if(i == 6){
                    if(group[1].exist && (group[1].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[5].exist && (group[5].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[8].exist && (group[8].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[1].exist)
                        group[i].multi_hp += group[1].curhp;
                    if(group[5].exist)
                        group[i].multi_hp += group[5].curhp;
                    if(group[8].exist)
                        group[i].multi_hp += group[8].curhp;

                    if(group[1].exist)
                        group[i].multi_maxhp += group[1].maxhp;
                    if(group[5].exist)
                        group[i].multi_maxhp += group[5].maxhp;
                    if(group[8].exist)
                        group[i].multi_maxhp += group[8].maxhp;
                }
                else if(i == 7){
                    if(group[2].exist && (group[2].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[5].exist && (group[5].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[9].exist && (group[9].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[2].exist)
                        group[i].multi_hp += group[2].curhp;
                    if(group[5].exist)
                        group[i].multi_hp += group[5].curhp;
                    if(group[9].exist)
                        group[i].multi_hp += group[9].curhp;

                    if(group[2].exist)
                        group[i].multi_maxhp += group[2].maxhp;
                    if(group[5].exist)
                        group[i].multi_maxhp += group[5].maxhp;
                    if(group[9].exist)
                        group[i].multi_maxhp += group[9].maxhp;
                }
                else if(i == 8){
                    if(group[6].exist && (group[6].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[3].exist && (group[3].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[6].exist)
                        group[i].multi_hp += group[6].curhp;
                    if(group[3].exist)
                        group[i].multi_hp += group[3].curhp;

                    if(group[6].exist)
                        group[i].multi_maxhp += group[6].maxhp;
                    if(group[3].exist)
                        group[i].multi_maxhp += group[3].maxhp;
                }
                else if(i == 9){
                    if(group[4].exist && (group[4].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[7].exist && (group[7].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[4].exist)
                        group[i].multi_hp += group[4].curhp;
                    if(group[7].exist)
                        group[i].multi_hp += group[7].curhp;

                    if(group[4].exist)
                        group[i].multi_maxhp += group[4].maxhp;
                    if(group[7].exist)
                        group[i].multi_maxhp += group[7].maxhp;
                } else if(i == 0xA){
                    if(group[0xB].exist && (group[0xB].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[0xC].exist && (group[0xC].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[0xF].exist && (group[0xF].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[0xB].exist)
                        group[i].multi_hp += group[0xB].curhp;
                    if(group[0xC].exist)
                        group[i].multi_hp += group[0xC].curhp;
                    if(group[0xF].exist)
                        group[i].multi_hp += group[0xF].curhp;

                    if(group[0xB].exist)
                        group[i].multi_maxhp += group[0xB].maxhp;
                    if(group[0xC].exist)
                        group[i].multi_maxhp += group[0xC].maxhp;
                    if(group[0xF].exist)
                        group[i].multi_maxhp += group[0xF].maxhp;
                } else if(i == 0xB){
                    if(group[0x10].exist && (group[0x10].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[0xA].exist && (group[0xA].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[0xD].exist && (group[0xD].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[0x10].exist)
                        group[i].multi_hp += group[0x10].curhp;
                    if(group[0xA].exist)
                        group[i].multi_hp += group[0xA].curhp;
                    if(group[0xD].exist)
                        group[i].multi_hp += group[0xD].curhp;

                    if(group[0x10].exist)
                        group[i].multi_maxhp += group[0x10].maxhp;
                    if(group[0xA].exist)
                        group[i].multi_maxhp += group[0xA].maxhp;
                    if(group[0xD].exist)
                        group[i].multi_maxhp += group[0xD].maxhp;
                } else if(i == 0xC){
                    if(group[0x11].exist && (group[0x11].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[0xA].exist && (group[0xA].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[0xE].exist && (group[0xE].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[0x11].exist)
                        group[i].multi_hp += group[0x11].curhp;
                    if(group[0xA].exist)
                        group[i].multi_hp += group[0xA].curhp;
                    if(group[0xE].exist)
                        group[i].multi_hp += group[0xE].curhp;

                    if(group[0x11].exist)
                        group[i].multi_maxhp += group[0x11].maxhp;
                    if(group[0xA].exist)
                        group[i].multi_maxhp += group[0xA].maxhp;
                    if(group[0xE].exist)
                        group[i].multi_maxhp += group[0xE].maxhp;
                }else if(i == 0xD){
                    if(group[0x12].exist && (group[0x12].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[0xB].exist && (group[0xB].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[0x12].exist)
                        group[i].multi_hp += group[0x12].curhp;
                    if(group[0xB].exist)
                        group[i].multi_hp += group[0xB].curhp;

                    if(group[0x12].exist)
                        group[i].multi_maxhp += group[0x12].maxhp;
                    if(group[0xB].exist)
                        group[i].multi_maxhp += group[0xB].maxhp;
                }else if(i == 0xE){
                    if(group[0x13].exist && (group[0x13].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[0xC].exist && (group[0xC].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[0x13].exist)
                        group[i].multi_hp += group[0x13].curhp;
                    if(group[0xC].exist)
                        group[i].multi_hp += group[0xC].curhp;

                    if(group[0x13].exist)
                        group[i].multi_maxhp += group[0x13].maxhp;
                    if(group[0xC].exist)
                        group[i].multi_maxhp += group[0xC].maxhp;
                }else if(i == 0xF){
                    if(group[0x10].exist && (group[0x10].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[0x11].exist && (group[0x11].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[0xA].exist && (group[0xA].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[0x10].exist)
                        group[i].multi_hp += group[0x10].curhp;
                    if(group[0x11].exist)
                        group[i].multi_hp += group[0x11].curhp;
                    if(group[0xA].exist)
                        group[i].multi_hp += group[0xA].curhp;

                    if(group[0x10].exist)
                        group[i].multi_maxhp += group[0x10].maxhp;
                    if(group[0x11].exist)
                        group[i].multi_maxhp += group[0x11].maxhp;
                    if(group[0xA].exist)
                        group[i].multi_maxhp += group[0xA].maxhp;
                }else if(i == 0x10){
                    if(group[0xB].exist && (group[0xB].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[0xF].exist && (group[0xF].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[0x12].exist && (group[0x12].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[0xB].exist)
                        group[i].multi_hp += group[0xB].curhp;
                    if(group[0xF].exist)
                        group[i].multi_hp += group[0xF].curhp;
                    if(group[0x12].exist)
                        group[i].multi_hp += group[0x12].curhp;

                    if(group[0xB].exist)
                        group[i].multi_maxhp += group[0xB].maxhp;
                    if(group[0xF].exist)
                        group[i].multi_maxhp += group[0xF].maxhp;
                    if(group[0x12].exist)
                        group[i].multi_maxhp += group[0x12].maxhp;
                }else if(i == 0x11){
                    if(group[0xC].exist && (group[0xC].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[0xF].exist && (group[0xF].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[0x13].exist && (group[0x13].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[0xC].exist)
                        group[i].multi_hp += group[0xC].curhp;
                    if(group[0xF].exist)
                        group[i].multi_hp += group[0xF].curhp;
                    if(group[0x13].exist)
                        group[i].multi_hp += group[0x13].curhp;

                    if(group[0xC].exist)
                        group[i].multi_maxhp += group[0xC].maxhp;
                    if(group[0xF].exist)
                        group[i].multi_maxhp += group[0xF].maxhp;
                    if(group[0x13].exist)
                        group[i].multi_maxhp += group[0x13].maxhp;
                }else if(i == 0x12){
                    if(group[0x10].exist && (group[0x10].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[0xD].exist && (group[0xD].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[0x10].exist)
                        group[i].multi_hp += group[0x10].curhp;
                    if(group[0xD].exist)
                        group[i].multi_hp += group[0xD].curhp;

                    if(group[0x10].exist)
                        group[i].multi_maxhp += group[0x10].maxhp;
                    if(group[0xD].exist)
                        group[i].multi_maxhp += group[0xD].maxhp;
                }else if(i == 0x13){
                    if(group[0x11].exist && (group[0x11].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;
                    if(group[0xE].exist && (group[0xE].flags & FL_DEBUFF_ANY))
                        group[i].debuff ++;

                    if(group[0x11].exist)
                        group[i].multi_hp += group[0x11].curhp;
                    if(group[0xE].exist)
                        group[i].multi_hp += group[0xE].curhp;

                    if(group[0x11].exist)
                        group[i].multi_maxhp += group[0x11].maxhp;
                    if(group[0xE].exist)
                        group[i].multi_maxhp += group[0xE].maxhp;
                }

                //qDebug("pos %d debuff %X", i, group[i].debuff);
            }
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

bool CBattleWorker::CheckProtect()
{
    if(m_bLevelOneProtect)
    {
        for(int i = 0xA;i < 20; ++i)
        {
            if(m_BattleContext.m_UnitGroup[i].exist && m_BattleContext.m_UnitGroup[i].level == 1)
                return true;
        }
    }
    if(m_bBOSSProtect)
    {
        int bgm = 0;
        if(g_CGAInterface->GetBGMIndex(bgm) && bgm == 14)
        {
            return true;
        }
    }
    return false;
}

void CBattleWorker::OnNotifyBattleAction(int flags)
{
    int ingame = 0;

    //qDebug("OnNotifyBattleAction.");

    if(!g_CGAInterface->IsConnected() || !g_CGAInterface->IsInGame(ingame) || !ingame)
        return;

    if((flags & FL_BATTLE_ACTION_END) || (flags & FL_BATTLE_ACTION_BEGIN))
    {
        m_BattleContext.m_iLastRound = -1;
        m_BattleContext.m_bRoundZeroNotified = false;
        m_BattleContext.m_bIsPlayerActionPerformed = false;
        m_BattleContext.m_bIsPlayerEscaped = false;
        return;
    }

    int gameStatus = 0;
    if(!g_CGAInterface->GetGameStatus(gameStatus) && gameStatus != 10)
        return;

    CGA::cga_battle_context_t ctx;

    if(!g_CGAInterface->GetBattleContext(ctx))
        return;

    m_BattleContext.m_iPlayerPosition = ctx.player_pos;
    m_BattleContext.m_iPlayerStatus = ctx.player_status;
    m_BattleContext.m_iRoundCount = ctx.round_count;
    m_BattleContext.m_iPetId = ctx.petid;
    m_BattleContext.m_iWeaponAllowBit = ctx.weapon_allowbit;
    m_BattleContext.m_iSkillAllowBit = ctx.skill_allowbit;
    m_BattleContext.m_iPetSkillAllowBit = ctx.petskill_allowbit;

    if(m_BattleContext.m_iPlayerStatus != 1 && m_BattleContext.m_iPlayerStatus != 4)
     {
        //qDebug("m_iPlayerStatus =%d", m_BattleContext.m_iPlayerStatus);
        return;
    }

    m_BattleContext.m_bIsPlayer = (flags & FL_BATTLE_ACTION_ISPLAYER) ? true : false;
    m_BattleContext.m_bIsDouble = (flags & FL_BATTLE_ACTION_ISDOUBLE) ? true : false;
    m_BattleContext.m_bIsSkillPerformed = (flags & FL_BATTLE_ACTION_ISSKILLPERFORMED) ? true : false;

    if(m_BattleContext.m_iRoundCount == 0 && m_BattleContext.m_iLastRound != m_BattleContext.m_iRoundCount)
        m_BattleContext.m_bRoundZeroNotified = true;

    m_BattleContext.m_bIsPetDoubleAction = m_bPetDoubleAction;
    m_BattleContext.m_bIsPlayerForceAction = m_bPlayerForceAction;

    GetBattleUnits();

    if(m_bAutoBattle)
    {
        //qDebug("petid = %d m_bIsSkillPerformed = %d\n", m_BattleContext.m_iPetId, m_BattleContext.m_bIsSkillPerformed ? 1 : 0);

        if(CheckProtect()){
            //qDebug("Found Lv1 enemy, stopped.");
            return;
        }

        if(m_bNoSwitchAnim && m_BattleContext.m_iLastRound != m_BattleContext.m_iRoundCount )
        {
            int randDelay = qrand() * (m_iDelayTo - m_iDelayFrom) / RAND_MAX + m_iDelayFrom;
            if(randDelay < 1)
                randDelay = 1;
            if(randDelay > 10000)
                randDelay = 10000;

            if(!m_BattleContext.m_bRoundZeroNotified && m_BattleContext.m_iRoundCount == 1)
                randDelay += 4000;

            QTimer::singleShot( randDelay, this, SLOT(OnPerformanceBattle()) );
        }
        else if(((m_BattleContext.m_iRoundCount == 0 && !m_bFirstRoundNoDelay) || m_BattleContext.m_iRoundCount > 0) && m_BattleContext.m_iLastRound != m_BattleContext.m_iRoundCount)
        {
            int randDelay = qrand() * (m_iDelayTo - m_iDelayFrom) / RAND_MAX + m_iDelayFrom;
            if(randDelay < 1)
                randDelay = 1;
            if(randDelay > 10000)
                randDelay = 10000;

            QTimer::singleShot( randDelay, this, SLOT(OnPerformanceBattle()) );
        }
        else
        {
            OnPerformanceBattle();
        }

        //qDebug("m_iRoundCount %d m_iLastRound %d", m_BattleContext.m_iRoundCount, m_BattleContext.m_iLastRound);
    }

    if(m_BattleContext.m_iRoundCount != m_BattleContext.m_iLastRound)
    {
        m_BattleContext.m_iLastRound = m_BattleContext.m_iRoundCount;
        m_BattleContext.m_bIsPlayerActionPerformed = false;
        m_BattleContext.m_bIsPlayerEscaped = false;
    }
}

void CBattleWorker::OnNotifyAttachProcessOk(quint32 ProcessId, quint32 ThreadId, quint32 port, quint32 hWnd)
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
}

void CBattleWorker::OnSetFRND(int state)
{
    m_bFirstRoundNoDelay = state ? true : false;
}

void CBattleWorker::OnSetLv1Protect(int state)
{
    m_bLevelOneProtect = state ? true : false;
}

void CBattleWorker::OnSetBOSSProtect(int state)
{
    m_bBOSSProtect = state ? true : false;
}

void CBattleWorker::OnSetLockCountdown(int state)
{
    m_bLockCountdown = state ? true : false;
}

void CBattleWorker::OnSetNoSwitchAnim(int state)
{
    m_bNoSwitchAnim = state ? true : false;
}

void CBattleWorker::OnSetPetDoubleAction(int state)
{
    m_bPetDoubleAction = state ? true : false;
}

void CBattleWorker::OnSetPlayerForceAction(int state)
{
    m_bPlayerForceAction = state ? true : false;
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
