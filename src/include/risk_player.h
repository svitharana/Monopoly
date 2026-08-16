#ifndef RISK_PLAYER_H
#define RISK_PLAYER_H

#include "types.h"

int risk_decide_property_renovation(Square property, Player player);
int risk_decide_renovation(Square square, Player player, int building_condition);
int risk_decide_purchase(Square *square, Player *player);
int risk_decide_loan(Square *board, Player player);
int risk_decide_loan_repayment(Player player, int *amount);
int risk_desired_loan_amount(Square *board, Player player);
int risk_decide_loan_collateral(Square *board, Player player, int *eligible_properties, int eligible_count, int *selected_collateral, int desired_amount);
int risk_decide_loan_extention(Player player);
int risk_decide_loan_refinance(Square *board, Player player);
int risk_decide_construction(Square property, Player player);
int risk_decide_mortgage_for_construction(Square *board, Player player, int target_cost);
InsuranceType risk_decide_insurance(Square property, Economy economy, Player player);
int risk_decide_bid(Square square, Player player, int bidding_price);

#endif
