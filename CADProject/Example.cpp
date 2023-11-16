#include "Example.h"

Example::Example() {
	for(int i = 0; i < snum; i++) scene[i] = new Scene();
	Solid *solid1 = example1(scene[0]);
	Solid *solid2 = example2(scene[0]);
	Solid *solid3 = buildLetterI(scene[2]);
	Solid *solid4 = buildLetterL(scene[2]);
	Solid *solid5 = buildLetterO(scene[2]);
	Solid *solid6 = buildLetterV(scene[2]);
	Solid *solid7 = buildLetterE(scene[2]);
	Solid *solid8 = buildLetterC(scene[2]);
	Solid *solid9 = buildLetterA(scene[2]);
	Solid *solid10 = buildLetterD(scene[2]);
	Solid *solid11 = buildFlag(scene[1]);
	Solid *solid12 = chamfer1(scene[3]);
	Solid *solid13 = chamfer2(scene[3]);
	solid1->move(glm::vec3(0, 0, 1));
	solid2->move(glm::vec3(0, 0, 1));
	solid3->move(glm::vec3(-5, 1, -3));
	solid4->move(glm::vec3(-0.8, 1, -3));
	solid5->move(glm::vec3(2, 1, -3));
	solid6->move(glm::vec3(5.2, 1, -3));
	solid7->move(glm::vec3(8.2, 1, -3));
	solid8->move(glm::vec3(-2, -5, -3));
	solid9->move(glm::vec3(1.3, -5, -3));
	solid10->move(glm::vec3(4.5, -5, -3));
	solid11->move(glm::vec3(3, -2, 0));
	solid12->move(glm::vec3(-4, -3, -1));
	solid13->move(glm::vec3(3, -3, -1));
}

Solid *Example::buildFlag(Scene *&scene) {

	const float WIDTH = 15.;
	const float HEIGHT = 10.;
	const float ROTATE_ANGLE = PI * 2 / 5.;
	const float proption1 = tan(PI / 10) / (tan(PI / 10) + tan(PI / 5));
	const float proption2 = tan(PI / 5);

	EulerOperator *opt = new EulerOperator();

	int vnum = 4 + 5 * 10;
	double pos[4][3] = {{-WIDTH / 2, HEIGHT / 2, 0}, {WIDTH / 2, HEIGHT / 2, 0}, {WIDTH / 2, -HEIGHT / 2, 0}, {-WIDTH / 2, -HEIGHT / 2, 0}};
	Point *pList[4 + 5 * 10];
	for(int i = 0; i < 4; i++) pList[i] = new Point(pos[i], glm::vec3(1.0, 0., 0.));
	Vertex *vertexList[4 + 5 * 10];
	for(int i = 0; i < 4; i++) vertexList[i] = new Vertex(pList[i]);

	// Construct the bottom face of the national flag
	Solid *s = opt->mvfs(scene, vertexList[0]);
	Face *f1 = s->sFaces;
	Loop *lp1 = f1->outerLoop;
	for(int i = 0; i < 3; i++) opt->mev(scene, vertexList[i], vertexList[i + 1], lp1);
	Face *f2 = opt->mef(scene, vertexList[3], vertexList[0], lp1);
	Loop *lp2 = f2->outerLoop;

	Face *faces[5];
	float centers[5][2] = {{10., 5.}, {5., 8.}, {3., 6.}, {3., 3.}, {5., 1.}};
	float ds[5] = {3.0, 1.0, 1.0, 1.0, 1.0};
	float base_angle[5] = {PI / 10, atan(3.0 / 5) + PI, atan(1.0 / 7) + PI, PI / 10, atan(5.0 / 4) + PI / 2};
	for(int j = 0; j < 5; j++) {
		Edge *killEdge = new Edge();
		glm::vec3 center(-WIDTH / 30.0 * centers[j][0], HEIGHT / 20.0 * centers[j][1], 0);
		float d = HEIGHT / 20.0 * ds[j];
		for(int i = 0; i < 5; i++) {
			int k = 4 + 2 * (5 * j + i);
			float angle = base_angle[j] + i * ROTATE_ANGLE;
			glm::vec3 point(center.x + d * cos(angle), center.y + d * sin(angle), 0);
			glm::vec3 cp(point.x - center.x, point.y - center.y, 0);
			glm::vec3 ch(proption1 * cp.x, proption1 * cp.y, 0);
			glm::vec3 hm(proption2 * ch.y, proption2 * -ch.x, 0);
			glm::vec3 m(center.x + ch.x + hm.x, center.y + ch.y + hm.y, 0);
			pList[k] = new Point(m, glm::vec3(1.0, 1.0, 0.));
			vertexList[k] = new Vertex(pList[k]);
			pList[k+1] = new Point(point, glm::vec3(1.0, 1.0, 0.));
			vertexList[k+1] = new Vertex(pList[k+1]);
			if(i==0) killEdge = opt->mev(scene, vertexList[0], vertexList[k], lp2);
			else opt->mev(scene, vertexList[k - 1], vertexList[k], lp2);
			opt->mev(scene, vertexList[k], vertexList[k+1], lp2);
		}
		faces[j] = opt->mef(scene, vertexList[13 + 10 * j], vertexList[4 + 10 * j], lp2);
		Loop *outerLp = faces[j]->outerLoop;
		Loop *innerLp = opt->kemr(scene, killEdge, f1); // a inner loop
	}

	// Sweep the bottom face
	glm::vec3 direct(0, 0, 1);
	opt->sweep(scene, s, f2, direct, 0.1);
	for(int j = 0; j < 5; j++) {
		opt->sweep(scene, s, faces[j], direct, 0.1);
		opt->kfmrh(scene, faces[j], f2);
	}

	return s;
}

Solid *Example::buildLetterI(Scene *&scene) {

	EulerOperator *opt = new EulerOperator();

	int vnum = 12;
	double pos[12][3] = {{-1, 2, 0}, {1, 2, 0}, {1, 1.9, 0}, {0.1, 1.9, 0},
						{0.1, -1.9, 0}, {1, -1.9, 0}, {1, -2, 0}, {-1, -2, 0},
						{-1, -1.9, 0}, {-0.1, -1.9, 0}, {-0.1, 1.9, 0}, {-1, 1.9, 0}};
	Point *pList[12];
	for(int i = 0; i < vnum; i++) pList[i] = new Point(pos[i]);
	Vertex *vertexList[12];
	for(int i = 0; i < vnum; i++) vertexList[i] = new Vertex(pList[i]);
	
	// Construct the bottom face of the letter I
	Solid *s = opt->mvfs(scene, vertexList[0]);
	Face *f1 = s->sFaces;
	Loop *lp1 = f1->outerLoop;
	for(int i = 0; i < vnum - 1; i++) opt->mev(scene, vertexList[i], vertexList[i + 1], lp1);
	Face *f2 = opt->mef(scene, vertexList[vnum-1], vertexList[0], lp1);

	// Sweep the bottom face
	opt->sweep(scene, s, f2, glm::vec3(0, 0, 1), 0.3);

	return s;
}

Solid *Example::buildLetterL(Scene *&scene) {

	EulerOperator *opt = new EulerOperator();

	int vnum = 6;
	double pos[6][3] = {{-1, 2, 0}, {-0.9, 2, 0}, {-0.9, -1.9, 0},
						{1, -1.9, 0}, {1, -2, 0}, {-1, -2, 0}};
	Point *pList[6];
	for(int i = 0; i < vnum; i++) pList[i] = new Point(pos[i]);
	Vertex *vertexList[6];
	for(int i = 0; i < vnum; i++) vertexList[i] = new Vertex(pList[i]);

	// Construct the bottom face of the letter L
	Solid *s = opt->mvfs(scene, vertexList[0]);
	Face *f1 = s->sFaces;
	Loop *lp1 = f1->outerLoop;
	for(int i = 0; i < vnum - 1; i++) opt->mev(scene, vertexList[i], vertexList[i + 1], lp1);
	Face *f2 = opt->mef(scene, vertexList[vnum-1], vertexList[0], lp1);

	// Sweep the bottom face
	opt->sweep(scene, s, f2, glm::vec3(0, 0, 1), 0.3);

	return s;
}

Solid *Example::buildLetterO(Scene *&scene) {

	EulerOperator *opt = new EulerOperator();

	int vnum = 24;
	double pos[24][3] = {{-1.25, 1.5, 0}, {-0.5, 2, 0}, {0.5, 2, 0}, {1.25, 1.5, 0},
						{1.5, 1, 0}, {1.5, -1, 0}, {1.25, -1.5, 0}, {0.5, -2, 0},
						{-0.5, -2, 0}, {-1.25, -1.5, 0}, {-1.5, -1, 0}, {-1.5, 1, 0},
						{-1.125, 1.25, 0}, {-0.5, 1.75, 0}, {0.5, 1.75, 0}, {1.125, 1.25, 0},
						{1.25, 0.75, 0}, {1.25, -0.75, 0}, {1.125, -1.25, 0}, {0.5, -1.75, 0},
						{-0.5, -1.75, 0}, {-1.125, -1.25, 0}, {-1.25, -0.75, 0}, {-1.25, 0.75, 0}};
	Point *pList[24];
	for(int i = 0; i < vnum; i++) pList[i] = new Point(pos[i]);
	Vertex *vertexList[24];
	for(int i = 0; i < vnum; i++) vertexList[i] = new Vertex(pList[i]);

	// Construct the bottom face of the letter O
	Solid *s = opt->mvfs(scene, vertexList[0]);
	Face *f1 = s->sFaces;
	Loop *lp1 = f1->outerLoop;
	for(int i = 0; i < vnum/2 - 1; i++) opt->mev(scene, vertexList[i], vertexList[i + 1], lp1);
	Face *f2 = opt->mef(scene, vertexList[vnum/2-1], vertexList[0], lp1);
	Loop *lp2 = f2->outerLoop;

	// Construct a inner loop
	Edge *killEdge1 = opt->mev(scene, vertexList[0], vertexList[vnum/2], lp2);
	for(int i = vnum/2; i < vnum - 1; i++) opt->mev(scene, vertexList[i], vertexList[i + 1], lp2);
	Face *f3 = opt->mef(scene, vertexList[vnum-1], vertexList[vnum/2], lp2);
	Loop *lp3 = f3->outerLoop;
	Loop *lp4 = opt->kemr(scene, killEdge1, f1); // a inner loop

	// Sweep the bottom face
	glm::vec3 direct(0, 0, 1);
	opt->sweep(scene, s, f2, direct, 0.3);
	opt->sweep(scene, s, f3, direct, 0.3);
	opt->kfmrh(scene, f3, f2);

	return s;
}

Solid *Example::buildLetterV(Scene *&scene) {

	EulerOperator *opt = new EulerOperator();

	int vnum = 6;
	double pos[6][3] = {{-1.5, 2, 0}, {-1.4, 2, 0}, {0, -1.9, 0},
						{1.4, 2, 0}, {1.5, 2, 0}, {0, -2, 0}};
	Point *pList[6];
	for(int i = 0; i < vnum; i++) pList[i] = new Point(pos[i]);
	Vertex *vertexList[6];
	for(int i = 0; i < vnum; i++) vertexList[i] = new Vertex(pList[i]);

	// Construct the bottom face of the letter V
	Solid *s = opt->mvfs(scene, vertexList[0]);
	Face *f1 = s->sFaces;
	Loop *lp1 = f1->outerLoop;
	for(int i = 0; i < vnum - 1; i++) opt->mev(scene, vertexList[i], vertexList[i + 1], lp1);
	Face *f2 = opt->mef(scene, vertexList[vnum-1], vertexList[0], lp1);

	// Sweep the bottom face
	opt->sweep(scene, s, f2, glm::vec3(0, 0, 1), 0.3);

	return s;
}

Solid *Example::buildLetterE(Scene *&scene) {

	EulerOperator *opt = new EulerOperator();

	int vnum = 12;
	double pos[12][3] = {{-1, 2, 0}, {1, 2, 0}, {1, 1.9, 0}, {-0.9, 1.9, 0},
						{-0.9, 0.05, 0}, {1, 0.05, 0}, {1, -0.05, 0}, {-0.9, -0.05, 0},
						{-0.9, -1.9, 0}, {1, -1.9, 0}, {1, -2, 0}, {-1, -2, 0}};
	Point *pList[12];
	for(int i = 0; i < vnum; i++) pList[i] = new Point(pos[i]);
	Vertex *vertexList[12];
	for(int i = 0; i < vnum; i++) vertexList[i] = new Vertex(pList[i]);

	// Construct the bottom face of the letter E
	Solid *s = opt->mvfs(scene, vertexList[0]);
	Face *f1 = s->sFaces;
	Loop *lp1 = f1->outerLoop;
	for(int i = 0; i < vnum - 1; i++) opt->mev(scene, vertexList[i], vertexList[i + 1], lp1);
	Face *f2 = opt->mef(scene, vertexList[vnum-1], vertexList[0], lp1);

	// Sweep the bottom face
	opt->sweep(scene, s, f2, glm::vec3(0, 0, 1), 0.3);

	return s;
}

Solid *Example::buildLetterC(Scene *&scene) {

	EulerOperator *opt = new EulerOperator();

	int vnum = 20;
	double pos[20][3] = {{-1.25, 1.5, 0}, {-0.5, 2, 0}, {0.5, 2, 0}, {1.25, 1.5, 0},
						{1.125, 1.25, 0}, {0.5, 1.75, 0}, {-0.5, 1.75, 0}, {-1.125, 1.25, 0},
						{-1.25, 0.75, 0}, {-1.25, -0.75, 0}, {-1.125, -1.25, 0}, {-0.5, -1.75, 0},
						{0.5, -1.75, 0}, {1.125, -1.25, 0}, {1.25, -1.5, 0}, {0.5, -2, 0},
						{-0.5, -2, 0}, {-1.25, -1.5, 0}, {-1.5, -1, 0}, {-1.5, 1, 0}};
	Point *pList[20];
	for(int i = 0; i < vnum; i++) pList[i] = new Point(pos[i]);
	Vertex *vertexList[20];
	for(int i = 0; i < vnum; i++) vertexList[i] = new Vertex(pList[i]);

	// Construct the bottom face of the letter C
	Solid *s = opt->mvfs(scene, vertexList[0]);
	Face *f1 = s->sFaces;
	Loop *lp1 = f1->outerLoop;
	for(int i = 0; i < vnum-1; i++) opt->mev(scene, vertexList[i], vertexList[i+1], lp1);
	Face *f2 = opt->mef(scene, vertexList[vnum-1], vertexList[0], lp1);

	// Sweep the bottom face
	opt->sweep(scene, s, f2, glm::vec3(0, 0, 1), 0.3);

	return s;
}

Solid *Example::buildLetterA(Scene *&scene) {

	EulerOperator *opt = new EulerOperator();

	int vnum = 12;
	double pos[12][3] = {{-0.3, 2, 0}, {0.3, 2, 0}, {1.5, -2, 0}, {1.35, -2, 0},
						{0.78, -0.1, 0}, {-0.78, -0.1, 0}, {-1.35, -2, 0}, {-1.5, -2, 0},
						{-0.21, 1.8, 0}, {0.21, 1.8, 0}, {0.72, 0.1, 0}, {-0.72, 0.1, 0}};
	Point *pList[12];
	for(int i = 0; i < vnum; i++) pList[i] = new Point(pos[i]);
	Vertex *vertexList[12];
	for(int i = 0; i < vnum; i++) vertexList[i] = new Vertex(pList[i]);

	// Construct the bottom face of the letter A
	Solid *s = opt->mvfs(scene, vertexList[0]);
	Face *f1 = s->sFaces;
	Loop *lp1 = f1->outerLoop;
	for(int i = 0; i < 7; i++) opt->mev(scene, vertexList[i], vertexList[i + 1], lp1);
	Face *f2 = opt->mef(scene, vertexList[7], vertexList[0], lp1);
	Loop *lp2 = f2->outerLoop;

	// Construct a inner loop
	Edge *killEdge1 = opt->mev(scene, vertexList[0], vertexList[8], lp2);
	for(int i = 8; i < vnum - 1; i++) opt->mev(scene, vertexList[i], vertexList[i + 1], lp2);
	Face *f3 = opt->mef(scene, vertexList[vnum-1], vertexList[8], lp2);
	Loop *lp3 = f3->outerLoop;
	Loop *lp4 = opt->kemr(scene, killEdge1, f1); // a inner loop

	// Sweep the bottom face
	glm::vec3 direct(0, 0, 1);
	opt->sweep(scene, s, f2, direct, 0.3);
	opt->sweep(scene, s, f3, direct, 0.3);
	opt->kfmrh(scene, f3, f2);

	return s;
}

Solid *Example::buildLetterD(Scene *&scene) {

	EulerOperator *opt = new EulerOperator();

	int vnum = 16;
	double pos[16][3] = {{-1, 2, 0}, {0.5, 2, 0}, {1.25, 1.5, 0}, {1.5, 1, 0}, {1.5, -1, 0}, {1.25, -1.5, 0}, {0.5, -2, 0}, {-1, -2, 0},
						{-0.8, 1.8, 0}, {0.5, 1.8, 0}, {1.135, 1.3, 0}, {1.35, 0.75, 0}, {1.35, -0.75, 0}, {1.135, -1.3, 0}, {0.5, -1.8, 0}, {-0.8, -1.8, 0}};
	Point *pList[16];
	for(int i = 0; i < vnum; i++) pList[i] = new Point(pos[i]);
	Vertex *vertexList[16];
	for(int i = 0; i < vnum; i++) vertexList[i] = new Vertex(pList[i]);

	// Construct the bottom face of the letter D
	Solid *s = opt->mvfs(scene, vertexList[0]);
	Face *f1 = s->sFaces;
	Loop *lp1 = f1->outerLoop;
	for(int i = 0; i < vnum / 2 - 1; i++) opt->mev(scene, vertexList[i], vertexList[i + 1], lp1);
	Face *f2 = opt->mef(scene, vertexList[vnum / 2 - 1], vertexList[0], lp1);
	Loop *lp2 = f2->outerLoop;

	// Construct a inner loop
	Edge *killEdge1 = opt->mev(scene, vertexList[0], vertexList[vnum / 2], lp2);
	for(int i = vnum / 2; i < vnum - 1; i++) opt->mev(scene, vertexList[i], vertexList[i + 1], lp2);
	Face *f3 = opt->mef(scene, vertexList[vnum - 1], vertexList[vnum / 2], lp2);
	Loop *lp3 = f3->outerLoop;
	Loop *lp4 = opt->kemr(scene, killEdge1, f1); // a inner loop

	// Sweep the bottom face
	glm::vec3 direct(0, 0, 1);
	opt->sweep(scene, s, f2, direct, 0.3);
	opt->sweep(scene, s, f3, direct, 0.3);
	opt->kfmrh(scene, f3, f2);

	return s;
}

Solid *Example::chamfer1(Scene *&scene) {

	EulerOperator *opt = new EulerOperator();

	int vnum = 11;
	double pos[8][3] = {{-2, 2, 0}, {-2, 2, 4}, {2, 2, 4}, {2, 2, 0},
						{-2, -2, 0}, {-2, -2, 4}, {2, -2, 4}, {2, -2, 0}};
	Point *pList[8];
	for(int i = 0; i < 8; i++) pList[i] = new Point(pos[i], glm::vec3(0.5, 0.5, 0.5));
	Vertex *vertexList[11];
	for(int i = 0; i < 8; i++) vertexList[i] = new Vertex(pList[i]);

	// Construct the bottom face of the cube
	Solid *s = opt->mvfs(scene, vertexList[0]);
	Face *f1 = s->sFaces;
	Loop *lp1 = f1->outerLoop;
	opt->mev(scene, vertexList[0], vertexList[1], lp1);
	Edge* eg1 = opt->mev(scene, vertexList[1], vertexList[2], lp1);
	Edge *eg2 = opt->mev(scene, vertexList[2], vertexList[3], lp1);
	Face *f2 = opt->mef(scene, vertexList[3], vertexList[0], lp1);
	Loop *lp2 = f2->outerLoop;

	// Construct the sides and top faces of the cube
	opt->mev(scene, vertexList[0], vertexList[4], lp2);
	opt->mev(scene, vertexList[3], vertexList[7], lp2);
	Face *f3 = opt->mef(scene, vertexList[4], vertexList[7], lp2);
	Edge *eg3 = opt->mev(scene, vertexList[2], vertexList[6], lp2);
	Face *f4 = opt->mef(scene, vertexList[7], vertexList[6], lp2);
	opt->mev(scene, vertexList[1], vertexList[5], lp2);
	Face *f5 = opt->mef(scene, vertexList[6], vertexList[5], lp2);
	Face *f6 = opt->mef(scene, vertexList[5], vertexList[4], lp2);

	// Chamfering
	Edge *eg4 = opt->semv(scene, eg1, 0.6);
	Edge *eg5 = opt->semv(scene, eg2, 0.4);
	Edge *eg6 = opt->semv(scene, eg3, 0.4);
	vertexList[8] = eg4->firstHe->startV;
	vertexList[9] = eg5->firstHe->startV;
	vertexList[10] = eg6->firstHe->startV;
	Face *f7 = opt->mef(scene, vertexList[8], vertexList[9], lp1);
	Face *f8 = opt->mef(scene, vertexList[9], vertexList[10], f4->outerLoop);
	Face *f9 = opt->mef(scene, vertexList[10], vertexList[8], f5->outerLoop);
	opt->kef(scene, eg4);
	opt->kef(scene, eg2);
	opt->kev(scene, vertexList[2], eg3);

	return s;
}

Solid *Example::chamfer2(Scene *&scene) {

	EulerOperator *opt = new EulerOperator();

	int vnum = 12;
	double pos[8][3] = {{2, -2, 4}, {2, 2, 4}, {2, 2, 0}, {2, -2, 0},
						{-2, -2, 4}, {-2, 2, 4}, {-2, 2, 0}, {-2, -2, 0}};
	Point *pList[8];
	for(int i = 0; i < 8; i++) pList[i] = new Point(pos[i], glm::vec3(0.5, 0.5, 0.5));
	Vertex *vertexList[12];
	for(int i = 0; i < 8; i++) vertexList[i] = new Vertex(pList[i]);

	// Construct the bottom face of the cube
	Solid *s = opt->mvfs(scene, vertexList[0]);
	Face *f1 = s->sFaces;
	Loop *lp1 = f1->outerLoop;
	Edge *eg1 = opt->mev(scene, vertexList[0], vertexList[1], lp1);
	Edge *eg9 = opt->mev(scene, vertexList[1], vertexList[2], lp1);
	Edge *eg4 = opt->mev(scene, vertexList[2], vertexList[3], lp1);
	Face *f2 = opt->mef(scene, vertexList[3], vertexList[0], lp1);
	Loop *lp2 = f2->outerLoop;

	// Construct the sides and top faces of the cube
	opt->mev(scene, vertexList[0], vertexList[4], lp2);
	opt->mev(scene, vertexList[3], vertexList[7], lp2);
	Face *f3 = opt->mef(scene, vertexList[4], vertexList[7], lp2);
	Edge *eg3 = opt->mev(scene, vertexList[2], vertexList[6], lp2);
	Face *f4 = opt->mef(scene, vertexList[7], vertexList[6], lp2);
	Edge *eg2 = opt->mev(scene, vertexList[1], vertexList[5], lp2);
	Face *f5 = opt->mef(scene, vertexList[6], vertexList[5], lp2);
	Face *f6 = opt->mef(scene, vertexList[5], vertexList[4], lp2);

	// Chamfering
	Edge *eg5 = opt->semv(scene, eg1, 0.6);
	Edge *eg6 = opt->semv(scene, eg2, 0.4);
	Edge *eg7 = opt->semv(scene, eg3, 0.4);
	Edge *eg8 = opt->semv(scene, eg4, 0.4);
	vertexList[8] = eg5->firstHe->startV;
	vertexList[9] = eg6->firstHe->startV;
	vertexList[10] = eg7->firstHe->startV;
	vertexList[11] = eg8->firstHe->startV;
	Face *f7 = opt->mef(scene, vertexList[8], vertexList[9], f6->outerLoop);
	Face *f8 = opt->mef(scene, vertexList[9], vertexList[10], f5->outerLoop);
	Face *f9 = opt->mef(scene, vertexList[10], vertexList[11], f4->outerLoop);
	Face *f10 = opt->mef(scene, vertexList[11], vertexList[8], lp1);
	opt->kef(scene, eg2);
	opt->kef(scene, eg3);
	opt->kef(scene, eg9);
	opt->kev(scene, vertexList[1], eg5);
	opt->kev(scene, vertexList[2], eg4);

	return s;
}

Solid *Example::example1(Scene *&scene) {

	EulerOperator *opt = new EulerOperator();

	int vnum = 16;
	double pos[16][3] = {{-4, 0, 0}, {-4, -3, 0}, {-1, -3, 0}, {-1, 0, 0},
						{-4, 0, 1}, {-4, -3, 1}, {-1, -3, 1}, {-1, 0, 1},
						{-3, -1, 1}, {-3, -2, 1}, {-2, -2, 1}, {-2, -1, 1},
						{-3, -1, 0}, {-3, -2, 0}, {-2, -2, 0}, {-2, -1, 0}};
	// glm::vec3 color(128, 138, 135);
	// color /= 255.f;
	Point *pList[16];
	for(int i = 0; i < vnum; i++) pList[i] = new Point(pos[i]);
	Vertex *vertexList[16];
	for(int i = 0; i < vnum; i++) vertexList[i] = new Vertex(pList[i]);

	// Construct the bottom face of the cube
	Solid *s = opt->mvfs(scene, vertexList[0]);
	Face *f1 = s->sFaces;
	Loop *lp1 = f1->outerLoop;
	opt->mev(scene, vertexList[0], vertexList[1], lp1);
	opt->mev(scene, vertexList[1], vertexList[2], lp1);
	opt->mev(scene, vertexList[2], vertexList[3], lp1);
	Face *f2 = opt->mef(scene, vertexList[3], vertexList[0], lp1);
	Loop *lp2 = f2->outerLoop;

	// Construct the sides and top faces of the cube
	opt->mev(scene, vertexList[0], vertexList[4], lp2);
	opt->mev(scene, vertexList[3], vertexList[7], lp2);
	Face *f3 = opt->mef(scene, vertexList[4], vertexList[7], lp2);
	opt->mev(scene, vertexList[2], vertexList[6], lp2);
	Face *f4 = opt->mef(scene, vertexList[7], vertexList[6], lp2);
	opt->mev(scene, vertexList[1], vertexList[5], lp2);
	Face *f5 = opt->mef(scene, vertexList[6], vertexList[5], lp2);
	Face *f6 = opt->mef(scene, vertexList[5], vertexList[4], lp2);

	// Construct a inner loop
	Edge *killEdge1 = opt->mev(scene, vertexList[4], vertexList[8], lp2);
	opt->mev(scene, vertexList[8], vertexList[9], lp2);
	opt->mev(scene, vertexList[9], vertexList[10], lp2);
	opt->mev(scene, vertexList[10], vertexList[11], lp2);
	Face *f7 = opt->mef(scene, vertexList[11], vertexList[8], lp2);
	Loop *lp7 = f7->outerLoop;
	Loop *lp8 = opt->kemr(scene, killEdge1, f2); // a inner loop
	Edge *killEdge2 = opt->mekr(scene, lp2, lp8, vertexList[4], vertexList[8]);
	lp8 = opt->kemr(scene, killEdge2, f2);

	// Construct a handle
	opt->mev(scene, vertexList[8], vertexList[12], lp7);
	opt->mev(scene, vertexList[9], vertexList[13], lp7);
	Face *f8 = opt->mef(scene, vertexList[13], vertexList[12], lp7);
	opt->mev(scene, vertexList[10], vertexList[14], lp7);
	Face *f9 = opt->mef(scene, vertexList[14], vertexList[13], lp7);
	opt->mev(scene, vertexList[11], vertexList[15], lp7);
	Face *f10 = opt->mef(scene, vertexList[15], vertexList[14], lp7);
	Face *f11 = opt->mef(scene, vertexList[12], vertexList[15], lp7);
	opt->kfmrh(scene, f7, f1);
	Face *f12 = opt->mfkrh(scene, lp7);
	opt->kfmrh(scene, f12, f1);

	return s;
}

Solid *Example::example2(Scene *&scene) {

	EulerOperator *opt = new EulerOperator();

	int vnum = 9;
	double pos[9][3] = {{1, 3, 0}, {1, -3, 0}, {5.5, 0, 0}, 
						{1.5, 2, 0}, {1.5, 0.25, 0}, {4.5, 0.25, 0}, 
						{1.5, -2, 0}, {1.5, -0.25, 0}, {4.5, -0.25, 0}};
	Point *pList[9];
	for(int i = 0; i < vnum; i++) pList[i] = new Point(pos[i]);
	Vertex *vertexList[9];
	for(int i = 0; i < vnum; i++) vertexList[i] = new Vertex(pList[i]);

	// Construct the bottom face of the triangular cylinder
	Solid *s = opt->mvfs(scene, vertexList[0]);
	Face *f1 = s->sFaces;
	Loop *lp1 = f1->outerLoop;
	opt->mev(scene, vertexList[0], vertexList[1], lp1);
	opt->mev(scene, vertexList[1], vertexList[2], lp1);
	Face *f2 = opt->mef(scene, vertexList[2], vertexList[0], lp1);
	Loop *lp2 = f2->outerLoop;

	// Construct a inner loop
	Edge *killEdge1 = opt->mev(scene, vertexList[0], vertexList[3], lp2);
	opt->mev(scene, vertexList[3], vertexList[4], lp2);
	opt->mev(scene, vertexList[4], vertexList[5], lp2);
	Face *f3 = opt->mef(scene, vertexList[5], vertexList[3], lp2);
	Loop *lp3 = f3->outerLoop;
	Loop *lp4 = opt->kemr(scene, killEdge1, f1); // a inner loop

	// Construct another inner loop
	Edge *killEdge2 = opt->mev(scene, vertexList[1], vertexList[6], lp2);
	opt->mev(scene, vertexList[6], vertexList[7], lp2);
	opt->mev(scene, vertexList[7], vertexList[8], lp2);
	Face *f4 = opt->mef(scene, vertexList[8], vertexList[6], lp2);
	Loop *lp5 = f4->outerLoop;
	Loop *lp6 = opt->kemr(scene, killEdge2, f1); // a inner loop

	// Sweep the bottom face
	glm::vec3 direct(0, 0, 1);
	opt->sweep(scene, s, f2, direct, 1.);
	opt->sweep(scene, s, f3, direct, 1.);
	opt->sweep(scene, s, f4, direct, 1.);
	opt->kfmrh(scene, f3, f2);
	opt->kfmrh(scene, f4, f2);

	return s;
}
