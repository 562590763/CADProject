#pragma once

#define CALLBACK __stdcall

#include "HalfEdgeDataStructure.h"
#include "Example.h"

void DrawLoop1(Loop *_loop);
void DrawLoop2(Loop *_loop);
void DrawFace1(Face *face);
void DrawFace2(Face *face);
void DrawSolid(Solid *solid, const bool _line);
void Display(Solid *solid, const bool _line);
