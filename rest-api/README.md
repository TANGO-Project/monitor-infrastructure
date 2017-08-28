# rest-api

&copy; Atos Spain S.A. 2016

The rest-api module is a subcomponent of the Monitor Infrastructure and part of the European Project TANGO (http://tango-project.eu ).

-----------------------

The **REST API** application connects to [INFLUXDB](https://docs.influxdata.com/influxdb) in order to retrieve the values from Collectd.

[INFLUXDB](https://docs.influxdata.com/influxdb/v1.2/guides/querying_data/) returns JSON. The results of your query appear in the "results" array. If an error occurs, InfluxDB sets an "error" key with an explanation of the error.

Authentication in InfluxDB is disabled by default.

## Prerequisites

1. Java version 8

2. [Leiningen][] 2.0.0 or above installed.

[leiningen]: https://github.com/technomancy/leiningen

## Running

A. To start a web server for the application, run:

```bash
lein ring server
```

B. Generate a .war file, and deploy it in a server container

```bash
lein uberwar
```

## API

### API methods:

1. Get REST API status

```
GET "/api"
```

Response:

```json
{"status":"Rest Api Service running [influxdb:http://192.168.252.41:8086]...","message":"","response":"","version":"0.1.0"}
```
-----------------------

2. Get INFLUXDB API url

```
GET "/api/influxdb-url"
```

Response:

```json
{"status":"EXECUTED","message":"","response":"http://192.168.252.41:8086"}
```

-----------------------

3. Set INFLUXDB API url

```
POST "/api/influxdb-url"
```


-----------------------

4. Get INFLUXDB DATABASE (reference to Collectd connection configured previously in InfluxDB)

```
GET "/api/db"
```

-----------------------

5. Set INFLUXDB DATABASE

```
POST "/api/db"
```

-----------------------

6. Ping to service

```
GET "/api/ping"
```

-----------------------

7. Get monitored hosts

```
GET "/monitored/hosts"
```

Response:

```json
{"monitored-hosts": ["ns50.bullx", "ns51.bullx", "ns54.bullx", "ns55.bullx", "ns56.bullx"]}
```

-----------------------

8. Get monitored series (collectd names assigned to the different metrics)

```
GET "/monitored/series"
```

Response:

```json
["cpu_value","monitoring_value","nvidia_value","power_value","xeonphi_value"]
```

-----------------------

9. Get a list of the hosts and the series / metrics monitored in each of these hosts

```
GET "/monitored/hosts-series"
```

Response:

```json
{
  "cpu_value": ["ns50.bullx","ns51","ns51.bullx","ns52.bullx","ns53.bullx","ns54.bullx","ns55.bullx","ns56.bullx","ns57.bullx"],
  "monitoring_value":["ns50.bullx","ns51.bullx"],"nvidia_value":["ns50.bullx","ns51"],
  "power_value":["ns50.bullx","ns51.bullx","ns55.bullx","ns56.bullx","ns57.bullx"],
  "xeonphi_value":["ns52.bullx","ns53.bullx"]
}
```

-----------------------

10. Get the power stats of a host

**Note:** Time parameters should be expressed in Universal Time (UTC).

**Note:** If a metric was not found, it the service returns "NOT FOUND" for this concrete metric.

**Note:** Temperature is expressed in degrees **celsius**, Memory in **MB**, Power in **Watts**, Usage in **%**.

```
GET "/power-stats/:host/:t1/:t2"
```

```
http://localhost:8082/api/power-stats/ns50.bullx/2016-08-02T00:00:00Z/2017-08-28T00:00:00Z
```

Response: Node with NVIDIA GPUs

```json
{
	"power-stats": {
		"ns50.bullx": {
			"cpu_value": "not implemented",
			"gpu0": {
				"power": {
					"mean": 29.773641143756812,
					"min": 29.645999908447266,
					"max": 57.42399978637695
				},
				"usage": {
					"mean": 0,
					"min": 0,
					"max": 0
				},
				"apps": {
					"mean": 0,
					"min": 0,
					"max": 0
				},
				"temperature": {
					"mean": 26.58017891597965,
					"min": 25,
					"max": 28
				},
				"memory": {
					"mean": 0,
					"min": 0,
					"max": 0
				}
			},
			"gpu1": {
				...
			}
		}
	}
}
```

Response: Node with XEON PHI processors

```json
{
	"power-stats": {
		"ns53.bullx": {
			"cpu_value": "not implemented",
			"xeon0": {
				"power": {
					"mean": 84.95901639344262,
					"min": 41,
					"max": 121
				},
				"usage": {
					"mean": "NOT FOUND",
					"min": "NOT FOUND",
					"max": "NOT FOUND"
				},
				"apps": {
					"mean": "NOT FOUND",
					"min": "NOT FOUND",
					"max": "NOT FOUND"
				},
				"temperature": {
					"mean": 47.09836065573771,
					"min": 37,
					"max": 50
				},
				"memory": {
					"mean": 292.89344262295083,
					"min": 292,
					"max": 294
				}
			},
			"xeon1": {
				...
			}
		}
	}
}
```

-----------------------

11. Get stats info of a host

This method returns the (last) values of the different metrics gathered by Collectd (in a host).

**Note:** If a metric was not found, it the service returns "NOT FOUND" for this concrete metric.

**Note:** Temperature is expressed in degrees **celsius**, Memory in **MB**, Power in **Watts**, Usage in **%**.

```
GET "/info-stats/:host"
```

Response: Node with NVIDIA GPUs

```json
{
	"hosts_info": {
		"ns50.bullx": {
			"HAS_GPU": true,
			"CPU_USAGE": {
				"cpu0": {
					"CPU_USAGE_LAST_VALUE": 0.0
				},
				"cpu13": {
					"CPU_USAGE_LAST_VALUE": 0.0
				},
				...
				"cpu1": {
					"CPU_USAGE_LAST_VALUE": 0.0
				}
			},
			"GPU_USED": "NOT_IMPLEMENTED",
			"APPS_ALLOCATED_TO_HOST_COUNT": "NOT_IMPLEMENTED",
			"GPU": {
				"GPU0": {
					"POWER_LAST_VALUE": 29.779,
					"USAGE_LAST_VALUE": 0.0,
					"APPS_RUNNING_LAST_VALUE": 0.0
          "TEMPERATURE_LAST_VALUE": 26.5,
					"MEMORY_LAST_VALUE": 0.0
				},
				"GPU1": {
					"POWER_LAST_VALUE": 30.29,
					"USAGE_LAST_VALUE": 0.0,
					"APPS_RUNNING_LAST_VALUE": 0.0
          "TEMPERATURE_LAST_VALUE": 27.0,
					"MEMORY_LAST_VALUE": 0.0
				}
			},
			"APPS_STATUS": "NOT_IMPLEMENTED",
			"APPS_RUNNING_ON_HOST_COUNT": "NOT_IMPLEMENTED",
			"GPU_NAME": "NVIDIA",
			"GPU_COUNT": 2,
			"HAS_ACCELERATOR": true
		}
	}
}
```

Response: Node with XEON PHI processors

```json
{
	"hosts_info": {
		"ns53.bullx": {
			"HAS_GPU": false,
			"CPU_USAGE": {
				"cpu0": {
					"CPU_USAGE_LAST_VALUE": 0.0
				},
				"cpu13": {
					"CPU_USAGE_LAST_VALUE": 0.0
				},
				...
			},
			"GPU_USED": "NOT_IMPLEMENTED",
			"APPS_ALLOCATED_TO_HOST_COUNT": "NOT_IMPLEMENTED",
			"XEON": {
				"XEON0": {
					"POWER_LAST_VALUE": 97.0,
					"USAGE_LAST_VALUE": "NOT FOUND",
					"TEMPERATURE_LAST_VALUE": 50.0,
					"MEMORY_LAST_VALUE": 295.0
				},
				"XEON1": {
					"POWER_LAST_VALUE": 97.0,
					"USAGE_LAST_VALUE": "NOT FOUND",
					"TEMPERATURE_LAST_VALUE": 45.0,
					"MEMORY_LAST_VALUE": 289.0
				}
			},
			"XEON_COUNT": 2,
			"APPS_STATUS": "NOT_IMPLEMENTED",
			"APPS_RUNNING_ON_HOST_COUNT": "NOT_IMPLEMENTED",
			"GPU_NAME": "",
			"GPU_COUNT": 0,
			"HAS_ACCELERATOR": false
		}
	}
}
```

-----------------------

12. Get stats info of all hosts

This method returns the (last) values of the different metrics gathered by Collectd (in **all** host).

**Note:** If a metric was not found, it the service returns "NOT FOUND" for this concrete metric.

**Note:** Temperature is expressed in degrees **celsius**, Memory in **MB**, Power in **Watts**, Usage in **%**.


```
GET "/info-stats"
```

```json
{
	"monitored_hosts": ["ns50.bullx", "ns51", "ns51.bullx", "ns52.bullx", "ns53.bullx", "ns54.bullx", "ns55.bullx", "ns56.bullx", "ns57.bullx"],
	"hosts_info": {
		"ns50.bullx": {...},
		"ns55.bullx": {...},
		"ns56.bullx": {...},
		"ns51": {
			"HAS_GPU": true,
			"CPU_USAGE": {
				"cpu0": {
					"CPU_USAGE_LAST_VALUE": 0.0
				},
				...
				"cpu1": {
					"CPU_USAGE_LAST_VALUE": 0.0
				}
			},
			"GPU_USED": "NOT_IMPLEMENTED",
			"APPS_ALLOCATED_TO_HOST_COUNT": "NOT_IMPLEMENTED",
			"GPU": {
				"GPU0": {
					"POWER_LAST_VALUE": 30.367,
					"USAGE_LAST_VALUE": 0.0,
					"APPS_RUNNING_LAST_VALUE": 0.0,
					"TEMPERATURE_LAST_VALUE": 26.0,
					"MEMORY_LAST_VALUE": 0.0
				},
				"GPU1": {
					"POWER_LAST_VALUE": 29.889,
					"USAGE_LAST_VALUE": 0.0,
					"APPS_RUNNING_LAST_VALUE": 0.0,
					"TEMPERATURE_LAST_VALUE": 25.0,
					"MEMORY_LAST_VALUE": 0.0
				}
			},
			"APPS_STATUS": "NOT_IMPLEMENTED",
			"APPS_RUNNING_ON_HOST_COUNT": "NOT_IMPLEMENTED",
			"GPU_NAME": "NVIDIA",
			"GPU_COUNT": 0,
			"HAS_ACCELERATOR": true
		},
		"ns54.bullx": {...},
		"ns57.bullx": {...},
		"ns52.bullx": {...},
		"ns51.bullx": {...},
		"ns53.bullx": {...}
	}
}
```

-----------------------

13. Get stats info (with avergares) of a host

**Note:** If a metric was not found, it the service returns "NOT FOUND" for this concrete metric.

**Note:** Temperature is expressed in degrees **celsius**, Memory in **MB**, Power in **Watts**, Usage in **%**.

**Note:** Parameter :t (time) examples:
- take values from last minute

> GET "/info-stats-with-avg/:host/1m"

- take values from last two days

> GET "/info-stats-with-avg/:host/2d"


```
GET "/info-stats-with-avg/:host/:t"
```

```
http://localhost:8082/api/info-stats-with-avg/ns50.bullx/30s
```

Response: Node with NVIDIA GPUs

```json
{
	"monitored_hosts": ["ns50.bullx", "ns51", "ns51.bullx", "ns52.bullx", "ns53.bullx", "ns54.bullx", "ns55.bullx", "ns56.bullx", "ns57.bullx"],
	"hosts_info": {
		"ns50.bullx": {
			"HAS_GPU": true,
			"CPU_USAGE": {
				"cpu0": {
					"CPU_USAGE_LAST_VALUE": 0.0,
					"CPU_USAGE_MEAN_VALUE": 0.004,
					"CPU_USAGE_MAX_VALUE": 0.1,
					"CPU_USAGE_MIN_VALUE": 0.0
				},
				...
				"cpu1": {
					"CPU_USAGE_LAST_VALUE": 0.0,
					"CPU_USAGE_MEAN_VALUE": 0.019,
					"CPU_USAGE_MAX_VALUE": 0.2,
					"CPU_USAGE_MIN_VALUE": 0.0
				}
			},
			"GPU_USED": "NOT_IMPLEMENTED",
			"APPS_ALLOCATED_TO_HOST_COUNT": "NOT_IMPLEMENTED",
			"GPU": {
				"GPU0": {
					"POWER_LAST_VALUE": 29.771,
					"USAGE_LAST_VALUE": 0.0,
					"APPS_RUNNING_LAST_VALUE": 0.0,
					"TEMPERATURE_LAST_VALUE": 27.0,
					"MEMORY_LAST_VALUE": 0.0
				},
				"GPU1": {
					"POWER_LAST_VALUE": 30.29,
					"USAGE_LAST_VALUE": 0.0,
					"APPS_RUNNING_LAST_VALUE": 0.0,
					"TEMPERATURE_LAST_VALUE": 25.0,
					"MEMORY_LAST_VALUE": 0.0
				}
			},
			"APPS_STATUS": "NOT_IMPLEMENTED",
			"APPS_RUNNING_ON_HOST_COUNT": "NOT_IMPLEMENTED",
			"GPU_NAME": "NVIDIA",
			"GPU_COUNT": 0,
			"HAS_ACCELERATOR": true
		}
	}
}
```

Response: Node with XEON PHI processors

```json
{
	"monitored_hosts": ["ns50.bullx", "ns51", "ns51.bullx", "ns52.bullx", "ns53.bullx", "ns54.bullx", "ns55.bullx", "ns56.bullx", "ns57.bullx"],
	"hosts_info": {
		"ns53.bullx": {
			"HAS_GPU": false,
			"CPU_USAGE": {},
			"GPU_USED": "NOT_IMPLEMENTED",
			"APPS_ALLOCATED_TO_HOST_COUNT": "NOT_IMPLEMENTED",
			"XEON": {
				"XEON0": {
					"POWER_LAST_VALUE": 97.0,
					"USAGE_LAST_VALUE": "NOT FOUND",
					"TEMPERATURE_LAST_VALUE": 50.0,
					"MEMORY_LAST_VALUE": 295.0
				},
				"XEON1": {
					"POWER_LAST_VALUE": 97.0,
					"USAGE_LAST_VALUE": "NOT FOUND",
					"TEMPERATURE_LAST_VALUE": 45.0,
					"MEMORY_LAST_VALUE": 289.0
				}
			},
			"XEON_COUNT": 2,
			"APPS_STATUS": "NOT_IMPLEMENTED",
			"APPS_RUNNING_ON_HOST_COUNT": "NOT_IMPLEMENTED",
			"GPU_NAME": "",
			"GPU_COUNT": 0,
			"HAS_ACCELERATOR": false
		}
	}
}
```

-----------------------

14. Get stats info (with avergares) of all hosts

**Note:** If a metric was not found, it the service returns "NOT FOUND" for this concrete metric.

**Note:** Temperature is expressed in degrees **celsius**, Memory in **MB**, Power in **Watts**, Usage in **%**.


```
GET "/info-stats-with-avg"
```

-----------------------

## License

  &copy; Atos 2016
