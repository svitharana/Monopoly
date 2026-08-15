#include <stdio.h>
#include "include/game.h"
#include "include/players.h"
#include "include/board.h"
#include "include/finance.h"
#include "include/events.h"
#include "include/types.h"
#include "include/utils.h"

int roll_dice()
{
    return random_generator(1, 6);
}

void display_players(PlayerOrder *playerOrder)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        printf("%s rolls %d.\n", playerOrder[i].player->player_name, playerOrder[i].rolled_value);
    }
    printf("\n");
}

// TODO: try a insertion sort
void bubble_sort(PlayerOrder *playerOrder)
{
    while (1)
    {
        int swapped = 0;

        for (int i = 0; i < MAX_PLAYERS - 1; i++)
        {
            PlayerOrder player = playerOrder[i];
            if (playerOrder[i].isOrderCorrect || playerOrder[i + 1].isOrderCorrect)
            {
                continue;
            }

            if (playerOrder[i].rolled_value < playerOrder[i + 1].rolled_value)
            {
                PlayerOrder temp = playerOrder[i + 1];
                playerOrder[i + 1] = playerOrder[i];
                playerOrder[i] = temp;
                swapped = 1;
            }
        }
        if (swapped == 0)
            break;
    }
}

void tie_breaker(PlayerOrder *playerOrder)
{
    int tied;

    do
    {
        tied = 0;
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            if (playerOrder[i].isOrderCorrect)
            {
                continue;
            }
            int isTied = 0;
            for (int j = i + 1; j < MAX_PLAYERS; j++)
            {
                if (playerOrder[i].rolled_value != playerOrder[j].rolled_value)
                {
                    break;
                }
                if (playerOrder[j].isOrderCorrect)
                {
                    break;
                }
                isTied = 1;
                tied = 1;
                i = j;
            }
            if (!isTied)
            {
                playerOrder[i].isOrderCorrect = 1;
            }
        }
        if (tied)
        {
            for (int i = 0; i < MAX_PLAYERS; i++)
            {
                if (!playerOrder[i].isOrderCorrect)
                {
                    playerOrder[i].rolled_value = roll_dice();
                    playerOrder[i].rolled_value += roll_dice();
                    printf("%s was tied so he rolled again and got %d\n", playerOrder[i].player->player_name, playerOrder[i].rolled_value);
                }
            }
            bubble_sort(playerOrder);
        }
    } while (tied);
}

void determine_playerOrder(PlayerOrder *playerOrder)
{

// TODO: remove before releasing
#ifdef DEBUG
    // Debug Mode: Bypass dice roll sorting and set Player 1 as starting player
    playerOrder[0].player = playerOrder[0].player;
    playerOrder[0].rolled_value = 12;
    playerOrder[0].isOrderCorrect = 1;

    print_heading("DEBUG MODE ACTIVE");
    printf("%s will begin the game (Debug Single-Player Mode).\n\n", playerOrder[0].player->player_name);
#else
    // Normal Mode: Original random turn order setup
    printf("\n--- Determining turn order ---\n");
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        playerOrder[i].rolled_value = roll_dice();
        playerOrder[i].rolled_value += roll_dice();
    }

    bubble_sort(playerOrder);
    display_players(playerOrder);

    tie_breaker(playerOrder);

    printf("\n%s will begin the game.\n", playerOrder[0].player->player_name);

    printf("\nTurn order:\n");
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        printf("%s\n", playerOrder[i].player->player_name);
    }

    printf("\n");
#endif
}

void initial_msg(Player *players)
{
    print_heading("MONOPOLY-LK Simulation");
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        printf("Player %d: %s\n", i + 1, players[i].player_name);
    }
    printf("\nEach player begins with LKR %d.\n", INITIAL_CASH);
}

void player_in_jail(Player *player, int dice_1, int dice_2)
{
    player->in_jail_turns++;

    if (player->in_jail_turns < MAX_TURNS_INJAIL)
    {
        if (dice_1 == dice_2)
        {
            printf("\n\t%s rolls doubles gets out of jail.\n", player->player_name);
            player->is_in_jail = 0;
            player->in_jail_turns = 0;
        }
        else
        {
            printf("\n\t%s remains in jail for %d more rounds.\n", player->player_name, MAX_TURNS_INJAIL - player->in_jail_turns);
        }
    }
    else
    {
        printf("\n\t%s gets out of jail after %d turns.\n", player->player_name, MAX_TURNS_INJAIL);
        player->is_in_jail = 0;
        player->in_jail_turns = 0;
    }
}

void check_for_monopoly(Square *board, Player *player)
{
    for (int group = 0; group < MAX_PROPERTY_GRPS; group++)
    {
        if (player_has_monopoly(board, player->playerId, group) == 1)
        {
            int announced = 0;
            while (1)
            {
                int property_index = get_property_index_toBuild(board, group);
                if (property_index == -1)
                {
                    break;
                }

                if (decide_construction(board[property_index], *player) == 0)
                {
                    break;
                }

                if (announced == 0)
                {
                    printf("\n%s has monopoly.\n", player->player_name);
                }
                execute_construction(&board[property_index], player);
            }
        }
    }
}

void play_turn(Player *players, PlayerId player_id, Square *board, Economy economy)
{
    for (int i = 0; i < MAX_SQUARES; i++)
    {
        if (board[i].is_damaged == 1)
        {
            printf("\tRepairing Damamged Buildings (%s)\n", board[i].square_name);
            repair_damaged_property(&board[i], &players[board[i].ownership]);
        }
    }

    Player *player = &players[player_id];

    for (int i = 0; i < MAX_SQUARES; i++) {
        if (board[i].ownership != player->playerId){
            continue;
        }

        if (board[i].is_mortgage == 0) {
            continue;
        }

        if (player->cash > board[i].mortgage_value * 2) {
            execute_unmortgage(&board[i], player);
        }
    }
    printf("\n---- %s's turn ----\n", player->player_name);

    if (player->is_in_jail == 1)
    {
        if (decide_pay_bail(player) == 1)
        {
            pay_bail(player);
            player->is_in_jail = 0;
            player->in_jail_turns = 0;
        }
    }

    // TODO: implement unmortgage
    
    resolve_renovations(board, player);

    int dice_1 = 0;
    int dice_2 = 0;

    // TODO: remove before releasing

#ifdef DEBUG
    // Debug Mode Menu: Interactive movement and state manipulation
    print_heading("DEBUG TURN MENU");
    printf("Active Player: %s | Position: Square %d (%s) | Cash: LKR %d\n",
           player->player_name, player->current_position, board[player->current_position].square_name, player->cash);
    printf("1. Roll Manual Dice Values\n");
    printf("2. Teleport directly to a Square (0-39)\n");
    printf("3. Add/Subtract Cash\n");
    printf("4. Roll Standard Random Dice\n");
    printf("Select Choice: ");

    int choice = 1;
    scanf("%d", &choice);

    if (choice == 1)
    {
        printf("Enter Dice 1 and Dice 2 (e.g., 3 4): ");
        scanf("%d %d", &dice_1, &dice_2);
    }
    else if (choice == 2)
    {
        int target_square;
        printf("Enter target Square Index (0 to 39): ");
        scanf("%d", &target_square);

        if (target_square >= 0 && target_square < MAX_SQUARES)
        {
            player->current_position = target_square;
            printf("%s teleported directly to Square %d (%s).\n",
                   player->player_name, target_square, board[target_square].square_name);
        }
        dice_1 = 0;
        dice_2 = 0;
    }
    else if (choice == 3)
    {
        int cash_change;
        printf("Enter Cash adjustment (+/- LKR): ");
        scanf("%d", &cash_change);
        player->cash += cash_change;
        printf("New Balance: LKR %d.\n", player->cash);

        dice_1 = roll_dice();
        dice_2 = roll_dice();
    }
    else
    {
        dice_1 = roll_dice();
        dice_2 = roll_dice();
    }
#else
    // Normal Mode: Automatic random dice rolling
    dice_1 = roll_dice();
    dice_2 = roll_dice();
#endif

    player->rolled_value = dice_1 + dice_2;

    printf("\t%s rolled %d and %d (total %d).\n", player->player_name, dice_1, dice_2, player->rolled_value);

    if (player->is_in_jail == 1)
    {
        player_in_jail(player, dice_1, dice_2);
    }

    if (player->is_in_jail != 1 && player->rolled_value > 0)
    {
        move_player(player, player->rolled_value, board);
    }

    resolve_landingSquare(board, players, player, economy);

    check_for_monopoly(board, player);
}

int check_game_round(Player *players, int game_round)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        Player *player = &players[i];
        if (player->is_in_jail == 1)
        {
            continue;
        }

        if (player->is_bankrupt == 1)
        {
            continue;
        }

        if (player->player_round > game_round)
        {
            continue;
        }

        return 0; // there is at least one player still in the current game round
    }

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].is_in_jail == 1)
        {
            players[i].player_round++;
        }
    }
    return 1; // all players (except in Jail) have passed go
}

void update_game_data(Square *board, Player *players, int game_round, Economy *economy)
{
    printf("\n========================================\n");
    printf("  Round %d Summary\n", game_round);
    printf("========================================\n\n");

    if (game_round % 15 == 0) {
        if (economy->active_regional_card != -1) {
            remove_regional_development_card_effect(board, economy->active_regional_card);
        }

        draw_regional_development_card(board, economy);
    }

    if (game_round % 10 == 0)
    {
        update_inflation(economy);
        update_board_data(economy, board);

        update_dynamic_property_market(board, economy);

        int damaged_property_index = create_disaster(board);
        if (damaged_property_index != -1)
        {

            Square damaged_property = board[damaged_property_index];

            char disasters[][30] = {
                [FLOOD] = "Flood",
                [FIRE] = "Fire",
                [RIOT] = "Riot",
                [BUILDING_COLLAPSE] = "Building Collapse",
                [ELECTRICAL_FAILURE] = "Electrical Failure"};

            printf("\tDisaster (%s)\n", disasters[damaged_property.damaged_by]);
            printf("\t----------------\n");
            printf("\tAffected Property: %s\n", damaged_property.square_name);
            printf("\tOwner: %s\n", players[damaged_property.ownership].player_name);
            players[damaged_property.ownership].incured_loss = 1;

            if (damaged_property.is_insured == 1)
            {
                check_insurance_compensation(damaged_property, &players[damaged_property.ownership]);
            }
            else
            {
                printf("\tNo insurance. %s bears repair costs.\n", players[damaged_property.ownership].player_name);
            }
        }
    }

    // boom or decline grp update
    for (int i = 0; i < MAX_PROPERTY_GRPS; i++)
    {
        if (economy->boom_decline_grp_cooldown[i] > 0)
        {
            economy->boom_decline_grp_cooldown[i]--;
        }
    }

    if (economy->boom_rounds_remaining > 0)
    {
        economy->boom_rounds_remaining--;
    }
    if (economy->decline_rounds_remaining > 0)
    {
        economy->decline_rounds_remaining--;
    }

    char property_grps[][30] = {
        [BROWN] = "Brown",
        [LIGHT_BLUE] = "Light Blue",
        [PINK] = "Pink",
        [ORANGE] = "Orange",
        [RED] = "Red",
        [YELLOW] = "Yellow",
        [GREEN] = "Green",
        [DARK_BLUE] = "Dark Blue"};

    printf("\n========================================\n");
    printf("  Current Market Conditions\n");
    printf("========================================\n\n");

    if (economy->boom_rounds_remaining > 0)
    {
        printf("\tMarket Boom\n");
        printf("\t----------------\n");
        printf("\t%s (+%d%%)\n", property_grps[economy->boom_group], 20);
        printf("\tRounds remaining : %d\n", economy->boom_rounds_remaining);

        printf("\n");

        printf("\tMarket Decline\n");
        printf("\t----------------\n");
        printf("\t%s (-%d%%)\n", property_grps[economy->decline_group], 15);
        printf("\tRounds remaining : %d\n", economy->decline_rounds_remaining);

        printf("\n");
    }

    printf("\tInflation\n");
    printf("\t----------------\n");
    printf("\t%d%%", economy->inflation);

    printf("\n");

    printf("\tCurrent Loan Interest\n");
    printf("\t------------------------\n");
    printf("\t%d%%", economy->loan_interest_rate);

    printf("\n");

    char card_names[][50] = {
    [SOUTHERN_TOURISM_BOOM] = "Southern Tourism Boom (+40%% Rent)",
    [PORT_CITY_EXPANSION] = "Port City Expansion (+25%% Value)",
    [IT_INDUSTRY_GROWTH] = "IT Industry Growth (+20%% Value)",
    [NORTHERN_DEVELOPMENT_PROGRAMME] = "Northern Development (+30%% Value)",
    [TEA_EXPORT_BOOM] = "Tea Export Boom (+35%% Value)",
    [AIRPORT_EXPANSION] = "Airport Expansion (+30%% Rent)",
    [UNIVERSITY_CITY_GROWTH] = "University City Growth (+20%% Value)",
    [BEACH_POLLUTION] = "Beach Pollution (-30%% Rent)",
    [FLOOD_DAMAGE] = "Flood Damage (-20%% Value)",
    [TRANSPORT_STRIKE] = "Transport Strike (-40%% Railway Rent)",
    [ELECTRICITY_TARIFF_INCREASE] = "Electricity Tariff Increase (+25%% Rent)",
    [WATER_SHORTAGE] = "Water Shortage (+20%% Utility, -10%% Value)"
};

printf("\n* Regional Development Card Drawn (Round %d)\n", game_round);
printf("  Active Card: %s\n", card_names[economy->active_regional_card]);

    for (int i = 0; i < MAX_SQUARES; i++)
    {
        Square *square = &board[i];

        if (square->ownership == UNOWNED)
        {
            continue;
        }

        if (square->square_type == PROPERTY)
        {
            if (square->has_hotel == 1)
            {
                square->hotel_condition -= 2;
            }
            else if (square->house_count > 0)
            {
                for (int j = 0; j < square->house_count; j++)
                {
                    square->house_conditons[j] -= 2;
                }
            }

            if (square->insurance_rounds_remaining > 0)
            {
                square->insurance_rounds_remaining--;

                if (square->insurance_rounds_remaining <= 3)
                {
                    printf("\t%s has %d rounds remaining for the insurance of %s property.\n", players[square->ownership].player_name, square->insurance_rounds_remaining, square->square_name);
                }

                if (square->insurance_rounds_remaining == 0)
                {
                    square->insurance_type = NO_INSURANCE;
                    square->is_insured = 0;
                    printf("\tInsurance for %s property has expired.\n", square->square_name);
                    printf("\tOwner: %s\n", players[square->ownership].player_name);
                }
            }
        }
    }

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].is_bankrupt == 1)
        {
            continue;
        }
        printf("\n---- %s's Summary ----\n", players[i].player_name);

        printf("\n\tCash : LKR %d.", players[i].cash);
        int property_count = 0;
        int hotel_count = 0;
        for (int j = 0; j < MAX_SQUARES; j++)
        {
            if (board[j].ownership != players[i].playerId)
            {
                continue;
            }
            property_count++;

            if (board[j].has_hotel == 1)
            {
                hotel_count++;
            }
        }
        printf("\n\tProperties : %d.", property_count);
        printf("\n\tHotels : %d.", hotel_count);

        // LOANs
        if (players[i].has_active_loan == 1)
        {
            check_player_loan(board, &players[i], players);
        }
    }

    printf("\n\tBankrupt Players.\n");
    printf("\t---------------------\n");
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].is_bankrupt == 1)
        {
            printf("\t%d. %s\n", i, players[i].player_name);
        }
    }
}

void show_winner(Player *players)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].is_bankrupt == 0)
        {
            printf("\n================= %s won the game by avoding bankruptcy =================\n", players[i].player_name);
        }
    }
}

void check_winner(Player *players)
{
    Player winner = players[0];

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].is_bankrupt == 1)
        {
            continue;
        }

        if (players[i].cash > winner.cash)
        {
            winner = players[i];
        }
    }

    printf("\n================= %s won the game by maximum assets =================\n", winner.player_name);
}

void start_game()
{
    Square board[MAX_SQUARES] = {0};
    Player players[MAX_PLAYERS] = {0};
    PlayerOrder playerOrder[MAX_PLAYERS] = {0};

    Economy economy = {0};

    economy.inflation = 0;
    economy.loan_interest_rate = INITIAL_LOAN_INTEREST_RATE;
    economy.income_tax_rate = 15;
    economy.community_fund_rate = 10;
    
    economy.active_regional_card = -1;

    int game_round = 1;
    int active_players = MAX_PLAYERS;

    initialize_board(board);
    initialize_players(players, playerOrder);
    initial_msg(players);

    determine_playerOrder(playerOrder);

    printf("\n========================================\n");
    printf("  Round 1\n");
    printf("========================================\n\n");

    Player *current_player = &players[0];
    int game_over = 0;

    while (game_over == 0)
    {

        // TODO: remove before releasing

#ifdef DEBUG
        // Debug Mode: Loop executes only for index 0 (your active player)
        for (int i = 0; i < 1; i++)
        {
            play_turn(players, playerOrder[i].player->playerId, board, economy);

            if (check_game_round(players, game_round) == 1)
            {
                game_round++;
                printf("\n========================================\n");
                printf("  Round %d\n", game_round);
                printf("========================================\n\n");
            }
        }
#else

        // Normal Mode: Loop executes across all players
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            // current_player = &players[playerOrder[i].player->playerId];
            current_player = playerOrder[i].player;

            if (current_player->is_bankrupt == 1)
            {
                continue;
            }

            play_turn(players, current_player->playerId, board, economy);

            if (current_player->is_bankrupt == 1)
            {
                active_players--;
            }

#ifndef NO_EARLY_WIN
            if (active_players == 1)
            {
                show_winner(players);
                game_over = 1;
                break;
            }
#endif

            if (check_game_round(players, game_round) == 1)
            {
                update_game_data(board, players, game_round, &economy);

                if (game_round == MAX_ROUNDS)
                {
                    game_over = 1;
                    check_winner(players);
                    break;
                }
                game_round++;
                printf("\n========================================\n");
                printf("  Round %d\n", game_round);
                printf("========================================\n\n");
                // print_summary(players, board);
            }
        }
#endif
    }
}