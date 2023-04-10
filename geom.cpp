#include "geom.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <iostream>
#include <set>

#include <vector>

#define EPSILON .001

using namespace std; 

double det2(double a1, double a2, double b1, double b2) {
  return a1 * b1 - a2 * b2;
}

double det3(double a1, double a2, double a3, double b1, double b2, double b3, double c1, double c2, double c3) {
  return a1 * det2(b2, b3, c2, c3) - a2 * det2(b1, b3, c1, c3) + a3 * det2(b1, b2, c1, c2);
}

/* returns 6 times the signed volume of abcd. The volume is positive
   if d is behind abc (i.e. on opposite side as the normal); negative
   if d is in front (i.e. same side as the normal) of abc, and 0 if
   abcd are coplanar.
 */
double signed_volume(point3d a, point3d b, point3d c, point3d d) {

  double det = a.x * det3(b.y, b.z, 1, c.y, c.z, 1, d.y, d.z, 1) 
  - a.y * det3(b.x, b.z, 1, c.x, c.y, 1, d.x, d.z, 1) 
  + a.z * det3(b.x, b.y, 1, c.x, c.y, 1, d.x, d.y, 1) 
  - det3(b.x, b.y, b.z, c.x, c.y, c.z, d.x, d.y, d.z);

  return det; 
}


/* return True if points are on the same plane, and False otherwise */
bool  coplanar(point3d a, point3d b, point3d c, point3d d) {
  double vol = signed_volume(a, b, c, d);

  if (vol < EPSILON && vol > - EPSILON) return true;

  return false;
}


/* return True if d is  strictly in front of abc; False otherwise */
bool  infront (point3d a, point3d b, point3d c, point3d d) {

  double vol = signed_volume(a, b, c, d);

  return (vol < EPSILON); 
}


bool face_is_extreme(int i, int j, int k, vector<point3d> & points) {

  // printf("face:");
  int nFront = 0;
  int nBack = 0;
  for (int s = 0; s < points.size(); s++) {
    if ((s == i) || (s == j) || (s == k)) continue;
    if (coplanar(points[i], points[j], points[k], points[s])) continue;
    assert(!coplanar(points[i], points[j], points[k], points[s]));

    if (infront(points[i], points[j], points[k], points[s])) {
      //printf("front\n");
      nFront++;
    }
    else {
      //printf("back\n");
      nBack++;
    }

    //printf(" %d %d", nFront, nBack);

    if (nFront * nBack > 0) {
      return false;
    }

    
  }

  if (nFront == 0 || nBack == 0) {
      return true;
  }

  return false;
}

bool pointEqual(point3d p1, point3d p2) {
  return (p1.x == p2.x) && (p1.y == p2.y) && (p1.z == p2.z);
}

bool face_is_extreme(point3d p1, point3d p2, point3d p3, vector<point3d> & points) {

  // printf("face:");
  int nFront = 0;
  int nBack = 0;
  for (int s = 0; s < points.size(); s++) {
    if ((pointEqual(p1, points[s])) || pointEqual(p2, points[s]) || pointEqual(p3, points[s])) continue;
    if (coplanar(p1, p2, p3, points[s])) continue;
    assert(!coplanar(p1, p2, p3, points[s]));

    if (infront(p1, p2, p3, points[s])) {
      //printf("front\n");
      nFront++;
    }
    else {
      //printf("back\n");
      nBack++;
    }

    //printf(" %d %d", nFront, nBack);

    if (nFront * nBack > 0) {
      return false;
    }

    
  }

  if (nFront == 0 || nBack == 0) {
      return true;
  }

  return false;
}

/* compute the convex hull of the points */
void naive_hull(vector<point3d>& points, vector<triangle3d>& hull) {
  
  //your code goes here 
  for (int i = 0; i < points.size(); i++) {
    for (int j = i + 1; j < points.size(); j++) {
      for (int k = j + 1; k < points.size(); k++) {
        if (face_is_extreme(i, j, k, points)) {

          printf("face is extreme\n");

          triangle3d t;
          t.a = & points[i];
          t.b = & points[j];
          t.c = & points[k];

          t.color.r = (float)rand()/RAND_MAX;
          t.color.g = (float)rand()/RAND_MAX;
          t.color.b = (float)rand()/RAND_MAX;

          hull.push_back(t);
        }
      }
    }
  }
}

/* compute the convex hull of the points */
void incremental_hull(vector<point3d>& points, vector<triangle3d> & hull) {
  
  //your code goes here
  
  

} 

/* compute the convex hull of the points */
void giftwrapping_hull(vector<point3d>& points, vector<triangle3d>& hull) {

  vector<point3d> & remainingPoints = points;

  set<edge> edges;
  
  point3d firstPoint;
  int firstPointIndex;
  double currMin = -INT_MAX;

  for (int i = 0; i < points.size(); i++) {
    if (points[i].x > currMin) {
      firstPoint = points[i];
      firstPointIndex = i;
      currMin = points[i].x;
    }
  }

  // find second point

  point3d tempPoint;
  tempPoint.x = firstPoint.x + 1;
  tempPoint.y = firstPoint.y;
  tempPoint.z = firstPoint.z;

  int secondPointIndex = -1;
  for (int i = 0; i < points.size(); i++) {
    if (i == firstPointIndex) continue;
    if (secondPointIndex == -1) {
      secondPointIndex = i;
    }
    else {
      if (infront(points[firstPointIndex], tempPoint, points[secondPointIndex], points[i])) {
        secondPointIndex = i;
      }
    }
  }

  point3d secondPoint = points[secondPointIndex];

  // find third point

  int thirdPointIndex = -1;
  for (int i = 0; i < points.size(); i++) {
    if (i == firstPointIndex) continue;
    if (i == secondPointIndex) continue;
    if (thirdPointIndex == -1) {
      thirdPointIndex = i;
    }
    else {
      if (infront(points[firstPointIndex], points[secondPointIndex], points[thirdPointIndex], points[i])) {
        thirdPointIndex = i;
      }
    }
  }

  point3d thirdPoint = points[thirdPointIndex];


  cout << "got here" << endl;
  triangle3d face;
  face.a = & points[firstPointIndex];
  face.b = & points[secondPointIndex];
  face.c = & points[thirdPointIndex];

  face.color.r = (float)rand()/RAND_MAX;
  face.color.g = (float)rand()/RAND_MAX;
  face.color.b = (float)rand()/RAND_MAX;

  hull.push_back(face);

  remainingPoints.erase(remainingPoints.begin() + firstPointIndex);
  remainingPoints.erase(remainingPoints.begin() + secondPointIndex);
  remainingPoints.erase(remainingPoints.begin() + thirdPointIndex);

  


  while (remainingPoints.size() > 0) {
    // take a random edge in the existing hull
    edge edgeChosen;

    for (int i = 0; i < hull.size(); i++) {

      edge currentEdge;
      
      currentEdge.a = * hull[i].a;
      currentEdge.b = * hull[i].b;

      if (!(edges.find(currentEdge) == edges.end())) {
        edgeChosen = currentEdge;
      }

      currentEdge.a = * hull[i].a;
      currentEdge.b = * hull[i].c;

      if (!(edges.find(currentEdge) == edges.end())) {
        edgeChosen = currentEdge;
      }

      currentEdge.a = * hull[i].b;
      currentEdge.b = * hull[i].c;

      if (!(edges.find(currentEdge) == edges.end())) {
        edgeChosen = currentEdge;
      }

    }

    // add this edge to the edges set to avoid choosing it again in the future

    edges.insert(edgeChosen);

    // now, begin to look through each remaining point and see which face formed is extreme. If extreme, add to the hull

    for (int j = 0; j < remainingPoints.size(); j++) {
      triangle3d newFace;
      newFace.a = & edgeChosen.a;
      newFace.b = & edgeChosen.b;
      newFace.c = & remainingPoints[j];

      if (face_is_extreme(* newFace.a, * newFace.b, * newFace.c, remainingPoints)) {
        hull.push_back(newFace);

        remainingPoints.erase(remainingPoints.begin() + j);
      }
    }
  }

} 



