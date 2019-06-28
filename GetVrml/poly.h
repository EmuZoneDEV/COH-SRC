#ifndef _POLY_H
#define _POLY_H

#include "mathutil.h"

typedef struct
{
	Vec2	st;
	Vec3	rgb;
	Vec3	pos;
	Vec3	norm;
//	signed char	plane_idx;
//	char	tag;
//	char	blended;
//	int		edge_index;
//	Polygon	*edge_poly;
//	Vec2	light_st;
} Vertex;

typedef unsigned char PixData[4];

typedef struct _Polygon
{
	int		nverts;
	Vertex	*verts;
	int		matidx;
	Vec3	norm;
	struct _Polygon *next,*prev;
	F32		min_y,max_y;
	int		flags;
	char	tag;
	PixData	*lightmap;
	int		width;
	int		height;
	int		idx;
} Polygon;

// Generated by mkproto
void polyResetStats();
void polyPrintStats();
void polyPrintVerts(Polygon *p);
Polygon *polyClone(Polygon *poly);
Polygon *polyFirstLink(Polygon *poly);
Polygon *polyLastLink(Polygon *poly);
int polyNumLinks(Polygon *poly);
int polyLegal(Polygon *p);
Polygon *polyCreateSimple(Vec3 v1,Vec3 v2,Vec3 v3,Vec3 v4);
Polygon *polyCreate(int numv,Vec3 *verts,Vec3 *norms,Vec2 *sts,Polygon *parent,int reuse);
void polyFree(Polygon *poly);
Polygon *polyLink(Polygon *list,Polygon *poly);
Polygon *polyUnlink(Polygon *poly);
Polygon *polyLinkList(Polygon *parent,Polygon *child);
void polyFreeList(Polygon *poly);
Polygon *polyTriangulateList(Polygon *list);
// End mkproto
#endif