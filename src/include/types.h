#ifndef TYPES_H
#define TYPES_H

// constants
#define MAX_PLAYERS 4
#define MAX_SQUARES 40
#define MAX_HOUSES 4
#define MAX_ROUNDS 500

#define MAX_PROPERTY_GRPS 8
#define MAX_PROPERTIES_IN_GRPS 3

#define INITIAL_CASH 30000
#define GO_PASSED_AMOUNT 2000

#define MAX_TURNS_INJAIL 3
#define GOTO_JAIL_SQUARE 30
#define JAIL_SQUARE 10

// PLAYERS
typedef enum PlayerId
{
    PLAYER_1,
    PLAYER_2,
    PLAYER_3,
    PLAYER_4,
    UNOWNED = -1 // intial ownership of squares
} PlayerId;

typedef enum PlayerStrategy
{
    AGGRESSIVE_INVESTOR,
    CONSERVATIVE_BANKER,
    RISK_TAKER,
    OPPORTUNISTIC_TRADER,
} PlayerStrategy;

typedef struct Player
{
    PlayerId playerId;
    char *player_name;
    PlayerStrategy strategy;

    int cash;
    int current_position;
    int rolled_value;

    int isInJail;
    int inJail_turns;

    int isBankrupt;

    int player_round;

} Player;

typedef struct PlayerOrder
{
    Player *player;
    int rolled_value;
    int isOrderCorrect;

} PlayerOrder;

// SQUARES

typedef enum SquareType
{
    PROPERTY,
    RAILWAY,
    UTILITY,
    BANK,
    INSURANCE,
    TAX,
    EVENT,
    JAIL,
    SPECIAL
} SquareType;

typedef enum
{
    BROWN,
    LIGHT_BLUE,
    PINK,
    ORANGE,
    RED,
    YELLOW,
    GREEN,
    DARK_BLUE,
    NONE
} PropertyGroup;

typedef struct Square
{
    char *square_name;
    int property_index;
    SquareType square_type;
    PlayerId ownership;

    int purchase_price;
    int base_rent;

    PropertyGroup property_group;
    int mortgage_value;
    int house_constructionCost;
    int hotel_constructionCost;
    int house_count;
    int hasHotel;
    int builing_condition;
    int isMonopoly;

    int isMortgage;
    int isInsured;

} Square;

#endif