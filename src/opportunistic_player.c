#include "include/opportunistic_player.h"
#include "include/board.h"
#include "include/finance.h"
#include "include/types.h"
#include "include/utils.h"

#include <stdio.h>

// --------------- BAIL ------------------------
int opportunistic_decide_pay_bail(Square *board, Player *player)
{
    if (player->cash < BAIL_AMOUNT)
    {
        return 0;
    }

    int unowned_count = 0;
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        if (board[i].ownership == UNOWNED &&
            (board[i].square_type == PROPERTY || board[i].square_type == RAILWAY || board[i].square_type == UTILITY))
        {
            unowned_count++;
        }
    }

    if (unowned_count >= 15)
    {
        return 1;
    }
    return 0;
}

// --------------- PROPERTY RENOVATION --------------
int opportunistic_decide_property_renovation(Square property, Player player)
{
    int property_depreciation = (property.property_age - 50) / 5;
    if (property_depreciation >= 15)
    {
        int renovation_cost = apply_percentage(property.current_market_value, 10);
        return player.cash >= renovation_cost;
    }

    return 0;
}

// --------------- RENOVATION ------------------------
int opportunistic_decide_renovation(Square square, Player player, int building_condition)
{
    // Renovate once depreciation exceeds 15% (condition < 85%) (§3.4)
    if (building_condition >= 85)
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
        renovation_cost = apply_percentage(renovation_cost, 150); // Structural damage penalty
    }

    return player.cash > renovation_cost;
}

// --------------- PURCHASE - HELPER ---------------

int property_in_decline(Square *square, Economy economy)
{
    if (square->square_type == PROPERTY)
    {
        if (economy.decline_rounds_remaining > 0 && square->property_group == economy.decline_group)
        {
            return 1;
        }
    }

    if (economy.active_regional_card == BEACH_POLLUTION)
    {
        if (square->property_group == YELLOW || square->property_index == 25)
        {
            return 1;
        }
    }
    else if (economy.active_regional_card == FLOOD_DAMAGE)
    {
        if (square->property_group == LIGHT_BLUE || square->property_group == ORANGE)
        {
            return 1;
        }
    }
    else if (economy.active_regional_card == TRANSPORT_STRIKE)
    {
        if (square->square_type == RAILWAY)
        {
            return 1;
        }
    }
    else if (economy.active_regional_card == WATER_SHORTAGE)
    {
        if (square->property_index == 27 || square->property_index == 29)
        {
            return 1;
        }
    }

    return 0;
}

int has_positive_market_advantage(Square *square, Economy economy)
{
    if (economy.boom_rounds_remaining > 0 && square->property_group == economy.boom_group)
    {
        return 1;
    }

    if (square->square_type == RAILWAY && economy.active_government_regulation == RAILWAY_MODERNIZATION)
    {
        return 1;
    }
    if (square->square_type == UTILITY && economy.active_government_regulation == ELECTRICAL_TARIFF_REVISION)
    {
        return 1;
    }

    if (square->square_type == PROPERTY) 
    {
        if (economy.active_government_regulation == HOUSING_SUBSIDY || economy.active_economic_event == GOVERNMENT_HOUSING_PROGRAMME)
        {
            return 1;
        }
    }

    switch (economy.active_regional_card)
    {
    case SOUTHERN_TOURISM_BOOM:
        if (square->property_group == YELLOW)
            return 1;
        break;
    case PORT_CITY_EXPANSION:
        if (square->property_group == BROWN || square->property_index == 5)
            return 1;
        break;
    case IT_INDUSTRY_GROWTH:
        if (square->property_group == PINK)
            return 1;
        break;
    case NORTHERN_DEVELOPMENT_PROGRAMME:
        if (square->property_group == GREEN)
            return 1;
        break;
    case TEA_EXPORT_BOOM:
        if (square->property_index == 37)
            return 1; // Nuwara Eliya
        break;
    case AIRPORT_EXPANSION:
        if (square->property_group == ORANGE)
            return 1;
        break;
    case UNIVERSITY_CITY_GROWTH:
        if (square->property_index == 21 || square->property_index == 23)
            return 1;
        break;
    case ELECTRICITY_TARIFF_INCREASE:
        if (square->property_index == 12)
            return 1;
        break;
    case WATER_SHORTAGE:
        if (square->property_index == 28)
            return 1;
        break;
    default:
        break;
    }

    return 0;
}

int can_have_monopoly(Square *board, Square *square, PlayerId playerId)
{
    if (square->square_type != PROPERTY)
    {
        return 0;
    }

    for (int i = 0; i < MAX_SQUARES; i++)
    {
        if (board[i].square_type == PROPERTY && board[i].property_group == square->property_group)
        {
            if (board[i].ownership == playerId)
            {
                return 1;
            }
        }
    }

    return 0;
}

// --------------- PURCHASE ------------------------
int opportunistic_decide_purchase(Square *board, Square *square, Player *player, Economy economy)
{
    int purchase = 0;

    if (square->current_market_value > square->purchase_price)
    {
        purchase = 1;
    }

    if (can_have_monopoly(board, square, player->playerId) == 1)
    {
        purchase = 1;
    }

    if (has_positive_market_advantage(square, economy) == 1)
    {
        purchase = 1;
    }

    if (square->square_type == RAILWAY && count_owned_by_type(board, player->playerId, RAILWAY) == 0)
    {
        purchase = 1;
    }
    if (square->square_type == UTILITY && count_owned_by_type(board, player->playerId, UTILITY) == 0)
    {
        purchase = 1;
    }

    if (property_in_decline(square, economy) == 1)
    {
        purchase = 0;
    }

    if (purchase == 1) 
    {
        return player->cash >= square->purchase_price;
    }

    return 0;
}

// --------------- LOAN - REPAYMENT ---------------
int opportunistic_decide_loan_repayment(Player player, int *amount)
{
    if (player.cash >= player.loan_amount + 1000)
    {
        *amount = player.loan_amount;
        return 1;
    }

    int partial_amount = apply_percentage(player.loan_amount, 50);
    if (partial_amount > 0 && player.cash >= partial_amount + 1000)
    {
        *amount = partial_amount;
        return 1;
    }

    partial_amount = apply_percentage(player.loan_amount, 25);
    if (partial_amount > 0 && player.cash >= partial_amount + 1000)
    {
        *amount = partial_amount;
        return 1;
    }

    return 0;
}

// --------------- LOAN - COLLATERAL & DESIRED AMOUNT ---------------
int opportunistic_desired_loan_amount(Square *board, Player player)
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

int opportunistic_decide_loan_collateral(Square *board, Player player, int *eligible_properties, int eligible_count, int *selected_collateral, int desired_amount)
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
int opportunistic_decide_loan_extention(Player player)
{
    if (player.loan_rounds_remaining < 5) {
        return 1;
    }
    return 0;
}

// --------------- LOAN - REFINANCE ---------------
int opportunistic_decide_loan_refinance(Square *board, Player player)
{
    int has_unpledged = 0;
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        if (board[i].ownership == player.playerId && board[i].is_loan_locked == 0 && board[i].is_mortgage == 0)
        {
            has_unpledged = 1;
            break;
        }
    }

    if (has_unpledged == 0)
    {
        return 0;
    }

    int needed_funds = opportunistic_desired_loan_amount(board, player);
    return needed_funds > 0;
}

// ---------------- LOANS ----------------------
int opportunistic_decide_loan(Square *board, Player player, Economy economy)
{
    if (player.cash < 1000)
    {
        return 1;
    }

    if (economy.active_economic_event == ECONOMIC_RECESSION || economy.loan_interest_rate > 10)
    {
        return 0;
    }

    int desired_funds = opportunistic_desired_loan_amount(board, player);
    if (desired_funds > 0)
    {
        return 1;
    }

    return 0;
}


// --------------- MORTGAGE FOR CONSTRUCTION ------------------------
int opportunistic_decide_mortgage_for_construction(Square *board, Player player, int target_cost)
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

        if (square.has_hotel == 1 || square.house_count > 0)
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

// --------------- CONSTRUCTION ------------------------
int opportunistic_decide_construction(Square property, Player player, Economy economy)
{
    int cost = 0;

    if (property.house_count == 4) {
            cost = property.hotel_constructionCost;
    } else {
            cost = property.house_constructionCost;
    }

    if (player.cash < cost)
    {
        return 0;
    }

    if (economy.active_government_regulation == HOUSING_SUBSIDY || economy.active_economic_event == GOVERNMENT_HOUSING_PROGRAMME)
    {
        return player.cash >= cost + 500;
    }

    if (economy.inflation >= 5)
    {
        return 0;
    }
    return player.cash >= cost + 1000;
}

// --------------- INSURANCE ----------------------
InsuranceType opportunistic_decide_insurance(Square property, Economy economy, Player player)
{
    if (property.has_hotel == 1)
    {
        return COMPREHENSIVE;
    }

    if (property.house_count >= 3) // YELLOW, GREEN, DARK BLUE
    {
        if (property.property_group >= YELLOW && property.property_group <= DARK_BLUE)
        {
            return COMPREHENSIVE;
        }
    }

    if (economy.active_economic_event == HEAVY_MONSOON || economy.active_government_regulation == INSURANCE_REGULATION)
    {
        if (property.house_count >= 2)
        {
            return BASIC_PROPERTY;
        }
    }

    return NO_INSURANCE;
}

// --------------- AUCTION ------------------------
int opportunistic_decide_bid(Square square, Player player, int bidding_price, Economy economy)
{
    if (player.cash < bidding_price)
    {
        return 0;
    }

    int max_percentage = 90; 

    if (has_positive_market_advantage(&square, economy) == 1)
    {
        max_percentage = 100; 
    }
    
    else if (property_in_decline(&square, economy) == 1)
    {
        max_percentage = 70; 
    }

    int max_bid_limit = apply_percentage(square.current_market_value, max_percentage);

    return bidding_price <= max_bid_limit;
}
