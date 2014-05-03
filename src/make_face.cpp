// ==========================================================================
//                          MAKE_FACE_C
// =============================================================================
//
//    FUNCTION NAMES
//
//    face_reset					-- reset the face to neutral.
//    create_face					-- creates the face data
//    create_face					-- returns a pointer to the head datastructure.
//    make_face						-- makes a face from the two input files.
//    add_polygon_to_face			-- adds a polygon to the face data structure.
//    reflect_polygon 				-- reflect a polygon in the Y axis.
//    void averaged_vertex_normals	-- compute the average vertex normals.
//    data_struct 					-- create the datastructure for the face.
//
//    C SPECIFICATIONS
//
//    void face_reset 				( HEAD *face )
//    HEAD *create_face 			( char *f1, char *f2 )	
//    HEAD *create_face 			( f1, f2 )
//    make_face 					( HEAD *face ) 
//    add_polygon_to_face			( POLYGON *p, HEAD *face )
//    reflect_polygon 				( POLYGON *poly, HEAD *face ) 
//    void averaged_vertex_normals	( HEAD *face, int p, 
//                                    float *n1, float *n2, float *n3 ) 
//    data_struct 					( HEAD *face )

//    DESCRIPTION
//
//		This module is where the face data structures are created.
//		This module comes as is with no warranties.  
//
//    HISTORY
//		16-Dec-94  Keith Waters (waters@crl.dec.com) Created at Digital Equipment
//			       Cambridge Research Labatory
//		10-Aug-98  Keith Waters (waters@crl.dec.com) Modified for OpenGL on W95 and 
//				   WNT at Compaq Cambridge Research Laboratory
//
//============================================================================ 

#include <math.h>           // C header for any math functions               
#include <stdio.h>          // C header for standard I/O                     
#include "memory.h"         // Local memory allocation macros                
#include "Head.h"			// local header for the face

#include <iostream>

void read_polygon_line(const char*, HEAD*);
void read_polygon_indices(const char*, HEAD*);
void read_vertex_constrictors(const char *, HEAD*);
void make_face(HEAD*);
void add_polygon_to_face(POLYGON*, HEAD*);
void reflect_polygon(POLYGON*, HEAD*);
void data_struct(HEAD*);
void activate_muscle(HEAD*, int, float);

// ========================================================================= 
// face_reset	                                                             
// ========================================================================= 
//
// Resets the geometry of the face to neutral.
//

void face_reset(HEAD *face) {
	int i, j, k;

	for (i = 0; i < face->npolygons; i++) {

		for (j = 0; j < 3; j++) {

			for (k = 0; k < 3; k++) {
				face->polygon[i]->vertex[j]->xyz[k] = face->polygon[i]->vertex[j]->nxyz[k];
			}
		}
	}
}

// =========================================================================   
// create_face	                                                             
// ========================================================================= 
//
// create the default structures for the face and retrun a pointer.
//

HEAD *create_face(const char *f1, const char *f2, const char * f3) {
	HEAD *h;

	h = _new ( HEAD );

	h->mouthOpen = false;

	h->transitioningRotation = false;
	h->rotationTransitionCounter = 0;

    h->transitioningExpression = false;
    h->expressionTransitionCounter = 0;
    h->currentExpression = 0;
    h->nextExpression = 1;
    
	h->npolygons = 0;
	h->npindices = 0;
	h->npolylinenodes = 0;
    h->nvertexconstrictiontags = 0;
	h->nmuscles = 0;
	h->current_muscle = 0;
	h->current_exp = 0;
    //h->rendermode = 0;
	h->rendermode = 1;//TODO Change back to 0

	read_polygon_indices(f1, h);
	read_polygon_line(f2, h);
    read_vertex_constrictors(f3, h);

	make_face(h);

	return (h);

}

// ========================================================================= 
// make_face                                                                 
// ========================================================================= 
//
// Construct the face from the two input files.
//

void make_face(HEAD *face) {
	POLYGON *p;
	int i, ii, j, k, p1, p2, p3, p4;
	int parray[4];

	for (i = 0, ii = 0; i < face->npindices; i++, ii += 4) {

		p1 = face->indexlist[ii] - 1;
		p2 = face->indexlist[ii + 1] - 1;
		p3 = face->indexlist[ii + 2] - 1;
		p4 = face->indexlist[ii + 3] - 1;

		for (j = 0; j < 4; j++)
			parray[j] = face->indexlist[ii + j] - 1;

		if (p1 == 999) {

			p = _new ( POLYGON );
			for (j = 0; j < 3; j++) {
				p->vertex[j] = _new ( VERTEX );
				p->vertex[j]->np = 0;
                p->vertex[j]->isLL = face->tagList[parray[j + 1] * 4];
                p->vertex[j]->isUL = face->tagList[parray[j + 1] * 4 + 1];
                p->vertex[j]->isLEL = face->tagList[parray[j + 1] * 4 + 2];
                p->vertex[j]->isUEL = face->tagList[parray[j + 1] * 4 + 3];
			}

			for (j = 0; j < 3; j++)
				p->vertex[0]->nxyz[j] = p->vertex[0]->xyz[j] = face->polyline[p2 * 3 + j];
            

			for (j = 0; j < 3; j++)
				p->vertex[1]->nxyz[j] = p->vertex[1]->xyz[j] = face->polyline[p3 * 3 + j];

			for (j = 0; j < 3; j++)
				p->vertex[2]->nxyz[j] = p->vertex[2]->xyz[j] = face->polyline[p4 * 3 + j];
            
			add_polygon_to_face(p, face);
			reflect_polygon(p, face);
		} else {
			p = _new ( POLYGON );
			for (j = 0; j < 3; j++) {
				p->vertex[j] = _new ( VERTEX );
				p->vertex[j]->np = 0;
                p->vertex[j]->isLL = face->tagList[parray[j] * 4];
                p->vertex[j]->isUL = face->tagList[parray[j] * 4 + 1];
                p->vertex[j]->isLEL = face->tagList[parray[j] * 4 + 2];
                p->vertex[j]->isUEL = face->tagList[parray[j] * 4 + 3];
			}

			for (k = 0; k < 3; k++) {
				for (j = 0; j < 3; j++)
					p->vertex[k]->nxyz[j] = p->vertex[k]->xyz[j] = face->polyline[parray[k] * 3 + j];
			}

			add_polygon_to_face(p, face);
			reflect_polygon(p, face);

			p = _new ( POLYGON );
			for (j = 0; j < 3; j++) {
                int poly = (j == 0 ? p1 : (j == 1 ? p3 : p4));
				p->vertex[j] = _new ( VERTEX );
				p->vertex[j]->np = 0;
                p->vertex[j]->isLL = face->tagList[poly * 4];
                p->vertex[j]->isUL = face->tagList[poly * 4 + 1];
                p->vertex[j]->isLEL = face->tagList[poly * 4 + 2];
                p->vertex[j]->isUEL = face->tagList[poly * 4 + 3];
			}

			for (j = 0; j < 3; j++)
				p->vertex[0]->nxyz[j] = p->vertex[0]->xyz[j] = face->polyline[p1 * 3 + j];

			for (j = 0; j < 3; j++)
				p->vertex[1]->nxyz[j] = p->vertex[1]->xyz[j] = face->polyline[p3 * 3 + j];

			for (j = 0; j < 3; j++)
				p->vertex[2]->nxyz[j] = p->vertex[2]->xyz[j] = face->polyline[p4 * 3 + j];

			add_polygon_to_face(p, face);
			reflect_polygon(p, face);
		}
	}

}

// ========================================================================= 
// add_polygon_to_face                                                       
// ========================================================================= 
//
// add a polygon to the face structure.
//

void add_polygon_to_face(POLYGON *p, HEAD *face) {
	int nn;

	if (face->npolygons == 0)
		face->polygon = _new_array(POLYGON *, 500);
	else if (face->npolygons % 500 == 0)
		face->polygon = _resize_array(face->polygon,POLYGON *,face->npolygons+500);

	nn = face->npolygons;
	face->polygon[nn] = p;

	face->npolygons++;

}

// ========================================================================= 
// reflect_polygon                                                           
// ========================================================================= 
//
//  Reflects all the polygons in the half-face and adds them to
//  the data structure.
//

void reflect_polygon(POLYGON *poly, HEAD *face) {
	POLYGON *newp;
	float temp[3];
	int i, j;

	// Allocate memory for the new polygon.
	newp = _new ( POLYGON );
	for (j = 0; j < 3; j++) {
        int index = (j == 0 ? 1 : (j == 1 ? 0 : 2));
		newp->vertex[j] = _new ( VERTEX );
		newp->vertex[j]->np = 0;
        newp->vertex[j]->isLL = poly->vertex[index]->isLL;
        newp->vertex[j]->isUL = poly->vertex[index]->isUL;
        newp->vertex[j]->isLEL = poly->vertex[index]->isLEL;
        newp->vertex[j]->isUEL = poly->vertex[index]->isUEL;
	}

	// Load the old polygon values.
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			newp->vertex[i]->nxyz[j] = newp->vertex[i]->xyz[j] = poly->vertex[i]->xyz[j];

	// flip the X component.
	for (i = 0; i < 3; i++)
		newp->vertex[i]->nxyz[0] = newp->vertex[i]->xyz[0] = -newp->vertex[i]->xyz[0];

	// Re-order the vertices, flip 0 and 1.
	for (j = 0; j < 3; j++)
		temp[j] = newp->vertex[0]->xyz[j];

	for (j = 0; j < 3; j++)
		newp->vertex[0]->nxyz[j] = newp->vertex[0]->xyz[j] = newp->vertex[1]->xyz[j];

	for (j = 0; j < 3; j++)
		newp->vertex[1]->nxyz[j] = newp->vertex[1]->xyz[j] = temp[j];

	add_polygon_to_face(newp, face);
}

// ========================================================================= 
// averaged_vertex_normals                                                   
// ========================================================================= 
//
// Caculates the averaged polygon normal.
//

void averaged_vertex_normals(HEAD *face, int p, float *n1, float *n2, float *n3) {
	int i, j, np, pt;
	float norm[3];

	for (i = 0; i < 3; i++)
		norm[i] = 0.0;

	np = face->polygon[p]->vertex[0]->np;

	for (i = 0; i < np; i++) {
		pt = face->polygon[p]->vertex[0]->plist[i];

		for (j = 0; j < 3; j++) {
			norm[j] += face->polygon[pt]->vertex[0]->norm[j];
		}
	}

	for (i = 0; i < 3; i++)
		norm[i] = norm[i] / (float) np;

	for (i = 0; i < 3; i++)
		n1[i] = norm[i];

	for (i = 0; i < 3; i++)
		norm[i] = 0.0;

	np = face->polygon[p]->vertex[1]->np;

	for (i = 0; i < np; i++) {
		pt = face->polygon[p]->vertex[1]->plist[i];

		for (j = 0; j < 3; j++) {
			norm[j] += face->polygon[pt]->vertex[1]->norm[j];
		}
	}

	for (i = 0; i < 3; i++)
		norm[i] = norm[i] / (float) np;

	for (i = 0; i < 3; i++)
		n2[i] = norm[i];

	for (i = 0; i < 3; i++)
		norm[i] = 0.0;

	np = face->polygon[p]->vertex[2]->np;

	for (i = 0; i < np; i++) {
		pt = face->polygon[p]->vertex[2]->plist[i];

		for (j = 0; j < 3; j++) {
			norm[j] += face->polygon[pt]->vertex[2]->norm[j];
		}
	}

	for (i = 0; i < 3; i++)
		norm[i] = norm[i] / (float) np;

	for (i = 0; i < 3; i++)
		n3[i] = norm[i];

}

// ========================================================================= 
// data_struct	                                                             
// ========================================================================= 
//
// Create a new data structure for the polygons.
//
//

#define DATA_STRUCT_DEBUG 0

void data_struct(HEAD *face) {
	int i, j, n;
	int flag, cptr;
	float x1, y1, z1, x2, y2, z2, x3, y3, z3;
	float tx1, ty1, tz1, tx2, ty2, tz2, tx3, ty3, tz3;

	for (i = 0; i < face->npolygons; i++) {

		x1 = face->polygon[i]->vertex[0]->xyz[0];
		y1 = face->polygon[i]->vertex[0]->xyz[1];
		z1 = face->polygon[i]->vertex[0]->xyz[2];

		x2 = face->polygon[i]->vertex[1]->xyz[0];
		y2 = face->polygon[i]->vertex[1]->xyz[1];
		z2 = face->polygon[i]->vertex[1]->xyz[2];

		x3 = face->polygon[i]->vertex[2]->xyz[0];
		y3 = face->polygon[i]->vertex[2]->xyz[1];
		z3 = face->polygon[i]->vertex[2]->xyz[2];
#if DATA_STRUCT_DEBUG
		fprintf (stderr,"BASE polygon: %d\n", i);
		fprintf (stderr,"x1: %f y1: %f z1: %f\n", x1,y1,z1);
		fprintf (stderr,"x1: %f y1: %f z1: %f\n", x2,y2,z2);
		fprintf (stderr,"x1: %f y1: %f z1: %f\n", x3,y3,z3);
#endif
		j = 0;
		flag = 0;
		while (!flag && j < face->npolygons) {

			tx1 = face->polygon[j]->vertex[0]->xyz[0];
			ty1 = face->polygon[j]->vertex[0]->xyz[1];
			tz1 = face->polygon[j]->vertex[0]->xyz[2];

			tx2 = face->polygon[j]->vertex[1]->xyz[0];
			ty2 = face->polygon[j]->vertex[1]->xyz[1];
			tz2 = face->polygon[j]->vertex[1]->xyz[2];

			tx3 = face->polygon[j]->vertex[2]->xyz[0];
			ty3 = face->polygon[j]->vertex[2]->xyz[1];
			tz3 = face->polygon[j]->vertex[2]->xyz[2];
#if DATA_STRUCT_DEBUG
			fprintf (stderr, "COMPARED TO polygon: %d\n", j);
			fprintf (stderr,"tx1: %f ty1: %f tz1: %f\n", tx1,ty1,tz1);
			fprintf (stderr,"tx1: %f ty1: %f tz1: %f\n", tx2,ty2,tz2);
			fprintf (stderr,"tx1: %f ty1: %f tz1: %f\n", tx3,ty3,tz3);
#endif	
			if (((x1 == tx1) && (y1 == ty1) && (z1 == tz1)) || ((x1 == tx2) && (y1 == ty2) && (z1 == tz2)) || ((x1 == tx3) && (y1 == ty3) && (z1 == tz3))) {
				cptr = j;
#if DATA_STRUCT_DEBUG
				fprintf (stderr,"found a vertex match on polygon: %d and %d\n", i,j);
#endif
				n = face->polygon[i]->vertex[0]->np;
				face->polygon[i]->vertex[0]->plist[n] = cptr;
				face->polygon[i]->vertex[0]->np++;
#if DATA_STRUCT_DEBUG
				fprintf (stderr,"loaded: %d onto polygon: %d vertex[0]\n", cptr, i);
				fprintf (stderr,"total on vertex: %d\n", face->polygon[i]->vertex[0]->np);
#endif
			}
			j++;

		} // end while

		j = 0;
		flag = 0;
		while (!flag && j < face->npolygons) {
			tx1 = face->polygon[j]->vertex[0]->xyz[0];
			ty1 = face->polygon[j]->vertex[0]->xyz[1];
			tz1 = face->polygon[j]->vertex[0]->xyz[2];

			tx2 = face->polygon[j]->vertex[1]->xyz[0];
			ty2 = face->polygon[j]->vertex[1]->xyz[1];
			tz2 = face->polygon[j]->vertex[1]->xyz[2];

			tx3 = face->polygon[j]->vertex[2]->xyz[0];
			ty3 = face->polygon[j]->vertex[2]->xyz[1];
			tz3 = face->polygon[j]->vertex[2]->xyz[2];

			if ((x2 == tx1 && y2 == ty1 && z2 == tz1) || (x2 == tx2 && y2 == ty2 && z2 == tz2) || (x2 == tx3 && y2 == ty3 && z2 == tz3)) {
				cptr = j;

				n = face->polygon[i]->vertex[1]->np;
				face->polygon[i]->vertex[1]->plist[n] = j;
				face->polygon[i]->vertex[1]->np++;

			}
			j++;

		} // end while

		j = 0;
		flag = 0;
		while (!flag && j < face->npolygons) {
			tx1 = face->polygon[j]->vertex[0]->xyz[0];
			ty1 = face->polygon[j]->vertex[0]->xyz[1];
			tz1 = face->polygon[j]->vertex[0]->xyz[2];

			tx2 = face->polygon[j]->vertex[1]->xyz[0];
			ty2 = face->polygon[j]->vertex[1]->xyz[1];
			tz2 = face->polygon[j]->vertex[1]->xyz[2];

			tx3 = face->polygon[j]->vertex[2]->xyz[0];
			ty3 = face->polygon[j]->vertex[2]->xyz[1];
			tz3 = face->polygon[j]->vertex[2]->xyz[2];

			if ((x3 == tx1 && y3 == ty1 && z3 == tz1) || (x3 == tx2 && y3 == ty2 && z3 == tz2) || (x3 == tx3 && y3 == ty3 && z3 == tz3)) {
				cptr = j;

				n = face->polygon[i]->vertex[2]->np;
				face->polygon[i]->vertex[2]->plist[n] = cptr;
				face->polygon[i]->vertex[2]->np++;

			}
			j++;

		} // end while
	}  // end for i
}

// ========================================================================= 
//	make_expession
// ========================================================================= 
//
// Produces the facial expressions as indicated by the muscle contraction
// vector macro.
//

void make_expression(HEAD *face, int e) {
#if 0
	FILE *OutFile;
#endif
	int m;

	fprintf(stderr, "\n%s\n", face->expression[e]->name );

#if 0
//  Check the FileName
	if (( OutFile = fopen ( "c:/temp/expressiondata.dat", "w+" )) == 0 )
	fprintf (stderr, "Can't open:%s\n", OutFile );
	else
	fprintf (OutFile, "%s\n",face->expression[e]->name );
#endif

	for (m = 0; m < face->nmuscles; m++) {
		float muscle_contraction;
		muscle_contraction = face->expression[e]->m[m];

		activate_muscle(face, m, muscle_contraction);

		fprintf(stderr, "muscle:%d contract: %f\n", m, muscle_contraction );
	}

#if 0
				fprintf (OutFile, "%s\n","===============");
				fclose ( OutFile );
#endif

			}

void transitionExpression(HEAD *face, int e1, int e2, int timePassed, int totalTime){
    printf("\n%s to %s\n", face->expression[e1]->name, face->expression[e2]->name);

    for(int m = 0; m < face->nmuscles; ++m){
        float fraction = (float) timePassed / totalTime;
//        float muscleContraction = (face->expression[e1]->m[m] * (1 - fraction)) + (face->expression[e2]->m[m] * fraction);//Linear

//        float muscleContraction = ((face->expression[e2]->m[m] - face->expression[e1]->m[m]) * fraction) + face->expression[e1]->m[m];//Linear

        float cosFrac = (cos(fraction * 3.14159265358979f) + 1) / 2;
        float muscleContraction = (face->expression[e1]->m[m] * cosFrac) + (face->expression[e2]->m[m] * (1 - cosFrac));//Cosine

        activate_muscle(face, m, muscleContraction);
    }
}

