#include "include/board.h"
#include "include/players.h"
#include "include/finance.h"

void resolve_utilitySquare(Square *board, Square *square, Player *players, Player *player)
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

        Player *utilityCompany_owner = &players[square->ownership];
        int utilityCompany_count = 0;

        for (int i = 0; i < MAX_SQUARES; i++)
        {
            if (board[i].square_type == UTILITY && board[i].ownership == utilityCompany_owner->playerId)
            {
                utilityCompany_count++;
            }
        }

        switch (utilityCompany_count)
        {
        case 1:
            rent *= 2;
            break;
        case 2:
            rent *= 7;
            break;
        default:
            break;
        }
        pay_rent(square, player, utilityCompany_owner, rent);
    }
}