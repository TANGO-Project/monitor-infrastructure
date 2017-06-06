/*
 * Collectd NVIDIA plugin
 *
 * Copyright: Roi Sucasas Font, Atos Research and Innovation, 2017.
 *
 * This code is being developed for the TANGO Project: http://tango-project.euThis
 * and is licensed under an Apache 2.0 license. Please, refer to the LICENSE.TXT file for more information
 *
 * This plugin has been created taken as a basis the plugin template for collectd written by Sebastian Harl <sh@tokkee.org>
 * https://github.com/collectd/collectd/tree/master/contrib/examples/myplugin.c
 *
 */

/*
 * Notes:
 * - plugins are executed in parallel, thus, thread-safe functions need to be used
 * - each of the functions below (except module_register) is optional
 */

#if ! HAVE_CONFIG_H

#include <stdlib.h>
#include <string.h>

#ifndef __USE_ISOC99 /* required for NAN */
# define DISABLE_ISOC99 1
# define __USE_ISOC99 1
#endif /* !defined(__USE_ISOC99) */

#include <math.h>

#if DISABLE_ISOC99
# undef DISABLE_ISOC99
# undef __USE_ISOC99
#endif /* DISABLE_ISOC99 */

#include <time.h>

#endif /* ! HAVE_CONFIG */

#include "collectd.h"
#include "common.h"
#include "plugin.h"

#include <stdio.h>

#include "nvml.h"


/*
 * https://collectd.org/wiki/index.php/Data_source
 *
 * data source definition:
 * - name of the data source
 * - type of the data source (DS_TYPE_GAUGE, DS_TYPE_COUNTER)
 * - minimum allowed value
 * - maximum allowed value
 */
static data_source_t dsrc[1] =
{
	{ "watts", DS_TYPE_GAUGE, 0, NAN }
};

/*
 * https://collectd.org/wiki/index.php/Data_set
 *
 * data set definition:
 * - name of the data set
 * - number of data sources
 * - list of data sources
 *
 * NOTE: If you're defining a custom data-set, you have to make that known to any servers as well.
 * Else, the server is not able to store values using the type defined by that data-set.
 * It is strongly recommended to use one of the types and data-sets pre-defined in the types.db file.
 */
static data_set_t ds =
{
	"nvidia_plugin", STATIC_ARRAY_SIZE (dsrc), dsrc
};


/*
 * This function is called once upon startup to initialize the plugin.
 */
static int my_init(void) {
	nvmlReturn_t result;

	/* open sockets, initialize data structures, ... */
	result = nvmlInit();
	if (NVML_SUCCESS != result) {
		printf("Failed to initialize NVML: %s\n", nvmlErrorString(result));
		plugin_log(LOG_WARNING, "Failed to initialize NVML");
		return -1;
	}

	/* A return value != 0 indicates an error and causes the plugin to be disabled. */
    return 0;
}


/*
 * This is a utility function used by the read callback to populate a value_list_t and pass it to plugin_dispatch_values.
 */
static int submitPower(gauge_t value, unsigned int deviceIndex) {
	char strDeviceIndex[8];
	//char strPlugin[128] = "nvidia";
	/*
	 *  value_list_t: https://collectd.org/wiki/index.php/Value_list
	 *
	 *	The structure is defined in src/plugin.h as follows:
	 *	 struct value_list_s
	 *	 {
	 *	   value_t *values;
	 *	   int      values_len;
	 *	   cdtime_t time;
	 *	   cdtime_t interval;
	 *	   char     host[DATA_MAX_NAME_LEN];
	 *	   char     plugin[DATA_MAX_NAME_LEN];
	 *	   char     plugin_instance[DATA_MAX_NAME_LEN];
	 *	   char     type[DATA_MAX_NAME_LEN];
	 *	   char     type_instance[DATA_MAX_NAME_LEN];
	 *	   meta_data_t *meta;
	 *	 };
	 *	 typedef struct value_list_s value_list_t;
	 *
	 *
	 *  collectd-unixsock: 	Hostname/Plugin/Type
	 * 	Examples:
	 * 		myhost/cpu-0/cpu-user
	 * 		myhost/disk-sda/disk_octets
	 */
	value_list_t vl = VALUE_LIST_INIT;

	vl.values = &(value_t) { .gauge = value }; /* Convert the gauge_t to a value_t and add it to the value_list_t. */
	vl.values_len = 1;

	sprintf(strDeviceIndex, "%d", deviceIndex);

	sstrncpy(vl.host, hostname_g, sizeof (vl.host));
	sstrncpy(vl.plugin, "monitoring", sizeof (vl.plugin));
	sstrncpy(vl.plugin_instance, "nvidia", sizeof (vl.plugin_instance)); // <<<<<
	sstrncpy(vl.type, "power", sizeof (vl.type));
	sstrncpy(vl.type_instance, strDeviceIndex, sizeof (vl.type_instance));

	/* dispatch the values to collectd which passes them on to all registered write functions */
	return plugin_dispatch_values(&vl);
}


/*
 * This function is called in regular intervalls to collect the data.
 */
static int my_read (void) {
	nvmlReturn_t result;
	//gauge_t value;
	unsigned int device_count, i;

	/* do the magic to read the data : gauge_t value = random (); */
	result = nvmlDeviceGetCount(&device_count);
	if (NVML_SUCCESS != result) {
		printf("Failed to query device count: %s\n", nvmlErrorString(result));
		plugin_log(LOG_WARNING, "Failed to query device count");
		return 0;
	}

	for (i = 0; i < device_count; i++) {
		nvmlDevice_t device;
		char name[NVML_DEVICE_NAME_BUFFER_SIZE];
		nvmlPciInfo_t pci;
		float totalWatts;
		unsigned int *power;
		unsigned ipower = 1;
		power = &ipower;

		// Query for device handle to perform operations on a device
		// You can also query device handle by other features like:
		// nvmlDeviceGetHandleBySerial
		// nvmlDeviceGetHandleByPciBusId
		result = nvmlDeviceGetHandleByIndex(i, &device);
		if (NVML_SUCCESS != result) {
			printf("Failed to get handle for device %i: %s\n", i, nvmlErrorString(result));
			plugin_log(LOG_WARNING, "Failed to get handle for device");
			return 0;
		}

		result = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
		if (NVML_SUCCESS != result) {
			printf("Failed to get name of device %i: %s\n", i, nvmlErrorString(result));
			plugin_log(LOG_WARNING, "Failed to get name of device");
			return 0;
		}

		// pci.busId is very useful to know which device physically you're talking to
		// Using PCI identifier you can also match nvmlDevice handle to CUDA device.
		result = nvmlDeviceGetPciInfo(device, &pci);
		if (NVML_SUCCESS != result) {
			printf("Failed to get pci info for device %i: %s\n", i, nvmlErrorString(result));
			plugin_log(LOG_WARNING, "Failed to get pci info for device");
			return 0;
		}

		/*
		 * The reading is accurate to within a range of +/- 5 watts. It is only available if power management mode is supported
		 */
		result = (nvmlReturn_t)nvmlDeviceGetPowerUsage(device, power);
		if (NVML_SUCCESS != result) {
			printf("ERROR: %s\n", nvmlErrorString(result));
			plugin_log(LOG_WARNING, "Failed to get power usage");
			return 0;
		}
		else {
			totalWatts = *power / 1000.0;
			printf(">> power: %f Watts\n", totalWatts);


			if (submitPower(totalWatts, i) != 0) {
				WARNING("nvidia_plugin plugin: Dispatching a value failed.");
			}
		}
	}

	/* A return value != 0 indicates an error and the plugin will be skipped for an increasing amount of time. */
	return 0;
}


/*
 * This function is called after values have been dispatched to collectd.
 */
 /*
static int my_write(const data_set_t *ds, const value_list_t *vl, user_data_t *ud) {
	return 0;
}
*/


/*
 * This function is called when plugin_log() has been used.
 */
static void my_log(int severity, const char *msg, user_data_t *ud) {
	printf("LOG: %i - %s\n", severity, msg);
	return;
}


/*
 * This function is called when plugin_dispatch_notification () has been used.
 */
/* static int my_notify(const notification_t *notif, user_data_t *ud) {
	return 0;
} */


/*
 * This function is called before shutting down collectd.
 */
static int my_shutdown(void) {
	nvmlReturn_t result;

	/* close sockets, free data structures, ... */
	result = nvmlShutdown();
	if (NVML_SUCCESS != result) {
		printf("Failed to shutdown NVML: %s\n", nvmlErrorString(result));
		plugin_log(LOG_WARNING, "Failed to shutdown NVML");
		return -1;
	}

	return 0;
}


/*
 * This function is called after loading the plugin to register it with collectd.
 */
void module_register(void) {
	plugin_register_log("nvidia_plugin", my_log, /* user data */ NULL);
	//plugin_register_notification("nvidia_plugin", my_notify, /* user data */ NULL);
	plugin_register_data_set(&ds);
	plugin_register_read("nvidia_plugin", my_read);
	plugin_register_init("nvidia_plugin", my_init);
	//plugin_register_write("nvidia_plugin", my_write, /* user data */ NULL);
	plugin_register_shutdown("nvidia_plugin", my_shutdown);
    return;
}
