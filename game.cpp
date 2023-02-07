#include <iostream>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QDebug>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include "game.h"


#include "Shape.h"

using namespace std;



Game::Game(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlotWindow)
{
    // we need to set up the ui before we draw on our scene
    ui->setupUi(this);

    // scene is a pointer field of plot window
    scene = new QGraphicsScene;
    // graph is a pointer field to the graph
    graph = new QGraphicsScene;

    QGraphicsView * view = ui->plotGraphicsView;
    QGraphicsView * graphView = ui->GraphView;

    // Set the scene for the grid and the graph
    view->setScene(scene);
    view->setSceneRect(0,0,view->frameSize().width(),view->frameSize().height());
    graphView->setScene(graph);
    graphView->setSceneRect(0,0,graphView->frameSize().width(), graphView->frameSize().height());
    // Get rid of the scroll bars for the graph
    graphView->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    graphView->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );

    // so that you have random available and properly seeded
    srand(time(0));

    // Create a grid
    // Add vertical lines to grid
    for(int i = 0; i <= view->frameSize().width(); i+=20)
    {
        scene->addLine(i,0,i,view->frameSize().height());
    }
    // Add Horizontal lines
    for(int i = 0; i <= view->frameSize().height(); i+=20)
    {
        scene->addLine(0, i, view->frameSize().width(), i);
    }
    // Initialize the variables.
    alive_ = 0;
    turn_ = 0;
    speed_ = 1000;

    // Initialize timer for when the game is played
    t = new QTimer(this);
    t -> setInterval(speed_);
    connect(t, &QTimer::timeout, this, &Game::timer_action);

    // Set the center position of each Square in the grid
    for(int i = 0; i < 30; i++)
    {
        for(int j = 0; j < 15; j++)
        {
            Square *s = new Square();
            grid[i][j] = s;
            grid[i][j]->set_x(i*20);
            grid[i][j]->set_y(j*20);

            // Randomly determine which cells are alive and dead
            double r = ((double) rand() / (RAND_MAX));
            // 50% chance. If greater than 0.5, is alive.
            if (r > 0.5)
            {
                alive_ += 1;
                grid[i][j]->set_alive(true);
//                grid[i][j]->set_color(QColor(254, 127, 156, 255));
            }
            // Connect each cell to the clicked signals and slots.
            connect(grid[i][j], &Square::SquareLeftClicked,
                    this, &Game::SquareLeftClickedSlot);
            connect(grid[i][j], &Square::SquareRightClicked,
                    this, &Game::SquareRightClickedSlot);
            scene->addItem(grid[i][j]);
        }
        // Set the initial population to 0 for each element in the array.
        prop_[i] = 0;
    }
    // Update the labels depending on the initial random initialization.
    Update_Neighbors();
    Update_Colors();

    Update_Alive();
    Update_Turn();
    Update_Speed();

    Update_Graph();

    //Stuff for the speed slider
    ui -> SpeedScrollBar -> setMaximum(1000);
    ui -> SpeedScrollBar -> setMinimum(100);
    ui -> SpeedScrollBar -> setInvertedAppearance(true);

    // Connect the buttons to the slots for their functions.
    connect(ui->SpeedScrollBar, &QAbstractSlider::sliderMoved,
            this, &Game::on_SpeedScrollBar_sliderMoved);

    connect(ui->stepButton, &QAbstractButton::clicked,
            this, &Game::on_StepButton_clicked);

    connect(ui->playButton, &QAbstractButton::clicked,
            this, &Game::on_PlayButton_clicked);

    connect(ui->pauseButton, &QAbstractButton::clicked,
            this, &Game::on_PauseButton_clicked);

    connect(ui->skipButton, &QAbstractButton::clicked,
            this, &Game::on_SkipButton_clicked);
}

// Deconstructor.
Game::~Game()
{
    delete ui;
}

// Updates the label for the number of cells alive
// Also sets the population array for that turn.
void Game::Update_Alive()
{
    // If turn is less than the size of the array, set the next element in the array as the current population proportion.
    if(turn_ < 30)
    {
        prop_[turn_] = (int)((float)alive_ / 450.0 * 100.0);
    }
    // If turn is greater than size of array, need to move each element to the left and put new population as last element.
    else
    {
        int temp = 0;
        int turnProp = (int)((float)alive_ / 450.0 * 100.0);
        // Need to shift everything in the array over first;
        for(int i = 29; i > -1; i -= 1)
        {
            temp = prop_[i];
            prop_[i] = turnProp;
            turnProp = temp;
        }
    }

    // Update the alive label with the count and proportion of cells alive.
    string count = to_string(alive_);
    string prop = to_string((int)((float)alive_ / 450.0 * 100)) + string("%");
    string text = string("Population: ") + count + string(" ") + prop;
    ui->PopulationCounter->setText(QString(text.c_str()));
}

// Increment the label for the number of turns that have occured
void Game::Update_Turn()
{
    string turn = to_string(turn_);
    string text = string("Turn: ") + turn;
    ui -> TurnCounter -> setText(QString(text.c_str()));
}

// Update the speed label based on the slider.
void Game::Update_Speed()
{
    // Want to display updates per second so divide the speed by 1000.
    string speed = to_string((float)speed_ / 1000);
    string text = string("Speed: ") + speed;
    ui -> SpeedLabel -> setText(QString(text.c_str()));
}

// The actual game step, updates the grid based on the rules provided.
void Game::GameStep()
{
    // Iterate over entire grid and determine if they live or die based on the previous number of neighbors.
    // Updates the entire thing at once because only update the number of neighbors after all the cells have been assigned.
    for(int i = 0; i < 30; i++)
    {
        for(int j = 0; j < 15; j++)
        {
            int neighbors = grid[i][j] -> get_neighbors();
            // Determine if cell lives or dies based on the number of neighbors.
            // If was dead and has 3 neighbors, becomes alive
            if ((!grid[i][j]->get_alive()) && (neighbors == 3))
            {
                alive_ += 1;
                grid[i][j]->set_alive(true);
//                grid[i][j]->set_color(QColor(254, 127, 156, 255));
            }
            // If was alive and has less than 2 or more than 3 neighbors, then dies
            else if ((grid[i][j]->get_alive()) && ((neighbors<2) || (neighbors>3)))
            {
                alive_ -= 1;
                grid[i][j]->set_alive(false);
//                grid[i][j]->set_color(QColor(255, 255, 255, 255));
            }
            // Otherwise state does not change.
        }
    }
    // Update the number neighbors and color of each square.
    Update_Neighbors();
    Update_Colors();
}

// Counts the number of neighbors around every cell in the grid, using the wrapping technique.
void Game::Update_Neighbors()
{
    // Iterate over entire grid
    for(int i = 0; i < 30; i++)
    {
        for(int j = 0; j < 15; j++)
        {
            // Find the number of neighbors to the cell.
            // Save that to the object.
            grid[i][j] -> set_neighbors(Find_Neighbors(i, j));
        }
    }
}

// Find the total number of alive cells surrounding the cell.
int Game::Find_Neighbors(int i, int j)
{
    int neighbors = 0;
    int topSide = j-1;
    int botSide = j+1;
    int rightSide = i+1;
    int leftSide = i-1;

    // Check if the walls need to wrap around.
    // If cell is at row 0, then topSide is bottom of grid.
    if(j == 0) {topSide = 14;}
    // If cell is at row 14, the botSide is top of grid.
    if(j == 14) {botSide = 0;}
    // If cell is at col 0, then leftSide is rightmost of grid.
    if(i == 0) {leftSide = 29;}
    // If cell is at col 29, then rightSide is leftmost of grid.
    if(i == 29) {rightSide = 0;}

    // Check the 8 surrounding cells and increase neighbors per alive.
    // Directly above.
    if(grid[i][topSide] -> get_alive()) {neighbors += 1;}
    // TopRight
    if(grid[rightSide][topSide] -> get_alive()) {neighbors += 1;}
    // Directly Right
    if(grid[rightSide][j] -> get_alive()) {neighbors += 1;}
    // BotRight
    if(grid[rightSide][botSide] -> get_alive()) {neighbors += 1;}
    // Directly Below
    if(grid[i][botSide] -> get_alive()) {neighbors += 1;}
    // BotLeft
    if(grid[leftSide][botSide] -> get_alive()) {neighbors += 1;}
    // Directly Left
    if(grid[leftSide][j] -> get_alive()) {neighbors += 1;}
    // TopLeft
    if(grid[leftSide][topSide] -> get_alive()) {neighbors += 1;}

    return neighbors;
}

void Game::Update_Colors()
{
    // Iterate through the entire graph to update the color of each cell, given the new number of neighbors.
    for(int i = 0; i < 30; i++)
    {
        for(int j = 0; j < 15; j++)
        {
            // Get the number of neighbors for each cell.
            int neighbors = grid[i][j] -> get_neighbors();
            // Determine the color of the cell based on the number of neighbors

            // If the cell is dead: white
            if(!grid[i][j] -> get_alive()) {grid[i][j] -> set_color(QColor(255, 255, 255, 255));}

            // 0 neighbors: pink
            else if(neighbors == 0) {grid[i][j] -> set_color(QColor(254, 127, 156, 255));}
            // 1 neighbor: red
            else if(neighbors == 1) {grid[i][j] -> set_color(QColor(255, 35, 1, 255));}
            // 2 neighbors: orange
            else if(neighbors == 2) {grid[i][j] -> set_color(QColor(236, 124, 38, 255));}
            // 3 neighbors: yellow
            else if(neighbors == 3) {grid[i][j] -> set_color(QColor(255, 255, 73, 255));}
            // 4 neighbors: dark green
            else if(neighbors == 4) {grid[i][j] -> set_color(QColor(11, 102, 35, 255));}
            // 5 neighbors: light green
            else if(neighbors == 5) {grid[i][j] -> set_color(QColor(91, 194, 54, 255));}
            // 6 neighbors: teal
            else if(neighbors == 6) {grid[i][j] -> set_color(QColor(126, 249, 255, 255));}
            // 7 neighbors: blue
            else if(neighbors == 7) {grid[i][j] -> set_color(QColor(17, 30, 108, 255));}
            // 8 neighbors: purple
            else {grid[i][j] -> set_color(QColor(64, 0, 64, 255));}
        }
    }
}

// Print the graph.
void Game::Update_Graph()
{
    // Clear the graph from the previous turn.
    graph -> clear();
    // Print the graph for each turn in the array.
    for(int i = 0; i < 30; i++)
    {
        graph -> addRect(i*20, 100 - prop_[i], 20, prop_[i]);
    }
}

// Take on step in a turn
void Game::on_StepButton_clicked()
{
    // Perform one iteration of the game.
    GameStep();
    // Update the information labels
    turn_ += 1;
    Update_Turn();
    Update_Alive();
    Update_Speed();
    // Update the graph of the population
    Update_Graph();
    update();
}

// Automatically play the game
void Game::on_PlayButton_clicked()
{
    // Start a timer that goes off every [speed_] milliseconds.
    t -> start(speed_);
}

// Stop automatically playing the game
void Game::on_PauseButton_clicked()
{
    // Stop the timer.
    t -> stop();
}

// Take a certain number of steps in a turn
void Game::on_SkipButton_clicked()
{
    // Make sure some input was there.
    bool safe = false;
    int skips = ui->skipNumber->text().toInt(&safe);
    if (!safe) {
        return;
    }

    // Perform the input number of game iterations.
    for(int i = 0; i < skips; i++)
    {
        GameStep();
    }
    // Then increment the labels as if it was one turn.
    turn_ += 1;
    Update_Turn();
    Update_Alive();
    Update_Speed();

    Update_Graph();
    update();
}

// Left clicking a cell makes it alive
void Game::SquareLeftClickedSlot(Square *s)
{
    // Set the cell alive to true
    s -> set_alive(true);
    // Update the neighbors and colors of the grid.
    Update_Neighbors();
    Update_Colors();

    update();
}

// Right clicking a cell makes it dead
void Game::SquareRightClickedSlot(Square *s)
{
    // Set the cell to dead
    s -> set_alive(false);
    // Update the number of neighbors and colors of the grid.
    Update_Neighbors();
    Update_Colors();

    update();
}

// When the timer goes off, want it to perform a game step.
void Game::timer_action()
{
    // Perform one game step
    GameStep();
    // Update the labels and graph
    turn_ += 1;
    Update_Turn();
    Update_Alive();
    Update_Speed();

    Update_Graph();
    update();
}

// Update the speed based on the position of the scrollbar.
void Game::on_SpeedScrollBar_sliderMoved(int position)
{
    // Speed is the position of the scrollbar.
    speed_ = position;
    // Set how often the timer goes off based on the speed.
    t -> setInterval(speed_);
    // Update the speed label.
    Update_Speed();
}
