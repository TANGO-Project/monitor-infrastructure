pipeline {
    agent { docker { image 'circleci/clojure:lein-2.7.1' } }

    stages {
        stage ('Initialize') {
            steps {
                sh '''
                    echo "PATH = ${PATH}"
                    echo "M2_HOME = ${M2_HOME}"
                '''
            }
        }
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
