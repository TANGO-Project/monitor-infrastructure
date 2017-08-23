#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <miclib.h>
#include <unistd.h>

#define MAX_DEVICES    (32)
#define MAX_CORES      (256)
#define NUM_SAMPLES    (3)


int main(void) {
  //
  int ncards;
  struct mic_devices_list *mdl;
  struct mic_device *mdh;
  int ret;
  uint32_t device_type;
  unsigned int card_num;

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

  for (card_num = 0; card_num < ncards; card_num++) {
    int card, total_util;
    struct mic_power_util_info *pinfo;
    uint32_t pwr;
    struct mic_memory_util_info *memory;
    uint32_t total_size, avail_size;
    struct mic_thermal_info *thermal;
    uint32_t temp;
    struct mic_core_util *cutil = NULL;
    uint64_t idle_sum, sys_sum, nice_sum, user_sum;

    printf("    >>\n");

    /* Get card at index i */
    if (mic_get_device_at_index(mdl, card_num, &card) != E_MIC_SUCCESS) {
        fprintf(stderr, "Error: Failed to get card at index %d: %s: %s\n", card_num, mic_get_error_string(), strerror(errno));
        mic_free_devices(mdl);
        return 4;
    }

    if (mic_open_device(&mdh, card) != E_MIC_SUCCESS) {
        fprintf(stderr, "Error: Failed to open card %d: %s: %s\n", card_num, mic_get_error_string(), strerror(errno));
        return 5;
    }

    if (mic_get_device_type(mdh, &device_type) != E_MIC_SUCCESS) {
        fprintf(stderr, "Error: Failed to get device type %s! ", mic_get_device_name(mdh));
        (void)mic_close_device(mdh);
        return 6;
    }

    if (device_type != KNC_ID) {
      fprintf(stderr, "Error: Unknown device Type: %u\n", device_type);
      (void)mic_close_device(mdh);
      return 7;
    }

    /***************************************************************************************
		 * POWER:
     *    int *mic_get_power_utilization_info*(struct mic_device *device, struct mic_power_util_info **power_info);
     *    int mic_get_inst_power_readings(struct mic_power_util_info *power_info, uint32_t *power)
     *    int *mic_free_power_utilization_info*(struct mic_power_util_info *power_info);
		 ***************************************************************************************/
    if (mic_get_power_utilization_info(mdh, &pinfo) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to get power utilization information: %s\n", mic_get_device_name(mdh));
      return 8;
    }

    if (mic_get_inst_power_readings(pinfo, &pwr) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to get instant power readings: %s\n", mic_get_device_name(mdh));
      (void)mic_free_power_utilization_info(pinfo);
      return 9;
    }

    printf("    Instant power reading: %u watts\n", pwr / 1000000);
    (void)mic_free_power_utilization_info(pinfo);

    /***************************************************************************************
		 * UTILIZATION & MEMORY:
          int mic_alloc_core_util(struct mic_core_util **cutil);

          int mic_update_core_util(struct mic_device *device, struct mic_core_util *cutil);

          int mic_get_idle_sum(struct mic_core_util *cutil, uint64_t *idle_sum);
          int mic_get_sys_sum(struct mic_core_util *cutil, uint64_t *sys_sum);
          int mic_get_nice_sum(struct mic_core_util *cutil, uint64_t *nice_sum);
          int mic_get_user_sum(struct mic_core_util *cutil, uint64_t *user_sum);

          int mic_free_core_util(struct mic_core_util *cutil);
		 ***************************************************************************************/
    if (mic_alloc_core_util(&cutil) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to allocate Core utilization information: %s\n", mic_get_device_name(mdh));
      return 10;
    }

    if (mic_update_core_util(mdh, cutil) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to update Core utilization information: %s\n", mic_get_device_name(mdh));
      (void)mic_free_core_util(cutil);
      return 11;
    }

    if (mic_get_idle_sum(cutil, &idle_sum) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to get the idle sum: %s\n", mic_get_device_name(mdh));
      (void)mic_free_core_util(cutil);
      return 12;
    }

    if (mic_get_nice_sum(cutil, &nice_sum) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to get the nice sum: %s\n", mic_get_device_name(mdh));
      (void)mic_free_core_util(cutil);
      return 13;
    }

    if (mic_get_sys_sum(cutil, &sys_sum) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to get the system sum: %s\n", mic_get_device_name(mdh));
      (void)mic_free_core_util(cutil);
      return 14;
    }

    if (mic_get_user_sum(cutil, &user_sum) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to get the user sum: %s\n", mic_get_device_name(mdh));
      (void)mic_free_core_util(cutil);
      return 10;
    }

    total_util = (int)idle_sum + (int)nice_sum + (int)sys_sum + (int)user_sum;

    printf("    Utilization: \n");
    printf("     -Idle: %g \n", (((double)idle_sum)/total_util)*100);
    printf("     -Nice: %g \n", (((double)nice_sum)/total_util)*100);
    printf("     -System: %g \n", (((double)sys_sum)/total_util)*100);
    printf("     -User: %g \n", (((double)user_sum)/total_util)*100);

    (void)mic_free_core_util(cutil);

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
      return 11;
    }

    if (mic_get_total_memory_size(memory, &total_size) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to get total_memory_size: %s\n", mic_get_device_name(mdh));
      (void)mic_free_memory_utilization_info(memory);
      return 11;
    }

    if (mic_get_available_memory_size(memory, &avail_size) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to get available_memory_size: %s\n", mic_get_device_name(mdh));
      (void)mic_free_memory_utilization_info(memory);
      return 11;
    }

    printf("    Instant memory reading: %d MB\n", ((total_size-avail_size) / 1024));
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
      return 11;
    }

    if (mic_get_die_temp(thermal, &temp) != E_MIC_SUCCESS) {
      fprintf(stderr, "Error: Failed to get instant thermal readings: %s\n", mic_get_device_name(mdh));
      (void)mic_free_thermal_info(thermal);
      return 11;
    }

    printf("    Instant thermal reading: %u C\n", temp );
    (void)mic_free_thermal_info(thermal);

    /***************************************************************************************
 		 * RUNNING PROCESSES
 		 ***************************************************************************************/

    printf("    Found KNC device '%s'\n", mic_get_device_name(mdh));
  }

  (void)mic_free_devices(mdl);

  (void)mic_close_device(mdh);
  return 0;
}
