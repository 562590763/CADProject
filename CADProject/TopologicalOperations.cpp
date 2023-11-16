#include "TopologicalOperations.h"

// ******************** Other operations ********************
// 设置新生成边的指针
void EulerOperator::setPtrsOfNewEdge(Edge* eg, HalfEdge* he1, HalfEdge* he2) {

    // Set ptrs of HalfEdges
    he1->edge = he2->edge = eg;
    he1->adjacentHe = he2; he2->adjacentHe = he1;

    // Set ptrs of Edge
    eg->firstHe = he1, eg->secondHe = he2;
}

// 设置新环内各个半边的环指针
void EulerOperator::UpdateLoopPtrs(HalfEdge* he, Loop* newLoop) {
    HalfEdge *heTemp = he->nextHe;
    while(heTemp && heTemp != he) {
        heTemp->heLoop = newLoop;
        heTemp = heTemp->nextHe;
    }
}

// 将新生成的点放入体中
void EulerOperator::addVertexIntoSolid(Solid *s, Vertex *v) {
    v->id = s->vnum++;

    Vertex *vertexList = s->sVertexs;
    if(vertexList == nullptr) {
        s->sVertexs = v;
    } else {
        while(vertexList->nextV != nullptr) vertexList = vertexList->nextV;
        vertexList->nextV = v;
        v->prevV = vertexList;
    }
}

// 将新生成的边放入体中
void EulerOperator::addEdgeIntoSolid(Solid* s, Edge* eg) {
    Edge* edgeList = s->sEdges;
    if (edgeList == nullptr) {
        s->sEdges = eg;
    } else {
        while (edgeList->nextEg != nullptr) edgeList = edgeList->nextEg;
        edgeList->nextEg = eg;
        eg->prevEg = edgeList;
    }
}

// 将新生成的环放入面中
void EulerOperator::addLoopIntoFace(Face* f, Loop* lp) {
    lp->lFace = f;

    if (f->outerLoop == nullptr) {
        // Only one outer loop
        f->outerLoop = lp;
    } else {
        // May be multiple inner loops
        Loop* tmpLoop = f->innerLoops;
        if(tmpLoop == nullptr) {
            f->innerLoops = lp;
            f->outerLoop->nextL = lp;
            lp->prevL = f->outerLoop;
        } else {
            while (tmpLoop->nextL != nullptr) tmpLoop = tmpLoop->nextL;
            tmpLoop->nextL = lp;
            lp->prevL = tmpLoop;
        }
        lp->SetInnerLoop(true);
        ++f->innum;
    }
}

// 将新生成的面放入体中
void EulerOperator::addFaceIntoSolid(Solid* s, Face* f) {
    f->fSolid = s;
    f->id = s->fnum++;

    Face* tempFace = s->sFaces;
    if (tempFace == nullptr) s->sFaces = f;
    else {
        while (tempFace->nextF != nullptr) tempFace = tempFace->nextF;
        tempFace->nextF = f;
        f->prevF = tempFace;
    }
}

// 将点从体中删除
void EulerOperator::dropVertexFromSolid(Solid *s, Vertex *v) {
    s->vnum--;

    Vertex *vertexList = s->sVertexs;
    while(vertexList) {
        if(vertexList->id > v->id) vertexList->id--;
        vertexList = vertexList->nextV;
    }

    vertexList = s->sVertexs;
    if(vertexList == v) {
        s->sVertexs = v->nextV;
        if(s->sVertexs != nullptr) s->sVertexs->prevV = nullptr;
    } else {
        while(vertexList->nextV != v) vertexList = vertexList->nextV;
        vertexList->nextV = v->nextV;
        if(v->nextV != nullptr) v->nextV->prevV = vertexList;
    }
    delete v;
}

// 将边从体中删除
void EulerOperator::dropEdgeFromSolid(Solid *s, Edge *eg) {
    Edge *edgeList = s->sEdges;
    if(edgeList == eg) {
        s->sEdges = eg->nextEg;
        if(s->sEdges != nullptr) s->sEdges->prevEg = nullptr;
    } else {
        while(edgeList->nextEg != eg) edgeList = edgeList->nextEg;
        edgeList->nextEg = eg->nextEg;
        if(eg->nextEg != nullptr) eg->nextEg->prevEg = edgeList;
    }
    delete eg->firstHe, eg->secondHe, eg;
}

// 将环从面中删除
void EulerOperator::dropLoopFromFace(Face *f, Loop *lp) {

    if(f->outerLoop == lp) {
        f->outerLoop = nullptr;
    } else {
        Loop *tmpLoop = f->innerLoops;
        if(tmpLoop == lp) f->innerLoops = tmpLoop->nextL;
        while(tmpLoop != lp) tmpLoop = tmpLoop->nextL;
        tmpLoop->prevL->nextL = tmpLoop->nextL;
        if(tmpLoop->nextL) tmpLoop->nextL->prevL = tmpLoop->prevL;
        --f->innum;
    }
    delete lp;
}

// 将面从体中删除
void EulerOperator::dropFaceFromSolid(Solid *s, Face *f) {
    s->fnum--;

    Face *faceList = s->sFaces;
    while(faceList) {
        if(faceList->id > f->id) faceList->id--;
        faceList = faceList->nextF;
    }

    faceList = s->sFaces;
    if(faceList == f) {
        s->sFaces = f->nextF;
        if(s->sFaces != nullptr) s->sFaces->prevF = nullptr;
    } else {
        while(faceList->nextF != f) faceList = faceList->nextF;
        faceList->nextF = f->nextF;
        if(f->nextF != nullptr) f->nextF->prevF = faceList;
    }
    delete f;
}

// 扫掠操作
void EulerOperator::sweep(Scene*& scene, Solid* solid, Face* f, glm::vec3 direction, float distance) {
    // Up* are topological elements on the new face
    Loop* lp = f->outerLoop;
    HalfEdge* he = lp->lEdge;
    Vertex* firstV = he->startV;
    Vertex* nextV = he->endV;

    Point* upPoint = new Point(firstV->point->pos + distance * direction);
    Vertex* upV = new Vertex(upPoint);
    Vertex *firstUpV = upV;
    mev(scene, firstV, upV, lp);
    Vertex* preUpV = upV;

    while (nextV != firstV){
        he = he->nextHe;
        Point* upPoint = new Point(nextV->point->pos + distance * direction);
        upV = new Vertex(upPoint);
        mev(scene, nextV, upV, lp);
        mef(scene, upV, preUpV, lp);
        preUpV = upV;
        nextV = he->endV;
    }

    mef(scene, firstUpV, preUpV, lp);
}

// ******************** Euler operations ********************
// 从无到有的构造一个点、一个面、一个体
Solid* EulerOperator::mvfs(Scene*& scene, Vertex*& newVertex) {

    // Create topological elements
    Loop* newLoop = new Loop();
    Face* newFace = new Face();
    Solid* newSolid = new Solid();

    // Set id
    newLoop->id = newSolid->lnum++;

    scene->pushVertex(newVertex);
    scene->pushLoop(newLoop);
    scene->pushSolid(newSolid);

    // Set ptrs
    newSolid->sScene = scene;
    addVertexIntoSolid(newSolid, newVertex);
    addLoopIntoFace(newFace, newLoop);
    addFaceIntoSolid(newSolid, newFace);

    return newSolid;
}

// 生成一个新点，同时生成一条连接新点与给定点的边
Edge* EulerOperator::mev(Scene*& scene, Vertex* v1, Vertex* v2, Loop* lp) {

    Solid* s = lp->lFace->fSolid;

    // Create topological elements
    HalfEdge* he1 = new HalfEdge(), * he2 = new HalfEdge();
    Edge* eg = new Edge();

    // Set ptrs
    setPtrsOfNewEdge(eg, he1, he2);
    he1->startV = v1, he1->endV = v2;
    he2->startV = v2, he2->endV = v1;
    he1->nextHe = he2, he2->prevHe = he1;
    he1->heLoop = lp, he2->heLoop = lp;

    // Find the insertion position
    if (lp->lEdge == nullptr) {
        he2->nextHe = he1, he1->prevHe = he2;
        lp->lEdge = he1;
    } else {
        HalfEdge* tmpHe = lp->lEdge;
        while (tmpHe->nextHe->startV != v1) tmpHe = tmpHe->nextHe;
        he2->nextHe = tmpHe->nextHe, tmpHe->nextHe->prevHe = he2;
        he1->prevHe = tmpHe, tmpHe->nextHe = he1;
    }

    scene->pushVertex(v2);
    addVertexIntoSolid(s, v2);
    addEdgeIntoSolid(s, eg);

    return eg;
}

// 在两点之间生成一个新边，并生成一个新面
Face* EulerOperator::mef(Scene*& scene, Vertex* v1, Vertex* v2, Loop* lp) {

    Solid* s = lp->lFace->fSolid;

    // Create topological elements
    HalfEdge* he1 = new HalfEdge(), * he2 = new HalfEdge();
    Edge* eg = new Edge();
    Loop* newLoop = new Loop();

    // Set ptrs
    setPtrsOfNewEdge(eg, he1, he2);
    he1->startV = v1, he1->endV = v2;
    he2->startV = v2, he2->endV = v1;

    // Find the edges ending at v1 and v2 respectively
    HalfEdge* heTemp = lp->lEdge;
    HalfEdge* heTemp1, * heTemp2, * heTemp3;

    while (heTemp->endV != v1) heTemp = heTemp->nextHe;
    heTemp1 = heTemp;

    while (heTemp->endV != v2) heTemp = heTemp->nextHe;
    heTemp2 = heTemp;

    heTemp = heTemp->nextHe;
    while (heTemp->endV != v2) heTemp = heTemp->nextHe;
    heTemp3 = heTemp;
    if(heTemp3 == nullptr) heTemp3 = heTemp2;

    // Divide the big loop into two small loops
    he1->nextHe = heTemp3->nextHe, heTemp3->nextHe->prevHe = he1;
    heTemp3->nextHe = he2, he2->prevHe = heTemp3;

    he2->nextHe = heTemp1->nextHe, heTemp1->nextHe->prevHe = he2;
    heTemp1->nextHe = he1, he1->prevHe = heTemp1;

    he1->heLoop = newLoop, he2->heLoop = lp;
    UpdateLoopPtrs(he1, newLoop);
    newLoop->lEdge = he1, lp->lEdge = he2;

    // Construct a new surface with newLoop
    Face *newFace = new Face();
    newLoop->id = s->lnum++;
    scene->pushLoop(newLoop);

    addEdgeIntoSolid(s, eg);
    addLoopIntoFace(newFace, newLoop);
    addFaceIntoSolid(s, newFace);

    return newFace;
}

// 删除给定边，并生成一个内环
Loop* EulerOperator::kemr(Scene*& scene, Edge* killEdge, Face* outerFace) {
    // killEdge must belong to the outerFace
    Solid* s = outerFace->fSolid;
    HalfEdge* he1 = killEdge->firstHe, * he2 = killEdge->secondHe;

    // Construct a new loop from he1 to he2
    HalfEdge* firstHe = he1->nextHe;
    HalfEdge* lastHe = he2->prevHe;
    lastHe->nextHe = firstHe;
    firstHe->prevHe = lastHe;
    Loop* newLoop = new Loop();
    newLoop->lEdge = firstHe;
    newLoop->id = s->lnum++;
    firstHe->heLoop = newLoop;
    UpdateLoopPtrs(firstHe, newLoop);
    addLoopIntoFace(outerFace, newLoop);

    // Reconstruct the loop from he2 to he1
    firstHe = he2->nextHe;
    lastHe = he1->prevHe;
    lastHe->nextHe = firstHe;
    firstHe->prevHe = lastHe;
    firstHe->heLoop->lEdge = firstHe;

    // delete edge
    dropEdgeFromSolid(s, killEdge);

    return newLoop;
}

// 删除与面outerFace相接触的一个面innerFace，生成面outerFace上的一个内环，并形成体上的一个通孔
void EulerOperator::kfmrh(Scene*& scene, Face* innerFace, Face* outerFace) {

    Loop* lp = innerFace->outerLoop;
    addLoopIntoFace(outerFace, lp);
    dropFaceFromSolid(innerFace->fSolid, innerFace);
}

// 在给定边上定义一个新的点，同时以新点和给定边的终点为两端点定义一条新的边
Edge *EulerOperator::semv(Scene *&scene, Edge *eg, float interp) {

    HalfEdge *he1 = eg->firstHe, *he2 = eg->secondHe;
    Vertex *v1 = he1->startV, *v2 = he1->endV;
    Solid *s = he1->heLoop->lFace->fSolid;

    // Create topological elements
    Point *newPoint = new Point(v1->point->pos + interp * (v2->point->pos - v1->point->pos));
    Vertex *newVertex = new Vertex(newPoint);
    HalfEdge *he3 = new HalfEdge(), *he4 = new HalfEdge();
    Edge *newEdge = new Edge();

    he1->endV = newVertex, he2->startV = newVertex;
    setPtrsOfNewEdge(newEdge, he3, he4);
    he3->startV = newVertex, he3->endV = v2;
    he4->startV = v2, he4->endV = newVertex;
    he3->nextHe = he1->nextHe, he1->nextHe->prevHe = he3, he1->nextHe = he3, he3->prevHe = he1;
    he4->prevHe = he2->prevHe, he2->prevHe->nextHe = he4, he4->nextHe = he2, he2->prevHe = he4;
    he3->heLoop = he1->heLoop, he4->heLoop = he2->heLoop;

    scene->pushVertex(newVertex);
    addVertexIntoSolid(s, newVertex);
    addEdgeIntoSolid(s, newEdge);

    return newEdge;
}

// ******************** Inverse Euler operations ********************
// 消去体的最后一个点、一个外环、一个面及体本身
void EulerOperator::kvfs(Scene *&scene, Vertex *vertex, Face *f, Solid *solid) {

    Loop *lp = f->outerLoop;
    scene->dropVertex(vertex);
    scene->dropLoop(lp);
    scene->dropSolid(solid);

    dropVertexFromSolid(solid, vertex);
    dropLoopFromFace(f, lp);
    dropFaceFromSolid(solid, f);
}

// 消去一个点及与其相连的一条边，该点仅与一条边相连
void EulerOperator::kev(Scene *&scene, Vertex* killV, Edge* killEdge) {

    Solid *s = killEdge->firstHe->heLoop->lFace->fSolid;
    HalfEdge *he1 = killEdge->firstHe, *he2 = killEdge->secondHe;

    // Set ptrs
    if(he1->prevHe == he2 && he2->prevHe == he1) {
        he1->heLoop->lEdge = nullptr;
    } else if(he2->prevHe == he1) {
        he1->heLoop->lEdge = he1->prevHe;
        he1->prevHe->nextHe = he2->nextHe;
        he2->nextHe->prevHe = he1->prevHe;
    } else {
        he2->heLoop->lEdge = he2->prevHe;
        he2->prevHe->nextHe = he1->nextHe;
        he1->nextHe->prevHe = he2->prevHe;
    }

    scene->dropVertex(killV);
    dropVertexFromSolid(s, killV);
    dropEdgeFromSolid(s, killEdge);
}

// 消去一个面，一个环及面中的一条边，这里边与两个面相连
void EulerOperator::kef(Scene *&scene, Edge *killEdge) {

    Solid *s = killEdge->firstHe->heLoop->lFace->fSolid;
    HalfEdge *he1 = killEdge->firstHe, *he2 = killEdge->secondHe;

    // Set ptrs
    he1->prevHe->nextHe = he2->nextHe, he2->nextHe->prevHe = he1->prevHe;
    he2->prevHe->nextHe = he1->nextHe, he1->nextHe->prevHe = he2->prevHe;
    UpdateLoopPtrs(he2->prevHe, he2->heLoop);
    he2->heLoop->lEdge = he2->prevHe;

    s->lnum--;
    Face *faceList = s->sFaces;
    while(faceList) {
        Loop *loopList = faceList->outerLoop;
        while(loopList) {
            if(loopList->id > he1->heLoop->id) loopList->id--;
            loopList = loopList->nextL;
        }
        faceList = faceList->nextF;
    }

    Face *kiilFace = he1->heLoop->lFace;
    scene->dropLoop(he1->heLoop);
    dropLoopFromFace(he1->heLoop->lFace, he1->heLoop);
    dropFaceFromSolid(s, kiilFace);
    dropEdgeFromSolid(s, killEdge);
}

// 生成一条边，消去一个内环，这里边的两端点共面不共环
Edge *EulerOperator::mekr(Scene *&scene, Loop *outerLoop, Loop *innerLoop, Vertex *v1, Vertex *v2) {

    Face *f = outerLoop->lFace;
    Solid *s = f->fSolid;
    
    // Create topological elements
    HalfEdge *he1 = new HalfEdge(), *he2 = new HalfEdge();
    Edge *eg = new Edge();

    // Set ptrs
    setPtrsOfNewEdge(eg, he1, he2);
    he1->startV = v1, he1->endV = v2;
    he2->startV = v2, he2->endV = v1;
    he1->heLoop = he2->heLoop = outerLoop;

    HalfEdge *firstHe = innerLoop->lEdge;
    UpdateLoopPtrs(firstHe, outerLoop);
    while(firstHe->endV != v2) firstHe = firstHe->nextHe;
    firstHe->nextHe->prevHe = he1, he1->nextHe = firstHe->nextHe;
    firstHe->nextHe = he2, he2->prevHe = firstHe;

    firstHe = outerLoop->lEdge;
    while(firstHe->endV != v1) firstHe = firstHe->nextHe;
    firstHe->nextHe->prevHe = he2, he2->nextHe = firstHe->nextHe;
    firstHe->nextHe = he1, he1->prevHe = firstHe;

    s->lnum--;
    Face *faceList = s->sFaces;
    while(faceList) {
        Loop *loopList = faceList->outerLoop;
        while(loopList) {
            if(loopList->id > innerLoop->id) loopList->id--;
            loopList = loopList->nextL;
        }
        faceList = faceList->nextF;
    }

    dropLoopFromFace(f, innerLoop);
    addEdgeIntoSolid(s, eg);

    return eg;
}

// 消除一个内环，一个通孔，定义一个新面
Face *EulerOperator::mfkrh(Scene *&scene, Loop *lp) {

    Face *newFace = new Face(), *f = lp->lFace;

    // Set ptrs
    Loop *tmpLoop = f->innerLoops;
    if(tmpLoop == lp) f->innerLoops = tmpLoop->nextL;
    while(tmpLoop != lp) tmpLoop = tmpLoop->nextL;
    tmpLoop->prevL->nextL = tmpLoop->nextL;
    if(tmpLoop->nextL) tmpLoop->nextL->prevL = tmpLoop->prevL;
    --f->innum;

    addLoopIntoFace(newFace, lp);
    addFaceIntoSolid(f->fSolid, newFace);

    return newFace;
}
