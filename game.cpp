#include <ctime>
#include <stdio.h> 
#include <tuple>
#include "game.h"

using namespace std;

// set initial values for playboard, height, width, minepositions and playes mines
Game :: Game(int iwidth, int iheight, int numberofmines, int count) {
    height = iheight;
    width = iwidth;
    minesnumber = numberofmines;
    thread_count=count;
    
    playedmines = 0;
    mines = (int**)malloc(minesnumber * sizeof(int*));
    board = (int**)malloc(height * sizeof(int*));
    playboard = (int**)malloc(height * sizeof(int*));
    checkboard = (int**)malloc(height * sizeof(int*));
    openmpPlayboard = (int**)malloc(height * sizeof(int*));
    playmines = (int*)calloc(minesnumber * 2, sizeof(int));

    for (int i = 0; i < minesnumber; i++) {
        mines[i] = (int*)calloc(2, sizeof(int));
    }

    for (int i = 0; i < height; i++) {
        board[i] = (int*)calloc(width, sizeof(int));
        playboard[i] = (int*)calloc(width, sizeof(int));
        checkboard[i] = (int*)malloc(height * sizeof(int*));
        openmpPlayboard[i] = (int*)calloc(width, sizeof(int));
    }

    for (int i = 0; i < minesnumber * 2; i++) {
        playmines[i] = -1;
    }
}

// reset playboard, openmpPlayboard, playedmines, positions.
void Game :: resetPlayboardsAndRelatedValues() {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            playboard[i][j] = 0;
            checkboard[i][j] = 0;
            openmpPlayboard[i][j] = 0;
        }
    }
    for (int i = 0; i < 2*minesnumber; i++) {
        playmines[i] = -1;
    }
    playedmines = 0;
}

// to bandwidth
float Game :: toBandwidth(int bytes, float seconds) {
    const int bytesInGigabyte = 1024 * 1024 * 1024;
    const float bytesToGigabytes = 1.0f / bytesInGigabyte;

    // bandwidth = bytes * bytesToGigabytes / seconds
    return static_cast<float>(bytes) * bytesToGigabytes / seconds;
}

// print the board if -p flag is passed.
void Game :: printBoard() {
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            // B = mine opened accedentally
            // F = flagged mine
            if(playboard[row][col]==1){
                if (board[row][col] < 0) {
                    if(checkboard[row][col]==1){
                        printf("B");
                    }
                    else{
                        printf("F");
                    }
                        
                }else if (board[row][col] == 0) {
                    printf(" ");
                } else {
                    printf("%d ", board[row][col]);
                }
            }
            else{
                printf("* ");
            }
        }
        printf("\n");
    }
}


// print the board if -p flag is passed.
void Game :: OMPprintBoard() {
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            // B = mine opened accedentally
            // F = flagged mine
            if(openmpPlayboard[row][col]==1){
                if (board[row][col] < 0) {
                    if(checkboard[row][col]==1){
                        printf("B ");
                    }
                    else{
                        printf("F ");
                    }
                        
                }else if (board[row][col] == 0) {
                    printf(" ");
                } else {
                    printf("%d ", board[row][col]);
                }
            }
            else{
                printf("* ");
            }
        }
        printf("\n");
    }
}

bool Game::checkTheResult() {
    for (int i = 0; i < minesnumber*2; i+=2){
        if (playmines[i] == -1 || playmines[i+1] == -1) {
            return false;
        }
        if (board[playmines[i]][playmines[i+1]] != -1) {
            return false;
        }
    }
    return true;
        

}

bool Game :: isValidPosition(int x, int y) {
    return x >= 0 && x < height && y >= 0 && y < width;
}

// set values in the playboard
void Game :: setMines() {
    srand(time(NULL));

    for (int mineIndex = 0; mineIndex < minesnumber; mineIndex++) {
        int x, y;

        // Generate random mine coordinates until a position is found that is not a mine.
        do {
            x = rand() % height;
            y = rand() % width;
        } while (board[x][y] == -1);

        // Set mine position on the board.
        mines[mineIndex][0] = x;
        mines[mineIndex][1] = y;
        board[x][y] = -1;

        // Populate the number hints around the mine.
        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                int xi = x + i;
                int yi = y + j;

                // Check if the position is within bounds and not the mine's position.
                if (isValidPosition(xi, yi) && !(i == 0 && j == 0) && board[xi][yi] != -1) {
                    board[xi][yi] += 1;
                }
            }
        }
    }
}
