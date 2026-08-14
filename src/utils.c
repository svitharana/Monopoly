#include <stdio.h>
#include <stdlib.h>
#include "include/utils.h"

int random_generator(int min, int max)
{
    int value = min + (rand() % (max - min + 1));
    return value;
}

void print_heading(char *heading)
{
    int decoration_len = 40;
    for (int x = 0; x < decoration_len; x++)
    {
        printf("=");
    }

    printf("\n\t%s\n", heading);

    for (int x = 0; x < decoration_len; x++)
    {
        printf("=");
    }

    printf("\n\n");
}

int apply_percentage(int base, int percentage)
{
    return (base * percentage) / 100;
}

void format_amount(int amount) // formats 30000 -> 30,000
{
    int arr[5];
    int size = sizeof(arr) / sizeof(arr[0]);

    for (int i = size - 1; i >= 0; i--)
    {
        arr[i] = -1;

        if (amount != 0)
        {
            arr[i] = amount % 1000;
            amount /= 1000;
        }
    }
    int isFirstIndex = 1;

    for (int i = 0; i < size; i++)
    {
        if (arr[i] != -1)
        {
            if (!isFirstIndex)
            {
                printf(",");
            }

            if (isFirstIndex)
            {
                printf("LKR %d", arr[i]);
                isFirstIndex = 0;
            }
            else
            {
                printf("%03d", arr[i]);
            }
        }
    }
    printf(".\n\n");
}