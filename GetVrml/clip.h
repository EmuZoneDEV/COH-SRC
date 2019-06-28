#ifndef _CLIP_H
#define _CLIP_H

#include "poly.h"

typedef struct
{
	Vec3	pos;
	Vec3	norm;
} Plane;

#define POLY_CUT 1
#define POLY_FRONT 2
#define POLY_BACK 3
#define POLY_ON 4

// Generated by mkproto
F32 pointPlaneDist(Vec3 VV,Vec3 NN,Vec3 pt);
int VWhichSide(Vec3 VV,Vec3 NN,Vec3 pt);
int PWhichSide(Vec3 pt,Vec3 norm,Polygon *poly);
int SplitPoly(Vec3 pos,Vec3 norm,Polygon *poly,Polygon **front,Polygon **back);
void SplitPolyList(Vec3 pos,Vec3 norm,Polygon *list,Polygon **front_p,Polygon **back_p);
Polygon *ClipPoly(Vec3 pos,Vec3 norm,Polygon *poly,int reuse);
Polygon *ClipPolyList(Vec3 pos,Vec3 norm,Polygon *list);
Polygon *clipPolyToPlanes(Plane *planes,int count,Polygon *poly,int alloc,int except);
void boxToPlanes(Vec3 min,Vec3 max,Plane *planes);
int triInBox(Vec3 min,F32 cube_size,Vec3 v1,Vec3 v2,Vec3 v3);
// End mkproto
#endif
