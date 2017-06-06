(ns rest-api.handler
  (:use [compojure.core])
  (:require [ring.util.response :as response]
            [ring.middleware.json :as middleware]
            [clj-http.client :as http-client]
            [compojure.route :as route]
            [compojure.handler :as handler]
            [ring.middleware.cors :refer [wrap-cors]]
            [rest-api.api-funcs :as api]))

;; ROUTES / APP / SERVER ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defroutes app-routes
  ; test service
  (GET "/" [] "Service running...")
  ; api
  (context "/api" []
    (defroutes api-users-routes
      (GET    "/"                    {headers :headers}                                   (api/api-get))
      (GET    "/influxdb-url"        {headers :headers}                                   (api/get-influxdb-url))
      (POST   "/influxdb-url"        {headers :headers, body :body}                       (api/set-influxdb-url body))
      (GET    "/ping"                {headers :headers}                                   (api/api-ping))
      (GET    "/test-query"          {headers :headers}                                   (api/api-test-query))
      (GET    "/query/:db/:query"    {{db :db query :query} :params, headers :headers}    (api/api-query db query))
      (GET    "/query-by-id/:id"     {{id :id} :params, headers :headers}                 (api/api-query-by-id id))))
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
