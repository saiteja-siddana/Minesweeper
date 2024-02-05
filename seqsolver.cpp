#include <list> 
#include <string>
#include <tuple>
#include <chrono>

#include "game.h"

using namespace std;

tuple<int,int> Game::chooseRandomMove() {
    int xinit, yinit;

    // Generate random coordinates until a position is found that is covered
    do {
        xinit = rand() % height;
        yinit = rand() % width;
    } while (playboard[xinit][yinit] == 1);
    
    return make_tuple(xinit,yinit);
}


// Count mines adjacent to (x,y) that are marked already i.e uncovered
int Game::countAdjMines(int x, int y) {
    int count = 0;
    for (int rowOffset = -1; rowOffset <= 1; ++rowOffset) {
	    for (int colOffset = -1; colOffset <= 1; ++colOffset) {        
            int neighborX = x+rowOffset;
            int neighborY = y+colOffset;
	        if (isValidPosition(neighborX, neighborY) && !(rowOffset == 0 && colOffset == 0)) {
                if (playboard[neighborX][neighborY] == 1 && board[neighborX][neighborY] == -1) {
                    count++;
                } 
            }
        }
    } 
    return count;
}



void Game :: revealNeighbors(int x, int y) {
    for (int rowOffset = -1; rowOffset <= 1; ++rowOffset) {
        for (int colOffset = -1; colOffset <= 1; ++colOffset) {
            int neighborX = x + rowOffset;
            int neighborY = y + colOffset;

	        // Check if the neighbor is within the valid bounds and not the current cell
            if (isValidPosition(neighborX, neighborY) && !(rowOffset == 0 && colOffset == 0)) {
		        // Check if the neighbour is 0
                if (playboard[neighborX][neighborY] == 0) {
                    playboard[neighborX][neighborY] = 1;

		            // Check if the neighbour is mine
                    if (board[neighborX][neighborY] == -1) {
                        printf("Detected a mine at (%d, %d)\n", neighborX, neighborY);
                    }
                }
            }
        }
    }
}


void Game::markNeighbors(int x, int y) {
    for (int rowOffset = -1; rowOffset <= 1; ++rowOffset) {
	    for (int colOffset = -1; colOffset <= 1; ++colOffset) {
	        int neighborX = x + rowOffset;
            int neighborY = y + colOffset;
	
	        if (isValidPosition(neighborX, neighborY) && !(rowOffset == 0 && colOffset == 0)) {
		        if (playboard[neighborX][neighborY] == 0) {
		            playboard[neighborX][neighborY] = 1;
                    playmines[playedmines * 2] = neighborX;
                    playmines[playedmines * 2 + 1] = neighborY;
                    playedmines++;
		        }
	        }
	    }
    }
}


int Game::countUnrevealed(int x, int y) {
    int count = 0;
    for (int rowOffset = -1; rowOffset <= 1; ++rowOffset) {
	    for (int colOffset = -1; colOffset <= 1; ++colOffset) {
	        int neighborX = x + rowOffset;
            int neighborY = y + colOffset;

	        if (isValidPosition(neighborX, neighborY) && !(rowOffset == 0 && colOffset == 0)) {
		        if (playboard[neighborX][neighborY] == 0) {
                    count++;
                }
	        }
	    }
    }
    return count;
}



double Game::seqSolve(int iteration) {
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
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    // Check if the current cell is revealed and not a mine
                    if (playboard[i][j] == 1 && board[i][j] != -1 ) {
                
                        // count adjacent mines and unrevealed cells
                        int adjacentMines = countAdjMines(i,j);
                        int unrevealedCells = countUnrevealed(i,j);

                        // there are unrevealed cells
                        if (unrevealedCells != 0) {
                            // The number of adjacent mines is equal to the value of the current cell. It means that all adjacent mines are found, and the neighbors can be safely revealed.
                            if (adjacentMines == board[i][j]) {
                                madeProgress = true;
                                revealNeighbors(i, j);
                            }

                            // Check if the number of unrevealed cells matches the remaining mines
                            if (unrevealedCells == board[i][j] - adjacentMines && unrevealedCells >= 0) {
                                madeProgress = true;
                                markNeighbors(i, j);
                            }
                        }   						
                    }
                }
            }
        }
    }

    // end timer
    auto finish = std::chrono::high_resolution_clock::now();
    double overallDuration = chrono::duration_cast<chrono::milliseconds>(finish - start).count();
    return overallDuration;
}

