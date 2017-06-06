;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; This code is being developed for the TANGO Project: http://tango-project.eu
;;
;; Copyright: Roi Sucasas Font, Atos Research and Innovation, 2017.
;;
;; This code is licensed under an Apache 2.0 license. Please, refer to the
;; LICENSE.TXT file for more information
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(ns rest-api.api-resp-parser
  (:require [cheshire.core :as cheshire.core]
            [rest-api.logs :as logs]))

;; FUNCTION: get-results
(defn- get-results ""
  [resp]
  ((cheshire.core/parse-string (resp :body)) "results"))

;; FUNCTION: gen-resp-query
(defn gen-resp-query ""
  [resp]
  (if (nil? (resp :body))
    {:status "ERROR"
     :message "Operation returned nil"}
    {:status "EXECUTED"
     :message (str "Request time: " (resp :request-time))
     :code (resp :status)
     :response (get-results resp)}))

;; FUNCTION: gen-resp-query
(defn gen-resp-query-headers ""
  [resp]
  {:status "EXECUTED"
   :message (str "Request time: " (resp :request-time))
   :code (resp :status)
   :response (resp :headers)})

;; FUNCTION: gen-resp-query
(defn gen-resp ""
  [message]
  {:status "EXECUTED"
   :message ""
   :response message})

;; FUNCTION: gen-resp-query
(defn gen-resp-map ""
  [m message response]
  (merge {:status "EXECUTED" :message message :response response} m))

;; FUNCTION: gen-not-implemented
(defn gen-not-implemented ""
  []
  {:status "WARNING"
   :message "Operation not implemented"})

;; FUNCTION: gen-error-txt
(defn gen-error-txt ""
  [txt]
  {:status "ERROR"
   :message txt})

;; FUNCTION: gen-error-response
(defn gen-error-response ""
  [txt ex]
  {:status "ERROR"
   :message txt
   :stacktrace (logs/get-error-stacktrace ex)})
