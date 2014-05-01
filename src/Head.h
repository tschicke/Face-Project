#ifndef __HEAD__
#define __HEAD__

#include <glm/glm.hpp>

typedef struct TAG {

	int poly;      // an index to a tagged polygon
	int vert;      // an index to the tagged vertex

} TAG;

typedef struct EXPRESSION {

	char name[80];    // name of the expression
	float m[20];    // an expression vector
	float bias;    // an bias control for the muscles

} EXPRESSION;

typedef struct MUSCLE {

	int active;     // activity switch for the muscle
	float head[3];     // head of the muscle vector
	float tail[3];     // tail of the muscle vector
	float zone,                // zone of influence
			fs, fe, mval;     // zone, start, end, contraction
	char name[80];     // name of the muscle
	float clampv;     // clamping value
	float mstat;     // current contraction value
    
    bool canAffectLowerLips;
    bool canAffectUpperLips;
    bool canAffectLowerEyeLids;
    bool canAffectUpperEyeLids;

} MUSCLE;

typedef struct VERTEX {

	float xyz[3];     // x,y,z of the vertex (modified)
	float nxyz[3];     // x,y,z of the vertex (never modified)
	int np;     // number of polygons associated with node
	int plist[30];     // list of polygons associated with node
	float norm[3];     // polygon vertex normal
    
    bool isLL, isUL, isUEL, isLEL;
    
} VERTEX;

typedef struct POLYGON {

	VERTEX *vertex[3];     // pointer to an array of three vertices

} POLYGON;

typedef struct HEAD {
    
	bool mouthOpen;

	bool eyesLooking;
	bool headLooking;
	int lookingCounter;
	glm::vec3 targetPoint;
	glm::vec3 lastPoint;

    bool transitioning;
    int transitionCounter;
    int currentExpression;
    int nextExpression;

	int npindices;  // number of polygon indices
	int *indexlist;  // integer index list of size npindices*4

	int npolylinenodes;  // number of nodes in the poly line
	float *polyline;  // xyz nodes in the poly line
    
    int nvertexconstrictiontags;
    bool * tagList;

	int npolygons;  // total number of polygons
	POLYGON **polygon;  // pointer to the polygon list

	int neyelidtags;  // number of eyelid tags
	TAG **eyelidtag;  // pointer to the eyelid tags
	float eyelidang;  // rotation of the eyelids

	int njawtags;  // number of jaw tags
	TAG **jawtag;  // pointer to the jaw tags
	float jawang;  // rotation of the jaw

	int nmuscles;  // number of muscles in the face
	MUSCLE **muscle;  // pointer to the muscle list

	int nexpressions;  // number of expressions in the
	EXPRESSION **expression;  // point to an expression vector

	int rendermode;  // wireframe, flat, smooth or textured
	int current_muscle;  // the current muscle to be activated
	int current_exp;  // the current facial expression

	int TextureFunction;  // The texture function DECAL or MODULATE

} HEAD;

struct EyeInfo{
    float x, y, z;
    int pitch, yaw, roll;
    float size;
    float radius;
};

#endif

