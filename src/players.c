#include "include/players.h"
#include "include/finance.h"
#include "include/utils.h"

#include <stdio.h>

// --------------- BAIL ------------------------
int decide_pay_bail(Player *player)
{
    switch (player->strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return player->cash > BAIL_AMOUNT;
    case CONSERVATIVE_BANKER:
        return 0;
    case RISK_TAKER:
        return player->cash > BAIL_AMOUNT;
    case OPPORTUNISTIC_TRADER:
        return 0; // TODO: add opportunistic bail strategy
    default:
        return 0;
    }
}

// --------------- PURCHASE ------------------------
int aggressive_decide_purchase(Square *board, Square *square, Player *player)
{
    int max_rent = 0;

    for (int i = square->property_index + 1; i < MAX_SQUARES; i++)
    {
        if (board[i].ownership == player->playerId || board[i].ownership == UNOWNED)
        {
            continue;
        }

        if (board[i].square_type == PROPERTY)
        {
            int rent_multiplier[] = {1, 2, 3, 5, 7};
            int rent = board[i].base_rent;

            if (board[i].has_hotel == 1)
            {
                rent *= 10;
            }
            else
            {
                rent *= rent_multiplier[board[i].house_count];
            }

            if (rent > max_rent)
            {
                max_rent = rent;
            }
        }
    }

    return player->cash >= square->purchase_price + max_rent;
}

int conservative_decide_purchase(Square *square, Player *player)
{
    return player->cash / 2 > square->purchase_price;
}

int risk_decide_purchase(Square *square, Player *player)
{
    return player->cash >= square->purchase_price;
}

int opportunistic_decide_purchase(Square *square, Player *player)
{
    // TODO: add opportunistic purchase strategy
    return player->cash >= square->purchase_price;
}

int decide_purchase(Square *board, Square *square, Player *player)
{
    switch (player->strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_purchase(board, square, player);
    case CONSERVATIVE_BANKER:
        return conservative_decide_purchase(square, player);
    case RISK_TAKER:
        return risk_decide_purchase(square, player);
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_purchase(square, player);
    default:
        return 0;
    }
}

// ---------------- LOANS ----------------------

int risk_decide_loan(Square *board, Player player)
{
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        if (board[i].ownership != player.playerId)
        {
            continue;
        }

        return 1; // if there is atleast one property
    }
    return 0;
}

int aggressive_decide_loan(Square *board, Player player)
{
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        if (board[i].ownership != player.playerId)
        {
            continue;
        }

        if (board[i].square_type == RAILWAY || board[i].square_type == UTILITY)
        {
            continue;
        }

        if (board[i].has_hotel == 1)
        {
            continue;
        }

        if (board[i].house_count < 4 && board[i].house_constructionCost > player.cash)
        {
            return 1;
        }
    }

    return 0;
}

int conservative_decide_loan(Square *board, Player player)
{
    // TODO: Add conservative loan decision
    return 1;
}

int opportunistic_decide_loan(Square *board, Player player)
{
    // TODO: Add opportunistic loan decision
    return 1;
}

int decide_loan(Square *board, Player player)
{
    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_loan(board, player);
    case CONSERVATIVE_BANKER:
        return conservative_decide_loan(board, player);
    case RISK_TAKER:
        return risk_decide_loan(board, player);
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_loan(board, player);
    default:
        return 0;
    }
}

// --------------- LOAN - REPAYMENT ---------------

int aggressive_decide_loan_repayment(Player player, int *amount)
{
    if (player.cash > player.loan_amount * 2)
    {
        *amount = player.loan_amount;

        return 1; // repay loan
    }

    return 0;
}

int conservative_decide_loan_repayment(Player player, int *amount)
{
    if (player.cash >= player.loan_amount)
    {
        *amount = player.loan_amount;

        return 1; // repay loan
    }

    return 0;
}

int risk_decide_loan_repayment(Player player, int *amount)
{
    return 0; // never repay loan
}

int opportunistic_decide_loan_repayment(Player player, int *amount)
{
    // TODO: Add opportunistic loan repayment
    return 1;
}

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

// --------------- LOAN - EXTENTION ---------------

int aggressive_decide_loan_extention(Player player)
{
    return 0; // TODO: check aggressive loan extention strategy
}

int conservative_decide_loan_extention(Player player)
{
    return 0; // never extend loan
}

int risk_decide_loan_extention(Player player)
{
    // TODO: check risk loan extention strategy
    return 1; // always extend loan
}

int opportunistic_decide_loan_extention(Player player)
{
    // TODO: Add opportunistic loan extention strategy
    return 0; // never extend loan
}

int decide_loan_extention(Player player)
{
    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_loan_extention(player); // always extend loan
    case CONSERVATIVE_BANKER:
        return conservative_decide_loan_extention(player); // never extend loan
    case RISK_TAKER:
        return risk_decide_loan_extention(player); // always extend loan
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_loan_extention(player); // always extend loan
    default:
        return 0;
    }
}

// --------------- LOAN - REFINANCE ---------------

int aggressive_decide_loan_refinance(Square *board, Player player)
{
    return 0; // TODO: check aggressive loan refinance strategy
}

int conservative_decide_loan_refinance(Square *board, Player player)
{
    return 0; // TODO: check conservative loan refinance strategy
}

int risk_decide_loan_refinance(Square *board, Player player)
{
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        if (board[i].ownership != player.playerId)
        {
            continue;
        }

        if (board[i].is_loan_locked == 0 && board[i].is_mortgage == 0) // if there is atleast one property that is not used as collateral or mortgage
        {
            return 1;
        }
    }

    return 0;
}

int opportunistic_decide_loan_refinance(Square *board, Player player)
{
    return 0; // TODO: check opportunistic loan refinance strategy
}

int decide_loan_refinance(Square *board, Player player)
{
    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_loan_refinance(board, player); // always refinance loan
        break;
    case CONSERVATIVE_BANKER:
        return conservative_decide_loan_refinance(board, player); // never refinance loan
    case RISK_TAKER:
        return risk_decide_loan_refinance(board, player); // always refinance loan
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_loan_refinance(board, player); // always refinance loan
    default:
        return 0;
    }
}

// --------------- CONSTRUCTION ------------------------

int aggressive_decide_construction(Square property, Player player)
{
    if (property.house_count < 4)
    {
        return player.cash >= property.house_constructionCost;
    }
    else if (property.has_hotel == 0)
    {
        return player.cash >= property.hotel_constructionCost;
    }
    return 0;
}

int conservative_decide_construction(Square property, Player player)
{
    if (property.house_count < 4)
    {
        return player.cash >= property.house_constructionCost * 2;
    }
    else if (property.has_hotel == 0 && player.has_active_loan == 0)
    {
        return player.cash >= property.hotel_constructionCost * 2;
    }

    return 0;
}

int risk_decide_construction(Square property, Player player)
{
    if (property.house_count < 4)
    {
        return player.cash >= property.house_constructionCost;
    }
    else if (property.has_hotel == 0)
    {
        return player.cash >= property.hotel_constructionCost;
    }
    return 0;
}

int opportunistic_decide_construction(Square property, Player player)
{
    return 1; // TODO: check opportunistic construction strategy
}

int decide_construction(Square property, Player player)
{
    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_construction(property, player);
    case CONSERVATIVE_BANKER:
        return conservative_decide_construction(property, player);
    case RISK_TAKER:
        return risk_decide_construction(property, player);
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_construction(property, player);
    default:
        return 0;
    }
}

// --------------- AUCTION ------------------------

int aggressive_decide_bid(Square square, Player player, int bidding_price)
{
    return player.cash >= bidding_price && bidding_price <= apply_percentage(square.current_market_value, 120);
}

int conservative_decide_bid(Square square, Player player, int bidding_price)
{
    return player.cash >= bidding_price && bidding_price < square.current_market_value;
}

int risk_decide_bid(Square square, Player player, int bidding_price)
{
    return player.cash >= bidding_price;
}

int opportunistic_decide_bid(Square square, Player player, int bidding_price)
{
    return player.cash >= bidding_price;
}

int decide_bid(Square square, Player player, int bidding_price)
{
    switch (player.strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_bid(square, player, bidding_price);
    case CONSERVATIVE_BANKER:
        return conservative_decide_bid(square, player, bidding_price);
    case RISK_TAKER:
        return risk_decide_bid(square, player, bidding_price);
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_bid(square, player, bidding_price);
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