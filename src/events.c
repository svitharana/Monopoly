#include "include/events.h"
#include "include/finance.h"
#include "include/types.h"
#include "include/utils.h"

#include <stdio.h>

void update_inflation(Economy *economy)
{
    int rates[] = {-3, 0, 2, 5, 8, 12};

    economy->inflation = rates[random_generator(0, 5)];
}

void update_board_data(Economy *economy, Square *board)
{
    Square *square;

    // TODO: need to add: insurance premiums, repair costs
    economy->loan_interest_rate = apply_percentage(economy->loan_interest_rate, 100 + economy->inflation);

    for (int i = 0; i < MAX_SQUARES; i++)
    {
        square = &board[i];

        if (square->square_type != PROPERTY)
        {
            continue;
        }

        square->current_market_value = apply_percentage(square->current_market_value, 100 + economy->inflation);
        square->house_constructionCost = apply_percentage(square->house_constructionCost, 100 + economy->inflation);
        square->hotel_constructionCost = apply_percentage(square->hotel_constructionCost, 100 + economy->inflation);
        square->base_rent = apply_percentage(square->base_rent, 100 + economy->inflation);
    }
}

void update_dynamic_property_market(Square *board, Economy *economy)
{
    PropertyGroup boom_grp = -1;
    PropertyGroup decline_grp = -1;

    do
    {
        boom_grp = random_generator(0, MAX_PROPERTY_GRPS - 1);

    } while (economy->boom_decline_grp_cooldown[boom_grp] != 0);

    economy->boom_decline_grp_cooldown[boom_grp] = 30;
    economy->boom_group = boom_grp;
    economy->boom_rounds_remaining = 10;

    do
    {
        decline_grp = random_generator(0, MAX_PROPERTY_GRPS - 1);

    } while (economy->boom_decline_grp_cooldown[decline_grp] != 0);

    economy->boom_decline_grp_cooldown[decline_grp] = 30;
    economy->decline_group = decline_grp;
    economy->decline_rounds_remaining = 10;

    for (int i = 0; i < MAX_SQUARES; i++)
    {
        Square *property = &board[i];
        if (property->square_type != PROPERTY)
        {
            continue;
        }

        if (property->property_group == economy->boom_group)
        {
            property->purchase_price = apply_percentage(property->purchase_price, 100 + 15);
            property->mortgage_value = apply_percentage(property->mortgage_value, 100 + 15);
            property->base_rent = apply_percentage(property->base_rent, 100 + 25);
            property->house_constructionCost = apply_percentage(property->house_constructionCost, 100 + 10);
            property->hotel_constructionCost = apply_percentage(property->hotel_constructionCost, 100 + 10);
            property->current_market_value = apply_percentage(property->current_market_value, 100 + 20);
        }
        else if (property->property_group == economy->decline_group)
        {
            // TODO: add auction starting price decrease by 25%
            property->mortgage_value = apply_percentage(property->mortgage_value, 100 - 10);
            property->base_rent = apply_percentage(property->base_rent, 100 - 20);
            property->current_market_value = apply_percentage(property->current_market_value, 100 - 15);
        }
    }
}

int create_disaster(Square *board) {

    Disasters disaster = random_generator(0, MAX_DISASTERS - 1);

    Square *property;

    int developed_properties_exists = 0;
    for (int i = 0; i < MAX_SQUARES; i++) {
        if (board[i].house_count > 0 || board[i].has_hotel == 1)
        {
            developed_properties_exists = 1;
        }
    }

    if (developed_properties_exists == 1) {
        do {

            int random_property_index = random_generator(0, MAX_SQUARES - 1);
            property = &board[random_property_index];

        } while (property->square_type != PROPERTY || (property->has_hotel == 0 && property->house_count == 0));

    property->is_damaged = 1;
    property->damaged_by = disaster;

    return property->property_index;
    } else {
        return -1;
    }
}

void remove_regional_development_card_effect(Square *board, RegionalDevelopmentCards card)
{
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        Square *square = &board[i];

        switch (card)
        {
        case SOUTHERN_TOURISM_BOOM:
            if (square->property_group == YELLOW)
            {
                square->base_rent = remove_percentage(square->base_rent, 100 + 40);
            }
            break;
        case PORT_CITY_EXPANSION:
            if (square->property_group == BROWN || square->property_index == 5) // 5 - Colombo Fort Station
            {
                square->current_market_value = remove_percentage(square->current_market_value, 100 + 25);
            }
            break;
        case IT_INDUSTRY_GROWTH:
            if (square->property_group == PINK)
            {
                square->current_market_value = remove_percentage(square->current_market_value, 100 + 20);
            }
            break;
        case NORTHERN_DEVELOPMENT_PROGRAMME:
            if (square->property_group == GREEN)
            {
                square->current_market_value = remove_percentage(square->current_market_value, 100 + 30);
            }
            break;
        case TEA_EXPORT_BOOM:
            if (square->property_index == 37) // Nuwara Eliya
            {
                square->current_market_value = remove_percentage(square->current_market_value, 100 + 35);
            }
            break;
        case AIRPORT_EXPANSION:
            if (square->property_group == ORANGE)
            {
                square->base_rent = remove_percentage(square->base_rent, 100 + 30);
            }
            break;
        case UNIVERSITY_CITY_GROWTH:
            if (square->property_index == 21 || square->property_index == 23)
            {
                square->current_market_value = remove_percentage(square->current_market_value, 100 + 20);
            }
            break;
        case BEACH_POLLUTION:
            if (square->property_group == YELLOW || square->property_index == 25)
            {
                square->base_rent = remove_percentage(square->base_rent, 100 - 30);
            }
            break;
        case FLOOD_DAMAGE:
            if (square->property_group == LIGHT_BLUE || square->property_group == ORANGE)
            {
                square->current_market_value = remove_percentage(square->current_market_value, 100 - 20);
            }
            break;
        case TRANSPORT_STRIKE:
            if (square->square_type == RAILWAY)
            {
                square->base_rent = remove_percentage(square->base_rent, 100 - 40);
            }
            break;
        case ELECTRICITY_TARIFF_INCREASE:
            if (square->property_index == 12)
            {
                square->base_rent = remove_percentage(square->base_rent, 100 + 25);
            }
            break;
        case WATER_SHORTAGE:
            if (square->property_index == 28)
            {
                square->base_rent = remove_percentage(square->base_rent, 100 + 20);
            }
            if (square->property_index == 27 || square->property_index == 29) // Unawatuna, Hikkaduwa
            {
                square->current_market_value = remove_percentage(square->current_market_value, 100 - 10);
            }
            break;
        default:
            break;
        }
    }
}


void draw_regional_development_card(Square *board, Economy *economy) {
    RegionalDevelopmentCards card = random_generator(0, MAX_REGIONAL_DEVELOPMENT_CARDS - 1);

    economy->active_regional_card = card;

    for (int i = 0; i < MAX_SQUARES; i++) {
        Square *square = &board[i];

        switch (card) {
        case SOUTHERN_TOURISM_BOOM: 
            if (square->property_group == YELLOW) {
                square->base_rent = apply_percentage(square->base_rent, 100 + 40);
            }
            break;
        case PORT_CITY_EXPANSION: 
            if (square->property_group == BROWN || square->property_index == 5) { // 5 - Colombo Fort Station
                square->current_market_value = apply_percentage(square->current_market_value, 100 + 25);
            }
            break;
        case IT_INDUSTRY_GROWTH: 
            if (square->property_group == PINK) {
                square->current_market_value = apply_percentage(square->current_market_value, 100 + 20);
            }
            break;
        case NORTHERN_DEVELOPMENT_PROGRAMME:
            if (square->property_group == GREEN) {
                square->current_market_value = apply_percentage(square->current_market_value, 100 + 30);
            }
            break;
        case TEA_EXPORT_BOOM: 
            if (square->property_index == 37) { // Nuwara-Eliya
                square->current_market_value = apply_percentage(square->current_market_value, 100 + 35);
            }
            break;
        case AIRPORT_EXPANSION: 
            if(square->property_group == ORANGE) {
                square->base_rent = apply_percentage(square->base_rent, 100 + 30);
            }
            break;
        case UNIVERSITY_CITY_GROWTH: 
            if (square->property_index == 21 || square->property_index == 23) {
                square->current_market_value = apply_percentage(square->current_market_value, 100 + 20);
            }
            break;
        case BEACH_POLLUTION: 
            if (square->property_group == YELLOW || square->property_index == 25) {
                square->base_rent = apply_percentage(square->base_rent, 100 - 30);
            }
            break;
        case FLOOD_DAMAGE:
            if (square->property_group == LIGHT_BLUE || square->property_group == ORANGE) {
                square->current_market_value = apply_percentage(square->current_market_value, 100 - 20);
            }
            break;
        case TRANSPORT_STRIKE: 
            if (square->square_type == RAILWAY) {
                square->base_rent = apply_percentage(square->base_rent, 100 - 40);
            }
            break;
        case ELECTRICITY_TARIFF_INCREASE: 
            if (square->property_index == 12) { 
                square->base_rent = apply_percentage(square->base_rent, 100 + 25);
            }
            break;
        case WATER_SHORTAGE: 
            if (square->property_index == 28) {
                square->base_rent = apply_percentage(square->base_rent, 100 + 20);
            }
            if (square->property_index == 27 || square->property_index == 29) { // Unawatuna, Hikkaduwa
                square->current_market_value = apply_percentage(square->current_market_value, 100 - 10);
            }
            break;
        default: 
            break;
    }
    }
}

void run_economic_event(Square *board, Economy *economy) {
    EconmicEvents event = random_generator(0, MAX_ECONOMIC_EVENTS - 1);

    economy->active_economic_event = event;

    switch (event) {
    case ECONOMIC_RECESSION:
        economy->loan_interest_rate = apply_percentage(economy->loan_interest_rate, 100 + 15);
        break;
    case STOCK_MARKET_BOOM:
        economy->loan_interest_rate = apply_percentage(economy->loan_interest_rate, 100 - 10);
        break;
    default:
        break;
    }

    for (int i = 0; i < MAX_SQUARES; i++) {
        Square *square = &board[i];

        switch (event) {
        case TOURISM_BOOM:
            if (square->has_hotel == 1) {
                square->base_rent *= 2;
            }
            if (square->property_group == YELLOW || square->property_index == 25) {
                square->base_rent = apply_percentage(square->base_rent, 100 + 15);
            }
            break;
        case FUEL_CRISIS:
            if (square->square_type == RAILWAY) {
                square->base_rent *= 2;
            }

            if (square->square_type == PROPERTY) {
                square->hotel_constructionCost = apply_percentage(square->hotel_constructionCost, 100 + 20);
                square->house_constructionCost = apply_percentage(square->house_constructionCost, 100 + 20);
            }
            break;
        case HEAVY_MONSOON:
            if (square->property_group == LIGHT_BLUE || square->property_group == ORANGE) {
                square->current_market_value = apply_percentage(square->current_market_value, 100 - 10);
            }
            break;
        case ECONOMIC_RECESSION:
            if (square->square_type == PROPERTY || square->square_type == RAILWAY || square->square_type == UTILITY) {
                square->current_market_value = apply_percentage(square->current_market_value, 100 - 15); 
                square->base_rent = apply_percentage(square->base_rent, 100 - 10);
            }

            break;
        case STOCK_MARKET_BOOM:
            if (square->square_type == PROPERTY || square->square_type == RAILWAY || square->square_type == UTILITY) {
                square->current_market_value = apply_percentage(square->current_market_value, 100 + 10); 
            }
            break;
        case GOVERNMENT_HOUSING_PROGRAMME:
            if (square->square_type == PROPERTY) {
                square->house_constructionCost = apply_percentage(square->house_constructionCost, 100 - 25);
            }
            break;
        case FOREIGN_INVESTMENT:
            if (square->property_group == GREEN || square->property_group == DARK_BLUE) {
                square->current_market_value = apply_percentage(square->current_market_value, 100 + 20);
            }
            break;
        case POLITICAL_UNREST:
            if (square->has_hotel == 1) {
                square->base_rent = apply_percentage(square->base_rent, 100 - 50);
            }
            break;
        default:
            break;
    }
    }
}

void run_government_regulations(Square *board, Economy *economy, Player *players) {
    GovernmentRegulations regulation = random_generator(0, MAX_GOVERNMENT_REGULATIONS - 1);

    economy->active_government_regulation = regulation;

    switch(regulation){
        case INCREASE_PROPERTY_TAX:
            economy->income_tax_rate = apply_percentage(economy->income_tax_rate, 100 + 50);
            break;
        case REDUCE_LOAN_INTEREST:
            economy->loan_interest_rate -= 2; 
            break;
        default:
            break;
    }

    for (int i = 0; i < MAX_SQUARES; i++){
        Square *square = &board[i];
        switch (regulation) {
            case HOUSING_SUBSIDY:
            if (square->square_type == PROPERTY) {
                square->house_constructionCost = apply_percentage(square->house_constructionCost, 100 - 30);
            }
                break;
            case RAILWAY_MODERNIZATION:
                if (square->square_type == RAILWAY) {
                    square->base_rent = apply_percentage(square->base_rent, 100 + 25);
                }
                break;
            case ELECTRICAL_TARIFF_REVISION:
                if (square->square_type == UTILITY) {
                    square->base_rent = apply_percentage(square->base_rent, 100 + 20);
                }
                break;
            case LUXURY_PROPERTY_TAX:
                if (board[i].has_hotel == 1 && board[i].ownership != UNOWNED) {
                    int maintenance_tax = apply_percentage(square->current_market_value, 25);
                    
                    Player *owner = &players[board[i].ownership];

                    if (check_player_bankrupt(board, owner, players, maintenance_tax) == 0) {
                        execute_tax_collection(owner, maintenance_tax);
                        printf("\t%s paid LKR %d Luxury Property Tax for hotel on %s.\n", owner->player_name, maintenance_tax, square->square_name);
                    }
                }
                break;
            default:
                break;
        }
    }
}