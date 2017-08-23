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
// Required for Windows and Visual Studio 2012 and newer
#ifndef __linux__
#define _CRT_SECURE_NO_WARNINGS    1
#endif

#include <errno.h>
#include <limits.h>
#include <miclib.h>
#ifdef __linux__
#include <unistd.h>
#else
#define usleep(arg)    Sleep(arg / 1000)
#define NAME_MAX       1000
#endif

#define MAX_DEVICES    (32)
#define MAX_CORES      (256)
#define NUM_SAMPLES    (3)


/*
 * This function is called once upon startup to initialize the plugin.
 */
static int my_init(void) {
	nvmlReturn_t result;

	/* open sockets, initialize data structures, ... */


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

	for (i = 0; i < 2; i++) {

		/***************************************************************************************
		 * POWER
		 ***************************************************************************************/
		 /*
		 if (submitValue(totalWatts, "power", i) != 0) {
			 WARNING("xeonphi_plugin plugin: Dispatching a value failed.");
		 }
		 */

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
		 ***************************************************************************************/
		 /*
		 if (submitValue(nvmlUtilization.memory, "memory", i) != 0) {
			WARNING("xeonphi_plugin plugin: Dispatching a value [memory] failed.");
			}
		*/

		/***************************************************************************************
		 * TEMPERATURE
		 ***************************************************************************************/
		/*
			 Retrieves the current temperature readings for the device, in degrees C
		 */
		 /*
		 if (submitValue(ret, "temperature", i) != 0) {
			 WARNING("xeonphi_plugin plugin: Dispatching a value failed.");
		 }
		 */

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
	nvmlReturn_t result;

	/* close sockets, free data structures, ... */


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
