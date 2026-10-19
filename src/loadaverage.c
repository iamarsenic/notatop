#define _DEFAULT_SOURCE // for GNU/BSD extensions

#include <stdlib.h> // for getloadavg

#include "../include/loadaverage.h" // header

LoadAvg get_system_load(void) {
    double la_array[3] = {}; 

    if (getloadavg(la_array, 3) == -1) {
        return (LoadAvg){ .success = false }; // self check false
    }

    return (LoadAvg){
        .min_1 = la_array[0],
        .min_5 = la_array[1],
        .min_15 = la_array[2],
        .success = true
    };
}
