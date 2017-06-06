(ns rest-api.api-funcs
  (:require [rest-api.common :as com]
            [rest-api.api-resp-parser :as r]
            [rest-api.config :as cfg]
            [rest-api.queries :as q]
            [ring.util.response :as ring-resp]
            [cheshire.core :as cheshire.core]
            [rest-api.logs :as logs]))

;; FUNCTION: gen-response
(defn- gen-response "Executes function f and generates response"
  [txt f]
  (try
    (do
      (logs/log-info txt)
      (ring-resp/response (f)))
    (catch Exception e
      (do
        (logs/log-exception e)
        (ring-resp/response (r/gen-error-response txt e))))))

;; FUNCTION: api-get
(defn api-get "" []
  (gen-response
    "GET /api"
    #(r/gen-resp-map {:version cfg/APP-VERSION
                      :status (str "Rest Api Service running [influxdb:" (cfg/get-influxdb-api) "]...")} "" "")))

;; FUNCTION: get-influxdb-url
(defn get-influxdb-url "" []
  (gen-response "GET /api/influxdb-url" #(r/gen-resp (cfg/get-influxdb-api))))

;; FUNCTION: set-influxdb-url
(defn set-influxdb-url ""
  [body]
  (let [formatted-body (cheshire.core/parse-string (slurp body))]
    (do
      (logs/log-info formatted-body)
      (gen-response "POST /api/influxdb-url" #(r/gen-not-implemented)))))

;; FUNCTION: api-ping
(defn api-ping "" []
  (gen-response "GET /api/ping" #(r/gen-resp-query-headers (com/GET (str (cfg/get-influxdb-api) "/ping")))))

;; FUNCTION: api-test-query
(defn api-test-query "" []
  (gen-response
    "GET /api/test-query"
    #(r/gen-resp-query (com/GET (str (cfg/get-influxdb-api) "/query?db=collectd&q=SHOW MEASUREMENTS")))))

;; FUNCTION: api-query
(defn api-query ""
  [db query]
  (gen-response
    (str "GET /api/query/:db/:query [" db "] [" query "]")
    #(r/gen-resp-query (com/GET (str (cfg/get-influxdb-api) "/query?db=" db "&q=" query)))))

;; FUNCTION: api-query-by-id
(defn api-query-by-id ""
  [id]
  (let [res (q/query id)]
    (if-not (nil? res)
      (let [db    (res :db) 
            query (res :query)]
        (api-query db query))
      (gen-response (str "GET /api/query-by-id/ [" id "]") #(r/gen-error-txt "Query not found")))))
