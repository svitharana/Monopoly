#include "include/players.h"
#include "include/finance.h"
#include "include/utils.h"

#include <stdio.h>

int decide_purchase(Square *square, Player *player)
{
    // TODO: Remove
    if (player->strategy == AGGRESSIVE_INVESTOR)
    {
        return player->cash >= square->purchase_price;
    }
    return 0;
}

int decide_construction(Square property, Player player)
{

    if (property.house_count == MAX_HOUSES)
    {
        return player.cash >= property.hotel_constructionCost;
    }

    return player.cash >= property.house_constructionCost;
}

int decide_bid(Square square, Player player, int bidding_price)
{
    return player.cash >= bidding_price;
}

int decide_loan_repayment(Player player, int *amount)
{
    int loan_amount_with_interest = calculate_loan_payable(player);

    if (player.cash >= loan_amount_with_interest * 2)
    {
        *amount = loan_amount_with_interest;

        return 1; // repay loan
    }

    return 0; // decline repayment
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
    players[PLAYER_4].strategy = OPPORTUNISTIC_TRADER;

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        players[i].cash = INITIAL_CASH;
        players[i].rolled_value = 0;
        players[i].current_position = 0;
        players[i].player_round = 1;

        playerOrder[i].player = &players[i];
        playerOrder[i].rolled_value = 0;
        playerOrder[i].isOrderCorrect = 0;
    }
}