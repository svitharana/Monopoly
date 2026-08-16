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

int remove_percentage(int base, int percentage)
{
    return (base * 100) / percentage;
}
