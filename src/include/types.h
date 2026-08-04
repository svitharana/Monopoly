#ifndef TYPES_H
#define TYPES_H

// constants
#define MAX_PLAYERS 4
#define MAX_SQUARES 40
#define MAX_HOUSES 4
#define MAX_ROUNDS 6

#define INITIAL_CASH 30000
#define GO_PASSED_AMOUNT 2000

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

typedef struct House
{
    int hasBuild;
    float condition;

} House;

typedef struct Hotel
{
    int hasBuild;
    float condition;
} Hotel;

typedef struct Square
{
    char *square_name;
    SquareType square_type;
    PlayerId ownership;

    int purchase_price;
    int base_rent;

    PropertyGroup property_group;
    int mortgage_value;
    int house_constructionCost;
    int hotel_constructionCost;
    int isMortgage;
    int isInsured;

    int house_count;
    int hotel_count;
    int builing_condition;

} Square;

#endif