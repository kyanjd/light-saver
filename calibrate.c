#include <xc.h>
#include "calibrate.h"
#include "clock.h"

void calibrate_clock(unsigned int event, struct Clock *c, unsigned int *sunrise, unsigned int *sunset, struct calibrate_clock_params *p)
{
    unsigned int record_sunrise[5] = {480, 480, 480, 480, 480}; // store past 5 sunrises for averaging
    unsigned int record_sunset[5] = {984, 984, 984, 984, 984}; // store past 5 sunsets for averaging
    // p->sunrise_index = array indexer for appending sunrises
    // p->sunset_index = array indexer for appending sunsets
    
    if (event != 0) {
        if (event == 1) { // append sunrise time in mins
            record_sunrise[p->sunrise_index] = c->hrs * 60 + c->mins;
            p->sunrise_index = (p->sunrise_index + 1) % 5;
        }
        else if (event == 2) { // append sunset time in mins
            record_sunset[p->sunset_index] = c->hrs * 60 + c->mins;
            p->sunset_index = (p->sunset_index + 1) % 5;
        }
        if (p->sunrise_index == 4) { // calibrate every 5 combined sunrises
            unsigned int sunrise_avg = 0;
            unsigned int sunset_avg = 0;
            for (unsigned int i = 0; i < 5; i++) { // sum all sunrise and sunset times in the arrays
                sunrise_avg += record_sunrise[i];
                sunset_avg += record_sunset[i];
            }
            sunrise_avg /= 5; // average the sunrise and sunset sums in mins
            sunset_avg /= 5;
            unsigned int solar_midday = (sunrise_avg + sunset_avg) / 2; // calculate the solar midday in minutes from midnight

            unsigned int ideal_solar_midday = ((sunrise[c->mons - 1] * 60 + sunrise[c->mons + 11]) + (sunset[c->mons - 1] * 60 + sunset[c->mons + 11])) / 2; // calculate the ideal solar midday from the month's dawn and dusk

            int diff = (int)(solar_midday) - (int)(ideal_solar_midday); 
            int calibrated_hrs = diff / 60; // take only the hrs part of the difference
            int calibrated_mins = diff % 60; // take only the mins part of the difference

            // handle negative mins/hrs
            if (calibrated_mins < 0) {
                calibrated_mins += 60;
                calibrated_hrs--;
            }
            
            if (calibrated_hrs < 0) {
                calibrated_hrs += 24;
                c->days --;
            }
            c->hrs += calibrated_hrs; // calibrate clock
            c->mins += calibrated_mins; 
        }
    }
}