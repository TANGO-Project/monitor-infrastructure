#include <stdio.h>
#include <time.h>
#include "nvml.h"


// FUNCTIONS //////////////////////////////////////////////////////////////////
/*
 * Function: Initializes NVML lib
 */
int nvidiaInit(nvmlReturn_t*);

/*
 * Function: Shutdown NVML lib
 */
int nvidiaStop(nvmlReturn_t*);
