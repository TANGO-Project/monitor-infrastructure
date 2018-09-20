/*
 * Collectd IPMI plugin
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

// COLLECTD //
#include "collectd.h"
#include "common.h"
#include "plugin.h"

// IPMI //
/*
 * freeipmi libs
 */
#include <ipmi_monitoring.h>
#include <ipmi_monitoring_bitmasks.h>


/*
 * GLOBALS / IPMI configuration
 */

/* Hostname, NULL for In-band communication, non-null for a hostname */
char *hostname = NULL;

/* In-band Communication Configuration */
/*
 * On linux, you may wish to change driver_type to
 * IPMI_MONITORING_DRIVER_TYPE_OPENIPMI if you use /dev/ipmi0
 */
int driver_type = IPMI_MONITORING_DRIVER_TYPE_KCS; /* or -1 for default */
int disable_auto_probe = 0;     /* probe for in-band device */
unsigned int driver_address = 0; /* not used if probing */
unsigned int register_spacing = 0; /* not used if probing */
char *driver_device = NULL;     /* not used if probing */
unsigned int workaround_flags = 0; /* Workarounds - specify workaround flags if necessary */


/* Initialize w/ record id numbers to only monitor specific record ids */
/*
    88  | CPU0 Pwr        | Power Supply            | 134.00     | W     | 'OK'
    89  | CPU1 Pwr        | Power Supply            | 66.00      | W     | 'OK'
    90  | CPU0 DIM01 Pwr  | Power Supply            | 5.00       | W     | 'OK'
    91  | CPU0 DIM23 Pwr  | Power Supply            | 4.00       | W     | 'OK'
    92  | CPU1 DIM01 Pwr  | Power Supply            | 5.00       | W     | 'OK'
    93  | CPU1 DIM23 Pwr  | Power Supply            | 4.00       | W     | 'OK'
    94  | Blade Pwr       | Power Supply            | 246.00     | W     | 'OK'
*/
unsigned int record_ids[] = {88, 89, 90, 91, 92, 93, 94};
unsigned int record_ids_length = 7;

/* Initialize w/ sensor types to only monitor specific sensor types
 * see ipmi_monitoring.h sensor types list.
 */
unsigned int sensor_types[] = {0};
unsigned int sensor_types_length = 0;


/* Set to an appropriate alternate if desired */
char *sdr_cache_directory = "/tmp";
char *sensor_config_file = NULL;

/* Initialization flags
 *
 * Most commonly bitwise OR IPMI_MONITORING_FLAGS_DEBUG and/or
 * IPMI_MONITORING_FLAGS_DEBUG_IPMI_PACKETS for extra debugging
 * information.
 */
unsigned int ipmimonitoring_init_flags = 0;

/* IPMI context */
ipmi_monitoring_ctx_t ctx = NULL;

/**
 * Set to 1 or 0 to enable these sensor reading flags
 * - See ipmi_monitoring.h for descriptions of these flags: https://github.com/elitak/freeipmi/blob/master/libipmimonitoring/ipmi_monitoring.h.in
 *
 * REREAD_SDR_CACHE - Re-read the SDR cache
 *
 * IGNORE_NON_INTERPRETABLE_SENSORS - Do not read sensors that cannot be interpreted.
 *
 * BRIDGE_SENSORS - Attempt to bridge sensors not owned by the BMC
 *
 * INTERPRET_OEM_DATA - Attempt to interpret OEM data if read.
 *
 * SHARED_SENSORS - Iterate through shared sensors if found
 *
 * DISCRETE_READING - Allow sensor readings to be read even if the event/reading type
 *                    code for the sensor is not valid.  This option works around
 *                    poorly defined (and arguably illegal) SDR records that list
 *                    non-discrete sensor expectations along with discrete state
 *                    conditions.
 *
 * IGNORE_SCANNING_DISABLED - Ignore the scanning bit and read sensors no matter
 *                            what.  This option works around motherboards
 *                            that incorrectly indicate sensors as disabled.
 *
 * ASSUME_BMC_OWNER - Assume the BMC is the sensor owner no matter what.  This option
 *                    works around motherboards that incorrectly indicate a non-BMC
 *                    sensor owner (e.g. usually bridging is required).
 *
 * ENTITY_SENSOR_NAMES - Return sensor names with appropriate entity
 *                       id and instance prefixed when appropriate.
 *
 * ASSUME_MAX_SDR_RECORD_COUNT - If motherboard does not implement SDR
 *                               record reading properly, do not fail
 *                               out.  Assume a max count.
 */
int reread_sdr_cache = 0;
int ignore_non_interpretable_sensors = 1;
int bridge_sensors = 0;
int interpret_oem_data = 0;
int shared_sensors = 0;
int discrete_reading = 0;
int ignore_scanning_disabled = 0;
int assume_bmc_owner = 0;
int entity_sensor_names = 0;



struct ipmi_monitoring_ipmi_config ipmi_config;



/**
 * 'ipmi_config' initialization
 *
 */
int init_ipmi_config (struct ipmi_monitoring_ipmi_config *ipmi_config) {
  assert(ipmi_config);

  ipmi_config->driver_type = driver_type;
  ipmi_config->disable_auto_probe = disable_auto_probe;
  ipmi_config->driver_address = driver_address;
  ipmi_config->register_spacing = register_spacing;
  ipmi_config->driver_device = driver_device;
  ipmi_config->workaround_flags = workaround_flags;

  return 0;
}



/**
 * gets the sensor's type string
 *
 */
const char * get_sensor_type_string (int sensor_type) {
	switch (sensor_type) {
		case IPMI_MONITORING_SENSOR_TYPE_RESERVED:
			return ("Reserved");
		case IPMI_MONITORING_SENSOR_TYPE_TEMPERATURE:
			return ("Temperature");
		case IPMI_MONITORING_SENSOR_TYPE_VOLTAGE:
			return ("Voltage");
		case IPMI_MONITORING_SENSOR_TYPE_CURRENT:
			return ("Current");
		case IPMI_MONITORING_SENSOR_TYPE_FAN:
			return ("Fan");
		case IPMI_MONITORING_SENSOR_TYPE_PHYSICAL_SECURITY:
			return ("Physical Security");
		case IPMI_MONITORING_SENSOR_TYPE_PLATFORM_SECURITY_VIOLATION_ATTEMPT:
			return ("Platform Security Violation Attempt");
		case IPMI_MONITORING_SENSOR_TYPE_PROCESSOR:
			return ("Processor");
		case IPMI_MONITORING_SENSOR_TYPE_POWER_SUPPLY:
			return ("Power Supply");
		case IPMI_MONITORING_SENSOR_TYPE_POWER_UNIT:
			return ("Power Unit");
		case IPMI_MONITORING_SENSOR_TYPE_COOLING_DEVICE:
			return ("Cooling Device");
		case IPMI_MONITORING_SENSOR_TYPE_OTHER_UNITS_BASED_SENSOR:
			return ("Other Units Based Sensor");
		case IPMI_MONITORING_SENSOR_TYPE_MEMORY:
			return ("Memory");
		case IPMI_MONITORING_SENSOR_TYPE_DRIVE_SLOT:
			return ("Drive Slot");
		case IPMI_MONITORING_SENSOR_TYPE_POST_MEMORY_RESIZE:
			return ("POST Memory Resize");
		case IPMI_MONITORING_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS:
			return ("System Firmware Progress");
		case IPMI_MONITORING_SENSOR_TYPE_EVENT_LOGGING_DISABLED:
			return ("Event Logging Disabled");
		case IPMI_MONITORING_SENSOR_TYPE_WATCHDOG1:
			return ("Watchdog 1");
		case IPMI_MONITORING_SENSOR_TYPE_SYSTEM_EVENT:
			return ("System Event");
		case IPMI_MONITORING_SENSOR_TYPE_CRITICAL_INTERRUPT:
			return ("Critical Interrupt");
		case IPMI_MONITORING_SENSOR_TYPE_BUTTON_SWITCH:
			return ("Button/Switch");
		case IPMI_MONITORING_SENSOR_TYPE_MODULE_BOARD:
			return ("Module/Board");
		case IPMI_MONITORING_SENSOR_TYPE_MICROCONTROLLER_COPROCESSOR:
			return ("Microcontroller/Coprocessor");
		case IPMI_MONITORING_SENSOR_TYPE_ADD_IN_CARD:
			return ("Add In Card");
		case IPMI_MONITORING_SENSOR_TYPE_CHASSIS:
			return ("Chassis");
		case IPMI_MONITORING_SENSOR_TYPE_CHIP_SET:
			return ("Chip Set");
		case IPMI_MONITORING_SENSOR_TYPE_OTHER_FRU:
			return ("Other Fru");
		case IPMI_MONITORING_SENSOR_TYPE_CABLE_INTERCONNECT:
			return ("Cable/Interconnect");
		case IPMI_MONITORING_SENSOR_TYPE_TERMINATOR:
			return ("Terminator");
		case IPMI_MONITORING_SENSOR_TYPE_SYSTEM_BOOT_INITIATED:
			return ("System Boot Initiated");
		case IPMI_MONITORING_SENSOR_TYPE_BOOT_ERROR:
			return ("Boot Error");
		case IPMI_MONITORING_SENSOR_TYPE_OS_BOOT:
			return ("OS Boot");
		case IPMI_MONITORING_SENSOR_TYPE_OS_CRITICAL_STOP:
			return ("OS Critical Stop");
		case IPMI_MONITORING_SENSOR_TYPE_SLOT_CONNECTOR:
			return ("Slot/Connector");
		case IPMI_MONITORING_SENSOR_TYPE_SYSTEM_ACPI_POWER_STATE:
			return ("System ACPI Power State");
		case IPMI_MONITORING_SENSOR_TYPE_WATCHDOG2:
			return ("Watchdog 2");
		case IPMI_MONITORING_SENSOR_TYPE_PLATFORM_ALERT:
			return ("Platform Alert");
		case IPMI_MONITORING_SENSOR_TYPE_ENTITY_PRESENCE:
			return ("Entity Presence");
		case IPMI_MONITORING_SENSOR_TYPE_MONITOR_ASIC_IC:
			return ("Monitor ASIC/IC");
		case IPMI_MONITORING_SENSOR_TYPE_LAN:
			return ("LAN");
		case IPMI_MONITORING_SENSOR_TYPE_MANAGEMENT_SUBSYSTEM_HEALTH:
			return ("Management Subsystem Health");
		case IPMI_MONITORING_SENSOR_TYPE_BATTERY:
				return ("Battery");
		case IPMI_MONITORING_SENSOR_TYPE_SESSION_AUDIT:
			return ("Session Audit");
		case IPMI_MONITORING_SENSOR_TYPE_VERSION_CHANGE:
			return ("Version Change");
		case IPMI_MONITORING_SENSOR_TYPE_FRU_STATE:
			return ("FRU State");
	}

	return ("Unrecognized");
}


/**
 * Initializes IPMI library
 *
 */
int intialize_ipmi() {
	int errnum;

	/* ipmi_monitoring_init */
	if (ipmi_monitoring_init(ipmimonitoring_init_flags, &errnum) < 0) {
		fprintf(stderr, "ipmi_monitoring_init: %s", ipmi_monitoring_ctx_strerror(errnum));
		return -1;
	}
	else {
		printf("  > ipmi_monitoring initialized \n");
	}

	/* ipmi_monitoring_ctx_create */
	if (!(ctx = ipmi_monitoring_ctx_create())) {
		fprintf(stderr, "ipmi_monitoring_ctx_create");
		return -1;
	}
	else {
		printf("  > ipmi_monitoring context created \n");
	}

	/* ipmi_monitoring_ctx_sdr_cache_directory */
	if (sdr_cache_directory) {
		if (ipmi_monitoring_ctx_sdr_cache_directory(ctx, sdr_cache_directory) < 0) {
			fprintf(stderr, "ipmi_monitoring_ctx_sdr_cache_directory: %s", ipmi_monitoring_ctx_errormsg(ctx));
			return -1;
		}
		else {
			printf("  > ipmi_monitoring context sdr_cache_directory: %s \n", sdr_cache_directory);
		}
	}

	/* Must call otherwise only default interpretations ever used */
	if (ipmi_monitoring_ctx_sensor_config_file(ctx, sensor_config_file) < 0) {
		fprintf(stderr, "ipmi_monitoring_ctx_sensor_config_file: %s", ipmi_monitoring_ctx_errormsg(ctx));
		return -1;
	}
	else {
		printf("  > ipmi_monitoring context sensor_config_file: %s \n", sensor_config_file);
	}

	return 0;
}


/*
 * This function is called once upon startup to initialize the plugin.
 */
static int my_init(void) {
	if (init_ipmi_config(&ipmi_config) == 0) {
		printf("  IPMI configuration done\n");
		printf("- Initializing IPMI ...\n");
		if (intialize_ipmi() == 0) {
			printf("  IPMI library intialized\n");
		}
		else {
			printf("- ERROR: during the IMPI initialization!\n");
			return -1;
		}
	}
	else {
		printf("- ERROR: while configuring IMPI!\n");
		return -1;
	}

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
	sstrncpy(vl.plugin, "ipmi", sizeof (vl.plugin));
	sstrncpy(vl.plugin_instance, strDeviceIndex, sizeof (vl.plugin_instance));
	sstrncpy(vl.type, type, sizeof (vl.type));

	/* dispatch the values to collectd which passes them on to all registered write functions */
	return plugin_dispatch_values(&vl);
}


/*
 * processmetric:
 * 	process values from metric.
 */
int processmetric(int record_id, char * sensor_name) {
	int sensor_type, sensor_reading_type, sensor_units;
	void *sensor_reading;
	const char *sensor_type_str;

	/* TYPE */
	if ((sensor_type = ipmi_monitoring_sensor_read_sensor_type(ctx)) < 0) {
		fprintf(stderr, "ipmi_monitoring_sensor_read_sensor_type: %s\n", ipmi_monitoring_ctx_errormsg(ctx));
		return -1;
	}
	sensor_type_str = get_sensor_type_string(sensor_type);

	printf("%u, %s, %s", record_id, sensor_name, sensor_type_str);

	/* UNITS */
	if ((sensor_units = ipmi_monitoring_sensor_read_sensor_units (ctx)) < 0)
	{
		fprintf(stderr, "ipmi_monitoring_sensor_read_sensor_units: %s\n", ipmi_monitoring_ctx_errormsg(ctx));
		return -1;
	}

	/* VALUE !! */
	if ((sensor_reading_type = ipmi_monitoring_sensor_read_sensor_reading_type(ctx)) < 0) {
		fprintf(stderr, "ipmi_monitoring_sensor_read_sensor_reading_type: %s\n", ipmi_monitoring_ctx_errormsg(ctx));
		return -1;
	}
	sensor_reading = ipmi_monitoring_sensor_read_sensor_reading(ctx);

	if (sensor_reading) {
		const char *sensor_units_str;

		if (sensor_reading_type == IPMI_MONITORING_SENSOR_READING_TYPE_UNSIGNED_INTEGER8_BOOL)
			printf (", %s", (*((uint8_t *)sensor_reading) ? "true" : "false"));
		else if (sensor_reading_type == IPMI_MONITORING_SENSOR_READING_TYPE_UNSIGNED_INTEGER32)
			printf (", %u", *((uint32_t *)sensor_reading));
		else if (sensor_reading_type == IPMI_MONITORING_SENSOR_READING_TYPE_DOUBLE)
			printf (", %.2f", *((double *)sensor_reading));
		else
			printf (", N/A");

		if (sensor_units == IPMI_MONITORING_SENSOR_UNITS_CELSIUS)
			sensor_units_str = "C";
		else if (sensor_units == IPMI_MONITORING_SENSOR_UNITS_FAHRENHEIT)
			sensor_units_str = "F";
		else if (sensor_units == IPMI_MONITORING_SENSOR_UNITS_VOLTS)
			sensor_units_str = "V";
		else if (sensor_units == IPMI_MONITORING_SENSOR_UNITS_AMPS)
			sensor_units_str = "A";
		else if (sensor_units == IPMI_MONITORING_SENSOR_UNITS_RPM)
			sensor_units_str = "RPM";
		else if (sensor_units == IPMI_MONITORING_SENSOR_UNITS_WATTS)
			sensor_units_str = "W";
		else if (sensor_units == IPMI_MONITORING_SENSOR_UNITS_PERCENT)
			sensor_units_str = "%";
		else
			sensor_units_str = "N/A";

		printf (", %s", sensor_units_str);
	}
	else {
		printf (", N/A, N/A");
	}

  /* COLLECTD */
  // TODO collectd types!!!
  // TODO cardnum = 1
  if (sensor_reading_type == IPMI_MONITORING_SENSOR_READING_TYPE_UNSIGNED_INTEGER32) {
    if (submitValue(*((uint32_t *)sensor_reading), "power", record_id) != 0) {
      WARNING("ipmi_plugin: Dispatching a value [power] failed.");
    }
  }
  else if (sensor_reading_type == IPMI_MONITORING_SENSOR_READING_TYPE_DOUBLE) {
    if (submitValue(*((double *)sensor_reading), "power", record_id) != 0) {
      WARNING("ipmi_plugin: Dispatching a value [power] failed.");
    }
  }

	return 0;
}


/*
 * This function is called in regular intervalls to collect the data.
 */
static int my_read (void) {
	unsigned int sensor_reading_flags = 0;
	int sensor_count;
	unsigned int i;

	if (reread_sdr_cache)
		sensor_reading_flags |= IPMI_MONITORING_SENSOR_READING_FLAGS_REREAD_SDR_CACHE;

	if (ignore_non_interpretable_sensors)
		sensor_reading_flags |= IPMI_MONITORING_SENSOR_READING_FLAGS_IGNORE_NON_INTERPRETABLE_SENSORS;

	if (bridge_sensors)
		sensor_reading_flags |= IPMI_MONITORING_SENSOR_READING_FLAGS_BRIDGE_SENSORS;

	if (interpret_oem_data)
		sensor_reading_flags |= IPMI_MONITORING_SENSOR_READING_FLAGS_INTERPRET_OEM_DATA;

	if (shared_sensors)
		sensor_reading_flags |= IPMI_MONITORING_SENSOR_READING_FLAGS_SHARED_SENSORS;

	if (discrete_reading)
		sensor_reading_flags |= IPMI_MONITORING_SENSOR_READING_FLAGS_DISCRETE_READING;

	if (ignore_scanning_disabled)
		sensor_reading_flags |= IPMI_MONITORING_SENSOR_READING_FLAGS_IGNORE_SCANNING_DISABLED;

	if (assume_bmc_owner)
		sensor_reading_flags |= IPMI_MONITORING_SENSOR_READING_FLAGS_ASSUME_BMC_OWNER;

	if (entity_sensor_names)
		sensor_reading_flags |= IPMI_MONITORING_SENSOR_READING_FLAGS_ENTITY_SENSOR_NAMES;


	if (!record_ids_length && !sensor_types_length) {
		if ((sensor_count = ipmi_monitoring_sensor_readings_by_record_id(ctx, hostname, &ipmi_config, sensor_reading_flags, NULL, 0, NULL, NULL)) < 0) {
			fprintf (stderr, "ipmi_monitoring_sensor_readings_by_record_id: %s\n", ipmi_monitoring_ctx_errormsg(ctx));
			return -1;
		}
		else {
			printf("  > Total sensors (I): %i \n", sensor_count);
		}
	}
	else if (record_ids_length) {
		if ((sensor_count = ipmi_monitoring_sensor_readings_by_record_id(ctx, hostname, &ipmi_config, sensor_reading_flags, record_ids, record_ids_length, NULL, NULL)) < 0) {
			fprintf(stderr, "ipmi_monitoring_sensor_readings_by_record_id: %s\n", ipmi_monitoring_ctx_errormsg(ctx));
			return -1;
		}
		else {
			printf("  > Total sensors (II): %i \n", sensor_count);
		}
	}
	else {
		if ((sensor_count = ipmi_monitoring_sensor_readings_by_sensor_type(ctx, hostname, &ipmi_config, sensor_reading_flags, sensor_types, sensor_types_length, NULL, NULL)) < 0) {
			fprintf(stderr, "ipmi_monitoring_sensor_readings_by_sensor_type: %s\n", ipmi_monitoring_ctx_errormsg(ctx));
			return -1;
		}
		else {
			printf("  > Total sensors (III): %i \n", sensor_count);
		}
	}

  for (i = 0; i < sensor_count; i++, ipmi_monitoring_sensor_iterator_next(ctx)) {
		int record_id;
		char *sensor_name = NULL;

		/* ID */
		if ((record_id = ipmi_monitoring_sensor_read_record_id(ctx)) < 0) {
			fprintf(stderr, "ipmi_monitoring_sensor_read_record_id: %s\n", ipmi_monitoring_ctx_errormsg(ctx));
			return -1;
    }

		/* NAME */
		if (!(sensor_name = ipmi_monitoring_sensor_read_sensor_name(ctx))) {
			fprintf(stderr, "ipmi_monitoring_sensor_read_sensor_name: %s\n", ipmi_monitoring_ctx_errormsg(ctx));
			return -1;
    }
		if (!strlen(sensor_name)) {
			sensor_name = "N/A";
		}

		processmetric(record_id, sensor_name);
	}

	return 0;
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
	plugin_register_read("ipmi_plugin", my_read);
	plugin_register_init("ipmi_plugin", my_init);
	plugin_register_shutdown("ipmi_plugin", my_shutdown);
	return;
}
