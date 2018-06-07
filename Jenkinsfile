pipeline {
    agent { docker { image 'circleci/clojure:lein-2.7.1' } }

    stages {
        stage('Checkout code') {
            steps {
                checkout scm
                echo 'Checking folder...'
                sh 'ls'
            }
        }
        stage('Build') {
            steps {
                echo 'Building...'
            }
        }
        stage('Test') {
            steps {
                echo 'Testing...'
                sh 'lein -version'
                sh "cd ./rest-api && lein test"
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying...'
            }
        }
    }
}
