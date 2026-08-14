#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"

int create_disaster(Square *board);
void update_inflation(Economy *economy);
void update_board_data(Economy *economy, Square *board);
void update_dynamic_property_market(Square *board, Economy *economy);

#endif