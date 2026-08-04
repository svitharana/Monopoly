#include <stdio.h>
#include "include/game.h"
#include "include/players.h"
#include "include/board.h"
#include "include/utils.h"

static int roll_dice()
{
    return random_generator(6);
}

static void display_players(PlayerOrder *playerOrder)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        printf("%s rolls %d.\n", playerOrder[i].player->player_name, playerOrder[i].rolled_value);
    }
    printf("\n");
}

static void bubble_sort(PlayerOrder *playerOrder)
{
    while (1)
    {
        int swapped = 0;

        for (int i = 0; i < MAX_PLAYERS - 1; i++)
        {
            PlayerOrder player = playerOrder[i];
            if (playerOrder[i].isOrderCorrect || playerOrder[i + 1].isOrderCorrect)
            {
                continue;
            }

            if (playerOrder[i].rolled_value < playerOrder[i + 1].rolled_value)
            {
                PlayerOrder temp = playerOrder[i + 1];
                playerOrder[i + 1] = playerOrder[i];
                playerOrder[i] = temp;
                swapped = 1;
            }
        }
        if (swapped == 0)
            break;
    }
}

static void tie_breaker(PlayerOrder *playerOrder)
{
    int tied;

    do
    {
        tied = 0;
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            if (playerOrder[i].isOrderCorrect)
            {
                continue;
            }
            int isTied = 0;
            for (int j = i + 1; j < MAX_PLAYERS; j++)
            {
                if (playerOrder[i].rolled_value != playerOrder[j].rolled_value)
                {
                    break;
                }
                if (playerOrder[j].isOrderCorrect)
                {
                    break;
                }
                isTied = 1;
                tied = 1;
                i = j;
            }
            if (!isTied)
            {
                playerOrder[i].isOrderCorrect = 1;
            }
        }
        if (tied)
        {
            for (int i = 0; i < MAX_PLAYERS; i++)
            {
                if (!playerOrder[i].isOrderCorrect)
                {
                    playerOrder[i].rolled_value = roll_dice();
                    playerOrder[i].rolled_value += roll_dice();
                    printf("%s was tied so he rolled again and got %d\n", playerOrder[i].player->player_name, playerOrder[i].rolled_value);
                }
            }
            bubbleSort(playerOrder);
        }
    } while (tied);
}

static void determine_playerOrder(PlayerOrder *playerOrder)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        playerOrder[i].rolled_value = roll_dice();
        playerOrder[i].rolled_value += roll_dice();
    }

    bubble_sort(playerOrder);
    display_players(playerOrder);

    tie_breaker(playerOrder);

    printf("\n%s will begin the game.\n\n", playerOrder[0].player->player_name);

    printf("Turn order:\n");
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        printf("%s\n", playerOrder[i].player->player_name);
    }

    printf("\n");
}

static void initial_msg(Player *players)
{
    print_heading("MONOPOLY-LK Simulation");
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        printf("Player %d: %s\n", i + 1, players[i].player_name);
    }
    printf("\nEach player begins with LKR %d.\n", INITIAL_CASH);
}

static void play_turn(Player *players, PlayerId player_id, Square *board)
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