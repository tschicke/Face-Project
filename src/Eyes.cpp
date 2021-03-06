//
//  Eyes.c
//  Face Test
//
//  Created by Tyler Schicke on 12/3/13.
//  Copyright (c) 2013 Tyler Schicke. All rights reserved.
//

#include <stdio.h>
#include <cmath>
#include <GL/gl.h>
#include "Head.h"

#include <iostream>

float toRad(float deg) {
	return deg * 3.14159265358979f / 180;
}

EyeInfo initEyeInfo(float x, float y, float z, int yaw, int pitch, int roll, float size, float radius);

EyeInfo initEyeInfo(float x, float y, float z, int yaw, int pitch, int roll, float size, float radius) {
	EyeInfo eyeInfo;
	eyeInfo.x = x;
	eyeInfo.y = y;
	eyeInfo.z = z;
	eyeInfo.yaw = yaw;
	eyeInfo.pitch = pitch;
	eyeInfo.roll = roll;
	eyeInfo.size = size;
	eyeInfo.radius = radius;
	eyeInfo.blinking = false;
	eyeInfo.timeToNextBlink = 1000000;

	return eyeInfo;
}

void drawEye(float x, float y, float z, float radius, float yaw, float pitch, float roll) {
	float pupilAngle = 14;
	float irisAngle = 14;
	float whiteAngle = (90 - (pupilAngle + irisAngle)) / 2;

	float cosPupil = cos(toRad(pupilAngle));
	float cosIris = cos(toRad(irisAngle + pupilAngle));
	float cosWhite1 = cos(toRad(whiteAngle + irisAngle + pupilAngle));
	float cosWhite2 = 0;

	float pupilRadius = radius * sin(toRad(pupilAngle));
	float irisRadius = radius * sin(toRad(irisAngle + pupilAngle));
	float white1Radius = radius * sin(toRad(whiteAngle + irisAngle + pupilAngle));
	float white2Radius = radius;

	glPushMatrix();

	glTranslatef(x, y, z);
	glRotatef(yaw, 0, 1, 0);
	glRotatef(roll, 0, 0, 1);
	glRotatef(pitch, 1, 0, 0);

//	glBegin(GL_LINES);
//	glColor3f(1, 0, 0);
//	glVertex3f(0, 0, 0);
//	glVertex3f(radius * 1.5, 0, 0);
//	glColor3f(0, 1, 0);
//	glVertex3f(0, 0, 0);
//	glVertex3f(0, radius * 1.5, 0);
//	glColor3f(0, 0, 1);
//	glVertex3f(0, 0, 0);
//	glVertex3f(0, 0, radius * 1.5);
//	glEnd();

//TODO make drawTriangleRing() function

//Pupil
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0, 0, 0);
	glVertex3f(pupilRadius, 0, radius * cosPupil);
	glVertex3f(0, 0, radius * cosPupil);
	for (int angle = 360 / 12; angle < 360; angle += 360 / 12) {
		glVertex3f(pupilRadius * cos(toRad(angle)), pupilRadius * sin(toRad(angle)), radius * cosPupil);
	}
	glEnd();

	//Iris
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(0.4, 0.75f, 0.95);
	glVertex3f(pupilRadius, 0, radius * cosPupil);
	glVertex3f(irisRadius, 0, radius * cosIris);
	for (int i = 360 / 12; i < 360 + (360 / 12); i += 360 / 12) {
		glVertex3f(pupilRadius * cos(toRad(i)), pupilRadius * sin(toRad(i)), radius * cosPupil);
		glVertex3f(irisRadius * cos(toRad(i)), irisRadius * sin(toRad(i)), radius * cosIris);
	}
	glEnd();

	//Eye White 1
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glVertex3f(irisRadius, 0, radius * cosIris);
	glVertex3f(white1Radius, 0, radius * cosWhite1);
	for (int i = 360 / 12; i < 360 + (360 / 12); i += 360 / 12) {
		glVertex3f(irisRadius * cos(toRad(i)), irisRadius * sin(toRad(i)), radius * cosIris);
		glVertex3f(white1Radius * cos(toRad(i)), white1Radius * sin(toRad(i)), radius * cosWhite1);
	}
	glEnd();

	//Eye White 2
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(white1Radius, 0, radius * cosWhite1);
	glVertex3f(white2Radius, 0, radius * cosWhite2);
	for (int i = 360 / 12; i < 360 + (360 / 12); i += 360 / 12) {
		glVertex3f(white1Radius * cos(toRad(i)), white1Radius * sin(toRad(i)), radius * cosWhite1);
		glVertex3f(white2Radius * cos(toRad(i)), white2Radius * sin(toRad(i)), radius * cosWhite2);
	}
	glEnd();

	//Back of eye
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(pupilRadius, 0, -radius * cosPupil);
	glVertex3f(0, 0, -radius * cosPupil);
	for (int angle = 360 / 12; angle < 360; angle += 360 / 12) {
		glVertex3f(pupilRadius * cos(toRad(angle)), pupilRadius * sin(toRad(angle)), -radius * cosPupil);
	}
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(pupilRadius, 0, -radius * cosPupil);
	glVertex3f(irisRadius, 0, -radius * cosIris);
	for (int i = 360 / 12; i < 360 + (360 / 12); i += 360 / 12) {
		glVertex3f(pupilRadius * cos(toRad(i)), pupilRadius * sin(toRad(i)), -radius * cosPupil);
		glVertex3f(irisRadius * cos(toRad(i)), irisRadius * sin(toRad(i)), -radius * cosIris);
	}
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(irisRadius, 0, -radius * cosIris);
	glVertex3f(white1Radius, 0, -radius * cosWhite1);
	for (int i = 360 / 12; i < 360 + (360 / 12); i += 360 / 12) {
		glVertex3f(irisRadius * cos(toRad(i)), irisRadius * sin(toRad(i)), -radius * cosIris);
		glVertex3f(white1Radius * cos(toRad(i)), white1Radius * sin(toRad(i)), -radius * cosWhite1);
	}
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(white1Radius, 0, radius * -cosWhite1);
	glVertex3f(white2Radius, 0, radius * -cosWhite2);
	for (int i = 360 / 12; i < 360 + (360 / 12); i += 360 / 12) {
		glVertex3f(white1Radius * cos(toRad(i)), white1Radius * sin(toRad(i)), -radius * cosWhite1);
		glVertex3f(white2Radius * cos(toRad(i)), white2Radius * sin(toRad(i)), -radius * cosWhite2);
	}
	glEnd();

	glPopMatrix();
}

void drawEyeLid(float x, float y, float z, float radius, float pitch) {
	float pupilAngle = 14;
	float irisAngle = 14;
	float whiteAngle = (90 - (pupilAngle + irisAngle)) / 2;

	float cosPupil = cos(toRad(pupilAngle));
	float cosIris = cos(toRad(irisAngle + pupilAngle));
	float cosWhite1 = cos(toRad(whiteAngle + irisAngle + pupilAngle));
	float cosWhite2 = 0;

	float pupilRadius = radius * sin(toRad(pupilAngle));
	float irisRadius = radius * sin(toRad(irisAngle + pupilAngle));
	float white1Radius = radius * sin(toRad(whiteAngle + irisAngle + pupilAngle));
	float white2Radius = radius;

	glPushMatrix();

	glTranslatef(x, y, z);
	glRotatef(pitch, 1, 0, 0);

	//Pupil
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.898, 0.722, .561);
	glVertex3f(pupilRadius, 0, radius * cosPupil);
	glVertex3f(0, 0, radius * cosPupil);
	for (int angle = 360 / 12; angle < 180 + (360 / 12); angle += 360 / 12) {
		glVertex3f(pupilRadius * cos(toRad(angle)), pupilRadius * sin(toRad(angle)), radius * cosPupil);
	}
	glEnd();

	//Iris
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(pupilRadius, 0, radius * cosPupil);
	glVertex3f(irisRadius, 0, radius * cosIris);
	for (int i = 360 / 12; i < 180 + (360 / 12); i += 360 / 12) {
		glVertex3f(pupilRadius * cos(toRad(i)), pupilRadius * sin(toRad(i)), radius * cosPupil);
		glVertex3f(irisRadius * cos(toRad(i)), irisRadius * sin(toRad(i)), radius * cosIris);
	}
	glEnd();

	//Eye White 1
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(irisRadius, 0, radius * cosIris);
	glVertex3f(white1Radius, 0, radius * cosWhite1);
	for (int i = 360 / 12; i < 180 + (360 / 12); i += 360 / 12) {
		glVertex3f(irisRadius * cos(toRad(i)), irisRadius * sin(toRad(i)), radius * cosIris);
		glVertex3f(white1Radius * cos(toRad(i)), white1Radius * sin(toRad(i)), radius * cosWhite1);
	}
	glEnd();

	//Eye White 2
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(white1Radius, 0, radius * cosWhite1);
	glVertex3f(white2Radius, 0, radius * cosWhite2);
	for (int i = 360 / 12; i < 180 + (360 / 12); i += 360 / 12) {
		glVertex3f(white1Radius * cos(toRad(i)), white1Radius * sin(toRad(i)), radius * cosWhite1);
		glVertex3f(white2Radius * cos(toRad(i)), white2Radius * sin(toRad(i)), radius * cosWhite2);
	}
	glEnd();

	//Back of eye
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(pupilRadius, 0, -radius * cosPupil);
	glVertex3f(0, 0, -radius * cosPupil);
	for (int angle = 360 / 12; angle < 180 + (360 / 12); angle += 360 / 12) {
		glVertex3f(pupilRadius * cos(toRad(angle)), pupilRadius * sin(toRad(angle)), -radius * cosPupil);
	}
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(pupilRadius, 0, -radius * cosPupil);
	glVertex3f(irisRadius, 0, -radius * cosIris);
	for (int i = 360 / 12; i < 180 + (360 / 12); i += 360 / 12) {
		glVertex3f(pupilRadius * cos(toRad(i)), pupilRadius * sin(toRad(i)), -radius * cosPupil);
		glVertex3f(irisRadius * cos(toRad(i)), irisRadius * sin(toRad(i)), -radius * cosIris);
	}
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(irisRadius, 0, -radius * cosIris);
	glVertex3f(white1Radius, 0, -radius * cosWhite1);
	for (int i = 360 / 12; i < 180 + (360 / 12); i += 360 / 12) {
		glVertex3f(irisRadius * cos(toRad(i)), irisRadius * sin(toRad(i)), -radius * cosIris);
		glVertex3f(white1Radius * cos(toRad(i)), white1Radius * sin(toRad(i)), -radius * cosWhite1);
	}
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(white1Radius, 0, radius * -cosWhite1);
	glVertex3f(white2Radius, 0, radius * -cosWhite2);
	for (int i = 360 / 12; i < 180 + (360 / 12); i += 360 / 12) {
		glVertex3f(white1Radius * cos(toRad(i)), white1Radius * sin(toRad(i)), -radius * cosWhite1);
		glVertex3f(white2Radius * cos(toRad(i)), white2Radius * sin(toRad(i)), -radius * cosWhite2);
	}
	glEnd();

	glPopMatrix();
}

void drawEyeSystem(float sysX, float sysY, float sysZ, int sysPitch, int sysYaw, int sysRoll, EyeInfo& eye1Info, EyeInfo& eye2Info, bool shouldBlink) {
	glPushMatrix();

	glTranslatef(sysX, sysY, sysZ);
	glRotatef(sysPitch, 1, 0, 0);
	glRotatef(sysYaw, 0, 1, 0);
	glRotatef(sysRoll, 0, 0, 1);

	drawEye(eye1Info.x, eye1Info.y, eye1Info.z, eye1Info.radius, eye1Info.yaw, eye1Info.pitch, eye1Info.roll);
	drawEye(eye2Info.x, eye2Info.y, eye2Info.z, eye2Info.radius, eye2Info.yaw, eye2Info.pitch, eye2Info.roll);

	int blinkTime = 200000;
	float startPitch = -30;
	float endPitch = 40;

	float timeToNextBlink = (((rand() / (float) RAND_MAX) * 0.9) + 0.1) * 3500000;

	float eyelid1Pitch = startPitch;

	if (eye1Info.blinking) {
		bool eyelid1Finished = false;

		if (eye1Info.eyelidCounter > blinkTime) {
			eye1Info.eyelidCounter = blinkTime;
			eyelid1Finished = true;
		}
		eyelid1Pitch = startPitch + (1 - 2 * fabs((eye1Info.eyelidCounter / (float) blinkTime) - 0.5)) * (endPitch - startPitch);
		if (eyelid1Finished) {
			eye1Info.blinking = false;
			eye1Info.eyelidCounter = 0;
			eye1Info.timeToNextBlink = timeToNextBlink;
		}
	} else if (shouldBlink && eye1Info.eyelidCounter > eye1Info.timeToNextBlink) {
		eye1Info.eyelidCounter = 0;
		eye1Info.blinking = true;
	}

	float eyelid2Pitch = startPitch;

	if (eye2Info.blinking) {
		bool eyelid2Finished = false;

		if (eye2Info.eyelidCounter > blinkTime) {
			eye2Info.eyelidCounter = blinkTime;
			eyelid2Finished = true;
		}
		eyelid2Pitch = startPitch + (1 - 2 * fabs((eye2Info.eyelidCounter / (float) blinkTime) - 0.5)) * (endPitch - startPitch);
		if (eyelid2Finished) {
			eye2Info.blinking = false;
			eye2Info.eyelidCounter = 0;
			eye2Info.timeToNextBlink = timeToNextBlink;
		}
	} else if (shouldBlink && eye2Info.eyelidCounter > eye2Info.timeToNextBlink) {
		eye2Info.eyelidCounter = 0;
		eye2Info.blinking = true;
	}

//	std::cout << eye2Info.eyelidCounter << '\n';

	drawEyeLid(eye1Info.x, eye1Info.y, eye1Info.z, 1.16, eyelid1Pitch);
	drawEyeLid(eye2Info.x, eye2Info.y, eye2Info.z, 1.16, eyelid2Pitch);

	glPopMatrix();
}
