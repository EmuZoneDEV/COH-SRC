/******************************************************************************
A .ANIMX is the text file format exported by the animation exporter in 3DS MAX.
The purpose of this code is to parse that source data into something we
can operate on. The name is derived from: ANIMation Export.
*****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stdtypes.h"
#include "utils.h"
#include "earray.h"
#include "gimmeDLLWrapper.h"
#include "mathutil.h"
#include "import_animx.h"

///////////////////////////////////////////////////////////////////////////////
// ANIMX animation export format and code for parsing
//
// ** IMPORTANT **: Keep in sync with structures in process_animx.c
// use by the GetAnimation2 tool.
///////////////////////////////////////////////////////////////////////////////

AUTO_STRUCT;
typedef struct TAnimX_Transform
{
  Vec3 vAxis;
  F32 fAngle;
  Vec3 vTranslation;
  Vec3 vScale;
} TAnimX_Transform;

AUTO_STRUCT;
typedef struct TAnimX_Bone
{
  const char* pcName; AST(STRUCTPARAM POOL_STRING)
  TAnimX_Transform** eaTransform;
} TAnimX_Bone;

AUTO_STRUCT;
typedef struct TAnimX
{
	int					iVersion;
	const char*	pcSourceName;		// MAX file animation exported from
	int					iTotalFrames;		// exported frame range, i.e. max key values a node can have, could have less
	int					iFirstFrame;

	TAnimX_Bone**	eaBone;				// Array of node tracks
}TAnimX;

// Code generated by structparser to parse text ANIMX files
#include "AutoGen/import_animx_c_ast.h"
#include "AutoGen/import_animx_c_ast.c"

///////////////////////////////////////////////////////////////////////////////
//                             DEBUG
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//                             Globals
///////////////////////////////////////////////////////////////////////////////

TAnimX g_imported_animx = {0};

///////////////////////////////////////////////////////////////////////////////
//                             PARSER
///////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 Read the input file. Returns the number of animation frames, or 0 for error
*****************************************************************************/
unsigned int LoadAnimation( const char* sourcepath ) 
{
	U32 numFrames;
	U32 numNodes;

	// we use the utilities library for the struct parser
	// but we don't want any gimme operations/error messages from our MAX plugin.
	gimmeDLLDisable(true);

	// Read ANIMX file
	ParserLoadFiles(NULL, (char*)sourcepath, 0, 0, parse_TAnimX, &g_imported_animx, 0, NULL, NULL);

	// How many nodes are in this animation?
	numNodes = eaSize(&(g_imported_animx.eaBone));
	if (numNodes < 1)
	{
		printf("ERROR: No nodes in animation data: '%s'\n", sourcepath );
		return 0;
	}
	
	// How many frames are in this animation?
	// Note that an individual node may have less than this number of frames
	// if the node stops animating beyond some key in the move.
//	numFrames = eaSize(&(g_imported_animx.eaBone[0]->eaTransform));
	numFrames = g_imported_animx.iTotalFrames;
	if (numFrames < 2)
	{
		printf("ERROR: Not enough frames in animation data: '%s'\n", sourcepath );
		return 0;
	}

	return numFrames;
}

/******************************************************************************
	Free data from last read animation
*****************************************************************************/
void FreeAnimation( void ) 
{
	if (g_imported_animx.eaBone)
	{
		eaDestroy(&g_imported_animx.eaBone);
	}
	memset( &g_imported_animx, 0, sizeof(g_imported_animx));
}

/******************************************************************************
 Given a node name return the handle (pointer) to the information for that
 in the import data if any.
*****************************************************************************/
NodeAnimHandle GetNodeAnimHandle( const char* node_name )
{
	U32 numNodes;
	U32 i;

	numNodes = eaSize(&(g_imported_animx.eaBone));

	for (i=0; i<numNodes; i++)
	{
		if (stricmp(g_imported_animx.eaBone[i]->pcName, node_name)==0)
			return g_imported_animx.eaBone[i];
	}
	return NULL;	// not present in data
}

/******************************************************************************
 Given a node name and a frame number return the node transform information:
	t = 3 floats for translation
	rot = 4 floats in axis angle
	scale = 3 floats

	Note that a particular node may only have real animation data out to a
	frame n < numFrames in the animation. (e.g., if the joint stopped moving
	and held its location before the move ended)

*****************************************************************************/
static float s_fail_floats[8];

float*	GetNodeFrameTranslation( NodeAnimHandle hNode, int iFrame )
{
	TAnimX_Bone* pBone = (TAnimX_Bone*)hNode;
	int iFrameMax = eaSize(&pBone->eaTransform) - 1;
	int iFrameClamped = MIN( iFrame, iFrameMax );

	if (iFrameClamped >= 0)
	{
		float* pFloats = pBone->eaTransform[iFrameClamped]->vTranslation;
		return pFloats;
	}
	else
	{
		return s_fail_floats;
	}
}

float*	GetNodeFrameAxisAngle( NodeAnimHandle hNode, int iFrame )
{
	TAnimX_Bone* pBone = (TAnimX_Bone*)hNode;
	int iFrameMax = eaSize(&pBone->eaTransform) - 1;
	int iFrameClamped = MIN( iFrame, iFrameMax );

	if (iFrameClamped >= 0)
	{
		float* pFloats = pBone->eaTransform[iFrameClamped]->vAxis;
		return pFloats;
	}
	else
	{
		return s_fail_floats;
	}
}

float*	GetNodeFrameScale( NodeAnimHandle hNode, int iFrame )
{
	TAnimX_Bone* pBone = (TAnimX_Bone*)hNode;
	int iFrameMax = eaSize(&pBone->eaTransform) - 1;
	int iFrameClamped = MIN( iFrame, iFrameMax );

	if (iFrameClamped >= 0)
	{
		float* pFloats = pBone->eaTransform[iFrameClamped]->vScale;
		return pFloats;
	}
	else
	{
		return s_fail_floats;
	}
}