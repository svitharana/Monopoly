
#include "include/players.h"

void initialize_players(Player *players, PlayerOrder *playerOrder)
{
    // Aggressive Investor
    players[AGGRESSIVE_INVESTOR].playerId = PLAYER_1;
    players[AGGRESSIVE_INVESTOR].name = "Aggressive Investor";

    // Conservative Banker
    players[CONSERVATIVE_BANKER].playerId = PLAYER_2;
    players[CONSERVATIVE_BANKER].name = "Conservative Banker";

    // Risk Taker
    players[RISK_TAKER].playerId = PLAYER_3;
    players[RISK_TAKER].name = "Risk Taker";

    // Opportunistic Trader
    players[OPPORTUNISTIC_TRADER].playerId = PLAYER_4;
    players[OPPORTUNISTIC_TRADER].name = "Opportunistic Trader";

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