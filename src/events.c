#include "include/events.h"
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