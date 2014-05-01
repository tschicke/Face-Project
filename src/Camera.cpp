///*
// * Camera.cpp
// *
// *  Created on: Apr 14, 2014
// *      Author: tchicke
// */
//
//#include "Camera.h"
//
//Camera::Camera() {
//	camNumber = 0;
//	baseDistance = 0;
//	baseRadius = 0;
//	bottomEdge = 0;
//	leftEdge = 0;
//	rightEdge = 0;
//	topEdge = 0;
//}
//
//Camera::Camera(int camNumber) {
//	this->camNumber = camNumber;
//	openStream(camNumber);
//}
//
//void Camera::openStream(int camNumber) {
//	capture.open(camNumber);
//}
//
//cv::Mat Camera::getCameraFrame() {
//	cv::Mat frame;
//	capture >> frame;
//	return frame;
//}
//
//void Camera::calibrateDepth(float baseDistance) {
//	Circle circle = getFrameCircle();
//	this->baseDistance = baseDistance;
//	this->baseRadius = circle.radius;
//}
//
//void Camera::calibrateSides(bool left, float xDistance) {
//	if (left) {
//		leftEdge = xDistance;
//	} else {
//		rightEdge = xDistance;
//	}
//}
//
//void Camera::calibrateBottomTop(bool bottom, float yDistance) {
//	if (bottom) {
//		bottomEdge = yDistance;
//	} else {
//		topEdge = yDistance;
//	}
//}
//
//Circle Camera::getFrameCircle(){
//}
//
//glm::vec3 Camera::getObjectPosition(Circle circle){
//	float x;
//	float y;
//	float z;
//
//	z = baseRadius * baseDistance / circle.radius;
//	x = (circle.positionScreenSpace.x * z * rightEdge) / (baseDistance * 500);
//	y = (circle.positionScreenSpace.y * z * topEdge) / (baseDistance * 500);
//
//	return glm::vec3(x, y, z);
//}
////Calibrator::Calibrator(std::string path, float sideLength, int width, int height) {
////	this->path = path;
////	this->sideLength = sideLength;
////	this->width = width;
////	this->height = height;
////}
////
////void Calibrator::calibrate() {
////}
////
////cv::Mat Calibrator::getCameraMatrix() {
////}
////
////cv::Mat Calibrator::getDisortionCoefficients() {
////}
////
////void Calibrator::calcImagePoints() {
////}
