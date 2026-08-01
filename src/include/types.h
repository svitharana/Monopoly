#ifndef TYPES_H
#define TYPES_H

// constants
#define MAX_PLAYERS 4

// PLAYERS
typedef enum PlayerId
{
    PLAYER_1,
    PLAYER_2,
    PLAYER_3,
    PLAYER_4,
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
    char *name;
    PlayerStrategy strategy;

    int cash;
    int current_position;
    int rolled_value;

    int isInJail;
    int inJail_turns;

    int isBankrupt;

    int isOrderCorrect; // only for finding correct order

} Player;

// SQUARES

typedef enum SquareType
{
    PROPERTY,
    RAILWAY,
    UTILITY,
    BANK_SQUARE,
    INSURANCE,
    TAX,
    EVENT,
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

typedef enum BuildingType
{
    HOUSE_1,
    HOUSE_2,
    HOUSE_3,
    HOUSE_4,
    HOTEL,
} BuildingType;

typedef struct Buildings
{
    BuildingType building;
    float condition;

} Buildings;

typedef struct Square
{
    char *name;
    SquareType type;
    PlayerId ownership;

    int purchase_price;
    int base_rent;

    PropertyGroup group;
    int mortgage_value;
    int house_constructionCost;
    int hotel_constructionCost;
    int isMortgage;
    int isInsured;

    Buildings buildings;

} Square;

#endif