#ifndef FINANCE_H
#define FINANCE_H

#include "types.h"

void execute_purchase(Square *square, Player *player);
void pay_rent(Square *square, Player *player, Player *owner, int rent);
void execute_construction(Square *property, Player *player);

#endif