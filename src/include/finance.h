#ifndef FINANCE_H
#define FINANCE_H

#include "types.h"

void execute_purchase(Square *square, Player *player, int purchase_price);
void pay_rent(Square *square, Player *player, Player *owner, int rent);

void execute_construction(Square *property, Player *player);
void execute_renovation(Square *square, Player *player, int house_index);

void run_auction(Square *square, Player *players);

int check_player_bankrupt(Square *board, Player *player, Player *players, int debt_amount);

int check_building_rent(int avg_condition);

void pay_bail(Player *player);
void check_player_loan(Square *board, Player *player, Player *players);
void loan_period_extention(Player *player);
void repay_loan(Square *board, Player *player, int payment_amount);
int calculate_loan_amount(Square *board, int *eligible_properties, int eligible_property_count);
void issue_loan(Square *board, Player *player, int *eligible_properties, int eligible_property_count, int loan_amount);

#endif