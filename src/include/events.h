#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"

int create_disaster(Square *board);
void initialize_national_event_cards(NationalEventCards *cards);
void update_inflation(Economy *economy);
void update_board_data(Economy *economy, Square *board);
void update_dynamic_property_market(Square *board, Economy *economy);
void remove_economic_event(Square *board, Economy *economy, EconmicEvents event);
void remove_government_regulation(Square *board, Economy *economy, GovernmentRegulations regulation);
void run_government_regulations(Square *board, Economy *economy, Player *players);
void run_economic_event(Square *board, Economy *economy);
void draw_regional_development_card(Square *board, Economy *economy );
void remove_regional_development_card_effect(Square *board, RegionalDevelopmentCards card);
NationalEventCards draw_national_event_card(NationalEventCards *cards, Player *player);
void apply_national_event_effect(Square *board, Economy *economy, NationalEventCards card, Player *player, Player *players);

#endif