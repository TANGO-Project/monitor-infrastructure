;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; This code is being developed for the TANGO Project: http://tango-project.eu
;;
;; Copyright: Roi Sucasas Font, Atos Research and Innovation, 2017.
;;
;; This code is licensed under an Apache 2.0 license. Please, refer to the
;; LICENSE.TXT file for more information
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(ns rest-api.api-funcs
  (:use [clojure.string :only [index-of]])
  (:require [rest-api.common :as common]
            [rest-api.api-resp-parser :as r]
            [rest-api.config :as cfg]
            [rest-api.queries :as queries]
            [ring.util.response :as ring-resp]
            [cheshire.core :as cheshire.core]
            [rest-api.logs :as logs]
            [clojure.data.json :as json]))

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

;; FUNCTION: get-db
(defn get-db "" []
  (gen-response "GET /api/db" #(r/gen-resp (cfg/get-db))))

;; FUNCTION: set-db
(defn set-db ""
  [body]
  (let [formatted-body (cheshire.core/parse-string (slurp body))]
    (do
      (logs/log-info formatted-body)
      (gen-response "POST /api/db" #(r/gen-not-implemented)))))

;; FUNCTION: api-ping
(defn api-ping "" []
  (gen-response "GET /api/ping" #(r/gen-resp-query-headers (common/GET (str (cfg/get-influxdb-api) "/ping")))))

;; FUNCTION: get-map-body
(defn- get-map-body ""
  [query]
  (try
    ((common/GET (str (cfg/get-influxdb-api) "/query?db=" (cfg/get-db) "&q=" query)) :body)
    (catch Exception e (do (logs/log-exception e) nil))))

;; FUNCTION: get-monitored-hosts
;;  EXAMPLE: (get-monitored-hosts)
(defn get-monitored-hosts "Gets a list of all hosts monitored by Collectd / InfluxDB"
  []
  (if-let [m (get-map-body "SHOW TAG VALUES WITH KEY=host")]
    (let [results (json/read-str m :key-fn keyword)]
      (vec (distinct
        (reduce #(concat %1 %2) '() (for [x ((first (results :results)) :series)]
                                      (for [hosts (x :values)] (second hosts)))))))
    (do (logs/log-error "Error parsing restuls from Hosts") [])))

;; FUNCTION: get-monitored-series
;;  EXAMPLE: (get-monitored-series)
(defn get-monitored-series "Gets a list of all series names monitored by Collectd / InfluxDB"
  []
  (if-let [m (get-map-body "SHOW TAG VALUES WITH KEY=host")]
    (let [results (json/read-str m :key-fn keyword)]
      (vec (for [x ((first (results :results)) :series)] (x :name))))
    (do (logs/log-error "Error parsing restuls from Hosts") [])))

;; FUNCTION: get-query-result
;;  EXAMPLE: (get-query-result "SHOW TAG VALUES WITH KEY=host")
(defn- get-query-result ""
  [query]
  (try
    (let [response  (json/read-str ((common/GET (str (cfg/get-influxdb-api) "/query?db=" (cfg/get-db) "&q=" query)) :body) :key-fn keyword)]
      {:res "SUCCESS" :response response})
    (catch Exception e (do (logs/log-error e) {:res "ERROR" :error e}))))

;; FUNCTION: get-monitored-series-hosts
;;  EXAMPLE: (get-monitored-series-hosts)
(defn get-monitored-series-hosts ""
  []
  (let [res (get-query-result "SHOW TAG VALUES WITH KEY=host")]
    (if (= "SUCCESS" (res :res))
      (into {}
        (for [x ((first (get-in res [:response :results])) :series)]
           {(x :name) (vec (for [hosts (x :values)] (second hosts)))}))
      res)))

;; FUNCTION: get-total-intances
(defn- get-total-intances ""
  [k v]
  (cond
    (= k "cpu_value")   (for [x v]
                          {x (let [res (get-query-result (str "SHOW SERIES FROM " k " WHERE host='" x "' AND type_instance = 'idle'"))]
                                (count ((first ((first (get-in res [:response :results])) :series)) :values)))})
    :else               (for [x v]
                          {x (let [res (get-query-result (str "SHOW SERIES FROM " k " WHERE host='" x "'"))]
                                (count ((first ((first (get-in res [:response :results])) :series)) :values)))})))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; MAPS

;; LIST OF MONITORED HOSTS
(def MONITORED-HOSTS
  (delay (get-monitored-hosts)))

(defn get-MONITORED-HOSTS ""
  []
  (ring-resp/response (deref MONITORED-HOSTS)))

;; MONITORED TAGS
(def MONITORED-SERIES
  (delay (get-monitored-series)))

(defn get-MONITORED-SERIES ""
  []
  (ring-resp/response (deref MONITORED-SERIES)))

;; MONITORED TAGS & HOSTS
(def MONITORED-HOSTS-SERIES
  (delay (get-monitored-series-hosts)))

(defn get-MONITORED-HOSTS-SERIES ""
  []
  (ring-resp/response (deref MONITORED-HOSTS-SERIES)))

;; NUMBER OF INSTANCES PER HOST & TAG
(def HOSTS-TAGS-INSTANCES
  (delay (into {}
    (for [[k v] @MONITORED-HOSTS-SERIES]
      {k (into {} (get-total-intances k v))}))))

;(get-in HOSTS-TAGS-INSTANCES ["cpu_value" "ns50.bullx"])

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; PROGRAMMING MODEL DATA:
;;    Example:    getPowerStats("ns2", initTime, endTime)
;;    Response:   power stats {
;;                  node =ns2,
;;                  global =[max=130, min=115, mean=126],
;;                  cpu1 =[max=20, min=15, mean=17],
;;                  ...
;;                  gpu1 =[max=5, min=3, mean=4]
;;                  ...
;;                }

;; FUNCTION: get-val-node
;;  Example: (get-val-node "monitoring_value" "max" "ns50.bullx" "1" "120d")
(defn- get-val-node ""
  [metric metric-value node instance time-period]
  (try
    (let [response  (json/read-str
                      ((let [query  (str "SELECT " metric-value "(value) FROM " metric " WHERE host = '" node "' "
                                         "AND type_instance = '" instance "' AND time > now() - " time-period ";")]
                        (common/GET (str (cfg/get-influxdb-api) "/query?db=" (cfg/get-db) "&q=" query))) :body)
                      :key-fn keyword)]
      {:res "SUCCESS" :response response})
    (catch Exception e (do (logs/log-error e) {:res "ERROR" :error e}))))

;; FUNCTION: process-response
(defn- process-response ""
  [r]
  (second (first ((first ((first ((r :response) :results)) :series)) :values))))

;; FUNCTION: extract-result
(defn- extract-result ""
  [metric host instance type time]
  (let [res (get-val-node metric type host instance time)]
    (if (= "SUCCESS" (res :res))
      (process-response res)
      nil)))

;; FUNCTION: get-power-stats
;;    EXAMPLE: (get-power-stats "ns50.bullx" nil nil)
(defn get-power-stats ""
  [host time endTime]
  (ring-resp/response
    (try
      (let [metrics-for-host  (remove nil?
                                (for [[k v] @MONITORED-HOSTS-SERIES]
                                  (when (some #(= host %) v) k)))
            instances-per-metric  (into {}
                                    (for [x metrics-for-host]
                                      {x (get-in @HOSTS-TAGS-INSTANCES [x host])}))]
        {:power-stats
          { host
            (into {}
              (remove nil?
                (for [[k v] instances-per-metric]
                  (cond
                    (= k "monitoring_value")  (into {}
                                                (for [i (range 0 v)]
                                                  { (keyword (str "gpu" i))
                                                    [:max   (extract-result k host i "max" time)
                                                     :min   (extract-result k host i "min" time)
                                                     :mean  (extract-result k host i "mean" time)]}))
                    :else                      {k "not implemented"}))))}})
      (catch Exception e (do (logs/log-error e) {:res "ERROR" :error e})))))


; (get-in HOSTS-TAGS-INSTANCES ["cpu_value" "ns50.bullx"])


;(get-power-stats "ns50.bullx" "120d" nil)


;{:power-stats
;  (let [host "ns50.bullx"]
;    { host
;      (into {}
;        (remove nil?
;          (for [[k v] instances-per-metric]
;            (when (= k "monitoring_value")
;              (into {}
;                (for [i (range 0 v)]
;                  { (keyword (str "gpu" i))
;                    [:max (extract-result k host i "max")
;                     :min (extract-result k host i "min")
;                     :mean (extract-result k host i "mean")]}))))))})}


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; DEPRECATED:

;; FUNCTION: api-test-query
(defn api-test-query "" []
  (gen-response
    "GET /api/test-query"
    #(r/gen-resp-query (common/GET (str (cfg/get-influxdb-api) "/query?db=collectd&q=SHOW MEASUREMENTS")))))

;; FUNCTION: api-query
(defn api-query ""
  [db query]
  (gen-response
    (str "GET /api/query/:db/:query [" db "] [" query "]")
    #(r/gen-resp-query (common/GET (str (cfg/get-influxdb-api) "/query?db=" db "&q=" query)))))

;; FUNCTION: api-query-by-id
(defn api-query-by-id ""
  [id]
  (let [res (queries/query id)]
    (if-not (nil? res)
      (let [db    (res :db)
            query (res :query)]
        (api-query db query))
      (gen-response (str "GET /api/query-by-id/ [" id "]") #(r/gen-error-txt "Query not found")))))
