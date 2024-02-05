#ifndef __BOARD_H__
#define __BOARD_H__
#include <stdlib.h> 
#include <tuple>
#include <list>
#include <cassert>

using namespace std; 

class Game {
public:
    int width;
    int height;
    int minesnumber;
    int thread_count;
    int** mines; //2d list minesnumber x 2 that stores the coordinates of the mines
    int** board; // board represents the actual Minesweeper game board that includes information about the mines and numbers.
    int** playboard; // playboard is a 2D array used to keep track of the cells that the player has uncovered during the game. It is updated based on the player's moves, and cells that are uncovered by the player are marked with the value 1.
    int** checkboard; // checkboard is a 2D array used to keep track of the cells that the player has checked during the game.
    int** openmpPlayboard; // To avoid race conditions, the openmpPlayboard is used as a temporary board that each thread can update independently without conflicts.
    int* playmines; // playmines is an array used to store the positions. It is a 1D array.
    int playedmines;
    Game(int iwidth, int iheight, int numberofmines, int count);
    void printBoard();
    void OMPprintBoard();
    void setMines();
    void resetPlayboardsAndRelatedValues();
    bool isValidPosition(int x, int y);
    
    int countAdjMines(int x, int y);
    int countUnrevealed(int x, int y);
    void markNeighbors(int x, int y);
    void openmpMarkNeighbours(int x, int y);
    void revealNeighbors(int x, int y);
    void openmpRevealNeighbours(int x, int y);
    tuple<int,int> chooseRandomMove();
    double seqSolve(int i);
    double parSolve(int i);
    double sharedParSolve(int i);
    double openmpSolve(int i);
    bool checkTheResult();

    float toBandwidth(int bytes, float sec);
};

#endif