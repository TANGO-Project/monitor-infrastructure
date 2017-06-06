;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; This code is being developed for the TANGO Project: http://tango-project.eu
;;
;; Copyright: Roi Sucasas Font, Atos Research and Innovation, 2017.
;;
;; This code is licensed under an Apache 2.0 license. Please, refer to the
;; LICENSE.TXT file for more information
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(ns rest-api.queries
  (:require [rest-api.common :as common]))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; queries.definition configuration values:

;; properties (as map)
(def ^:private queries (common/read-content "queries.definition.clj"))

;; get query vaules
(defn query [query-id]
  (queries (keyword query-id)))
