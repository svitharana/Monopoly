#ifndef FINANCE_H
#define FINANCE_H

#include "types.h"

void execute_purchase(Square *square, Player *player, int purchase_price);
void pay_rent(Square *square, Player *player, Player *owner, int rent);

void execute_construction(Square *property, Player *player);

void run_auction(Square *square, Player *players, PlayerId starting_playerId);

int check_player_loan(Square *board, Player *player);
void repay_loan(Square *board, Player *player, int payment_amount);
int calculate_loan_amount(Square *board, int *eligible_properties, int eligible_property_count);
void issue_loan(Square *board, Player *player, int *eligible_properties, int eligible_property_count, int loan_amount);

#endif