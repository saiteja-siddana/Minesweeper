#include <list> 
#include <string>
#include <tuple>
#include <chrono>
#include <omp.h>
#include "game.h"

using namespace std;


// mark neighbours and update playmines
void Game :: openmpMarkNeighbours(int x, int y) {
    if(thread_count!=0){
        omp_set_dynamic(0);
        omp_set_num_threads(thread_count);
    }
    for (int rowOffset = -1; rowOffset <= 1; ++rowOffset) {
        for (int colOffset = -1; colOffset <= 1; ++colOffset) {
            int neighborX = x + rowOffset;
            int neighborY = y + colOffset;

            // Check if the neighbor is within the valid bounds and not the center cell.
            // Without the critical section, multiple threads could potentially check and update the shared variables simultaneously, leading to data inconsistency and race conditions.
            if (isValidPosition(neighborX, neighborY) && !(rowOffset == 0 && colOffset == 0)) {
                int hasToStore = false;
                int tempMineCount;

                #pragma omp critical 
                {
                    // Check if the neighbor has not been marked yet
                    if (openmpPlayboard[neighborX][neighborY] == 0) {
                        openmpPlayboard[neighborX][neighborY] = 1;
                        hasToStore = true;
                        tempMineCount = playedmines;
                        playedmines++;
                    }
                }

                if (hasToStore) {
                    // Store the coordinates.
                    playmines[tempMineCount * 2] = neighborX;
                    playmines[tempMineCount * 2 + 1] = neighborY;
                }
            }
        }
    } 
}

// reveal neighbours of the cell.
void Game :: openmpRevealNeighbours(int x, int y) {
    if(thread_count!=0){
        omp_set_dynamic(0);
        omp_set_num_threads(thread_count);
    }
    for (int rowOffset = -1; rowOffset <= 1; ++rowOffset) {
        for (int colOffset = -1; colOffset <= 1; ++colOffset) {
            int neighborX = x + rowOffset;
            int neighborY = y + colOffset;

            // Check if the neighbor is within the valid bounds and not the current cell
            if (isValidPosition(neighborX, neighborY) && !(rowOffset == 0 && colOffset == 0)) {
                // Check if the neighbour is 0
                if (playboard[neighborX][neighborY] == 0) {
                    openmpPlayboard[neighborX][neighborY] = 1;

                    // Check if the neighbour is mine
                    if (board[neighborX][neighborY] == -1) {
                        printf("Detected a mine at (%d, %d)\n", neighborX, neighborY);
                    }
                }
            }
        }
    }
}

double Game::openmpSolve(int iteration) {
    if(thread_count!=0){
        omp_set_dynamic(0);
        omp_set_num_threads(thread_count);
    }
    auto start = std::chrono::high_resolution_clock::now();
    srand(time(0) * iteration);

    while (playedmines < minesnumber) {
        // Choose a random move
        tuple<int, int> randomMove = chooseRandomMove();
        int xCoord = get<0>(randomMove);
        int yCoord = get<1>(randomMove);

        // Check if the selected cell contains a mine
        checkboard[xCoord][yCoord]=1;
        if (board[xCoord][yCoord] == -1) {
            printf("Mine detected. Returning...\n");
            return -1;
        } else {
            // Mark the cell as revealed
            playboard[xCoord][yCoord] = 1;
        }

        bool madeProgress = true;

        // Iterate until no more progress can be made
        while (madeProgress) {
            madeProgress = false;

            // Iterate over the game board
            #pragma omp parallel for collapse(2)
             // #pragma omp parallel
             // #pragma omp single nowait
            for (int i = 0; i < height; i++) {
                // #pragma omp task
                for (int j = 0; j < width; j++) {
                    // Check if the current cell is revealed and not a mine
                    if (playboard[i][j] == 1 && board[i][j] != -1) {

                        // count adjacent mines and unrevealed cells
                        int adjacentMines = countAdjMines(i, j);
                        int unrevealedCells = countUnrevealed(i, j);

                        // there are unrevealed cells
                        if (unrevealedCells != 0) {
                            // The number of adjacent mines is equal to the value of the current cell. It means that all adjacent mines are found, and the neighbors can be safely revealed.
                            if (adjacentMines == board[i][j]) {
                                // Reveal neighbors
                                madeProgress = true;
                                openmpRevealNeighbours(i, j);
                            }

                            // Check if the number of unrevealed cells matches the remaining mines
                            if (unrevealedCells == board[i][j] - adjacentMines && unrevealedCells >= 0) {
                                // Mark neighbors as mines
                                madeProgress = true;
                                openmpMarkNeighbours(i, j);
                            }
                        }
                    }
                }
            }

            // Update the playboard with the marked neighbors
            #pragma omp parallel for collapse(2)
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    playboard[i][j] |= openmpPlayboard[i][j];
                }
            }
        }
    }

    // end timer
    auto finish = std::chrono::high_resolution_clock::now();
    double overallDuration = chrono::duration_cast<chrono::milliseconds>(finish - start).count();
    return overallDuration;
}