#ifndef PLAYERS_H
#define PLAYERS_H

#include "types.h"

void initialize_players(Player *players, PlayerOrder *playerOrder);

int decide_purchase(Square *square, Player *player);
int decide_construction(Square property, Player player);

#endif