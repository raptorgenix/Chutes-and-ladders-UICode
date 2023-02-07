
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>

#include "Shape.h"

// Name: Adam Ten Hoeve

namespace Ui {
class Game;
}

class Game : public QMainWindow
{
    Q_OBJECT

public:
    explicit Game(QWidget *parent = 0);
    ~Game();

private slots:
    // Perform one iteration of the game.
    void on_StepButton_clicked();
    // Play the game automatically at the current speed.
    void on_PlayButton_clicked();
    // Stop the timer so doesn't play the game automatically.
    void on_PauseButton_clicked();
    // Perform a certain number of iterations in 1 turn.
    void on_SkipButton_clicked();
    // Make the cell dead or alive by clicking it.
    // Left click is alive, right click is dead.
    void SquareLeftClickedSlot(Square*);
    void SquareRightClickedSlot(Square*);
    // What happens when scroll bar is moved. Updates speed.
    void on_SpeedScrollBar_sliderMoved(int position);
    // What to do when timer goes off. Automatically playes the game.
    void timer_action();

private:
    // Update labels.
    void Update_Alive();
    void Update_Turn();
    void Update_Speed();
    // Updates the number of neighbors of each cell in the graph.
    void Update_Neighbors();
    // Update the colors of the graph given the number of neighbors.
    void Update_Colors();
    // Updates the visuals of the graph
    void Update_Graph();

    // Function to step the game 1 time.
    void GameStep();
    // Finds the number of neighbors of input cell.
    int Find_Neighbors(int i, int j);

    Ui::Game *ui;
    // The two scenes of the GUI
    QGraphicsScene *scene;
    QGraphicsScene *graph;

    // Number of cells alive out of 450.
    int alive_;
    int prop_[30];
    // Increments each time a turn is taken.
    int turn_;
    // Number of seconds between steps, when the game is playing.
    int speed_;
    // Timer to automatically play the game.
    QTimer *t;
    // Grid of squares that can be clicked. Represent the cells of the game.
    Square *grid[30][15];
};
