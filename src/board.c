#include <stdio.h>
#include "include/board.h"
#include "include/finance.h"
#include "include/players.h"

// PROPERTY
static void resolve_property(Square *square, Player *players, Player *player)
{
    if (square->ownership == UNOWNED)
    {
        if (decide_purchase(square, player) == 0)
        {
            return;
        }
        execute_purchase(square, player);
    }
    else if (square->ownership != player->playerId)
    {
        int rent = square->base_rent;
        int rent_multiplier[] = {0, 2, 3, 5, 7};
        Player *property_owner = &players[square->ownership];

        if (square->hasHotel == 1)
        {
            rent *= 10;
        }
        else
        {
            rent *= rent_multiplier[square->house_count];
        }
        pay_rent(square, player, property_owner, rent);
    }
}

// RAILWAY STATION
static void resolve_railwayStation(Square *board, Square *square, Player *players, Player *player)
{
    if (square->ownership == UNOWNED)
    {
        if (!decide_purchase(square, player))
        {
            return;
        }
        execute_purchase(square, player);
    }
    else if (square->ownership != player->playerId)
    {
        int rent = square->base_rent;

        Player *railwayStation_owner = &players[square->ownership];
        int railwayStation_count = 0;

        for (int i = 0; i < MAX_SQUARES; i++)
        {
            if (board[i].square_type == RAILWAY && board[i].ownership == railwayStation_owner->playerId)
            {
                railwayStation_count++;
            }
        }

        // rent multiplier
        int multiplier = 1 << (railwayStation_count - 1);

        rent *= multiplier;

        pay_rent(square, player, railwayStation_owner, rent);
    }
}

// UTILITY COMPANIES
static void resolve_utilityCompany(Square *board, Square *square, Player *players, Player *player)
{
    if (square->ownership == UNOWNED)
    {
        if (decide_purchase(square, player) == 0)
        {
            return;
        }
        execute_purchase(square, player);
    }
    else if (square->ownership != player->playerId)
    {
        int rent = square->base_rent;

        Player *utilityCompany_owner = &players[square->ownership];
        int utilityCompany_count = 0;

        for (int i = 0; i < MAX_SQUARES; i++)
        {
            if (board[i].square_type == UTILITY && board[i].ownership == utilityCompany_owner->playerId)
            {
                utilityCompany_count++;
            }
        }

        switch (utilityCompany_count)
        {
        case 1:
            rent *= 2;
            break;
        case 2:
            rent *= 7;
            break;
        default:
            break;
        }
        pay_rent(square, player, utilityCompany_owner, rent);
    }
}

// JAIL
static void resolve_jail(Square *square, Player *player)
{
    if (player->current_position == GOTO_JAIL_SQUARE)
    {
        printf("\n\t%s went to Jail.\n", player->player_name);
        player->current_position = JAIL_SQUARE; // player moved to jail
        player->isInJail = 1;                   // player in jail
    }
}

void resolve_landingSquare(Square *board, Player *players, Player *player)
{
    // Square *square = &board[5];
    Square *square = &board[player->current_position];

    switch (square->square_type)
    {
    case PROPERTY:
        resolve_property(square, players, player);
        break;
    case RAILWAY:
        resolve_railwayStation(board, square, players, player);
        break;
    case UTILITY:
        resolve_utilityCompany(board, square, players, player);
        break;
    case JAIL:
        resolve_jail(square, player);
        break;

    default:
        break;
    }
}

int get_property_index_toBuild(Square *board, PropertyGroup group)
{
    int property_index = -1;
    int lowest_house_count = MAX_HOUSES + 1; // offset this to make hotel construction possible
    for (int p = 0; p < MAX_SQUARES; p++)
    {
        if (board[p].square_type != PROPERTY)
        {
            continue;
        }

        if (board[p].property_group != group)
        {
            continue;
        }

        if (board[p].hasHotel == 1)
        {
            continue;
        }
        if (board[p].house_count < lowest_house_count)
        {
            lowest_house_count = board[p].house_count;
            property_index = p; // property to build
        }
    }

    return property_index;
}

int player_has_monopoly(Square *board, PlayerId playerId, PropertyGroup group)
{
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        if (board[i].square_type != PROPERTY)
        {
            continue;
        }
        if (board[i].property_group != group)
        {
            continue;
        }

        if (board[i].ownership != playerId)
        {
            return 0;
        }
    }

    return 1;
}

void move_player(Player *player, int move_by, Square *board)
{
    int previous_position = player->current_position;
    player->current_position = (previous_position + move_by) % MAX_SQUARES;

    printf("\t%s moved from Square %d (%s) to Square %d (%s).\n", player->player_name, previous_position, board[previous_position].square_name, player->current_position, board[player->current_position].square_name);

    if (previous_position + move_by >= 40)
    {
        printf("\n\t%s passed GO.\n", player->player_name);
        printf("\tCollected LKR %d.\n", GO_PASSED_AMOUNT);
        player->cash += GO_PASSED_AMOUNT;

        printf("\tCurrent Balance : %d.\n", player->cash);
        player->player_round++;
    }
}

void initialize_board(Square *board)
{

    for (int i = 0; i < MAX_SQUARES; i++)
    {
        board[i].property_index = i;
        board[i].ownership = UNOWNED;
        board[i].purchase_price = 0;
        board[i].base_rent = 0;
        board[i].house_count = 0;
        board[i].hasHotel = 0;

        board[i].builing_condition = 100;
    }

    board[0].square_name = "GO";
    board[0].square_type = SPECIAL;

    board[1].square_name = "Pettah";
    board[1].square_type = PROPERTY;
    board[1].property_group = BROWN;
    board[1].purchase_price = 1500;
    board[1].base_rent = 100;

    board[2].square_name = "Community Development Fund";
    board[2].square_type = EVENT;

    board[3].square_name = "Maradana";
    board[3].square_type = PROPERTY;
    board[3].property_group = BROWN;
    board[3].purchase_price = 1800;
    board[3].base_rent = 120;

    board[4].square_name = "Income Tax";
    board[4].square_type = TAX;

    board[5].square_name = "Colombo Fort Railway Station";
    board[5].square_type = RAILWAY;

    board[6].square_name = "Bambalapitiya";
    board[6].square_type = PROPERTY;
    board[6].property_group = LIGHT_BLUE;
    board[6].purchase_price = 2500;
    board[6].base_rent = 180;

    board[7].square_name = "National Event Card";
    board[7].square_type = EVENT;

    board[8].square_name = "Wellawatte";
    board[8].square_type = PROPERTY;
    board[8].property_group = LIGHT_BLUE;
    board[8].purchase_price = 2700;
    board[8].base_rent = 200;

    board[9].square_name = "Mount Lavinia";
    board[9].square_type = PROPERTY;
    board[9].property_group = LIGHT_BLUE;
    board[9].purchase_price = 3000;
    board[9].base_rent = 220;

    board[10].square_name = "Jail / Just Visiting";
    board[10].square_type = JAIL;

    board[11].square_name = "Nugegoda";
    board[11].square_type = PROPERTY;
    board[11].property_group = PINK;
    board[11].purchase_price = 3500;
    board[11].base_rent = 260;

    board[12].square_name = "Ceylon Electricity Board";
    board[12].square_type = UTILITY;

    board[13].square_name = "Maharagama";
    board[13].square_type = PROPERTY;
    board[13].property_group = PINK;
    board[13].purchase_price = 3800;
    board[13].base_rent = 280;

    board[14].square_name = "Kottawa";
    board[14].square_type = PROPERTY;
    board[14].property_group = PINK;
    board[14].purchase_price = 4000;
    board[14].base_rent = 300;

    board[15].square_name = "Kandy Railway Station";
    board[15].square_type = RAILWAY;

    board[16].square_name = "Negombo";
    board[16].square_type = PROPERTY;
    board[16].property_group = ORANGE;
    board[16].purchase_price = 4500;
    board[16].base_rent = 350;

    board[17].square_name = "Sri Lanka Insurance";
    board[17].square_type = INSURANCE;

    board[18].square_name = "Katunayake";
    board[18].square_type = PROPERTY;
    board[18].property_group = ORANGE;
    board[18].purchase_price = 4700;
    board[18].base_rent = 370;

    board[19].square_name = "Ja-Ela";
    board[19].square_type = PROPERTY;
    board[19].property_group = ORANGE;
    board[19].purchase_price = 5000;
    board[19].base_rent = 400;

    board[20].square_name = "Free Parking";
    board[20].square_type = SPECIAL;

    board[21].square_name = "Kandy City";
    board[21].square_type = PROPERTY;
    board[21].property_group = RED;
    board[21].purchase_price = 5500;
    board[21].base_rent = 450;

    board[22].square_name = "National Event Card";
    board[22].square_type = EVENT;

    board[23].square_name = "Peradeniya";
    board[23].square_type = PROPERTY;
    board[23].property_group = RED;
    board[23].purchase_price = 5800;
    board[23].base_rent = 480;

    board[24].square_name = "Katugastota";
    board[24].square_type = PROPERTY;
    board[24].property_group = RED;
    board[24].purchase_price = 6000;
    board[24].base_rent = 500;

    board[25].square_name = "Galle Railway Station";
    board[25].square_type = RAILWAY;

    board[26].square_name = "Galle Fort";
    board[26].square_type = PROPERTY;
    board[26].property_group = YELLOW;
    board[26].purchase_price = 6500;
    board[26].base_rent = 600;

    board[27].square_name = "Unawatuna";
    board[27].square_type = PROPERTY;
    board[27].property_group = YELLOW;
    board[27].purchase_price = 6800;
    board[27].base_rent = 620;

    board[28].square_name = "National Water Supply and Drainage Board";
    board[28].square_type = UTILITY;

    board[29].square_name = "Hikkaduwa";
    board[29].square_type = PROPERTY;
    board[29].property_group = YELLOW;
    board[29].purchase_price = 7000;
    board[29].base_rent = 650;

    board[30].square_name = "Go To Jail";
    board[30].square_type = JAIL;

    board[31].square_name = "Jaffna Town";
    board[31].square_type = PROPERTY;
    board[31].property_group = GREEN;
    board[31].purchase_price = 8000;
    board[31].base_rent = 750;

    board[32].square_name = "Nallur";
    board[32].square_type = PROPERTY;
    board[32].property_group = GREEN;
    board[32].purchase_price = 8300;
    board[32].base_rent = 780;

    board[33].square_name = "Ceylinco Insurance";
    board[33].square_type = INSURANCE;

    board[34].square_name = "Trincomalee";
    board[34].square_type = PROPERTY;
    board[34].property_group = GREEN;
    board[34].purchase_price = 8500;
    board[34].base_rent = 800;

    board[35].square_name = "Jaffna Railway Station";
    board[35].square_type = RAILWAY;

    board[36].square_name = "National Event Card";
    board[36].square_type = EVENT;

    board[37].square_name = "Nuwara Eliya";
    board[37].square_type = PROPERTY;
    board[37].property_group = DARK_BLUE;
    board[37].purchase_price = 10000;
    board[37].base_rent = 1000;

    board[38].square_name = "Bank of Ceylon";
    board[38].square_type = BANK;

    board[39].square_name = "Galle Face";
    board[39].square_type = PROPERTY;
    board[39].property_group = DARK_BLUE;
    board[39].purchase_price = 12000;
    board[39].base_rent = 1200;

    for (int i = 0; i < MAX_SQUARES; i++)
    {

        if (board[i].square_type == RAILWAY)
        {
            board[i].purchase_price = 1500;
            board[i].base_rent = 250;
        }

        if (board[i].square_type == PROPERTY)
        {
            switch (board[i].property_group)
            {
            case BROWN:
                board[i].house_constructionCost = 500;
                board[i].hotel_constructionCost = 2000;
                break;
            case LIGHT_BLUE:
                board[i].house_constructionCost = 750;
                board[i].hotel_constructionCost = 3000;
                break;
            case PINK:
                board[i].house_constructionCost = 1000;
                board[i].hotel_constructionCost = 4000;
                break;
            case ORANGE:
                board[i].house_constructionCost = 1250;
                board[i].hotel_constructionCost = 5000;
                break;
            case RED:
                board[i].house_constructionCost = 1500;
                board[i].hotel_constructionCost = 6000;
                break;
            case YELLOW:
                board[i].house_constructionCost = 2000;
                board[i].hotel_constructionCost = 8000;
                break;
            case GREEN:
                board[i].house_constructionCost = 2500;
                board[i].hotel_constructionCost = 10000;
                break;
            case DARK_BLUE:
                board[i].house_constructionCost = 3000;
                board[i].hotel_constructionCost = 12000;
                break;
            }
        }
        board[i].mortgage_value = board[i].purchase_price / 2;
    }
}