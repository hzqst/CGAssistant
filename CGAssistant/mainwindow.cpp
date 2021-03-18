#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "processform.h"
#include "playerform.h"
#include "scriptform.h"
#include "autobattleform.h"
#include "accountform.h"
#include "itemform.h"
#include "mapform.h"
#include "chatform.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    auto playerWorker = new CPlayerWorker();
    playerWorker->moveToThread(&m_playerWorkerThread);
    connect(&m_playerWorkerThread, SIGNAL(finished()), playerWorker, SLOT(deleteLater()));

    auto processWorker = new CProcessWorker();
    processWorker->moveToThread(&m_processWorkerThread);
    connect(&m_processWorkerThread, SIGNAL(finished()), processWorker, SLOT(deleteLater()));

    auto battleWorker = new CBattleWorker();
    battleWorker->moveToThread(&m_battleWorkerThread);
    connect(&m_battleWorkerThread, SIGNAL(finished()), battleWorker, SLOT(deleteLater()));

    ProcessForm *processFrom = new ProcessForm(processWorker, this);
    connect(this, SIGNAL(NotifyCloseWindow()), processFrom, SLOT(OnCloseWindow()));
    ui->tabWidget->addTab(processFrom, QIcon(), tr("Attach"));

    PlayerForm *playerFrom = new PlayerForm(playerWorker, battleWorker, this);
    connect(this, SIGNAL(NotifyCloseWindow()), playerFrom, SLOT(OnCloseWindow()));
    ui->tabWidget->addTab(playerFrom, QIcon(), tr("Player"));

    AutoBattleForm *autoBattleForm = new AutoBattleForm(battleWorker, playerWorker, this);
    connect(this, SIGNAL(NotifyCloseWindow()), autoBattleForm, SLOT(OnCloseWindow()));
    ui->tabWidget->addTab(autoBattleForm, QIcon(), tr("Auto Battle"));

    ItemForm *itemForm = new ItemForm(playerWorker, this);
    connect(this, SIGNAL(NotifyCloseWindow()), itemForm, SLOT(OnCloseWindow()));
    ui->tabWidget->addTab(itemForm, QIcon(), tr("Item"));

    MapForm *mapForm = new MapForm(this);
    connect(this, SIGNAL(NotifyCloseWindow()), mapForm, SLOT(OnCloseWindow()));
    ui->tabWidget->addTab(mapForm, QIcon(), tr("Map"));

    ScriptForm *scriptForm = new ScriptForm(this);
    connect(this, SIGNAL(NotifyCloseWindow()), scriptForm, SLOT(OnCloseWindow()));
    ui->tabWidget->addTab(scriptForm, QIcon(), tr("Script"));

    AccountForm *accountForm = new AccountForm(this);
    ui->tabWidget->addTab(accountForm, QIcon(), tr("Account"));

    ChatForm *chatForm = new ChatForm(this);
    ui->tabWidget->addTab(chatForm, QIcon(), tr("Chat"));

    autoBattleForm->SyncAutoBattleWorker();

    connect(mapForm, &MapForm::runNavigatorScript, scriptForm, &ScriptForm::RunNavigatorScript);
    connect(mapForm, &MapForm::stopNavigatorScript, scriptForm, &ScriptForm::StopNavigatorScript);
    connect(scriptForm, &ScriptForm::ReportNavigatorPath, mapForm, &MapForm::OnReportNavigatorPath);
    connect(scriptForm, &ScriptForm::ReportNavigatorFinish, mapForm, &MapForm::OnReportNavigatorFinish);
    connect(playerFrom, &PlayerForm::ParseItemTweaker, itemForm, &ItemForm::ParseItemTweaker);
    connect(playerFrom, &PlayerForm::ParseItemDropper, itemForm, &ItemForm::ParseItemDropper);
    connect(playerFrom, &PlayerForm::ParseItemIdMap, itemForm, &ItemForm::ParseItemIdMap);
    connect(playerFrom, &PlayerForm::ParseBattleSettings, autoBattleForm, &AutoBattleForm::ParseBattleSettings);
    connect(playerFrom, &PlayerForm::ParseChatSettings, chatForm, &ChatForm::ParseChatSettings);
    connect(playerFrom, &PlayerForm::SaveItemIdMap, itemForm, &ItemForm::SaveItemIdMap);
    connect(playerFrom, &PlayerForm::SaveItemDropper, itemForm, &ItemForm::SaveItemDropper);
    connect(playerFrom, &PlayerForm::SaveItemTweaker, itemForm, &ItemForm::SaveItemTweaker);
    connect(playerFrom, &PlayerForm::SaveBattleSettings, autoBattleForm, &AutoBattleForm::SaveBattleSettings);
    connect(playerFrom, &PlayerForm::SaveChatSettings, chatForm, &ChatForm::SaveChatSettings);
    connect(this, &MainWindow::NotifyChangeWindow, processFrom, &ProcessForm::OnNotifyChangeWindow);

    connect(this, &MainWindow::NotifyFillAutoLogin, accountForm, &AccountForm::OnNotifyFillAutoLogin);
    connect(this, &MainWindow::NotifyFillLoadScript, scriptForm, &ScriptForm::OnNotifyFillLoadScript);
    connect(this, &MainWindow::NotifyFillLoadSettings, playerFrom, &PlayerForm::OnNotifyFillLoadSettings);
    connect(this, &MainWindow::NotifyFillMaxFreezeTime, processWorker, &CProcessWorker::OnNotifyFillMaxFreezeTime);

    connect(playerWorker, &CPlayerWorker::NotifyGetSkillsInfo, autoBattleForm, &AutoBattleForm::OnNotifyGetSkillsInfo, Qt::ConnectionType::QueuedConnection);
    connect(playerWorker, &CPlayerWorker::NotifyGetPetsInfo, autoBattleForm, &AutoBattleForm::OnNotifyGetPetsInfo, Qt::ConnectionType::QueuedConnection);
    connect(playerWorker, &CPlayerWorker::NotifyGetItemsInfo, autoBattleForm, &AutoBattleForm::OnNotifyGetItemsInfo, Qt::ConnectionType::QueuedConnection);
    connect(playerWorker, &CPlayerWorker::NotifyGetSkillsInfo, battleWorker, &CBattleWorker::OnNotifyGetSkillsInfo, Qt::ConnectionType::QueuedConnection);
    connect(playerWorker, &CPlayerWorker::NotifyGetPetsInfo, battleWorker, &CBattleWorker::OnNotifyGetPetsInfo, Qt::ConnectionType::QueuedConnection);
    connect(playerWorker, &CPlayerWorker::NotifyGetItemsInfo, battleWorker, &CBattleWorker::OnNotifyGetItemsInfo, Qt::ConnectionType::QueuedConnection);
    connect(playerWorker, &CPlayerWorker::NotifyGetMapCellInfo, mapForm, &MapForm::OnNotifyGetMapCellInfo, Qt::ConnectionType::QueuedConnection);
    connect(playerWorker, &CPlayerWorker::NotifyGetMapInfo, mapForm, &MapForm::OnNotifyGetMapInfo, Qt::ConnectionType::QueuedConnection);
    connect(playerWorker, &CPlayerWorker::NotifyRefreshMapRegion, mapForm, &MapForm::OnNotifyRefreshMapRegion, Qt::ConnectionType::QueuedConnection);
    connect(mapForm, &MapForm::RequestDownloadMap, playerWorker, &CPlayerWorker::OnDownloadMap, Qt::ConnectionType::QueuedConnection);
    connect(processWorker, &CProcessWorker::NotifyAttachProcessOk, battleWorker, &CBattleWorker::OnNotifyAttachProcessOk, Qt::QueuedConnection);
    connect(processWorker, &CProcessWorker::NotifyAttachProcessOk, scriptForm, &ScriptForm::OnNotifyAttachProcessOk, Qt::QueuedConnection);
    connect(processWorker, &CProcessWorker::NotifyAttachProcessOk, playerWorker, &CPlayerWorker::OnNotifyAttachProcessOk, Qt::QueuedConnection);
    connect(ui->tabWidget, &QTabWidget::currentChanged, playerWorker, &CPlayerWorker::OnTabChanged, Qt::QueuedConnection);
    connect(playerWorker, &CPlayerWorker::NotifyGetInfoFailed, this, &MainWindow::OnNotifyGetInfoFailed, Qt::ConnectionType::QueuedConnection);
    connect(playerWorker, &CPlayerWorker::NotifyGetPlayerInfo, this, &MainWindow::OnNotifyGetPlayerInfo, Qt::ConnectionType::QueuedConnection);
    connect(playerWorker, &CPlayerWorker::NotifyGetPlayerInfo, chatForm, &ChatForm::OnNotifyGetPlayerInfo, Qt::ConnectionType::QueuedConnection);
    connect(playerWorker, &CPlayerWorker::NotifyChatMsg, chatForm, &ChatForm::OnNotifyChatMsg, Qt::ConnectionType::QueuedConnection);
    connect(playerWorker, &CPlayerWorker::NotifyConnectionState, accountForm, &AccountForm::OnNotifyConnectionState, Qt::ConnectionType::QueuedConnection);
    connect(accountForm, &AccountForm::NotifyKillProcess, processWorker, &CProcessWorker::OnKillProcess);
    connect(accountForm, &AccountForm::NotifyAutoAttachProcess, processWorker, &CProcessWorker::OnAutoAttachProcess, Qt::QueuedConnection);

    connect(this, &MainWindow::HttpGetGameProcInfo, processWorker, &CProcessWorker::OnHttpGetGameProcInfo, Qt::DirectConnection);
    connect(this, &MainWindow::HttpGetSettings, playerFrom, &PlayerForm::OnHttpGetSettings);
    connect(this, &MainWindow::HttpLoadSettings, playerFrom, &PlayerForm::OnHttpLoadSettings);
    connect(this, &MainWindow::HttpLoadScript, scriptForm, &ScriptForm::OnHttpLoadScript);
    connect(this, &MainWindow::HttpLoadAccount, accountForm, &AccountForm::OnHttpLoadAccount);

    m_playerWorkerThread.start();
    m_processWorkerThread.start();
    m_battleWorkerThread.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_playerWorkerThread.quit();
    m_playerWorkerThread.wait();

    m_processWorkerThread.quit();
    m_processWorkerThread.wait();

    m_battleWorkerThread.quit();
    m_battleWorkerThread.wait();

    NotifyCloseWindow();
}

void MainWindow::changeEvent(QEvent *event)
{
    if(event->type() != QEvent::WindowStateChange)
        return;

    NotifyChangeWindow(windowState());
}

void MainWindow::on_tabWidget_currentChanged(int index)
{

}

void MainWindow::OnNotifyGetPlayerInfo(QSharedPointer<CGA_PlayerInfo_t> player)
{
    if(player->serverindex != -1 && !player->name.isEmpty())
        setWindowTitle(tr("CGAssistant [%1] (server %2)").arg(player->name).arg(player->serverindex));
}

void MainWindow::OnNotifyGetInfoFailed(bool bIsConnected, bool bIsInGame)
{
    setWindowTitle(tr("CGAssistant"));
}
