(ns rest-api.queries
  (:require [rest-api.common :as common]
            [rest-api.config :as config]
            [rest-api.logs :as logs]
            [clojure.data.json :as json]))

;; FUNCTION: query
(defn- query ""
  [query]
  (try
    (let [response  (json/read-str
                      ((common/GET (str (config/get-influxdb-api) "/query?db=" (config/get-db) "&q=" query)) :body)
                      :key-fn keyword)]
      {:res "SUCCESS" :response response})
    (catch Exception e (do (logs/log-error e) {:res "ERROR" :error e}))))

;; FUNCTION: get-val-node
;; EXAMPLE: (get-val-node "nvidia_value" "max" "ns50.bullx" 0 "5m")
(defn get-val-node ""
  [metric metric-value node instance time-period]
  (query (str "SELECT " metric-value "(value) FROM " metric " WHERE host = '" node "' "
              "AND type = '" instance "' AND time > now() - " time-period ";")))

;; FUNCTION: get-val-node-v2
;; type = objects, percent, power
;; instance = 0, 1
;; EXAMPLE: (get-val-node-v2 "nvidia_value" "ns50.bullx" "0" "2017-08-02T00:00:00Z" "2017-08-03T00:00:00Z")
(defn get-val-node-v2 ""
  [metric node instance t1 t2]
  (query (str "SELECT last(value), mean(value), min(value), max(value) FROM " metric " WHERE host = '" node "' "
              "AND instance = '" instance "' AND time > '" t1 "' AND time < '" t2 "' GROUP BY type;")))

;; FUNCTION: values-get-val-node-v2
(defn values-get-val-node-v2 ""
  [query param]
  (first ((first (filter #(= (get-in % [:tags :type]) param) ((first ((query :response) :results)) :series))) :values)))

;; FUNCTION: get-lastval-CPU-PLUGIN
(defn get-lastval-CPU-PLUGIN "Get last value from CPU-PLUGIN and type_instance<>'idle'"
  [metric host]
  (query (str "SELECT last(value) FROM " metric " WHERE host = '" host "' " "AND type_instance<>'idle' GROUP BY instance;")))

;; FUNCTION: get-lastval-NVIDIA-PLUGIN
(defn get-lastval-NVIDIA-PLUGIN "Get last value from NVIDIA-PLUGIN"
  [metric host]
  (query (str "SELECT last(value) FROM " metric " WHERE host = '" host "' " "GROUP BY type_instance;")))

;; FUNCTION: get-avgvals-CPU-PLUGIN
(defn get-avgvals-CPU-PLUGIN "Get average values from CPU-PLUGIN and type_instance<>'idle'"
  [metric host t]
  (query (str "SELECT mean(value), max(value), min(value) FROM " metric " WHERE host = '" host "' "
          "AND type_instance<>'idle' AND time > now() - " t " GROUP BY instance;")))
