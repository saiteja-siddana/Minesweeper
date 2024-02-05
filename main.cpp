#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h>

#include "game.h"

using namespace std;

int main(int argc, char *argv[]) {
    int mode = -1; // 0 for seq and 1 for parallel
    int print = 0;
    int test = 0;
    
    // set defaults height width and mines
    int minesnumber = 250;
    int height = 100;
    int width = 100;
    int count=0;
    
    int opt;

    // get values from command line
    while ((opt = getopt(argc, argv, "h:w:n:c:m:pt")) != -1) {
        switch (opt) {
            // height
            case 'h':                   
                height = atoi(optarg);
                break;
            // width
            case 'w':                   
                width = atoi(optarg);
                break;
            // number of mines
            case 'n':                   
                minesnumber = atoi(optarg);
                break;
            case 'c':                   
                count = atoi(optarg);
                break;
            // mode (seq or openmp)
            case 'm':
                mode = atoi(optarg);
                break;
            // print
            case 'p':
                print = 1;
                break;
            // testing
            case 't':
                test = 1;
                break;
        }
    }

    Game* game = new Game(width, height, minesnumber,count);
    game->setMines();


    // testing mode
    if (test) {
        double sequentialressum = 0;
        double openmpsum = 0;

        int sequentialcount = 0;
        int openmpcount = 0;

        if (mode == 0) {
            printf("Test Sequential\n");
            for (int i = 0; i < 100; i++) {
                game = new Game(width, height, minesnumber, count);
                game->setMines();
                double seqsolve = game->seqSolve(i+1);
                bool sequentialresult = game->checkTheResult();
                if (print) {
                    game->printBoard();
                }
                game->resetPlayboardsAndRelatedValues();
                if (sequentialresult) {
                    sequentialcount++;
                    sequentialressum += seqsolve;
                } 
                printf("%d: seq %0.4f ms \n", i, seqsolve * 1000.f * sequentialresult);

            }
            printf("Summary: \n");
            printf("Average Sequential Result: %0.4f ms \n", sequentialressum * 1000.f / sequentialcount);
        } else if (mode == 1) {
            printf("Test OpenMP\n");
            for (int i = 0; i < 100; i++) {
                game = new Game(width, height, minesnumber, count);
                game->setMines();
                double seqsolve = game->seqSolve(i+1);
                bool sequentialresult = game->checkTheResult();
                if (print) {
                    game->printBoard();
                }
                game->resetPlayboardsAndRelatedValues();

                double openmpsolve = game->openmpSolve(i+1);
                bool openmpresult = game->checkTheResult();
                if (print) {
                    game->OMPprintBoard();
                }
                game->resetPlayboardsAndRelatedValues();

                if (sequentialresult) {
                    sequentialcount++;
                    sequentialressum += seqsolve;
                }

                if (openmpresult) {
                    openmpcount++;
                    openmpsum += openmpsolve;
                }
                // printf("%0.4f\n", (sequentialressum * 1000.f) / (openmpsum * 1000.f));

            }
            printf("Summary: \n");
            printf("Average Sequential Result: %0.4f ms \n", sequentialressum * 1000.f / sequentialcount);
            printf("Average OpenMP Result: %0.4f ms \n", openmpsum * 1000.f / openmpcount);
            printf("Speedup: %0.4f \n", (sequentialressum * 1000.f / sequentialcount) /(openmpsum * 1000.f / openmpcount));
            // printf("%d %d \n", sequentialcount, openmpcount);
        } else {
            printf("Invalid Mode\n");
            exit(1);
        }
    }

    // Non testing normal mode
    else {
        // sequential
        if (mode == 0) {
            double seqsolve = game->seqSolve(2);
            bool sequentialresult = game->checkTheResult();
            if (print) {
                game->printBoard();
            }
            if (sequentialresult) {
                printf("Average Sequential Result: %0.4f ms \n", seqsolve * 1000.f);
            } else {
                printf("seq failed because of a mine.\n");
            }
        } else if (mode == 1) {
            game->resetPlayboardsAndRelatedValues();
            double seqsolve = game->seqSolve(2);
            bool sequentialresult = game->checkTheResult();
            if (print) {
                game->printBoard();
            }
            game->resetPlayboardsAndRelatedValues();

            double openmpsolve = game->openmpSolve(2);
            bool openmpresult = game->checkTheResult();
            if (print) {
                game->OMPprintBoard();
            }

            if (sequentialresult && openmpresult) {
                printf("seq %0.3f ms \t omp %0.4f ms \t Speedup: %0.4f \n", seqsolve * 1000.f * sequentialresult, openmpsolve * 1000.f * openmpresult, (seqsolve * 1000.f * sequentialresult) / (openmpsolve * 1000.f * openmpresult));
            } else {
                printf("seq %d or omp %d failed\n",sequentialresult,openmpresult);
            }
        } else {
            printf("Invalid Mode\n");
            exit(1);
        }
    }

    if (print) {
        if (!test) {
            printf("Mines:\n");
            for (int i = 0 ; i < 2*game->minesnumber; i+=2){
                printf("(%d, %d), ",game->playmines[i],game->playmines[i+1]);
            }
            printf("\n");
        }
    }

    return 0;
}