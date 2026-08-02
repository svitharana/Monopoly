#include <stdio.h>
#include "include/board.h"
#include "include/utils.h"

void resolve_property(Square *square, Player *players, Player *player)
{
    if (square->ownership == UNOWNED)
    {
        player->cash -= square->purchase_price;
        square->ownership = player->playerId;

        print_heading("Purchasing Property");
        printf("%s purchased %s for LKR %d.\n\n", player->player_name, square->square_name, square->purchase_price);
        printf("Remaining Balance : LKR %d.\n\n", player->cash);
    }
    else if (square->ownership != player->playerId)
    {
        int rent = square->base_rent;
        Player property_owner = players[square->ownership];

        print_heading("Paying Rent");
        printf("%s landed on %s.\n\n", player->player_name, square->square_name);
        printf("Rent Paid : LKR %d.\n\n", rent);
        printf("Owner : %s.\n\n", property_owner.player_name);

        player->cash -= rent;
        property_owner.cash += rent;
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