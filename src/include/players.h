#ifndef PLAYERS_H
#define PLAYERS_H

#include "types.h"

void initialize_players(Player *players, PlayerOrder *playerOrder);

InsuranceType decide_insurance(Square property, Economy economy, Player player);

int decide_purchase(Square *board, Square *square, Player *player, Economy economy);
int decide_construction(Square property, Player player, Economy economy);
int decide_bid(Square square, Player player, int bidding_price, Economy economy);
int decide_property_renovation(Square square, Player player);
int decide_renovation(Square square, Player player, int building_condition);
int decide_pay_bail(Square *board, Player *player);
int decide_loan(Square *board, Player player, Economy economy);
int decide_loan_extention(Player player);
int decide_loan_refinance(Square *board, Player player);
int decide_loan_repayment(Player player, int *amount);
int decide_loan_collateral(Square *board, Player player, int *eligible_properties, int eligible_count, int *selected_collateral, Economy economy);
int decide_mortgage_for_construction(Square *board, Player player, int target_cost);

#endif