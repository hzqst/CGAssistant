#include "playerform.h"
#include "ui_playerform.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

PlayerForm::PlayerForm(CPlayerWorker *worker, CBattleWorker *bworker, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerForm), m_worker(worker)
{
    ui->setupUi(this);

    m_model_Pet = new QStandardItemModel(ui->treeView_pets);
    ui->treeView_pets->setModel(m_model_Pet);

    m_model_Skill = new QStandardItemModel(ui->treeView_skills);
    ui->treeView_skills->setModel(m_model_Skill);

    ui->label_spdvalue->setText(tr("%1 %").arg(ui->horizontalSlider_movespd->value()));
    ui->label_workdelayval->setText(tr("%1 ms").arg(ui->horizontalSlider_workdelay->value()));
    ui->label_workaccval->setText(tr("%1 %").arg(ui->horizontalSlider_workacc->value()));

    ui->comboBox_petFoodAt->addItem("75%");
    ui->comboBox_petFoodAt->addItem("50%");
    ui->comboBox_petFoodAt->addItem("25%");
    ui->comboBox_petFoodAt->addItem("1000");
    ui->comboBox_petFoodAt->addItem("500");
    ui->comboBox_petFoodAt->addItem("250");

    ui->comboBox_useFoodAt->addItem("75%");
    ui->comboBox_useFoodAt->addItem("50%");
    ui->comboBox_useFoodAt->addItem("25%");
    ui->comboBox_useFoodAt->addItem("1000");
    ui->comboBox_useFoodAt->addItem("500");
    ui->comboBox_useFoodAt->addItem("250");

    ui->comboBox_petMedAt->addItem("75%");
    ui->comboBox_petMedAt->addItem("50%");
    ui->comboBox_petMedAt->addItem("25%");
    ui->comboBox_petMedAt->addItem("1000");
    ui->comboBox_petMedAt->addItem("500");
    ui->comboBox_petMedAt->addItem("250");

    ui->comboBox_useMedAt->addItem("75%");
    ui->comboBox_useMedAt->addItem("50%");
    ui->comboBox_useMedAt->addItem("25%");
    ui->comboBox_useMedAt->addItem("1000");
    ui->comboBox_useMedAt->addItem("500");
    ui->comboBox_useMedAt->addItem("250");

    connect(ui->checkBox_SwitchAnim, SIGNAL(stateChanged(int)), bworker, SLOT(OnSetNoSwitchAnim(int)), Qt::QueuedConnection);
    connect(ui->checkBox_SwitchAnim, SIGNAL(stateChanged(int)), m_worker, SLOT(OnSetNoSwitchAnim(int)), Qt::QueuedConnection);
    connect(ui->checkBox_autoSupply, SIGNAL(stateChanged(int)), m_worker, SLOT(OnSetAutoSupply(int)), Qt::QueuedConnection);
    connect(ui->checkBox_useFood, SIGNAL(stateChanged(int)), m_worker, SLOT(OnSetUseFood(int)), Qt::QueuedConnection);
    connect(ui->checkBox_useMed, SIGNAL(stateChanged(int)), m_worker, SLOT(OnSetUseMed(int)), Qt::QueuedConnection);
    connect(ui->checkBox_petFood, SIGNAL(stateChanged(int)), m_worker, SLOT(OnSetPetFood(int)), Qt::QueuedConnection);
    connect(ui->checkBox_petMed, SIGNAL(stateChanged(int)), m_worker, SLOT(OnSetPetMed(int)), Qt::QueuedConnection);
    connect(ui->comboBox_useFoodAt, SIGNAL(currentTextChanged(QString)), m_worker, SLOT(OnSetUseFoodAt(QString)), Qt::QueuedConnection);
    connect(ui->comboBox_useMedAt, SIGNAL(currentTextChanged(QString)), m_worker, SLOT(OnSetUseMedAt(QString)), Qt::QueuedConnection);
    connect(ui->comboBox_petFoodAt, SIGNAL(currentTextChanged(QString)), m_worker, SLOT(OnSetPetFoodAt(QString)), Qt::QueuedConnection);
    connect(ui->comboBox_petMedAt, SIGNAL(currentTextChanged(QString)), m_worker, SLOT(OnSetPetMedAt(QString)), Qt::QueuedConnection);
    connect(ui->horizontalSlider_movespd, SIGNAL(valueChanged(int)), m_worker, SLOT(OnSetMoveSpeed(int)), Qt::ConnectionType::QueuedConnection);
    connect(ui->horizontalSlider_movespd, SIGNAL(valueChanged(int)), this, SLOT(OnSetMoveSpeed(int)));
    connect(ui->horizontalSlider_workdelay, SIGNAL(valueChanged(int)), m_worker, SLOT(OnSetWorkDelay(int)), Qt::ConnectionType::QueuedConnection);
    connect(ui->horizontalSlider_workdelay, SIGNAL(valueChanged(int)), this, SLOT(OnSetWorkDelay(int)));
    connect(ui->horizontalSlider_workacc, SIGNAL(valueChanged(int)), m_worker, SLOT(OnSetWorkAcc(int)), Qt::ConnectionType::QueuedConnection);
    connect(ui->horizontalSlider_workacc, SIGNAL(valueChanged(int)), this, SLOT(OnSetWorkAcc(int)));
    connect(m_worker, &CPlayerWorker::NotifyGetInfoFailed, this, &PlayerForm::OnNotifyGetInfoFailed, Qt::QueuedConnection);
    connect(m_worker, &CPlayerWorker::NotifyGetPlayerInfo, this, &PlayerForm::OnNotifyGetPlayerInfo, Qt::QueuedConnection);
    connect(m_worker, &CPlayerWorker::NotifyGetPetsInfo, this, &PlayerForm::OnNotifyGetPetsInfo, Qt::QueuedConnection);
    connect(m_worker, &CPlayerWorker::NotifyGetSkillsInfo, this, &PlayerForm::OnNotifyGetSkillsInfo, Qt::QueuedConnection);
    connect(m_worker, &CPlayerWorker::NotifyGetMapInfo, this, &PlayerForm::OnNotifyGetMapInfo, Qt::QueuedConnection);

    ui->comboBox_useFoodAt->setCurrentText("0");
    ui->comboBox_useMedAt->setCurrentText("0");
    ui->comboBox_petFoodAt->setCurrentText("0");
    ui->comboBox_petMedAt->setCurrentText("0");
}

PlayerForm::~PlayerForm()
{
    delete ui;
}

void PlayerForm::OnCloseWindow()
{

}

void PlayerForm::OnSetMoveSpeed(int value)
{
    ui->label_spdvalue->setText(tr("%1 %").arg(value));
}

void PlayerForm::OnSetWorkDelay(int value)
{
    ui->label_workdelayval->setText(tr("%1 ms").arg(value));
}

void PlayerForm::OnSetWorkAcc(int value)
{
    ui->label_workaccval->setText(tr("%1 %").arg(value));
}

void PlayerForm::OnNotifyGetPetsInfo(QSharedPointer<CGA_PetList_t> pets)
{
    if(pets->empty())
        return;

    QString str;
    for(int i = 0; i < pets->size(); ++i)
    {
        const CGA_PetInfo_t &pet = pets->at(i);

        if(!pet.name.isEmpty())
            str = QString("Lv %1 %2 (%3)").arg(QString::number(pet.level), pet.realname, pet.name);
        else
            str = QString("Lv %1 %2").arg(QString::number(pet.level), pet.realname);

#if 0
            str += QString(" f=%1").arg(pet.battle_flags,0,16);
#endif

        QStandardItem *item = nullptr;

        if(i < m_model_Pet->rowCount()) {
            item = m_model_Pet->item(i);
            if( str != item->text() )
                item->setText( str );
        } else {
            item = new QStandardItem(str);
            m_model_Pet->insertRow(i, item);
        }

        str = QObject::tr("HP: %1 / %2").arg(pet.hp).arg(pet.maxhp);
        if(item->rowCount() >= 1){
            if( str != item->child(0)->text() )
                item->child(0)->setText(str);
        } else {
            item->insertRow(0, new QStandardItem(str));
        }

        str = QObject::tr("MP: %1 / %2").arg(pet.mp).arg(pet.maxmp);
        if(item->rowCount() >= 2){
            if( str != item->child(1)->text() )
                item->child(1)->setText(str);
        } else {
            item->insertRow(1, new QStandardItem(str));
        }

        str = QObject::tr("XP: %1 / %2").arg(pet.xp).arg(pet.maxxp);
        if(item->rowCount() >= 3){
            if( str != item->child(2)->text() )
                item->child(2)->setText(str);
        } else {
            item->insertRow(2, new QStandardItem(str));
        }

        str = QObject::tr("Upgrade: %1").arg(pet.maxxp-pet.xp);
        if(item->rowCount() >= 4){
            if( str != item->child(3)->text() )
                item->child(3)->setText(str);
        } else {
            item->insertRow(3, new QStandardItem(str));
        }

        str = QObject::tr("Loyality: %1").arg(pet.loyality);
        if(item->rowCount() >= 5){
            if( str != item->child(4)->text() )
                item->child(4)->setText(str);
        } else {
            item->insertRow(4, new QStandardItem(str));
        }

        for(int j = 0;j < pet.skills.size(); ++j)
        {
            const CGA_PetSkillInfo_t &sk = pet.skills.at(j);
            str = QString("%1 MP %2").arg( sk.name, QString::number(sk.cost) );
            if(item->rowCount() >= 6+j){
                if( str != item->child(5+j)->text() )
                    item->child(5+j)->setText(str);
            } else {
                item->insertRow(5+j, new QStandardItem(str));
            }
        }
    }
    if(m_model_Pet->rowCount() > pets->size())
    {
        m_model_Pet->removeRows(pets->size(), m_model_Pet->rowCount() - pets->size());
    }
}

void PlayerForm::OnNotifyGetSkillsInfo(QSharedPointer<CGA_SkillList_t> skills)
{
    if(skills->empty())
        return;

    QString str;
    for(int i = 0; i < skills->size(); ++i)
    {
        const CGA_SkillInfo_t &skill = skills->at(i);

        if(skill.lv == skill.maxlv)
            str = QString("%1 Lv %2").arg(skill.name, QString::number(skill.lv));
        else
            str = QString("%1 Lv %2 / %3").arg(skill.name, QString::number(skill.lv), QString::number(skill.maxlv));

        QStandardItem *item = nullptr;

        if(i < m_model_Skill->rowCount()) {
            item = m_model_Skill->item(i);
            if( str != item->text() )
                item->setText( str );
        } else {
            item = new QStandardItem(str);
            m_model_Skill->insertRow(i, item);
        }

        if(skill.lv == skill.maxlv)
            str = QString("XP: %1").arg(skill.xp);
        else
            str = QString("XP: %1 / %2").arg(skill.xp).arg(skill.maxxp);
        if(item->rowCount() >= 1){
            if( str != item->child(0)->text() )
                item->child(0)->setText(str);
        } else {
            item->insertRow(0, new QStandardItem(str));
        }

        for(int j = 0;j < skill.subskills.size(); ++j)
        {
            const CGA_SubSkill_t &subskill = skill.subskills.at(j);

            str = QString("%1 MP %2").arg(subskill.name).arg(subskill.cost);
            if(item->rowCount() >= j + 2){
                if( str != item->child(j + 1)->text() )
                    item->child(j + 1)->setText(str);
            } else {
                item->insertRow(j + 1, new QStandardItem(str));
            }
        }
    }
    if(m_model_Skill->rowCount() > skills->size())
    {
        m_model_Skill->removeRows(skills->size(), m_model_Skill->rowCount() - skills->size());
    }
}

void PlayerForm::OnNotifyGetMapInfo(QString name, int index1, int index2, int index3, int x, int y, int worldStatus, int gameStatus)
{
    if(name != ui->lineEdit_map->text())
        ui->lineEdit_map->setText(name);

    QString str = QString("%1, %2, %3 (%4, %5)").arg(x).arg(y).arg(index3).arg(worldStatus).arg(gameStatus);
    if(str != ui->lineEdit_mapxy->text())
        ui->lineEdit_mapxy->setText(str);
}

void PlayerForm::OnNotifyGetPlayerInfo(QSharedPointer<CGA_PlayerInfo_t> player)
{
    if(player->level == 0)
        return;

    if(ui->lineEdit_playerName->text() != player->name)
        ui->lineEdit_playerName->setText(player->name);

    if(ui->lineEdit_job->text() != player->job)
        ui->lineEdit_job->setText(player->job);

    QString str;

    str = QString::number(player->level);
    if(ui->lineEdit_level->text() != str)
        ui->lineEdit_level->setText(str);

    str = QString::number(player->gold);
    if(ui->lineEdit_gold->text() != str)
        ui->lineEdit_gold->setText(str);

    str = FormatPunchClock(player->punchclock);
    if(ui->lineEdit_punchClock->text() != str)
        ui->lineEdit_punchClock->setText(str);

    if(player->usingpunchclock)
        ui->lineEdit_punchClock->setStyleSheet("{color:red;}");
    else
         ui->lineEdit_punchClock->setStyleSheet("");

    str = QString("%1 / %2").arg(player->hp).arg(player->maxhp);
    if(ui->lineEdit_hp->text() != str)
        ui->lineEdit_hp->setText(str);

    str = QString("%1 / %2").arg(player->mp).arg(player->maxmp);
    if(ui->lineEdit_mp->text() != str)
        ui->lineEdit_mp->setText(str);

    str = QString("%1 / %2").arg(player->xp).arg(player->maxxp);
    if(ui->lineEdit_xp->text() != str)
        ui->lineEdit_xp->setText(str);

    str = QString::number(player->maxxp - player->xp);
    if(ui->lineEdit_xp_remain->text() != str)
        ui->lineEdit_xp_remain->setText(str);
}

void PlayerForm::OnNotifyGetInfoFailed(bool bIsConnected, bool bIsInGame)
{
    ClearMapInfo(bIsConnected, bIsInGame);
    ClearPlayerInfo();
}

QString PlayerForm::FormatPunchClock(int val)
{
    val /= 1000;
    int hours = val / 3600;
    int mins = (val - hours * 3600) / 60;
    return QString("%1:%2").arg(hours).arg(mins, 2);
}

void PlayerForm::ClearMapInfo(bool bIsConnected, bool bIsInGame)
{
    QString str;

    if(!bIsConnected)
        str = tr("= Not connected =");
    else if(!bIsInGame)
        str = tr("= Not in game =");
    if(str != ui->lineEdit_map->text())
        ui->lineEdit_map->setText(str);
    if(!ui->lineEdit_mapxy->text().isEmpty())
        ui->lineEdit_mapxy->clear();
}

void PlayerForm::ClearPlayerInfo()
{
    if(!ui->lineEdit_playerName->text().isEmpty())
        ui->lineEdit_playerName->clear();
    if(!ui->lineEdit_job->text().isEmpty())
        ui->lineEdit_job->clear();
    if(!ui->lineEdit_level->text().isEmpty())
        ui->lineEdit_level->clear();
    if(!ui->lineEdit_gold->text().isEmpty())
        ui->lineEdit_gold->clear();
    if(!ui->lineEdit_hp->text().isEmpty())
        ui->lineEdit_hp->clear();
    if(!ui->lineEdit_mp->text().isEmpty())
        ui->lineEdit_mp->clear();
    if(!ui->lineEdit_xp->text().isEmpty())
        ui->lineEdit_xp->clear();
    if(!ui->lineEdit_xp_remain->text().isEmpty())
        ui->lineEdit_xp_remain->clear();

    m_model_Pet->clear();
    m_model_Skill->clear();
}

void PlayerForm::SaveSettings(QJsonDocument &doc)
{
    QJsonObject obj;

    QJsonObject player;
    player.insert("noswitchanim", ui->checkBox_SwitchAnim->isChecked());
    player.insert("autosupply", ui->checkBox_autoSupply->isChecked());

    player.insert("usefood", ui->checkBox_useFood->isChecked());
    player.insert("usemed", ui->checkBox_useMed->isChecked());
    player.insert("petfood", ui->checkBox_petFood->isChecked());
    player.insert("petmed", ui->checkBox_petMed->isChecked());

    player.insert("usefoodat", ui->comboBox_useFoodAt->currentText());
    player.insert("usemedat", ui->comboBox_useMedAt->currentText());
    player.insert("petfoodat", ui->comboBox_petFoodAt->currentText());
    player.insert("petmedat", ui->comboBox_petMedAt->currentText());

    player.insert("movespd", ui->horizontalSlider_movespd->value());
    player.insert("workacc", ui->horizontalSlider_workacc->value());
    player.insert("workdelay", ui->horizontalSlider_workdelay->value());

    obj.insert("player", player);

    QJsonObject battle;
    SaveBattleSettings(battle);
    obj.insert("battle", battle);

    QJsonObject idmap;
    SaveItemIdMap(idmap);
    obj.insert("itemidmap", idmap);

    QJsonArray drop;
    SaveItemDropper(drop);
    obj.insert("itemdroplist", drop);

    QJsonArray tweak;
    SaveItemTweaker(tweak);
    obj.insert("itemtweaklist", tweak);

    QJsonObject chat;
    SaveChatSettings(chat);
    obj.insert("chat", chat);

    doc.setObject(obj);
}

void PlayerForm::on_pushButton_save_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("Save player settings"));
    fileDialog->setDirectory(".");
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    fileDialog->setNameFilter(tr("json files(*.json)"));
    fileDialog->setFileMode(QFileDialog::AnyFile);
    if(fileDialog->exec() == QDialog::Accepted)
    {
        QString filePath = fileDialog->selectedFiles()[0];
        QFile file(filePath);
        if(file.open(QFile::WriteOnly | QFile::Text))
        {
            QJsonDocument doc;
            SaveSettings(doc);
            file.write(doc.toJson());
            file.close();
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to save settings file.\nerror: %1").arg(file.errorString()), QMessageBox::Ok, 0);
        }
    }
}

bool PlayerForm::ParsePlayerSettings(const QJsonValue &val)
{
    if(!val.isObject())
        return false;

    auto playerobj = val.toObject();

   ui->checkBox_SwitchAnim->setChecked(playerobj.take("noswitchanim").toBool());
   ui->checkBox_autoSupply->setChecked(playerobj.take("autosupply").toBool());

   ui->checkBox_useFood->setChecked(playerobj.take("usefood").toBool());
   ui->checkBox_useMed->setChecked(playerobj.take("usemed").toBool());
   ui->checkBox_petFood->setChecked(playerobj.take("petfood").toBool());
   ui->checkBox_petMed->setChecked(playerobj.take("petmed").toBool());

   ui->comboBox_useFoodAt->setCurrentText(playerobj.take("usefoodat").toString());
   ui->comboBox_useMedAt->setCurrentText(playerobj.take("usemedat").toString());
   ui->comboBox_petFoodAt->setCurrentText(playerobj.take("petfoodat").toString());
   ui->comboBox_petMedAt->setCurrentText(playerobj.take("petmedat").toString());

   ui->horizontalSlider_movespd->setValue(playerobj.take("movespd").toInt());
   ui->horizontalSlider_workacc->setValue(playerobj.take("workacc").toInt());
   ui->horizontalSlider_workdelay->setValue(playerobj.take("workdelay").toInt());

    return true;
}

bool PlayerForm::ParseSettings(const QByteArray &data, QJsonDocument &doc)
{
    QJsonParseError err;
    doc = QJsonDocument::fromJson(data, &err);
    if(err.error != QJsonParseError::NoError)
        return false;

    if(!doc.isObject())
        return false;

    QJsonObject obj = doc.object();
    if(obj.contains("player"))
    {
        ParsePlayerSettings(obj.take("player"));
    }
    if(obj.contains("battle"))
    {
        ParseBattleSettings(obj.take("battle"));
    }
    if(obj.contains("itemidmap"))
    {
        ParseItemIdMap(obj.take("itemidmap"));
    }
    if(obj.contains("itemdroplist"))
    {
        ParseItemDropper(obj.take("itemdroplist"));
    }
    if(obj.contains("itemtweaklist"))
    {
        ParseItemTweaker(obj.take("itemtweaklist"));
    }
    if(obj.contains("chat"))
    {
        ParseChatSettings(obj.take("chat"));
    }
    return true;
}

void PlayerForm::on_pushButton_load_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("Load player settings"));
    fileDialog->setDirectory(".");
    fileDialog->setNameFilter(tr("json files(*.json)"));
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    if(fileDialog->exec() == QDialog::Accepted)
    {
        QString filePath = fileDialog->selectedFiles()[0];
        QFile file(filePath);
        QJsonDocument doc;
        bool bSuccess = false;
        if(file.open(QFile::ReadOnly | QFile::Text))
        {
            bSuccess = ParseSettings(file.readAll(), doc);
            file.close();
        }
        if(file.error() != QFile::FileError::NoError)
            QMessageBox::critical(this, tr("Error"), tr("Failed to load item settings from file."), QMessageBox::Ok, 0);
        else if(!bSuccess)
            QMessageBox::critical(this, tr("Error"), tr("Failed to load item settings with invalid format."), QMessageBox::Ok, 0);
    }
}

void PlayerForm::OnNotifyFillLoadSettings(QString path)
{
    if(!path.isEmpty())
    {
        QFile file(path);
        if(file.exists())
        {
            QJsonDocument doc;
            if(file.open(QFile::ReadOnly | QFile::Text))
            {
                ParseSettings(file.readAll(), doc);
                file.close();
            }
        }
    }
}


void PlayerForm::OnHttpGetSettings(QJsonDocument *doc)
{
    SaveSettings(*doc);

    doc->object().insert("errcode", 0);
}

void PlayerForm::OnHttpLoadSettings(QString query, QByteArray postdata, QJsonDocument* doc)
{
    QJsonObject obj;

    QJsonDocument newdoc;
    if(ParseSettings(postdata, newdoc))
    {
       obj.insert("errcode", 0);
    }
    else
    {
       obj.insert("errcode", 1);
       obj.insert("message", tr("json parse error"));
    }

   doc->setObject(obj);
}
