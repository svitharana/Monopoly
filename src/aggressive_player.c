#include "include/aggressive_player.h"
#include "include/board.h"
#include "include/finance.h"
#include "include/types.h"
#include "include/utils.h"

#include <stdio.h>

// --------------- PROPERTY RENOVATION --------------
int aggressive_decide_property_renovation(Square property, Player player)
{
    int property_depreciation = (property.property_age - 50) / 5;
    int cost = apply_percentage(property.current_market_value, 10);
    if (property_depreciation >= 5)
    {
        return player.cash >= cost * 2;
    }
    return 0;
}

// --------------- RENOVATION ------------------------
int aggressive_decide_renovation(Square square, Player player, int building_condition)
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
        renovation_cost = apply_percentage(renovation_cost, 150); // Increase cost by 50% if condition is below 60
    }

    return player.cash > renovation_cost;
}

// --------------- PURCHASE ------------------------
int aggressive_decide_purchase(Square *board, Square *square, Player *player)
{
    // Nuwara Eliya (37) Galle Face (39)
    if (square->property_index == 37 || square->property_index == 39)
    {
        return player->cash >= square->purchase_price;
    }

    // Monopoly
    if (square->square_type == PROPERTY)
    {
        for (int i = 0; i < MAX_SQUARES; i++)
        {
            if (board[i].square_type == PROPERTY &&
                board[i].property_group == square->property_group &&
                board[i].ownership == player->playerId)
            {
                return player->cash >= square->purchase_price;
            }
        }
    }

    int max_rent = 0; // highest rent currently

    for (int i = 0; i < MAX_SQUARES; i++)
    {
        if (board[i].ownership == player->playerId || board[i].ownership == UNOWNED || board[i].is_mortgage == 1)
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
        else if (board[i].square_type == RAILWAY)
        {
            PlayerId railwayStation_owner = board[i].ownership;
            int railwayStation_count = 0;
            int rent = board[i].base_rent;
            for (int j = 0; j < MAX_SQUARES; j++)
            {
                if (board[j].square_type == RAILWAY && board[j].ownership == railwayStation_owner && board[j].is_mortgage == 0)
                {
                    railwayStation_count++;
                }
            }

            int multiplier = 1 << (railwayStation_count - 1);
            rent *= multiplier;

            if (rent > max_rent)
            {
                max_rent = rent;
            }
        }
        else if (board[i].square_type == UTILITY)
        {
            PlayerId utilityCompany_owner = board[i].ownership;
            int utilityCompany_count = 0;
            int rent_multiplier[] = {0, 4, 10};

            for (int j = 0; j < MAX_SQUARES; j++)
            {
                if (board[j].square_type == UTILITY && board[j].ownership == utilityCompany_owner && board[j].is_mortgage == 0)
                {
                    utilityCompany_count++;
                }
            }
            int standard_rent = 12 * rent_multiplier[utilityCompany_count];
            int rent = apply_percentage(standard_rent, board[i].base_rent);
            if (rent > max_rent)
            {
                max_rent = rent;
            }
        }
    }

    return player->cash >= square->purchase_price + max_rent;
}

// ---------------- LOANS ----------------------
int aggressive_decide_loan(Square *board, Player player)
{
    for (int group = 0; group < MAX_PROPERTY_GRPS; group++)
    {
        if (player_has_monopoly(board, player.playerId, (PropertyGroup)group) == 1)
        {
            for (int i = 0; i < MAX_SQUARES; i++)
            {
                Square property = board[i];

                if (property.ownership != player.playerId || property.square_type != PROPERTY || property.property_group != (PropertyGroup)group)
                {
                    continue;
                }

                if (property.has_hotel == 1)
                {
                    continue;
                }

                if (property.house_count == 4 && property.hotel_constructionCost > player.cash)
                {
                    return 1;
                }

                if (property.house_count < 4 && property.house_constructionCost > player.cash)
                {
                    return 1;
                }
            }
        }
    }

    return 0;
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

// --------------- LOAN - COLLATERAL & DESIRED AMOUNT ---------------
int aggressive_desired_loan_amount(Square *board, Player player)
{
    int development_cost = 0;
    for (int group = 0; group < MAX_PROPERTY_GRPS; group++)
    {
        if (player_has_monopoly(board, player.playerId, (PropertyGroup)group) == 1)
        {
            for (int i = 0; i < MAX_SQUARES; i++)
            {
                Square property = board[i];
                if (property.ownership != player.playerId || property.square_type != PROPERTY || property.property_group != (PropertyGroup)group)
                {
                    continue;
                }

                if (property.has_hotel == 0)
                {
                    int remaining_houses = 4 - property.house_count;
                    development_cost += remaining_houses * property.house_constructionCost;
                    development_cost += property.hotel_constructionCost;
                }
            }
        }
    }

    int needed_funds = development_cost - player.cash;

    if (needed_funds > 0)
    {
        return needed_funds;
    }

    return 0;
}

int aggressive_decide_loan_collateral(Square *board, Player player, int *eligible_properties, int eligible_count, int *selected_collateral, int desired_amount)
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

            if (property.square_type == PROPERTY)
            {
                if (player_has_monopoly(board, player.playerId, property.property_group) == 1)
                {
                    continue;
                }
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
int aggressive_decide_loan_extention(Player player)
{
    return player.cash <= player.loan_amount * 2;
}

// --------------- LOAN - REFINANCE ---------------
int aggressive_decide_loan_refinance(Square *board, Player player)
{
    int has_unpledged_collateral = 0;
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        Square property = board[i];

        if (property.ownership != player.playerId || property.is_loan_locked == 1 || property.is_mortgage == 1)
        {
            continue;
        }

        has_unpledged_collateral = 1;
        break;
    }

    if (has_unpledged_collateral == 0)
    {
        return 0;
    }

    for (int group = 0; group < MAX_PROPERTY_GRPS; group++)
    {
        if (player_has_monopoly(board, player.playerId, (PropertyGroup)group))
        {
            for (int i = 0; i < MAX_SQUARES; i++)
            {
                Square property = board[i];

                if (property.ownership != player.playerId || property.square_type != PROPERTY || property.property_group != (PropertyGroup)group)
                {
                    continue;
                }

                if (property.has_hotel == 1)
                {
                    continue;
                }

                if (property.house_count == 4 && property.hotel_constructionCost > player.cash)
                {
                    return 1;
                }

                if (property.house_count < 4 && property.house_constructionCost > player.cash)
                {
                    return 1;
                }
            }
        }
    }

    return 0;
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

// --------------- MORTGAGE FOR CONSTRUCTION ------------------------
int aggressive_decide_mortgage_for_construction(Square *board, Player player, int target_cost)
{
    // Aggressive Investor never voluntarily sells/mortgages properties unless bankruptcy is unavoidable (§3.1)
    return 0;
}

// --------------- INSURANCE ----------------------
InsuranceType aggressive_decide_insurance(Square property, Economy economy, Player player)
{
    if (property.has_hotel == 1)
    {
        return COMPREHENSIVE;
    }
    else if (property.house_count > 0)
    {
        return BASIC_PROPERTY;
    }
    return NO_INSURANCE;
}

// --------------- AUCTION ------------------------
int aggressive_decide_bid(Square square, Player player, int bidding_price)
{
    return player.cash >= bidding_price && bidding_price <= apply_percentage(square.current_market_value, 120);
}
