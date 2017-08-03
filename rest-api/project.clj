;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; This code is being developed for the TANGO Project: http://tango-project.eu
;;
;; Copyright: Roi Sucasas Font, Atos Research and Innovation, 2017.
;;
;; This code is licensed under an Apache 2.0 license. Please, refer to the
;; LICENSE.TXT file for more information
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defproject rest-api "0.1.1"
  :description "Monitoring Infrastructure REST API"
  :url "http://localhost"
  :min-lein-version "1.8.0"
  :dependencies [ [org.clojure/clojure "1.8.0"]
                  [compojure "1.5.1"]
                  [cheshire "5.7.1"]                    ; MIT License ; https://github.com/dakrone/cheshire
                  [clj-http "3.5.0"]                    ; MIT License ; https://github.com/dakrone/clj-http/
                  [ring-cors/ring-cors "0.1.10"]        ; Eclipse Public License ; https://github.com/r0man/ring-cors
                  [ring/ring-defaults "0.3.0"]          ; MIT License ; https://github.com/ring-clojure/ring-defaults
                  [ring/ring-json "0.4.0"]              ; MIT License ; https://github.com/ring-clojure/ring-json
                  [org.clojure/tools.logging "0.3.1"]   ; Eclipse Public License - Version 1.0 ; https://github.com/clojure/tools.logging
                  [log4j/log4j "1.2.17"                 ; Apache License, Version 2.0 ; http://logging.apache.org/log4j/1.2/
                    :exclusions [ javax.mail/mail
                                  javax.jms/jms
                                  com.sun.jdmk/jmxtools
                                  com.sun.jmx/jmxri]]
                  [clj-time "0.14.0"]                   ; MIT License ; https://github.com/clj-time/clj-time
                  [org.clojure/data.json "0.2.6"]       ; Eclipse Public License - Version 1.0  https://github.com/clojure/data.json
                  [proto-repl "0.3.1"]]
  :plugins [[lein-ring "0.12.0"]
            [lein-uberwar "0.2.0"]]
  :uberwar {:handler rest-api.handler/app}
  :ring {:handler rest-api.handler/app
         :port 8082
         :open-browser? true
         :resources-war-path "WEB-INF/classes/"}
  :profiles
    {:dev {:dependencies [[javax.servlet/servlet-api "2.5"]
                          [ring/ring-mock "0.3.0"]]}
  ;; jvm configuration
  :jvm-opts ["-Xmx256M"]})
