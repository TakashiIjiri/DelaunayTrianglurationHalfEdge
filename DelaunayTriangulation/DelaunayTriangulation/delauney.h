#pragma once

#include <vector>
#include <array>
#include <iostream>

namespace delaunay 
{

class HEVert 
{
public:
  double x, y;
  int   edge;//��������o�Ă���edge idx
  HEVert(double x, double y, int _edge = -1) : x(x), y(y), edge(_edge) {}

  double NormSq()const{ return x*x + y*y;}
};

class HEFace 
{
  public:
    int edge; // ���͂�edge���
    HEFace(int _edge = -1) : edge(_edge) {}
};


/*-----------------------------
* User half edge data structure 
* 
         e.next.vert
          * 
        /| |
 e.next/ | |
      / e| |e.oppo
      \  | |
 e.next\ | |
  .next   +      
         e.vert
 
//��������
//oppo = -1 �Ȃ炻�̎O�p�`�̓o�E���_��
-----------------------------*/

class HEEdge 
{
public:
  int vert; 
  int oppo;
  int next;
  int face;
  HEEdge(int _vert = -1, int _oppo = -1, int _next = -1, int _face = -1) : 
    vert(_vert), oppo(_oppo), next(_next), face(_face) {}


  void SetNextFace(int _next, int _face)
  {
    next = _next;
    face = _face;
  }

  void SetVertNext(int _vert, int _next)
  {
    vert = _vert;
    next = _next;
  }

  void Trace() {
    std::cout << "-------------\n";
    std::cout << "vert: " << vert << " oppo: " << oppo << " next: " << next << " face: " << face << "\n";
  }


};


class DelaunayMesh
{
public:
  std::vector<HEVert> m_verts;
  std::vector<HEFace> m_faces;
  std::vector<HEEdge> m_edges;

  DelaunayMesh(){}
  void InitMesh(std::vector<std::array<double,2>>& points);

  bool CheckAllEdge();

private:
  int SearchFaceCotainPoint(double x, double y);
  void AddNewVertex(double x, double y);


  //get (v0,v1,v2) and (e0,e1,e2) of face[fidx]
  void GetFaceVsEs(int fidx, int &e0, int &e1, int &e2, 
                             int &v0, int &v1, int &v2) const;


  void InitByVsFs(std::vector<std::array<double,2>> &verts, 
                  std::vector<std::array<int   ,3>> &faces);
};



}




