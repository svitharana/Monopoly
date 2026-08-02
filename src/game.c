#include <stdio.h>
#include "include/game.h"
#include "include/players.h"
#include "include/board.h"
#include "include/utils.h"

void initial_msg(Player *players)
{
    printf("MONOPOLY-LK Simulation\n\n");
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        printf("Player %d: %s\n", i + 1, players[i].player_name);
    }
    printf("\nEach player begins with ");
    format_amount(30000);
}

void start_game()
{
    Square board[MAX_SQUARES] = {0};
    Player players[MAX_PLAYERS] = {0};
    PlayerOrder playerOrder[MAX_PLAYERS] = {0};

    initialize_board(board);
    initialize_players(players, playerOrder);
    // initial_msg(players);

    determine_playerOrder(playerOrder);
}