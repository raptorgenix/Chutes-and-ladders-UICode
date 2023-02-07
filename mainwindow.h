#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Shape.h"
#include <QMainWindow>
#include <QGraphicsView>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    std::vector<bool> hasRolled;
    std::vector<bool> hasPlayerStarted;
    int nextTile;
    int getXOfTile(int x, bool y);
    int getYOfTile(int x, bool y);
    int timeInMove;
    int currRoll;
    void paintEvent(QPaintEvent *);

private slots:
    void on_gameStart();

    void on_timer_fired();

    void updateCountdown();

    void on_undo();

   void on_quit();

    void on_newgame();

    void on_end();

    void on_reroll();

    void on_roll();

    void on_leaderboard();

private:
    Ui::MainWindow *ui;


    QGraphicsView * view;
    QPainter *painter;
    int time_steps_;
    bool gameStarted = false;
    int numPlayers = 0;
    int playerTurn = 1;
    int winner = -1;

    QTimer *t;
    QTimer *timer;
    std::vector<QString> labelColors;
    std::vector<ShapeObj *> points_;
    std::vector<int> grid;
    std::vector<QColor> playerColors;
    std::vector<QString> playerNames;
    std::vector<int> playerTile;
    std::vector<int> prevPlayerTiles;
    std::vector<int> undos;
    std::vector<int> rerolls;
    std::vector<QGraphicsLineItem*> lines;
};
#endif // MAINWINDOW_H
