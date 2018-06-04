pipeline {
    agent any

    stages {
        stage('Checkout code') {
            steps {
                checkout scm
            }
        }
        stage('Build') {
            steps {
                echo 'Building..'
                dir './rest-api'
                sh 'lein test'
            }
        }
        stage('Test') {
            steps {
                echo 'Testing..'
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
            }
        }
    }
}
