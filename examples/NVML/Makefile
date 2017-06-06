ARCH   := $(shell getconf LONG_BIT)

ifeq (${ARCH},$(filter ${ARCH},32 64))
   NVML_LIB := lib/
else
   $(error Unknown architecture!)
endif

LDFLAGS := -lnvidia-ml -L $(NVML_LIB)

nvml01: nvml01.o
	$(CC) $< $(LDFLAGS) -o $@

clean:
	-@rm -f nvml01.o
	-@rm -f nvml01
