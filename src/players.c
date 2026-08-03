#include "include/players.h"

int decide_purchase(Square *square, Player *player)
{
    return player->cash >= (player->cash - square->purchase_price);
}

void initialize_players(Player *players, PlayerOrder *playerOrder)
{
    // Aggressive Investor
    players[AGGRESSIVE_INVESTOR].playerId = PLAYER_1;
    players[AGGRESSIVE_INVESTOR].player_name = "Aggressive Investor";

    // Conservative Banker
    players[CONSERVATIVE_BANKER].playerId = PLAYER_2;
    players[CONSERVATIVE_BANKER].player_name = "Conservative Banker";

    // Risk Taker
    players[RISK_TAKER].playerId = PLAYER_3;
    players[RISK_TAKER].player_name = "Risk Taker";

    // Opportunistic Trader
    players[OPPORTUNISTIC_TRADER].playerId = PLAYER_4;
    players[OPPORTUNISTIC_TRADER].player_name = "Opportunistic Trader";

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        players[i].cash = 30000;
        players[i].rolled_value = 0;
        players[i].current_position = 0;

        playerOrder[i].player = &players[i];
        playerOrder[i].rolled_value = 0;
        playerOrder[i].isOrderCorrect = 0;
    }
}