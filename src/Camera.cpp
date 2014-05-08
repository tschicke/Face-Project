///*
// * Camera.cpp
// *
// *  Created on: Apr 14, 2014
// *      Author: tchicke
// */

#include "Camera.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>

Camera::Camera() {
	camNumber = 0;
	baseDistance = 0;
	baseRadius = 0;
	bottomEdge = 0;
	leftEdge = 0;
	rightEdge = 0;
	topEdge = 0;
	frameWidth = frameHeight = 0;
	Circle circle;
	circle.radius = 120;
	pastCircles[0] = circle;
	cv::namedWindow("Camera", CV_WINDOW_AUTOSIZE);
}

Camera::Camera(int camNumber) {
	this->camNumber = camNumber;
	openStream(camNumber);
}

void Camera::openStream(int camNumber) {
	capture.open(camNumber);
}

cv::Mat Camera::getCameraFrame() {
	cv::Mat frame;
	capture >> frame;
	return frame;
}

void Camera::calibrateDepth(float baseDistance) {
	Circle circle = getFrameCircle();
	this->baseDistance = baseDistance;
	this->baseRadius = circle.radius;
	std::cout << "Camera Base Radius " << baseRadius << '\n';
}

void Camera::calibrateSides(bool left, float xDistance) {
	if (left) {
		leftEdge = xDistance;
	} else {
		rightEdge = xDistance;
	}
}

void Camera::calibrateBottomTop(bool bottom, float yDistance) {
	if (bottom) {
		bottomEdge = yDistance;
	} else {
		topEdge = yDistance;
	}
}

bool Camera::calibrated() {
	return leftEdge && rightEdge && topEdge && bottomEdge && baseDistance;
}

Circle Camera::getFrameCircle() {
	cv::Mat frame = getCameraFrame();
	cv::namedWindow("Image", CV_WINDOW_AUTOSIZE);
	cv::imshow("Image", frame);
	cv::cvtColor(frame, frame, CV_BGR2HSV);

	std::vector<cv::Mat> channels;
	for (cv::MatIterator_<cv::Vec3b> it = frame.begin<cv::Vec3b>(), end = frame.end<cv::Vec3b>(); it != end; ++it) {
		int hue = (*it)[0];
		int saturation = (*it)[1];
		int value = (*it)[2];

		bool black = false;

		if (hue < 50 || hue > 65) {
			(*it)[2] = 0;
			black = true;
		}
		if (!black && (saturation < 50 || saturation > 220)) {
			(*it)[2] = 0;
			black = true;
		}
		if (!black && (value < 20 || value > 220)) {
			(*it)[2] = 0;
			black = true;
		}

		if (!black) {
			(*it)[1] = 0;
			(*it)[2] = 255;
		}
	}

	cv::cvtColor(frame, frame, CV_HSV2BGR);
	cv::cvtColor(frame, frame, CV_BGR2GRAY);

	cv::resize(frame, frame, cv::Size(), 1.88f / 1.5f, 1, cv::INTER_NEAREST);

	frameWidth = frame.cols;
	frameHeight = frame.rows;

	cv::GaussianBlur(frame, frame, cv::Size(9, 9), 2, 2);
	cv::GaussianBlur(frame, frame, cv::Size(9, 9), 2, 2);

	std::vector<cv::Vec3f> circles;

	cv::HoughCircles(frame, circles, CV_HOUGH_GRADIENT, 1, frame.rows / 8, 400, 20, 0, 0);

	for (int i = 0; i < circles.size(); ++i) {
		cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);

		cv::circle(frame, center, 3, cv::Scalar(128, 128, 128), -1, 8, 0);
		cv::circle(frame, center, radius, cv::Scalar(128, 128, 128), 3, 8, 0);
	}

	cv::imshow("Camera", frame);

	Circle returnCircle;

	if (circles.size() == 0) {
		return pastCircles[0]; //TODO extrapolate;
	} else if (circles.size() == 1) {
		cv::Vec3f circle = circles[0];
		returnCircle.positionScreenSpace.x = circle[0];
		returnCircle.positionScreenSpace.y = circle[1];
		returnCircle.radius = circle[2];
		for (int i = 0; i < 4; ++i) {
			pastCircles[4 - i] = pastCircles[3 - i];
		}
		pastCircles[0] = returnCircle;
		return returnCircle;
	}

	cv::Vec3f correctCircle = circles[0];
	for (int i = 1; i < circles.size(); ++i) {
		if (circles[i][2] > correctCircle[2]) {
			correctCircle[0] = circles[i][0];
			correctCircle[1] = circles[i][1];
			correctCircle[2] = circles[i][2];
		}
	}

	if(abs(correctCircle[0] - (pastCircles[0].positionScreenSpace.x)) < 100 || abs(correctCircle[1] - pastCircles[0].positionScreenSpace.y) < 100 || abs(correctCircle[2] - pastCircles[0].radius) > 30){
		return pastCircles[0];
	}

	returnCircle.positionScreenSpace.x = correctCircle[0];
	returnCircle.positionScreenSpace.y = correctCircle[1];
	returnCircle.radius = correctCircle[2];
	std::cout << returnCircle.radius << '\n';
	for (int i = 0; i < 4; ++i) {
		pastCircles[4 - i] = pastCircles[3 - i];
	}
	pastCircles[0] = returnCircle;
	std::cout << "Radius " << returnCircle.radius << '\n';
	return returnCircle;

}

glm::vec3 Camera::getObjectPosition(Circle circle) {
	float x;
	float y;
	float z;

	z = baseRadius * baseDistance / circle.radius;
	x = -((circle.positionScreenSpace.x - (frameWidth / 2)) * z * rightEdge) / (baseDistance * 500);
	y = (((frameHeight / 2) - circle.positionScreenSpace.y) * z * topEdge) / (baseDistance * 500);

	return glm::vec3(x, y, z);
}
//Calibrator::Calibrator(std::string path, float sideLength, int width, int height) {
//	this->path = path;
//	this->sideLength = sideLength;
//	this->width = width;
//	this->height = height;
//}
//
//void Calibrator::calibrate() {
//}
//
//cv::Mat Calibrator::getCameraMatrix() {
//}
//
//cv::Mat Calibrator::getDisortionCoefficients() {
//}
//
//void Calibrator::calcImagePoints() {
//}
