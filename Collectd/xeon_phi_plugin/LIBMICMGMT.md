-----------------------------
**Copyright 2010-2013 Intel Corporation.**

This library is free software; you can redistribute it and/or modify it  under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, version 2.1.

This library is distributed in the hope that it will be useful,  but WITHOUT ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU  Lesser General Public License for more details.

Disclaimer: The codes contained in these modules may be specific to the Intel Software Development Platform codenamed Knights Ferry, and the Intel product codenamed Knights Corner, and are not backward compatible with other Intel products. Additionally, Intel will NOT support the codes or instruction set in future products.

Intel offers no warranty of any kind regarding the code. This code is licensed on an "AS IS" basis and Intel is not obligated to provide any support, assistance, installation, training, or other services of any kind. Intel is also not obligated to provide any updates, enhancements or extensions. Intel specifically disclaims any warranty of merchantability, non-infringement, fitness for any particular purpose, and any other warranty.

Further, Intel disclaims all liability of any kind, including but not limited to liability for infringement of any proprietary rights, relating to the use of the code, even if Intel is notified of the possibility of such liability. Except as expressly stated in an Intel license agreement provided with this code and agreed upon with Intel, no license, express or implied, by estoppel or otherwise, to any intellectual property rights is granted herein.

-----------------------------

# LIBMICMGMT(7) #

-----------------------

- [NAME](#name)
- [SYNOPSIS](#synopsis)
  - [Identify Available Coprocessors](#identify-available-coprocessors)
  - [Open Close Coprocessor](#open-close-coprocessor)
  - [Memory Information](#memory-information)
  - [Thermal Information](#thermal-information)
  - [Power Utilization Information](#power-utilization-information)
  - [Memory Utilization](#memory-utilization)
- [DESCRIPTION](#description)
  - [Memory Information Functions](#memory-information-functions)
  - [Processor Information Functions](#processor-information-functions)
  - [Coprocessor OS Information Functions](#coprocessor-os-information-functions)
  - [Thermal Information Functions](#thermal-information-functions)
  - [Memory Utilization Functions](#memory-utilization-functions)
- [COPYRIGHT](#copyright)

-----------------------------

## NAME ##

**libmicmgmt** - C-library to access and update Intel(R) Xeon Phi(TM) Coprocessor parameters.

-----------------------------

## SYNOPSIS ##

*#include <miclib.h>*

"libmicmgmt" library provides a C-library interface to Intel(R) Xeon Phi(TM) coprocessors (also known as 'Many Integrated Core' or MIC) to applications running on the host system. These are essentially wrappers built on top of the MIC host driver interfaces that are available in the form of ioctls, sysfs entries(in Linux), WMI entries(in Microsoft Windows) and scif calls. Various functions are available to query the state of each coprocessor, or list or modify some of its parameters. Library functions are categorized into the following groups: +


#### Identify Available Coprocessors ####

```c
int *mic_get_devices*(struct mic_devices_list **devices);

int *mic_free_devices*(struct mic_devices_list *devices);

int *mic_get_ndevices*(struct mic_devices_list *devices, int *ndevices);

int *mic_get_device_at_index*(struct mic_devices_list *devices, int index, int *device);
```

#### Open Close Coprocessor ####

```c
int *mic_open_device*(struct mic_device **device, uint32_t device_num);

int *mic_close_device*(struct mic_device *device);
```

#### Driver Information ####

```c
int *mic_get_device_type*(struct mic_device *device, uint32_t *type);

const char **mic_get_device_name*(struct mic_device *device);

int *mic_get_sysfs_attribute*(struct mic_device *device, const char *name, char *value, size_t *size);
```

#### Error Reporting ####

```c
const char **mic_get_error_string*(void);

int *mic_clear_error_string*(void);

int *mic_get_ras_errno*(void);

const char **mic_get_ras_error_string*(int ras_errno);
```

#### Query/modify Coprocessor State ####

```c
int *mic_enter_maint_mode*(struct mic_device *device);

int *mic_leave_maint_mode*(struct mic_device *device);

int *mic_in_maint_mode*(struct mic_device *device, int *mode);

int *mic_in_ready_state*(struct mic_device *device, int *state);

int *mic_get_post_code*(struct mic_device *device, char *post_code, size_t *size);
```

#### Flash Operations ####

```c
int *mic_flash_size*(struct mic_device *device, size_t *size); +

int *mic_flash_active_offs*(struct mic_device *device, off_t *active); +

int *mic_flash_update_start*(struct mic_device *device, void *buf, size_t bufsize, struct mic_flash_op **desc); +

int *mic_flash_update_done*(struct mic_flash_op *desc); +

int *mic_flash_read_start*(struct mic_device *device, void *buf, size_t bufsize, struct mic_flash_op **desc); +

int *mic_flash_read_done*(struct mic_flash_op *desc); +

int *mic_set_ecc_mode_start*(struct mic_device *mdh, uint16_t ecc_enabled, struct mic_flash_op **desc); +

int *mic_set_ecc_mode_done*(struct mic_flash_op *desc); +

int *mic_get_flash_status_info*(struct mic_flash_op *desc, struct mic_flash_status_info **status); +

int *mic_free_flash_status_info*(struct mic_flash_status_info *status);

int *mic_get_progress*(struct mic_flash_status_info *status, uint32_t *percent); +

int *mic_get_status*(struct mic_flash_status_info *status, int *cmd_status); +

int *mic_get_ext_status*(struct mic_flash_status_info *status, int *ext_status); +

int *mic_flash_version*(struct mic_device *device, void *buf, char *str, size_t strsize); +

int *mic_get_flash_vendor_device*(struct mic_device *device, char *buf, size_t *size); +
```

#### PCI Configuration Parameters ####

```c
int *mic_get_pci_config*(struct mic_device *device, struct mic_pci_config **conf); +

int *mic_free_pci_config*(struct mic_pci_config *conf); +

int *mic_get_bus_number*(struct mic_pci_config *conf, uint16_t *bus_num); +

int *mic_get_device_number*(struct mic_pci_config *conf, uint16_t *dev_num); +

int *mic_get_vendor_id*(struct mic_pci_config *conf, uint16_t *id); +

int *mic_get_device_id*(struct mic_pci_config *conf, uint16_t *id); +

int *mic_get_revision_id*(struct mic_pci_config *conf, uint8_t *id); +

int *mic_get_subsystem_id*(struct mic_pci_config *conf, uint16_t *id); +

int *mic_get_link_speed*(struct mic_pci_config *conf, char *speed, size_t *size); +

int *mic_get_link_width*(struct mic_pci_config *conf, uint32_t *width); +

int *mic_get_max_payload*(struct mic_pci_config *conf, uint32_t *payload); +

int *mic_get_max_readreq*(struct mic_pci_config *conf, uint32_t *readreq); +

int *mic_get_pci_class_code*(struct mic_pci_config *conf, char *class_code, size_t *size);+

int *mic_get_pci_domain_id*(struct mic_pci_config *conf, uint16_t *domain);+
```

#### Memory Information ####

View Memory Information [functions](#memory-information-functions) description.

```c
int *mic_get_memory_info*(struct mic_device *device, struct mic_device_mem **mem_info); +

int *mic_free_memory_info*(struct mic_device_mem *mem_info); +

int *mic_get_memory_vendor*(struct mic_device_mem *mem_info, char *vendor, size_t *size); +

int *mic_get_memory_revision*(struct mic_device_mem *mem_info, uint32_t *revision); +

int *mic_get_memory_density*(struct mic_device_mem *mem_info, uint32_t *density); +

int *mic_get_memory_size*(struct mic_device_mem *mem_info, uint32_t *size); +

int *mic_get_memory_speed*(struct mic_device_mem *mem_info, uint32_t *speed); +

int *mic_get_memory_type*(struct mic_device_mem *mem_info, char *type, size_t *size); +

int *mic_get_memory_frequency*(struct mic_device_mem *mem_info, uint32_t *freq); +

int *mic_get_memory_voltage*(struct mic_device_mem *mem, uint32_t *buf); +

int *mic_get_ecc_mode*(struct mic_device_mem *mem_info, uint16_t *mode); +
```

#### Processor Information ####

View Processor Information [functions](#processor-information-functions) description.

```c
int *mic_get_processor_info*(struct mic_device *device, struct mic_processor_info **proc_info); +

int *mic_free_processor_info*(struct mic_processor_info *proc_info); +

int *mic_get_processor_model*(struct mic_processor_info *proc_info, uint16_t *model, uint16_t *model_ext); +

int *mic_get_processor_family*(struct mic_processor_info *proc_info, uint16_t *family, uint16_t *family_ext); +

int *mic_get_processor_type*(struct mic_processor_info *proc_info, uint16_t *type); +

int *mic_get_processor_steppingid*(struct mic_processor_info *proc_info, uint32_t *id); +

int *mic_get_processor_stepping*(struct mic_processor_info *proc_info, char *stepping, size_t *size)
```

#### Coprocessor OS Information ####

View Coprocessor OS Information [functions](#coprocessor-os-information-functions) description.

```c
int *mic_get_cores_info*(struct mic_device *device, struct mic_cores_info **cores_info); +

int *mic_free_cores_info*(struct mic_cores_info *cores_info); +

int *mic_get_cores_count*(struct mic_cores_info *cores_info, uint32_t *count); +

int *mic_get_cores_voltage*(struct mic_cores_info *cores_info, uint32_t *voltage); +

int *mic_get_cores_frequency*(struct mic_cores_info *cores_info, uint32_t *frequency); +

int *mic_alloc_core_util*(struct mic_core_util **cutil); +

int *mic_free_core_util(struct mic_core_util *cutil); +

int *mic_update_core_util*(struct mic_device *device, struct mic_core_util *cutil); +

int *mic_get_idle_counters*(struct mic_core_util *cutil, uint64_t *idle_counters); +

int *mic_get_nice_counters*(struct mic_core_util *cutil, uint64_t *nice_counters); +

int *mic_get_sys_counters*(struct mic_core_util *cutil, uint64_t *sys_counters); +

int *mic_get_user_counters*(struct mic_core_util *cutil, uint64_t *user_counters); +

int *mic_get_tick_count*(struct mic_core_util *cutil, uint32_t *tick_count);+

int *mic_get_idle_sum*(struct mic_core_util *cutil, uint64_t *idle_sum); +

int *mic_get_sys_sum*(struct mic_core_util *cutil, uint64_t *sys_sum); +

int *mic_get_nice_sum*(struct mic_core_util *cutil, uint64_t *nice_sum); +

int *mic_get_user_sum*(struct mic_core_util *cutil, uint64_t *user_sum); +

int *mic_get_jiffy_counter*(struct mic_core_util *cutil, uint64_t *jiffy); +

int *mic_get_num_cores*(struct mic_core_util *cutil, uint16_t *num_cores); +

int *mic_get_threads_core*(struct mic_core_util *cutil, uint16_t *threads_core); +
```

#### Thermal Information ####

View Thermal Information [functions](#thermal-information-functions) description.

```c
int *mic_get_thermal_info*(struct mic_device *device, struct mic_thermal_info **thermal);

int *mic_free_thermal_info*(struct mic_thermal_info *thermal);

int *mic_get_smc_hwrevision*(struct mic_thermal_info *thermal, char *revision, size_t *size);

int *mic_get_smc_fwversion*(struct mic_thermal_info *thermal, char *version, size_t *size);

int *mic_is_smc_boot_loader_ver_supported*(struct mic_thermal_info *thermal, int *supported);

int *mic_get_smc_boot_loader_ver*(struct mic_thermal_info *thermal, char *version, size_t *size);

int *mic_get_fsc_status*(struct mic_thermal_info *thermal, uint32_t *status);

int *mic_get_die_temp*(struct mic_thermal_info *thermal, uint32_t *temp);

int *mic_is_die_temp_valid*(struct mic_thermal_info *thermal, int *valid);

int *mic_get_gddr_temp*(struct mic_thermal_info *thermal, uint16_t *temp);

int *mic_is_gddr_temp_valid*(struct mic_thermal_info *thermal, int *valid);

int *mic_get_fanin_temp*(struct mic_thermal_info *thermal, uint16_t *temp);

int *mic_is_fanin_temp_valid*(struct mic_thermal_info *thermal, int *valid);

int *mic_get_fanout_temp*(struct mic_thermal_info *thermal, uint16_t *temp);

int *mic_is_fanout_temp_valid*(struct mic_thermal_info *thermal, int *valid);

int *mic_get_vccp_temp*(struct mic_thermal_info *thermal, uint16_t *temp);

int *mic_is_vccp_temp_valid*(struct mic_thermal_info *thermal, int *valid);

int *mic_get_vddg_temp*(struct mic_thermal_info *thermal, uint16_t *temp);

int *mic_is_vddg_temp_valid*(struct mic_thermal_info *thermal, int *valid);

int *mic_get_vddq_temp*(struct mic_thermal_info *thermal, uint16_t *temp);

int *mic_is_vddq_temp_valid*(struct mic_thermal_info *thermal, int *valid);

int *mic_get_fan_rpm*(struct mic_thermal_info *thermal, uint32_t *rpm);

int *mic_get_fan_pwm*(struct mic_thermal_info *thermal, uint32_t *pwm);
```

#### Version Information ####

```c
int *mic_get_version_info*(struct mic_device *device, struct mic_version_info **version);

int *mic_free_version_info*(struct mic_version_info *version);

int *mic_get_uos_version*(struct mic_version_info *version, char *uos_version, size_t *size);

int *mic_get_flash_version*(struct mic_version_info *version, char *flash_version, size_t *size);

int *mic_get_fsc_strap*(struct mic_version_info *version, char *strap, size_t *size);
```

#### SKU Information ####

```c
int *mic_get_silicon_sku*(struct mic_device *device, char *sku, size_t *size);
```

#### Serial Number ####

```c
int *mic_get_serial_number*(struct mic_device *device, char *serial, size_t *size);
```

#### Power Utilization Information ####

```c
int *mic_get_power_utilization_info*(struct mic_device *device, struct mic_power_util_info **power_info);

int *mic_free_power_utilization_info*(struct mic_power_util_info *power_info);

int *mic_get_total_power_readings_w0*(struct mic_power_util_info *power_info, uint32_t *power);

int *mic_get_total_power_sensor_sts_w0*(struct mic_power_util_info *power_info, uint32_t *status);

int *mic_get_total_power_readings_w1*(struct mic_power_util_info *power_info, uint32_t *power);

int *mic_get_total_power_sensor_sts_w1*(struct mic_power_util_info *power_info, uint32_t *status);

int *mic_get_inst_power_readings*(struct mic_power_util_info *power_info, uint32_t *power);

int *mic_get_inst_power_sensor_sts*(struct mic_power_util_info *power_info, uint32_t *status);

int *mic_get_max_inst_power_readings*(struct mic_power_util_info *power_info, uint32_t *power);

int *mic_get_max_inst_power_sensor_sts*(struct mic_power_util_info *power_info, uint32_t *status);

int *mic_get_pcie_power_readings*(struct mic_power_util_info *power_info, uint32_t *power);

int *mic_get_pcie_power_sensor_sts*(struct mic_power_util_info *power_info, uint32_t *status);

int *mic_get_c2x3_power_readings*(struct mic_power_util_info *power_info, uint32_t *power);

int *mic_get_c2x3_power_sensor_sts*(struct mic_power_util_info *power_info, uint32_t *status);

int *mic_get_c2x4_power_readings*(struct mic_power_util_info *power_info, uint32_t *power);

int *mic_get_c2x4_power_sensor_sts*(struct mic_power_util_info *power_info, uint32_t *status);

int *mic_get_vccp_power_readings*(struct mic_power_util_info *power_info, uint32_t *power);

int *mic_get_vccp_power_sensor_sts*(struct mic_power_util_info *power_info, uint32_t *status);

int *mic_get_vccp_current_readings*(struct mic_power_util_info *power_info, uint32_t *current);

int *mic_get_vccp_current_sensor_sts*(struct mic_power_util_info *power_info, uint32_t *status);

int *mic_get_vccp_voltage_readings*(struct mic_power_util_info *power_info, uint32_t *voltage);

int *mic_get_vccp_voltage_sensor_sts*(struct mic_power_util_info *power_info, uint32_t *status);

int *mic_get_vddg_power_readings*(struct mic_power_util_info *power_info, uint32_t *power);

int *mic_get_vddg_power_sensor_sts*(struct mic_power_util_info *power_info, uint32_t *status);

int *mic_get_vddg_current_readings*(struct mic_power_util_info *power_info, uint32_t *current);

int *mic_get_vddg_current_sensor_sts*(struct mic_power_util_info *power_info, uint32_t *status);

int *mic_get_vddg_voltage_readings*(struct mic_power_util_info *power_info, uint32_t *voltage);

int *mic_get_vddg_voltage_sensor_sts*(struct mic_power_util_info *power_info, uint32_t *status);

int *mic_get_vddq_power_readings*(struct mic_power_util_info *power_info, uint32_t *power);

int *mic_get_vddq_power_sensor_sts*(struct mic_power_util_info *power_info, uint32_t *status);

int *mic_get_vddg_current_readings*(struct mic_power_util_info *power_info, uint32_t *current);

int *mic_get_vddq_current_sensor_sts*(struct mic_power_util_info *power_info, uint32_t *status);

int *mic_get_vddq_voltage_readings*(struct mic_power_util_info *power_info, uint32_t *voltage);

int *mic_get_vddq_voltage_sensor_sts*(struct mic_power_util_info *power_info, uint32_t *status);
```

#### Memory Utilization ####

```c
int *mic_get_memory_utilization_info*(struct mic_device *device, struct mic_memory_util_info **memory);

int *mic_get_total_memory_size*(struct mic_memory_util_info *memory, uint32_t *total_size);

int *mic_get_available_memory_size*(struct mic_memory_util_info *memory, uint32_t *avail_size);

int *mic_get_memory_buffers_size*(struct mic_memory_util_info *memory, uint32_t *bufs);

int *mic_free_memory_utilization_info*(struct mic_memory_util_info *memory);
```

#### Power Limits ####

```c
int *mic_get_power_limit*(struct mic_device *device, struct mic_power_limit **limit);

int *mic_get_power_phys_limit*(struct mic_power_limit *limit, uint32_t *phys_lim);

int *mic_get_power_hmrk*(struct mic_power_limit *limit, uint32_t *hmrk);

int *mic_get_power_lmrk*(struct mic_power_limit *limit, uint32_t *lmrk);

int *mic_get_time_window0*(struct mic_power_limit *limit, uint32_t *time_window);

int *mic_get_time_window1*(struct mic_power_limit *limit, uint32_t *time_window);

int *mic_set_power_limit0*(struct mic_device *mdh, uint32_t power, uint32_t time_window);

int *mic_set_power_limit1*(struct mic_device *mdh, uint32_t power, uint32_t time_window);

int *mic_free_power_limit*(struct mic_power_limit *limit);
```

#### Throttle State ####

```c
int *mic_get_throttle_state_info*(struct mic_device *device, struct mic_throttle_state_info **ttl_state);

int *mic_get_thermal_ttl_active*(struct mic_throttle_state_info *ttl_state, int *active);

int *mic_get_thermal_ttl_current_len*(struct mic_throttle_state_info *ttl_state, uint32_t *current);

int *mic_get_thermal_ttl_count*(struct mic_throttle_state_info *ttl_state, uint32_t *count);

int *mic_get_thermal_ttl_time*(struct mic_throttle_state_info *ttl_state, uint32_t *time);

int *mic_get_power_ttl_active*(struct mic_throttle_state_info *ttl_state, int *active);

int *mic_get_power_ttl_current_len*(struct mic_throttle_state_info *ttl_state, uint32_t *current);

int *mic_get_power_ttl_count*(struct mic_throttle_state_info *ttl_state,  uint32_t *count);

int *mic_get_power_ttl_time*(struct mic_throttle_state_info *ttl_state, uint32_t *time);

int *mic_free_throttle_state_info*(struct mic_throttle_state_info *ttl_state);
```

#### Turbo State ####

```c
int *mic_get_turbo_state_info*(struct mic_device *device, struct mic_turbo_info **turbo);

int *mic_get_turbo_state*(struct mic_turbo_info *turbo, uint32_t *active);

int *mic_get_turbo_mode*(struct mic_turbo_info *turbo, uint32_t *mode);

int *mic_set_turbo_mode*(struct mic_device *device, uint32_t *mode);

int *mic_get_turbo_state_valid*(struct mic_turbo_info *turbo, uint32_t *valid);

int *mic_free_turbo_info*(struct mic_turbo_info *turbo);
```

#### Miscellaneous ####

```c
int *mic_get_led_alert*(struct mic_device *device, uint32_t *led_alert);

int *mic_set_led_alert*(struct mic_device *device, uint32_t *led_alert);

int *mic_get_uuid*(struct mic_device *device, uint8_t *uuid, size_t *size);

int *mic_is_ras_avail*(struct mic_device *device, int *ras_avail);

int *mic_get_smc_persistence_flag*(struct mic_device *device, int *persist_flag);

int *mic_set_smc_persistence_flag*(struct mic_device *device, int persist_flag);
```

#### Coprocessor OS Power Management Configuration ####

```c
int *mic_get_uos_pm_config*(struct mic_device *mdh, struct mic_uos_pm_config **pm_config);

int *mic_get_cpufreq_mode*(struct mic_uos_pm_config *pm_config, int *mode);

int *mic_get_corec6_mode*(struct mic_uos_pm_config *pm_config, int *mode);

int *mic_get_pc3_mode*(struct mic_uos_pm_config *pm_config, int *mode);

int *mic_get_pc6_mode*(struct mic_uos_pm_config *pm_config, int *mode);

int *mic_free_uos_pm_config*(struct mic_uos_pm_config *pm_config);
```

-----------------------------

## DESCRIPTION ##

In order to access an Intel(R) Xeon Phi(TM) Coprocessor it must first be opened by a call to *mic_open_device()*.

#### int *mic_open_device*(struct mic_device **device, uint32_t device_num);

The input *device_num* argument must contain the device number *<n>* as it appears in the */sys/class/mic/mic<n>* entry on a Linux machine or in the WMI entry <board_id> for the coprocessor to be accessed on a Microsoft Windows machine. A list of available coprocessors may also be obtained by making a call to *mic_get_devices()*.

The *mic_open_device()* call returns a handle in the *device* argument upon success. This handle must be used to access the coprocessor board until it's closed by a subsequent *mic_close_device()* function call.

Note that this function, and the library in general, guarantees support of up to 31 concurrent calls to the same coprocessor due to limitations imposed by the coprocessor implementation.

-----------------------------

#### int *mic_close_device*(struct mic_device *device);

The input argument *device* refers to the handle returned by a previous
successful *mic_open_device()* call. This function closes the opened coprocessor
and releases all resources that were allocated by the corresponding
*mic_open_device()*. The result is undefined if the function is called with
a handle that was previously closed, corrupted, or not returned by a
previous open call.

-----------------------------

#### int *mic_get_devices*(struct mic_devices_list **devices);

This function returns the list of coprocessors present on the system in the
*devices* argument. Note that this structure is dynamically allocated and, when
no longer needed, should be freed by calling *mic_free_devices()*. Accessor
functions, *mic_get_ndevices()* and *mic_get_device_at_index()*, are available
to retrieve the individual fields of this list.

-----------------------------

#### int *mic_get_ndevices*(struct mic_devices_list *devices, int *ndevices);

#### int *mic_get_device_at_index*(struct mic_devices_list *devices, int index, int *device);

Upon successful completion of *mic_get_ndevices()* function, the argument
*ndevices* is returned with the number of Intel(R) Xeon Phi(TM) coprocessors
present on the system.

On Linux, the *mic_get_device_at_index()* function returns in the argument *device*
the number of coprocessor *<n>* as found in */sys/class/mic/mic<n>*. On Microsoft
Windows, the *mic_get_device_at_index()* function returns in the argument *device*
the number of coprocessor *<n>* as found in the WMI entry <board_id>. The input
argument *index* must contain the position of the coprocessor in the list
referenced by the *devices* argument and must contain a value between 0 and
*(number of coprocessors present - 1)* (see *mic_get_ndevices()* above)
or *E_MIC_RANGE* is returned.

The input *devices* argument must point to a *struct mic_devices_list**
structure returned by a previous successful *mic_get_devices()* call that
was also not freed by *mic_free_devices()*. Otherwise, the behavior of these
functions is undefined.

-----------------------------

#### int *mic_free_devices*(struct mic_devices_list *devices);

This function frees the memory allocated by a previous, successful
*mic_get_devices()* call that was not already freed. Any other input will result
in undefined behavior.

-----------------------------

In the following functions, as always, the input *struct mic_device**
argument must contain a valid value from a *mic_open_device()* call that has not
been closed, or the behavior of the function is undefined.

Moreover, any error and return value that is unique to a particular function is
listed in its description. Otherwise, refer to the *RETURN VALUE* section in the
*mic_open_device()* description for values returned upon success and error.

-----------------------------

#### int *mic_get_device_type*(struct mic_device *device, uint32_t *type);

This function returns the type of the Intel(R) Xeon Phi(TM) coprocessor to the
*type* argument. On Linux, the value is an enumeration that is read from the
*/sys/class/mic/mic<n>/family* file. On Microsoft Windows, the value is an
enumeration that is read from the *family* WMI entry. Currently, the only
value returned is *KNC_ID*.

-----------------------------

#### const char **mic_get_device_name*(struct mic_device *device);

This function returns the name of the coprocessor. On Linux, this is *mic<n>* in
*/sys/class/mic/mic<n>*, which is referenced by the input *device* argument. On
Microsoft Windows, it is the *mic<n>* in *<board_id>* WMI entry. Note that the
returned string points to a statically allocated memory location that may be
overwritten by subsequent calls. A value of NULL is returned upon error.

-----------------------------

#### int *mic_get_sysfs_attribute*(struct mic_device *device, const char *name, char *value, size_t *size);

If successful, this function will return the value of the specified
sysfs attribute.  The behavior is undetermined if the input size is greater
than the actual buffer size, or if the buffer references a NULL or
invalid memory location. The appropriate *size* value can be queried by
setting its value to 0. No more than *size* bytes will be written to the
buffer, including the nul terminator.

This function is only supported in Linux; if called in another OS, this
fuction will return E_MIC_NOT_IMPLEMENTED.

-----------------------------

#### const char **mic_get_error_string(void);

This function may be used upon failure to get a more verbose message from the
library. In failure state it returns a string that specifies the object and the
operation that failed. The string references statically allocated thread-safe
memory that may be overwritten by subsequent errors encountered by the same
thread. The *mic_clear_error_string()* function may be used to reset this
message.

-----------------------------

#### int *mic_get_ras_errno*(void);

This function returns the latest RAS error number returned by the RAS module
during the most recent interaction between the RAS module and micmgmt API
functions. This function should be called when a function returns
*E_MIC_RAS_ERROR* and before making the next API function call.  In other words,
the RAS error number returned by this function is only relevant when the most
recent API function call returned *E_MIC_RAS_ERROR*.  The RAS error number is
not guaranteed to be maintained across function calls.

-----------------------------

#### const char **mic_get_ras_error_string*(int ras_errno);

This function returns the error string corresponding to the given RAS error
number (in *ras_errno*).  If the provided RAS error number is out of range,
NULL will be returned.

-----------------------------

#### int *mic_enter_maint_mode*(struct mic_device *device);

The Intel(R) Xeon Phi(TM) coprocessor x100 product family provides a
maintenance operation mode that allows certain privileged tasks like upgrading
the contents of on-board flash memory and System Management Controller, and
also implements RAS features and more. This function provides an interface to
switch the operation of the specified coprocessor to maintenance mode. Note that
this mode may be entered only from *ready* state, which is equivalent to the
coprocessor being reset and, therefore, not running any operating system and
applications.

Since maintenance mode is a trusted mode in which many otherwise unavailable
registers are left in unlocked state, a well behaved application should perform
its maintenance task and promptly return to ready state with a call to
*mic_leave_maint_mode()*.

Switching modes is an asynchronous operation. This function returns
immediately after requesting a switch to maintenance mode. On Linux, it is
done by writing to the */sys/class/mic/mic<n>/state* file. On Microsoft Windows,
it is done by writing to the *state* WMI entry. It only returns an error if
this write fails. To ensure that the switch is successfully completed the state
must be polled with the *mic_in_maint_mode()* function.

-----------------------------

#### int *mic_in_maint_mode*(struct mic_device *device, int *mode);

This function returns a non-zero value in the *int *mode* argument if
the specified coprocessor is in maintenance mode, otherwise it is set to 0.
Applications must poll the state after requesting a change to maintenance
mode using the *mic_enter_maint_mode()* function call. On Linux, the coprocessor
state is polled by reading the */sys/class/mic/mic<n>/state* and
*/sys/class/mic/mic<n>/mode* files. On Microsoft Windows, the coprocessor state
is polled by reading the WMI entries *state* and *mode* respectively.

-----------------------------

#### int *mic_leave_maint_mode*(struct mic_device *device);

This function may be used to begin transition back to *ready* state from
maintenance mode for the specified coprocessor. Well behaved applications must
make sure that it is called after a successful change to maintenance mode. This
is also an asynchronous operation and the *mic_in_ready_state()* function must
be polled to check the state of the board.

-----------------------------

#### int *mic_in_ready_state*(struct mic_device *device, int *state);

A call to *mic_leave_maint_mode()* may be followed by *mic_in_ready_state()*.
The *int *state* argument is set to a non-zero value if the
transition to ready state is completed, otherwise it is set to 0. The board
state is returned from the */sys/class/mic/mic<n>/state* file on a Linux machine.
On a Microsoft Windows machine, the board state is returned from the *state*
WMI entry.

-----------------------------

#### int *mic_get_post_code*(struct mic_device *device, char *post_code, size_t *size);

This functions returns the current POST code of the specified *device*.
The *size_t *size* argument is an input/output argument. Its input
value is used to determine the maximum number of characters (include trailing
nul) that may be transferred to the buffer. The appropriate *size* value
can be queried by setting its value to 0. The behavior is undetermined
if the input size is greater than the actual buffer size, or if the buffer
references a NULL or invalid memory location. On a Linux machine, the current
POST code is read from */sys/class/mic/mic<n>/post_code*. On Microsoft Windows,
the current POST code is read from *post_code* WMI entry.

-----------------------------

#### int *mic_flash_size*(struct mic_device *device, size_t *size);

This function may be called to get the flash size in maintenance mode.
The total flash memory size, in bytes, of the specified coprocessor is returned
in *size_t *size* argument. The value is 0x200000 (2 Mega-bytes) for the
Intel(R) Xeon Phi(TM) coprocessor x100 product family.

The behavior is undetermined if either of the output parameters reference
NULL or an invalid memory location.

-----------------------------

#### int *mic_flash_active_offs*(struct mic_device *device, off_t *active);

In order to recover from a situation where a flash update operation
may fail, two images are kept in the flash at any time. Upon successful update
the new image is made active whereas the previous one is de-activated. This
function may be used to get the offset of the active flash image. The
offset is returned in the *off_t *active* argument. On Linux, this value is read
from */sys/class/mic/mic<n>/fail_safe_offset* file. On Microsoft Windows, this
value is read from *fail_safe_offset* WMI entry.

-----------------------------

#### int *mic_flash_update_start*(struct mic_device *device, void *buf, size_t bufsize, struct mic_flash_op **desc);

This function may be called to write a flash image in maintenance mode.
*size_t bufsize* bytes referenced by *void *buf* are written to the
flash. Note that this is an asynchronous operation and its state must
be monitored with *mic_get_flash_status_info()*. *struct mic_flash_op **desc*
is set to reference a descriptor that may be used to monitor the update
operation and should be freed with the *mic_flash_update_done()* function.
The update operation uses the host driver *FLASH_CMD_WRITE* ioctl.

Note: The buffer referenced by *void *buf* should not be modified after
calling *mic_flash_update_start()*. If the buffer *void *buf* is held in a dynamically
allocated memory location, this memory should be kept valid until
*mic_flash_update_done()* is called. This function should not be called concurrently
more than once on each device. The behavior of this function is undefined
if *void *buf* is modified directly at any time after calling *mic_flash_update_start()*,
or if any of the previous conditions are not met.

-----------------------------

#### int *mic_flash_update_done*(struct mic_flash_op *desc);

Use *mic_flash_update_done()* to free resources allocated by a previous
*mic_flash_update_start()* function call. Before calling this function,
*mic_get_flash_status_info()* should state the update operation is not in
progress. The behavior of this function is undetermined if any of these conditions are
not met.

For additional information see *mic_flash_update_start()* documentation.

-----------------------------

#### int *mic_flash_read_start*(struct mic_device *device, void *buf, size_t bufsize, struct mic_flash_op **desc);

This function may be called to read the flash memory in maintenance mode.
This function reads the first *bufsize* bytes of flash memory of the
specified coprocessor into a buffer referenced by *void *buf*. Upon
successful start of this asynchronous operation a descriptor is returned to
*struct mic_flash_op **desc* that should be released with
*mic_flash_read_done()*. *mic_get_flash_status_info()* may be used to monitor
the progress of the operation. The host driver *FLASH_CMD_READ* ioctl is used
to commence the flash read operation.

Note: The buffer referenced by *void *buf* should not be read after calling
*mic_flash_read_start()* and before calling *mic_flash_read_done()*. If the buffer
*void *buf* is held in a dynamically allocated memory location, this memory
should be kept valid until *mic_flash_read_done()* is called. This function
should not be called concurrently more than once on each device. The behavior
of this function is undefined if *void *buf* is modified directly before calling
*mic_flash_read_done()*, or if any of the previous conditions are not met.

-----------------------------

#### int *mic_flash_read_done*(struct mic_flash_op *desc);

Use *mic_flash_read_done()* to free resources allocated by a previous
*mic_flash_read_start()*. Before calling this function, *mic_get_flash_status_info()* should
state the update operation is not in progress. The behavior of this function is
undetermined if any of these conditions are not met.

For additional information see *mic_flash_read_done()* documentation.

-----------------------------

#### int *mic_set_ecc_mode_start*(struct mic_device *mdh, uint16_t ecc_enabled, struct mic_flash_op **desc);

This function may be called to set the ECC mode while the coprocessor is in
"ready" state. Upon successful start of this asynchronous operation, a
descriptor is returned in *struct mic_flash_op **desc* that may be
released with *mic_set_ecc_mode_done()* upon completion.
*mic_get_flash_status_info()* may be used to monitor the progress of the
operation. The host driver *FLASH_CMD_READ* ioctl is used to commence the flash
read operation.

-----------------------------

#### int *mic_set_ecc_mode_done*(struct mic_flash_op *desc);

Use *mic_set_ecc_mode_done()* to free resources allocated by a previous
*mic_set_ecc_mode_start()* call that have not already been freed. Any
other input value will result in undetermined behavior.

-----------------------------

#### int *mic_get_flash_status_info*(struct mic_flash_op *desc, struct mic_flash_status_info **status_info);

This function may be called to get the flash status in maintenance mode.
The progress of a flash update or read request may be determined with
a *mic_get_flash_status_info()* function call.
*struct mic_flash_op *desc* should be a valid descriptor returned from a
previous *mic_flash_update_start()*, or *mic_flash_read_start()* call.
*struct mic_flash_status_info **status_info* is set to an opaque
structure that may be used by *mic_get_progress()*, *mic_get_status()* and
*mic_get_ext_status()* accessor functions and should be freed by calling
the *mic_free_flash_status_info()* function.

The host driver *FLASH_CMD_STATUS* ioctl is used to monitor the progress.

-----------------------------

#### int *mic_free_flash_status_info*(struct mic_flash_status_info *desc);

This function frees the valid *struct mic_flash_op *desc* that was
returned by a previous *mic_get_flash_status_info()* call.

-----------------------------

#### int *mic_get_progress*(struct mic_flash_status_info *desc, uint32_t *progress)

*uint32_t *progress* is set to the progress (in percent) of the Flash
or SMC operation that returned the *struct mic_flash_op *desc*
handle that has not been freed. Note that this should be used with the
*mic_get_status()* call, as it describes the actual operation being performed.

-----------------------------

#### int *mic_get_status*(struct mic_flash_status_info *status, int *status_val);


This function returns the Flash or SMC operation in progress inside
*uint32_t *status_val*. There may be instances when one request may
return status as an SMC or Flash operation in progress at different times. For
instance, the status may return *FLASH_OP_IN_PROGRESS* or *SMC_OP_IN_PROGRESS*
for the same update operation if the update image contained both Flash and SMC
firmware images. It is left to the calling application to determine how many
images are present. If both Flash and SMC Firmware are present in the update
image and the status shows *SMC_OP_IN_PROGRESS* then it implies that the Flash
update was successful and SMC operation is in progress.

Valid values returned in *uint32_t *status_val* are:

*FLASH_OP_IDLE* +

*FLASH_OP_INVALID* +

*FLASH_OP_IN_PROGRESS* +

*FLASH_OP_COMPLETED* +

*FLASH_OP_FAILED* +

*FLASH_OP_AUTH_FAILED* +

*SMC_OP_IN_PROGRESS* +

*SMC_OP_COMPLETED* +

*SMC_OP_FAILED* +

*SMC_OP_AUTH_FAILED* +

-----------------------------

#### int *mic_get_ext_status*(struct mic_flash_status_info *status, int *ext_status);

This function returns the extended status of a failed operation in *uint32_t
*ext_status*. This should be called only if the status of the
corresponding operation returned *FLASH_OP_FAILED*, *FLASH_OP_AUTH_FAILED*, or
*SMC_OP_FAILED*.

// Detail ext status here?

-----------------------------

#### int *mic_flash_version*(struct mic_device *device, void *buf, char *str, size_t strsize);

This function may be called to read the current flash version in maintenance
mode. The current Flash version for the specified coprocessor may be read
from its header. *void *buf* should point to memory that was filled by a
previous Flash read operation. The application should set the size of the buffer in
*size_t *strsize* and upon successful completion it is set to the actual
string size (including trailing nul). No more than the original *strsize* bytes
are written into *char *str*, which is always returned nul terminated.
The behavior of this function is undefined if *strsize* is set to a value larger
than the size of the buffer pointed to by *str*.

-----------------------------

#### int *mic_get_flash_vendor_device*(struct mic_device *device, char *buf, size_t *size);

This function may be called to get the flash vendor device in maintenance mode.
This function returns the Flash vendor and device information in the
buffer referenced by *void *buf*. The calling function must set
*size_t *size* to the size of the buffer. A larger value will result
in undefined behavior, or if the buffer references a NULL or invalid memory
location. The appropriate *size_t *size* value can be queried by
setting the *size* value to 0. No more than the original *size* bytes
are written into this buffer, which is always returned nul terminated.

-----------------------------

#### int *mic_get_pci_config*(struct mic_device *device, struct mic_pci_config **conf);

This function returns a handle in *struct mic_pci_config **conf* that
may be used by accessor functions described below to get PCI configuration
information about the Intel(R) Xeon Phi(TM) coprocessor specified by
*struct mic_device *device*. The resources allocated by this function
may be released by calling *mic_free_pci_config()* function. The behavior of
this function is undefined if *struct mic_device *device* doesn't point
to an open coprocessor, or if the *conf* argument doesn't reference valid memory.
The returned handle should be used by the accessor or free functions below
only if this function returned *E_MIC_SUCCESS*.

Note that all the functions below that use *struct mic_pci_config *conf*
must specify a *conf* handle that has not already been released, or was not
returned by a previous successful call to *mic_get_pci_config()*; or the
associated *struct mic_device *device* was not subsequently closed.
Otherwise, their behavior is undefined.

-----------------------------

#### int *mic_free_pci_config*(struct mic_pci_config *conf);

Resources allocated by a *mic_get_pci_config()* call may be released by
calling *mic_free_pci_config()*.

-----------------------------

#### int *mic_get_bus_number*(struct mic_pci_config *conf, uint16_t *bus_num);

This function returns the PCI bus number for the Intel(R) Xeon Phi(TM)
coprocessor that was used to return the *struct mic_pci_config *conf*
handle into memory referenced by *uint16_t *bus_num*. The valid values
for *uint16_t pass[*]bus_num are 0 to 255.

-----------------------------

#### int *mic_get_device_number*(struct mic_pci_config *conf, uint16_t *dev_num);

This accessor function returns the PCI slot number (0 to 31) in memory
referenced by *uint16_t *dev_num*.

-----------------------------

#### int *mic_get_vendor_id*(struct mic_pci_config *conf, uint16_t *id);

This function returns the 16-bit PCI vendor id in *uint16_t *id. This
value is 0x8086 for Intel(R) PCI devices.

-----------------------------

#### int *mic_get_device_id*(struct mic_pci_config *conf, uint16_t *id);

This function returns the 16-bit PCI Device ID for the associated
Intel(R) Xeon Phi(TM) coprocessor.

-----------------------------

#### int *mic_get_revision_id*(struct mic_pci_config *conf, uint8_t *id);

This function returns the 8-bit PCI Revision ID of the coprocessor in
*uint8_t *id*.

-----------------------------

#### int *mic_get_subsystem_id*(struct mic_pci_config *conf, uint16_t *id);

This function returns the 16-bit PCI Subsystem ID of the coprocessor in
*uint16_t *id*.

-----------------------------

#### int *mic_get_link_speed*(struct mic_pci_config *conf, char *speed, size_t *size); +

This function returns the current PCI Link speed of the Intel(R) Xeon Phi(TM)
coprocessor in the character buffer referenced by *char *speed*. The
calling program should specify the size of the buffer in *size_t *size*.
The behavior is undetermined if the input *size* is greater than the
required buffer size, or if the buffer references a NULL or invalid
memory location. The appropriate *size* value can be queried by setting
its value to 0.

-----------------------------

#### int *mic_get_link_width*(struct mic_pci_config *conf, uint32_t *width);

This function returns the existing PCI Link width of the associated coprocessor
in *uint32_t *width.

-----------------------------

#### int *mic_get_max_payload*(struct mic_pci_config *conf, uint32_t *payload);

This function returns the maximum PCI data payload size of the associated
coprocessor in *uint32_t *payload*.

-----------------------------

#### int *mic_get_max_readreq*(struct mic_pci_config *conf, uint32_t *readreq);

This function gets the maximum PCI read request size of the associated
coprocessor in *uint32_t *readreq*.

-----------------------------

#### int *mic_get_pci_class_code*(struct mic_pci_config *conf, char *class_code, size_t *size);

This function returns the current PCI Class code of the Intel(R) Xeon Phi(TM)
coprocessor in the character buffer referenced by *char *class_code*. The
calling program should specify the size of the buffer in *size_t *size*.
The behavior is undetermined if the input size  is greater than the actual
buffer size, or if the buffer references a NULL or invalid memory location.
The appropriate *size* value can be queried by setting its value to 0.  No
more than *size* bytes will be written to the buffer, including the nul terminator.

-----------------------------

#### int *mic_get_pci_domain_id*(struct mic_pci_config *conf, uint16_t *domain);

This function returns the 16-bit PCI domain id in *uint16_t *domain.

-----------------------------

### Memory Information Functions ###

#### int *mic_get_memory_info*(struct mic_device *device, struct mic_device_mem **mem_info);

This function returns a reference to an opaque structure in
*struct mic_device_mem **mem_info* as a handle. This handle may be used
by accessor functions described below to get some parameters about memory device
present on the specified Intel(R) Xeon Phi(TM) Coprocessor.

-----------------------------

#### int *mic_free_memory_info*(struct mic_device_mem *mem_info);

Resources allocated by *mic_get_memory_info()* may be freed by calling
*mic_free_memory_info* and passing a valid *struct mic_device_mem
*mem_info* from a previous *mic_get_memory_info()* call.

-----------------------------

#### int *mic_get_memory_vendor*(struct mic_device_mem *mem_info, char *vendor, size_t *size);

This accessor function uses a valid *struct mic_device_mem *mem_info*
handle and returns the memory device vendor's name in *char *vendor*.
This string is always nul terminated. The input value of *size_t *size*
must provide the size of the *char *vendor* buffer and it is set to
actual string length including the terminating nul character. On Linux, the
library internally uses the */sys/class/mic/mic<n>/meminfo* sysfs entry to
retrieve this information. The behavior is undetermined if the input size is
greater than the actual buffer size, or if the buffer references a NULL or
invalid memory location. The appropriate *size* value can be queried by setting its
value to 0.

-----------------------------

#### int *mic_get_memory_revision*(struct mic_device_mem *mem_info, uint32_t *revision);

The hardware revision of the memory device is returned in *uint32_t
*revision*. This function also retrieves the value from
*/sys/class/mic/mic<n>/meminfo* on Linux machines.

-----------------------------

#### int *mic_get_memory_density*(struct mic_device_mem *mem_info, uint32_t *density);

Memory density, in Mega-bits per device, of the memory device present on
specified Intel(R) Xeon Phi(TM) coprocessor is returned in
*uint32_t *density*. As with the previous two functions, this value is
also internally obtained from */sys/class/mic/mic<n>/meminfo* on Linux machines.

-----------------------------

#### int *mic_get_memory_size*(struct mic_device_mem *mem_info, uint32_t *size);

This function returns the memory size of the specified Intel(R) Xeon Phi(TM)
coprocessor, in Kilo-bytes, in the *uint32_t *size* argument.
On Linux, this value is read from */sys/class/mic/mic<n>/memsize*. On Microsoft
Windows, this value is read from * memsize * WMI entry.

-----------------------------

#### int *mic_get_memory_speed*(struct mic_device_mem *mem_info, uint32_t *speed);

*mic_get_memory_speed()* returns the specified coprocessor's memory speed,
or transaction speed, in kT/sec, in *uint32_t *speed*. On Linux, the
library uses */sys/class/mic/mic<n>/memspeed* to read this value. On Microsoft
Windows, the library uses the *memspeed* WMI entry.

-----------------------------

#### int *mic_get_memory_frequency*(struct mic_device_mem *mem_info, uint32_t *frequency);

*mic_get_memory_frequency()* returns the specified coprocessor's memory
frequency, in one-tenths of GT/sec, in *uint32_t *frequency*.
Internally, on Linux, the library uses */sys/class/mic/mic<n>/memfrequency* to
read this value, while it uses the * memfrequency * WMI entry on Microsoft
Windows.

-----------------------------

#### int *mic_get_memory_type*(struct mic_device_mem *mem_info, char *type, size_t *size);

This interface returns the coprocessor memory type (GDDR5 etc.) in
*char *type*. The input value of *size_t *size* is used to ensure
that the *char *type* buffer won't overflow. The behavior is
undetermined if the input size is greater than the actual buffer size, or
if the buffer references a NULL or invalid memory location. The appropriate
*size* value can be queried by setting its value to 0. No more than *size*
bytes will be written to the buffer, including the nul terminator.

This function currently always returns "GDDR5" for the Intel(R) Xeon Phi(TM)
coprocessor x100 product family.

-----------------------------

#### int *mic_get_memory_voltage*(struct mic_device_mem *mem_info, uint32_t *volt);

The current card memory voltage (in volts) is returned in *uint32 *volt*.
This value is internally retrieved from */sys/class/mic/mic<n>/memoryvoltage*
sysfs file on Linux. On Microsoft Windows, the value is internally retrieved
from *memoryvoltage* WMI entry.

-----------------------------

#### int *mic_get_ecc_mode*(struct mic_device_mem *mem_info, uint16_t *ecc);

Return if ECC mode is enabled in the associated Intel(R) Xeon Phi(TM)
coprocessor in *uint16_t *ecc*. A zero value indicates that it is
disabled; a non-zero value indicates that ECC mode is enabled.

-----------------------------

### Processor Information Functions ###

#### int *mic_get_processor_info*(struct mic_device *device, struct mic_processor_info **proc_info);

This interface returns an aggregate of coprocessor related parameters of the
specified Intel(R) Xeon Phi(TM) Coprocessor board inside
*struct mic_processor_info **proc_info* structure, which may be used as a
handle in the accessor functions below to return these parameters. Memory and
resources allocated by this function may be released by a call to
*mic_free_processor_info()*.

Note that the use of a handle from a call to *mic_get_processor_info()* that
either was not successful, has already been freed, or is associated with a
closed coprocessor may result in undetermined behavior.

-----------------------------

#### int *mic_free_processor_info*(struct mic_processor_info *proc_info);

-----------------------------

Use this function to free resources allocated in a previous, successful
*mic_get_processor_info()* call that was not already released.


#### int *mic_get_processor_model*(struct mic_processor_info *proc_info, uint16_t *model, uint16_t *model_ext);

This function returns the processor model and extended model of the associated
coprocessor. On Linux, these values are read from */sys/class/mic/mic<n>/model* and
*/sys/class/mic/mic<n>/model_ext* sysfs entries, and correspond to bits 4-7 and
bits 20-27, respectively, of the result of CPUID instruction with EAX=1 when run
on the coprocessor. On Microsoft Windows, these values are read from WMI entries
*model* and *extended_model*, and correspond to bits 4-7 and bits 20-27, respectively,
of the result of CPUID instruction with EAX=1 when run on the coprocessor.


-----------------------------

#### int *mic_get_processor_family*(struct mic_processor_info *proc_info, uint16_t *family, uint16_t *family_ext);

This function returns the processor model and extended model of the associated
coprocessor. On Linux, these values are read from */sys/class/mic/mic<n>/family_data* and
*/sys/class/mic/mic<n>/extended_family* sysfs entries and correspond to bits
8-11 and bits 16-19, respectively, of the result of the result of CPUID
instruction with EAX=1 when run on the coprocessor. On Microsoft Windows, this function
returns the processor model and extended model of the associated coprocessor. These values
are read from *family_data* and *extended_family* WMI entries and correspond to bits
8-11 and bits 16-19, respectively, of the result of the result of CPUID instruction with
EAX=1 when run on the coprocessor.

-----------------------------

#### int *mic_get_processor_type*(struct mic_processor_info *proc_info, uint16_t *type);

On Linux, this function returns the processor type in *uint16_6 *type that is read
from */sys/class/mic/mic<n>/processor* sysfs entry and corresponds to bits 12-13
of the result of CPUID instruction with EAX=1 when run on the coprocessor.
On Microsoft Windows, this function returns the processor type in *uint16_6 pass:[*]type
that is read from *processor* WMI entry and corresponds to bits 12-13 of the result
of CPUID instruction with EAX=1 when run on the coprocessor.

-----------------------------

#### int *mic_get_processor_steppingid*(struct mic_processor_info *proc_info, uint32_t *id);

The returned *uint32_t *id* constitutes two quantities. On Linux, Bits 4-7 are
returned from */sys/class/mic/mic<n>/stepping_data* and correspond to bits 0-3
of the result of CPUID instruction with EAX=1 run on the coprocessor. The lower
four bits (0-3) are read from */sys/class/mic/mic<n>substepping_data*. On Microsoft
Windows, Bits 4-7 are returned from *stepping_data* WMI entry and correspond to bits 0-3
of the result of CPUID instruction with EAX=1 run on the coprocessor. The lower
four bits (0-3) are read from the *substepping_data* WMI entry.

-----------------------------

#### int *mic_get_processor_stepping*(struct mic_processor_info *proc_info, char *stepping, size_t *size);

This function returns a nul terminated character string representing the
processor stepping and sub-stepping information in *char *stepping. The
input value of *size_t *size* should specify the buffer size referenced
by *char *stepping* including the nul terminator. The behavior is
undetermined if the input size is greater than the actual buffer size,
or if the buffer references a NULL or invalid memory location. The
appropriate *size* value can be queried by setting its value to 0.

-----------------------------

### Coprocessor OS Information Functions ###

#### int *mic_get_cores_info*(struct mic_device *device, struct mic_cores_info **cores_info);

This function uses scif requests to populate the
*struct mic_cores_info **cores* structure that is returned after a
successful call. The accessor functions described below may be used to get the
coprocessor cores related parameters. The card specified by
*struct mic_device *device* must be booted with Linux and scif
communication established with the host for this set of APIs to work.

-----------------------------

#### int *mic_free_cores_info*(struct mic_cores_info *cores_info);

This function frees the resources allocated in a previous successful
*mic_get_cores_info()* call that has not already been freed.

-----------------------------

#### int *mic_get_cores_count*(struct mic_cores_info *cores_info, uint32_t *count);

This function returns the number of active cores in the associated
Intel(R) Xeon Phi(R) coprocessor in *uint32_t *count*.

-----------------------------

#### int *mic_get_cores_voltage*(struct mic_cores_info *cores_info, uint32_t *voltage);

This function returns the cores voltage in the associated Intel(R) Xeon Phi(R)
coprocessor in *uint32_t *voltage*, in uV.

-----------------------------

#### int *mic_get_cores_frequency*(struct mic_cores_info *cores_info, uint32_t *frequency);

This function returns the cores frequency in the associated Intel(R) Xeon Phi(TM)
coprocessor in *uint32_t *voltage* in kHz.

-----------------------------

#### int *mic_alloc_core_util*(struct mic_core_util **cutil);

This function allocates a structure that may be used to get the
Intel(R) Xeon Phi(TM) coprocessor cores utilization date using the
*mic_update_core_util()* API.

-----------------------------

#### int *mic_free_core_util*(struct mic_core_util *cutil);

This function frees the memory allocated by a previous successful
mic_alloc_core_util()* call.

-----------------------------

#### int *mic_update_core_util*(struct mic_device *device, struct mic_core_util *cutil);

This function uses a scif call to populate *struct mic_core_util *cutil*
structure with core utilization information from the Intel(R) Xeon Phi(TM)
coprocessor specified by a valid *struct mic_device *device*. The
*struct mic_core_util *cutil* should have been returned by a previous successful
call to *mic_alloc_core_util()*. This structure may be used as a handle in the
accessor functions described below to get the core utilization information.

Most of this information is available in */proc/stat* on the coprocessor.

-----------------------------

#### int *mic_get_threads_core*(struct mic_core_util *cutil, uint16_t *threads_core);

This function returns the number of threads per core in
*uint16_t *threads_core*. This value is 4 on Intel(R) Xeon Phi(TM)
coprocessors.

....
....

int *mic_get_num_cores*(struct mic_core_util *cutil,
					uint16_t *num_cores); +

This function returns the number of active cores on the associated coprocessor
in *uint16_t *num_cores.

Note that the functions below that return per-thread information return
references to arrays that may be indexed from 0 to
*<number of cores> * <threads per core>*. The calling program is
responsible for allocating such an array before calling the accessor functions.

....
....


int *mic_get_jiffy_counter*(struct mic_core_util *cutil, uint64_t *jiffy); +

This function returns the jiffy value in *uint64_t *jiffy* for the
information that was populated in *struct mic_core_util *cutil* by a
previous successful call to *mic_update_core_util()*. The *jiffy* value
specifies the unit for all the counters returned by the following cores
utilization accessor functions.

....
....


int *mic_get_idle_counters*(struct mic_core_util *cutil,
					uint64_t *idle_counters); +

Upon successful completion, this function returns the idle count (in jiffies)
for each thread running on the coprocessor in a pre-allocated array referenced
by *uint64_t *idle_counters*.

....
....


int *mic_get_nice_counters*(struct mic_core_util *cutil,
					uint64_t *nice_counters); +

This accessor function returns the jiffies spent by processes running at reduced
priority (*nice*) for each thread running on the coprocessor in a pre-allocated
array referenced by *uint64_t *idle_counters*.

....
....


int *mic_get_sys_counters*(struct mic_core_util *cutil,
					uint64_t *sys_counters); +

This function returns the time spent by each thread on the card in system
(kernel) mode inside a pre-allocated array pointed to by
*uint64_t *sys_counters*.

....
....


int *mic_get_user_counters*(struct mic_core_util *cutil,
					uint64_t *user_counters); +

This function returns, inside a pre-allocated array
*uint64_t *user_counters*, the time spent by each thread on the
associated coprocessor in user mode.

....
....


int *mic_get_tick_count*(struct mic_core_util *cutil,
                       		     uint32_t *tick_count); +

This function returns the tick value in *uint32_t *tick_count* for the
information that was populated in *struct mic_core_util *cutil* by a
previous successful call to *mic_update_core_util()*. The *tick* value
is for measuring internal system time.
....
....


int *mic_get_idle_sum*(struct mic_core_util *cutil, uint64_t *idle_sum); +

This function returns the total sum of idle time on all threads in
*uint64_t *idle_sum*.

....
....


int *mic_get_sys_sum*(struct mic_core_util *cutil, uint64_t *sys_sum); +

This function returns the sum aggregate of time spent in system (kernel) mode
on all threads in *uint64_t *sys_num*.

....
....


int *mic_get_nice_sum*(struct mic_core_util *cutil, uint64_t *nice_sum); +

This function returns the sum total of time spent by processes running at
reduced priority on all threads in *uint64_t *nice_num*.

....
....


int *mic_get_user_sum*(struct mic_core_util *cutil, uint64_t *user_sum); +

This function returns, in *uint64_t *user_sum*, the aggregate sum of time
spent in user mode on all threads in the associated coprocessor.

-----------------------------

### Thermal Information Functions [top](#thermal-information-functions) ###

#### int *mic_get_thermal_info*(struct mic_device *device, struct mic_thermal_info **thermal);

This function returns on-board SMC (System Management Controller), and thermal
related information of the specified Intel(R) Xeon Phi(TM) coprocessor inside
*struct mic_processor_info **thermal* structure, which may be used as a
handle in the accessor functions below to return this information. Memory and
resources allocated by this function may be released by a call to
*mic_free_thermal_info()*.

Note that the use of a handle from a call to *mic_get_thermal_info()* that
either was not successful, has already been freed, or is one associated with a
closed coprocessor may result in undetermined behavior.

-----------------------------

#### int *mic_free_thermal_info*(struct mic_thermal_info *thermal);

This function frees the resources allocated by a previous, successful
*mic_get_thermal_info()* call.

-----------------------------

#### int *mic_get_smc_hwrevision*(struct mic_thermal_info *thermal, char *revision, size_t *size);

The SMC hardware revision is returned in the buffer referenced by
*(char *)revision*. The buffer size is passed in the *size_t *size*
argument, and no more than *size* bytes are written into the *revision* buffer,
including the trailing nul character. The format of the output string is:
“%s %s %s %s”, board_type, board_power, hsink_type, mem_config. The behavior
is undetermined if the input size is greater than the actual buffer size, or
if the buffer references a NULL or invalid memory location. The appropriate *size*
value can be queried by setting its value to 0

-----------------------------

#### int *mic_get_smc_fwversion*(struct mic_thermal_info *thermal, char *version, size_t *size);

The SMC firmware version is returned in the buffer referenced by
*(char *)version*. The buffer size is passed in the *size_t *size*
argument, and no more than *size* bytes are written into *version* buffer,
including the trailing nul character. The behavior is undetermined if the
input size is greater than the actual buffer size, or if the buffer
references a NULL or invalid memory location. The appropriate *size* value
can be queried by setting its value to 0.

-----------------------------

#### int *mic_is_smc_boot_loader_ver_supported*(struct mic_thermal_info *thermal, int *supported);

This function returns 0 in *int *supported* if the Boot-loader version is
not supported by the current SMC firmware on the associated
Intel(R) Xeon Phi(TM) coprocessor. Otherwise, it is set to a non-zero value.

-----------------------------

#### int *mic_get_smc_boot_loader_ver*(struct mic_thermal_info *thermal, char *version, size_t *size);

If it is determined from the *mic_is_smc_boot_loader_ver_supported()* call that
the SMC Boot-loader version is supported, then this call may be used to retrieve
the version in the buffer referenced by *char *version*. The input
*size_t *size* value is used to make sure that the *version* buffer will
not overflow. The returned string is always nul terminated. The behavior is
undetermined if the input size is greater than the actual buffer size, or
if the buffer references a NULL or invalid memory location. The appropriate
*size* value can be queried by setting its value to 0.

-----------------------------

#### int *mic_get_fsc_status*(struct mic_thermal_info *thermal, uint32_t *status);

This function returns the Fan Speed Controller status in
*uint32_t *status*. A value of zero indicates that the controller is off.
A non-zero value indicates that it is on.

-----------------------------

#### int *mic_get_die_temp*(struct mic_thermal_info *thermal, uint32_t *temp);

This function returns the Die temperature read from a sensor in
*uint32_t *temp*. The value returned is in degrees Celsius.

-----------------------------

#### int *mic_is_die_temp_valid*(struct mic_thermal_info *thermal, int *valid);

This function returns the Die temperature validity flag read from a sensor in
*uint32_t *valid*. A non-zero value indicates the die temperature is
valid, and zero indicates that it is not.

-----------------------------

#### int *mic_get_gddr_temp*(struct mic_thermal_info *thermal, uint16_t *temp);

This function returns the gddr temperature read from a sensor in
*uint16_t *temp*. The value returned is in degrees Celsius.

-----------------------------

#### int *mic_is_gddr_temp_valid*(struct mic_thermal_info *thermal, int *valid);

This function returns the gddr temperature validity flag read from a sensor in
*uint16_t *valid*. A non-zero value indicates the fan-in temperature is
valid, and zero indicates that it is not.

-----------------------------

#### int *mic_get_fanin_temp*(struct mic_thermal_info *thermal, uint16_t *temp);

This function returns the fan-in temperature read from a sensor in
*uint16_t *temp*. The value returned is in degrees Celsius.

-----------------------------

#### int *mic_is_fanin_temp_valid*(struct mic_thermal_info *thermal, int *valid);

This function returns the fan-in temperature validity flag read from a sensor in
*uint16_t *valid*. A non-zero value indicates the fan-in temperature is
valid, and zero indicates that it is not.

-----------------------------

#### int *mic_get_fanout_temp*(struct mic_thermal_info *thermal, uint16_t *temp);

This function returns the fan-out temperature read from a sensor in
*uint16_t *temp*. The value returned is in degrees Celsius.

-----------------------------

#### int *mic_is_fanout_temp_valid*(struct mic_thermal_info *thermal, int *valid);

This function returns the fan-out temperature validity flag read from a sensor
in *uint16_t *valid*. A non-zero value indicates the fan-out temperature
is valid, and zero indicates that it is not.

-----------------------------

#### int *mic_get_vccp_temp*(struct mic_thermal_info *thermal, uint16_t *temp);

This function returns the vccp (core rail) temperature read from a sensor in
*uint16_t *temp*. The value returned is in degrees Celsius.

-----------------------------

#### int *mic_is_vccp_temp_valid*(struct mic_thermal_info *thermal, int *valid);

This function returns the vccp temperature validity flag read from a sensor in
*uint16_t *valid*. A non-zero value indicates the vccp temperature is
valid, and zero indicates that it is not.

-----------------------------

#### int *mic_get_vddg_temp*(struct mic_thermal_info *thermal, uint16_t *temp);

This function returns the vddg (uncore rail) temperature read from a sensor in
*uint16_t *temp*. The value returned is in degrees Celsius.

-----------------------------

#### int *mic_is_vddg_temp_valid*(struct mic_thermal_info *thermal, int *valid);

This function returns the vddg temperature validity flag read from a sensor in
*uint16_t *valid*. A non-zero value indicates the vddg temperature is
valid, and zero indicates that it is not.

-----------------------------

#### int *mic_get_vddq_temp*(struct mic_thermal_info *thermal, uint16_t *temp);

Return the vddq (memory subsystem rail) temperature read from a sensor in
*uint16_t *temp*. The value returned is in degrees Celsius.

-----------------------------

#### int *mic_is_vddq_temp_valid*(struct mic_thermal_info *thermal, int *valid);

This function returns the vddq temperature validity flag read from a sensor in
*uint16_t *valid*. A non-zero value indicates the vddq temperature is
valid, and zero indicates that it is not.

-----------------------------

#### int *mic_get_fan_rpm*(struct mic_thermal_info *thermal, uint32_t *rpm);

This accessor function returns the Fan RPM in *uint32_t *rpm.

-----------------------------

#### int *mic_get_fan_pwm*(struct mic_thermal_info *thermal, uint32_t *pwm);

This function returns if Fan PWM is enabled in *uint32_t *pwm. A non-zero
value indicates that it is enabled, and a value of zero indicates that it is
not.

-----------------------------

### Other functions ###

////

 TBD

////

....
....


int *mic_get_version_info*(struct mic_device *device,
                         struct mic_version_info **version); +

This function returns the *struct mic_version_info **version* handle that
references an opaque structure containing version information about various
components of the Intel(R) Xeon Phi(TM) coprocessor specified by
*struct mic_device *device*. Memory and resources allocated by this
function may be released by calling *mic_free_version_info()*. Each component's
version is retrieved from the Coprocessor OS using a scif call, so the
associated card must be booted for this set of APIs to function. The individual
components version may be retrieved by the accessor functions described below,
which operate on the handle returned by this function.

Note that the use of a handle from a call to this function that either was not
successful, has already been freed, or is one associated with a closed
coprocessor may result in undefined behavior.

....
....


int *mic_free_version_info*(struct mic_version_info *version); +

This function frees the resources allocated by *mic_get_version_info()*.

....
....


int *mic_get_uos_version*(struct mic_version_info *version, char *uos_version,
                        size_t *size); +

The version of the Linux Operating System running on the associated
Intel(R) Xeon Phi(TM) coprocessor is returned in the buffer referenced by
*(char *)uos_version*. The buffer size is input in the
*size_t *size* argument by the caller. No more than *size* bytes are
written into the *uos_version* buffer, including the trailing nul character.
The behavior is undetermined if the input size is greater than the actual buffer
size, or if the buffer references a NULL or invalid memory location.
The appropriate *size* value can be queried by setting its value to 0.

....
....


int *mic_get_fsc_strap*(struct mic_version_info **version,
			char *strap, size_t *size); +

The buffer size is input in *size_t *size* argument by the caller. No more than
*size* bytes are written into *version* buffer, including the trailing nul
character. The behavior is undetermined if the input size is greater than
the actual buffer size, or if the buffer references a NULL or invalid memory
location. The appropriate *size* value can be queried by setting its value to 0.

....
....


int *mic_get_flash_version*(struct mic_version_info *version,
			char *flash_version, size_t *size); +

The Flash version of the associated Intel(R) Xeon Phi(TM) coprocessor is
returned in the buffer referenced by *(char *)flash_version*. The buffer
size is input in *size_t *size* argument by the caller. No more than
*size* bytes are written into *version* buffer, including the trailing nul
character. The behavior is undetermined if the input size is greater than
the actual buffer size, or if the buffer references a NULL or invalid memory
location. The appropriate *size* value can be queried by setting its value to 0.

Note that this version is retrieved using a scif call and is different from the
*mic_flash_version()* function described above, which returns version
information from a pre-populated Flash header.

-----------------------------

#### int *mic_get_silicon_sku*(struct mic_device *device, char *sku, size_t *size);

The SKU of the Intel(R) Xeon Phi(TM) coprocessor specified by
*struct mic_device *device* is returned in the buffer referenced by
*(char *)sku*. The buffer size is input in the *size_t *size*
argument by the caller. No more than *size* bytes are written into the *sku*
buffer, including the trailing nul character. The behavior is
undetermined if the input size is greater than the actual buffer size, or
if the buffer references a NULL or invalid memory location. The appropriate
*size* value can be queried by setting its value to 0.

--------------------------------

**Serial Number**

#### int *mic_get_serial_number*(struct mic_device *device, char *serial, size_t *size);

The serial number of the Intel(R) Xeon Phi(TM) coprocessor board specified by
*struct mic_device *device* is returned in the buffer referenced by
*(char *)serial*. The buffer size is input in the *size_t *size*
argument by the caller. No more than *size* bytes are written into the *release*
buffer, including the trailing nul character. The behavior is
undetermined if the input size is greater than the actual buffer size, or
if the buffer references a NULL or invalid memory location. The appropriate
*size* value can be queried by setting its value to 0.

-----------------------------

#### int *mic_get_power_utilization_info*(struct mic_device *device, struct mic_power_util_info **power_info);

The Intel(R) Xeon Phi(TM) coprocessor board has many sensors that provide
information about power utilization within different components. These sensors
are available to SMC and the values can be read by Coprocessor OS running on the
coprocessor and communicating with SMC. Therefore, the Coprocessor OS has to be
booted on the card for this set of APIs to function.

This API returns the values and status of power monitoring sensors in
*struct mic_power_util_info **power* structure for the card specified by
*struct mic_device *device*. Resources allocated by this call may be
released by calling *mic_free_power_utilization_info()*. The individual components'
power utilization may be retrieved by calling the accessor functions below.

Please note that the use of a handle from a call to this function that either
was not successful, has already been freed, or is one associated with a closed
coprocessor may result in undefined behavior.


-----------------------------

#### int *mic_free_power_utilization_info*(struct mic_power_util_info *power_info);

This function frees memory allocated by a previous, successful
*mic_get_power_utilization_info()* call.

-----------------------------

#### int *mic_get_total_power_readings_w0*(struct mic_power_util_info *power_info, uint32_t *power);

This function returns the power utilized in Window 0 in *uint32_t *power*
for the associated Intel(R) Xeon Phi(TM) coprocessor. The status of this
register is determined by *mic_get_total_power_sensor_sts_w0()*.

////

TBD

////

-----------------------------

#### int *mic_get_total_power_sensor_sts_w0*(struct mic_power_util_info *power_info, uint32_t *status);

This function returns the status of the Window 0 power utilization register of
the associated Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *status*.

////

TBD

////

-----------------------------

#### int *mic_get_total_power_readings_w1*(struct mic_power_util_info *power_info, uint32_t *power);

This function returns the power utilized in Window 1 in *uint32_t *power*
for the associated Intel(R) Xeon Phi(TM) coprocessor board. The status of this
sensor is determined by *mic_get_total_power_sensor_sts_w1()*.

////

TBD

////

-----------------------------

#### int *mic_get_total_power_sensor_sts_w1*(struct mic_power_util_info *power_info, uint32_t *status);

This function returns the status of the Window 1 power utilization sensor for
the associated Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *status*.

////

TBD

////

-----------------------------

#### int *mic_get_inst_power_readings*(struct mic_power_util_info *power_info, uint32_t *power);

This function returns instantaneous power utilization in
*uint32_t *power* for the associated Intel(R) Xeon Phi(TM) coprocessor.
The status of this sensor is determined by *mic_get_inst_power_sensor_sts()*.

////

TBD

////

-----------------------------

#### int *mic_get_inst_power_sensor_sts*(struct mic_power_util_info *power_info, uint32_t *status);

This function returns the status of the instantaneous power readings register of
the associated Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *status*.

////

TBD

////

....
....


int *mic_get_max_inst_power_readings*(struct mic_power_util_info *power_info,
                                    uint32_t *power); +

////

TBD

////

....
....


int *mic_get_max_inst_power_sensor_sts*(struct mic_power_util_info *power_info,
                                      uint32_t *status); +

////

TBD

////


....
....


int *mic_get_pcie_power_readings*(struct mic_power_util_info *power_info,
                                uint32_t *power); +

This function returns the value from the PCIe power sensor in
*uint32_t *power* for the associated Intel(R) Xeon Phi(TM) coprocessor.
The status of this sensor is determined by *mic_get_pcie_power_sensor_sts()*.

////

TBD

////

....
....


int *mic_get_pcie_power_sensor_sts*(struct mic_power_util_info *power_info,
                                  uint32_t *status); +

This function returns the status of the PCIe power sensor of the associated
Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *status*.

////

TBD

////

....
....


int *mic_get_c2x3_power_readings*(struct mic_power_util_info *power_info,
                                uint32_t *power); +

This function returns the value from 2X3 connector sensor in
*uint32_t *power* for the associated Intel(R) Xeon Phi(TM) coprocessor.
The status of this sensor is determined by *mic_get_c2x3_power_sensor_sts()*.

////

TBD

////

....
....


int *mic_get_c2x3_power_sensor_sts*(struct mic_power_util_info *power_info,
                                  uint32_t *status); +

This function returns the status of the 2X3 power connector sensor of the
associated Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *status*.

////

TBD

////

....
....


int *mic_get_c2x4_power_readings*(struct mic_power_util_info *power_info,
                                uint32_t *power); +

This function returns the value from 2X4 power connector sensor in
*uint32_t *power* for the associated Intel(R) Xeon Phi(TM) coprocessor.
The status of this sensor is determined by *mic_get_c2x4_power_sensor_sts()*.

////

TBD

////

....
....


int *mic_get_c2x4_power_sensor_sts*(struct mic_power_util_info *power_info,
                                  uint32_t *status); +

This function returns the status of the 2X4 power connector sensor of the
associated Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *status*.

////

TBD

////

....
....


int *mic_get_vccp_power_readings*(struct mic_power_util_info *power_info,
                                uint32_t *power); +

This function returns the Vccp power utilization in *uint32_t *power*
for the associated Intel(R) Xeon Phi(TM) coprocessor. The status of this sensor
is determined by *mic_get_vccp_power_sensor_sts()*.

////

TBD

////

....
....


int *mic_get_vccp_power_sensor_sts*(struct mic_power_util_info *power_info,
                                  uint32_t *status); +

This function returns the status of the Vccp power register of the associated
Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *status*.

////

TBD

////

....
....


int *mic_get_vccp_current_readings*(struct mic_power_util_info *power_info,
                                  uint32_t *current); +

This function returns the Vccp current reading in *uint32_t *current* for
the associated Intel(R) Xeon Phi(TM) coprocessor. The status of this sensor is
determined by *mic_get_vccp_current_sensor_sts()*.

////

TBD

////

....
....


int *mic_get_vccp_current_sensor_sts*(struct mic_power_util_info *power_info,
                                    uint32_t *status); +

This function returns the status of the Vccp current sensor of the associated
Intel(R) Xeon Phi(TM) coprocessor is returned in *uint32_t *status*.

////

TBD

////

....
....


int *mic_get_vccp_voltage_readings*(struct mic_power_util_info *power_info,
                                  uint32_t *voltage); +

This function returns the value for Vccp voltage sensor in
*uint32_t *voltage* for the associated Intel(R) Xeon Phi(TM) coprocessor.
The status of this sensor is determined by *mic_get_vccp_voltage_sensor_sts()*.

////

TBD

////

....
....


int *mic_get_vccp_voltage_sensor_sts*(struct mic_power_util_info *power_info,
                                    uint32_t *status); +


This function returns the status of the Vccp voltage sensor of the associated
Intel(R) Xeon Phi(TM) coprocessor is returned in *uint32_t *status*.

////

TBD

////

....
....


int *mic_get_vddg_power_readings*(struct mic_power_util_info *power_info,
                                uint32_t *power); +

This function returns the value for Vddg power register in
*uint32_t *power* for the associated Intel(R) Xeon Phi(TM) coprocessor.
The status of this sensor is determined by *mic_get_vddg_power_sensor_sts()*.

////

TBD

////

....
....


int *mic_get_vddg_power_sensor_sts*(struct mic_power_util_info *power_info,
                                    uint32_t *status); +

This function returns the status of the Vddg power register of the associated
Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *status*.

////

TBD

////

....
....


int *mic_get_vddg_current_readings*(struct mic_power_util_info *power_info,
                                  uint32_t *current); +

This function returns the value for Vddg current sensor in
*uint32_t *current* for the associated Intel(R) Xeon Phi(TM) coprocessor.
The status of this sensor is determined by *mic_get_vddg_current_sensor_sts()*.

////

TBD

////

....
....


int *mic_get_vddg_current_sensor_sts*(struct mic_power_util_info *power_info,
                                    uint32_t *status); +

This function returns the status of the Vddg current sensor of the associated
Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *status*.

////

TBD

////

....
....


int *mic_get_vddg_voltage_readings*(struct mic_power_util_info *power_info,
                                  uint32_t *voltage); +

This function returns the value for Vddg voltage sensor in
*uint32_t *voltage* for the associated Intel(R) Xeon Phi(TM) coprocessor.
The status of this sensor is determined by *mic_get_vddg_voltage_sensor_sts()*.

////

TBD

////

....
....


int *mic_get_vddg_voltage_sensor_sts*(struct mic_power_util_info *power_info,
                                    uint32_t *status); +

This function returns the status of the Vddg voltage sensor of the associated
Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *status*.

////

TBD

////

....
....



int *mic_get_vddq_power_readings*(struct mic_power_util_info *power_info,
                                uint32_t *power); +

This function returns the value for Vddg power register in
*uint32_t *power* for the associated Intel(R) Xeon Phi(TM) coprocessor.
The status of this sensor is determined by *mic_get_vddq_power_sensor_sts()*.

////

TBD

////

....
....


int *mic_get_vddq_power_sensor_sts*(struct mic_power_util_info *power_info,
                                    uint32_t *status); +

This function returns the status of the Vddg power register of the associated
Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *status*.

////

TBD

////

....
....


int *mic_get_vddq_current_readings*(struct mic_power_util_info *power_info,
                                  uint32_t *current); +

This function returns the value for Vddg current sensor in
*uint32_t *current* for the associated Intel(R) Xeon Phi(TM) coprocessor.
The status of this sensor is determined by *mic_get_vddq_current_sensor_sts()*.

////

TBD

////

....
....


int *mic_get_vddq_current_sensor_sts*(struct mic_power_util_info *power_info,
                                    uint32_t *status); +

This function returns the status of the Vddg current sensor of the associated
Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *status*.

////

TBD

////

....
....


int *mic_get_vddq_voltage_readings*(struct mic_power_util_info *power_info,
                                  uint32_t *voltage); +

This function returns the value for Vddg voltage sensor in
*uint32_t *voltage* for the associated Intel(R) Xeon Phi(TM) coprocessor.
The status of this sensor is determined by *mic_get_vddq_voltage_sensor_sts()*.

////

TBD

////

....
....


int *mic_get_vddq_voltage_sensor_sts*(struct mic_power_util_info *power_info,
                                    uint32_t *status); +

This function returns the status of the Vddg voltage sensor of the associated
Intel(R) Xeon Phi(TM) coprocessor is returned in *uint32_t *status*.

////

TBD

////

-----------------------------

### Memory Utilization Functions [top](#memory-utilization) ###

#### int *mic_get_memory_utilization_info*(struct mic_device *device, struct mic_memory_util_info **memory);

This function returns a handle in *struct mic_memory_util_info **memory*
that may be used by accessor functions described below to get memory utilization
information about the Intel(R) Xeon Phi(TM) coprocessor specified by
*struct mic_device *device*. The resources allocated by this function may
be released by calling *mic_free_memory_utilization_info()* function. The behavior of
this function is undefined if *struct mic_device *device* does not point
to an open coprocessor, or if the *memory* argument does not reference valid
memory. The returned handle should be used by the accessor or free functions
below only if this function returned *E_MIC_SUCCESS*.

This function interacts with Linux running on the specified
Intel(R) Xeon Phi(TM) coprocessor and can only be used when the coprocessor is
booted.

Note that all of the functions below that use
*struct mic_memory_util_info *memory* must specify a *memory* handle that
has not already been released, or was not returned by a previous successful call
to *mic_get_pci_config()*; or the associated *struct mic_device *device*
was not subsequently closed. Otherwise, their behavior is undefined.

-----------------------------

#### int *mic_free_memory_utilization_info*(struct mic_memory_util_info *memory);

This function frees resources allocated by a previous
*mic_get_memory_utilization_info()* call.

-----------------------------

#### int *mic_get_total_memory_size*(struct mic_memory_util_info *memory, uint32_t *total_size);

This function returns the total memory size, as known to kernel, running on the
associated Intel(R) Xeon Phi(TM) coprocessor, in *uint32_t *total_size*.

-----------------------------

#### int *mic_get_available_memory_size*(struct mic_memory_util_info *memory, uint32_t *avail_size);

This function returns the size of free memory on the associated
Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *avail_size*.

-----------------------------

#### int *mic_get_memory_buffers_size*(struct mic_memory_util_info *memory, uint32_t *bufs);

This function returns the amount of memory used in kernel buffers on the
associated Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *bufs*.

-----------------------------

#### int *mic_get_power_limit*(struct mic_device *device, struct mic_power_limit **limit);

This function returns a handle in *struct mic_power_limit **limit*. It
may be used by accessor functions described below to get various Power Limit
related parameters about the Intel(R) Xeon Phi(TM) coprocessor specified by
*struct mic_device *device*. The resources allocated by this function may
be released by calling *mic_free_power_limit()*. The behavior of this function
is undefined if *struct mic_device *device* does not point to an open
coprocessor, or if the *limit* argument does not reference valid memory. The
returned handle should be used by the accessor or free functions below only if
this function returned *E_MIC_SUCCESS*.

This function interacts with Linux running on the specified
Intel(R) Xeon Phi(TM) coprocessor, and can only be used when the coprocessor is
booted.

Note that all the functions below that use
*struct mic_power_limit *limit* must specify a *limit* handle that either
has not already been released, was not returned by a previous successful call to
*mic_get_power_limit()*, or its associated *struct mic_device *device*
was not subsequently closed. Otherwise, their behavior is undefined.

////

TBD: Check accessor functions description

////

-----------------------------

#### int *mic_free_power_limit*(struct mic_power_limit *limit);

This function frees memory and resources allocated by a previous, successful
*mic_get_power_limit()* call.

-----------------------------

#### int *mic_get_power_phys_limit*(struct mic_power_limit *limit, uint32_t *phys_lim);

This function returns the physical power limit on the associated
Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *phys_lim*.

-----------------------------

#### int *mic_get_power_hmrk*(struct mic_power_limit *limit, uint32_t *hmrk);

This function returns the highest instantaneous power used on the associated
Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *hmrk*.

-----------------------------

#### int *mic_get_power_lmrk*(struct mic_power_limit *limit, uint32_t *lmrk);

This function returns the lowest instantaneous power used on the associated
Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *lmrk*.

-----------------------------

#### int *mic_get_time_window0*(struct mic_power_limit *limit, uint32_t *time_window);

This function returns the time window level 0 in milliseconds on the associated
Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *time_window*.

-----------------------------

#### int *mic_get_time_window1*(struct mic_power_limit *limit, uint32_t *time_window);

This function returns the time window level 1 in milliseconds on the associated
Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *time_window*.

-----------------------------

#### int *mic_set_power_limit0*(struct mic_device *mdh, uint32_t power, uint32_t time_window);

This function sets the power limit 0. The power limit 0 (hmrk) is set by the
given *uint32_t *power*, and time_window 0 is set by the given
*uint32_t *time_window*. It is recommended that power limit 0 be less
than power limit 1, and time window 0 be greater than time window 1 as well as
a multiple of 50. If time window 0 is not a multiple of 50, it will be rounded
down to the closest 50 decrement. Also, if time window 0 is not greater than
50 and less than 1000 or power limit 0 is less than 50 or greater than 400 then
an E_MIC_RAS_ERROR will be thrown.

-----------------------------

#### int *mic_set_power_limit1*(struct mic_device *mdh, uint32_t power, uint32_t time_window);

This function sets the power limit 1. The power limit 1 (lmrk) is set by the
given *uint32_t *power*, and time_window 1 is set by the given
*uint32_t *time_window*. It is recommended that power limit 1 be greater
than power limit 0, and time window 1 be less than time window 0 as well as
a multiple of 50. If time window 1 is not a multiple of 50, it will be rounded
down to the closest 50 decrement. Also, if time window 1 is not greater than
50 and less than 1000 or power limit 1 is less than 50 or greater than 400 then
an E_MIC_RAS_ERROR will be thrown.

-----------------------------

#### int *mic_get_throttle_state_info*(struct mic_device *device, struct mic_throttle_state_info **ttl_state);

This function returns a handle in
*struct mic_throttle_state_info **ttl_state* that may be used by accessor
functions described below to get Throttle state information about the
Intel(R) Xeon Phi(TM) card specified by *struct mic_device *device*. The
resources allocated by this function may be released by calling the
*mic_free_throttle_state_info()* function. The behavior of this function is
undefined if *struct mic_device *device* does not point to an open
coprocessor, or if the *ttl_state* argument does not reference valid memory. The
returned handle should be used by the accessor or free functions below only if
this function returned *E_MIC_SUCCESS*.

This function interacts with Linux running on the specified
Intel(R) Xeon Phi(TM) coprocessor, and can only be used when the coprocessor is
booted.

Note that all the functions below that use
*struct mic_throttle_state_info *ttl_state* must specify a *ttl_state*
handle that either has not already been released, was not returned by a
previous, successful call to *mic_get_throttle_state_info()*, or its associated
*struct mic_device *device* was not subsequently closed. Otherwise, their
behavior is undefined.

-----------------------------

#### int *mic_free_throttle_state_info*(struct mic_throttle_state_info *ttl_state);

This function frees resources allocated by a previous, successful call to
*mic_get_throttle_state_info()*.

-----------------------------

#### int *mic_get_thermal_ttl_active*(struct mic_throttle_state_info *ttl_state, int *active);

This function returns the state of thermal throttling for the associated
Intel(R) Xeon Phi(TM) coprocessor in *int *active*. A zero value
indicates that it is inactive, and a non-zero value indicates that thermal
throttling is active.

-----------------------------

#### int *mic_get_thermal_ttl_current_len*(struct mic_throttle_state_info *ttl_state, uint32_t *current);

This function returns the duration of the current thermal throttling event in
*uint32_t *current* for the associated Intel(R) Xeon Phi(TM) coprocessor.

-----------------------------

#### int *mic_get_thermal_ttl_count*(struct mic_throttle_state_info *ttl_state, uint32_t *count);

This function returns the total count of thermal throttling events in
*uint32_t *count* for the associated Intel(R) Xeon Phi(TM) coprocessor.

-----------------------------

#### int *mic_get_thermal_ttl_time*(struct mic_throttle_state_info *ttl_state, uint32_t *time);

This function returns the total time spent in thermal throttling events in
*uint32_t *time* for the associated Intel(R) Xeon Phi(TM) coprocessor.

-----------------------------

#### int *mic_get_power_ttl_active*(struct mic_throttle_state_info *ttl_state, int *active);

This function returns the state of power throttling for the associated
Intel(R) Xeon Phi(TM) coprocessor in *int *active*. A zero value
indicates that it is inactive and a non-zero value indicates that power
throttling is active.

-----------------------------

#### int *mic_get_power_ttl_current_len*(struct mic_throttle_state_info *ttl_state, uint32_t *current);

This function returns the duration of the current power throttling event in
*uint32_t *current* for the associated Intel(R) Xeon Phi(TM) coprocessor.

-----------------------------

#### int *mic_get_power_ttl_count*(struct mic_throttle_state_info *ttl_state, uint32_t *count);

This function returns the total time spent in power throttling events in
*uint32_t *time* for the associated Intel(R) Xeon Phi(TM) coprocessor.

-----------------------------

#### int *mic_get_power_ttl_time*(struct mic_throttle_state_info *ttl_state, uint32_t *time);

This function returns the total time spent in power throttling events in
*uint32_t *time* for the associated Intel(R) Xeon Phi(TM) coprocessor.

-----------------------------

#### int *mic_get_turbo_state_info*(struct mic_device *device, struct mic_turbo_info **turbo);

This function returns a handle in *struct mic_turbo_info **turbo* that
may be used by accessor functions described below to get turbo state information
about the Intel(R) Xeon Phi(TM) coprocessor specified by
*struct mic_device *device*. The resources allocated by this function may
be released by calling the *mic_free_turbo_info()* function. The behavior
of this function is undefined if *struct mic_device *device* does not
point to an open coprocessor, or if the *turbo* argument does not reference
valid memory. The returned handle should be used by the accessor or free
functions below only if this function returned *E_MIC_SUCCESS*.

This function interacts with Linux running on the specified
Intel(R) Xeon Phi(TM) coprocessor, and can only be used when the coprocessor is
booted.

Note that all the functions below that use
*struct mic_turbo_info *turbo* must specify a *turbo* handle that
either has not already been released, was not returned by a previous, successful
call to *mic_get_turbo_state_info()*, or its associated
*struct mic_device *device* was not subsequently closed. Otherwise, their
behavior is undefined.

....
....


int *mic_free_turbo_info*(struct mic_turbo_info *turbo);

This function frees resources allocated by a previous, successful call to
*mic_get_turbo_state_info()*.

....
....


int *mic_get_turbo_state*(struct mic_turbo_info *turbo, uint32_t *active);

This function returns the current turbo state of the associated
Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *active*. A non-zero value
indicates that state is active whereas a zero value indicates an inactive turbo
mode.

....
....


int *mic_get_turbo_state_valid*(struct mic_turbo_info *turbo, uint32_t *valid);

This function returns a value in *uint32_t *valid* to indicate whether
the associated Intel(R) Xeon Phi(TM) coprocessor supports turbo mode. A non-zero
value indicates that turbo mode is supported and a zero means that it is not.

....
....


int *mic_get_turbo_mode*(struct mic_turbo_info *turbo, uint32_t *mode);

This function returns the current turbo mode of the associated
Intel(R) Xeon Phi(TM) coprocessor in *uint32_t *mode*. A non-zero value
indicates that turbo mode is enabled; a zero value indicates disabled turbo
mode.

....
....


int *mic_set_turbo_mode*(struct mic_device *device, uint32_t *mode);

This function sets the turbo mode of the associated Intel(R) Xeon Phi(TM)
coprocessor given by *uint32_t *mode*. It accepts either 0 or 1 as input.
0 will disable turbo mode and 1 will enable turbo mode. This function requires
super user permissions, and will otherwise fail with error code 05 from
micras_api.h, which denotes *Failure, privileged command*.

....
....


int *mic_get_led_alert*(struct mic_device *device, uint32_t *led_alert);

This function returns whether LED alert is set in the Intel(R) Xeon Phi(TM)
coprocessor specified by *struct mic_device *device*.
*uint32_t *led_alert* is set to zero if LED alert is not set, while a
non-zero value indicates that LED alert is set.

....
....


int *mic_set_led_alert*(struct mic_device *device, uint32_t *led_alert);

This function sets LED alert in the Intel(R) Xeon Phi(TM) coprocessor specified
by *struct mic_device *device*. *uint32_t *led_alert* is the input
that specifies the value to be set. 1 indicates 'Identify mode' and 0 indicates
'Normal mode'. The valid values for *uint32_t *led_alert* are 0 and 1.

....
....


int *mic_get_uuid*(struct mic_device *device, uint8_t *uuid, size_t *size);

This function returns the UUID of the Intel(R) Xeon Phi(TM) coprocessor in the
character buffer referenced by *char *uuid*. The calling program should
specify the size of the buffer in *size_t *size* and, upon successful
return, it is set to the length of the string returned, including the nul
character. The behavior is undetermined if the input size is greater than
the actual buffer size, or if the buffer references a NULL or invalid
memory location. The appropriate *size* value can be queried by setting
its value to 0.

....
....


int *mic_is_ras_avail*(struct mic_device *device, int *ras_avail);

This function checks whether scif connection with the Intel(R) Xeon Phi(TM)
coprocessor specified by *struct mic_device *device* is available. A zero
value is returned in *int *ras_avail* if scif connection is not
available, while a non-zero value is returned if scif connection is available.

....
....


int *mic_get_smc_persistence_flag*(struct mic_device *device, int *persist_flag);

This function returns whether SMC's persistence flag is set in the Intel(R)
Xeon(TM) coprocessor specified by *struct mic_device *device*.
*uint32_t *persist_flag* is set to zero (0) if persistence is turned OFF,
while a nonzero value indicates that persistence is turned ON. Once this flag
is turned ON, the SMC will retain certain parameters, like Power Limits and
Time Windows, even after a power cycle.

....
....


int *mic_set_smc_persistence_flag*(struct mic_device *device, int persist_flag);

This function sets SMC's Persistence flag in the Intel(R) Xeon Phi(TM)
coprocessor specified by *struct mic_device *device*. *uint32_t
*persist_flag* is the input that specifies the value to be set. One (1)
indicates 'ON' and zero (0) indicates 'OFF'. The valid values for *uint32_t
*persist_flag are zero (0) and one (1).

....
....


int *mic_get_uos_pm_config*(struct mic_device *mdh,
                          struct mic_uos_pm_config **pm_config); +

This function returns a handle in *struct mic_uos_pm_config **pm_config*
that may be used by accessor functions described below to get Coprocessor OS power
management information about the Intel(R) Xeon Phi(TM) coprocessor specified by
*struct mic_device *device*.

The resources allocated by this function may be released by calling the
*mic_free_uos_pm_config()* function. The behavior of this function is undefined
if *struct mic_device *device* does not point to an open coprocessor, or
if the *pm_config* argument does not reference valid memory. The returned handle
should be used by the accessor or free functions below only if this function
returned *E_MIC_SUCCESS*.

Note that all the functions below that use
*struct mic_uos_pm_config *pm_config* must specify a *pm_config* handle
that either has not already been released, was not returned by a previous,
successful call to *mic_get_uos_pm_config()*, or the associated
*struct mic_device *device* was not subsequently closed. Otherwise, their
behavior is undefined.

....
....


int *mic_free_uos_pm_config*(struct mic_uos_pm_config *pm_config); +

This function frees resources allocated by a *mic_get_uos_pm_config()* call.

....
....


int *mic_get_cpufreq_mode*(struct mic_uos_pm_config *pm_config, int *mode);

This function returns the current state of the "cpufreq" (P state) power
management feature of the associated Intel(R) Xeon Phi(TM) coprocessor in
*int *mode*. A non-zero value indicates that the "cpufreq" power
management feature is enabled, whereas a zero value indicates it is disabled.

....
....


int *mic_get_corec6_mode*(struct mic_uos_pm_config *pm_config, int *mode);

This function returns the current state of the "corec6" (Core C6 state) power
management feature of the associated Intel(R) Xeon Phi(TM) coprocessor in
*int *mode*. A non-zero value indicates that the "corec6" power
management feature is enabled, whereas a zero value indicates it is disabled.

....
....


int *mic_get_pc3_mode*(struct mic_uos_pm_config *pm_config, int *mode);

This function returns the current state of the "pc3" (Package C3 state) power
management feature of the associated Intel(R) Xeon Phi(TM) coprocessor in
*int *mode*. A non-zero value indicates that the "pc3" power management
feature is enabled, whereas a zero value indicates it is disabled.

....
....


int *mic_get_pc6_mode*(struct mic_uos_pm_config *pm_config, int *mode);

Return the current state of the "pc6" (Package C6 state) power management
feature of the associated Intel(R) Xeon Phi(TM) coprocessor in
*int *mode*. A non-zero value indicates that the "pc6" power management
feature is enabled, whereas a zero value indicates it is disabled.

....
....

----
----

RETURN VALUE
------------

All *miclib* functions return E_MIC_SUCCESS upon successful completion, whereas
one of the following errors is returned in case of failure. +

*E_MIC_INVAL* +

*E_MIC_ACCESS* +

*E_MIC_NOENT* +

*E_MIC_UNSUPPORTED_DEV* +

*E_MIC_NOT_IMPLEMENTED* +

*E_MIC_DRIVER_INIT* +

*E_MIC_DRIVER_NOT_LOADED* +

*E_MIC_IOCTL_FAILED* +

*E_MIC_ERROR_NOT_FOUND* +

*E_MIC_NOMEM* +

*E_MIC_RANGE* +

*E_MIC_INTERNAL* +

*E_MIC_SYSTEM* +

*E_MIC_SCIF_ERROR* +

*E_MIC_RAS_ERROR* +

*E_MIC_STACK*

Additionally the standard C library variable errno(3) is set to indicate
the system error. The failing *miclib* function may have made many C library
or system calls internally, and the errno(3) from the first failure is
returned to the application. Furthermore, some *miclib* functions may interact
with the RAS module. RAS error numbers can be retrieved with the
*mic_get_ras_errno(void)* function. Finally, the *mic_get_error_string(void)*
function may be called to return the object (and operation) that encountered
the failure. Their use is described in the *EXAMPLES* section.

----------------------------------------------------

## COPYRIGHT ##

Copyright 2012-2015 Intel Corporation. All Rights Reserved.


----------------------------------------------------

Use the following command to create manpage:

```
:!a2x --doctype manpage --format manpage libmicmgmt.7.txt
```

**TBD**: Units of power utilization, memory utilization, power limits, throttling

Check turbo state APIs
