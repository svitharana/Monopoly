#include "include/players.h"
#include "include/finance.h"
#include "include/types.h"

#include "include/aggressive_player.h"
#include "include/conservative_player.h"
#include "include/risk_player.h"
#include "include/opportunistic_player.h"

#include <stdio.h>

// --------------- BAIL ------------------------
int decide_pay_bail(Square *board, Player *player)
{
    switch (player->strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return player->cash >= BAIL_AMOUNT;
    case CONSERVATIVE_BANKER:
        return 0;
    case RISK_TAKER:
        return player->cash >= BAIL_AMOUNT;
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_pay_bail(board, player);
    default:
        return 0;
    }
}

// --------------- PROPERTY RENOVATION --------------
int decide_property_renovation(Square square, Player player)
{
    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_property_renovation(square, player);
    case CONSERVATIVE_BANKER:
        return conservative_decide_property_renovation(square, player);
    case RISK_TAKER:
        return risk_decide_property_renovation(square, player);
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_property_renovation(square, player);
    default:
        return 0;
    }
}

// --------------- RENOVATION ------------------------
int decide_renovation(Square square, Player player, int building_condition)
{
    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_renovation(square, player, building_condition);
    case CONSERVATIVE_BANKER:
        return conservative_decide_renovation(square, player, building_condition);
    case RISK_TAKER:
        return risk_decide_renovation(square, player, building_condition);
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_renovation(square, player, building_condition);
    default:
        return 0;
    }
}

// --------------- PURCHASE ------------------------
int decide_purchase(Square *board, Square *square, Player *player, Economy economy)
{
    if (economy.active_government_regulation == ANTI_SPECULATION_ACT && square->square_type == PROPERTY)
    {
        int undeveloped_count = 0;
        for (int i = 0; i < MAX_SQUARES; i++)
        {
            if (board[i].square_type != PROPERTY || board[i].ownership != player->playerId)
            {
                continue;
            }
            if (board[i].house_count == 0 && board[i].has_hotel == 0)
            {
                undeveloped_count++;
            }
        }
        if (undeveloped_count >= 3)
        {
            return 0;
        }
    }

    switch (player->strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_purchase(board, square, player);
    case CONSERVATIVE_BANKER:
        return conservative_decide_purchase(square, player, economy);
    case RISK_TAKER:
        return risk_decide_purchase(square, player);
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_purchase(board, square, player, economy);
    default:
        return 0;
    }
}

// ---------------- LOANS ----------------------
int decide_loan(Square *board, Player player, Economy economy)
{
    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_loan(board, player);
    case CONSERVATIVE_BANKER:
        return conservative_decide_loan(board, player, economy);
    case RISK_TAKER:
        return risk_decide_loan(board, player);
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_loan(board, player, economy);
    default:
        return 0;
    }
}

// --------------- LOAN - REPAYMENT ---------------
int decide_loan_repayment(Player player, int *amount)
{
    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_loan_repayment(player, amount);
    case CONSERVATIVE_BANKER:
        return conservative_decide_loan_repayment(player, amount);
    case RISK_TAKER:
        return risk_decide_loan_repayment(player, amount);
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_loan_repayment(player, amount);
    default:
        return 0;
    }
}

// --------------- LOAN - COLLATERAL & DESIRED AMOUNT ---------------
int decide_loan_collateral(Square *board, Player player, int *eligible_properties, int eligible_count, int *selected_collateral, Economy economy)
{
    (void)economy;
    int desired_amount = 0;
    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
        desired_amount = aggressive_desired_loan_amount(board, player);
        return aggressive_decide_loan_collateral(board, player, eligible_properties, eligible_count, selected_collateral, desired_amount);
    case CONSERVATIVE_BANKER:
        desired_amount = conservative_desired_loan_amount(board, player);
        return conservative_decide_loan_collateral(board, player, eligible_properties, eligible_count, selected_collateral, desired_amount);
    case RISK_TAKER:
        desired_amount = risk_desired_loan_amount(board, player);
        return risk_decide_loan_collateral(board, player, eligible_properties, eligible_count, selected_collateral, desired_amount);
    case OPPORTUNISTIC_TRADER:
        desired_amount = opportunistic_desired_loan_amount(board, player);
        return opportunistic_decide_loan_collateral(board, player, eligible_properties, eligible_count, selected_collateral, desired_amount);
    default:
        return 0;
    }
}

// --------------- LOAN - EXTENTION ---------------
int decide_loan_extention(Player player)
{
    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_loan_extention(player);
    case CONSERVATIVE_BANKER:
        return conservative_decide_loan_extention(player);
    case RISK_TAKER:
        return risk_decide_loan_extention(player);
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_loan_extention(player);
    default:
        return 0;
    }
}

// --------------- LOAN - REFINANCE ---------------
int decide_loan_refinance(Square *board, Player player)
{
    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_loan_refinance(board, player);
    case CONSERVATIVE_BANKER:
        return conservative_decide_loan_refinance(board, player);
    case RISK_TAKER:
        return risk_decide_loan_refinance(board, player);
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_loan_refinance(board, player);
    default:
        return 0;
    }
}

// --------------- CONSTRUCTION ------------------------
int decide_construction(Square property, Player player, Economy economy)
{
    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_construction(property, player);
    case CONSERVATIVE_BANKER:
        return conservative_decide_construction(property, player, economy);
    case RISK_TAKER:
        return risk_decide_construction(property, player);
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_construction(property, player, economy);
    default:
        return 0;
    }
}

// --------------- MORTGAGE FOR CONSTRUCTION ------------------------
int decide_mortgage_for_construction(Square *board, Player player, int target_cost)
{
    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_mortgage_for_construction(board, player, target_cost);
    case CONSERVATIVE_BANKER:
        return conservative_decide_mortgage_for_construction(board, player, target_cost);
    case RISK_TAKER:
        return risk_decide_mortgage_for_construction(board, player, target_cost);
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_mortgage_for_construction(board, player, target_cost);
    default:
        return 0;
    }
}

// --------------- INSURANCE ----------------------
InsuranceType decide_insurance(Square property, Economy economy, Player player)
{
    InsuranceType insurance_type;

    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
        insurance_type = aggressive_decide_insurance(property, economy, player);
        break;
    case CONSERVATIVE_BANKER:
        insurance_type = conservative_decide_insurance(property, economy, player);
        break;
    case RISK_TAKER:
        insurance_type = risk_decide_insurance(property, economy, player);
        break;
    case OPPORTUNISTIC_TRADER:
        insurance_type = opportunistic_decide_insurance(property, economy, player);
        break;
    default:
        insurance_type = NO_INSURANCE;
        break;
    }
    if (insurance_type == NO_INSURANCE)
    {
        return insurance_type;
    }

    int premium = calculate_insurance_premium(property, insurance_type, economy, player);

    if (player.cash >= premium)
    {
        return insurance_type;
    }

    return NO_INSURANCE;
}

// --------------- AUCTION ------------------------
int decide_bid(Square square, Player player, int bidding_price, Economy economy)
{
    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_bid(square, player, bidding_price);
    case CONSERVATIVE_BANKER:
        return conservative_decide_bid(square, player, bidding_price, economy);
    case RISK_TAKER:
        return risk_decide_bid(square, player, bidding_price);
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_bid(square, player, bidding_price, economy);
    default:
        return 0;
    }
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