#pragma once

#ifndef __EULER_OPERATOR__
#define __EULER_OPERATOR__

#include "HalfEdgeDataStructure.h"

class EulerOperator {
public:
    void setPtrsOfNewEdge(Edge* eg, HalfEdge* he1, HalfEdge* he2);
    void UpdateLoopPtrs(HalfEdge *he, Loop *newLoop);
    void addVertexIntoSolid(Solid *s, Vertex *v);
    void addEdgeIntoSolid(Solid* s, Edge* eg);
    void addLoopIntoFace(Face* f, Loop* lp);
    void addFaceIntoSolid(Solid* s, Face* f);
    void dropVertexFromSolid(Solid *s, Vertex *v);
    void dropEdgeFromSolid(Solid *s, Edge *eg);
    void dropLoopFromFace(Face *f, Loop *lp);
    void dropFaceFromSolid(Solid *s, Face *f);
    void sweep(Scene*& scene, Solid* solid, Face* f, glm::vec3 direction, float distance);

    Edge *semv(Scene *&scene, Edge *eg, float t);

    Solid* mvfs(Scene*& scene, Vertex*& newVertex);
    Edge* mev(Scene*& scene, Vertex* v1, Vertex* v2, Loop* lp);
    Face* mef(Scene*& scene, Vertex* v1, Vertex* v2, Loop* lp);
    Loop* kemr(Scene*& scene, Edge* killEdge, Face* outerFace);
    void kfmrh(Scene*& scene, Face* innerFace, Face* outerFace);

    void kvfs(Scene *&scene, Vertex *vertex, Face *f, Solid *solid);
    void kev(Scene *&scene, Vertex *killV, Edge *killEdge);
    void kef(Scene *&scene, Edge *killEdge);
    Edge *mekr(Scene *&scene, Loop *outerLoop, Loop *innerLoop, Vertex *v1, Vertex *v2);
    Face *mfkrh(Scene *&scene, Loop *lp);
};

#endif
