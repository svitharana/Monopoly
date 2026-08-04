#include "include/board.h"
#include "include/players.h"
#include "include/finance.h"

void resolve_railwayStation(Square *board, Square *square, Player *players, Player *player)
{
    if (square->ownership == UNOWNED)
    {
        if (!decide_purchase(square, player))
        {
            return;
        }
        execute_purchase(square, player);
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

        pay_rent(square, player, railwayStation_owner, rent);
    }
}