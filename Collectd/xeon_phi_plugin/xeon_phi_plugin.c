/*
 * Collectd XEON PHI plugin
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

// XEON //
#include <errno.h>
#include <limits.h>
#include <miclib.h>
#include <unistd.h>

int ncards;
struct mic_devices_list *mdl;


/*
 * This function is called once upon startup to initialize the plugin.
 */
static int my_init(void) {
	int ret;

	/* open sockets, initialize data structures, ... */
	ret = mic_get_devices(&mdl);
  if (ret == E_MIC_DRIVER_NOT_LOADED) {
    fprintf(stderr, "Error: The driver is not loaded! ");
    fprintf(stderr, "Load the driver before using this tool.\n");
    return 1; /* Return a general error to the shell */
  }
  else if (ret == E_MIC_ACCESS) {
    fprintf(stderr, "Error: Access is denied to the driver! ");
    fprintf(stderr, "Do you have permissions to access the driver?\n");
    return 1; /* Return a general error to the shell */
  }
  else if (ret != E_MIC_SUCCESS) {
    fprintf(stderr, "Failed to get cards list: %s: %s\n", mic_get_error_string(), strerror(errno));
    return 1;
  }

  if (mic_get_ndevices(mdl, &ncards) != E_MIC_SUCCESS) {
    fprintf(stderr, "Error: Failed to get number of cards! ");
    (void)mic_free_devices(mdl);
    return 2;
  }

  if (ncards == 0) {
    fprintf(stderr, "Error: No MIC card found! ");
    (void)mic_free_devices(mdl);
    return 3;
  }
  printf("    Found %d ncards \n", ncards);

	/* A return value != 0 indicates an error and causes the plugin to be disabled. */
  return 0;
}


/*
 * submitValue:
 * 	This is a utility function used by the read callback to populate a value_list_t
 * 	and pass it to plugin_dispatch_values.
 */
static int submitValue(gauge_t value, const char *type, unsigned int deviceIndex) {
	char strDeviceIndex[8];

	value_list_t vl = VALUE_LIST_INIT;

	vl.values = &(value_t) { .gauge = value }; /* Convert the gauge_t to a value_t and add it to the value_list_t. */
	vl.values_len = 1;

	sprintf(strDeviceIndex, "%d", deviceIndex);

	sstrncpy(vl.host, hostname_g, sizeof (vl.host));
	sstrncpy(vl.plugin, "xeonphi", sizeof (vl.plugin));
	sstrncpy(vl.plugin_instance, strDeviceIndex, sizeof (vl.plugin_instance));
	sstrncpy(vl.type, type, sizeof (vl.type));

	/* dispatch the values to collectd which passes them on to all registered write functions */
	return plugin_dispatch_values(&vl);
}


/*
 * This function is called in regular intervalls to collect the data.
 */
static int my_read (void) {
	struct mic_device *mdh;
	uint32_t device_type;
  unsigned int card_num;

	for (card_num = 0; card_num < ncards; card_num++) {
    int card;
    struct mic_power_util_info *pinfo;
    uint32_t pwr;
    struct mic_memory_util_info *memory;
    uint32_t total_size, avail_size;
    struct mic_thermal_info *thermal;
    uint32_t temp;
    //struct mic_core_util *cutil = NULL;
    //uint64_t idle_sum, sys_sum, nice_sum, user_sum;

		/* Get card at index card_num */
    if (mic_get_device_at_index(mdl, card_num, &card) != E_MIC_SUCCESS) {
        fprintf(stderr, "Error: Failed to get card at index %d: %s: %s\n", card_num, mic_get_error_string(), strerror(errno));
        mic_free_devices(mdl);
        return 1;
    }

    if (mic_open_device(&mdh, card) != E_MIC_SUCCESS) {
        fprintf(stderr, "Error: Failed to open card %d: %s: %s\n", card_num, mic_get_error_string(), strerror(errno));
        return 2;
    }

    if (mic_get_device_type(mdh, &device_type) != E_MIC_SUCCESS) {
        fprintf(stderr, "Error: Failed to get device type %s! ", mic_get_device_name(mdh));
        (void)mic_close_device(mdh);
        return 3;
    }

    if (device_type != KNC_ID) {
      fprintf(stderr, "Error: Unknown device Type: %u\n", device_type);
      (void)mic_close_device(mdh);
      return 4;
    }

		/***************************************************************************************
		 * POWER:
     *    int *mic_get_power_utilization_info*(struct mic_device *device, struct mic_power_util_info **power_info);
     *    int mic_get_inst_power_readings(struct mic_power_util_info *power_info, uint32_t *power)
     *    int *mic_free_power_utilization_info*(struct mic_power_util_info *power_info);
		 ***************************************************************************************/
    if (mic_get_power_utilization_info(mdh, &pinfo) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to get power utilization information: %s\n", mic_get_device_name(mdh));
    }
    else if (mic_get_inst_power_readings(pinfo, &pwr) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to get instant power readings: %s\n", mic_get_device_name(mdh));
    }
		else if (submitValue((pwr / 1000000), "power", card_num) != 0) {
			plugin_log(LOG_WARNING, "xeonphi_plugin plugin: Dispatching a value failed.");
		}
    (void)mic_free_power_utilization_info(pinfo);

		/***************************************************************************************
		 * UTILIZATION
		 ***************************************************************************************/
		 /*
		 if (submitValue(nvmlUtilization.gpu, "percent", i) != 0) {
			 WARNING("xeonphi_plugin plugin: Dispatching a value [percent] failed.");
		 }
		 */

		 /***************************************************************************************
 		 * MEMORY
      *    int *mic_get_memory_utilization_info*(struct mic_device *device, struct mic_memory_util_info **memory);
      *    int *mic_get_total_memory_size*(struct mic_memory_util_info *memory, uint32_t *total_size);
      *    int *mic_get_available_memory_size*(struct mic_memory_util_info *memory, uint32_t *avail_size);
      *    int *mic_get_memory_buffers_size*(struct mic_memory_util_info *memory, uint32_t *bufs);
      *    int *mic_free_memory_utilization_info*(struct mic_memory_util_info *memory);
 		 ***************************************************************************************/
    if (mic_get_memory_utilization_info(mdh, &memory) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to get memory utilization information: %s\n", mic_get_device_name(mdh));
    }
    else if (mic_get_total_memory_size(memory, &total_size) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to get total_memory_size: %s\n", mic_get_device_name(mdh));
    }
    else if (mic_get_available_memory_size(memory, &avail_size) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to get available_memory_size: %s\n", mic_get_device_name(mdh));
    }
		else if (submitValue(((total_size-avail_size) / 1024), "memory", card_num) != 0) {
			plugin_log(LOG_WARNING, "xeonphi_plugin plugin: Dispatching a value [memory] failed.");
		}
		(void)mic_free_memory_utilization_info(memory);

		/***************************************************************************************
 		 * TEMPERATURE
     *    int *mic_get_thermal_info*(struct mic_device *device, struct mic_thermal_info **thermal);
     *    int mic_get_die_temp(struct mic_thermal_info *thermal, uint32_t *temp);
     *    int mic_get_gddr_temp(struct mic_thermal_info *thermal, uint16_t *temp);
     *    int *mic_free_thermal_info*(struct mic_thermal_info *thermal);
 		 ***************************************************************************************/
    if (mic_get_thermal_info(mdh, &thermal) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to get thermal information: %s\n", mic_get_device_name(mdh));
    }
    else if (mic_get_die_temp(thermal, &temp) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to get instant thermal readings: %s\n", mic_get_device_name(mdh));
    }
		else if (submitValue(temp, "temperature", card_num) != 0) {
			plugin_log(LOG_WARNING, "xeonphi_plugin plugin: Dispatching a value failed.");
		}
    (void)mic_free_thermal_info(thermal);

		/***************************************************************************************
		 * RUNNING PROCESSES
		 ***************************************************************************************/
		/*
			This function returns about compute running processes
		*/
		/*
		if (submitValue(num_procs, "objects", i) != 0) {
			WARNING("xeonphi_plugin plugin: Dispatching a value failed.");
		}
		*/

	}

	/* A return value != 0 indicates an error and the plugin will be skipped for an increasing amount of time. */
	return 0;
}


/*
 * This function is called when plugin_log() has been used.
 */
static void my_log(int severity, const char *msg, user_data_t *ud) {
	printf("LOG: %i - %s\n", severity, msg);
	return;
}


/*
 * This function is called before shutting down collectd.
 */
static int my_shutdown(void) {
	/* close sockets, free data structures, ... */
	(void)mic_free_devices(mdl);

	return 0;
}


/*
 * This function is called after loading the plugin to register it with collectd.
 */
void module_register(void) {
	plugin_register_log("xeonphi_plugin", my_log, /* user data */ NULL);
	plugin_register_read("xeonphi_plugin", my_read);
	plugin_register_init("xeonphi_plugin", my_init);
	plugin_register_shutdown("xeonphi_plugin", my_shutdown);
  return;
}
