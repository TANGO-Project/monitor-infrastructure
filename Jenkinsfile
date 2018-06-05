pipeline {
    agent { docker { image 'clojure:latest' } }

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
                
                dir('/home/atos/.jenkins/workspace') {
                    sh '''#!/bin/sh
                          ls -al'''
                }
                
                dir('./rest-api') {
                    sh 'java -version'
                    sh '''#!/bin/sh
                          ls -al'''
                    sh '''#!/bin/sh
                          lein test'''
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
