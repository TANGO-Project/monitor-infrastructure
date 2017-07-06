;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; This code is being developed for the TANGO Project: http://tango-project.eu
;;
;; Copyright: Roi Sucasas Font, Atos Research and Innovation, 2017.
;;
;; This code is licensed under an Apache 2.0 license. Please, refer to the
;; LICENSE.TXT file for more information
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(ns rest-api.config
  (:require [rest-api.common :as common]))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; app.properties configuration values:

;; default configuration:
(def ^:private default-configuration {
  :name         "Monitoring infrastructure REST API"
  :version      "0.1.0"
  :influxdb-api "http://192.168.252.41:8086"
  :db           "collectd"})

;; read configuration values into map
(defn- read-configuration [path]
  (let [fpath (common/get-resource path)]
    (if (nil? fpath)
      default-configuration
      (read-string (slurp fpath)))))

;; properties (as map)
(def ^:private props (read-configuration "app.properties.clj"))

;; get properties vaules from props
(defn- conf [& path] (get-in props (vec path)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; PUBLIC FUNCTIONS / DEFs:

(def APP-NAME (conf :name))
(def APP-VERSION (conf :version))
(def ^:private INFLUXDB-API (atom (conf :influxdb-api)))
(def ^:private DB (atom (conf :db)))

;; FUNCTION: get-INFLUXDB-API
(defn get-influxdb-api "" [] (deref INFLUXDB-API))

;; FUNCTION: change-influxdb-api-url
(defn change-influxdb-api-url ""
  [new-url]
  (swap! INFLUXDB-API (constantly new-url)))

;; FUNCTION: get-DB
(defn get-db "" [] (deref DB))

;; FUNCTION: change-DB
(defn change-db ""
  [new-db]
  (swap! DB (constantly new-db)))
