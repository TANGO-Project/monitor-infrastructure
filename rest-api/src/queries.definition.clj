;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Queries
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
{
  :query-test {:db "collectd" :query "SHOW MEASUREMENTS"}
  :query-test2 {:db "collectd" :query "SELECT value FROM cpu_value where time > now() - 1h limit 100;"}
}
