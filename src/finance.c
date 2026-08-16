#include <stdio.h>
#include "include/finance.h"
#include "include/board.h"
#include "include/players.h"
#include "include/types.h"
#include "include/utils.h"

int get_repair_cost(Square property)
{
    if (property.has_hotel == 1)
    {
        return apply_percentage(property.hotel_constructionCost, DAMAGED_BUILDING_REPAIR_PERCENTAGE);
    }
    else if (property.house_count > 0)
    {
        return apply_percentage(property.house_constructionCost, DAMAGED_BUILDING_REPAIR_PERCENTAGE) * property.house_count;
    }
    return 0;
}

void repair_damaged_property(Square *property, Player *player)
{
    int repair_cost = get_repair_cost(*property);

    if (player->cash >= repair_cost)
    {
        printf("\t%s spend LKR %d on repairing %s due to a disaster.\n", player->player_name, repair_cost, property->square_name);
        player->cash -= repair_cost;
        printf("\tRemaining Balance: LKR %d\n", player->cash);
        property->damaged_by = -1;

        property->hotel_condition = 100;
        for (int i = 0; i < property->house_count; i++)
        {
            property->house_conditons[i] = 100;
        }
        property->is_damaged = 0;
    }
    else
    {
        printf("\t%s does not have enough cash to repair the property\n", player->player_name);
    }
}

void check_insurance_compensation(Square property, Player *player)
{
    int repair_cost = get_repair_cost(property);
    int compensation = 0;
    switch (property.insurance_type)
    {
    case BASIC_PROPERTY:
        // Basic covers fire and flood 
        if (property.damaged_by == FIRE || property.damaged_by == FLOOD)
        {
            compensation = apply_percentage(repair_cost, 80);
        }
        break;
    case COMPREHENSIVE:
        // Comprehensive all
        compensation = repair_cost;
        break;
    case BUSINESS_INTERRUPTION:
        // Covers 100 + 5 rounds of hotel rent
        compensation = repair_cost + (property.base_rent * 10 * 5);
        break;
    default:
        break;
    }
    player->cash += compensation;
    printf("\n\tInsurance Claim Approved\n");
    printf("\tCompensation Paid : LKR %d.\n", compensation);
    printf("\tRemaining Balance : LKR %d.\n", player->cash);
}

int calculate_insurance_premium(Square property, InsuranceType insurance_type, Economy economy, Player player)
{
    int rate = 0;

    switch (insurance_type)
    {
    case BASIC_PROPERTY:
        rate = 5;
        break;
    case COMPREHENSIVE:
        rate = 10;
        break;
    case BUSINESS_INTERRUPTION:
        rate = 15;
        break;
    default:
        return 0;
    }

    int premium = apply_percentage(property.current_market_value, rate);

    if (economy.active_government_regulation == INSURANCE_REGULATION)
    {
        premium = apply_percentage(premium, 100 - 15);
    }

    if (player.active_national_event_card.index == 15) // Insurance Discount
    {
        premium = apply_percentage(premium, 100 - 20);
    }

    return premium;
}

void execute_property_renovation(Square *property, Player *player)
{
    int renovation_cost = apply_percentage(property->current_market_value, 10);
    property->property_age = 0;
    printf("\n\t%s renovated %s for LKR %d.\n", player->player_name, property->square_name, renovation_cost);
    player->cash -= renovation_cost;
    printf("\tRemaining Balance : LKR %d.\n", player->cash);
}

void execute_insurance_transaction(Square *property, Player *player, InsuranceType insurance_type, Economy economy)
{

    char insurance_types[][30] = {
        [NO_INSURANCE] = "",
        [BASIC_PROPERTY] = "Basic Property",
        [COMPREHENSIVE] = "Comprehensive",
        [BUSINESS_INTERRUPTION] = "Business Interruption"};

    int premium = calculate_insurance_premium(*property, insurance_type, economy, *player);

    player->cash -= premium;

    if (property->insurance_type == insurance_type) // renewing a insurance
    {
        printf("\n\t* Renewed Insurance\n");
    }
    else
    {
        // new purchase or update insurance
        property->insurance_type = insurance_type;
        property->is_insured = 1;
        printf("\n\t* Purchasing Insurance\n");
    }

    property->insurance_rounds_remaining = MAX_INSURANCE_ROUNDS;

    printf("\t%s Insurance active on %s.\n", insurance_types[insurance_type], property->square_name);
    printf("\tPremium : LKR %d.\n", premium);
    printf("\tRemaining Balance : LKR %d.\n", player->cash);
}

int calculate_net_worth(Square *board, Player player)
{
    int net_worth = 0;

    net_worth += player.cash;

    if (player.has_active_loan == 1)
    {
        net_worth -= player.loan_amount;
    }

    for (int i = 0; i < MAX_SQUARES; i++)
    {
        Square square = board[i];

        if (square.ownership != player.playerId)
        {
            continue;
        }

        net_worth += square.current_market_value;

        if (square.is_mortgage == 1)
        {
            net_worth -= square.mortgage_value;
        }

        if (square.has_hotel == 1)
        {
            net_worth += square.hotel_constructionCost;
        }
        else if (square.house_count > 0)
        {
            net_worth += square.house_constructionCost * square.house_count;
        }
    }
    return net_worth;
}

void execute_tax_collection(Player *player, int tax_amount)
{
    player->cash -= tax_amount;
    printf("\tRemaining Balance : LKR %d.\n", player->cash);
}

int check_building_rent(int condition)
{
    if (condition >= 90 && condition <= 100)
    {
        return 100; // full rent
    }
    else if (condition >= 75 && condition <= 89)
    {
        return 90;
    }
    else if (condition >= 50 && condition <= 74)
    {
        return 75;
    }
    else if (condition >= 25 && condition <= 49)
    {
        return 50;
    }
    else
    {
        return -1;
    }
}

void execute_renovation(Square *property, Player *player, int house_index)
{

    int renovation_cost = 0;
    if (property->has_hotel == 1)
    {
        renovation_cost = apply_percentage(property->hotel_constructionCost, HOTEL_RENOVATION_COST_PERCENTAGE);
        if (property->hotel_condition < 60)
        {
            renovation_cost = apply_percentage(renovation_cost, 150); 
        }
        player->cash -= renovation_cost;

        printf("\n\t%s renovated a hotel in %s from %d%% to 100%% condition for LKR %d.\n", player->player_name, property->square_name, property->hotel_condition, renovation_cost);
        property->hotel_condition = 100;
        printf("\tRemaining Balance : LKR %d.\n", player->cash);
    }
    else
    {
        renovation_cost = apply_percentage(property->house_constructionCost, HOUSE_RENOVATION_COST_PERCENTAGE);
        if (property->house_conditons[house_index] < 60)
        {
            renovation_cost = apply_percentage(renovation_cost, 150); 
        }
        player->cash -= renovation_cost;

        printf("\n\t%s renovated a house %s from %d%% to 100%% condition for LKR %d.\n", player->player_name, property->square_name, property->house_conditons[house_index], renovation_cost);
        property->house_conditons[house_index] = 100;
        printf("\tRemaining Balance : LKR %d.\n", player->cash);
    }
}

void pay_bail(Player *player)
{
    player->cash -= BAIL_AMOUNT;
    printf("\n\t%s paid LKR %d for bail.\n", player->player_name, BAIL_AMOUNT);
    printf("\tRemaining Balance : LKR %d.\n", player->cash);
}

void execute_purchase(Square *square, Player *player, int purchase_price)
{
    player->cash -= purchase_price;
    square->ownership = player->playerId;

    printf("\n\t%s purchased %s for LKR %d.\n", player->player_name, square->square_name, purchase_price);
    printf("\tRemaining Balance : LKR %d.\n", player->cash);
}

void pay_rent(Square *square, Player *player, Player *owner, int rent)
{
    printf("\n\t%s landed on %s.\n", player->player_name, square->square_name);
    printf("\tRent Paid : LKR %d.\n", rent);
    printf("\tOwner : %s.\n", owner->player_name);
    player->cash -= rent;
    owner->cash += rent;
    printf("\tRemaining Balance : LKR %d.\n", player->cash);
}

void execute_construction(Square *property, Player *player)
{
    if (property->has_hotel == 0)
    {
        if (property->house_count == MAX_HOUSES)
        {
            property->has_hotel = 1;
            player->cash -= property->hotel_constructionCost;
            property->hotel_condition = 100;

            printf("\n\t%s upgraded %s to a Hotel.\n", player->player_name, property->square_name);
            printf("\tRemaining Balance : LKR %d.\n", player->cash);
        }
        else
        {
            property->house_conditons[property->house_count] = 100;
            property->house_count++;
            player->cash -= property->house_constructionCost;
            printf("\n\t%s constructed one house on %s.\n", player->player_name, property->square_name);
            printf("\tConstruction Cost : LKR %d.\n", property->house_constructionCost);
            printf("\tRemaining Balance : LKR %d.\n", player->cash);
        }
    }
}

int finance_construction_by_mortgage(Square *board, Player *player, int target_cost)
{
    while (player->cash < target_cost)
    {
        int lowest_index = -1;
        int lowest_value = 9999999;

        for (int i = 0; i < MAX_SQUARES; i++)
        {
            if (board[i].ownership != player->playerId)
            {
                continue;
            }

            if (board[i].is_mortgage == 1 || board[i].is_loan_locked == 1)
            {
                continue;
            }

            if (board[i].square_type == PROPERTY)
            {
                if (player_has_monopoly(board, player->playerId, board[i].property_group) == 1)
                {
                    continue;
                }
            }

            if (board[i].current_market_value < lowest_value)
            {
                lowest_value = board[i].current_market_value;
                lowest_index = i;
            }
        }

        if (lowest_index == -1)
        {
            break;
        }

        printf("\n\tMortgaging property %s for finance development.\n", board[lowest_index].square_name);
        execute_mortgage(&board[lowest_index], player);
    }

    return player->cash >= target_cost;
}

void run_auction(Square *square, Player *players, Economy economy)
{
    int bidding_price = square->current_market_value / 2;

    printf("\n---- Auction Started: %s (Starting Bid: LKR %d)\n", square->square_name, bidding_price);

    int player_has_withdrawn[MAX_PLAYERS] = {0, 0, 0, 0}; // for each player, 0 - active in the auction
    int active_bidders = 0;

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].is_bankrupt == 1)
        {
            player_has_withdrawn[i] = 1;
            continue;
        }
        active_bidders++;
    }

    if (active_bidders <= 1)
    {
        printf("\tNo one to bid.\n");
        return;
    }

    int highest_bid = bidding_price;
    int highest_bidder = -1;

    printf("\n\tActive bidders:\n");
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (player_has_withdrawn[i] == 0)
        {
            printf("\t\t- %-20s : LKR %d\n", players[i].player_name, players[i].cash);
        }
    }

    while (active_bidders > 1)
    {
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            if (active_bidders <= 1 && highest_bidder != -1)
            {
                break;
            }

            PlayerId current_bidding_player = i;

            if (player_has_withdrawn[current_bidding_player] == 1)
            {
                continue;
            }

            Player *player = &players[current_bidding_player];
            if (decide_bid(*square, *player, bidding_price, economy) == 0)
            {
                printf("\n\t%s withdrew from the bid at LKR %d.\n", player->player_name, highest_bid);
                player_has_withdrawn[current_bidding_player] = 1;
                active_bidders--;
            }
            else
            {
                printf("\n\t%s placed a bid for LKR %d.", player->player_name, bidding_price);
                highest_bid = bidding_price;
                highest_bidder = current_bidding_player;
                bidding_price += BID_INCREMENT;
            }
        }
    }

    if (highest_bidder != -1)
    {
        execute_purchase(square, &players[highest_bidder], highest_bid);
    }
    else
    {
        printf("\n\tNo bids were placed, %s remained unowned.\n", square->square_name);
    }
}

void process_loan_default(Square *board, Player *player, Player *players, Economy economy)
{
    int forecloased_properties[MAX_SQUARES] = {0};
    int forecloased_property_count = 0;

    printf("\n\t%s has defaulted.\n", player->player_name);
    printf("\tCollateral has been foreclosed:\n");
    for (int i = 0; i < MAX_SQUARES; i++)
    {

        if (board[i].ownership != player->playerId)
        {
            continue;
        }

        if (board[i].is_loan_locked == 0)
        {
            continue;
        }

        printf("\t\t- %s\n", board[i].square_name);
        board[i].ownership = UNOWNED;
        board[i].is_loan_locked = 0;
        board[i].has_hotel = 0;
        board[i].house_count = 0;
        board[i].is_mortgage = 0;
        board[i].is_insured = 0;
        board[i].insurance_type = NO_INSURANCE;
        board[i].insurance_rounds_remaining = 0;

        forecloased_properties[forecloased_property_count] = board[i].property_index;
        forecloased_property_count++;
    }

    player->loan_amount = 0;
    player->loan_interest_rate = 0;
    player->loan_rounds_remaining = 0;
    player->has_active_loan = 0;
    printf("\tOutstanding debt cleared.\n");

    for (int i = 0; i < forecloased_property_count; i++)
    {
        run_auction(&board[forecloased_properties[i]], players, economy);
    }
}

void liquidate_player_assets(Square *board, Player *player, Player *players, Economy economy)
{
    int forecloased_properties[MAX_SQUARES] = {0};
    int forecloased_property_count = 0;

    player->cash = 0;

    if (player->has_active_loan == 1)
    {
        process_loan_default(board, player, players, economy);
    }

    printf("\n\tFollowing properties are transferred to Bank:\n");
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        if (board[i].ownership != player->playerId)
        {
            continue;
        }

        if (board[i].is_loan_locked == 1)
        {
            continue;
        }

        printf("\n\t\t - %s", board[i].square_name);

        board[i].ownership = UNOWNED;
        board[i].is_loan_locked = 0;
        board[i].has_hotel = 0;
        board[i].house_count = 0;
        board[i].is_mortgage = 0;
        board[i].is_insured = 0;
        board[i].insurance_type = NO_INSURANCE;
        board[i].insurance_rounds_remaining = 0;

        forecloased_properties[forecloased_property_count] = board[i].property_index;
        forecloased_property_count++;
    }

    for (int i = 0; i < forecloased_property_count; i++)
    {
        run_auction(&board[forecloased_properties[i]], players, economy);
    }
}

void check_player_loan(Square *board, Player *player, Player *players, Economy economy)
{
    if (player->loan_rounds_remaining > 0)
    {
        player->loan_amount += apply_percentage(player->loan_amount, player->loan_interest_rate);
        player->loan_rounds_remaining--;
        printf("\n\t%s has an outstanding loan of LKR %d.\n", player->player_name, player->loan_amount);
        printf("\tLoan due in %d Rounds.", player->loan_rounds_remaining);

        if (calculate_net_worth(board, *player) < 0)
        {
            printf("\n\t%s's liabilities exceed total assets! Declared bankrupt.\n", player->player_name);
            player->is_bankrupt = 1;

            int active_player_count = 0;
            for (int i = 0; i < MAX_PLAYERS; i++)
            {
                if (players[i].is_bankrupt == 0)
                {
                    active_player_count++;
                }
            }
            if (active_player_count > 1)
            {
                liquidate_player_assets(board, player, players, economy);
            }
        }
    }
    else
    {
        process_loan_default(board, player, players, economy);
    }
}

void loan_period_extention(Player *player)
{
    player->loan_rounds_remaining += MAX_LOAN_ROUNDS;
    printf("\n\t%s's loan extended by %d Rounds.", player->player_name, MAX_LOAN_ROUNDS);
}

void repay_loan(Square *board, Player *player, int payment_amount)
{
    player->cash -= payment_amount;
    player->loan_amount -= payment_amount;

    if (player->loan_amount <= 0)
    {
        player->loan_amount = 0;
        player->loan_interest_rate = 0;
        player->loan_rounds_remaining = 0;
        player->has_active_loan = 0;

        for (int i = 0; i < MAX_SQUARES; i++)
        {
            if (board[i].ownership != player->playerId)
            {
                continue;
            }
            board[i].is_loan_locked = 0;
        }
        printf("\t%s fully settled the loan.\n", player->player_name);
    }
    else
    {
        printf("\t%s repaid LKR %d.\n", player->player_name, payment_amount);
        printf("\tOutstanding Balance : LKR %d.\n", player->loan_amount);
    }
}

int calculate_loan_amount(Square *board, int *eligible_properties, int eligible_property_count)
{

    int total_mortage_value = 0;
    for (int i = 0; i < eligible_property_count; i++)
    {
        total_mortage_value += board[eligible_properties[i]].mortgage_value;
    }

    int loan_amount = apply_percentage(total_mortage_value, 75);

    return loan_amount;
}

void issue_loan(Square *board, Player *player, Economy economy, int *eligible_properties, int eligible_property_count, int loan_amount)
{
    player->cash += loan_amount;
    player->loan_amount += loan_amount;

    if (player->has_active_loan == 0)
    {
        player->has_active_loan = 1;
        player->loan_interest_rate = economy.loan_interest_rate;
        player->loan_rounds_remaining = MAX_LOAN_ROUNDS;

        printf("\n\t%s obtained a secured loan.\n", player->player_name);
        printf("\tLoan Amount : LKR %d.\n", loan_amount);
        printf("\tCollateral :\n");
        for (int i = 0; i < eligible_property_count; i++)
        {
            printf("\t\t- %s\n", board[eligible_properties[i]].square_name);
        }
    }
    else
    {
        printf("\n\t%s increased the loan amount by LKR %d.\n", player->player_name, loan_amount);
        printf("\tAdditional Collateral :\n");
        for (int i = 0; i < eligible_property_count; i++)
        {
            printf("\t\t- %s\n", board[eligible_properties[i]].square_name);
        }
    }

    for (int i = 0; i < eligible_property_count; i++)
    {
        board[eligible_properties[i]].is_loan_locked = 1;
    }

    printf("\tInterest Rate : %d%%\n", player->loan_interest_rate);
    printf("\tDuration : %d Rounds\n", player->loan_rounds_remaining);
}

void execute_mortgage(Square *square, Player *player)
{
    square->is_mortgage = 1;
    player->cash += square->mortgage_value;

    printf("\t%s mortgaged %s for LKR %d.\n", player->player_name, square->square_name, square->mortgage_value);
    printf("\tRemaining cash: LKR %d.\n", player->cash);
}

void execute_unmortgage(Square *square, Player *player)
{
    player->cash -= square->mortgage_value;
    square->is_mortgage = 0;

    printf("\n\t%s unmortgaged %s for LKR %d.\n",
           player->player_name, square->square_name, square->mortgage_value);
    printf("\t  Remaining Balance : LKR %d.\n", player->cash);
}

int check_player_bankrupt(Square *board, Player *player, Player *players, int debt_amount, Economy economy)
{
    if (player->cash >= debt_amount)
    {
        return 0;
    }

    printf("\n\t%s has insufficient cash (LKR %d) to pay LKR %d. Mortgaging properties...\n", player->player_name, player->cash, debt_amount);

    for (int i = 0; i < MAX_SQUARES; i++)
    {
        if (board[i].ownership != player->playerId)
        {
            continue;
        }

        if (board[i].is_mortgage == 1)
        {
            continue;
        }

        if (board[i].is_loan_locked == 1)
        {
            continue;
        }   

        if (board[i].has_hotel == 1 || board[i].house_count > 0) {
            continue;
        }

        execute_mortgage(&board[i], player);

        if (player->cash >= debt_amount)
        {
            return 0;
        }
    }

    player->is_bankrupt = 1;
    printf("\n\t%s declares bankrupt.\n", player->player_name);

    int active_player_count = 0;
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].is_bankrupt == 0)
        {
            active_player_count++;
        }
    }

    if (active_player_count == 1)
    {
        return 1;
    }

    liquidate_player_assets(board, player, players, economy);

    return 1; // bankrupt
}