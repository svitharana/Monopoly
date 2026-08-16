#ifndef CONSERVATIVE_PLAYER_H
#define CONSERVATIVE_PLAYER_H

#include "types.h"

int conservative_decide_property_renovation(Square property, Player player);
int conservative_decide_renovation(Square square, Player player, int building_condition);
int conservative_decide_purchase(Square *square, Player *player, Economy economy);
int conservative_decide_loan(Square *board, Player player, Economy economy);
int conservative_decide_loan_repayment(Player player, int *amount);
int conservative_desired_loan_amount(Square *board, Player player);
int conservative_decide_loan_collateral(Square *board, Player player, int *eligible_properties, int eligible_count, int *selected_collateral, int desired_amount);
int conservative_decide_loan_extention(Player player);
int conservative_decide_loan_refinance(Square *board, Player player);
int conservative_decide_construction(Square property, Player player, Economy economy);
int conservative_decide_mortgage_for_construction(Square *board, Player player, int target_cost);
InsuranceType conservative_decide_insurance(Square property, Economy economy, Player player);
int conservative_decide_bid(Square square, Player player, int bidding_price, Economy economy);

#endif
