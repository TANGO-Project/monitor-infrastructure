## Monitor Infrastructure - Tests Examples

&copy; Atos 2016

Collectd plugin for Nvidia GPUs. NOT TESTED !!!!!!

#### Steps to create and compile a Collectd plugin written in C

1. Get [collectd](https://github.com/collectd/collectd.git) source code

2. build.sh

3. ./configure && make

4. Create the program following the instructions from [Plugin architecture](https://collectd.org/wiki/index.php/Plugin_architecture)

5. Compile plugin C program. For example:

    ```
    gcc -DHAVE_CONFIG_H -Wall -Werror -g -O2 -shared -fPIC -Isrc/ -Isrc/daemon/ -lnvidia-ml -L nvidia/lib/ -ldl -o nvidia_plugin.so nvidia/nvidia_plugin.c
	```
