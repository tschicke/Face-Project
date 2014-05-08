// ==========================================================================
//                            SIMPLEFACE.C
// ==========================================================================
//
// This is an OpenGL implementation of the simple face from Appendix 1 in 
// Computer Facial Animation. It demonstrates the use of the muscle model 
// to create facial expressions.
//
//
//    HISTORY
//	16-Dec-94  Keith Waters (waters@crl.dec.com) Created at Digital Equipment
//		       Cambridge Research Laboratory
//
//	10-Aug-98  Keith Waters (waters@crl.dec.com) Modified for OpenGL on W95 and 
//			   WNT at Compaq Cambridge Research Laboratory. This code attempts
//			   to be as close to the original code as possible. Module names
//			   and function definitions have been maintained for clarity. A 
//			   few extra components have been added in particular expression I/O
//			   so you can make your own expressions. In addition transparency
//			   has been added to observe the location of facial muscles.
//
// ============================================================================ 

#include "stdio.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include <SFML/Window.hpp>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "src/Camera.h"

#include <cmath>

//#include "src/MainWindow.h"

#include "src/Head.h"		// Face includes
void OpenGLInit(void);

// Forward declarations
void calc_normal(float*, float*, float*, float*);
void calculate_polygon_vertex_normal(HEAD *);
void FaceInit();
void paint_muscles(HEAD*);
void paint_polygons(HEAD*, int, int);
void data_struct(HEAD*);
void read_muscles(const char*, HEAD*);
void read_expression_macros(const char*, const char *, HEAD*);
void activate_muscle(HEAD*, int, float);
void face_reset(HEAD*);
void make_expression(HEAD*, int);
void transitionExpression(HEAD *face, int e1, int e2, int timePassed, int totalTime);
HEAD *create_face(const char*, const char*, const char *, const char *);

struct EyeInfo;

EyeInfo initEyeInfo(float x, float y, float z, int yaw, int pitch, int roll, float size, float radius);
void drawEyeSystem(float sysX, float sysY, float sysZ, int sysPitch, int sysYaw, int sysRoll, EyeInfo& eye1Info, EyeInfo& eye2Info, bool shouldBlink);
void drawEye(float x, float y, float z, float radius, float yaw, float pitch, float roll);

EyeInfo eye1Info, eye2Info;
int eyeYaw, eyePitch, eyeRoll;
float eyeX, eyeY, eyeZ;
float lookAtX, lookAtY, lookAtZ;
int expressionAnimationTime = 500000, rotationAnimationTime = 500000, blinkAnimationTime = 100000;

float cameraX = 0, cameraY = 0, cameraZ = 0;

GLfloat rotateX, rotateY;

Camera camera;

HEAD *face;

float max(float a, float b) {
	return (a > b ? a : b);
}

// ======================================================================== 
// Key bindings for callback proceedures
// ========================================================================   
//

static void Key_e(void)
		{
	face_reset(face);
	make_expression(face, face->current_exp);
	face->current_exp++;
	if (face->current_exp >= face->nexpressions)
		face->current_exp = 0;

}
static void Key_b(void)
		{
	face->rendermode++;
	if (face->rendermode > 3)
		face->rendermode = 0;
}
static void Key_c(void)
		{
	int cm;

	cm = face->current_muscle;

	// Record the muscle activation 
	face->muscle[cm]->mstat += 0.1;

	activate_muscle(face,
			cm,
			0.1);
}
static void Key_C(void)
		{
	int cm;

	cm = face->current_muscle;

	// Record the muscle activation 
	face->muscle[cm]->mstat -= 0.1;

	activate_muscle(face, cm, -0.1);

}
static void Key_n(void) {
	face->current_muscle++;
	if (face->current_muscle >= face->nmuscles)
		face->current_muscle = 0;

	std::cout << face->muscle[face->current_muscle]->name << '\n';

}

static void Key_N(void) {
	face->current_muscle--;
	if (face->current_muscle < 0)
		face->current_muscle = face->nmuscles - 1;

	std::cout << face->muscle[face->current_muscle]->name << '\n';
}

static void Key_q() {
	face->transitioningRotation = true;
	face->startPosition = glm::vec3(20, 10, 15);
	face->endPosition = glm::vec3(0, 0, 15);
}

static void Key_l() {
	face->lookingAround = !face->lookingAround;
}

static void Key_p() {
	face->shouldBlink = !face->shouldBlink;
}

static void Key_up(void) {
	rotateX -= 2.0f;
}
static void Key_down(void) {
	rotateX += 2.0f;
}
static void Key_left(void) {
	rotateY -= 2.0f;
}
static void Key_right(void) {
	rotateY += 2.0f;
}
static void Key_r(void) {
	face_reset(face);
	face->current_exp = 0;
}
static void Key_R(void) {
	face_reset(face);
	face->current_exp = 0;
}
static void Key_T() {
	if (!face->transitioningExpression) {
		face->currentExpression++;
		face->nextExpression++;
		if (face->currentExpression >= face->nexpressions) {
			face->currentExpression = 0;
		}
		if (face->nextExpression >= face->nexpressions) {
			face->nextExpression = 0;
		}
	}
	face->transitioningExpression = true;
}
static void Key_w(void) {
	FILE *OutFile;
	int i;

	//  Check the FileName
	if ((OutFile = fopen("../face-data/single-expression.dat", "w+")) == 0) {
		fprintf(stderr, "Can't open:../face-data/single-expression.dat\n");
		return;
	}

	for (i = 0; i < face->nmuscles; i++) {

		fprintf(OutFile, "%s %2.2f\t", face->muscle[i]->name, face->muscle[i]->mstat);
	}

	fclose(OutFile);
}

static void Key_h(void)
		{
	fprintf(stderr, "\n");
	fprintf(stderr, "b:       toggles the drawing  modes (Wireframe, facet, smooth & transparent)\n");
	fprintf(stderr, "c:       contract the current facial muscle\n");
	fprintf(stderr, "C:       relax the current facial muscle\n");
	fprintf(stderr, "e:       next expression\n");
	fprintf(stderr, "h:       outputs this message\n");
	fprintf(stderr, "n:       next muscle (to select another facial muscle to manipulate)\n");
	fprintf(stderr, "r:       reset the expression\n");
	fprintf(stderr, "R:       reread the expression file (../face-data/expression-macros.dat)\n");
	fprintf(stderr, "w:           \t outputs muscles state (../face-data/single-expression.dat)\n");
	fprintf(stderr, "Up arrow:    \t rotates the face up\n");
	fprintf(stderr, "Down arrow:  \t rotates the face down\n");
	fprintf(stderr, "Right arrow: \t rotates the face right\n");
	fprintf(stderr, "Left arrow:  \t rotates the face left\n");

	fprintf(stderr, "\n");
}

glm::vec2 getAnglesToLookAtPoint(glm::vec3 position, EyeInfo eyeInfo) {
	const float pi = 3.14159265358979;

	glm::vec4 eyePos(eyeInfo.x + eyeX, eyeInfo.y + eyeY, eyeInfo.z + eyeZ, 1);

	glm::mat4 rotationMatrix = glm::rotate(-(float) rotateX, glm::vec3(1, 0, 0)) * glm::rotate(-(float) rotateY, glm::vec3(0.f, 1.f, 0.f));

	glm::vec4 rotatedPosition = rotationMatrix * glm::vec4(position, 1);

	float yaw = (atan2(rotatedPosition.x - eyePos.x, rotatedPosition.z - eyePos.z) * 180 / pi);

	glm::mat4 eyeTransform = glm::translate(glm::vec3(eyePos)) * glm::rotate(-(float) yaw, glm::vec3(0, 1, 0)) * glm::translate(-glm::vec3(eyePos)) * rotationMatrix;
	glm::vec4 eyeTransformedPosition = eyeTransform * glm::vec4(position, 1);

	float pitch = (atan2(eyePos.y - eyeTransformedPosition.y, eyeTransformedPosition.z - eyePos.z) * 180 / pi);

	return glm::vec2(pitch, yaw);
}

void lookAtPoint(glm::vec3 position) {
	glm::vec2 eye1Angles = getAnglesToLookAtPoint(position, eye1Info);
	glm::vec2 eye2Angles = getAnglesToLookAtPoint(position, eye2Info);

	eye1Info.pitch = eye1Angles.x;
	eye2Info.pitch = eye2Angles.x;
	eye1Info.yaw = eye1Angles.y;
	eye2Info.yaw = eye2Angles.y;
}

glm::vec2 getAnglesToFacePoint(glm::vec3 position) {
	const float pi = 3.14159265358979;

	glm::vec3 eyeSysPos(eyeX, eyeY, eyeZ);

	glm::vec3 transformedPosition = position + glm::vec3(eyeX, eyeY, 0);

	float yaw = atan2(position.x - eyeSysPos.x, position.z) * 180 / pi;

	glm::vec4 rotatedPosition = glm::rotate(-(float) yaw, glm::vec3(0, 1, 0)) * glm::vec4(position, 1);

	float pitch = atan2(-(rotatedPosition.y - eyeSysPos.y), rotatedPosition.z) * 180 / pi;

	return glm::vec2(pitch, yaw);
}

void facePoint(glm::vec3 position) {
	glm::vec2 angles = getAnglesToFacePoint(position);
	rotateX = angles.x;
	rotateY = angles.y;
}

void transitionRotation(int dt, int animationTime) {
	bool finishedAnimation = false;
	face->rotationTransitionCounter += dt;
	if (face->rotationTransitionCounter > animationTime) {
		face->rotationTransitionCounter = animationTime;
		finishedAnimation = true;
	}
	float fracHeadStart = 1 / 4.f; //Fraction through animation time to start rotating
	float headFracTimePassed = (face->rotationTransitionCounter - fracHeadStart * animationTime) * (1 / (1 - fracHeadStart)) / (float) animationTime;
	headFracTimePassed = (headFracTimePassed < 0 ? 0 : headFracTimePassed);
	float eyeFracTimePassed = face->rotationTransitionCounter * (1 / (1 - fracHeadStart)) / (float) animationTime;
	eyeFracTimePassed = (eyeFracTimePassed > 1 ? 1 : eyeFracTimePassed);

	if (!face->transitioningExpression && eyeFracTimePassed == 1 && !face->useCamera) {
		face->transitioningExpression = true;
		face->currentExpression = face->nextExpression;
		face->nextExpression = rand() % face->nexpressions;
	}

	//Rotate Head
	glm::vec2 startHeadAngles = getAnglesToFacePoint(face->startPosition);
	glm::vec2 endHeadAngles = getAnglesToFacePoint(face->endPosition);
	rotateX = startHeadAngles.x + headFracTimePassed * (endHeadAngles.x - startHeadAngles.x);
	rotateY = startHeadAngles.y + headFracTimePassed * (endHeadAngles.y - startHeadAngles.y);

	//Rotate Eye1
	glm::vec2 startEye1Angles = getAnglesToLookAtPoint(face->startPosition, eye1Info);
	glm::vec2 endEye1Angles = getAnglesToLookAtPoint(face->endPosition, eye1Info);
	eye1Info.pitch = startEye1Angles.x + eyeFracTimePassed * (endEye1Angles.x - startEye1Angles.x);
	eye1Info.yaw = startEye1Angles.y + eyeFracTimePassed * (endEye1Angles.y - startEye1Angles.y);

	//Rotate Eye2
	glm::vec2 startEye2Angles = getAnglesToLookAtPoint(face->startPosition, eye2Info);
	glm::vec2 endEye2Angles = getAnglesToLookAtPoint(face->endPosition, eye2Info);
	eye2Info.pitch = startEye2Angles.x + eyeFracTimePassed * (endEye2Angles.x - startEye2Angles.x);
	eye2Info.yaw = startEye2Angles.y + eyeFracTimePassed * (endEye2Angles.y - startEye2Angles.y);
	eye1Info.yaw = startEye1Angles.y + eyeFracTimePassed * (endEye1Angles.y - startEye1Angles.y);

	if (finishedAnimation) {
		face->transitioningRotation = false;
		face->rotationTransitionCounter = 0;
		face->startPosition = face->endPosition;
	}
	eye2Info.pitch = startEye2Angles.x + eyeFracTimePassed * (endEye2Angles.x - startEye2Angles.x);
	eye2Info.yaw = startEye2Angles.y + eyeFracTimePassed * (endEye2Angles.y - startEye2Angles.y);

	if (finishedAnimation) {
		face->transitioningRotation = false;
		face->rotationTransitionCounter = 0;
		face->startPosition = face->endPosition;
	}
}

// ========================================================================
// ResizeWindow ()
// ========================================================================
//
// This get calls when the window gets resized.
//

static void ResizeWindow(GLsizei w, GLsizei h)
		{
	h = (h == 0) ? 1 : h;
	w = (w == 0) ? 1 : w;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, (GLfloat) w / (GLfloat) h, 1.0f, 200.0f);
//    glOrtho(-w / 50.f, w / 50.f, -h / 50.f, h / 50.f, 1.f, 200.f);

// select the Modelview matrix
	glMatrixMode(GL_MODELVIEW);
}

void input(sf::Event event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::E) {
			Key_e();
		} else if (event.key.code == sf::Keyboard::B) {
			Key_b();
		} else if (event.key.code == sf::Keyboard::C) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
				Key_C();
			} else {
				Key_c();
			}
		} else if (event.key.code == sf::Keyboard::N) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
				Key_N();
			} else {
				Key_n();
			}
		} else if (event.key.code == sf::Keyboard::Up) {
			Key_up();
		} else if (event.key.code == sf::Keyboard::Down) {
			Key_down();
		} else if (event.key.code == sf::Keyboard::Left) {
			Key_left();
		} else if (event.key.code == sf::Keyboard::Right) {
			Key_right();
		} else if (event.key.code == sf::Keyboard::R) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
				Key_R();
			} else {
				Key_r();
			}
		} else if (event.key.code == sf::Keyboard::W) {
			Key_w();
		} else if (event.key.code == sf::Keyboard::H) {
			Key_h();
		} else if (event.key.code == sf::Keyboard::T) {
			Key_T();
		} else if (event.key.code == sf::Keyboard::M) {
			face->mouthOpen = !(face->mouthOpen);
		} else if (event.key.code == sf::Keyboard::Comma) {
			face->drawMuscles = !face->drawMuscles;
		} else if (event.key.code == sf::Keyboard::Q) {
			Key_q();
		} else if (event.key.code == sf::Keyboard::L) {
			Key_l();
		} else if (event.key.code == sf::Keyboard::P) {
			Key_p();
		} else if (event.key.code == sf::Keyboard::O) {
			++face->colorMode;
			face->colorMode = face->colorMode % 3;
		} else if (event.key.code == sf::Keyboard::I) {
			face->drawEyes = !face->drawEyes;
		} else if (event.key.code == sf::Keyboard::U) {
			face->useCamera = !face->useCamera;
		} else if (event.key.code == sf::Keyboard::Numpad2) {
			std::cout << "Enter botton distance from center\n";
			int bottom;
			std::cin >> bottom;
			camera.calibrateBottomTop(true, bottom);
		} else if (event.key.code == sf::Keyboard::Numpad4) {
			std::cout << "Enter left distance from center\n";
			int left;
			std::cin >> left;
			camera.calibrateSides(true, left);
		} else if (event.key.code == sf::Keyboard::Numpad5) {
			std::cout << "Enter depth distance from center\n";
			int depth;
			std::cin >> depth;
			camera.calibrateDepth(depth);
		} else if (event.key.code == sf::Keyboard::Numpad6) {
			std::cout << "Enter right distance from center\n";
			int right;
			std::cin >> right;
			camera.calibrateSides(false, right);
		} else if (event.key.code == sf::Keyboard::Numpad8) {
			std::cout << "Enter top distance from center\n";
			int top;
			std::cin >> top;
			camera.calibrateBottomTop(false, top);
		}
	} else if (event.type == sf::Event::Resized) {
		ResizeWindow(event.size.width, event.size.height);
	}

}

void updateFace(int dt) {
	if (face->transitioningExpression) {
		bool finishedAnimation = false;
		face->expressionTransitionCounter += dt;
		if (face->expressionTransitionCounter > expressionAnimationTime) {
			face->expressionTransitionCounter = expressionAnimationTime;
			finishedAnimation = true;
		}
		face_reset(face);
		transitionExpression(face, face->currentExpression, face->nextExpression, face->expressionTransitionCounter, expressionAnimationTime);
		if (finishedAnimation) {
			face->transitioningExpression = false;
			face->expressionTransitionCounter = 0;
		}
	}
	if (face->transitioningRotation) {
		transitionRotation(dt, rotationAnimationTime);
	} else if (face->lookingAround) {
		if (!face->useCamera) {
			face->rotationTransitionCounter += dt;
			if (face->rotationTransitionCounter > 2000000) {
				face->endPosition.x = (((rand() / (float) RAND_MAX) * 2) - 1) * 7;
				face->endPosition.y = (((rand() / (float) RAND_MAX) * 2) - 1) * 7;
				face->endPosition.z = 20;
				face->rotationTransitionCounter = 0;
				face->transitioningRotation = true;
			}
		}
	}

	if (face->useCamera && face->lookingAround) {
		face->rotationTransitionCounter += dt;
		Circle circle = camera.getFrameCircle();
		glm::vec3 position = camera.getObjectPosition(circle);
		position += glm::vec3(cameraX, cameraY, cameraZ);
		face->endPosition = position;
		face->transitioningRotation = true;
	}

	if (face->shouldBlink || eye1Info.blinking || eye2Info.blinking) {
		eye1Info.eyelidCounter += dt;
		eye2Info.eyelidCounter += dt;
	}

	static bool calc = true;
	static int counter = 0;
	counter += 1;
	if (counter < 15)
		calc = true;
	if (calc) {
//		lookAtX = ((rand() % 20) - 10);
//		lookAtY = ((rand() % 20) - 10);
		lookAtX = -20;
		lookAtY = 1;
		lookAtZ = 0;
//		facePoint(glm::vec3(0.36397, 0.36397, 30));
		calc = false;
		counter = 0;
	}
}

// ======================================================================== 
// Animate ()																	
// ========================================================================   
//
// The big enchilda. The animation callback
//

static void Animate(void)
		{
	GLfloat light_position[] = { 30.0, 70.0, 100.0, 1.0 };

	// clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// clear current matrix (Modelview)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// back off thirty units down the Z axis
	glTranslatef(0.0f, 0.0f, -30.0f);

//	glLineWidth(10);
//	glBegin(GL_LINES);
//	glColor3f(1, 1, 0);
//	glm::vec4 eye1Pos = glm::rotate((float) rotateX, glm::vec3(1, 0, 0)) * glm::rotate((float) rotateY, glm::vec3(0.f, 1.f, 0.f)) * glm::vec4(eye1Info.x + eyeX, eye1Info.y + eyeY, eye1Info.z + eyeZ, 1);
//	glm::vec4 pos = glm::rotate((float) rotateY, 0.f, 1.f, 0.f) * glm::rotate((float) rotateX, 1.f, 0.f, 0.f) * glm::vec4(eyeX, eyeY, eyeZ, 1);
//	glVertex3f(0, 0, 0);
//	glVertex3f(face->endPosition.x , face->endPosition.y, face->endPosition.z);
//	glEnd();

// Use the keyboard to grab the rotations
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glRotatef(rotateX, 1.0f, 0.0f, 0.0f);

	glPushMatrix();
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glTranslated(0.0, 0.0, 50.0);
	glEnable(GL_LIGHTING);
	glPopMatrix();

	//NEW BLOCK
	glDisable(GL_LIGHTING);

	if (face->drawEyes) {
		drawEyeSystem(eyeX, eyeY, eyeZ, eyePitch, eyeYaw, eyeRoll, eye1Info, eye2Info, face->shouldBlink);
	}

//COLOR FACE
	glEnable(GL_LIGHTING);

	//END NEW BLOCK

	// Transparancy
	if (face->rendermode == 3) {
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	} else {
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	// Re-calculate the polygon normals as
	// the face could have been distorted
	calculate_polygon_vertex_normal(face);
//    glDisable(GL_LIGHTING);
	paint_polygons(face, face->rendermode, 0);

	// if the rendering mode is wireframe
	// or transparent then display the muscles
//	if ( face->rendermode == 0 || 
//		 face->rendermode == 3 ) {
	glDisable(GL_LIGHTING);
	if (face->drawMuscles) {
		paint_muscles(face);
	}
//	}

	// Now flush the pipeline and
	// swap the buffers
	glFlush();
}

// ======================================================================== 
// OpenGLInit ()																	
// ========================================================================   
//
// Initialize OpenGL
//

void OpenGLInit(void)
		{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glCullFace(GL_FRONT_AND_BACK);
	glClearColor(.1, .3, .5, 1);

//	GLfloat specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
//	GLfloat shininess[] = { 1.f };
//	GLfloat ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
//	GLfloat emit[] = { 0.1f, 0.1f, 0.1f, 1.f };
//    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
//    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
//    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
//    glMaterialfv(GL_FRONT, GL_EMISSION, emit);
//    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

// ======================================================================== 
// Main ()																	
// ========================================================================   
//
// Setup OpenGL, setup the face, hook up callbacks and start the main loop.
//

int main(int argc, char** argv)
		{
	sf::ContextSettings settings(24, 8, 0, 3, 3);
	sf::Window window;
	window.create(sf::VideoMode(1024, 1024, 32), "Face", 7U, settings);
	window.setFramerateLimit(60);
	window.setPosition(sf::Vector2i(50, 190)); //Temp

	camera.openStream(0);
	camera.calibrateBottomTop(true, -15);
	camera.calibrateBottomTop(false, 15);
	camera.calibrateSides(true, -15);
	camera.calibrateSides(false, 15);
	camera.calibrateDepth(15);

	// Initialize OpenGL
	OpenGLInit();
	ResizeWindow(1024, 1024);

	// Initialize the face
	FaceInit();

	bool running = true;
	sf::Clock clock;
	int frames, counter;
	while (running) {
		int dt = clock.restart().asMicroseconds();
		frames++;
		counter += dt;
		if (counter > 1000000) {
			counter = 0;
			std::cout << frames << '\n';
			frames = 0;
		}

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				running = false;
				break;
			} else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
				running = false;
				break;
			}
			input(event);
		}
		updateFace(dt);
		Animate();
		window.display();
	}

	return (0);
}

// ======================================================================== 
// FaceInit																	
// ======================================================================== 
//
// Read in the datafiles and initialize the face data structures.
//

void FaceInit(void)
		{
	face = create_face("face-data/index.dat", "face-data/faceline.dat", "face-data/face_colors.dat", "face-data/VertexZones.dat");
	read_muscles("face-data/muscle.dat", face);
	read_expression_macros("face-data/expression-macros.dat", "face-data/expression-macros-mouth-open.dat", face);
	data_struct(face);

	eye1Info = initEyeInfo(-2.026, 0, 0, 0, 0, 0, 0, 1.1);
	eye2Info = initEyeInfo(2.026, 0, 0, 0, 0, 0, 0, 1.1);
	eyeYaw = eyePitch = eyeRoll = 0;
	eyeX = 0;
	eyeY = 2.9;
	eyeZ = 6.6;
	lookAtX = lookAtY = lookAtZ = 0;
}
