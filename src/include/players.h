#ifndef PLAYERS_H
#define PLAYERS_H

#include "types.h"

void initialize_players(Player *players, PlayerOrder *playerOrder);

int decide_purchase(Square *board, Square *square, Player *player);
int decide_construction(Square property, Player player);
int decide_bid(Square square, Player player, int bidding_price);

int decide_pay_bail(Player *player);
int decide_loan(Square *board, Player player);
int decide_loan_extention(Player player);
int decide_loan_refinance(Square *board, Player player);
int decide_loan_repayment(Player player, int *amount);

#endif