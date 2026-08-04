#include <stdio.h>
#include "include/board.h"
#include "include/players.h"
#include "include/finance.h"
#include "include/utils.h"

void resolve_property(Square *square, Player *players, Player *player)
{
    if (square->ownership == UNOWNED)
    {
        if (decide_purchase(square, player) == 0)
        {
            return;
        }
        execute_purchase(square, player);
    }
    else if (square->ownership != player->playerId)
    {
        int rent = square->base_rent;
        Player *property_owner = &players[square->ownership];

        if (square->hotel.hasBuild)
        {
            rent *= 10;
        }
        else
        {
            switch (square->house_count)
            {
            case 1:
                rent *= 2;
                break;
            case 2:
                rent *= 3;
                break;
            case 3:
                rent *= 5;
                break;
            case 4:
                rent *= 7;
                break;
            }
        }
        pay_rent(square, player, property_owner, rent);
    }
    else
    {
        if (square->house_count < 4)
        {
            int house_cost = square->house_constructionCost;
            square->houses[square->house_count++].hasBuild = 1;

            player->cash -= house_cost;

            print_heading("Building Construction");
            printf("%s constructed one house on %s.\n\n", player->player_name, square->square_name);
            printf("Construction Cost : LKR %d.\n\n", house_cost);
        }
        else if (square->hotel.hasBuild != 1)
        {
            square->hotel.hasBuild = 1;
            player->cash -= square->hotel_constructionCost;

            print_heading("Hotel Construction");
            printf("%s upgraded %s to a Hotel.\n\n", player->player_name, square->square_name);
        }
    }
}