#include "include/players.h"

int decide_purchase(Square *square, Player *player)
{
    return player->cash >= square->purchase_price;
}

void initialize_players(Player *players, PlayerOrder *playerOrder)
{
    // Aggressive Investor
    players[PLAYER_1].playerId = PLAYER_1;
    players[PLAYER_1].player_name = "Aggressive Investor";
    players[PLAYER_1].strategy = AGGRESSIVE_INVESTOR;

    // Conservative Banker
    players[PLAYER_2].playerId = PLAYER_2;
    players[PLAYER_2].player_name = "Conservative Banker";
    players[PLAYER_2].strategy = CONSERVATIVE_BANKER;

    // Risk Taker
    players[PLAYER_3].playerId = PLAYER_3;
    players[PLAYER_3].player_name = "Risk Taker";
    players[PLAYER_3].strategy = RISK_TAKER;

    // Opportunistic Trader
    players[PLAYER_4].playerId = PLAYER_4;
    players[PLAYER_4].player_name = "Opportunistic Trader";
    players[PLAYER_4].strategy = AGGRESSIVE_INVESTOR;

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        players[i].cash = INITIAL_CASH;
        players[i].rolled_value = 0;
        players[i].current_position = 0;

        playerOrder[i].player = &players[i];
        playerOrder[i].rolled_value = 0;
        playerOrder[i].isOrderCorrect = 0;
    }
}