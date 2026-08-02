#include "include/board.h"

void initialize_board(Square *board)
{
    board[0].square_name = "GO";
    board[0].square_type = SPECIAL;
    board[0].property_group = NONE;

    board[1].square_name = "Pettah";
    board[1].square_type = PROPERTY;
    board[1].property_group = BROWN;

    board[2].square_name = "Community Development Fund";
    board[2].square_type = EVENT;
    board[2].property_group = NONE;

    board[3].square_name = "Maradana";
    board[3].square_type = PROPERTY;
    board[3].property_group = BROWN;

    board[4].square_name = "Income Tax";
    board[4].square_type = TAX;
    board[4].property_group = NONE;

    board[5].square_name = "Colombo Fort Railway Station";
    board[5].square_type = RAILWAY;
    board[5].property_group = NONE;

    board[6].square_name = "Bambalapitiya";
    board[6].square_type = PROPERTY;
    board[6].property_group = LIGHT_BLUE;

    board[7].square_name = "National Event Card";
    board[7].square_type = EVENT;
    board[7].property_group = NONE;

    board[8].square_name = "Wellawatte";
    board[8].square_type = PROPERTY;
    board[8].property_group = LIGHT_BLUE;

    board[9].square_name = "Mount Lavinia";
    board[9].square_type = PROPERTY;
    board[9].property_group = LIGHT_BLUE;

    board[10].square_name = "Jail / Just Visiting";
    board[10].square_type = SPECIAL;
    board[10].property_group = NONE;

    board[11].square_name = "Nugegoda";
    board[11].square_type = PROPERTY;
    board[11].property_group = PINK;

    board[12].square_name = "Ceylon Electricity Board";
    board[12].square_type = UTILITY;
    board[12].property_group = NONE;

    board[13].square_name = "Maharagama";
    board[13].square_type = PROPERTY;
    board[13].property_group = PINK;

    board[14].square_name = "Kottawa";
    board[14].square_type = PROPERTY;
    board[14].property_group = PINK;

    board[15].square_name = "Kandy Railway Station";
    board[15].square_type = RAILWAY;
    board[15].property_group = NONE;

    board[16].square_name = "Negombo";
    board[16].square_type = PROPERTY;
    board[16].property_group = ORANGE;

    board[17].square_name = "Sri Lanka Insurance";
    board[17].square_type = INSURANCE;
    board[17].property_group = NONE;

    board[18].square_name = "Katunayake";
    board[18].square_type = PROPERTY;
    board[18].property_group = ORANGE;

    board[19].square_name = "Ja-Ela";
    board[19].square_type = PROPERTY;
    board[19].property_group = ORANGE;

    board[20].square_name = "Free Parking";
    board[20].square_type = SPECIAL;
    board[20].property_group = NONE;

    board[21].square_name = "Kandy City";
    board[21].square_type = PROPERTY;
    board[21].property_group = RED;

    board[22].square_name = "National Event Card";
    board[22].square_type = EVENT;
    board[22].property_group = NONE;

    board[23].square_name = "Peradeniya";
    board[23].square_type = PROPERTY;
    board[23].property_group = RED;

    board[24].square_name = "Katugastota";
    board[24].square_type = PROPERTY;
    board[24].property_group = RED;

    board[25].square_name = "Galle Railway Station";
    board[25].square_type = RAILWAY;
    board[25].property_group = NONE;

    board[26].square_name = "Galle Fort";
    board[26].square_type = PROPERTY;
    board[26].property_group = YELLOW;

    board[27].square_name = "Unawatuna";
    board[27].square_type = PROPERTY;
    board[27].property_group = YELLOW;

    board[28].square_name = "National Water Supply and Drainage Board";
    board[28].square_type = UTILITY;
    board[28].property_group = NONE;

    board[29].square_name = "Hikkaduwa";
    board[29].square_type = PROPERTY;
    board[29].property_group = YELLOW;

    board[30].square_name = "Go To Jail";
    board[30].square_type = SPECIAL;
    board[30].property_group = NONE;

    board[31].square_name = "Jaffna Town";
    board[31].square_type = PROPERTY;
    board[31].property_group = GREEN;

    board[32].square_name = "Nallur";
    board[32].square_type = PROPERTY;
    board[32].property_group = GREEN;

    board[33].square_name = "Ceylinco Insurance";
    board[33].square_type = INSURANCE;
    board[33].property_group = NONE;

    board[34].square_name = "Trincomalee";
    board[34].square_type = PROPERTY;
    board[34].property_group = GREEN;

    board[35].square_name = "Jaffna Railway Station";
    board[35].square_type = RAILWAY;
    board[35].property_group = NONE;

    board[36].square_name = "National Event Card";
    board[36].square_type = EVENT;
    board[36].property_group = NONE;

    board[37].square_name = "Nuwara Eliya";
    board[37].square_type = PROPERTY;
    board[37].property_group = DARK_BLUE;

    board[38].square_name = "Bank of Ceylon";
    board[38].square_type = BANK;
    board[38].property_group = NONE;

    board[39].square_name = "Galle Face";
    board[39].square_type = PROPERTY;
    board[39].property_group = DARK_BLUE;

    for (int i = 0; i < MAX_SQUARES; i++)
    {
        board[i].ownership = UNOWNED;

        for (int j = 0; j < MAX_HOUSES; j++)
        {
            board[i].houses[j].condition = 1; // intialized condition of all houses to 100%
        }

        board[i].hotel.condition = 1; // intialized condition of hotel to 100%
    }
}