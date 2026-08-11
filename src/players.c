#include "include/players.h"
#include "include/finance.h"
#include "include/utils.h"

#include <stdio.h>

// --------------- PURCHASE ------------------------
int aggressive_decide_purchase(Square *square, Player *player)
{
    int offset = 1;
    int rent = 0;
    while (1)
    {
        Square *next_property = (square + offset);

        rent = next_property->base_rent;

        offset++;
        if (offset == 40)
        {
            offset = 0;
        }

        if (next_property->ownership == UNOWNED || next_property->ownership == player->playerId)
        {
            continue;
        }

        if (next_property->square_type == PROPERTY)
        {
            int rent_multiplier[] = {1, 2, 3, 5, 7};

            if (square->has_hotel == 1)
            {
                rent *= 10;
            }
            else
            {
                rent *= rent_multiplier[next_property->house_count];
            }

            break;
        } // TODO: see if i can add railway as well
    }

    return player->cash >= rent;
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
    // TODO: opportunistic purchase strategy
    return 1;
}

int decide_purchase(Square *square, Player *player)
{
    switch (player->strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_purchase(square, player);
        break;
    case CONSERVATIVE_BANKER:
        return conservative_deicde_purchase(square, player);
        break;
    case RISK_TAKER:
        return risk_decide_purchase(square, player);
        break;
    case OPPORTUNISTIC_TRADER:
        return opportunistic_decide_purchase(square, player);
        break;
    default:
        break;
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
    if (bidding_price <= (square.purchase_price + apply_percentage(square.purchase_price, 20)))
    {
        return player.cash >= bidding_price;
    }
    return 0; // withdrawn
}

// LOANs

int decide_loan(Square *board, Player player)
{
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        if (board[i].ownership != player.playerId)
        {
            continue;
        }

        return 1; // if there is atleast property
    }

    return 0;
}

int decide_loan_extention(Player player)
{
    return player.loan_rounds_remaining < 5;
}

int decide_loan_refiance(Square *board, Player player)
{
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        if (board[i].ownership != player.playerId)
        {
            continue;
        }

        return board[i].is_loan_locked == 0; // if there is atleast one property that is not used as collateral
    }
}

int decide_loan_repayment(Player player, int *amount)
{
    // int loan_amount_with_interest = calculate_loan_payable(player);

    if (player.cash >= player.loan_amount * 2)
    {
        *amount = player.loan_amount;

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