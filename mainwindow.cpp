//Copyright (c) 2021 Erik Leth Danielsen Meldco.dk
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QTimer>
#include <QIcon>

//Remember to run ~$ pulseaudio --kill //at startup else KDE volume controll dosent work, when (paprefs Simultaneous Output) //put it in /home/eld/.config/autostart/xx.desktop

//and sudo apt install qtmultimedia5-dev

//utelizing GStreamer
//https://gstreamer.freedesktop.org/documentation/frequently-asked-questions/using.html?gi-language=c
//:~$ gst-launch-1.0 playbin uri=http://94.228.133.3:80

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    LoadSettings();
    loadStations();

    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
    trayIcon->show();

    m_player = new QMediaPlayer; // Initialize the player (this)

    m_player->setVolume(100); // Set the volume for playing tracks
    trayIcon->setIcon(QIcon(":/images/radio_g.png"));

    connect(m_player, &QMediaPlayer::mediaStatusChanged,this, [&](QMediaPlayer::MediaStatus status)
    {
        qDebug()<<"state"<<status;
        if(status == QMediaPlayer::LoadedMedia) stateStop();
        if(status == QMediaPlayer::LoadingMedia) stateStop();
        if(status == QMediaPlayer::BufferedMedia) statePlay();
    });
    qDebug()<<"ready";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::LoadSettings()
{
    QSettings setting("meldco","radiotray-qt");
    QString x,y;
    setting.beginGroup("MainWindow");
    x="";
    x+="DK DR_P1,https://drliveradio.akamaized.net/hls/live/2022411/p1/masterab.m3u8\n";
    x+="DK Radio4,http://netradio.radio4.dk/radio4\n";
    x+="NL Radio STAD DEN HAAG,http://94.228.133.3:80\n";
    x+="DE 80er,https://tuner.m1.fm/80er.mp3\n";
    x+="DE 80er Hits,https://stream.antenne.de/80er-kulthits/stream/mp3?aw_0_1st.playerid=radio.de\n";
    x+="DE RSH,http://streams.rsh.de/rsh-live/mp3-128/streams.rsh.de\n";

    //ui->lineEdit->setText(setting.value("path","").toString());
    y=setting.value("stations","").toString();
    setting.endGroup();
    if(y.isEmpty())y=x;
    ui->textEdit->setLineWrapMode(QTextEdit::NoWrap);
    ui->textEdit->setText(y);
    SettingStations=y;
    qDebug() << setting.fileName();
}
void MainWindow::SaveSettings()
{
    QSettings setting("meldco","radiotray-qt");
    setting.beginGroup("MainWindow");
    SettingStations=ui->textEdit->toPlainText();
    setting.setValue("stations",SettingStations);
    setting.endGroup();
}

void MainWindow::on_pushButton_clicked()
{
    SaveSettings();
    this->hide();
}

void MainWindow::tstart()
{
    QMessageBox::information(this,tr("Title 1"), "teststart");
}

void MainWindow::about()
{
    QMessageBox::information(this,tr("About"), "Designed by ELD meldco.dk<br/><a href='https://www.meldco.dk'>www.meldco.dk</a>");
}

void MainWindow::stateStop()
{
    state=0;
    trayIcon->setIcon(QIcon(":/images/radio_g.png"));
}

void MainWindow::statePlay()
{
    trayIcon->setIcon(QIcon(":/images/radio_o.png"));
    if(state==0){
        //trayIcon->showMessage("Info",trayIcon->toolTip(),QSystemTrayIcon::Information,3000);//QIcon(":/images/radio_o.png")
    }
    state=1;
}

void MainWindow::mute()
{
    if (m_player->state()==m_player->PlayingState)
    {
        m_player->stop();
    }
    else {
        m_player->play();
    }
}

//https://stackoverflow.com/questions/52575175/how-to-use-qmenu-signals
void MainWindow::play(){
    if(QAction *menu = qobject_cast<QAction *>(sender()))
    {
        qDebug()<<"aboutToShow" << menu->text();
        qDebug()<<"aboutToShowd" << menu->data().toString();
        trayIcon->setToolTip("Radio Tray QT<br/><font size='-1'>" + menu->text() + "</font>");
        m_player->stop();
        m_player->setMedia(QUrl(menu->data().toString()));
        m_player->play();
    }
}

void MainWindow::loadStations()
{
    QAction *a;
    QStringList stations,stationx;
    trayIconMenu = new QMenu(this);

    a = new QAction(tr("About"), this);
    connect(a, &QAction::triggered, this, &MainWindow::about);
    trayIconMenu->addAction(a);
    trayIconMenu->setToolTipsVisible(true);

    stations=SettingStations.split("\n");
    foreach(QString ss, stations){
        stationx=ss.split(",");
        if(stationx.length()==2){
        a = new QAction(stationx.at(0), this);
        a->setData(stationx.at(1));
        a->setToolTip(a->data().toString());
        connect(a, &QAction::triggered, this, &MainWindow::play);
        trayIconMenu->addAction(a);
        }
    }

    a = new QAction(tr("&Mute"), this);
    a->setData("xMute");
    connect(a, &QAction::triggered, this, &MainWindow::mute);//::ee
    trayIconMenu->addAction(a);

    a = new QAction(tr("&Quit"), this);
    connect(a, &QAction::triggered, qApp, &QCoreApplication::quit);
    trayIconMenu->addAction(a);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        //QWidget::showNormal();
        //QWidget::raise();
        //QWidget::show();
        //QWidget::setWindowState(Qt::WindowActive);
        //this->raise();
        //this->setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        //this->setWindowFlags(Qt::WindowStaysOnBottomHint);
        if(this->isHidden()){
            this->setWindowFlags(Qt::WindowStaysOnTopHint);
            this->setWindowState(Qt::WindowActive);
            this->show();
        }
        else{
            this->hide();
        }
        //this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
        //this->setWindowFlags(Qt::WindowStaysOnTopHint);
        //this->setWindowState(Qt::WindowState::WindowActive);

        //iconComboBox->setCurrentIndex((iconComboBox->currentIndex() + 1) % iconComboBox->count());
        break;
    case QSystemTrayIcon::MiddleClick:
        //showMessage();
        break;
    default:
        ;
    }
}


