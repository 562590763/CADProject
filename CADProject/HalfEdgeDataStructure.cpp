#include <cstdio>

#include "HalfEdgeDataStructure.h"

Point::Point() : pos(0, 0, 0){
	color = glm::vec3(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f);
}

Point::Point(glm::vec3 _pos): Point() {
	setPos(_pos);
}

Point::Point(double posX, double posY, double posZ): Point() {
	glm::vec3 _pos(posX, posY, posZ);
	setPos(_pos);
}

Point::Point(double pos[3]): Point() {
	glm::vec3 _pos(pos[0], pos[1], pos[2]);
	setPos(_pos);
}

Point::Point(double pos[3], glm::vec3 _color): Point() {
	glm::vec3 _pos(pos[0], pos[1], pos[2]);
	setPos(_pos);
	setColor(_color);
}

Point::Point(glm::vec3 _pos, glm::vec3 _color): Point() {
	setPos(_pos);
	setColor(_color);
}

void Point::setPos(glm::vec3 _pos) {
	pos.x = _pos.x;
	pos.y = _pos.y;
	pos.z = _pos.z;
}

void Point::setColor(glm::vec3 _color) {
	color.x = _color.x;
	color.y = _color.y;
	color.z = _color.z;
}

void Point::setPoint(Point* _point) {
	setPos(_point->pos);
}

void Point::printPos() {
	printf("Point position = (%.2f, %.2f, %.2f)\n", pos.x, pos.y, pos.z);
}

bool Loop::isInnerLoop() { return innerLoop; }

void Loop::SetInnerLoop(bool state) { innerLoop = state; }

std::vector<Vertex*> Scene::getV_list() { return v_list; }

std::vector<Loop*> Scene::getLoop_list() { return l_list; }

std::vector<Solid*> Scene::getSolid_list() { return s_list; }

// 将Solid往direction方向移动
void Solid::move(glm::vec3 direction) {
	Vertex *vertexList = this->sVertexs;
	while(vertexList) {
		vertexList->point->pos += direction;
		vertexList = vertexList->nextV;
	}
}

void Scene::pushVertex(Vertex* v) { v_list.push_back(v); }

void Scene::pushLoop(Loop* lp) { l_list.push_back(lp); }

void Scene::pushSolid(Solid* s) {
	if(!s_list.empty()) {
		Solid *preS = s_list[s_list.size() - 1];
		preS->nextS = s;
		s->prevS = preS;
	}
	s_list.push_back(s); 
}

void Scene::dropVertex(Vertex *v) {
	int i = 0;
	while(v_list[i] != v) i++;
	v_list[i] = v_list[v_list.size() - 1];
	v_list.pop_back(); 
}

void Scene::dropLoop(Loop *lp) {
	int i = 0;
	while(l_list[i] != lp) i++;
	l_list[i] = l_list[l_list.size() - 1];
	l_list.pop_back();
}

void Scene::dropSolid(Solid *solid) {
	int i = 0;
	while(s_list[i] != solid) i++;
	s_list[i] = s_list[s_list.size() - 1];
	s_list.pop_back();
}
