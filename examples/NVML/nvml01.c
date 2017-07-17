#include <stdio.h>
#include <time.h>
#include "nvml.h"


int main(void) {
	nvmlReturn_t result;
	unsigned int device_count, i, j;
	// elapsed time
	time_t start,end;
	double dif;

	// 1- Initialize NVML library
	printf("------------------------------------------------------------\n");
	printf("[-] Initialize NVML library ...\n");
	time(&start);

	result = nvmlInit();
	if (NVML_SUCCESS != result) {
		printf("Failed to initialize NVML: %s\n", nvmlErrorString(result));
		printf("Press ENTER to continue...\n");
		getchar();
		return 1;
	}

	time(&end);
	dif = difftime(end,start);
	printf("> NVML initialization > elapsed time: %.2lf seconds.\n", dif );

	// 2- Get total nvidia devices
	printf("[-] nvmlDeviceGetCount\n");
	result = nvmlDeviceGetCount(&device_count);
	if (NVML_SUCCESS != result) {
		printf("Failed to query device count: %s\n", nvmlErrorString(result));
		printf("Press ENTER to continue...\n");
		getchar();
		goto Error;
	}
	printf("> Found %d device%s\n", device_count, device_count != 1 ? "s" : "");

	// 3- List found devices
	for (i = 0; i < device_count; i++) {
		nvmlDevice_t device;
		char name[NVML_DEVICE_NAME_BUFFER_SIZE];
		nvmlPciInfo_t pci;
		nvmlComputeMode_t compute_mode;
		float totalWatts;
		unsigned int *power;
		unsigned ipower = 1;
		power = &ipower;
		nvmlUtilization_t nvmlUtilization = { 0 };
		unsigned int num_procs = 32;
		nvmlProcessInfo_t procs[32];

		// Query for device handle to perform operations on a device
		// You can also query device handle by other features like:
		// nvmlDeviceGetHandleBySerial
		// nvmlDeviceGetHandleByPciBusId
		result = nvmlDeviceGetHandleByIndex(i, &device);
		if (NVML_SUCCESS != result) {
			printf("Failed to get handle for device %i: %s\n", i, nvmlErrorString(result));
			goto Error;
		}
		
		result = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
		if (NVML_SUCCESS != result) {
			printf("Failed to get name of device %i: %s\n", i, nvmlErrorString(result));
			goto Error;
		}

		// pci.busId is very useful to know which device physically you're talking to
		// Using PCI identifier you can also match nvmlDevice handle to CUDA device.
		result = nvmlDeviceGetPciInfo(device, &pci);
		if (NVML_SUCCESS != result) {
			printf("Failed to get pci info for device %i: %s\n", i, nvmlErrorString(result));
			goto Error;
		}
		printf("> [%d]. device: [%s], pci.busId: [%s]\n", i, name, pci.busId);
		
		/***************************************************************************************/
		/* RUNNING PROCESSES */
		// nvmlReturn_t nvmlDeviceGetComputeRunningProcesses (nvmlDevice_t device, unsigned int *infoCount, nvmlProcessInfo_t *infos)
		/*
			This function returns information only about compute running processes (e.g. CUDA
			application which have active context). Any graphics applications (e.g. using OpenGL,
			DirectX) won't be listed by this function.
		*/
		result = (nvmlReturn_t) nvmlDeviceGetComputeRunningProcesses(device, &num_procs, procs);
		if (NVML_SUCCESS != result) {
			printf("ERROR: %s\n", nvmlErrorString(result));
			goto Error;
		} else {
			printf(">>> Processes [1] %i \n", num_procs);
		}
		
		// nvmlReturn_t nvmlDeviceGetGraphicsRunningProcesses (nvmlDevice_t device, unsigned int *infoCount, nvmlProcessInfo_t *infos)
		/*
			This function returns information only about graphics based processes (eg. applications
			using OpenGL, DirectX)
		*/
		/*result = (nvmlReturn_t) nvmlDeviceGetGraphicsRunningProcesses(device, &num_procs, procs);
		if (NVML_SUCCESS != result) {
			printf("ERROR: %s\n", nvmlErrorString(result));
			goto Error;
		} else {
			printf(">>> Processes [2] %i \n", num_procs);
		}*/
		
		/***************************************************************************************/
		/* UTILIZATION & MEMORY */
		result = (nvmlReturn_t) nvmlDeviceGetUtilizationRates(device, &nvmlUtilization);
		if (NVML_SUCCESS != result) {
			printf("ERROR: %s\n", nvmlErrorString(result));
			goto Error;
		} else {
			printf(">>> Utilization: gpu=%u, gmem=%u \n", nvmlUtilization.gpu, nvmlUtilization.memory);
		}
		
		/***************************************************************************************/
		/* POWER */
		/*
		 * nvmlReturn_t DECLDIR nvmlDeviceGetPowerUsage (nvmlDevice_t device, unsigned int* power)
		 *		URL: http://developer.download.nvidia.com/assets/cuda/files/CUDADownloads/NVML/nvml.pdf
		 *
		 * Retrieves the power usage reading for the device, in milliwatts. This is the power draw for the entire board, including
		 * GPU, memory, etc.
		 *
		 * For "GF11x" Tesla and Quadro products from the Fermi family:
		 * 		- Requires NVML_INFOROM_POWER version 3.0 or higher.
		 * For Tesla and Quadro products from the Kepler family:
		 * 		- Does not require NVML_INFOROM_POWER object.
		 *
		 * The reading is accurate to within a range of +/- 5 watts. It is only available if power management mode is supported
		 *
		 */
		result = (nvmlReturn_t) nvmlDeviceGetPowerUsage(device, power);
		if (NVML_SUCCESS != result) {
			printf("ERROR: %s\n", nvmlErrorString(result));
			goto Error;
		} else {
			totalWatts = *power / 1000.0;
			printf(">>> power=%f Watts\n", totalWatts);
		}
	}

	//
	printf("[-] NVML shutdown ...\n");

	result = nvmlShutdown();
	if (NVML_SUCCESS != result)
		printf("Failed to shutdown NVML: %s\n", nvmlErrorString(result));

	printf("------------------------------------------------------------\n");

	return 0;

Error:
	result = nvmlShutdown();
	if (NVML_SUCCESS != result)
		printf("Failed to shutdown NVML: %s\n", nvmlErrorString(result));

	printf("Press ENTER to continue...\n");
	getchar();

	printf("------------------------------------------------------------\n");
	return 0;
}
