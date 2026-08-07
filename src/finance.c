#include <stdio.h>
#include "include/finance.h"

void execute_purchase(Square *square, Player *player)
{
    player->cash -= square->purchase_price;
    square->ownership = player->playerId;

    printf("\n\t%s purchased %s for LKR %d.\n", player->player_name, square->square_name, square->purchase_price);
    printf("\tRemaining Balance : LKR %d.\n", player->cash);
}

void pay_rent(Square *square, Player *player, Player *owner, int rent)
{
    printf("\n\t%s owned by %s.\n", square->square_name, owner->player_name);
    printf("\tRent Paid : LKR %d.\n", rent);
    player->cash -= rent;
    owner->cash += rent;
    printf("\tRemaining Balance : %d.\n", player->cash);
}

void execute_construction(Square *property, Player *player)
{
    if (property->hasHotel == 0)
    {
        if (property->house_count == MAX_HOUSES)
        {
            property->hasHotel = 1;
            player->cash -= property->hotel_constructionCost;

            printf("%s upgraded %s to a Hotel for LKR %d.\n", player->player_name, property->square_name, property->hotel_constructionCost);
            printf("Remaining Balance: LKR %d.\n", player->cash);
        }
        else
        {
            property->house_count++;
            player->cash -= property->house_constructionCost;
            printf("%s constructed one house on %s for LKR %d.\n", player->player_name, property->square_name, property->house_constructionCost);
            printf("Remaining Balance: LKR %d.\n", player->cash);
        }
    }
}