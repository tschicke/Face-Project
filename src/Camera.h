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

#include <string>
#include <vector.h>

#include <glm/glm.hpp>

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

struct Circle {
public:
	float radius;
	glm::vec2 positionScreenSpace;
};

class Camera {
public:
	Camera();
	Camera(int camNumber);

	void openStream(int camNumber);

	cv::Mat getCameraFrame();

	void calibrateDepth(float baseDistance);
	void calibrateSides(bool left, float xDistance);
	void calibrateBottomTop(bool bottom, float yDistance);

	Circle getFrameCircle();

	glm::vec3 getObjectPosition(Circle circle);

private:
	cv::VideoCapture capture;

	int camNumber;

	float baseDistance, baseRadius;
	float leftEdge, rightEdge, bottomEdge, topEdge;
};

#endif /* CAMERA_H_ */
