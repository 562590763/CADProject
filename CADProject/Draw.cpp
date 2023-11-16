#include "Draw.h"

#include <iostream>

#include <GL/glut.h>
#include <GL/freeglut_ext.h>

struct TessData {
    GLdouble data[6];
    TessData(glm::vec3 point, glm::vec3 color) {
        data[0] = point.x;
        data[1] = point.y;
        data[2] = point.z;
        data[3] = color.r;
        data[4] = color.g;
        data[5] = color.b;
    }
};

// GLU_TESS CALLBACKS
void CALLBACK tessBeginCB(GLenum which) {
    glBegin(which);
}

void CALLBACK tessEndCB() {
    glEnd();
}

void CALLBACK tessVertexCB(const GLvoid *data) {
    // cast back to double type
    const GLdouble *ptr = (const GLdouble *)data;
    glColor3dv(ptr + 3);
    glVertex3dv(ptr);
}

void DrawLoop1(Loop *_loop) {
    GLuint id = glGenLists(1); // create a display list
    if(!id)
        return; // failed to create a list, return 0

    GLUtesselator *tess = gluNewTess(); // create a tessellator
    if(!tess)
        return; // failed to create tessellation object, return 0

    GLdouble quad1[6] = {-1, 3, 0, 1, 1, 1};
    GLdouble quad2[6] = {0, 0, 0, 1, 1, 1};
    GLdouble quad3[6] = {1, 3, 0, 1, 1, 1};
    GLdouble quad4[6] = {0, 2, 0, 1, 1, 1};
    GLdouble quad5[6] = {-1, 5, 0, 1, 1, 1};

    // register callback functions
    gluTessCallback(tess, GLU_TESS_BEGIN, (void(CALLBACK *)())tessBeginCB);
    gluTessCallback(tess, GLU_TESS_END, (void(CALLBACK *)())tessEndCB);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void(CALLBACK *)())tessVertexCB);

    glNewList(id, GL_COMPILE);
    glColor3f(1, 1, 1);
    gluTessBeginPolygon(tess, 0); // with NULL data
    gluTessBeginContour(tess);
    HalfEdge *startHe = _loop->lEdge;
    HalfEdge *he = _loop->lEdge;
    Vertex *startV = he->startV;
    vector<TessData> quadList;
    do {
        glm::vec3 point = he->startV->point->pos;
        glm::vec3 color = he->startV->point->color;
        TessData data(point, color);
        quadList.push_back(data);
        he = he->nextHe;
    } while (nullptr != he && he != startHe);
    for(int i = 0; i < quadList.size(); i++) {
        //glColor3f(quadList[i].data[3], quadList[i].data[4], quadList[i].data[5]);
        gluTessVertex(tess, quadList[i].data, quadList[i].data);
    }
    gluTessEndContour(tess);
    gluTessEndPolygon(tess);
    glEndList();

    gluDeleteTess(tess); // delete after tessellation

    glCallList(id);
}

void DrawLoop2(Loop *loop) {
	glBegin(GL_LINE_LOOP);
	GLdouble *pos, *color;
	HalfEdge *he = loop->lEdge;
	Vertex *firstVertex = he->startV;
	do {
		pos = new GLdouble[3]{he->startV->point->pos.x, he->startV->point->pos.y, he->startV->point->pos.z};
		color = new GLdouble[3]{he->startV->point->color.r, he->startV->point->color.g, he->startV->point->color.b};
		glColor3f(color[0], color[1], color[2]);
		glVertex3f(pos[0], pos[1], pos[2]);
		//printf("Drawing pos = %.2lf, %.2lf, %.2lf\n", pos[0], pos[1], pos[2]);
		he = he->nextHe;
	} while(he->startV != firstVertex);
	glEnd();
}

void DrawFace1(Face *face) {
	// Draw inner loop
	glEnable(GL_STENCIL_TEST);
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);
	// Write 1's into stencil buffer where the hole will be
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glStencilFunc(GL_ALWAYS, 1, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	Loop *loop = face->innerLoops;
	while(loop) {
		DrawLoop1(loop);
		loop = loop->nextL;
	}

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_EQUAL, 0, ~0);
	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// Draw outer loop
	DrawLoop1(face->outerLoop);

	glDisable(GL_STENCIL_TEST);
}

void DrawFace2(Face *face) {
    // Draw inner loop
    Loop *loop = face->innerLoops;
    while(loop) {
        DrawLoop2(loop);
        loop = loop->nextL;
    }
    // Draw outer loop
    DrawLoop2(face->outerLoop);
}

void DrawSolid(Solid *solid, const bool _line) {
	Face *face = solid->sFaces;
	while(face) {
        if(!_line) DrawFace1(face);
        else {
            // Draw only the border
            DrawFace2(face);
        }
		face = face->nextF;
	}
}

void Display(Solid *solid, const bool _line) {
	Solid *tempSolid = solid;
	while(tempSolid) {
		DrawSolid(tempSolid, _line);
		tempSolid = tempSolid->nextS;
	}
}
