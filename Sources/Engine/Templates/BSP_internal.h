/* Copyright (c) 2002-2012 Croteam Ltd. 
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#ifndef SE_INCL_BSP_INTERNAL_H
#define SE_INCL_BSP_INTERNAL_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

extern FLOAT mth_fCSGEpsilon;

/*
 * Type used to identify BSP-node locations
 */
enum BSPNodeLocation {
  BNL_ILLEGAL=0,    // for illegal value
  BNL_INSIDE,       // inside leaf node
  BNL_OUTSIDE,      // outside leaf node
  BNL_BRANCH,       // branch node, unspecified location
};

/*
 * Template class for BSP vertex
 */
class BSPVertex : public DOUBLE3D {
public:
  /* Default constructor. */
  inline BSPVertex(void) {};

  /* Assignment operator with coordinates only. */
  inline BSPVertex &operator=(const DOUBLE3D &vCoordinates);
};

/*
 * Template class for BSP vertex container
 */
class BSPVertexContainer {
public:
  INDEX bvc_iMaxAxis;                     // index of largest axis of direction
  DOUBLE  bvc_tMaxAxisSign;                 // sign of largest axis of direction

  CStaticStackArray<BSPVertex> bvc_aVertices;  // array of vertices
public:
  DOUBLE3D bvc_vDirection;                  // direction of the split line

  /* Default constructor. */
  BSPVertexContainer(void);

  /* Initialize for a direction. */
  void Initialize(const DOUBLE3D &vDirection);
  /* Uninitialize. */
  void Uninitialize(void);

  /* Check if this container is in an unusable state (polygon coplanar with the splitter).*/
  inline BOOL IsPlannar(void) { return bvc_iMaxAxis==0; };

  /* Add a new vertex. */
  inline void AddVertex(const DOUBLE3D &vPoint);

  /* Sort vertices in this container along the largest axis of container direction. */
  void Sort(void);
  /* Elliminate paired vertices. */
  void ElliminatePairedVertices(void);
  /* Create edges from vertices in one container -- must be sorted before. */
  void CreateEdges(CDynamicArray<BSPEdge> &abedAll, size_t ulEdgeTag);
};

/*
 * Template class for BSP edge
 */
class BSPEdge {
public:
  DOUBLE3D bed_vVertex0;  // edge vertices
  DOUBLE3D bed_vVertex1;
  size_t bed_ulEdgeTag;   // tags for BSPs with tagged edges/planes

  /* Default constructor. */
  inline BSPEdge(void) : bed_ulEdgeTag(-1) {};
  /* Constructor with two vectors. */
  inline BSPEdge(const DOUBLE3D &vVertex0, const DOUBLE3D &vVertex1, size_t ulTag) :
    bed_vVertex0(vVertex0), bed_vVertex1(vVertex1), bed_ulEdgeTag(ulTag) {};
  /* Clear the object. */
  inline void Clear(void) {};
  // remove all edges marked for removal
  static void RemoveMarkedBSPEdges(CDynamicArray<BSPEdge> &abed);
  // optimize a polygon made out of BSP edges using tag information
  static void OptimizeBSPEdges(CDynamicArray<BSPEdge> &abed);
};

/*
 * Template class for polygons used in creating BSP-trees
 */
class BSPPolygon : public DOUBLEplane3D {
public:
  CDynamicArray<BSPEdge> bpo_abedPolygonEdges;  // array of edges in the polygon
  size_t bpo_ulPlaneTag;         // tags for BSPs with tagged planes (-1 for no tag)

  /* Add an edge to the polygon. */
  inline void AddEdge(const DOUBLE3D &vPoint0, const DOUBLE3D &vPoint1, size_t ulTag);

  /* Default constructor. */
  inline BSPPolygon(void) : bpo_ulPlaneTag(-1) {};
  /* Constructor with array of edges and plane. */
  inline BSPPolygon(
    DOUBLEplane3D &plPlane, CDynamicArray<BSPEdge> abedPolygonEdges, size_t ulPlaneTag)
    : DOUBLEplane3D(plPlane)
    , bpo_abedPolygonEdges(abedPolygonEdges)
    , bpo_ulPlaneTag(ulPlaneTag)
    {};

  /* Clear the object. */
  inline void Clear(void) {bpo_abedPolygonEdges.Clear();};
};

class BSPLine {
public:
  DOUBLE bl_tMin;
  DOUBLE bl_tMax;
};

/*
 * Template class for BSP-tree node of arbitrary dimensions and arbitrary type of members
 */
class BSPNode : public DOUBLEplane3D {  // split plane
public:
  enum BSPNodeLocation bn_bnlLocation;    // location of bsp node

  BSPNode *bn_pbnFront;        // pointer to child node in front of split plane
  BSPNode *bn_pbnBack;         // pointer to child node behind split plane
  size_t bn_ulPlaneTag;         // tags for BSPs with tagged planes (-1 for no tag)

public:
  /* Defualt constructor (for arrays only). */
  inline BSPNode(void) : bn_ulPlaneTag(-1), bn_pbnBack(NULL), bn_pbnFront(NULL), bn_bnlLocation(BNL_ILLEGAL) {};
  /* Constructor for a leaf node. */
  inline BSPNode(enum BSPNodeLocation bnl);
  /* Constructor for a branch node. */
  inline BSPNode(const DOUBLEplane3D &plSplitPlane, size_t ulPlaneTag, 
    BSPNode &bnFront, BSPNode &bnBack);
  /* Constructor for cloning a bsp (sub)tree. */
  BSPNode(BSPNode &bnRoot);
  /* Recursive destructor. */
  void DeleteBSPNodeRecursively(void);

  // find minimum/maximum parameters of points on a line that are inside - recursive
  void FindLineMinMax(BSPLine &bl, 
    const DOUBLE3D &v0,
    const DOUBLE3D &v1,
    DOUBLE t0, DOUBLE t1);

  /* Test if a sphere is inside, outside, or intersecting. (Just a trivial rejection test) */
  FLOAT TestSphere(const DOUBLE3D &vSphereCenter, DOUBLE tSphereRadius) const;
  /* Test if a box is inside, outside, or intersecting. (Just a trivial rejection test) */
  FLOAT TestBox(const OBBox<DOUBLE> &box) const;
};

/*
 * Template class that performs polygon cuts using BSP-tree
 */
class BSPCutter {
public:
  /* Split an edge with a plane. */
  static inline void SplitEdge(const DOUBLE3D &vPoint0, const DOUBLE3D &vPoint1, size_t ulEdgeTag,
    const DOUBLEplane3D &plSplitPlane,
    BSPPolygon &abedFront, BSPPolygon &abedBack,
    BSPVertexContainer &bvcFront, BSPVertexContainer &bvcBack);

  /* Cut a polygon with a BSP tree. */
  void CutPolygon(BSPPolygon &bpoPolygon, BSPNode &bn);

public:
  CDynamicArray<BSPEdge> bc_abedInside;       // edges of inside part of polygon
  CDynamicArray<BSPEdge> bc_abedOutside;      // edges of outside part of polygon
  CDynamicArray<BSPEdge> bc_abedBorderInside; // edges of border part of polygon facing inwards
  CDynamicArray<BSPEdge> bc_abedBorderOutside;// edges of border part of polygon facing outwards

  /* Split a polygon with a plane. */
  static inline BOOL SplitPolygon(BSPPolygon &bpoPolygon, const DOUBLEplane3D &plPlane, size_t ulPlaneTag,
    BSPPolygon &bpoFront, BSPPolygon &bpoBack);

  /* Constructor for splitting a polygon with a BSP tree. */
  BSPCutter(BSPPolygon &bpoPolygon, BSPNode &bnRoot);
  /* Destructor. */
  ~BSPCutter(void);
};


#endif  /* include-once check. */

