#include <stdio.h>
#include "include/game.h"
#include "include/players.h"

void start_game()
{
    Player players[MAX_PLAYERS];

    initialize_players(players);

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        printf("%s", players[i].name);
    }
}