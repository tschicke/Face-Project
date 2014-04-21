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
#include <opencv2/core/core.hpp>

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

	std::vector<std::vector<cv::Point2f> > imagePoints;
	std::vector<std::vector<cv::Point3f> > objectPoints;

public:
	Calibrator(std::string path, float sideLength, int width, int height);
	void calibrate();
	cv::Mat getCameraMatrix();
	cv::Mat getDisortionCoefficients();
	void calcImagePoints();
};


class Camera {
public:
	Camera();
	Camera(int camNumber);

	void openStream(int camNumber);

	cv::Mat getCameraFrame();

private:
	cv::VideoCapture capture;

	int camNumber;
};

class GridDetector {
public:
	GridDetector();
	GridDetector(int gridWidth, int gridHeight, int squareWidth, int squareHeight, int cameraID);

	void init(int gridWidth, int gridHeight, int squareWidth, int squareHeight, int cameraID);

	void update();

	cv::Point2f *const getGridCorners2d();
	cv::Point3f *const getGridCorners3d();

	void calcGridCorners2d();
	void calcGridCorners3d();
private:

	int gridWidth, gridHeight;//width and height in number of squares
	int squareWidth, squareHeight;//width and height of one square in centimeters

	Camera camera;

	cv::Point2f gridCorners2d[4];
	cv::Point3f gridCorners3d[4];
	float gridDepth;
};

#endif /* CAMERA_H_ */
