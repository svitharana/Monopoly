#include <stdio.h>
#include "include/finance.h"

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
    if (property->hasHotel == 0)
    {
        if (property->house_count == MAX_HOUSES)
        {
            property->hasHotel = 1;
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
    int bidding_price = square->purchase_price / 2;

    printf("\n---- Auction Started: %s (Starting Bid: LKR %d)\n", square->square_name, bidding_price);

    int player_has_withdrawn[MAX_PLAYERS] = {0, 0, 0, 0}; // for each player, 0 - active in the auction
    int active_bidders = 0;

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].isBankrupt == 1)
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
            printf("\t\t- %s\n", players[i].player_name);
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
            if (decide_makeBid(*square, *player, bidding_price) == 0)
            {
                printf("\n\t%s withdrew from the bid at LKR %d.\n", player->player_name, bidding_price - BID_INCREMENT);
                player_has_withdrawn[current_bidding_player] = 1;
                active_bidders--;
            }
            else
            {
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
        printf("\n\tNo bids were we placed, %s remaind unowned.\n", square->square_name);
    }
}