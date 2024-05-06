#include "pch.h"
#include "delauney.h"
#include <iostream>
#include <vector>
#include <array>
#include <stack>


using namespace delaunay;



//	  | a b | |s|    w1
//    | c d | |t|  = w2
static bool Delaunay_solve2by2Eq(
  const double a, const double b,
  const double c, const double d, 
  const double w1, const double w2,
  double& s, double& t)
{
  double det = (a * d - b * c);
  if (det == 0) return false;
  det = 1.0 / det;
  s = (d * w1 - b * w2) * det;
  t = (-c * w1 + a * w2) * det;
  return true;
}



//2辺の垂直二等分線の交点が中心　外接円
static bool Delaunay_CircumCircle(
    const HEVert& x0, 
    const HEVert& x1,
    const HEVert& x2, 
    double &cx, double &cy, double &cr) 
{
  double a = x0.x - x1.x;
  double b = x0.y - x1.y;
  double c = x1.x - x2.x;
  double d = x1.y - x2.y;

  double b0 = 0.5 * (x0.NormSq() - x1.NormSq());
  double b1 = 0.5 * (x1.NormSq() - x2.NormSq());

  if (!Delaunay_solve2by2Eq(a, b, c, d, b0, b1, cx, cy)) return false;

  cr = (x0.x - cx) * (x0.x - cx) + 
       (x0.y - cy) * (x0.y - cy);
  cr = sqrt(cr);
  return true;
}


static bool Delaunay_bPointInCircumCircle(
  const HEVert& x0,
  const HEVert& x1,
  const HEVert& x2,
  const HEVert& p)
{
  double cx, cy, cr;
  if (!Delaunay_CircumCircle(x0,x1,x2, cx, cy, cr)) return true; // x0 x1 x2が直線なら crが無限と考える

  double d = (p.x - cx) * (p.x - cx) + (p.y - cy) * (p.y - cy);
  return d < cr*cr;  
}




static void Delaunay_CalcBoundingBox(
  const std::vector<std::array<double, 2>>& points,
  double& minx, double& miny,
  double& maxx, double& maxy
  )
{
  if (points.size() <= 0)return;

  //step1 huge triangle 
  minx = points[0][0], miny = points[0][1];
  maxx = points[0][0], maxy = points[0][1];
  for (int i = 0; i < (int)points.size(); ++i)
  {
    minx = std::min(points[i][0], minx);
    miny = std::min(points[i][1], miny);
    maxx = std::max(points[i][0], maxx);
    maxy = std::max(points[i][1], maxy);
  }
}


void DelaunayMesh::InitMesh(std::vector<std::array<double, 2>>& points)
{
  if (points.size() <= 0)return;

  m_verts.clear();
  m_faces.clear();
  m_edges.clear();

  // step1 generate huge triangle  
  double minx, miny, maxx, maxy; 
  Delaunay_CalcBoundingBox(points, minx, miny, maxx, maxy);
  
  HEVert v0(minx - 100       , miny - 100, 0);
  HEVert v1(maxx + 100       , miny - 100, 1);
  HEVert v2(0.5*(minx + maxx), maxy + 100, 2);
  HEEdge e0(0, -1, 1, 0);
  HEEdge e1(1, -1, 2, 0);
  HEEdge e2(2, -1, 0, 0);
  HEFace f0(0);
  m_verts = { v0, v1, v2 };
  m_edges = { e0, e1, e2 };
  m_faces = { f0 };

  // Step2 add all vertex 
  for (int i = 0; i < (int)points.size(); ++i)
  {
    AddNewVertex(points[i][0], points[i][1]);
  }

  // Step2 remove triangles related to (v0, v1,v2)
  std::vector<std::array<double, 2>> verts;
  std::vector<std::array<int   , 3>> faces;

  for (int i = 3; i < (int)m_verts.size(); ++i)
  {
    verts.push_back({ m_verts[i].x, m_verts[i].y });
  }
  
  for (int i = 0; i < (int)m_faces.size(); ++i)
  {
    int v0,v1,v2,e0,e1,e2;
    GetFaceVsEs(i, e0,e1,e2, v0,v1,v2);
    if (v0 <= 2 || v1 <= 2 || v2 <= 2) continue;

    faces.push_back({v0-3, v1-3, v2-3});
  }
  InitByVsFs(verts, faces);

}



//returns {(b-a)X(c-a)}.z
static double CrossProductZ(const HEVert &a, const HEVert &b, const HEVert &c) 
{
  return (b.x - a.x) * (c.y - a.y) - 
         (b.y - a.y) * (c.x - a.x);
}

static bool isInTriangle(
    const HEVert& p,
    const HEVert& v0,
    const HEVert& v1,
    const HEVert& v2)
{
  double d0 = CrossProductZ(v0, v1, p);
  double d1 = CrossProductZ(v1, v2, p);
  double d2 = CrossProductZ(v2, v0, p);
  return (d0 > 0 && d1 > 0 && d2 > 0);
}


int DelaunayMesh::SearchFaceCotainPoint(double x, double y)
{
  HEVert p(x,y,-1);

  for (int i = 0; i < m_faces.size(); ++i)
  {
    const HEEdge& e0 = m_edges[m_faces[i].edge];
    const HEEdge& e1 = m_edges[e0.next];
    const HEEdge& e2 = m_edges[e1.next];

    if (isInTriangle(p, m_verts[e0.vert], m_verts[e1.vert], m_verts[e2.vert]))
      return i;
  }

  return -1;
}



void DelaunayMesh::AddNewVertex(double x, double y)
{
  int f0idx = SearchFaceCotainPoint(x,y);
  if (f0idx < 0) return;
  //existing triangle  
  
  const int e0idx = m_faces[f0idx].edge;
  const int e1idx = m_edges[e0idx].next;
  const int e2idx = m_edges[e1idx].next;
  const int v0idx = m_edges[e0idx].vert;
  const int v1idx = m_edges[e1idx].vert;
  const int v2idx = m_edges[e2idx].vert;

  //Add new vertex/face/edge 
  const int v3idx = (int)m_verts.size();
  const int f1idx = (int)m_faces.size() + 0;
  const int f2idx = (int)m_faces.size() + 1;
  const int e3idx = (int)m_edges.size() + 0, e4idx = (int)m_edges.size() + 1;
  const int e5idx = (int)m_edges.size() + 2, e6idx = (int)m_edges.size() + 3;
  const int e7idx = (int)m_edges.size() + 4, e8idx = (int)m_edges.size() + 5;

  m_verts.push_back(HEVert(x, y, e4idx));
  m_faces.push_back(HEFace(e8idx));
  m_faces.push_back(HEFace(e6idx));

  m_edges.push_back(HEEdge(v1idx, e8idx, e4idx, f0idx));//e3
  m_edges.push_back(HEEdge(v3idx, e5idx, e0idx, f0idx));//e4
  m_edges.push_back(HEEdge(v0idx, e4idx, e6idx, f2idx));//e5
  m_edges.push_back(HEEdge(v3idx, e7idx, e2idx, f2idx));//e6
  m_edges.push_back(HEEdge(v2idx, e6idx, e8idx, f1idx));//e7
  m_edges.push_back(HEEdge(v3idx, e3idx, e1idx, f1idx));//e8

  //modify existing face/edge
  m_faces[f0idx].edge = e0idx;
  m_verts[v0idx].edge = e0idx;
  m_verts[v1idx].edge = e1idx;
  m_verts[v2idx].edge = e2idx;

  m_edges[e0idx].SetNextFace(e3idx, f0idx);
  m_edges[e1idx].SetNextFace(e7idx, f1idx);
  m_edges[e2idx].SetNextFace(e5idx, f2idx);

 
  std::stack<int> Q;
  Q.push(e0idx);
  Q.push(e1idx);
  Q.push(e2idx);

  while (!Q.empty())
  {
    const int piv = Q.top();
    Q.pop();
    if (m_edges[piv].oppo == -1) continue;

    const int e0idx = piv;
    const int e1idx = m_edges[e0idx].next;
    const int e2idx = m_edges[e1idx].next;
    const int e3idx = m_edges[e0idx].oppo;
    const int e4idx = m_edges[e3idx].next;
    const int e5idx = m_edges[e4idx].next;
    
    const int v0idx = m_edges[e0idx].vert;
    const int v1idx = m_edges[e1idx].vert;
    const int v2idx = m_edges[e2idx].vert;
    const int v3idx = m_edges[e5idx].vert;
    
    const int f0idx = m_edges[e0idx].face;
    const int f1idx = m_edges[e3idx].face;

    bool tf = Delaunay_bPointInCircumCircle(m_verts[v0idx], m_verts[v1idx], m_verts[v2idx], m_verts[v3idx]);
    if ( !tf ) continue;

    //flip!
    m_edges[e0idx].SetVertNext(v2idx, e5idx);
    m_edges[e1idx].next = e0idx;
    m_edges[e2idx].SetNextFace(e4idx, f1idx);

    m_edges[e3idx].SetVertNext(v3idx, e2idx);
    m_edges[e4idx].next = e3idx;
    m_edges[e5idx].SetNextFace(e1idx, f0idx);

    m_faces[f0idx].edge = e0idx;
    m_faces[f1idx].edge = e3idx;
    
    m_verts[v0idx].edge = e4idx;
    m_verts[v1idx].edge = e1idx;
    m_verts[v2idx].edge = e2idx;
    m_verts[v3idx].edge = e5idx;
    
    Q.push(e4idx);
    Q.push(e5idx);
  }
}



bool DelaunayMesh::CheckAllEdge()
{
  bool result = true;

  for (int ei = 0; ei < (int)m_edges.size(); ++ei)
  {
    if (m_edges[ei].oppo == -1) continue;

    const int e0idx = ei;
    const int e1idx = m_edges[e0idx].next;
    const int e2idx = m_edges[e1idx].next;
    const int e3idx = m_edges[e0idx].oppo;
    const int e4idx = m_edges[e3idx].next;
    const int e5idx = m_edges[e4idx].next;
    const int v0idx = m_edges[e0idx].vert;
    const int v1idx = m_edges[e1idx].vert;
    const int v2idx = m_edges[e2idx].vert;
    const int v3idx = m_edges[e5idx].vert;

    bool tf1 = Delaunay_bPointInCircumCircle(m_verts[v0idx], m_verts[v1idx], m_verts[v2idx], m_verts[v3idx]);
    bool tf2 = Delaunay_bPointInCircumCircle(m_verts[v0idx], m_verts[v3idx], m_verts[v1idx], m_verts[v2idx]);

    if ( tf1 || tf2 )
    { 
      std::cout << "error " << ei << "\n";
      result = false;
    }
  }

  return result;

}




void DelaunayMesh::GetFaceVsEs(
  int fidx,
  int& e0, int& e1, int& e2,
  int& v0, int& v1, int& v2) const
{
  e0 = m_faces[fidx].edge;
  e1 = m_edges[e0].next;
  e2 = m_edges[e1].next;
  v0 = m_edges[e0].vert;
  v1 = m_edges[e1].vert;
  v2 = m_edges[e2].vert;
}



void DelaunayMesh::InitByVsFs(
  std::vector<std::array<double, 2>>& verts,
  std::vector<std::array<int   , 3>>& faces)
{
  std::vector<HEVert> new_vs;
  std::vector<HEEdge> new_es;
  std::vector<HEFace> new_fs;

  for (int i = 0; i < (int)verts.size(); ++i )
  { 
    new_vs.push_back(HEVert(verts[i][0], verts[i][1], -1));
  }

  //頂点 verts[i]から放射されるedgeidを保持  
  std::vector<std::vector<int>> v_out_edge(new_vs.size());

  for (int fi = 0; fi < (int)faces.size(); ++fi)
  {
    int v0 = faces[fi][0];
    int v1 = faces[fi][1];
    int v2 = faces[fi][2];
    int e0 = (int)new_es.size();
    int e1 = (int)new_es.size() + 1;
    int e2 = (int)new_es.size() + 2;

    //search opposite edges
    int e0oppo = -1, e1oppo = -1, e2oppo = -1;

    for (auto& e : v_out_edge[v1]) if (new_es[new_es[e].next].vert == v0) e0oppo = e;
    for (auto& e : v_out_edge[v2]) if (new_es[new_es[e].next].vert == v1) e1oppo = e;
    for (auto& e : v_out_edge[v0]) if (new_es[new_es[e].next].vert == v2) e2oppo = e;

    if (e0oppo != -1) new_es[e0oppo].oppo = e0;
    if (e1oppo != -1) new_es[e1oppo].oppo = e1;
    if (e2oppo != -1) new_es[e2oppo].oppo = e2;
    new_es.push_back(HEEdge(v0, e0oppo, e1, fi));
    new_es.push_back(HEEdge(v1, e1oppo, e2, fi));
    new_es.push_back(HEEdge(v2, e2oppo, e0, fi));
    new_fs.push_back(HEFace(e0));

    v_out_edge[v0].push_back(e0);
    v_out_edge[v1].push_back(e1);
    v_out_edge[v2].push_back(e2);
  }

  m_verts = new_vs;
  m_edges = new_es;
  m_faces = new_fs;

}



double DelaunayMesh::CalcAverateEdgeLength()
{
  double sum = 0;
  for (int i = 0; i < (int)m_edges.size(); ++i)
  {
      int v1 = m_edges[i].vert;
      int v2 = m_edges[m_edges[i].next].vert;
      sum += HEVert::Distance(m_verts[v1], m_verts[v2]);
  }
  return sum / (double) m_edges.size();
}


void DelaunayMesh::RemoveBoundingFacesWithLongEdge(double r)
{
  //step1 mark faces to remove 
  std::vector<bool> face_flg(m_faces.size(), false);

  while (true)
  {
    bool updated = false;
    for (int i = 0; i < m_faces.size(); ++i)
    {
      if (face_flg[i]) continue;

      int e0, e1, e2, v0, v1, v2;
      GetFaceVsEs(i, e0, e1, e2, v0, v1, v2);

      bool isBoundary = 
        (m_edges[e0].oppo == -1 || face_flg[m_edges[m_edges[e0].oppo].face]) ||
        (m_edges[e1].oppo == -1 || face_flg[m_edges[m_edges[e1].oppo].face]) ||
        (m_edges[e2].oppo == -1 || face_flg[m_edges[m_edges[e2].oppo].face]);
      if (!isBoundary) continue;

      double d0 = HEVert::Distance(m_verts[v0], m_verts[v1]);
      double d1 = HEVert::Distance(m_verts[v1], m_verts[v2]);
      double d2 = HEVert::Distance(m_verts[v2], m_verts[v0]);
      if (d0 <= r && d1 <= r && d2 <= r) continue;

      //remove this face
      face_flg[i] = true;
      updated = true;
    }
    if (!updated)break;
  }

  //step2 mark verts to remove
  std::vector<bool> vert_flg(m_verts.size(), true);
  for (int i = 0; i < m_faces.size(); ++i)
  {
    if (face_flg[i]) continue;
    int e0, e1, e2, v0, v1, v2;
    GetFaceVsEs(i, e0, e1, e2, v0, v1, v2);
    vert_flg[v0] = vert_flg[v1] = vert_flg[v2] = false;
  }

  //step3 listup all triangles and vertex
  std::vector<std::array<double, 2>> new_vs;
  std::vector<std::array<int, 3>>    new_fs;

  //new idx of new_vs
  std::vector<int> new_vidx(m_verts.size(), -1);

  for (int i = 0; i < m_verts.size(); ++i)
  {
    if (vert_flg[i]) continue;
    new_vidx[i] = (int)new_vs.size();
    new_vs.push_back({m_verts[i].x, m_verts[i].y});
  }
    
  for (int i = 0; i < m_faces.size(); ++i)
  {
    if (face_flg[i]) continue;
    int e0, e1, e2, v0, v1, v2;
    GetFaceVsEs(i, e0, e1, e2, v0, v1, v2);
    new_fs.push_back({ new_vidx[v0], new_vidx[v1], new_vidx[v2]});
  }

  this->InitByVsFs(new_vs, new_fs);
}
