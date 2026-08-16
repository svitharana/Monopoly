#include "include/conservative_player.h"
#include "include/board.h"
#include "include/finance.h"
#include "include/types.h"
#include "include/utils.h"

#include <stdio.h>

// --------------- PROPERTY RENOVATION --------------
int conservative_decide_property_renovation(Square property, Player player)
{
    int property_depreciation = (property.property_age - 50) / 5;
    if (property_depreciation >= 10)
    {
        return player.cash >= apply_percentage(property.current_market_value, 10);
    }

    return 0;
}

// --------------- RENOVATION ------------------------
int conservative_decide_renovation(Square square, Player player, int building_condition)
{
    if (building_condition >= 90)
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

    if (building_condition < 60)
    {
        renovation_cost = apply_percentage(renovation_cost, 150); 
    }

    return player.cash > renovation_cost;
}

// --------------- PURCHASE ------------------------
int conservative_decide_purchase(Square *square, Player *player, Economy economy)
{
    // economic recessions
    if (economy.active_economic_event == ECONOMIC_RECESSION)
    {
        return 0;
    }

    // railway stations and utility companies
    if (square->square_type == RAILWAY || square->square_type == UTILITY)
    {
        return player->cash >= apply_percentage(square->purchase_price, 150);
    }

    return player->cash >= square->purchase_price * 2;
}

// ---------------- LOANS ----------------------
int conservative_decide_loan(Square *board, Player player, Economy economy)
{
    int max_expense = get_max_opponent_rent(board, player);

    if (player.cash < max_expense)
    {
        return 1;
    }

    return 0;
}

// --------------- LOAN - REPAYMENT ---------------
int conservative_decide_loan_repayment(Player player, int *amount)
{
    if (player.cash >= player.loan_amount)
    {
        *amount = player.loan_amount;
        return 1;
    }

    // Partial repayment
    int patial_amount;

    patial_amount = apply_percentage(player.loan_amount, 25);
    if (patial_amount > 0 && player.cash >= patial_amount)
    {
        *amount = patial_amount;
        return 1;
    }

    patial_amount = apply_percentage(player.loan_amount, 15);
    if (patial_amount > 0 && player.cash >= patial_amount)
    {
        *amount = patial_amount;
        return 1;
    }

    patial_amount = apply_percentage(player.loan_amount, 10);
    if (patial_amount > 0 && player.cash >= patial_amount)
    {
        *amount = patial_amount;
        return 1;
    }

    return 0;
}

// --------------- LOAN - COLLATERAL & DESIRED AMOUNT ---------------
int conservative_desired_loan_amount(Square *board, Player player)
{
    int max_expense = get_max_opponent_rent(board, player);
    int needed_funds = max_expense - player.cash;

    if (needed_funds > 0)
    {
        return needed_funds;
    }

    return 0;
}

int conservative_decide_loan_collateral(Square *board, Player player, int *eligible_properties, int eligible_count, int *selected_collateral, int desired_amount)
{
    if (desired_amount <= 0)
    {
        return 0;
    }

    int selected_count = 0;
    int accumulated_amount = 0;
    int considered_properties[MAX_SQUARES] = {0};

  
    while (accumulated_amount < desired_amount)
    {
        int suitable_property_index = -1;
        int lowest_mortgage = 9999999;

        for (int i = 0; i < eligible_count; i++)
        {
            int property_index = eligible_properties[i];
            if (considered_properties[property_index] == 1)
            {
                continue;
            }

            Square property = board[property_index];

            // Avoid monopolies and railways/utilities in first pass
            if (property.square_type == PROPERTY)
            {
                if (player_has_monopoly(board, player.playerId, property.property_group) == 1)
                {
                    continue;
                }
            }
            else if (property.square_type == RAILWAY || property.square_type == UTILITY)
            {
                continue; 
            }

            if (property.mortgage_value < lowest_mortgage)
            {
                lowest_mortgage = property.mortgage_value;
                suitable_property_index = property_index;
            }
        }

        if (suitable_property_index == -1)
        {
            break;
        }

        considered_properties[suitable_property_index] = 1;
        selected_collateral[selected_count++] = suitable_property_index;
        accumulated_amount += apply_percentage(board[suitable_property_index].mortgage_value, 75);
    }

    return selected_count;
}

// --------------- LOAN - EXTENTION ---------------
int conservative_decide_loan_extention(Player player)
{
    return 0; // never extend loan
}

// --------------- LOAN - REFINANCE ---------------
int conservative_decide_loan_refinance(Square *board, Player player)
{
    return 0; 
}

// --------------- CONSTRUCTION ------------------------
int conservative_decide_construction(Square property, Player player, Economy economy)
{
    if (economy.active_economic_event == ECONOMIC_RECESSION)
    {
        return 0;
    }

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

// --------------- MORTGAGE FOR CONSTRUCTION ------------------------
int conservative_decide_mortgage_for_construction(Square *board, Player player, int target_cost)
{ 
    return 0;
}

// --------------- INSURANCE ----------------------
InsuranceType conservative_decide_insurance(Square property, Economy economy, Player player)
{
    if (property.has_hotel == 1 || property.house_count > 0)
    {
        return COMPREHENSIVE;
    }
    return NO_INSURANCE;
}

// --------------- AUCTION ------------------------
int conservative_decide_bid(Square square, Player player, int bidding_price, Economy economy)
{
    
    if (economy.active_economic_event == ECONOMIC_RECESSION)
    {
        return 0;
    }

    return player.cash >= bidding_price && bidding_price < square.current_market_value;
}
