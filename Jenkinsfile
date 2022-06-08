#!/usr/bin/groovy
pipeline {
	agent any

	stages {

		stage('Build') {
			steps {
				echo 'Building..'
				cd char_driver
				make host
			}
		}

		stage('Test') {
			steps {
				echo 'Testing..'
				sudo insmod ./char_drv_fops.ko
				echo "testing 1 2 3" > /dev/chardev
				cat /dev/chardev

			}
		}

		stage('Deploy') {
			steps {
				echo 'Deploying....'
				sudo rmmod char_drv_fops
			}
		}

	}
}
