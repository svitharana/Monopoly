#include <stdio.h>
#include "include/finance.h"
#include "include/utils.h"

void execute_purchase(Square *square, Player *player)
{
    player->cash -= square->purchase_price;
    square->ownership = player->playerId;

    print_heading("Purchasing Property");
    printf("%s purchased %s for LKR %d.\n\n", player->player_name, square->square_name, square->purchase_price);
    printf("Remaining Balance : LKR %d.\n\n", player->cash);
}

void pay_rent(Square *square, Player *player, Player *owner, int rent)
{
    print_heading("Paying Rent");
    printf("%s landed on %s.\n\n", player->player_name, square->square_name);
    printf("Rent Paid : LKR %d.\n\n", rent);
    printf("Owner : %s.\n\n", owner->player_name);

    player->cash -= rent;
    owner->cash += rent;
}