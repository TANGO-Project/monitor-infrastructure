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
                sh 'java -version'
                sh 'cd rest-api'
                sh 'ls'
                sh 'cd rest-api'
                sh 'ls'
                sh '/usr/local/bin/lein'
                sh '/usr/local/bin/lein -version'
                sh '/usr/local/bin/lein test'
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
            }
        }
    }
}
