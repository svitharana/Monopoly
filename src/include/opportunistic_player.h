#ifndef OPPORTUNISTIC_PLAYER_H
#define OPPORTUNISTIC_PLAYER_H

#include "types.h"

int opportunistic_decide_property_renovation(Square property, Player player);
int opportunistic_decide_renovation(Square square, Player player, int building_condition);
int opportunistic_decide_purchase(Square *board, Square *square, Player *player, Economy economy);
int opportunistic_decide_loan(Square *board, Player player, Economy economy);
int opportunistic_decide_loan_repayment(Player player, int *amount);
int opportunistic_desired_loan_amount(Square *board, Player player);
int opportunistic_decide_loan_collateral(Square *board, Player player, int *eligible_properties, int eligible_count, int *selected_collateral, int desired_amount);
int opportunistic_decide_loan_extention(Player player);
int opportunistic_decide_loan_refinance(Square *board, Player player);
int opportunistic_decide_construction(Square property, Player player, Economy economy);
int opportunistic_decide_mortgage_for_construction(Square *board, Player player, int target_cost);
InsuranceType opportunistic_decide_insurance(Square property, Economy economy, Player player);
int opportunistic_decide_bid(Square square, Player player, int bidding_price, Economy economy);

#endif
