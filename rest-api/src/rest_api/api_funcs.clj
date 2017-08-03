;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; This code is being developed for the TANGO Project: http://tango-project.eu
;;
;; Copyright: Roi Sucasas Font, Atos Research and Innovation, 2017.
;;
;; This code is licensed under an Apache 2.0 license. Please, refer to the
;; LICENSE.TXT file for more information
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(ns rest-api.api-funcs
  (:require [rest-api.common :as common]
            [rest-api.api-resp-parser :as api-resp-parser]
            [rest-api.config :as config]
            [ring.util.response :as ring-resp]
            [cheshire.core :as cheshire.core]
            [rest-api.logs :as logs]
            [clojure.data.json :as json]
            [clj-time.core :as t]
            [clj-time.local :as l]
            [rest-api.queries :as queries]))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; FUNCTION: gen-response
(defn- gen-response "Executes function f and generates response"
  [txt f]
  (try
    (do
      (logs/log-debug txt)
      (ring-resp/response (f)))
    (catch Exception e
      (do
        (logs/log-exception e)
        (ring-resp/response (api-resp-parser/gen-error-response txt e))))))

;; FUNCTION: api-get
(defn api-get ""
  []
  (gen-response
    "GET /api"
    #(api-resp-parser/gen-resp-map {:version config/APP-VERSION
                                    :status (str "Rest Api Service running [influxdb:" (config/get-influxdb-api) "]...")} "" "")))

;; FUNCTION: get-influxdb-url
(defn get-influxdb-url "" [] (gen-response "GET /api/influxdb-url" #(api-resp-parser/gen-resp (config/get-influxdb-api))))

;; FUNCTION: set-influxdb-url
(defn set-influxdb-url ""
  [body]
  (let [formatted-body (cheshire.core/parse-string (slurp body))]
    (do
      (logs/log-debug formatted-body)
      (gen-response "POST /api/influxdb-url" #(api-resp-parser/gen-not-implemented)))))

;; FUNCTION: get-db
(defn get-db ""  [] (gen-response "GET /api/db" #(api-resp-parser/gen-resp (config/get-db))))

;; FUNCTION: set-db
(defn set-db ""
  [body]
  (let [formatted-body (cheshire.core/parse-string (slurp body))]
    (do
      (logs/log-debug formatted-body)
      (gen-response "POST /api/db" #(api-resp-parser/gen-not-implemented)))))

;; FUNCTION: api-ping
(defn api-ping ""
  []
  (gen-response "GET /api/ping" #(api-resp-parser/gen-resp-query-headers (common/GET (str (config/get-influxdb-api) "/ping")))))

;; FUNCTION: get-map-body
(defn- get-map-body ""
  [query]
  (try
    ((common/GET (str (config/get-influxdb-api) "/query?db=" (config/get-db) "&q=" query)) :body)
    (catch Exception e (do (logs/log-exception e) nil))))

;; FUNCTION: get-monitored-hosts
;;  EXAMPLE: (get-monitored-hosts)
(defn- get-monitored-hosts "Gets a list of all hosts monitored by Collectd / InfluxDB"
  []
  (if-let [m (get-map-body "SHOW TAG VALUES WITH KEY=host")]
    (let [results (json/read-str m :key-fn keyword)]
      (vec (distinct
        (reduce #(concat %1 %2) '() (for [x ((first (results :results)) :series)]
                                      (for [hosts (x :values)] (second hosts)))))))
    (do (logs/log-error "Error parsing restuls from Hosts") [])))

;; FUNCTION: get-monitored-series
;;  EXAMPLE: (get-monitored-series)
(defn- get-monitored-series "Gets a list of all series names monitored by Collectd / InfluxDB"
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
    (let [response  (json/read-str ((common/GET (str (config/get-influxdb-api) "/query?db=" (config/get-db) "&q=" query)) :body) :key-fn keyword)]
      {:res "SUCCESS" :response response})
    (catch Exception e (do (logs/log-error e) {:res "ERROR" :error e}))))

;; FUNCTION: get-monitored-series-hosts
;;  EXAMPLE: (get-monitored-series-hosts)
(defn- get-monitored-series-hosts ""
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
    (= k config/SERIE-CPU-PLUGIN) (for [x v]
                                    {x (let [res (get-query-result (str "SHOW SERIES FROM " k " WHERE host='" x "' AND type_instance = 'idle'"))]
                                          (count ((first ((first (get-in res [:response :results])) :series)) :values)))})
    :else                         (for [x v]
                                    {x (let [res (get-query-result (str "SHOW SERIES FROM " k " WHERE host='" x "'"))]
                                          (count ((first ((first (get-in res [:response :results])) :series)) :values)))})))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; MAPS: information about monitored hosts and metrics (from Collectd)
;;  'delay' waits until call to execute code

;; LIST OF MONITORED HOSTS
(def ^:private MONITORED-HOSTS (delay (get-monitored-hosts)))
(defn get-MONITORED-HOSTS "" [] (ring-resp/response {:monitored-hosts (deref MONITORED-HOSTS)}))

;; MONITORED TAGS
(def ^:private MONITORED-SERIES (delay (get-monitored-series)))
(defn get-MONITORED-SERIES "" [] (ring-resp/response (deref MONITORED-SERIES)))

;; MONITORED TAGS & HOSTS
(def ^:private MONITORED-HOSTS-SERIES (delay (get-monitored-series-hosts)))
(defn get-MONITORED-HOSTS-SERIES "" [] (ring-resp/response (deref MONITORED-HOSTS-SERIES)))
;; get-MONITORED-HOSTS-SERIES result example:
;;    {:status 200, :headers {}, :body {
;;        "cpu_value" ["ns50.bullx" "ns51.bullx" "ns52.bullx" "ns53.bullx" "ns54.bullx" "ns55.bullx" "ns56.bullx" "ns57.bullx"],
;;        "monitoring_value" ["ns50.bullx" "ns51.bullx"],
;;        "nvidia_value" ["ns50.bullx"],
;;        "power_value" ["ns50.bullx" "ns51.bullx" "ns55.bullx" "ns56.bullx"]}}

;; NUMBER OF INSTANCES PER HOST & TAG
(def ^:private HOSTS-TAGS-INSTANCES
  (delay (into {}
    (for [[k v] @MONITORED-HOSTS-SERIES]
      {k (into {} (get-total-intances k v))}))))
;; (deref HOSTS-TAGS-INSTANCES) result example:
;; {"cpu_value" {"ns50.bullx" 16,
;;               "ns55.bullx" 48,
;;               "ns56.bullx" 48,
;;               "ns54.bullx" 48,
;;               "ns57.bullx" 48,
;;               "ns52.bullx" 16,
;;               "ns51.bullx" 16,
;;               "ns53.bullx" 16},
;;    "monitoring_value" {"ns50.bullx" 2, "ns51.bullx" 2},
;;    "nvidia_value" {"ns50.bullx" 6},
;;    "power_value" {"ns50.bullx" 1,
;;                   "ns55.bullx" 1,
;;                   "ns56.bullx" 1,
;;                   "ns51.bullx" 1}}


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; |PROGRAMMING MODEL| - DATA:
;;
;,      GET  "/power-stats/:h/:t"
;;      GET  "/power-stats/:h/:t1/:t2"
;;
;;    Example:    http://localhost:8082/api/power-stats/ns50.bullx/2016-08-02T00:00:00Z/2017-08-03T00:00:00Z
;;                GET /power-stats/:host/:t1/:t2
;;    Response:
;;    {
;;    	"power-stats": {
;;    		"ns50.bullx": {
;;    			"cpu_value": "not implemented",
;;    			"gpu0": {
;;    				"power": {
;;    					"mean": 29.79611899425668,
;;    					"min": 29.663000106811523,
;;    					"max": 57.42399978637695
;;    				},
;;    				"usage": {
;;    					"mean": 0,
;;    					"min": 0,
;;    					"max": 0
;;    				},
;;    				"apps": {
;;    					"mean": 0,
;;    					"min": 0,
;;    					"max": 0
;;    				}
;;    			},
;;    			"gpu1": {
;;    				"power": {
;;    					"mean": 30.316579741364194,
;;    					"min": 30.19300079345703,
;;    					"max": 61.058998107910156
;;    				},
;;    				"usage": {
;;    					"mean": 0.014404852160727824,
;;    					"min": 0,
;;    					"max": 76
;;    				},
;;    				"apps": {
;;    					"mean": 0,
;;    					"min": 0,
;;    					"max": 0
;;    				}
;;    			}
;;    		}
;;    	}
;;    }
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; FUNCTION: process-response
(defn- process-response "" [r] (second (first ((first ((first ((r :response) :results)) :series)) :values))))

;; FUNCTION: extract-result
;; EXAMPLE: (extract-result "nvidia_value" "ns50.bullx" 0 "max" "5m")
(defn- extract-result ""
  [metric host instance type time]
  (let [res (queries/get-val-node metric type host instance time)]
    (if (= "SUCCESS" (res :res))
      (process-response res)
      nil)))

;; FUNCTION: get-power-stats
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
                    ;; NVIDIA probes
                    ;;    "nvidia_value" {"ns50.bullx" 6} ==> 6 / 3 (power, percent, objects): defined in (config/SERIE-NVIDIA-GPUs-total-metrics)
                    (= k config/SERIE-NVIDIA-GPUs)  (into {}
                                                      (for [i (range 0 (/ v config/SERIE-NVIDIA-GPUs-total-metrics))]
                                                        { (keyword (str "gpu" i))
                                                          [:max   (extract-result k host i "max" time)
                                                           :min   (extract-result k host i "min" time)
                                                           :mean  (extract-result k host i "mean" time)]}))
                    ;; Others: not implemented
                    :else                           {k "not implemented"}))))}})
      (catch Exception e (do (logs/log-error e) {:res "ERROR" :error e})))))

;; FUNCTION: get-power-stats-v2
;; EXAMPLE: (get-power-stats-v2 "ns50.bullx" "2016-08-02T00:00:00Z" "2017-08-03T00:00:00Z")
(defn get-power-stats-v2 ""
  [host t1 t2]
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
                    ;; NVIDIA probes
                    (= k config/SERIE-NVIDIA-GPUs)  (into {}
                                                      (for [i (range 0 (/ v config/SERIE-NVIDIA-GPUs-total-metrics))]
                                                        { (keyword (str "gpu" i))
                                                          (let [query (queries/get-val-node-v2 k host i t1 t2)
                                                                power-values (queries/values-get-val-node-v2 query "power")
                                                                usage-values (queries/values-get-val-node-v2 query "percent")
                                                                apps-values (queries/values-get-val-node-v2 query "objects")]
                                                            {:power
                                                              {:mean (nth power-values 2)
                                                               :min (nth power-values 3)
                                                               :max (last power-values)}
                                                             :usage
                                                              {:mean (nth usage-values 2)
                                                               :min (nth usage-values 3)
                                                               :max (last usage-values)}
                                                             :apps
                                                              {:mean (nth apps-values 2)
                                                               :min (nth apps-values 3)
                                                               :max (last apps-values)}
                                                            })}))
                    ;;
                    (= k config/SERIE-CPU-PLUGIN)   {k "not implemented"}

                    ;; Others: not implemented
                    ;:else                           {k "not implemented"}
                  ))))}})
      (catch Exception e (do (logs/log-error e) {:res "ERROR" :error e})))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; |ADAPTER INFO|
;;
;,      GET  "/info-stats"
;;      GET  "/info-stats/:t"
;;
;;    Example:    http://URL/api/info-stats
;;                GET /api/info-stats/30s
;;    Response:
;;    {
;;    	"monitored_hosts": ["ns50.bullx", "ns51.bullx", "ns54.bullx", "ns55.bullx", "ns56.bullx"],
;;    	"hosts_info": {
;;    		"ns50.bullx": {
;;    			"HAS_GPU": true,
;;    			"CPU_USAGE": {
;;    				"cpu0": {
;;    					"CPU_USAGE_LAST_T": "2017-07-11T08:49:27.822145935Z",
;;    					"CPU_USAGE_LAST_VALUE": 0.0,
;;    					"CPU_USAGE_MEAN_VALUE": 0.029,
;;    					"CPU_USAGE_MAX_VALUE": 1.8,
;;    					"CPU_USAGE_MIN_VALUE": 0.0
;;    				},
;;    				"cpu13": {
;;    					...
;;    				},
;;    				...
;;    			},
;;    			"GPU_USED": "NOT_IMPLEMENTED",
;;    			"APPS_ALLOCATED_TO_HOST_COUNT": "NOT_IMPLEMENTED",
;;    			"GPU": {
;;    				"GPU0": {
;;    					"POWER_LAST_VALUE": 30.367000579833984,
;;    					"POWER_LAST_VALUE_T": "2017-07-11T08:49:27.821787597Z"
;;    				},
;;    				"GPU1": {
;;    					"POWER_LAST_VALUE": 30.083999633789062,
;;    					"POWER_LAST_VALUE_T": "2017-07-11T08:49:27.82211157Z"
;;    				}
;;    			},
;;    			"APPS_STATUS": "NOT_IMPLEMENTED",
;;    			"APPS_RUNNING_ON_HOST_COUNT": "NOT_IMPLEMENTED",
;;    			"GPU_NAME": "NVIDIA",
;;    			"GPU_COUNT": 2,
;;    			"HAS_ACCELERATOR": true
;;    		},
;;    		"ns51.bullx": {
;;    			...
;;    		},
;;    		...
;;    	}
;;    }
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  Data required:
;;     //Power and energy
;;     public static final String POWER_KPI_NAME = "power";
;;     public static final String ESTIMATED_POWER_KPI_NAME = "power-estimated";
;;     public static final String ENERGY_KPI_NAME = "energy";
;;     //Utilisation Metrics
;;     public static final String CPU_SPOT_USAGE_KPI_NAME = "cpu-measured";
;;     //Plus any similar ones for the GPU.
;;     //applications metrics
;;     public static final String APPS_ALLOCATED_TO_HOST_COUNT = "app_on_host_count";
;;     public static final String APPS_RUNNING_ON_HOST_COUNT = "app_on_host_count";
;;     public static final String APPS_STATUS = "application_status";
;;     //accelerators
;;     public static final String HAS_ACCELERATOR = "has_accelerator";
;;     public static final String HAS_GPU = "has_gpu";
;;     public static final String GPU_NAME = "gpu_name";
;;     public static final String GPU_COUNT = "gpu_count";
;;     public static final String GPU_USED = "gpu_used";
;;     public static final String HAS_MIC = "has_many_integrated_core";
;;     public static final String MIC_NAME = "mic_name";
;;     public static final String MIC_COUNT = "mic_count";
;;     public static final String MIC_USED = "mic_used";
;;
;; Data required: |GPU values: nvidia-smi utility|
;;   timestamp
;;   index.............................gpu index, identifies the card on the host
;;   name..............................identifies the card on the host
;;   uuid..............................identifies the card on the host
;;   serial............................identifies the card on the host
;;   utilization.gpu [%]...............useful, but indicates true if at least one SM on the card is
;;                                     working, it might be more useful to change it to take into
;;                                     account how many are in use in a given time period. i.e. this
;;                                     is percentage of time in the last n seconds that at least one
;;                                     SM is working.
;;   utilization.memory [%]
;;   memory.total [MiB]
;;   memory.free [MiB]
;;   memory.used [MiB]
;;   power.draw [W]....................This is the end goal of any model.
;;   power.limit [W]
;;   fan.speed [%]
;;   temperature.gpu
;;   compute_mode
;;   clocks.current.graphics [MHz].....at least one reporting clock rate helps
;;   clocks.current.sm [MHz]
;;   clocks.current.memory [MHz]
;;   clocks.current.video [MHz]
;;   gom.current
;;   pstate............................this helps (though is related to clock rate)
;;   clocks_throttle_reasons.active....(throttle reasons may also help)
;;   clocks_throttle_reasons.gpu_idle
;;   clocks_throttle_reasons.applications_clocks_setting
;;   clocks_throttle_reasons.sw_power_cap
;;   clocks_throttle_reasons.hw_slowdown
;;   clocks_throttle_reasons.sync_boost
;;   clocks_throttle_reasons.unknown
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; |QUERIES: InfluxDB|
;;    SELECT value, instance FROM cpu_value WHERE host = 'ns50.bullx' AND type_instance<>'idle' AND
;;    time > now() - 1d
;;
;;    SELECT last(value), instance FROM cpu_value WHERE host = 'ns50.bullx' AND type_instance<>'idle'
;;    AND time > now() - 1d GROUP BY instance
;;
;; 'mean, min, max' for each instance in a period of time
;;    SELECT mean(value), max(value), min(value) FROM cpu_value WHERE host = 'ns50.bullx' AND
;;    type_instance<>'idle' AND time > now() - 1d GROUP BY instance
;;
;; 'last 2 values' for each instance:
;;    SELECT value FROM cpu_value WHERE host = 'ns50.bullx' AND type_instance<>'idle' GROUP BY
;;    instance ORDER BY DESC LIMIT 2
;;
;;    SELECT last(value) FROM cpu_value WHERE host = 'ns50.bullx' AND type_instance<>'idle' GROUP
;;    BY instance
;;
;;    SELECT last(value) FROM nvidia_value GROUP BY host, type, type_instance
;;    SELECT last(value) FROM monitoring_value GROUP BY host, type_instance
;;
;;    SELECT value FROM cpu_value WHERE type_instance<>'idle' GROUP BY instance, host ORDER BY
;;    DESC LIMIT 1
;;
;;    SELECT mean(value), max(value), min(value) FROM cpu_value WHERE type_instance<>'idle'
;;    AND time > now() - 1d GROUP BY instance, host
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; FUNCTION: get-gpu-power-values-v2
(defn- get-gpu-power-values-v2 ""
  [host]
  (let [query-res (queries/get-lastval-NVIDIA-PLUGIN-v2 config/SERIE-NVIDIA-GPUs host)]
    (if (= "SUCCESS" (query-res :res))
      (let [total-gpus  (/ (count ((first (get-in query-res [:response :results])) :series)) config/SERIE-NVIDIA-GPUs-total-metrics)]
        (into {}
          (for [i (range 0 total-gpus)]
            (let [res-i-power (queries/values-get-lastval-NVIDIA-PLUGIN-v2 query-res "power" i)
                  res-i-percent (queries/values-get-lastval-NVIDIA-PLUGIN-v2 query-res "percent" i)
                  res-i-objects (queries/values-get-lastval-NVIDIA-PLUGIN-v2 query-res "objects" i)]
              {(keyword (str "GPU" i)) {
                  :POWER_LAST_VALUE         (common/round-number (second res-i-power) :precision 3)
                  :USAGE_LAST_VALUE         (common/round-number (second res-i-percent) :precision 3)
                  :APPS_RUNNING_LAST_VALUE  (common/round-number (second res-i-objects) :precision 3)
              }}))))
      nil)))

;; FUNCTION: gen-info-NVIDIA
(defn- gen-info-NVIDIA "Generates info map for NVIDIA measurements of a given host"
  [h]
  (when (some #(= h %) ((deref MONITORED-HOSTS-SERIES) config/SERIE-NVIDIA-GPUs))
    { :HAS_ACCELERATOR  true
      :HAS_GPU          true
      :GPU_NAME         "NVIDIA"
      :GPU_COUNT        (/ (get-in (deref HOSTS-TAGS-INSTANCES) [config/SERIE-NVIDIA-GPUs h]) config/SERIE-NVIDIA-GPUs-total-metrics)
      :GPU_USED         "NOT_IMPLEMENTED"
      :GPU              (get-gpu-power-values-v2 h)}))

;; FUNCTION: gen-map-response-info-NVIDIA
(defn- gen-map-response-info-NVIDIA "Generates response map with all info"
  [host]
  (if-let [res (gen-info-NVIDIA host)]
    res
    {:HAS_ACCELERATOR  false
     :HAS_GPU          false
     :GPU_NAME         ""
     :GPU_COUNT        0
     :GPU_USED         "NOT_IMPLEMENTED"}))

;; FUNCTION: gen-info-with-avg-CPU
(defn- gen-info-with-avg-CPU "Gets CPU usage info or the selected host. Returns empty map if any error / exception is found / thrown"
  [h t]
  (try
    (when (some #(= h %) ((deref MONITORED-HOSTS-SERIES) config/SERIE-CPU-PLUGIN))
      (into {}
        (let [res-last-val-series ((first (get-in (queries/get-lastval-CPU-PLUGIN config/SERIE-CPU-PLUGIN h) [:response :results])) :series)
              res-aggr-val-series ((first (get-in (queries/get-avgvals-CPU-PLUGIN config/SERIE-CPU-PLUGIN h t) [:response :results])) :series)]
          (for [x res-last-val-series]
            (let [instance            (get-in x [:tags :instance])
                  data-last-instance  ((first (filter #(= instance (get-in % [:tags :instance])) res-last-val-series)) :values)
                  data-aggr-instance  ((first (filter #(= instance (get-in % [:tags :instance])) res-aggr-val-series)) :values)]
              {(keyword (str "cpu" instance))
                { ;:CPU_USAGE_LAST_T       (ffirst data-last-instance)
                  :CPU_USAGE_LAST_VALUE   (common/round-number (second (first data-last-instance)) :precision 3)
                  ;:cpu-usage-aggr-time   (ffirst data-aggr-instance)
                  :CPU_USAGE_MEAN_VALUE   (common/round-number (second (first data-aggr-instance)) :precision 3)
                  :CPU_USAGE_MAX_VALUE    (common/round-number (nth (first data-aggr-instance) 2) :precision 3)
                  :CPU_USAGE_MIN_VALUE    (common/round-number (last (first data-aggr-instance)) :precision 3)}})))))
    (catch Exception e
      (do (logs/log-error e)
          (logs/log-error {:host h :res "ERROR in [gen-info-with-avg-CPU]"})
          (logs/log-debug {:host h :res "ERROR in [gen-info-with-avg-CPU]" :error e}) {}))))

;; FUNCTION: gen-info-CPU
(defn- gen-info-CPU "Gets CPU usage info or the selected host. Returns empty map if any error / exception is found / thrown"
  [h]
  (try
    (when (some #(= h %) ((deref MONITORED-HOSTS-SERIES) config/SERIE-CPU-PLUGIN))
      (into {}
        (let [res-last-val-series ((first (get-in (queries/get-lastval-CPU-PLUGIN config/SERIE-CPU-PLUGIN h) [:response :results])) :series)]
          (for [x res-last-val-series]
            (let [instance            (get-in x [:tags :instance])
                  data-last-instance  ((first (filter #(= instance (get-in % [:tags :instance])) res-last-val-series)) :values)]
              {(keyword (str "cpu" instance))
                { ;:CPU_USAGE_LAST_T       (ffirst data-last-instance)
                  :CPU_USAGE_LAST_VALUE   (common/round-number (second (first data-last-instance)) :precision 3)}})))))
    (catch Exception e
      (do (logs/log-error e)
          (logs/log-error {:host h :res "ERROR in [gen-info-CPU]"})
          (logs/log-debug {:host h :res "ERROR in [gen-info-CPU]" :error e}) {}))))

;; FUNCTION: gen-map-response-info-CPU
(defn- gen-map-response-info-CPU "Generates response map with all info"
  [host avg? t]
  (if avg?
    (if-let [res (gen-info-with-avg-CPU host t)]
      {:CPU_USAGE res}
      {:CPU_USAGE ""})
    (if-let [res (gen-info-CPU host)]
      {:CPU_USAGE res}
      {:CPU_USAGE ""})))

;; FUNCTION: gen-map-response-no-info-APPS
(defn- gen-map-response-no-info-APPS ""
  []
  {:APPS_ALLOCATED_TO_HOST_COUNT  "NOT_IMPLEMENTED"
   :APPS_RUNNING_ON_HOST_COUNT    "NOT_IMPLEMENTED"
   :APPS_STATUS                   "NOT_IMPLEMENTED"})

;; FUNCTION: gen-map-response-no-info-MICS
(defn- gen-map-response-no-info-MICS ""
  []
  {:APPS_ALLOCATED_TO_HOST_COUNT  "NOT_IMPLEMENTED"
   :APPS_RUNNING_ON_HOST_COUNT    "NOT_IMPLEMENTED"
   :APPS_STATUS                   "NOT_IMPLEMENTED"})

;; FUNCTION: get-info-stats-with-avg
(defn get-info-stats-with-avg "Gets stats (including averages) from all monitored hosts"
  [t]
  (ring-resp/response
    (try
      {:monitored_hosts (deref MONITORED-HOSTS)
       :hosts_info (into {}
                     (for [host (deref MONITORED-HOSTS)]
                       (let [res-info-NVIDIA (gen-map-response-info-NVIDIA host)          ;;accelerators: NVIDIA/GPUs TODO
                             res-info-CPU    (gen-map-response-info-CPU host true t)      ;; CPU
                             res-apps        (gen-map-response-no-info-APPS)              ;; applications metrics TODO
                             res-mics        (gen-map-response-no-info-MICS)]             ;; accelerators: MICS TODO
                         {host (merge res-info-NVIDIA res-info-CPU res-apps res-mics)})))}
      (catch Exception e (do (logs/log-error e) {:res "ERROR" :error e})))))

;; FUNCTION: get-info-stats
(defn get-info-stats "Gets stats from all monitored hosts"
  []
  (ring-resp/response
    (try
      {:monitored_hosts (deref MONITORED-HOSTS)
       :hosts_info (into {}
                     (for [host (deref MONITORED-HOSTS)]
                       (let [res-info-NVIDIA (gen-map-response-info-NVIDIA host)            ;; accelerators: NVIDIA/GPUs TODO
                             res-info-CPU    (gen-map-response-info-CPU host false nil)     ;; CPU
                             res-apps        (gen-map-response-no-info-APPS)                ;; applications metrics TODO
                             res-mics        (gen-map-response-no-info-MICS)]               ;; accelerators: MICS TODO
                         {host (merge res-info-NVIDIA res-info-CPU res-apps res-mics)})))}
      (catch Exception e (do (logs/log-error e) {:res "ERROR" :error e})))))

;; FUNCTION: get-info-stats-host
(defn get-info-stats-host "Gets stats from a monitored hosts"
  [host]
  (ring-resp/response
    (try
      {:hosts_info (into {}
                     (let [res-info-NVIDIA (gen-map-response-info-NVIDIA host)            ;; accelerators: NVIDIA/GPUs TODO
                           res-info-CPU    (gen-map-response-info-CPU host false nil)     ;; CPU
                           res-apps        (gen-map-response-no-info-APPS)                ;; applications metrics TODO
                           res-mics        (gen-map-response-no-info-MICS)]               ;; accelerators: MICS TODO
                       {host (merge res-info-NVIDIA res-info-CPU res-apps res-mics)}))}
      (catch Exception e (do (logs/log-error e) {:res "ERROR" :error e})))))
