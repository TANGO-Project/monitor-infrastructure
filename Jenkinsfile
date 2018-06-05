pipeline {
    agent { docker { image 'circleci/clojure:lein-2.7.1' } }

    stages {
        stage('Checkout code') {
            steps {
                checkout scm
                echo 'Checking folder..'
                sh 'ls'
            }
        }
        stage('Build') {
            steps {
                echo 'Building..'
            }
        }
        stage('Test') {
            steps {
                echo 'Testing..'
                dir('/home/atos/.jenkins/workspace/monitoring-infrastructure/rest-api') {
                    sh 'java -version'
                    sh 'lein -version'
                    sh '''#!/bin/sh
                          ls -al'''
                    sh 'lein test'
                }
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
            }
        }
    }
}
