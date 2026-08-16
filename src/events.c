#include "include/events.h"
#include "include/finance.h"
#include "include/types.h"
#include "include/utils.h"

#include <stdio.h>

void initialize_national_event_cards(NationalEventCards *cards)
{
    for (int i = 0; i < MAX_NATIONAL_EVENT_CARDS; i++)
    {
        cards[i].index = i;
    }
    cards[0].name = "Tourism Hype";
    cards[0].effect = "Hotels earn double rent for 5 rounds";

    cards[1].name = "Fuel Shortage";
    cards[1].effect = "Railway rent doubles for 5 rounds";

    cards[2].name = "Heavy Floods";
    cards[2].effect = "Random coastal property damaged";

    cards[3].name = "Political Rally";
    cards[3].effect = "One random property closed for 2 rounds";

    cards[4].name = "Stock Market Rise";
    cards[4].effect = "All property values increase by 10%";

    cards[5].name = "Economic Downturn";
    cards[5].effect = "Property values decrease by 15%";

    cards[6].name = "Housing Subsidy";
    cards[6].effect = "House construction cost reduced by 30%";

    cards[7].name = "Interest Rate Cut";
    cards[7].effect = "Loan interest reduced by 2%";

    cards[8].name = "Interest Rate Increase";
    cards[8].effect = "Loan interest increased by 2%";

    cards[9].name = "Tax Amnesty";
    cards[9].effect = "Each player receives LKR 2,000";

    cards[10].name = "Power Failure";
    cards[10].effect = "Utility income halved for 3 rounds";

    cards[11].name = "Foreign Funding";
    cards[11].effect = "Commercial property values increase by 15%";

    cards[12].name = "Port Expansion";
    cards[12].effect = "Railway station values increase by 20%";

    cards[13].name = "Festival Season";
    cards[13].effect = "Hotels receive 50% additional rent";

    cards[14].name = "Labour Strike";
    cards[14].effect = "Construction suspended for 2 rounds";

    cards[15].name = "Insurance Discount";
    cards[15].effect = "Premiums reduced by 20%";

    cards[16].name = "Property Revaluation";
    cards[16].effect = "Random property group appreciates by 15%";

    cards[17].name = "Currency Depreciation";
    cards[17].effect = "Construction costs increase by 10%";

    cards[18].name = "Government Grant";
    cards[18].effect = "Random player receives LKR 5,000";

    cards[19].name = "National Disaster";
    cards[19].effect = "Random developed property damaged";
}

int create_disaster(Square *board)
{

    Disasters disaster = random_generator(0, MAX_DISASTERS - 1);

    Square *property;

    int developed_properties_exists = 0;
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        if (board[i].house_count > 0 || board[i].has_hotel == 1)
        {
            developed_properties_exists = 1;
        }
    }

    if (developed_properties_exists == 1)
    {
        do
        {

            int random_property_index = random_generator(0, MAX_SQUARES - 1);
            property = &board[random_property_index];

        } while (property->square_type != PROPERTY || (property->has_hotel == 0 && property->house_count == 0));

        property->is_damaged = 1;
        property->damaged_by = disaster;

        return property->property_index;
    }
    else
    {
        return -1;
    }
}

NationalEventCards draw_national_event_card(NationalEventCards *cards, Player *player)
{
    NationalEventCards drawn_card = cards[0];

    for (int i = 0; i < MAX_NATIONAL_EVENT_CARDS - 1; i++)
    {
        cards[i] = cards[i + 1];
    }
    cards[MAX_NATIONAL_EVENT_CARDS - 1] = drawn_card;

    printf("\n\tNational Event Card Drawn\n");
    printf("\t------------------------------\n");
    printf("\tCard   : %s\n", drawn_card.name);
    printf("\tEffect : %s\n", drawn_card.effect);

    player->active_national_event_card = drawn_card;

    return drawn_card;
}

void remove_national_event_effect(Square *board, Economy *economy, Player *player)
{
    NationalEventCards card = player->active_national_event_card;

    switch (card.index)
    {
    case 3:
        for (int i = 0; i < MAX_SQUARES; i++)
        {
            if (board[i].ownership == player->playerId && board[i].is_closed == 1)
            {
                board[i].is_closed = 0;
                printf("\t%s reopened (Political Rally ended).\n", board[i].square_name);
            }
        }
        break;
    case 7:
        economy->loan_interest_rate += 2;
        break;
    case 8:
        economy->loan_interest_rate -= 2;
        if (economy->loan_interest_rate < 1)
            economy->loan_interest_rate = 1;
        break;
    default:
        break;
    }

    for (int i = 0; i < MAX_SQUARES; i++)
    {
        Square *square = &board[i];
        if (square->ownership != player->playerId)
        {
            continue;
        }
        switch (card.index)
        {
        case 0:
            if (square->has_hotel == 1)
            {
                square->base_rent /= 2;
            }
            break;
        case 1:
            if (square->square_type == RAILWAY)
            {
                square->base_rent /= 2;
            }
            break;
        case 4:
            square->current_market_value = remove_percentage(square->current_market_value, 100 + 10);
            break;
        case 5:
            square->current_market_value = remove_percentage(square->current_market_value, 100 - 15);
            break;
        case 6:
            if (square->square_type == PROPERTY)
            {
                square->house_constructionCost = remove_percentage(square->house_constructionCost, 100 - 30);
            }
            break;
        case 10:
            if (square->square_type == UTILITY)
            {
                square->base_rent *= 2;
            }
            break;
        case 11:
            if (square->property_group == DARK_BLUE || square->property_group == GREEN)
            {
                square->current_market_value = remove_percentage(square->current_market_value, 100 + 15);
            }
            break;
        case 12:
            if (square->square_type == RAILWAY)
            {
                square->current_market_value = remove_percentage(square->current_market_value, 100 + 20);
            }
            break;
        case 13:
            if (square->has_hotel == 1)
            {
                square->base_rent = remove_percentage(square->base_rent, 100 + 50);
            }
            break;
        case 16:
            square->current_market_value = remove_percentage(square->current_market_value, 100 + 15);
            break;
        case 17:
            if (square->square_type == PROPERTY)
            {
                square->house_constructionCost = remove_percentage(square->house_constructionCost, 100 + 10);
                square->hotel_constructionCost = remove_percentage(square->hotel_constructionCost, 100 + 10);
            }
            break;
        default:
            break;
        }
    }

    player->active_national_event_card.index = -1;
    player->active_national_event_card.name = "";
    player->active_national_event_card.effect = "";
}

void apply_national_event_effect(Square *board, Economy *economy, NationalEventCards card, Player *player, Player *players)
{
    player->national_event_card_rounds_remaining = 15;
    switch (card.index)
    {
    case 2:
        int square_count = MAX_SQUARES;
        while (square_count > 0)
        {
            int random_property_index = random_generator(0, MAX_SQUARES - 1);

            if (board[random_property_index].ownership == player->playerId)
            {

                if (board[random_property_index].has_hotel == 1 || board[random_property_index].house_count > 0)
                {

                    board[random_property_index].is_damaged = 1;
                    board[random_property_index].damaged_by = FLOOD;
                    printf("\t%s suffered Flood Damage!\n", board[random_property_index].square_name);
                    break;
                }
            }
            square_count--;
        }
        break;
    case 3:
        square_count = MAX_SQUARES;
        while (square_count > 0)
        {
            int random_property_index = random_generator(0, MAX_SQUARES - 1);

            if (board[random_property_index].ownership == player->playerId)
            {
                if (board[random_property_index].property_group == YELLOW || board[random_property_index].property_group == LIGHT_BLUE)
                {

                    if (board[random_property_index].has_hotel == 1 || board[random_property_index].house_count > 0)
                    {

                        board[random_property_index].is_closed = 1;
                        player->national_event_card_rounds_remaining = 2;
                        printf("\t%s is closed for 2 rounds due to Political Rally\n", board[random_property_index].square_name);
                        break;
                    }
                }
            }
            square_count--;
        }
        break;
    case 7:
        economy->loan_interest_rate -= 2;
        if (economy->loan_interest_rate < 1)
        {
            economy->loan_interest_rate = 1;
        }
        break;
    case 8:
        economy->loan_interest_rate += 2;
        break;
    case 9:
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            players[i].cash += 2000;
        }
        break;
    case 14:
        player->national_event_card_rounds_remaining = 2;
        break;
    case 15:
        break; // Insurance premiums
    case 16:
        PropertyGroup owned_groups[MAX_PROPERTY_GRPS];
        int owned_group_count = 0;

        for (int i = 0; i < MAX_SQUARES; i++)
        {
            if (board[i].ownership == player->playerId && board[i].square_type == PROPERTY)
            {

                int already_added = 0;
                for (int g = 0; g < owned_group_count; g++)
                {
                    if (owned_groups[g] == board[i].property_group)
                    {
                        already_added = 1;
                        break;
                    }
                }
                if (already_added == 0)
                {
                    owned_groups[owned_group_count++] = board[i].property_group;
                }
            }
        }

        if (owned_group_count > 0)
        {
            PropertyGroup selected_group = owned_groups[random_generator(0, owned_group_count - 1)];
            for (int i = 0; i < MAX_SQUARES; i++)
            {
                if (board[i].ownership != player->playerId || board[i].property_group != selected_group)
                {
                    continue;
                }

                board[i].current_market_value = apply_percentage(board[i].current_market_value, 100 + 15);
                printf("\t%s appreciated by 15%%!\n", board[i].square_name);
            }
        }
        break;
    case 18:

        PlayerId random_playerId;
        do
        {
            random_playerId = random_generator(0, MAX_PLAYERS - 1);
        } while (players[random_playerId].is_bankrupt == 1);

        players[random_playerId].cash += 5000;
        printf("\t%s received LKR 5,000\n", players[random_playerId].player_name);
        break;
    case 19:
        create_disaster(board);
        break;
    }
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        Square *square = &board[i];

        if (square->ownership != player->playerId)
        {
            continue;
        }

        switch (card.index)
        {
        case 0:
            if (square->has_hotel == 1)
            {
                square->base_rent *= 2;

                player->national_event_card_rounds_remaining = 5;
            }
            break;
        case 1:
            if (square->square_type == RAILWAY)
            {
                square->base_rent *= 2;

                player->national_event_card_rounds_remaining = 5;
            }
            break;
        case 4:
            square->current_market_value = apply_percentage(square->current_market_value, 100 + 10);
            break;
        case 5:
            square->current_market_value = apply_percentage(square->current_market_value, 100 - 15);
            break;
        case 6:
            if (square->square_type == PROPERTY)
            {
                square->house_constructionCost = apply_percentage(square->house_constructionCost, 100 - 30);
            }
            break;
        case 10:
            if (square->square_type == UTILITY)
            {
                square->base_rent /= 2;

                player->national_event_card_rounds_remaining = 3;
            }
            break;
        case 11:
            if (square->property_group == DARK_BLUE || square->property_group == GREEN)
            {
                square->current_market_value = apply_percentage(square->current_market_value, 100 + 15);
            }
            break;
        case 12:
            if (square->square_type == RAILWAY)
            {
                square->current_market_value = apply_percentage(square->current_market_value, 100 + 20);
            }
            break;
        case 13:
            if (square->has_hotel == 1)
            {
                square->base_rent = apply_percentage(square->base_rent, 100 + 50);
            }
            break;
        case 17:
            if (square->square_type == PROPERTY)
            {
                square->house_constructionCost = apply_percentage(square->house_constructionCost, 100 + 10);
                square->hotel_constructionCost = apply_percentage(square->hotel_constructionCost, 100 + 10);
            }
            break;
        default:
            break;
        }
    }
}

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

void draw_regional_development_card(Square *board, Economy *economy)
{
    RegionalDevelopmentCards card = random_generator(0, MAX_REGIONAL_DEVELOPMENT_CARDS - 1);

    economy->active_regional_card = card;

    for (int i = 0; i < MAX_SQUARES; i++)
    {
        Square *square = &board[i];

        switch (card)
        {
        case SOUTHERN_TOURISM_BOOM:
            if (square->property_group == YELLOW)
            {
                square->base_rent = apply_percentage(square->base_rent, 100 + 40);
            }
            break;
        case PORT_CITY_EXPANSION:
            if (square->property_group == BROWN || square->property_index == 5)
            { // 5 - Colombo Fort Station
                square->current_market_value = apply_percentage(square->current_market_value, 100 + 25);
            }
            break;
        case IT_INDUSTRY_GROWTH:
            if (square->property_group == PINK)
            {
                square->current_market_value = apply_percentage(square->current_market_value, 100 + 20);
            }
            break;
        case NORTHERN_DEVELOPMENT_PROGRAMME:
            if (square->property_group == GREEN)
            {
                square->current_market_value = apply_percentage(square->current_market_value, 100 + 30);
            }
            break;
        case TEA_EXPORT_BOOM:
            if (square->property_index == 37)
            { // Nuwara-Eliya
                square->current_market_value = apply_percentage(square->current_market_value, 100 + 35);
            }
            break;
        case AIRPORT_EXPANSION:
            if (square->property_group == ORANGE)
            {
                square->base_rent = apply_percentage(square->base_rent, 100 + 30);
            }
            break;
        case UNIVERSITY_CITY_GROWTH:
            if (square->property_index == 21 || square->property_index == 23)
            {
                square->current_market_value = apply_percentage(square->current_market_value, 100 + 20);
            }
            break;
        case BEACH_POLLUTION:
            if (square->property_group == YELLOW || square->property_index == 25)
            {
                square->base_rent = apply_percentage(square->base_rent, 100 - 30);
            }
            break;
        case FLOOD_DAMAGE:
            if (square->property_group == LIGHT_BLUE || square->property_group == ORANGE)
            {
                square->current_market_value = apply_percentage(square->current_market_value, 100 - 20);
            }
            break;
        case TRANSPORT_STRIKE:
            if (square->square_type == RAILWAY)
            {
                square->base_rent = apply_percentage(square->base_rent, 100 - 40);
            }
            break;
        case ELECTRICITY_TARIFF_INCREASE:
            if (square->property_index == 12)
            {
                square->base_rent = apply_percentage(square->base_rent, 100 + 25);
            }
            break;
        case WATER_SHORTAGE:
            if (square->property_index == 28)
            {
                square->base_rent = apply_percentage(square->base_rent, 100 + 20);
            }
            if (square->property_index == 27 || square->property_index == 29)
            { // Unawatuna, Hikkaduwa
                square->current_market_value = apply_percentage(square->current_market_value, 100 - 10);
            }
            break;
        default:
            break;
        }
    }
}

void remove_economic_event(Square *board, Economy *economy, EconmicEvents event)
{
    switch (event)
    {
    case ECONOMIC_RECESSION:
        economy->loan_interest_rate = remove_percentage(economy->loan_interest_rate, 100 + 15);
        break;
    case STOCK_MARKET_BOOM:
        economy->loan_interest_rate = remove_percentage(economy->loan_interest_rate, 100 - 10);
        break;
    default:
        break;
    }

    for (int i = 0; i < MAX_SQUARES; i++)
    {
        Square *square = &board[i];

        switch (event)
        {
        case TOURISM_BOOM:
            if (square->has_hotel == 1)
            {
                square->base_rent /= 2;
            }
            if (square->property_group == YELLOW || square->property_index == 25)
            {
                square->base_rent = remove_percentage(square->base_rent, 100 + 15);
            }
            break;
        case FUEL_CRISIS:
            if (square->square_type == RAILWAY)
            {
                square->base_rent /= 2;
            }
            if (square->square_type == PROPERTY)
            {
                square->hotel_constructionCost = remove_percentage(square->hotel_constructionCost, 100 + 20);
                square->house_constructionCost = remove_percentage(square->house_constructionCost, 100 + 20);
            }
            break;
        case HEAVY_MONSOON:
            if (square->property_group == LIGHT_BLUE || square->property_group == ORANGE)
            {
                square->current_market_value = remove_percentage(square->current_market_value, 100 - 10);
            }
            break;
        case ECONOMIC_RECESSION:
            if (square->square_type == PROPERTY || square->square_type == RAILWAY || square->square_type == UTILITY)
            {
                square->current_market_value = remove_percentage(square->current_market_value, 100 - 15);
                square->base_rent = remove_percentage(square->base_rent, 100 - 10);
            }
            break;
        case STOCK_MARKET_BOOM:
            if (square->square_type == PROPERTY || square->square_type == RAILWAY || square->square_type == UTILITY)
            {
                square->current_market_value = remove_percentage(square->current_market_value, 100 + 10);
            }
            break;
        case GOVERNMENT_HOUSING_PROGRAMME:
            if (square->square_type == PROPERTY)
            {
                square->house_constructionCost = remove_percentage(square->house_constructionCost, 100 - 25);
            }
            break;
        case FOREIGN_INVESTMENT:
            if (square->property_group == GREEN || square->property_group == DARK_BLUE)
            {
                square->current_market_value = remove_percentage(square->current_market_value, 100 + 20);
            }
            break;
        case POLITICAL_UNREST:
            if (square->has_hotel == 1)
            {
                square->base_rent = remove_percentage(square->base_rent, 100 - 50);
            }
            break;
        default:
            break;
        }
    }
}

void run_economic_event(Square *board, Economy *economy)
{
    EconmicEvents event = random_generator(0, MAX_ECONOMIC_EVENTS - 1);

    economy->active_economic_event = event;

    switch (event)
    {
    case ECONOMIC_RECESSION:
        economy->loan_interest_rate = apply_percentage(economy->loan_interest_rate, 100 + 15);
        break;
    case STOCK_MARKET_BOOM:
        economy->loan_interest_rate = apply_percentage(economy->loan_interest_rate, 100 - 10);
        break;
    default:
        break;
    }

    for (int i = 0; i < MAX_SQUARES; i++)
    {
        Square *square = &board[i];

        switch (event)
        {
        case TOURISM_BOOM:
            if (square->has_hotel == 1)
            {
                square->base_rent *= 2;
            }
            if (square->property_group == YELLOW || square->property_index == 25)
            {
                square->base_rent = apply_percentage(square->base_rent, 100 + 15);
            }
            break;
        case FUEL_CRISIS:
            if (square->square_type == RAILWAY)
            {
                square->base_rent *= 2;
            }

            if (square->square_type == PROPERTY)
            {
                square->hotel_constructionCost = apply_percentage(square->hotel_constructionCost, 100 + 20);
                square->house_constructionCost = apply_percentage(square->house_constructionCost, 100 + 20);
            }
            break;
        case HEAVY_MONSOON:
            if (square->property_group == LIGHT_BLUE || square->property_group == ORANGE)
            {
                square->current_market_value = apply_percentage(square->current_market_value, 100 - 10);
            }
            break;
        case ECONOMIC_RECESSION:
            if (square->square_type == PROPERTY || square->square_type == RAILWAY || square->square_type == UTILITY)
            {
                square->current_market_value = apply_percentage(square->current_market_value, 100 - 15);
                square->base_rent = apply_percentage(square->base_rent, 100 - 10);
            }

            break;
        case STOCK_MARKET_BOOM:
            if (square->square_type == PROPERTY || square->square_type == RAILWAY || square->square_type == UTILITY)
            {
                square->current_market_value = apply_percentage(square->current_market_value, 100 + 10);
            }
            break;
        case GOVERNMENT_HOUSING_PROGRAMME:
            if (square->square_type == PROPERTY)
            {
                square->house_constructionCost = apply_percentage(square->house_constructionCost, 100 - 25);
            }
            break;
        case FOREIGN_INVESTMENT:
            if (square->property_group == GREEN || square->property_group == DARK_BLUE)
            {
                square->current_market_value = apply_percentage(square->current_market_value, 100 + 20);
            }
            break;
        case POLITICAL_UNREST:
            if (square->has_hotel == 1)
            {
                square->base_rent = apply_percentage(square->base_rent, 100 - 50);
            }
            break;
        default:
            break;
        }
    }
}

void remove_government_regulation(Square *board, Economy *economy, GovernmentRegulations regulation)
{
    switch (regulation)
    {
    case INCREASE_PROPERTY_TAX:
        economy->income_tax_rate = remove_percentage(economy->income_tax_rate, 100 + 50);
        break;
    case REDUCE_LOAN_INTEREST:
        economy->loan_interest_rate += 2;
        break;
    default:
        break;
    }

    for (int i = 0; i < MAX_SQUARES; i++)
    {
        Square *square = &board[i];
        switch (regulation)
        {
        case HOUSING_SUBSIDY:
            if (square->square_type == PROPERTY)
            {
                square->house_constructionCost = remove_percentage(square->house_constructionCost, 100 - 30);
            }
            break;
        case RAILWAY_MODERNIZATION:
            if (square->square_type == RAILWAY)
            {
                square->base_rent = remove_percentage(square->base_rent, 100 + 25);
            }
            break;
        case ELECTRICAL_TARIFF_REVISION:
            if (square->square_type == UTILITY)
            {
                square->base_rent = remove_percentage(square->base_rent, 100 + 20);
            }
            break;
        default:
            break;
        }
    }
}

void run_government_regulations(Square *board, Economy *economy, Player *players)
{
    GovernmentRegulations regulation = random_generator(0, MAX_GOVERNMENT_REGULATIONS - 1);

    economy->active_government_regulation = regulation;

    switch (regulation)
    {
    case INCREASE_PROPERTY_TAX:
        economy->income_tax_rate = apply_percentage(economy->income_tax_rate, 100 + 50);
        break;
    case REDUCE_LOAN_INTEREST:
        economy->loan_interest_rate -= 2;
        break;
    default:
        break;
    }

    for (int i = 0; i < MAX_SQUARES; i++)
    {
        Square *square = &board[i];
        switch (regulation)
        {
        case HOUSING_SUBSIDY:
            if (square->square_type == PROPERTY)
            {
                square->house_constructionCost = apply_percentage(square->house_constructionCost, 100 - 30);
            }
            break;
        case RAILWAY_MODERNIZATION:
            if (square->square_type == RAILWAY)
            {
                square->base_rent = apply_percentage(square->base_rent, 100 + 25);
            }
            break;
        case ELECTRICAL_TARIFF_REVISION:
            if (square->square_type == UTILITY)
            {
                square->base_rent = apply_percentage(square->base_rent, 100 + 20);
            }
            break;
        case LUXURY_PROPERTY_TAX:
            if (board[i].has_hotel == 1 && board[i].ownership != UNOWNED)
            {
                int maintenance_tax = apply_percentage(square->current_market_value, 25);

                Player *owner = &players[board[i].ownership];

                if (check_player_bankrupt(board, owner, players, maintenance_tax, *economy) == 0)
                {
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