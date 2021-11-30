#include <stdio.h>

void handleInput(char *input, char *arr)
{
    if (input[0] == '0')
    {
        sprintf(arr, "%s", "\nWelcome to NormDB\n\tPress 1 for Insert Table\n\tPress 2 for Drop Table\n\tPress 3 for Insert\n\tPress 4 for Update\n\tPress 5 for Delete\n\tPress 6 to Show\n");
    }
    else if (input[1] == '1')
    {
        sprintf(arr, "%s", "\nTable Added\n");
    }
    else if (input[1] == '2')
    {
        sprintf(arr, "%s", "\nTable Dropped\n");
    }
    else if (input[1] == '3')
    {
        sprintf(arr, "%s", "\nRow Added\n");
    }
    else
    {
        sprintf(arr, "%s", "\nID\t Name\t Email\tPhone No.\n1\tabcd\tabcd@email.com\t1234567890\n2\taafd\ta34cd@email.com\t223344567890\n3\tabad\tasacd@email.com\t123453290\n");
    }
}