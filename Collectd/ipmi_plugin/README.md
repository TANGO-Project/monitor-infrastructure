## Monitor Infrastructure - IPMI plugin


This plugin relies in the [freeIPMI Library](https://www.gnu.org/software/freeipmi/) for monitoring IPMI devices.


### Configure and compile

Before compiling the plugin, modify the following content to add or remove the metrics you want to collect:

```c++
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
```

Then, execute the following to compile the plugin:

```bash
gcc -o ipmi_plugin ipmi_plugin_v2.c -lipmimonitoring
```

### Metrics collected

This plugin gets the values from the following metrics:
- power (from CPU)

```bash
88  | CPU0 Pwr        | Power Supply            | 134.00     | W     | 'OK'
89  | CPU1 Pwr        | Power Supply            | 66.00      | W     | 'OK'
90  | CPU0 DIM01 Pwr  | Power Supply            | 5.00       | W     | 'OK'
91  | CPU0 DIM23 Pwr  | Power Supply            | 4.00       | W     | 'OK'
92  | CPU1 DIM01 Pwr  | Power Supply            | 5.00       | W     | 'OK'
93  | CPU1 DIM23 Pwr  | Power Supply            | 4.00       | W     | 'OK'
94  | Blade Pwr       | Power Supply            | 246.00     | W     | 'OK'
```

## License

&copy; Atos 2017
