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

//#include "nvml.h"


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
	"xeon_phi_plugin", STATIC_ARRAY_SIZE (dsrc), dsrc
};


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
 * submitPower:
 * 	This is a utility function used by the read callback to populate a value_list_t
 * 	and pass it to plugin_dispatch_values.
 */
static int submitPower(gauge_t value, unsigned int deviceIndex) {
	char strDeviceIndex[8];

	value_list_t vl = VALUE_LIST_INIT;

	vl.values = &(value_t) { .gauge = value }; /* Convert the gauge_t to a value_t and add it to the value_list_t. */
	vl.values_len = 1;



	/* dispatch the values to collectd which passes them on to all registered write functions */
	return plugin_dispatch_values(&vl);
}


/*
 * This function is called in regular intervalls to collect the data.
 */
static int my_read (void) {


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


	return 0;
}


/*
 * This function is called after loading the plugin to register it with collectd.
 */
void module_register(void) {
	plugin_register_log("xeon_phi_plugin", my_log, /* user data */ NULL);
	//plugin_register_notification("xeon_phi_plugin", my_notify, /* user data */ NULL);
	plugin_register_data_set(&ds);
	plugin_register_read("xeon_phi_plugin", my_read);
	plugin_register_init("xeon_phi_plugin", my_init);
	//plugin_register_write("xeon_phi_plugin", my_write, /* user data */ NULL);
	plugin_register_shutdown("xeon_phi_plugin", my_shutdown);
    return;
}
