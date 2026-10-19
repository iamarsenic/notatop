#pragma once

// so, this structure gives load average variables, is new minor update
typedef struct {
    double min_1;	// 1 minute
    double min_5;	// 5 minute
    double min_15;	// 15 minute
    bool success;	// and self-check
} LoadAvg;

LoadAvg get_system_load(void);
