#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/game.h"

int main(int argc, char *argv[])
{
    int seed;
    if (argc > 1) {
        seed = atoi(argv[1]);
    } else {
        seed = time(NULL);
    }
    srand(seed);
    printf("Seed :%d\n\n", seed);
    start_game();
    return 0;
}