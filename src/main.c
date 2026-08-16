#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/game.h"

int main()
{
    int seed = time(NULL);
    srand(seed);
    start_game();
    return 0;
}