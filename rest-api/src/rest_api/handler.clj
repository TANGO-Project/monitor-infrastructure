;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; This code is being developed for the TANGO Project: http://tango-project.eu
;;
;; Copyright: Roi Sucasas Font, Atos Research and Innovation, 2017.
;;
;; This code is licensed under an Apache 2.0 license. Please, refer to the
;; LICENSE.TXT file for more information
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(ns rest-api.handler
  (:use [compojure.core])
  (:require [ring.util.response :as response]
            [ring.middleware.json :as middleware]
            [compojure.route :as route]
            [compojure.handler :as handler]
            [ring.middleware.cors :refer [wrap-cors]]
            [rest-api.api-funcs :as api-funcs]))

;; ROUTES / APP / SERVER ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defroutes app-routes
  ; test service
  (GET "/" [] "Service running...")
  ; api
  (context "/api" []
    (defroutes api-users-routes
      (GET    "/"                         {headers :headers}                                   (api-funcs/api-get))
      (GET    "/influxdb-url"             {headers :headers}                                   (api-funcs/get-influxdb-url))
      (POST   "/influxdb-url"             {headers :headers, body :body}                       (api-funcs/set-influxdb-url body))
      (GET    "/db"                       {headers :headers}                                   (api-funcs/get-db))
      (POST   "/db"                       {headers :headers, body :body}                       (api-funcs/set-db body))
      (GET    "/ping"                     {headers :headers}                                   (api-funcs/api-ping))
      ;; Monitored components: hosts, metrics...
      (GET    "/monitored/hosts"          {headers :headers}                                   (api-funcs/get-MONITORED-HOSTS))
      (GET    "/monitored/series"         {headers :headers}                                   (api-funcs/get-MONITORED-SERIES))
      (GET    "/monitored/hosts-series"   {headers :headers}                                   (api-funcs/get-MONITORED-HOSTS-SERIES))
      ;; POWER-STATS: used by Programming Model Component
      (GET    "/power-stats/:host/:time"  {{host :host time :time} :params, headers :headers}  (api-funcs/get-power-stats host time nil))))
  ; routes
  (route/resources "/")
  (route/not-found (response/response {:message "-Route not found-"})))

;; APP - RUN SERVER
;; wrap-cors: https://github.com/r0man/ring-cors
(def app
  (-> (handler/api (wrap-cors app-routes
                 :access-control-allow-origin [#".*"]
                 :access-control-allow-methods [:get :post]))
      (middleware/wrap-json-response)
      (middleware/wrap-json-body)))
