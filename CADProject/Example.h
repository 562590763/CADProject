#pragma once

#include "TopologicalOperations.h"

#define PI 3.14159265358979323846

class Example {
public:

	Solid *example1(Scene *&scene);
	Solid *example2(Scene *&scene);
	Solid *buildFlag(Scene *&scene);
	Solid *buildLetterI(Scene *&scene);
	Solid *buildLetterL(Scene *&scene);
	Solid *buildLetterO(Scene *&scene);
	Solid *buildLetterV(Scene *&scene);
	Solid *buildLetterE(Scene *&scene);
	Solid *buildLetterC(Scene *&scene);
	Solid *buildLetterA(Scene *&scene);
	Solid *buildLetterD(Scene *&scene);
	Solid *chamfer1(Scene *&scene);
	Solid *chamfer2(Scene *&scene);

	// Build scenes
	int snum = 10;
	Scene *scene[10];
	Example();
};
