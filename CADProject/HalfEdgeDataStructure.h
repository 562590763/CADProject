#pragma once

#ifndef __HALF_EDGE_DATA_STRUCTURE__
#define __HALF_EDGE_DATA_STRUCTURE__

#include <vector>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

class Point;
class Curve;
class Surface;
class Vertex;
class HalfEdge;
class Edge;
class Loop;
class Face;
class Solid;
class Scene;

// ******************** Geometric elements ********************
class Point {
public:
	glm::vec3 pos;
	glm::vec3 color;
	Point();
	Point(glm::vec3 _pos);
	Point(double posX, double posY, double posZ);
	Point(double pos[3]);
	Point(double pos[3], glm::vec3 _color);
	Point(glm::vec3 _pos, glm::vec3 _color);
	void setPos(glm::vec3 _pos);
	void setColor(glm::vec3 _color);
	void setPoint(Point *_point);
	void printPos();
};

class Curve {
public:

};

class Surface {
public:

};

// ******************** Topological elements ********************
class Vertex {
public:
	int id;
	Point *point;
	Vertex *prevV, *nextV;
	HalfEdge *vhe;
	Vertex() : id(0), prevV(nullptr), nextV(nullptr), point(nullptr), vhe(nullptr) {}
	Vertex(Point* _point) : id(0), prevV(nullptr), nextV(nullptr), point(_point), vhe(nullptr) {}
};

class HalfEdge {
public:
	Vertex *startV, *endV;
	HalfEdge *prevHe, *nextHe;
	HalfEdge *adjacentHe;
	Edge *edge;
	Loop *heLoop;
	HalfEdge() : startV(nullptr), endV(nullptr), prevHe(nullptr), nextHe(nullptr),
		adjacentHe(nullptr), edge(nullptr) , heLoop(nullptr) {}
};

class Edge {
public:
	HalfEdge *firstHe, *secondHe;
	Edge *prevEg, *nextEg;
	Edge() : firstHe(nullptr), secondHe(nullptr), prevEg(nullptr), nextEg(nullptr) {}
};

class Loop {
public:
	int id;
	bool innerLoop;
	HalfEdge *lEdge;
	Loop *prevL, *nextL;
	Face *lFace;
	Loop() : id(0), lEdge(nullptr), prevL(nullptr), nextL(nullptr),
		lFace(nullptr), innerLoop(false) {}
	bool isInnerLoop();
	void SetInnerLoop(bool state);
};

class Face {
public:
	int id;
	int innum; // count of inner loops
	Loop *outerLoop, *innerLoops;
	Face *prevF, *nextF;
	Solid *fSolid;
	Face() : id(0), innum(0), outerLoop(nullptr), innerLoops(nullptr),
		prevF(nullptr), nextF(nullptr), fSolid(nullptr) {}
};

class Solid {
public:
	int id;
	int vnum; // count of vertexs
	int fnum; // count of faces
	int lnum; // count of loops

	Vertex *sVertexs;
	Edge *sEdges;
	Face *sFaces;
	Solid *prevS, *nextS;
	Scene* sScene;
	Solid() : id(0), vnum(0), fnum(0), lnum(0), sVertexs(nullptr),
		sEdges(nullptr), sFaces(nullptr), prevS(nullptr), nextS(nullptr), sScene(nullptr) {}
	void move(glm::vec3 direction);
};

// 每个项目有且只有一个Scene的实例，所有Solid都放在Scene中
class Scene {
public:
	Scene() {
		v_list.clear();
		l_list.clear();
		s_list.clear();
	}

	std::vector<Vertex*> getV_list();
	std::vector<Loop*> getLoop_list();
	std::vector<Solid*> getSolid_list();

	void pushVertex(Vertex* v);
	void pushLoop(Loop* lp);
	void pushSolid(Solid* s);

	void dropVertex(Vertex *v);
	void dropLoop(Loop *lp);
	void dropSolid(Solid *solid);

private:
	std::vector<Vertex *> v_list;
	std::vector<Loop *> l_list;
	std::vector<Solid *> s_list;
};

#endif
