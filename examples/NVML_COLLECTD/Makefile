# gcc main.c collectd/collectd_client.c nvidia/gpu_nvidia.c -lnvidia-ml -L nvidia/lib
NVML_LIB := nvidia/lib

LDFLAGS := -lnvidia-ml -L $(NVML_LIB)

main: main.o
	gcc main.c collectd/collectd_client.c nvidia/gpu_nvidia.c $(LDFLAGS) -o $@

clean:
	-@rm -f main.o
	-@rm -f main
