;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; CONFIGURATION FILE:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
{
  :name         "Monitoring infrastructure REST API"
  :version      "0.1.0"
  :influxdb-api "http://192.168.252.2:8086"
  :db           "collectd"
  ;; names assigned in Collectd / plugins to the different components / series
  :monitored-series {
    :NVIDIA-GPUs  "monitoring_value"   ; NVIDIA plugin that collects 'power'
    :CPU-PLUGIN   "cpu_value"          ; CPU plugin that collects 'usage' and more metrics...
  }
  ;; interval / time for averages (in queries)
  :take-last-t "30s"
}
