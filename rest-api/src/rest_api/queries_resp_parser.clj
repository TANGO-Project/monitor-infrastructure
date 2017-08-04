;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; This code is being developed for the TANGO Project: http://tango-project.eu
;;
;; Copyright: Roi Sucasas Font, Atos Research and Innovation, 2017.
;;
;; This code is licensed under an Apache 2.0 license. Please, refer to the
;; LICENSE.TXT file for more information
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(ns rest-api.queries-resp-parser
  (:require [rest-api.logs :as logs]))

;; FUNCTION: process-response
(defn process-response-power-stats "" [r] (second (first ((first ((first ((r :response) :results)) :series)) :values))))

;; FUNCTION: values-get-val-node-v2
(defn values-get-val-node-v2 ""
  [query param]
  (first ((first (filter #(= (get-in % [:tags :type]) param) ((first ((query :response) :results)) :series))) :values)))

;; FUNCTION: values-get-lastval-NVIDIA-PLUGIN-v2
(defn values-get-lastval-NVIDIA-PLUGIN-v2 ""
  [query-res param-metric param-instance]
  (first ((first (filter  #(and (= (get-in % [:tags :type]) param-metric) (= (get-in % [:tags :instance]) (str param-instance)))
                          ((first ((query-res :response) :results)) :series))) :values)))
