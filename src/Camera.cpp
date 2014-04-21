/*
 * Camera.cpp
 *
 *  Created on: Apr 14, 2014
 *      Author: tchicke
 */

#include "Camera.h"

Camera::Camera() {
	camNumber = 0;
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

GridDetector::GridDetector() {
	gridWidth = gridHeight = 0;
	squareWidth = squareHeight = 0;
	gridDepth = 0;
}

GridDetector::GridDetector(int gridWidth, int gridHeight, int squareWidth, int squareHeight, int cameraID) {
	init(gridWidth, gridHeight, squareWidth, squareHeight, cameraID);
}

void GridDetector::init(int gridWidth, int gridHeight, int squareWidth, int squareHeight, int cameraID) {
	this->gridWidth = gridWidth;
	this->gridHeight = gridHeight;
	this->squareWidth = squareWidth;
	this->squareHeight = squareHeight;

	camera.openStream(cameraID);

	gridDepth = 0;
}

void GridDetector::update(){
	calcGridCorners2d();
	calcGridCorners3d();
}

void GridDetector::calcGridCorners2d(){

}

cv::Point2f* const GridDetector::getGridCorners2d() {
	return gridCorners2d;
}

cv::Point3f* const GridDetector::getGridCorners3d() {
	return gridCorners3d;
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
