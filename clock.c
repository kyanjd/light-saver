#include <xc.h>
#include "clock.h"
#include "interrupts.h"

unsigned int list_check(unsigned int list[], unsigned int len, unsigned int item)
{
    // function to check if an item is in a list
    for (int i = 0; i < len; i ++) {
        if (list[i] == item) {
            return 1; // item is in list
        }
    }
    return 0; // item isn't in list
}

void clock_update(struct Clock *c)
{   
    // function to keep a running clock, accounting for month lengths, leap years and daylight savings
    unsigned int long_months[7] = {1, 3, 5, 7, 8, 10, 12}; // 31 days
    unsigned int short_months[4] = {4, 6, 9, 11}; // 30 days
    
//    c->hrs += count; // TESTING MODE
    c->secs += count; // REAL MODE
    count = 0;
    static int check = 0;
    
    if (c->secs >= 60) { // seconds to minutes
        c->mins ++;
        c->secs = 0;
    }
    
    if (c->mins >= 60) { // minutes to hours
        c->hrs ++;
        c->mins = 0;
    }
    
    if (c->hrs >= 24) { // hours to days
        c->days ++;
        c->hrs = 0;
        c->dotw = (c->dotw + 1) % 7; // reset day of the week at day 7 (= day 0 = Monday)
    }
    
    if (c->mons == 3 && c->hrs == 2 && c->dotw == 6 && c->days + 7 > 31) { // daylight savings for final Sunday in March
        check = 0;
        c->hrs ++;
    }
    
    if (c->mons == 10 && c->hrs == 3 && c->dotw == 6 && c->days + 7 > 31 && check == 0) { // daylight savings for final Sunday in October
        check = 1;
        c->hrs --;
    }    
    
    if (list_check(long_months, 7, c->mons) && c->days > 31) { // account for 31 day months
        c->mons ++;
        c->days = 1;
    }
    else if (list_check(short_months, 4, c->mons) && c->days > 30) { // account for 30 day months
        c->mons ++;
        c->days = 1;
    }
    else if (c->mons == 2) { // February logic
        if ((c->yrs % 400 == 0) || ((c->yrs % 4 == 0 ) && (c->yrs % 100 != 0))) { // account for 29 days in a leap year
            if (c->days > 29) {
                c->mons ++;
                c->days = 1; 
            }
        }
        else if (c->days > 28) { // account for 28 days in a non-leap year
            c->mons ++;
            c->days = 1;  
        } // account for Feb in a leap year                   
    }
        
    if (c->mons > 12) { // months to years
        c->yrs ++;
        c->mons = 1;
    }
}

