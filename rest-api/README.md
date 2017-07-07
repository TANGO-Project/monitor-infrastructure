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

## License

  &copy; Atos 2016
