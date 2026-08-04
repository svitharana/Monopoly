#ifndef BOARD_H
#define BOARD_H

#include "types.h"

void initialize_board(Square *board);
void move_player(Player *player, int move_by);
void resolve_landingSquare(Square *board, Player *players, Player *player);

#endif