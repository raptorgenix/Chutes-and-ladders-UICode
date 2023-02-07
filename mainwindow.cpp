#include <QDebug>
#include <QTimer>
#include <QTime>

#include "Shape.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFile>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qsrand(static_cast<unsigned>(QTime::currentTime().msec()));
    QColor c = QColor(255, 100, 100);
    playerColors.push_back(c);
    QColor c2 = QColor(100, 255, 100);
    playerColors.push_back(c2);
    QColor c3 = QColor(100, 100, 255);
    playerColors.push_back(c3);
    QColor c4 = QColor(100, 255, 255);
    playerColors.push_back(c4);
    qDebug() << "width of scene";
    QGraphicsScene * scene = new QGraphicsScene;

    view = ui->graphicsView;

    QPushButton *startbutton = ui->pushButton;
    QPushButton *newgame = ui->pushButton_2;
   QPushButton *endgame = ui->pushButton_3;
 QPushButton *undo = ui->pushButton_4;
   QPushButton *reroll = ui->pushButton_5;
    QPushButton *roll = ui->pushButton_6;
    QPushButton *quitbutton = ui->quitButton;
    QPushButton *statsbutton = ui->leaderButton;

    view->setScene(scene);
    // make the scene the same size as the view containing it
    view->setSceneRect(0,0,view->frameSize().width(),view->frameSize().height());


    for(int i = 0; i <= 500; i+=50)//add lines to make a grid for the game
    {
        scene->addLine(i,0,i,view->frameSize().height());
    }
    // Add Horizontal lines
    for(int i = 0; i <= 500; i+=50)
    {
        if(i == 450){
        scene->addLine(0, i, 550, i);
        }
        scene->addLine(0, i, 500, i);
    }

    int counter = 1;
    bool leftright = true;
    for(int i = 1; i <= 100; i+=1)
    {
        int row = (i-1)/10;
        int col = (i-1)%10;


        int yval = 450 - row*50;
        int xval = col*50+5;
        if(!leftright){
        xval = 450 - (col*50-5);
        }
        std::string num = std::to_string(i);
        QLabel *label = new QLabel(this);

        QString qstr = QString::fromStdString(num);
        label->setText(qstr);


view->scene()->addWidget(label);
label->setGeometry(xval, yval, 20, 15); //create counter for labeling the individual tiles

        counter++;
        if(counter > 10){
            if(leftright){
                leftright = false;
            }
            else{
                leftright = true;
            }
            counter = 1;
        }

    }

    t = new QTimer(this);
    timer = new QTimer(this);
      t -> setInterval(10000);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateCountdown); //connecting buttons to functions
    connect(t, &QTimer::timeout, this, &MainWindow::on_timer_fired);
    connect(startbutton, &QPushButton::clicked, this, &MainWindow::on_gameStart);
    connect(roll, &QPushButton::clicked, this, &MainWindow::on_roll);
    connect(reroll, &QPushButton::clicked, this, &MainWindow::on_reroll);

    connect(undo, &QPushButton::clicked, this, &MainWindow::on_undo);
    connect(endgame, &QPushButton::clicked, this, &MainWindow::on_end);
    connect(quitbutton, &QPushButton::clicked, this, &MainWindow::on_quit);
    connect(newgame, &QPushButton::clicked, this, &MainWindow::on_newgame);
    connect(statsbutton, &QPushButton::clicked, this, &MainWindow::on_leaderboard);

















}

int MainWindow::getXOfTile(int tilenum, bool player){ //return position based on tile's number
    int row = (tilenum-1)/10;
    int col = (tilenum-1)%10;

    int xval = col*50+15;
    if((row+1)%2 == 0){
        xval = 450-(col*50-15);
    }

    if(hasPlayerStarted[playerTurn] == false && player){


        if(playerTurn == 0){
            return 500;
        }
        if(playerTurn == 1){
            return 520;
        }
        if(playerTurn == 2){
            return 500;
        }
        if(playerTurn == 3){
            return 520;
        }
    }
    return xval;

}
int MainWindow::getYOfTile(int tilenum, bool player){ //return y position based on tile's number
    int row = (tilenum-1)/10;

    int yval = 465 - row*50;

    if(hasPlayerStarted[playerTurn] == false && player){
        if(playerTurn == 0){
            return 450;
        }
        if(playerTurn == 1){
            return 450;
        }
        if(playerTurn == 2){
            return 470;
        }
        if(playerTurn == 3){
            return 470;
        }
    }
    return yval;


}

void generateBoard(std::vector<int> &board){ //generated board of chutes and ladder positions
    //chutes
    std::vector<int> v = {};

    board = v;
    for(int i = 0; i < 100; i++){
        board.push_back(0);
    }
    int chutes = -1;
    int ladders = 1;
    qDebug() << "Generating";

    while(chutes >= -6)//chute pairing and ladder pairings are indicated by a vector of the "tiles" - there will be two tiles marked -1 and, this is a chute pairing.The next chute is two tiles labeled -2 in the board grid.
    {
        int node = rand() % 98 + 2;
        int node2 = rand() % 98 + 2;
            if(board[node] == 0 && board[node2] == 0 && node != node2){
                board[node] = chutes;
                board[node2] = chutes;
                chutes --;
            }

    }

    while(ladders <= 7)
    {
        int node = rand() % 98 + 2;
        int node2 = rand() % 98 + 2;
            if(board[node] == 0 && board[node2] == 0 && node != node2){
                board[node] = ladders;
                board[node2] = ladders;
                ladders++;
            }
    }
    //ladders
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_gameStart() //initialize board and players/all tiles and relevant storage vectors
{
    if(!gameStarted){
    for(int i = 0; i < lines.size(); i++){
        QGraphicsLineItem *l = lines[i];
        view->scene()->removeItem(l);
    }
    lines.clear();
    for(int i = 0; i < points_.size(); i++){
        ShapeObj *l = points_[i];
        view->scene()->removeItem(l);
    }
    points_.clear(); //clears old players
    prevPlayerTiles.clear();
    playerTile.clear();
    labelColors.clear();
    playerColors.clear();

    timeInMove = 9; //start timer for move
    ui->notif->setText("");
    if(gameStarted == false){
    gameStarted = true;


    playerTurn = 0;
    QString nump =  ui->playerIn->text();


    numPlayers = 4;
    if(nump == "3"){
        numPlayers = 3;
    }

    if(nump == "2"){
        numPlayers = 2;
    }

    qDebug() << nump;
    qDebug() << "Num above";
    QColor c = QColor(255, 100, 100);
    playerColors.push_back(c);
    QColor c2 = QColor(100, 255, 100);
    playerColors.push_back(c2);
    QColor c3 = QColor(100, 100, 255);
    playerColors.push_back(c3);
    QColor c4 = QColor(0, 255, 255);
    playerColors.push_back(c4);

  t->start(10000);//start timers
  timer->start(1000);//for updating label each second

    for(int i = 0; i < numPlayers; i++){
        hasRolled.push_back(false);
    }
    for(int i = 0; i < numPlayers; i++){
        playerTile.push_back(0);
    }
    for(int i = 0; i < numPlayers; i++){
        hasPlayerStarted.push_back(false);
    }
    for(int i = 0; i < numPlayers; i++){


        int nextx = getXOfTile(0, true);
        int nexty = getYOfTile(0, true);
        ShapeObj * so = new ShapeObj(nextx, nexty, playerColors[playerTurn]);
        view->scene()->addItem(so);

        points_.push_back(so);
        if(playerTurn == numPlayers-1){
            playerTurn =0;
        }
        else{
            playerTurn++;
        }

    }


    generateBoard(grid);

    }

    currRoll = 0;

    labelColors.push_back("color: rgb(255, 100, 100);"); //colors for label initailized
    labelColors.push_back("color: rgb(100, 255, 100);");
    labelColors.push_back("color: rgb(100, 100, 255);");
    labelColors.push_back("color: rgb(0, 255, 255);");


    QString p1n =  ui->lineEdit->text(); //take in player name input
    QString p2n = ui->lineEdit_2->text();
    QString p3n = ui->lineEdit_3->text();
    QString p4n = ui->lineEdit_4->text();
    if(p1n == ""){
        p1n = "Player1"; //default name values if no input given
    }
    if(p2n == ""){
        p2n = "Player2";
    }
    if(p3n == ""){
        p3n = "Player3";
    }
    if(p4n == ""){
        p4n = "Player4";
    }

    playerNames.push_back(p1n);
    playerNames.push_back(p2n);
    playerNames.push_back(p3n);
    playerNames.push_back(p4n);
    ui->currPlayerLabel->setText(playerNames[0]); //set text of label for playername to first player



    for(int i = 0; i < numPlayers; i++){ //initialize undo vector for each player
        undos.push_back(3);
    }
    for(int i = 0; i < numPlayers; i++){ //initialize reroll vector counter for each player
        rerolls.push_back(5);
    }

    for(int i = 0; i < numPlayers; i++){ //undo previous tiles tracking
        prevPlayerTiles.push_back(0);
    }

    for(int i = 1; i < 7; i++){ //display chutes and ladder with lines
        int n1 = 0;
        int n2 = 0;
        for(int j = 1; j < 99; j++){
            if(-1*i == grid[j] && n1 == 0){
                n1 = j;
            }
            if(-1*i == grid[j] && n1 != 0 && j != n1){
                n2 = j;
            }

        }


    QPen pen(Qt::magenta, 3, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);

        QGraphicsLineItem *l = new QGraphicsLineItem(getXOfTile(n1, false), getYOfTile(n1, false), getXOfTile(n2, false), getYOfTile(n2, false)); //use to draw each line
        lines.push_back(l);
        l->setPen(pen);

           view->scene()->addItem(l);





    }

    for(int i = 1; i < 8; i++){ //now ladders displaying
            int n1 = 0;
            int n2 = 0;
            for(int j = 1; j < 99; j++){
                if(i == grid[j] && n1 == 0){
                    n1 = j;
                }
                if(i == grid[j] && n1 != 0 && j != n1){
                    n2 = j;
                }

            }
            qDebug() <<n1;
            qDebug() <<n2;

        QPen pen(Qt::green, 3, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
        QGraphicsLineItem *l = new QGraphicsLineItem(getXOfTile(n1, false), getYOfTile(n1, false), getXOfTile(n2, false), getYOfTile(n2, false));
        lines.push_back(l);
        l->setPen(pen);

           view->scene()->addItem(l);
    }


}
}
void MainWindow::updateCountdown() { //update the label that shows the time remaining in turn
    QString sec = QString::number(timeInMove);
    ui->timeLabel->setText(sec);
    if(timeInMove == 1){
        timeInMove = 10;
    }
    else{
        timeInMove--;
    }
}

void MainWindow::on_timer_fired() { //what to do when turn ends
    //qDebug() << "timer fired!";
    ui->notif->setText("");
    if(currRoll == 0){
        ui->notif->setText("Last player's turn was skipped.");
    }
    hasRolled[playerTurn] = false;
    if(hasPlayerStarted[playerTurn] == false){ //logic for checking if player has left the "starting zone" yet
        if(currRoll >= 6 || currRoll == 12){
            hasPlayerStarted[playerTurn] = true;
            currRoll = 1;
        }
    }
    if(playerTile[playerTurn] != 0){
        prevPlayerTiles[playerTurn] = playerTile[playerTurn];
    } //update previous tiles for undo if used
    qDebug() << prevPlayerTiles[playerTurn];
    int nextpos = playerTile[playerTurn] + currRoll;
    int nexttile = 0;
    if(nextpos < 100 && grid[nextpos] != 0){//ladders logic handling for when landed on
        if(grid[nextpos] > 0){
            int id = grid[nextpos];
            for(int i = 0; i < 100; i++){
                if(id == grid[i] && nextpos != i){
                    int pairpos = i;
                    if(pairpos > nextpos){
                        nexttile = pairpos;
                    }
                    else{
                        nexttile = nextpos;
                    }
                }
            }
        }
        else{ //chutes logic handling
            if(grid[nextpos] < 0){
                int id = grid[nextpos];
                for(int i = 0; i < 100; i++){
                    if(id == grid[i] && nextpos != i){
                        int pairpos = i;
                        if(pairpos < nextpos){
                            nexttile = pairpos;
                        }
                        else{
                            nexttile = nextpos;
                        }
                    }
                }
        }
    }
    }
    else{
        nexttile = nextpos;
    }
    if(nexttile >= 100){ //check win condition
        winner = playerTurn;
        nexttile = 100;
        on_end();


    }
    if(hasPlayerStarted[playerTurn] == true){
        playerTile[playerTurn] = nexttile;
    }

    int nextx = getXOfTile(nexttile, true); //get position for next tile
    int nexty = getYOfTile(nexttile, true);

    if(hasPlayerStarted[playerTurn]){
    nexty = nexty + (rand() % 30 -15);
    nextx = nextx + (rand() % 30 -15) ;
    }

    if (points_.size() == numPlayers) {

       ShapeObj * first = points_[0];
       points_.erase(points_.begin());
       view->scene()->removeItem(first);
    }

    ShapeObj * so = new ShapeObj(nextx, nexty, playerColors[playerTurn]);//generate player at new tile


    view->scene()->addItem(so);
    qDebug() << playerColors[playerTurn];
    points_.push_back(so);
    time_steps_++;
    if(playerTurn == numPlayers-1){
        playerTurn =0;
    }
    else{
        playerTurn++;
    }
    ui->currPlayerLabel->setStyleSheet(labelColors[playerTurn]); //update ui with all new data
    ui->currPlayerLabel->setText(playerNames[playerTurn]);
    qDebug() <<playerNames[playerTurn];
    currRoll = 0;
    QString reset = QString::number(currRoll);
    QString rrls = QString::number(rerolls[playerTurn]);
    QString unds = QString::number(undos[playerTurn]);
    ui->rerollLabel->setText(rrls);
    ui->undoLabel->setText(unds);
    ui->rollNum->setText(reset);



}
void MainWindow::paintEvent(QPaintEvent *) //obsolete
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

}
void MainWindow::on_roll() //what to do when roll is pressed

{

    if(gameStarted && !hasRolled[playerTurn]){
    int roll = rand() % 12 + 1; //generate roll
    currRoll = roll;
    QString roller = QString::number(roll);
    ui->rollNum->setText(roller); //update UI with rolled die
    hasRolled[playerTurn] = true;
    }


}
void MainWindow::on_reroll(){
if(gameStarted && hasRolled[playerTurn] && rerolls[playerTurn] > 0){
    int roll = rand() % 12 + 1; //same logic as roll but updates the old roll
    currRoll = roll;
    rerolls[playerTurn] = rerolls[playerTurn] - 1;
    QString rrls = QString::number(rerolls[playerTurn]);
    ui->rerollLabel->setText(rrls);
    QString roller = QString::number(roll);
    ui->rollNum->setText(roller);
}


}



void MainWindow::on_undo(){
    if(gameStarted && undos[playerTurn] > 0 && prevPlayerTiles[playerTurn] != 0 && undos[playerTurn] > 0){ //make sure conditions are met to be able to undo - game has to be started and player has to be on board

        QString und = QString::number(undos[playerTurn] -1);
        undos[playerTurn] = undos[playerTurn] - 1;
        ui->undoLabel->setText(und);


        ShapeObj * curr = points_[0];
        view->scene()->removeItem(curr);
         qDebug() << playerTurn;
        qDebug() << "Past erase";

        int nexttile = prevPlayerTiles[playerTurn]; //reposition to previous tile in data
        playerTile[playerTurn] = nexttile;

        int nextx = getXOfTile(nexttile, true);
        int nexty = getYOfTile(nexttile, true);

        nexty = nexty + (rand() % 30 -15);
        nextx = nextx + (rand() % 30 -15);
        ShapeObj * so = new ShapeObj(nextx, nexty, playerColors[playerTurn]);
        points_[0] = so;
        view->scene()->addItem(so); //regenerate player on previous tile in display
        qDebug() << "past all";

    }
}

void MainWindow::on_quit() {
    if(gameStarted){
    numPlayers = numPlayers - 1;
    if(numPlayers < 2){
        on_end();
    }

    //RESETS BOARD AND ALL TABLES/VARIABLES WITH ONE LESS PLAYER

    ShapeObj * curr = points_[0];
    view->scene()->removeItem(curr);
    points_.erase(points_.begin());
    playerColors.erase(playerColors.begin() + playerTurn);
    playerNames.erase(playerNames.begin() + playerTurn);
    playerTile.erase(playerTile.begin() + playerTurn);
    undos.erase(undos.begin() + playerTurn);
    prevPlayerTiles.erase(prevPlayerTiles.begin() + playerTurn);
    rerolls.erase(rerolls.begin() + playerTurn);

    labelColors.erase(labelColors.begin() + playerTurn);

//    if(playerTurn == 0){
//        playerTurn = numPlayers-1;

//    }
//    else{
//        playerTurn--;
//    }




    }

}

void MainWindow::on_end() {
    t->stop(); // stop timers
    timer->stop();
    gameStarted = false;
    ui->notif->setText("Game ended!");
    if(winner != -1){ //if game ends with a winner
        qDebug() << "Winner is " << playerTurn;
    }
}


void MainWindow::on_newgame() {
    on_gameStart();
}


void MainWindow::on_leaderboard(){
    std::vector<QString> lines;
    //auto filename = QFileDialog::getOpenFileName(this, "Open", QDir::rootPath(), "CSV File (*.csv)");
    QFile file("E://hw4//hw4//stats.csv");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            qDebug() << "no file";
            return;

        QTextStream in(&file);
        QString line = in.readLine();
        qDebug() << line;
        int iter = 0;
        while (!in.atEnd()) {

            auto line = in.readLine();
            qDebug() << line;
            auto values = line.split(",");
            int colcount = 4;

            for(int i = 0; i < colcount; i ++){

            }
            iter++;
        }




}
