#include <stdio.h>
#include <ctype.h>

static int to_int(const char *str)
{
    int n, neg;

    n = neg = 0;

    while (isspace(*str))
        str++;

    switch (*str) {
    case '-':
        neg = 1;
        /* Fall through */
    
    case '+':
        str++;
        /* Fall through */
    }

    while (isdigit(*str))
        n = 10 * n - (*str++ - '0');

    fprintf(stdout, "neg: %d n: %d\n", neg, n);
    return neg ? n : -n; 
}

int main()
{
    fprintf(stdout, "%d", to_int("-0"));
}
