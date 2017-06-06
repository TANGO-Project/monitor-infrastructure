(ns rest-api.queries
  (:require [rest-api.common :as common]))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; queries.definition configuration values:

;; properties (as map)
(def ^:private queries (common/read-content "queries.definition.clj"))

;; get query vaules
(defn query [query-id]
  (queries (keyword query-id)))
