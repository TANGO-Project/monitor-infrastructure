# Monitor Infrastructure

&copy; Atos Spain S.A. 2016

The Monitor Infrastructure is a component of the European Project TANGO (http://tango-project.eu ).

-----------------------

[Component architecture](#component-architecture)

-----------------------

### Description

The responsibility of this component is twofold. On one side it must provide running applications with metrics of the status of different devices, such as CPU, GPUs, FPGAs, etc. This information should be both energy consumption and performance status. On the other side it should also provide historical statistics for device metrics for latter analysis of upper components of the TANGO architecture.

![TANGO architecture](resources/tango-monitor_infr.png)

This component relies on [Collectd](https://collectd.org/), which will be used together with some [custom plugins](Collectd) done specifically for TANGO, in order to get and store energy consumption and performance values from the applications running in the different environments.

Available collection of probes to measure energy in heterogeneous hardware for the TANGO project: (http://tango-project.eu)
1. [NVIDIA collectd plugin](Collectd/nvidia_plugin) :white_check_mark:

2. [Xeon Phi collectd plugin](Collectd/xeon_phi_plugin) (needs to be tested)

3. [IPMI plugin from collectd](Collectd/ipmi_plugin) (needs to be tested)

-----------------------

### Component architecture

![Component architecture](resources/components-monitor_infr.png)
