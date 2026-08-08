# MONOPOLY-LK Progress

## Square (types.h)

- [x] index
- [x] name
- [x] type
- [x] group
- [x] ownership (UNOWNED sentinel)
- [x] purchase_price
- [x] base_rent
- [x] house_cost
- [x] hotel_cost
- [x] mortage_value
- [ ] is_mortgaged
- [ ] is_loan_locked
- [x] num_houses
- [x] has_hotel
- [ ] age_in_rounds
- [ ] depreciation_percent
- [ ] condition_percent

## Player (types.h)

- [x] name
- [x] id
- [x] strategy
- [x] cash (30000)
- [x] position
- [x] in_jail / jail_turns_remaining
- [x] is_bankrupt
- [ ] has_active_loan
- [ ] loan_amount
- [ ] loan_interest_rate
- [ ] loan_rounds_remaining

## Board setup

- [ ] board_layout.csv
- [x] load_board_layout()
- [x] railway/utility purchase price + mortgage value (blocked — ask instructor)

## Turn flow (game.c)

- [x] determine_playerOrder()
- [x] roll_dice() / move_player()
- [x] passing/landing GO
- [x] resolve_square() dispatcher
- [x] construction phase (separate from landing)
- [ ] financial transactions phase
- [x] end turn / round loop
- [ ] win condition check

## board.c

- [x] resolve_square()
- [x] resolve_property()
- [x] resolve_railway()
- [x] resolve_utility()
- [ ] resolve_bank_square()
- [ ] resolve_insurance_square()
- [ ] resolve_tax_square()
- [ ] resolve_event_square()
- [x] resolve_special_square()
- [ ] count_owned_by_type()
- [x] player_has_monopoly()
- [ ] get_eligible_collateral()

## players.c

- [x] decide_purchase() (placeholder version)
- [ ] decide_purchase() strategy dispatch (4 strategies)
- [x] decide_bid() (placeholder version)
- [ ] decide_bid() strategy dispatch (4 strategies)
- [ ] decide_loan_action()
- [ ] decide_loan_collateral()
- [ ] decide_insurance_action()
- [ ] decide_construction()

## finance.c

- [x] execute_purchase()
- [x] pay_rent()
- [ ] run_auction()
- [ ] calculate_max_loan()
- [ ] issue_loan()
- [ ] repay_loan()

## utils.c

- [x] random_int()
- [ ] apply_percentage()
- [ ] format_lkr()

## Open questions for instructor

- [ ] base_rent value missing (Appendix B)
- [ ] railway/utility purchase price + mortgage value missing
- [ ] insurance peril list vs disaster list mismatch
- [ ] can a mortgaged property be loan collateral
- [ ] construction: landing-triggered or independent turn phase
- [ ] regional development card: random or cycled deck
