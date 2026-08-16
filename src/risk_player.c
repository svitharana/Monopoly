#include "include/risk_player.h"
#include "include/board.h"
#include "include/types.h"
#include "include/utils.h"

#include <stdio.h>

// --------------- PROPERTY RENOVATION --------------
int risk_decide_property_renovation(Square property, Player player)
{
    int property_depreciation = (property.property_age - 50) / 5;
    if (property_depreciation >= 30)
    {
        return player.cash >= apply_percentage(property.current_market_value, 10);
    }
    return 0;
}

// --------------- RENOVATION ------------------------
int risk_decide_renovation(Square square, Player player, int building_condition)
{
    if (building_condition >= 25)
    {
        return 0;
    }

    int renovation_cost = 0;
    if (square.has_hotel == 1)
    {
        renovation_cost = apply_percentage(square.hotel_constructionCost, HOTEL_RENOVATION_COST_PERCENTAGE);
    }
    else
    {
        renovation_cost = apply_percentage(square.house_constructionCost, HOUSE_RENOVATION_COST_PERCENTAGE);
    }

    return player.cash >= apply_percentage(renovation_cost, 150);
}

// --------------- PURCHASE ------------------------
int risk_decide_purchase(Square *square, Player *player)
{
    return player->cash >= square->purchase_price;
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

// --------------- LOAN - REPAYMENT ---------------
int risk_decide_loan_repayment(Player player, int *amount)
{
    return 0; // never repay loan
}

// --------------- LOAN - COLLATERAL & DESIRED AMOUNT ---------------
int risk_desired_loan_amount(Square *board, Player player)
{
    int total_mortgage_value = 0;
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        Square square = board[i];
        if (square.ownership != player.playerId)
        {
            continue;
        }
        if (square.is_loan_locked == 1 || square.is_mortgage == 1)
        {
            continue;
        }
        total_mortgage_value += square.mortgage_value;
    }
    return apply_percentage(total_mortgage_value, 75);
}

int risk_decide_loan_collateral(Square *board, Player player, int *eligible_properties, int eligible_count, int *selected_collateral, int desired_amount)
{
    for (int i = 0; i < eligible_count; i++)
    {
        selected_collateral[i] = eligible_properties[i];
    }
    return eligible_count;
}

// --------------- LOAN - EXTENTION ---------------
int risk_decide_loan_extention(Player player)
{
    return 1; // always extend loan
}

// --------------- LOAN - REFINANCE ---------------
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

// --------------- CONSTRUCTION ------------------------
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

// --------------- MORTGAGE FOR CONSTRUCTION ------------------------
int risk_decide_mortgage_for_construction(Square *board, Player player, int target_cost)
{
    if (player.cash >= target_cost)
    {
        return 0;
    }

    int accumulated_amount = player.cash;
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        Square square = board[i];

        if (square.ownership != player.playerId)
        {
            continue;
        }

        if (square.is_mortgage == 1 || square.is_loan_locked == 1)
        {
            continue;
        }

        if (square.has_hotel == 1)
        {
            continue;
        }
        else if (square.house_count > 0)
        {
            continue;
        }

        if (square.square_type == PROPERTY)
        {
            if (player_has_monopoly(board, player.playerId, square.property_group) == 1)
            {
                continue;
            }
        }

        accumulated_amount += square.mortgage_value;

        if (accumulated_amount >= target_cost)
        {
            return 1;
        }
    }

    return 0;
}

// --------------- INSURANCE ----------------------
InsuranceType risk_decide_insurance(Square property, Economy economy, Player player)
{
    if (player.incured_loss == 1)
    {
        if (property.has_hotel == 1 || property.house_count > 0)
        {
            return BASIC_PROPERTY;
        }
    }
    return NO_INSURANCE;
}

// --------------- AUCTION ------------------------
int risk_decide_bid(Square square, Player player, int bidding_price)
{
    return player.cash >= bidding_price;
}
