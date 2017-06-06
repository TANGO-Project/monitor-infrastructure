# rest-api

&copy; Atos Spain S.A. 2016

The rest-api module is a subcomponent of the Monitor Infrastructure and part of the European Project TANGO (http://tango-project.eu ).

-----------------------

**REST API** application that connects to [INFLUXDB](https://docs.influxdata.com/influxdb) in order to retrieve the values from Collectd.

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

## About clojure

### Create ring project

```bash
lein new compojure PROJECT_NAME
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

3.

```
POST "/api/influxdb-url"
```


-----------------------

4.

```
GET "/api/ping"
```

-----------------------

5.

```
GET "/api/test-query"
```

-----------------------

6.

```
GET "/api/query/:db/:query"
```

-----------------------

7.

```
GET "/api/query-by-id/:id"
```

-----------------------

### Queries examples:

1. Get values from last hour:

```sql
SELECT value FROM cpu_value where time > now() - 1h limit 100;
```

2. Get max value:

```sql
SELECT MAX(value) FROM cpu_value where time > now() - 1h limit 100;
```

3. Get min value:

```sql
SELECT MIN(value) FROM cpu_value where time > now() - 1h limit 100;
```

## License

  &copy; Atos 2016
