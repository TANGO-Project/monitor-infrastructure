;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; This code is being developed for the TANGO Project: http://tango-project.eu
;;
;; Copyright: Roi Sucasas Font, Atos Research and Innovation, 2017.
;;
;; This code is licensed under a GNU General Public License, version 2 license.
;; Please, refer to the LICENSE.TXT file for more information
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; CONFIGURATION FILE:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
{
  :name         "Monitoring infrastructure REST API"
  :version      "0.1.0"
  :influxdb-api "http://localhost:8086"
  :db           "collectd"
  ;; names assigned in Collectd / plugins to the different components / series
  :monitored-series {
    :NVIDIA-GPUs  "nvidia_value"        ; NVIDIA plugin that collects 'memory', 'power', 'usage' (percent), 'temperature' and 'running_apps' (objects)
    :XEON-PHI     "xeonphi_value"       ; XEON PHI plugin that collects 'memory', 'power', 'usage' (percent - NOT READY -) and 'temperature'
    :CPU-PLUGIN   "cpu_value"           ; CPU plugin that collects 'usage' and more metrics...
  }
  ;; interval / time for averages (in queries)
  :take-last-t "30s"
  ;; total metrics
  :NVIDIA-GPUs-total-metrics 6          ; nvidia_value,host=ns51,instance=1,type=memory
                                        ; nvidia_value,host=ns51,instance=1,type=objects
                                        ; nvidia_value,host=ns51,instance=1,type=percent
                                        ; nvidia_value,host=ns51,instance=1,type=power
                                        ; nvidia_value,host=ns51,instance=1,type=temperature
                                        ; nvidia_value,host=ns51,instance=1,type=threads
  :XEON-PHI-total-metrics 3             ; xeonphi_value,host=ns53.bullx,instance=1,type=memory
                                        ; xeonphi_value,host=ns53.bullx,instance=1,type=power
                                        ; xeonphi_value,host=ns53.bullx,instance=1,type=temperature
}
