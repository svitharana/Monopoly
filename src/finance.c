#include <stdio.h>
#include "include/finance.h"
#include "include/players.h"
#include "include/utils.h"

void execute_purchase(Square *square, Player *player, int purchase_price)
{
    player->cash -= purchase_price;
    square->ownership = player->playerId;

    printf("\n\t%s purchased %s for LKR %d.\n", player->player_name, square->square_name, purchase_price);
    printf("\tRemaining Balance : LKR %d.\n", player->cash);
}

void pay_rent(Square *square, Player *player, Player *owner, int rent)
{
    printf("\n\t%s owned by %s.\n", square->square_name, owner->player_name);
    printf("\tRent Paid : LKR %d.\n", rent);
    player->cash -= rent;
    owner->cash += rent;
    printf("\tRemaining Balance : %d.\n", player->cash);
}

void execute_construction(Square *property, Player *player)
{
    if (property->has_hotel == 0)
    {
        if (property->house_count == MAX_HOUSES)
        {
            property->has_hotel = 1;
            player->cash -= property->hotel_constructionCost;

            printf("%s upgraded %s to a Hotel for LKR %d.\n", player->player_name, property->square_name, property->hotel_constructionCost);
            printf("Remaining Balance: LKR %d.\n", player->cash);
        }
        else
        {
            property->house_count++;
            player->cash -= property->house_constructionCost;
            printf("%s constructed one house on %s for LKR %d.\n", player->player_name, property->square_name, property->house_constructionCost);
            printf("Remaining Balance: LKR %d.\n", player->cash);
        }
    }
}

void run_auction(Square *square, Player *players, PlayerId starting_playerId)
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
        for (int i = 0; i < MAX_PLAYERS && active_bidders > 1; i++)
        {
            PlayerId current_bidding_player = (starting_playerId + i) % MAX_PLAYERS; // to make sure, the player who declined start the bidding

            if (player_has_withdrawn[current_bidding_player] == 1)
            {
                continue;
            }

            Player *player = &players[current_bidding_player];
            if (decide_bid(*square, *player, bidding_price) == 0)
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
        printf("\n\tNo bids were placed, %s remaind unowned.\n", square->square_name);
    }
}

void process_loan_default(Square *board, Player *player)
{
    printf("\n\t%s has defaulted.", player->player_name);
    printf("\n\tFollowing collateral has been foreclosed:");
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

        printf("\n\t\t - %s", board[i].square_name);
        board[i].ownership == UNOWNED;
        board[i].is_loan_locked == 0;
        board[i].has_hotel = 0;
        board[i].house_count = 0;
        board[i].builing_condition = 100;
        board[i].is_mortgage = 0;

        player->loan_amount = 0;
        player->loan_interest_rate = 0;
        player->loan_rounds_remaining = 0;
        player->has_active_loan = 0;
    }
}

int check_player_loan(Square *board, Player *player)
{
    if (player->loan_rounds_remaining > 0)
    {
        player->loan_amount += apply_percentage(player->loan_amount, player->loan_interest_rate);
        player->loan_rounds_remaining--;
        printf("\n\t%s has an outstanding loan of LKR %d.\n", player->player_name, player->loan_amount);
        printf("\tLoan due in %d Rounds.", player->loan_rounds_remaining);
    }
    else
    {
        process_loan_default(board, player);
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

    if (player->loan_amount == 0)
    {
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
        printf("\tOutstanding: LKR %d.\n", player->loan_amount);
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

void issue_loan(Square *board, Player *player, int *eligible_properties, int eligible_property_count, int loan_amount)
{
    int interest_rate = 8;
    player->cash += loan_amount;
    player->loan_amount += loan_amount;

    if (player->has_active_loan == 0)
    {
        player->has_active_loan = 1;
        player->loan_interest_rate = interest_rate;
        player->loan_rounds_remaining = MAX_LOAN_ROUNDS;

        printf("\n\t%s has obtained a secured loan\n", player->player_name);
        printf("\t\tLoan amount: LKR %d.\n", loan_amount);

        printf("\t\tCollateral:\n");
        for (int i = 0; i < eligible_property_count; i++)
        {
            printf("\t\t\t%s\n", board[eligible_properties[i]].square_name);
        }
    }
    else
    {
        printf("\n\t%s has increased the loan amount by LKR %d.\n", player->player_name, loan_amount);
        printf("\t\tAdditional Collateral:\n");
        for (int i = 0; i < eligible_property_count; i++)
        {
            printf("\t\t\t%s\n", board[eligible_properties[i]].square_name);
        }
    }

    for (int i = 0; i < eligible_property_count; i++)
    {
        board[eligible_properties[i]].is_loan_locked = 1;
    }

    printf("\n\tInterest rate: %d%\n", player->loan_interest_rate);
    printf("\tDuration: %d rounds\n", player->loan_rounds_remaining);
}

void liquidate_player_assets(Square *board, Player *player)
{
    if (player->has_active_loan == 1)
    {
        process_loan_default(board, player);
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

        // TODO: Need to add insurance policy
        printf("\n\t\t - %s", board[i].square_name);

        board[i].ownership = UNOWNED;
        board[i].is_loan_locked == 0;
        board[i].has_hotel = 0;
        board[i].house_count = 0;
        board[i].builing_condition = 100;
        board[i].is_mortgage = 0;
    }
}

int check_player_bankrupt(Square *board, Player *player, int debt_amount)
{
    if (player->cash >= debt_amount)
    {
        return 0;
    }

    player->is_bankrupt = 1;
    printf("\n\t%s declares bankrupt.\n", player->player_name);
    liquidate_player_assets(board, player);

    return 1; // bankrupt
}