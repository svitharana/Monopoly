#ifndef BOARD_H
#define BOARD_H

#include "types.h"

void initialize_board(Square *board);
void move_player(Player *player);
void resolve_landingSquare(Square *square, Player *players, Player *player);

void resolve_property(Square *square, Player *players, Player *player);

#endif