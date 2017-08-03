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

-----------------------

9. Get a list of the hosts and the series / metrics monitored in each of these hosts

```
GET "/monitored/hosts-series"
```

-----------------------

10. Get the power stats of a host

```
GET "/power-stats/:host/:time"
```

Response:

```json
{
  "power-stats": {
    "ns50.bullx": {
      "gpu0": ["max", 132.07000732421875, "min", 18.545000076293945, "mean", 20.290325199615207],
      "gpu1": ["max", 126.88400268554688, "min", 18.731000900268555, "mean", 20.317622574830658],
    }
  }
}

```

-----------------------

11. Get the power stats of a host

```
GET "/power-stats/:host/:t1/:t2"
```

```
http://localhost:8082/api/power-stats/ns50.bullx/2016-08-02T00:00:00Z/2017-08-03T00:00:00Z
```

Response:

```json
{
	"power-stats": {
		"ns50.bullx": {
			"cpu_value": "not implemented",
			"gpu0": {
				"power": {
					"mean": 29.79611899425668,
					"min": 29.663000106811523,
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
				}
			},
			"gpu1": {
				"power": {
					"mean": 30.316579741364194,
					"min": 30.19300079345703,
					"max": 61.058998107910156
				},
				"usage": {
					"mean": 0.014404852160727824,
					"min": 0,
					"max": 76
				},
				"apps": {
					"mean": 0,
					"min": 0,
					"max": 0
				}
			}
		}
	}
}
```

-----------------------

12. Get stats info of a host

```
GET "/info-stats/:host"
```

Response:

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
				},
				"GPU1": {
					"POWER_LAST_VALUE": 30.29,
					"USAGE_LAST_VALUE": 0.0,
					"APPS_RUNNING_LAST_VALUE": 0.0
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

-----------------------

13. Get stats info of all hosts

```
GET "/info-stats"
```

-----------------------

14. Get stats info (with avergares) of a host

```
GET "/info-stats-with-avg/:host/:t"
```

```
http://localhost:8082/api/info-stats-with-avg/ns50.bullx/30s
```

Response:

```json
{
	"monitored_hosts": ["ns50.bullx", "ns51.bullx", "ns52.bullx", "ns53.bullx", "ns54.bullx", "ns55.bullx", "ns56.bullx", "ns57.bullx"],
	"hosts_info": {
		"ns50.bullx": {
			"HAS_GPU": true,
			"CPU_USAGE": {
				"cpu0": {
					"CPU_USAGE_LAST_VALUE": 0.0,
					"CPU_USAGE_MEAN_VALUE": 0.0,
					"CPU_USAGE_MAX_VALUE": 0.0,
					"CPU_USAGE_MIN_VALUE": 0.0
				},
				...
				"cpu4": {
					"CPU_USAGE_LAST_VALUE": 0.0,
					"CPU_USAGE_MEAN_VALUE": 0.007,
					"CPU_USAGE_MAX_VALUE": 0.1,
					"CPU_USAGE_MIN_VALUE": 0.0
				},
				"cpu11": {
					"CPU_USAGE_LAST_VALUE": 0.0,
					"CPU_USAGE_MEAN_VALUE": 0.014,
					"CPU_USAGE_MAX_VALUE": 0.1,
					"CPU_USAGE_MIN_VALUE": 0.0
				},
				"cpu6": {
					"CPU_USAGE_LAST_VALUE": 0.0,
					"CPU_USAGE_MEAN_VALUE": 0.014,
					"CPU_USAGE_MAX_VALUE": 0.1,
					"CPU_USAGE_MIN_VALUE": 0.0
				},
				...
				"cpu1": {
					"CPU_USAGE_LAST_VALUE": 0.0,
					"CPU_USAGE_MEAN_VALUE": 0.014,
					"CPU_USAGE_MAX_VALUE": 0.1,
					"CPU_USAGE_MIN_VALUE": 0.0
				}
			},
			"GPU_USED": "NOT_IMPLEMENTED",
			"APPS_ALLOCATED_TO_HOST_COUNT": "NOT_IMPLEMENTED",
			"GPU": {
				"GPU0": {
					"POWER_LAST_VALUE": 29.77,
					"USAGE_LAST_VALUE": 0.0,
					"APPS_RUNNING_LAST_VALUE": 0.0
				},
				"GPU1": {
					"POWER_LAST_VALUE": 30.29,
					"USAGE_LAST_VALUE": 0.0,
					"APPS_RUNNING_LAST_VALUE": 0.0
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

-----------------------

15. Get stats info (with avergares) of all hosts

```
GET "/info-stats-with-avg"
```

-----------------------

## License

  &copy; Atos 2016
