# Minesweeper

This contains Instructions for running the Minesweeper setup and the solver and to testing setup 
## Compiling the files

To compile and run the game use the follwing command

```g++ -std=c++11 -m64 -O3 -Wall -fopenmp -o minesweeper main.cpp  game.cpp  ompsolver.cpp seqsolver.cpp```

Now to run the game you can use the command

```./minesweeper -h [Height of board] -w [Width of board] -n [Number of mines] -m [mode] -c [thread_count] -t```

Ex: ```./minesweeper -h 200 -w 200 -n 20 -m 1 -c 0 -t``` or ```./minesweeper -h 200 -w 200 -n 20 -m 1 -c 4 -t```

Optionally ```-v``` flag can be used to display the board after each solved board.

The mode is ```0``` for running only sequitial solution and ```1``` for running both sequential and parellel solution along with the speedup.

Thread count is ```0``` if we want to let openmp decide the thread count being used and anything else fixes the thread count.

Please keep the number of mines to less than 0.1 percent of the number of cells as increasing number of mines increases the probabilty of randomly opening a bomb 
