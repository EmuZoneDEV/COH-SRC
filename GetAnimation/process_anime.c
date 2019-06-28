/******************************************************************************
A .ANIME is the text file format exporter by the animation exporter in 3DS MAX
The purpose of this code is to convert that source data into the game runtime
animation format. The name is derived from: ANIMation Export.
*****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stdtypes.h"
#include "mathutil.h"
#include "Quat.h"
#include "process_anime.h"
#include "vrml.h"
#include "outputanim.h"
#include "assert.h"
#include "file.h"
#include "error.h"
#include "utils.h"
#include "earray.h"

///////////////////////////////////////////////////////////////////////////////
// ANIME animation export format and code for parsing
///////////////////////////////////////////////////////////////////////////////

AUTO_STRUCT;
typedef struct TAnimE_Transform
{
  Vec3 vAxis;
  F32 fAngle;
  Vec3 vTranslation;
  Vec3 vScale;
} TAnimE_Transform;

AUTO_STRUCT;
typedef struct TAnimE_Bone
{
  const char* pcName; AST(STRUCTPARAM POOL_STRING)
  TAnimE_Transform** eaTransform;
} TAnimE_Bone;

AUTO_STRUCT;
typedef struct TAnimE
{
	int					iVersion;
	const char*	pcSourceName;	// MAX file animation exported from
	int					iTotalFrames;	// exported frame range, i.e. max key values a node can have, could have less
	int					iFirstFrame;

  TAnimE_Bone**	eaBone;				// Array of node tracks
}TAnimE;

// Code generated by structparser to parse text ANIME files
#include "AutoGen/process_anime_c_ast.h"
#include "AutoGen/process_anime_c_ast.c"

///////////////////////////////////////////////////////////////////////////////
//                             DEBUG
///////////////////////////////////////////////////////////////////////////////

/******************************************************************************
  dump modified animation
*****************************************************************************/
#define OUTPUT_LINE_ANIM(a) fwrite(a, 1, strlen(a), f); 

static void debug_write_ANIME( char const* fname, TAnimE_Bone** eaUsedBoneList )
{
  char textbuf[1024];
  U32 uiBoneIndex;
  U32 numUsedBones = eaSize(&eaUsedBoneList);
  FILE* f;

  sprintf( textbuf, "%s.dbg.local", fname );
  f = fopen(textbuf, "w");

  for (uiBoneIndex = 0; uiBoneIndex < numUsedBones; ++uiBoneIndex)
  {
    TAnimE_Bone* pAnimBone = eaUsedBoneList[uiBoneIndex];
    U32 numFrames = eaSize(&(pAnimBone->eaTransform));
    U32 iFrame;

    fprintf( f, "Bone \"%s\"\n{\n\n", pAnimBone->pcName );

    for (iFrame=0; iFrame < numFrames; ++iFrame)
    {
      TAnimE_Transform* pT = pAnimBone->eaTransform[iFrame];
			fprintf( f, "\tTransform\n\t{\n" );
			fprintf( f, "\t\tAxis %.7g %.7g %.7g \n", pT->vAxis[0], pT->vAxis[1], pT->vAxis[2] );
			fprintf( f, "\t\tAngle %.7g\n", pT->fAngle );
			fprintf( f, "\t\tTranslation %.7g %.7g %.7g \n", pT->vTranslation[0], pT->vTranslation[1], pT->vTranslation[2] );
			fprintf( f, "\t\tScale %.7g %.7g %.7g \n",  pT->vScale[0], pT->vScale[1], pT->vScale[2] );
			fprintf( f, "\t}\n\n" );
    }
		fprintf( f, "}\n\n" );
  }
  fclose(f);
}

///////////////////////////////////////////////////////////////////////////////
//                             
///////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 Linear walk to find the named bone in the ANIME export data.
 Return NULL if not found.
*****************************************************************************/
static TAnimE_Bone* animeBoneFindFromName(TAnimE* pAnimE, const char* pcName)
{
  const U32 uiNumBones = eaSize(&pAnimE->eaBone);
  U32 uiBone;
  for (uiBone=0; uiBone<uiNumBones; ++uiBone)
  {
    TAnimE_Bone* pBone = pAnimE->eaBone[uiBone];
    if (stricmp(pBone->pcName, pcName)==0)
      return pBone;
  }
  return NULL;
}

/******************************************************************************
  Build a table we can use to lookup a pointer to the anim bone source data
  corresponding to a given bone ID.

  Note that nodes in the exporter animation that don't match a bone in the
  supplied skeleton get tacitly filtered out.

  Supply the root node of the skeleton obtained from the 'skel_*.wrl'
  processing.
*****************************************************************************/
static void animeBuildBoneMapTable( Node* pSkeletonRootNode, TAnimE* pAnimE, TAnimE_Bone** mapBoneID ) 
{
  Node* pSkeletonNode = pSkeletonRootNode;

  while (pSkeletonNode)
  {
    TAnimE_Bone* pAnimBone = animeBoneFindFromName(pAnimE, pSkeletonNode->name);
    if (pAnimBone)
    {
      U32 boneID = pSkeletonNode->anim_id;
      assert( bone_IdIsValid(boneID) );
      mapBoneID[boneID] = pAnimBone;
    }
    if (pSkeletonNode->child)
      animeBuildBoneMapTable(pSkeletonNode->child, pAnimE, mapBoneID);
    pSkeletonNode = pSkeletonNode->next;
  }
}

/******************************************************************************
  Build a list of the bones in the animation export that correspond to bones
  in the skeleton the animation is being targeted at. Nodes in the export
  that do not correspond to skeleton bones (e.g., "GEO_*" mesh nodes) are dropped.

  This also keeps the order we process the bone tracks the same as the skeleton
  hierarchy.

  Supply the root node of the skeleton obtained from the 'skel_*.wrl'
  processing.

  Returns the number of bones in the used bone list
*****************************************************************************/
static U32 animeCreateUsedBoneList( Node* pSkeletonRootNode, TAnimE_Bone** mapBoneID, TAnimE_Bone*** peaUsedBoneList ) 
{
  Node* pSkeletonNode = pSkeletonRootNode;
  U32   numUsedBones = 0;

  while (pSkeletonNode)
  {
    TAnimE_Bone* pAnimBone = mapBoneID[pSkeletonNode->anim_id];
    if (pAnimBone)
    {
      eaPush(peaUsedBoneList, pAnimBone);
      numUsedBones++;
    }
    if (pSkeletonNode->child)
      numUsedBones += animeCreateUsedBoneList(pSkeletonNode->child, mapBoneID, peaUsedBoneList);
    pSkeletonNode = pSkeletonNode->next;
  }
  return numUsedBones;
}

/******************************************************************************
 PrecisionAngleAxisFromQuat

 More precise Quaternion -> Angle/Axis for tool time than the current CoH
 function library provides so we can have more accurate and comparable data
 to the animation export from MAX. (based on one in VRML exporter)
 
 We also return angle in the range [-pi, pi] instead of [0, 2pi]
*****************************************************************************/
static void PrecisionAngleAxisFromQuat(const Quat q, Vec3 axis, F32* angle)
{
  double omega, s, x, y, z, w, l, c;
  x = (double)q[0];	
  y = (double)q[1];	
  z = (double)q[2];	
  w = (double)q[3];	
  l =  sqrt(x*x + y*y + z*z + w*w);
  if (l == 0.0)
  {
    w = 1.0;	
    y = z = x = 0.0;
  }
  else {	
    c = 1.0/l;
    x *= c;
    y *= c;
    z *= c;
    w *= c;
  }
  omega = acos(w);
  *angle = (float)(2.0*omega);
  if (*angle > PI)
    *angle = *angle - 2.0*PI;
  s = sin(omega);	
  if (fabs(s) > 0.000001f)
  {
    axis[0] = (float)(x / s);
    axis[1] = (float)(y / s);
    axis[2] = (float)(z / s);
  }
  else
  {
    axis[0] = axis[2] = 0.0f;
    axis[1] = 1.0f;
    *angle = 0.0f;
  }
}

/******************************************************************************
  Convert source data using the 3DS MAX coordinate frame (right handed Z up)
  into the in-game coordinate frame. This can be viewed as two xforms:
    - into the standard VRML Y-up right handed conversion: Z->Y and -Y->Z
    - into the in game left handed coord system with Y-up: X->-X
  Source and destination can be the same
*****************************************************************************/
static void ConvertCoordsFrom3DSMAX( Vec3 vOut, Vec3 vIn )
{
  F32 tmpV1 = vIn[1]; // use a temp so src and dst can be the same vector
  vOut[0] = -vIn[0];
  vOut[1] =  vIn[2];
  vOut[2] = -tmpV1;
}

/******************************************************************************
 Transform the src animation data from the MAX World Coordinates system into
 joint relative transforms and into the games in-game coordinate system. 
 (left handed with Y axis up, i.e. the VRML right handed y-up frame with X reversed)

 The src animation data exported from MAX has the joint data at each frame in
 absolute model (world) coordinates. Thus, we need to make this data joint relative.
 Since the animation data is all in world coordinates we can calculate the joint
 local coordinate frame as follows:

    T_child_local = Inverse( T_parent_world ) * T_child_world

*****************************************************************************/
static void animeTransformJointKeysRelative( TAnimE_Bone* pAnimBone,  TAnimE_Bone* pAnimBoneParent ) 
{
  U32 i, numFrames;

  assert( pAnimBoneParent && pAnimBone );

  // we should not have processed this bone or its parent yet, if we have
  // results will be wrong
  assert( eaSize(&pAnimBone->eaTransform) == eaSize(&pAnimBoneParent->eaTransform) );
  numFrames = eaSize(&pAnimBone->eaTransform);

  for ( i = 0; i < numFrames; ++i )
  {
    TAnimE_Transform* pTransformJoint   = pAnimBone->eaTransform[i];
    TAnimE_Transform* pTransformParent  = pAnimBoneParent->eaTransform[i];

    Quat qInvParentWorld;
    Vec3 vAxis;

    // Calc local rotation
    {
      Quat qJointLocal, qJointWorld, qParentWorld;

      // convert from 3DS MAX coordinate frame into in-game
      ConvertCoordsFrom3DSMAX( vAxis, pTransformJoint->vAxis );
      // sense of angle from MAX is opposite in export than used in VRML export and game
      axisAngleToQuat(vAxis, -pTransformJoint->fAngle, qJointWorld);
      quatNormalize(qJointWorld);

      ConvertCoordsFrom3DSMAX( vAxis, pTransformParent->vAxis );
      axisAngleToQuat(vAxis, -pTransformParent->fAngle, qParentWorld);
      quatNormalize(qParentWorld);

      quatInverse(qParentWorld, qInvParentWorld);
      quatMultiply(qJointWorld, qInvParentWorld, qJointLocal);

      // store the result back over original world xform
      PrecisionAngleAxisFromQuat( qJointLocal, pTransformJoint->vAxis, &pTransformJoint->fAngle );
    }

    // local translation
    {
      Vec3 vAbsPosDiff;
      Vec3 vAbsPosDiffInGame;
      Vec3 vParentSpacePosDiff;

      // Calculate how far, in abs. space, the bone pos is from the parent bone pos
      subVec3(pTransformJoint->vTranslation, pTransformParent->vTranslation, vAbsPosDiff);

      ConvertCoordsFrom3DSMAX( vAbsPosDiffInGame, vAbsPosDiff );

      // Rotate this abs diff into the space of the parent bone
      quatRotateVec3(qInvParentWorld, vAbsPosDiffInGame, vParentSpacePosDiff);

      // store the result back over original world xform
      copyVec3( vParentSpacePosDiff, pTransformJoint->vTranslation );
    }
  }
}

/******************************************************************************
For root nodes...
Transform the src animation data from the MAX World Coordinates system into
joint relative transforms and into the games in-game coordinate system. 
(left handed with Y axis up, i.e. the VRML right handed y-up frame with X reversed)

Root nodes have no parent so the transforms should already be 'local'.
We just need to update the coordinate frame 
*****************************************************************************/
static void animeTransformJointKeysRoot( TAnimE_Bone* pAnimBone ) 
{
  U32 i, numFrames;

  assert( pAnimBone );
  numFrames = eaSize(&pAnimBone->eaTransform);

  for ( i = 0; i < numFrames; ++i )
  {
    TAnimE_Transform* pTransformJoint   = pAnimBone->eaTransform[i];
    // Calc local rotation
    {
      // convert from 3DS MAX coordinate frame into in-game
      ConvertCoordsFrom3DSMAX( pTransformJoint->vAxis, pTransformJoint->vAxis );
      pTransformJoint->fAngle = -pTransformJoint->fAngle;
    }

    // local translation
    {
      ConvertCoordsFrom3DSMAX( pTransformJoint->vTranslation, pTransformJoint->vTranslation );
    }
  }
}

/******************************************************************************
 Transform the src animation data from the MAX World Coordinates system into
 joint relative transforms and into the games in-game coordinate system. 
 (left handed with Y axis up, i.e. the VRML right handed y-up frame with X reversed)

 The src animation data exported from MAX has the joint data at each frame in
 absolute model (world) coordinates. Thus, we need to make this data joint relative.

 Note that we could do these calculations progressing from the bottom of the hierarchy
 up to the top for the simple fact that if we went top down we would overwrite
 the parent xform information before we needed it to process the children.
*****************************************************************************/
static void animeTransformJointRelative( Node* pSkeletonNode,  Node* pSkeletonNodeParent, TAnimE_Bone** mapBoneID ) 
{
  // Process all children first. We want a bottom-up calculation so we don't
  // destroy the parent's world xform information information before we need it.
  Node* pChildsParentNode   = pSkeletonNode;
  Node* pChild              = pSkeletonNode->child;

  while (pChild)
  {
    animeTransformJointRelative(pChild, pChildsParentNode, mapBoneID);
    pChild = pChild->next;
  }

  // When we reach here we are at a leaf node or a parent of
  // an already processed sub hierarchy so we can update
  // the transforms on the animation channel for this node
  if (pSkeletonNode )
  {
    TAnimE_Bone* pAnimBone = mapBoneID[pSkeletonNode->anim_id];
    if ( pAnimBone )
    {
      if (pSkeletonNodeParent)
      {
        // we have a parent so we need to convert to local coords
        TAnimE_Bone* pAnimBoneParent = mapBoneID[pSkeletonNodeParent->anim_id];
        animeTransformJointKeysRelative( pAnimBone, pAnimBoneParent );
      }
      else
      {
        //...no parent, must be a root node
        // just update the coordnate frames
        animeTransformJointKeysRoot(pAnimBone);
      }
    }
  }
}


/******************************************************************************
  The runtime expects that the animation tracks will have frame 0 in the bind pose
  so we need to insert that information from the skeleton processing
  TODO: change the runtime so this is not necessary?
  TODO: probably clearer and more efficient to loop over used bones and lookup in skeleton tree
*****************************************************************************/
static void animePrependBasePoseFrame( Node* pSkeletonNode, TAnimE_Bone** mapBoneID ) 
{
  while (pSkeletonNode)
  {
    TAnimE_Bone* pAnimBone = mapBoneID[pSkeletonNode->anim_id];
    if (pAnimBone)
    {
      // get the base pose transform from the skeleton node
      TAnimE_Transform* pFrame0 = (TAnimE_Transform*)calloc(1, sizeof(TAnimE_Transform));

      // Note there is not currently supposed to be any rotation on the
      // the base pose so we should just be able to copy the translation
      copyVec3( &pSkeletonNode->rotate[0], pFrame0->vAxis );
      pFrame0->fAngle = pSkeletonNode->rotate[3];
      copyVec3( pSkeletonNode->translate, pFrame0->vTranslation );
      setVec3( pFrame0->vScale, 1.0f, 1.0f, 1.0f );

      // and insert it as frame 0
      // TODO: I'm not sure who is responsible for cleaning these up?
      eaInsert( &pAnimBone->eaTransform, pFrame0, 0 );
    }
    if (pSkeletonNode->child)
      animePrependBasePoseFrame(pSkeletonNode->child, mapBoneID);
    pSkeletonNode = pSkeletonNode->next;
  }
}

/******************************************************************************
 key value comparisons for reductions
*****************************************************************************/

#define EPSILON 0.00001f

bool posKeyIsSame(const Vec3 vKey1, const Vec3 vKey2)
{
  Vec3 vDiff;
  subVec3(vKey1, vKey2, vDiff);
  return (lengthVec3Squared(vDiff) < EPSILON*EPSILON);
}

bool scaleKeyIsSame(const Vec3 vKey1, const Vec3 vKey2)
{
  Vec3 vDiff;
  subVec3(vKey1, vKey2, vDiff);
  return (lengthVec3Squared(vDiff) < EPSILON*EPSILON);
}

// This function works by forcing the quaternions to the same side of a 4-sphere and taking their linear distance
// which is an approximate measure of distance for small angles of difference (which is what we are concerned with)

// TODO: add this to the math headers
#define lengthVec4Squared(v1)		(SQR((v1)[0]) + SQR((v1)[1]) + SQR((v1)[2]) + SQR((v1)[3]))

bool rotKeyIsSame(Quat qKey1, Quat qKey2)
{
  Vec4 vDiff;
  quatForceWPositive(qKey1);
  quatForceWPositive(qKey2);
  subVec4(qKey1, qKey2, vDiff);
  return (lengthVec4Squared(vDiff) < EPSILON*EPSILON);
}

/******************************************************************************
  Populate a runtime bone animation track from source asset animation data
*****************************************************************************/
static void animeBuildBoneTrackPos( BoneAnimTrack* pDstBoneTrack, TAnimE_Bone* pSrcAnimBone ) 
{
  U32 numSrcFrames, numDstFrames;
  U32 i;
  bool bKeysHaveValuesOverOne = false;
  bool bCompressValues = true;
  U32  iFrameNumberKeysBecomeStatic = 0;

  // scan the source data: 
  numSrcFrames = eaSize(&(pSrcAnimBone->eaTransform));

  //  - if no magnitude is > 1.0f then simple fixed point 16 bit quantization can be applied
  //    (runtime doesn't support moving the binary point)
  for ( i = 0; i < numSrcFrames; ++i )
  {
    Vec3* pKeyTranslate = &pSrcAnimBone->eaTransform[i]->vTranslation;

    if( fabs((*pKeyTranslate)[0]) >= 1.0f || fabs((*pKeyTranslate)[1]) >= 1.0f || fabs((*pKeyTranslate)[2]) >= 1.0f)
    {
      bKeysHaveValuesOverOne = true;
      break;
    }
  }

  //  - if at some point the channel becomes static up until the end of the animation
  //    then we can truncate the key stream at that point and save memory.
  //    In the case where the channel is completely constant we still add a single key entry
  //    as expected by the runtime.

  // walk the keys backwards looking for the first change
  for ( i = numSrcFrames - 1; i > 0; --i )
  {
    if (!posKeyIsSame(pSrcAnimBone->eaTransform[i-1]->vTranslation, pSrcAnimBone->eaTransform[i]->vTranslation))
    {
      iFrameNumberKeysBecomeStatic = i;
      break;
    }
    // if we make it all the way through then all keys are identical and we are static at frame 0
    // (which is what we initialized iFrameNumberKeysBecomeStatic to)
  } 

  // how many key values are we putting in the destination track?
  numDstFrames = iFrameNumberKeysBecomeStatic + 1;

  // allocate the memory for the track values
  bCompressValues = !bKeysHaveValuesOverOne;
  {
    U32 valueSize = bCompressValues ? SIZE_OF_POSKEY_COMPRESSED_TO_6_BYTES : SIZE_OF_UNCOMPRESSED_POSKEY;
    // use allocBlockMem() to allocate for the bone track data as outputanim.c relies on that.
    pDstBoneTrack->pos_idx = allocBlockMem( MEM_ANIMDATA, valueSize*numDstFrames );
  }

  // now populate the track data, compressing if necessary
  for (i=0; i < numDstFrames; ++i)
  {
    TAnimE_Transform* pT = pSrcAnimBone->eaTransform[i];
    if (bCompressValues)
    {
      // convert to fixed point
      S16* pValueS16 = &((S16*)pDstBoneTrack->pos_idx)[i*3];
      *pValueS16++ = quantizePositionValue( pT->vTranslation[0] );
      *pValueS16++ = quantizePositionValue( pT->vTranslation[1] );
      *pValueS16++ = quantizePositionValue( pT->vTranslation[2] );
    }
    else
    {
      // just copy full floats over
      F32* pValueF32 = &((F32*)pDstBoneTrack->pos_idx)[i*3];
      *pValueF32++ = pT->vTranslation[0];
      *pValueF32++ = pT->vTranslation[1];
      *pValueF32++ = pT->vTranslation[2];
    }
  }

  // set remainder of bone track header
  pDstBoneTrack->flags |= bCompressValues ? POSITION_COMPRESSED_TO_6_BYTES : POSITION_UNCOMPRESSED;
  pDstBoneTrack->pos_count = pDstBoneTrack->pos_fullkeycount = numDstFrames;

}

/******************************************************************************
Populate a runtime bone animation track from source asset animation data
*****************************************************************************/
static void animeBuildBoneTrackRotation( BoneAnimTrack* pDstBoneTrack, TAnimE_Bone* pSrcAnimBone ) 
{
  U32 numSrcFrames, numDstFrames;
  U32 i;
  bool bCompressValues = true;
  bool bKeysAreIdentical = true;
  U32  iFrameNumberKeysBecomeStatic = 0;

  // scan the source data: 
  numSrcFrames = eaSize(&(pSrcAnimBone->eaTransform));

  //  - if at some point the channel becomes static up until the end of the animation
  //    then we can truncate the key stream at that point and save memory.
  //    In the case where the channel is completely constant we still add a single key entry
  //    as expected by the runtime.

  {
    Quat q0;
    axisAngleToQuat(pSrcAnimBone->eaTransform[numSrcFrames - 1]->vAxis, pSrcAnimBone->eaTransform[numSrcFrames - 1]->fAngle, q0);

    // walk the keys backwards looking for the first change
    for ( i = numSrcFrames - 1; i > 0; --i )
    {
      Quat q1;
      axisAngleToQuat(pSrcAnimBone->eaTransform[i-1]->vAxis, pSrcAnimBone->eaTransform[i-1]->fAngle, q1);

      if (!rotKeyIsSame(q0, q1))
      {
        iFrameNumberKeysBecomeStatic = i;
        break;
      }
      // if we make it all the way through then all keys are identical and we are static at frame 0
      // (which is what we initialized iFrameNumberKeysBecomeStatic to)
    } 
  }

  // how many key values are we putting in the destination track?
  numDstFrames = iFrameNumberKeysBecomeStatic + 1;

  // allocate the memory for the track values
  {
    U32 valueSize = bCompressValues ? SIZE_OF_ROTKEY_COMPRESSED_TO_5_BYTES : SIZE_OF_ROTKEY_UNCOMPRESSED;
    // use allocBlockMem() to allocate for the bone track data as outputanim.c relies on that.
    pDstBoneTrack->rot_idx = allocBlockMem( MEM_ANIMDATA, valueSize*numDstFrames );
  }

  // now populate the track data, compressing if necessary
  for (i=0; i < numDstFrames; ++i)
  {
    TAnimE_Transform* pT = pSrcAnimBone->eaTransform[i];

    // convert key rotation value to a quaternion for storage in game runtime format
    Quat q1;
    axisAngleToQuat(pSrcAnimBone->eaTransform[i]->vAxis, pSrcAnimBone->eaTransform[i]->fAngle, q1);
    quatNormalize(q1);
    makeBiggestValuePositive(q1);

    if (bCompressValues)
    {
      U8* pValue = (U8*)pDstBoneTrack->rot_idx + i*SIZE_OF_ROTKEY_COMPRESSED_TO_5_BYTES;
      assert( finiteVec4( q1 ) );
      compressQuatToFiveBytes( q1, pValue );
    }
    else
    {
      // just copy full floats over
      F32* pValueF32 = &((F32*)pDstBoneTrack->rot_idx)[i*4];
      *pValueF32++ = q1[0];
      *pValueF32++ = q1[1];
      *pValueF32++ = q1[2];
      *pValueF32++ = q1[3];
    }
  }

  // set remainder of bone track header
  pDstBoneTrack->flags |= bCompressValues ? ROTATION_COMPRESSED_TO_5_BYTES : ROTATION_UNCOMPRESSED;
  pDstBoneTrack->rot_count = pDstBoneTrack->rot_fullkeycount = numDstFrames;

}

/******************************************************************************
 Populate a runtime bone animation track from source asset animation data
*****************************************************************************/
static void animeBuildBoneTrack( BoneAnimTrack* pDstBoneTrack, TAnimE_Bone* pSrcAnimBone ) 
{
  pDstBoneTrack->id = bone_IdFromText(pSrcAnimBone->pcName);

  // Note we do rotation first if we want the binary file to have
  // the same layout and offsets as the older WRL processing

  // populate the rotation key data
  animeBuildBoneTrackRotation( pDstBoneTrack, pSrcAnimBone );

  // populate the position key data
  animeBuildBoneTrackPos( pDstBoneTrack, pSrcAnimBone );
}

/******************************************************************************
  Read one ANIME file animation and convert to in-game runtime format
  We supply the skeleton root so that we can match and filter the animation
  data we are processing.

  Returns true for a good conversion, false if track should be discarded
*****************************************************************************/
bool animConvert_ANIME_To_AnimTrack( SkeletonAnimTrack * pAnimTrack, char * sourcepath, Node * skeletonRoot ) 
{
  TAnimE anime = {0};
  TAnimE_Bone** eaUsedBoneList = NULL;
  U32 numUsedBones;
  U32 numFrames;
  U32 uiBoneIndex;
  TAnimE_Bone* mapBoneID[BONES_ON_DISK];  // used to map a bone id to it's src animation data

  // Read ANIME file
  ParserLoadFiles(NULL, sourcepath, 0, 0, parse_TAnimE, &anime, 0, NULL, NULL);

  // check version of input file?

  // We will need to know what animation channels in the source data correspond
  // to given bones in the skeleton. So a build a table with that information
  // which will be useful when we need to know what src data to process and how
  // it fits into the skeleton hierarchy we are processing it against.
  memset(mapBoneID, 0, sizeof(mapBoneID));
  animeBuildBoneMapTable( skeletonRoot, &anime, mapBoneID );

  // Create validated and ordered list of ANIME bones used by skeleton
  // i.e. the subset of the input animation data we are going to process
  numUsedBones = animeCreateUsedBoneList(skeletonRoot, mapBoneID, &eaUsedBoneList);
  if (numUsedBones == 0)    // If nothing matches up then we likely have an asset problem
  {
    Errorf("ERROR: Failed to find any matching bone animation data in ANIME '%s' with skeleton '%s'", sourcepath, pAnimTrack->baseAnimName);
    return false;
  }
  pAnimTrack->bone_track_count = numUsedBones;

  // How many frames are in this animation?
  numFrames = anime.iTotalFrames;
  if (numFrames < 2)
  {
    Errorf("ERROR: Not enough frames in animation data: '%s'\n", sourcepath );
    return false;
  }
	// note that we implicitly use the fact that the exporter writes a frame
	// for every node even if the node doesn't change throughout the entire animation
	// (which is something that may change if we want to make the exporter files a little
	// smaller and more 'human-readable')
	if ( numFrames != eaSize(&(eaUsedBoneList[0]->eaTransform)) )
	{
		Errorf("ERROR: Number of frames on nodes does not match frame total in header: '%s'\n", sourcepath );
		return false;
	}

  // Convert the source animation data to be joint relative (from world space) and to
  // the in-game coordinate frame
  animeTransformJointRelative( skeletonRoot, NULL, mapBoneID );
  debug_write_ANIME( sourcepath, eaUsedBoneList );

  // runtime expects that the animation tracks will have frame 0 in the bind pose
  // so we need to insert that information from the skeleton processing
  // the values should already be parent relative
  // TODO: change the runtime so this is not necessary?
  animePrependBasePoseFrame( skeletonRoot, mapBoneID );
//  debug_write_ANIME( sourcepath, eaUsedBoneList );

  // Allocate the bone tracks
  // use allocBlockMem() to allocate for the bone track data as outputanim.c relies on that.
  pAnimTrack->bone_tracks = allocBlockMem( MEM_BONEANIMTRACKS, sizeof(BoneAnimTrack) * numUsedBones);
  //  pAnimTrack->bone_tracks = calloc(sizeof(BoneAnimTrack), numUsedBones);
  if (!pAnimTrack->bone_tracks)
  {
    Errorf("ERROR: Could not allocate bone tracks for animation: '%s'\n", sourcepath );
    return false;
  }

  // Populate the bone tracks from the source data
  for (uiBoneIndex = 0; uiBoneIndex < numUsedBones; ++uiBoneIndex)
  {
    BoneAnimTrack*  pBoneTrack = &pAnimTrack->bone_tracks[uiBoneIndex];
    TAnimE_Bone*      pAnimBone = eaUsedBoneList[uiBoneIndex];

    animeBuildBoneTrack( pBoneTrack, pAnimBone );
  }

  // Note the 'length' field was being populated with the largest number of keys
  // but minus one for some reason. Maybe because the track has frame 0 as base pose?
  // So we continue that.
	// TODO: This may need to be updated to reflect the longest length of keys that remain
	// in a bone track, e.g. all the bones stopped moving after frame n
  numFrames = eaSize(&(eaUsedBoneList[0]->eaTransform));
  pAnimTrack->length = (F32)(numFrames - 1);

  // Add this in as a proto version so we can ID files that the new pipeline is creating
  pAnimTrack->spare_room[0] = 200;
	pAnimTrack->spare_room[1] = 1;

  return true;
}
