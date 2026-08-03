#include <stdio.h>
#include "include/board.h"
#include "include/utils.h"

void resolve_railwayStation(Square *board, Square *square, Player *players, Player *player)
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

        Player *railwayStation_owner = &players[square->ownership];
        int railwayStation_count = 0;

        for (int i = 0; i < MAX_SQUARES; i++)
        {
            if (board[i].square_type == RAILWAY && board[i].ownership == railwayStation_owner->playerId)
            {
                railwayStation_count++;
            }
        }

        rent *= railwayStation_count;

        print_heading("Paying Rent");
        printf("%s landed on %s.\n\n", player->player_name, square->square_name);
        printf("Rent Paid : LKR %d.\n\n", rent);
        printf("Owner : %s.\n\n", railwayStation_owner->player_name);
    }
}