#ifndef BOARD_H
#define BOARD_H

#include "types.h"

void resolve_renovations(Square *board, Player *player);
void initialize_board(Square *board);
void move_player(Player *player, int move_by, Square *board);
void resolve_landingSquare(Square *board, Player *players, Player *player, Economy economy, NationalEventCards *cards);
int player_has_monopoly(Square *board, PlayerId playerId, PropertyGroup group);
int get_property_index_toBuild(Square *board, PropertyGroup group);
int get_max_opponent_rent(Square *board, Player player);

#endif