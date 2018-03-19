## Monitor Infrastructure - NVIDIA plugin

This plugin relies in the [NVIDIA Management Library](https://developer.nvidia.com/nvidia-management-library-nvml), a C-based [API](NVML_API_Reference_Guide.pdf) for monitoring and managing various states of the NVIDIA GPU devices. It provides a direct access to the queries and commands exposed via nvidia-smi. The runtime version of NVML ships with the NVIDIA display driver, and the SDK provides the appropriate header, stub libraries and sample applications.
  * **Supported OS platforms:**
    * Windows: Windows Server 2008 R2 64bit, Windows 7 64bit
    * Linux: 32-bit and 64-bit
  * **Supported products:**
    * Full Support
      * NVIDIA Tesla ™Line: S1070, S2050, C1060, C2050/70/75, M2050/70/75/90, X2070/90
      * NVIDIA Quadro ®Line: 4000, 5000, 6000, 7000, M2070-Q
      * NVIDIA GeForce ®Line: None
    * Limited Support
      * NVIDIA Tesla ™Line: None
      * NVIDIA Quadro ®Line: All other current and previous generation Quadro-branded parts
      * NVIDIA GeForce ®Line: All current and previous generation GeForce-branded parts

### Metrics collected

This plugin gets the values from the following metrics:
- power
- temperature
- memory used
- usage
- processes running


### types.db

New entries that need to be added to `types.db`:
- **frequency_gr** 			_value:GAUGE:0:U_
- **frequency_mem**  		_value:GAUGE:0:U_
- **frequency_sm**  		_value:GAUGE:0:U_


## License

  &copy; Atos 2016
