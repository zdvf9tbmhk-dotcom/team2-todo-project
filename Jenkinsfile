pipeline {
    agent any
    options { timestamps() }
    stages {
        stage('Configure') {
            steps {
                sh 'cmake -S . -B build -DCMAKE_BUILD_TYPE=Release'
            }
        }
        stage('Build + automatic tests') {
            steps {
                sh 'cmake --build build --parallel 2'
            }
        }
        stage('CTest') {
            steps {
                sh 'ctest --test-dir build --output-on-failure'
            }
        }
    }
    post {
        always { archiveArtifacts artifacts: 'build/**', allowEmptyArchive: true }
    }
}
