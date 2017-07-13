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
(defn get-val-node ""
  [metric metric-value node instance time-period]
  (query (str "SELECT " metric-value "(value) FROM " metric " WHERE host = '" node "' "
              "AND type_instance = '" instance "' AND time > now() - " time-period ";")))

;; TODO FUNCTION: get-val-node-between-t1t2
(defn get-val-node-between-t1t2 ""
  [metric metric-value node instance t1 t2]
  (query (str "SELECT " metric-value "(value) FROM " metric " WHERE host = '" node "' "
              "AND type_instance = '" instance "' AND time > now() - " t1 ";")))

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
