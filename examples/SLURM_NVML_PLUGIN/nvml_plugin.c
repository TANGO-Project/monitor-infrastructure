/*****************************************************************************\
 *  nvml_plugin.c - slurm energy accounting plugin for nvidia.
 *****************************************************************************
 *  Copyright (C) 2012
 *  Written by Bull- Yiannis Georgiou
 *  CODE-OCEC-09-009. All rights reserved.
 *
 *  This file is part of SLURM, a resource management program.
 *  For details, see <http://slurm.schedmd.com/>.
 *  Please also read the included file: DISCLAIMER.
 *
 *  SLURM is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  In addition, as a special exception, the copyright holders give permission
 *  to link the code of portions of this program with the OpenSSL library under
 *  certain conditions as described in each individual source file, and
 *  distribute linked combinations including the two. You must obey the GNU
 *  General Public License in all respects for all of the code used other than
 *  OpenSSL. If you modify file(s) with this exception, you may extend this
 *  exception to your version of the file(s), but you are not obligated to do
 *  so. If you do not wish to do so, delete this exception statement from your
 *  version.  If you delete this exception statement from all source files in
 *  the program, then also delete it here.
 *
 *  SLURM is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with SLURM; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA.
 *
 *  This file is patterned after jobcomp_linux.c, written by Morris Jette and
 *  Copyright (C) 2002 The Regents of the University of California.
\*****************************************************************************/

/*   nvml_plugin
 * This plugin does not initiate a node-level thread.
 * It will be used to load energy values from NVIDIA GPUs
 * sensors when harware/drivers are available
 */


#include <fcntl.h>
#include <signal.h>
#include "src/common/slurm_xlator.h"
#include "src/common/slurm_acct_gather_energy.h"
#include "src/common/slurm_protocol_api.h"
#include "src/common/slurm_protocol_defs.h"
#include "src/common/fd.h"
#include "src/slurmd/common/proctrack.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#include <unistd.h>
#include <math.h>

/* NVML */
#include "nvml.h"

/* From Linux sys/types.h */
#if defined(__FreeBSD__)
typedef unsigned long int	ulong;
#endif

#define MAX_PKGS        256

#define MSR_RAPL_POWER_UNIT             0x606

/* Package RAPL Domain */
#define MSR_PKG_RAPL_POWER_LIMIT        0x610
#define MSR_PKG_ENERGY_STATUS           0x611
#define MSR_PKG_PERF_STATUS             0x613
#define MSR_PKG_POWER_INFO              0x614

/* DRAM RAPL Domain */
#define MSR_DRAM_POWER_LIMIT            0x618
#define MSR_DRAM_ENERGY_STATUS          0x619
#define MSR_DRAM_PERF_STATUS            0x61B
#define MSR_DRAM_POWER_INFO             0x61C

union {
	uint64_t val;
	struct {
		uint32_t low;
		uint32_t high;
	} i;
} package_energy[MAX_PKGS], dram_energy[MAX_PKGS];

#define _DEBUG 1
#define _DEBUG_ENERGY 1

/*
 * These variables are required by the generic plugin interface.  If they
 * are not found in the plugin, the plugin loader will ignore it.
 *
 * plugin_name - a string giving a human-readable description of the
 * plugin.  There is no maximum length, but the symbol must refer to
 * a valid string.
 *
 * plugin_type - a string suggesting the type of the plugin or its
 * applicability to a particular form of data or method of data handling.
 * If the low-level plugin API is used, the contents of this string are
 * unimportant and may be anything.  SLURM uses the higher-level plugin
 * interface which requires this string to be of the form
 *
 *	<application>/<method>
 *
 * where <application> is a description of the intended application of
 * the plugin (e.g., "jobacct" for SLURM job completion logging) and <method>
 * is a description of how this plugin satisfies that application.  SLURM will
 * only load job completion logging plugins if the plugin_type string has a
 * prefix of "jobacct/".
 *
 * plugin_version - an unsigned 32-bit integer containing the Slurm version
 * (major.minor.micro combined into a single number).
 */
const char plugin_name[] = "NVIDIA plugin";
const char plugin_type[] = "nvidia/data";
const uint32_t plugin_version = SLURM_VERSION_NUMBER;

static acct_gather_energy_t *local_energy = NULL;
static uint64_t debug_flags = 0;

static int dataset_id = -1; /* id of the dataset for profile data */

/* one cpu in the package */
static int pkg2cpu[MAX_PKGS] = {[0 ... MAX_PKGS-1] = -1};
static int pkg_fd[MAX_PKGS] = {[0 ... MAX_PKGS-1] = -1};
static char hostname[MAXHOSTNAMELEN];

static int nb_pkg = 0;



/*
 * init() is called when the plugin is loaded, before any other functions
 * are called. Put global initialization here.
 */
extern int init(void) {
	nvmlReturn_t result;
	
	debug_flags = slurm_get_debug_flags();
	
	gethostname(hostname, MAXHOSTNAMELEN);

	// initialize nvml library
	result = nvmlInit();
	if (NVML_SUCCESS != result) {
		printf("Failed to initialize NVML: %s\n", nvmlErrorString(result));
		return -1;
	}
	
	/* put anything that requires the .conf being read in
	   acct_gather_energy_p_conf_parse */

	return SLURM_SUCCESS;
}


/*
 *
 */
extern int fini(void) {
	nvmlReturn_t result;

	result = nvmlShutdown();
	if (NVML_SUCCESS != result) {
		printf("Failed to shutdown NVML: %s\n", nvmlErrorString(result));
		return -1;
	}
	
	return SLURM_SUCCESS;
}


/*
 *
 */
extern int acct_gather_energy_p_update_node_energy(void) {
	int rc = SLURM_SUCCESS;

	return rc;
}


/*
 *
 */
extern int acct_gather_energy_p_get_data(enum acct_energy_type data_type, void *data) {
	int rc = SLURM_SUCCESS;
	
	return rc;
}


/*
 *
 */
extern int acct_gather_energy_p_set_data(enum acct_energy_type data_type, void *data) {
	int rc = SLURM_SUCCESS;

	return rc;
}


/*
 *
 */
extern void acct_gather_energy_p_conf_options(s_p_options_t **full_options, int *full_options_cnt) {
	
	return;
}


/*
 *
 */
extern void acct_gather_energy_p_conf_set(s_p_hashtbl_t *tbl) {

	return;
}


/*
 *
 */
extern void acct_gather_energy_p_conf_values(List *data) {
	
	return;
}
