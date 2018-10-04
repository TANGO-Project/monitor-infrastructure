;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; This code is being developed for the TANGO Project: http://tango-project.eu
;;
;; Copyright: Roi Sucasas Font, Atos Research and Innovation, 2017.
;;
;; This code is licensed under a GNU General Public License, version 2 license.
;; Please, refer to the LICENSE.TXT file for more information
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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

;; FUNCTION: get-metrics-node
;; type = objects, percent, power
;; instance = 0, 1
;; EXAMPLE: (get-metrics-node "nvidia_value" "ns50.bullx" "0" "2017-08-02T00:00:00Z" "2017-08-03T00:00:00Z")
(defn get-metrics-node ""
  [metric node instance t1 t2]
  (query (str "SELECT last(value), mean(value), min(value), max(value) FROM " metric " WHERE host = '" node "' "
              "AND instance = '" instance "' AND time > '" t1 "' AND time < '" t2 "' GROUP BY type;")))

;; FUNCTION: get-lastval-CPU-PLUGIN
(defn get-lastval-CPU-PLUGIN "Get last value from CPU-PLUGIN and type_instance<>'idle'"
  [metric host]
  (query (str "SELECT last(value) FROM " metric " WHERE host = '" host "' " "AND type_instance<>'idle' GROUP BY instance;")))

;; FUNCTION: get-lastval-NVIDIA-XEONPHI-plugin
(defn get-lastval-NVIDIA-XEONPHI-plugin "Get last value from NVIDIA-PLUGIN"
  [metric host]
  (query (str "SELECT last(value) FROM " metric " WHERE host = '" host "' " "GROUP BY instance, type;")))

;; FUNCTION: get-avgvals-CPU-PLUGIN
(defn get-avgvals-CPU-PLUGIN "Get average values from CPU-PLUGIN and type_instance<>'idle'"
  [metric host t]
  (query (str "SELECT mean(value), max(value), min(value) FROM " metric " WHERE host = '" host "' "
          "AND type_instance<>'idle' AND time > now() - " t " GROUP BY instance;")))
