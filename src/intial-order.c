#include <stdio.h>
#include "include/players.h"
#include "include/game.h"
#include "include/utils.h"

void display_players(PlayerOrder *playerOrder)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        printf("%s rolls %d.\n", playerOrder[i].player->player_name, playerOrder[i].rolled_value);
    }
    printf("\n");
}

void bubbleSort(PlayerOrder *playerOrder)
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

void tie_breaker(PlayerOrder *playerOrder)
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
                if (playerOrder[i].isOrderCorrect)
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

void determine_playerOrder(PlayerOrder *playerOrder)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        playerOrder[i].rolled_value = roll_dice();
        playerOrder[i].rolled_value += roll_dice();
    }

    bubbleSort(playerOrder);
    display_players(playerOrder);

    tie_breaker(playerOrder);

    printf("\n%s will begin the game.\n\n", playerOrder[0].player->player_name);
    printf("Turn order:\n");
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        printf("%s\n", playerOrder[i].player->player_name);
    }
}