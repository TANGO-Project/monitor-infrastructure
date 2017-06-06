;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; This code is being developed for the TANGO Project: http://tango-project.eu
;;
;; Copyright: Roi Sucasas Font, Atos Research and Innovation, 2017.
;;
;; This code is licensed under an Apache 2.0 license. Please, refer to the
;; LICENSE.TXT file for more information
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(ns rest-api.logs
  (:require [clojure.tools.logging :as logging]))

;; use to check not nil
(def ^:private not-nil? (complement nil?))

;; prefix
(def ^:private logs-prefix " #rest-api# ")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; PRIVATE FUNCTIONS:

;; FUNCTION:
(defn- print-log
  "prints log content on screen console"
  [debug-level txt]
  {:pre [(not-nil? debug-level) (not-nil? txt)]}
  (cond
    (= debug-level "DEBUG") (logging/debug txt)
    (= debug-level "INFO") (logging/info txt)
    (= debug-level "ERROR") (logging/error txt)
    (= debug-level "WARNING") (logging/warn txt)
    :else (logging/trace txt)))

;; FUNCTION:
(defn- pr-log
  [l-type & txt]
  (print-log l-type (str logs-prefix (apply str "" txt))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; PUBLIC FUNCTIONS:

;; FUNCTION:
(defn log-debug [& txt] {:pre [(not-nil? txt)]}
  (apply pr-log "DEBUG" txt))

;; FUNCTION:
(defn log-info [& txt] {:pre [(not-nil? txt)]}
  (apply pr-log "INFO" txt))

;; FUNCTION:
(defn log-error [& txt] {:pre [(not-nil? txt)]}
  (apply pr-log "ERROR" txt))

;; FUNCTION:
(defn log-warning [& txt] {:pre [(not-nil? txt)]}
  (apply pr-log "WARNING" txt))

;; FUNCTION:
(defn log-trace [& txt] {:pre [(not-nil? txt)]}
  (apply pr-log "TRACE" txt))

;; FUNCTION: creates a map with Exception info
(defn log-exception
  ([e]
   (log-error "> Caught exception: [" (.getMessage e) "], stackTrace: \n    " (clojure.string/join "\n    " (map str (.getStackTrace e)))))
  ([e m]
   (log-error "> [" m "] > Caught exception: [" (.getMessage e) "], stackTrace: \n    " (clojure.string/join "\n    " (map str (.getStackTrace e))))))

;; FUNCTION: create error map
(defn get-error-stacktrace [e] (clojure.string/join "\n    " (map str (.getStackTrace e))))
