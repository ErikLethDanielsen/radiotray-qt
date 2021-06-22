#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QMediaPlayer>
#include <QMenu>

#include <QHelpEvent>
#include <QToolTip>

class QAction;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void mute();

    void about();

    void statePlay();

    void stateStop();

    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    //https://stackoverflow.com/questions/52575175/how-to-use-qmenu-signals
    void play();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    void tstart();
    void loadStations();
    void SaveSettings();
    void LoadSettings();
    int state =0;

    QString SettingStations;
    QSystemTrayIcon *trayIcon;
    QMediaPlayer *m_player;
    QMenu *trayIconMenu;
};


#endif // MAINWINDOW_H
