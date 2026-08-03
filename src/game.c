#include <stdio.h>
#include "include/game.h"
#include "include/players.h"
#include "include/board.h"
#include "include/utils.h"

void initial_msg(Player *players)
{
    print_heading("MONOPOLY-LK Simulation");
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        printf("Player %d: %s\n", i + 1, players[i].player_name);
    }
    printf("\nEach player begins with LKR %d.\n", INITIAL_CASH);
}

int roll_dice()
{
    return random_generator(6);
}

void play_turn(Player *players, PlayerId player_id, Square *board)
{
    Player *player = &players[player_id];
    int dice_1 = roll_dice();
    int dice_2 = roll_dice();

    player->rolled_value = dice_1 + dice_2;
    // player->rolled_value = 5;
    print_heading("Dice Roll");
    printf("%s rolled %d.\n\n", player->player_name, player->rolled_value);

    print_heading("Player Movement");
    move_player(player, player->rolled_value);

    resolve_landingSquare(board, players, player);
}

void start_game()
{
    Square board[MAX_SQUARES] = {0};
    Player players[MAX_PLAYERS] = {0};
    PlayerOrder playerOrder[MAX_PLAYERS] = {0};

    int round = 1;
    int turn = 1;

    initialize_board(board);
    initialize_players(players, playerOrder);
    initial_msg(players);

    determine_playerOrder(playerOrder);

    while (round <= 6)
    {
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            play_turn(players, playerOrder[i].player->playerId, board);
        }
        round++;
    }
}