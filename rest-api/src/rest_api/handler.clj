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
      (GET  "/"                           {headers :headers}                                  (api-funcs/api-get))
      (GET  "/influxdb-url"               {headers :headers}                                  (api-funcs/get-influxdb-url))
      (POST "/influxdb-url"               {headers :headers, body :body}                      (api-funcs/set-influxdb-url body))
      (GET  "/db"                         {headers :headers}                                  (api-funcs/get-db))
      (POST "/db"                         {headers :headers, body :body}                      (api-funcs/set-db body))
      (GET  "/ping"                       {headers :headers}                                  (api-funcs/api-ping))
      ;; Monitored components: hosts, metrics...
      (GET  "/monitored/hosts"            {headers :headers}                                  (api-funcs/get-MONITORED-HOSTS))
      (GET  "/monitored/series"           {headers :headers}                                  (api-funcs/get-MONITORED-SERIES))
      (GET  "/monitored/hosts-series"     {headers :headers}                                  (api-funcs/get-MONITORED-HOSTS-SERIES))
      ;; INFO: used by
      (GET  "/info-stats"                 {headers :headers}                                  (api-funcs/get-info-stats))
      ;; TODO replace t value with config/TAKE-LAST-T
      (GET  "/info-stats-with-avg"        {{t :t} :params, headers :headers}                  (api-funcs/get-info-stats-with-avg "4d"))
      ;; POWER-STATS: used by Programming Model Component
      (GET  "/power-stats/:h/:t"          {{h :h t :t} :params, headers :headers}             (api-funcs/get-power-stats h t nil))
      (GET  "/power-stats/:h/:t1/:t2"     {{h :h t1 :t1 t2 :t2} :params, headers :headers}    (api-funcs/get-power-stats-t h t1 t2))))
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
