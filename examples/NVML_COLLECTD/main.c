#include "collectd/collectd_client.h"
#include "nvidia/gpu_nvidia.h"



int main(void) {
	// socket / collectd
	int s, res;
	struct sockaddr_un remote;
	char* str;
	char command1[1500] = "";
	char command2[1500] = "";
	char prefix[400] = "PUTVAL nvidia/gpu-";
	char sufix1[400] = "/watts N:";
	char sufix2[400] = "/processes N:";
	// nvidia
	int i, j;
	nvmlReturn_t result;
	unsigned int nvidia_devices_count;

	printf("------------------------------------------------------------\n");

	// 1. Initialize elements ...
	// Start nvidia
	if (nvidiaInit(&result) == -1) {
		goto Error;
	}

	// Create unix socket
	if ((s = createUNIXSocket()) == -1) {
		goto Error;
	}

	// Connect with collectd
	if ((res = connectToCollectd(s, remote)) == -1) {
		//goto Error;
	}


	// 2. Get total NVIDIA GPUs
	result = nvmlDeviceGetCount(&nvidia_devices_count);
	if (NVML_SUCCESS != result) {
		printf("> Failed to query device count: %s\n", nvmlErrorString(result));
		goto Error;
	}
	printf("[-] Found %d NVIDIA device%s\n", nvidia_devices_count, nvidia_devices_count != 1 ? "s" : "");

	// 3. Get energy consumption values from GPUs and send them to Collectd ...
	for (j = 0; j < 10; j++) {

		for (i = 0; i < nvidia_devices_count; i++) {
			nvmlDevice_t device;
			char name[NVML_DEVICE_NAME_BUFFER_SIZE];
			nvmlPciInfo_t pci;
			nvmlProcessInfo_t infos;
			float totalWatts;
			unsigned int infoCount;
			unsigned int *power;
			unsigned ipower = 1;
			power = &ipower;

			result = nvmlDeviceGetHandleByIndex(i, &device);
			if (NVML_SUCCESS != result) {
				printf("Failed to get handle for device %i: %s\n", i, nvmlErrorString(result));
				goto Error;
			}

			// name
			result = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
			if (NVML_SUCCESS != result) {
				printf("Failed to get name of device %i: %s\n", i, nvmlErrorString(result));
				goto Error;
			}
			// pci
			result = nvmlDeviceGetPciInfo(device, &pci);
			if (NVML_SUCCESS != result) {
				printf("Failed to get pci info for device %i: %s\n", i, nvmlErrorString(result));
				goto Error;
			}
			// processes running
			result = nvmlDeviceGetComputeRunningProcesses(device, &infoCount, &infos);
			if (NVML_SUCCESS != result) {
				printf("Failed to get running processes for device %i: %s\n", i, nvmlErrorString(result));
				goto Error;
			}
			// power usage
			result = (nvmlReturn_t)nvmlDeviceGetPowerUsage(device, power);
			if (NVML_SUCCESS != result) {
				printf("ERROR: %s\n", nvmlErrorString(result));
				goto Error;
			}
			else {
				totalWatts = *power / 1000.0;
				printf("> [%d]. device: [%s], pci.busId: [%s], power: %f Watts\n", i, name, pci.busId, totalWatts);

				// TODO
				// send to collectd
				printf("> Sending data to Collectd ... [not implemented], ");

				// This part needs to be updated and tested !!!
				char strwatts[50];
				snprintf(strwatts, 50, "%f", totalWatts);

				char str_i_device[15];
				sprintf(str_i_device, "%d", i);

				char strprocesses[15];
				sprintf(strprocesses, "%d", infoCount);

				sprintf(command1,"%s%s%s %s", prefix, str_i_device, sufix1, strwatts);
				sendFakeCommand(s, command1);

				sprintf(command2,"%s%s%s %s", prefix, str_i_device, sufix2, strprocesses);
				sendFakeCommand(s, command2);
			}
		}

		// wait 2 seconds
		sleep(2);
	}

	// 4. Stop all ...
	// Shutdown nvidia
	nvidiaStop(&result);

	// Close socket
	closeUNIXSocket(s);

	printf("------------------------------------------------------------\n");

	return 0;


Error:
	// Shutdown nvidia
	nvidiaStop(&result);

	// Close socket
	closeUNIXSocket(s);

	printf("ERROR: Press ENTER to continue...\n");
	getchar();
	printf("------------------------------------------------------------\n");

	return -1;
}
