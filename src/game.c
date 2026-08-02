#include <stdio.h>
#include "include/game.h"
#include "include/players.h"
#include "include/utils.h"

void initial_msg(Player *players)
{
    printf("MONOPOLY-LK Simulation\n\n");
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        printf("Player %d: %s\n", i + 1, players[i].name);
    }
    printf("\nEach player begins with ");
    format_amount(30000);
}

void start_game()
{
    Player players[MAX_PLAYERS];
    PlayerOrder playerOrder[MAX_PLAYERS];

    initialize_players(players, playerOrder);
    initial_msg(players);

    determine_playerOrder(playerOrder);

    // for (int i = 0; i < MAX_PLAYERS; i++)
    // {
    //     printf("%s", players[i].name);
    // }
}