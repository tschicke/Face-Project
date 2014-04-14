/*
 * Camera.h
 *
 *  Created on: Apr 14, 2014
 *      Author: tchicke
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <string.h>
#include <vector.h>

class Calibrator {
private:
	std::string path;
	std::vector<cv::Mat> images;
	cv::Mat cameraMatrix, distortionCoefficientss;
	bool show_chess_corners;
	float sideLength;
	int width, height;

	std::vector<std::vector<cv::Point2f>> imagePoints;
	std::vector<std::vector<cv::Point3f>> objectPoints;

public:
	Calibrator(std::string path, float sideLength, int width, int height);
	void calibrate();
	cv::Mat getCameraMatrix();
	cv::Mat getDisortionCoefficients();
	void calcImagePoints();
};
#endif /* CAMERA_H_ */
