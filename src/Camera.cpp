/*
 * Camera.cpp
 *
 *  Created on: Apr 14, 2014
 *      Author: tchicke
 */

#include "Camera.h"

Calibrator::Calibrator(std::string path, float sideLength, int width, int height) {
	this->path = path;
	this->sideLength = sideLength;
	this->width = width;
	this->height = height;
}

void Calibrator::calibrate() {
}

cv::Mat Calibrator::getCameraMatrix() {
}

cv::Mat Calibrator::getDisortionCoefficients() {
}

void Calibrator::calcImagePoints() {
}
