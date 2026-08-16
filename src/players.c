#include "include/players.h"
#include "include/board.h"
#include "include/finance.h"
#include "include/types.h"
#include "include/utils.h"

#include <stdio.h>

// --------------- BAIL ------------------------
int decide_pay_bail(Player *player)
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
        return 0; // TODO: add opportunistic bail strategy
    default:
        return 0;
    }
}

// --------------- PROPERTY RENOVATION --------------

int aggressive_decide_property_renovation(Square property, Player player) {
    int property_depreciation = (property.property_age - 50) / 5;
    int cost = apply_percentage(property.current_market_value, 10);
    if (property_depreciation >= 5) {
        return player.cash >= cost * 2;
    }
    return 0;
}

int conservative_decide_property_renovation(Square property, Player player) {
    int property_depreciation = (property.property_age - 50) / 5;
    if (property_depreciation >= 10) {
        return player.cash >= apply_percentage(property.current_market_value, 10);
    }

    return 0;
}

int risk_decide_property_renovation(Square property, Player player) {
    int property_depreciation = (property.property_age - 50) / 5;
    if (property_depreciation >= 30) {
        return player.cash >= apply_percentage(property.current_market_value, 10);;
    }
    return 0;
}

int opportunistic_decide_property_renovation(Square property, Player player) {
    return 0;
}

int decide_property_renovation(Square square, Player player) {
    switch (player.strategy) {
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
        renovation_cost = apply_percentage(renovation_cost, 150); // Increase cost by 50% if condition is below 60
    }

    return player.cash > renovation_cost;
}

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
        return 0; // TODO: add opportunistic renovation strategy
    default:
        return 0;
    }
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

int risk_decide_purchase(Square *square, Player *player)
{
    return player->cash >= square->purchase_price;
}

int opportunistic_decide_purchase(Square *square, Player *player)
{
    // TODO: add opportunistic purchase strategy
    return player->cash >= square->purchase_price;
}

int decide_purchase(Square *board, Square *square, Player *player, Economy economy)
{
    switch (player->strategy)
    {
    case AGGRESSIVE_INVESTOR:
        return aggressive_decide_purchase(board, square, player);
    case CONSERVATIVE_BANKER:
        return conservative_decide_purchase(square, player, economy);
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
    for (int group = 0; group < MAX_PROPERTY_GRPS; group++)
    {
        if (player_has_monopoly(board, player.playerId, group) == 1)  
        {
            for (int i = 0; i < MAX_SQUARES; i++)
            {
                Square property = board[i];

                if (property.ownership != player.playerId || property.square_type != PROPERTY || property.property_group != group)
                {
                    continue;
                }

                if (property.has_hotel == 1) {
                    continue;
                } 

                if (property.house_count == 4 && property.hotel_constructionCost > player.cash) {
                    return 1;
                }

                if (property.house_count < 4 && property.house_constructionCost > player.cash) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int conservative_decide_loan(Square *board, Player player, Economy economy)
{
    // Avoid loans unless bankruptcy is imminent.
    int max_expense = get_max_opponent_rent(board, player);

    // If current cash is lower than the max payable expense, bankruptcy is imminent -> take loan
    if (player.cash < max_expense)
    {
        return 1;
    }

    return 0;
}

int opportunistic_decide_loan(Square *board, Player player)
{
    // TODO: Add opportunistic loan decision
    return 1;
}

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
        return 1;
    }

    // Partial repaymen
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

int risk_decide_loan_repayment(Player player, int *amount)
{
    return 0; // never repay loan
}

int opportunistic_decide_loan_repayment(Player player, int *amount)
{
    // TODO: Add opportunistic loan repayment
    return player.cash >= player.loan_amount; // repay loan if possible
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

// --------------- LOAN - COLLATERAL & DESIRED AMOUNT ---------------

int aggressive_desired_loan_amount(Square *board, Player player)
{
    int development_cost = 0;
    for (int group = 0; group < MAX_PROPERTY_GRPS; group++)
    {
        if (player_has_monopoly(board, player.playerId, group) == 1)
        {
            for (int i = 0; i < MAX_SQUARES; i++)
            {
                Square property = board[i];
                if (property.ownership != player.playerId || property.square_type != PROPERTY || property.property_group != group)
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
    
    if (needed_funds > 0) {
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

    // Prioritize non-monopoly properties, railways, utilities first
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
                if (player_has_monopoly(board, player.playerId, property.property_group) == 1) {
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

    // Step 1: Prioritize isolated / non-monopoly regular properties first (starting with lowest mortgage value)
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
                continue; // Preserve predictable income sources
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

int opportunistic_desired_loan_amount(Square *board, Player player)
{
    // TODO: Add opportunistic desired loan amount
    return 0;
}

int opportunistic_decide_loan_collateral(Square *board, Player player, int *eligible_properties, int eligible_count, int *selected_collateral, int desired_amount)
{
    // TODO: Add opportunistic decide loan collateral
    return 0;
}

int decide_loan_collateral(Square *board, Player player, int *eligible_properties, int eligible_count, int *selected_collateral, Economy economy)
{
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

int aggressive_decide_loan_extention(Player player)
{
    return player.cash <= player.loan_amount * 2;
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
    int has_unpledged_collateral = 0;
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        Square property = board[i];
        
        if (property.ownership != player.playerId|| property.is_loan_locked == 1 || property.is_mortgage == 1)
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
        if (player_has_monopoly(board, player.playerId, group))
        {
            for (int i = 0; i < MAX_SQUARES; i++)
            {
                Square property = board[i];

                if (property.ownership != player.playerId || property.square_type != PROPERTY || property.property_group != group)
                {
                    continue;
                }

                if (property.has_hotel == 1) {
                    continue;
                } 

                if (property.house_count == 4 && property.hotel_constructionCost > player.cash) {
                    return 1;
                }

                if (property.house_count < 4 && property.house_constructionCost > player.cash) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int conservative_decide_loan_refinance(Square *board, Player player)
{
    return 0; // Conservative Banker never refinances loans
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

int conservative_decide_construction(Square property, Player player, Economy economy)
{
    // Avoid investments during economic recessions
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
        // Never develops hotels until all outstanding loans have been settled
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
    if (property.house_count < 4)
    {
        return player.cash >= property.house_constructionCost;
    }
    else if (property.has_hotel == 0)
    {
        return player.cash >= property.hotel_constructionCost;
    }
    return 0; // TODO: check opportunistic construction strategy
}

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
        return opportunistic_decide_construction(property, player);
    default:
        return 0;
    }
}

// --------------- MORTGAGE FOR CONSTRUCTION ------------------------

int aggressive_decide_mortgage_for_construction(Square *board, Player player, int target_cost)
{
    // Aggressive Investor never voluntarily sells/mortgages properties unless bankruptcy is unavoidable (§3.1)
    return 0;
}

int conservative_decide_mortgage_for_construction(Square *board, Player player, int target_cost)
{
    // Conservative Banker avoids unnecessary risks and debt (§3.2)
    return 0;
}

int risk_decide_mortgage_for_construction(Square *board, Player player, int target_cost)
{
    if (player.cash >= target_cost) {
        return 0;
    }
    
    int accumulated_amount = player.cash;
    for (int i = 0; i < MAX_SQUARES; i++) {
        Square square = board[i];

        if (square.ownership != player.playerId) {
            continue;
        }

        if (square.is_mortgage == 1 || square.is_loan_locked == 1) {
            continue;
        }

        if (square.has_hotel == 1) {
            continue;
        } else if (square.house_count > 0) {
            continue;
        }

        if (square.square_type == PROPERTY) {
            if (player_has_monopoly(board, player.playerId, square.property_group) == 1) {
                continue;
            }
        }

        accumulated_amount += square.mortgage_value;

        if (accumulated_amount >= target_cost) {
            return 1;
        }
    }

    return 0;
}

int opportunistic_decide_mortgage_for_construction(Square *board, Player player, int target_cost)
{
    // TODO: Add opportunistic mortgage strategy if required
    return 0;
}

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

InsuranceType aggressive_decide_insurance(Square property, Economy economy, Player player) {
    if (property.has_hotel == 1) {
        return COMPREHENSIVE;
    } else if (property.house_count > 0) {
        return BASIC_PROPERTY;
    }
    return NO_INSURANCE;
}

InsuranceType conservative_decide_insurance(Square property, Economy economy, Player player) {
    if (property.has_hotel == 1 || property.house_count > 0) {
        return COMPREHENSIVE;
    }
    return NO_INSURANCE;
}

InsuranceType risk_decide_insurance(Square property, Economy economy, Player player) {
    if (player.incured_loss == 1) {
        if (property.has_hotel == 1 || property.house_count > 0)
        {
            return BASIC_PROPERTY;
        }           
    }
    return NO_INSURANCE;
}

InsuranceType opportunistic_decide_insurance(Square property, Economy economy, Player player)
{
    // TODO: Need to implement opportunistic insurance strategy
    if (property.has_hotel == 1 || property.house_count > 0) {
        return COMPREHENSIVE;
    }
    return NO_INSURANCE;
}

InsuranceType decide_insurance(Square property, Economy economy, Player player) {

    InsuranceType insurance_type;

    switch (player.strategy) {
        case AGGRESSIVE_INVESTOR:
            insurance_type =  aggressive_decide_insurance(property, economy, player); 
            break;
        case CONSERVATIVE_BANKER:
            insurance_type =  conservative_decide_insurance(property, economy, player);
            break;
        case RISK_TAKER:
            insurance_type =  risk_decide_insurance(property, economy, player);
            break;
        case OPPORTUNISTIC_TRADER:
            insurance_type =  opportunistic_decide_insurance(property, economy, player);
            break;
        default:
            insurance_type = NO_INSURANCE;
            break;
    }
    if (insurance_type == NO_INSURANCE) {
        return insurance_type;
    }

    int premium = calculate_insurance_premium(property, insurance_type, economy, player);

    if (player.cash >= premium) {
        return insurance_type;
    }

    return NO_INSURANCE;
} 

// --------------- AUCTION ------------------------

int aggressive_decide_bid(Square square, Player player, int bidding_price)
{
    return player.cash >= bidding_price && bidding_price <= apply_percentage(square.current_market_value, 120);
}

int conservative_decide_bid(Square square, Player player, int bidding_price, Economy economy)
{
    // Avoid investments during economic recessions
    if (economy.active_economic_event == ECONOMIC_RECESSION)
    {
        return 0;
    }

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