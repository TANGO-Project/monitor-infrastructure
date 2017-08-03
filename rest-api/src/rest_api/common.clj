;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; This code is being developed for the TANGO Project: http://tango-project.eu
;;
;; Copyright: Roi Sucasas Font, Atos Research and Innovation, 2017.
;;
;; This code is licensed under an Apache 2.0 license. Please, refer to the
;; LICENSE.TXT file for more information
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(ns rest-api.common
  (:require [clj-http.client :as http-client]
            [cheshire.core :as cheshire.core]
            [rest-api.logs :as logs]))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; PUBLIC FUNCTIONS:

;; FUNCTION: get-resource
(defn get-resource "get file or nil if error"
  [path]
  (try
    (when path
      (-> (Thread/currentThread) .getContextClassLoader (.getResource path)))
    (catch Exception e
      (do (logs/log-exception e) nil))))

;; HTTP ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; common HTTP status codes:

;; OK : The request was fulfilled.
;(def HTRESP_OK 200)

;; CREATED : Following a POST command, this indicates success, but the textual part of the response line indicates
;;           the URI by which the newly created document should be known.
;(def HTRESP_CREATED 201)

;; ACCEPTED : The request has been accepted for processing, but the processing has not been completed.
(def HTRESP_ACCEPTED 202)

;; Errors...
;(def HTRESP_UNAUTHORIZED 401)

;; GET ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; FUNCTION: GET
(defn GET "GET function call"
  [rest-api-url]
  (do
    (logs/log-debug "Calling [" rest-api-url "] ...")
    (let [resp (http-client/get rest-api-url)]
      (do
        (logs/log-debug " -> :status " (resp :status) ", :body " (resp :body))
        resp))))

;; POST ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; FUNCTION: basic-post
(defn- basic-post "POST function call"
  [rest-api-url params http-map]
  (http-client/post (str rest-api-url params) http-map))

;; FUNCTION: POST
(defn POST ""
  [endpoint params body]
  (let [res (basic-post endpoint params {:body body})]
    (if (= HTRESP_ACCEPTED (:code res))
      (cheshire.core/parse-string (get-in res [:response :body]))
      {:code (:code res) :content res})))

;; MATHS ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; FUNCTION: round-number
;; Examples: (round-number 78.57898794 :precision 3) (round-number 78.57898794) (round-number 78.5)
(defn round-number "Rounds a number 'x'"
  [x & {p :precision}]
  (if p
    (let [scale (Math/pow 10 p)]
      (-> x (* scale) Math/round (/ scale)))
    (Math/round x)))

;; FUNCTION: parse-int
(defn parse-int [s]
  (Integer/parseInt (re-find #"\A-?\d+" s)))
