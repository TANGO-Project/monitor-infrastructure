#include <stdio.h>
#include <time.h>
#include "nvml.h"


/*
 * Function: Initializes NVML lib
 */
int nvidiaInit(nvmlReturn_t* result) {
	printf("[-] Initializing 'NVIDIA' lib ...\n");

	*result = nvmlInit();
	if (NVML_SUCCESS != *result) {
		printf("> Failed to initialize 'NVIDIA' lib: %s\n", nvmlErrorString(*result));
		return -1;
	}
	else {
		printf("> 'NVIDIA' lib initialized \n");
	}

	return 1;
}


/*
 * Function: Shutdown NVML lib
 */
int nvidiaStop(nvmlReturn_t* result) {
	printf("[-] Stopping 'NVIDIA' lib ...\n");

	*result = nvmlShutdown();
	if (NVML_SUCCESS != *result) {
		printf("> Failed to shutdown NVML: %s\n", nvmlErrorString(*result));
		return -1;
	}
	else {
		printf("> 'NVIDIA' lib stopped \n");
	}

	return 1;
}
