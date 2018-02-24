/*******************************************************************************
 * CMPT 434
 * Assignment 2
 * 
 * Lizzie Adams
 * ema651
 * 11139300
 ******************************************************************************/

#include <strconvert.h>

/*******************************************************************************
 * strtoint:
 * 
 * Converts the given string to a non-negative integer. If the string can
 * successfully be converted to an integer, the result of the conversion is
 * stored at the location pointer to by the res argument and 0 is returned. If
 * the string cannot be converted to an integer, the original value at res is
 * restored and 1 is returned.
 ******************************************************************************/
int strtoint(const char *str, int *res) {
    int prev;
    int i;
    int n;
    int pv=1;
    
    /* Store value of result before clearing it */
    prev = *res;
    *res = 0;
    
    /* Iterate over string, starting with rightmost character */
    for (i = strlen(str) - 1; i >= 0; i--) {
        /* If character is not a digit, exit on error */
        if (isdigit(str[i]) == 0) {
            *res = prev;
            return 1;
        }
        
        /* Get digit */
        n = str[i] - '0';
        
        /* Add digit to result and increment place value */
        *res += n * pv;
        pv *= 10;
    }
    
    return 0;
}
