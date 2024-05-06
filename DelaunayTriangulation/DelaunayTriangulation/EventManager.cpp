#include "pch.h"
#include "EventManager.h"
#include <random>
#include "MainForm.h"

#include <vector>
#include <array>

#pragma unmanaged



EventManager::EventManager()
{
  m_isL = m_isR = m_isM = false;

  //generate point cloud 
  const int N = 300;
  std::vector<std::array<double,2>> points;
  for (int i = 0; i < N; ++i) 
  {
    float x = 5 * (rand() / (float)RAND_MAX * 2.0f - 1.0f);
    float y = 5 * (rand() / (float)RAND_MAX * 2.0f - 1.0f);
    points.push_back({x,y});
  }
    
  m_mesh.InitMesh(points);
  std::cout << "check Delaunay: " << m_mesh.CheckAllEdge() << "\n";

  double ave_len = m_mesh.CalcAverateEdgeLength();
  m_mesh.RemoveBoundingFacesWithLongEdge(2.0 * ave_len);
  std::cout << "check Delaunay: " << m_mesh.CheckAllEdge() << "\n";

}

void EventManager::DrawScene()
{
  glLineWidth(2.0f);
  glDisable(GL_LIGHTING);//“d‹C‚ðÁ‚·

  //draw triangles;
  const std::vector<delaunay::HEEdge>& es = m_mesh.m_edges;
  const std::vector<delaunay::HEFace>& fs = m_mesh.m_faces;
  const std::vector<delaunay::HEVert>& vs = m_mesh.m_verts;

  glColor3d(1,1,0);
  glPointSize(8);
  glBegin(GL_POINTS);
  for(int i = 0; i < vs.size(); ++i)
    glVertex3f(vs[i].x, vs[i].y, 0);
  glEnd();


  glBegin(GL_LINES);
  for (int i = 0; i < es.size(); ++i)
  {
    if (es[i].oppo == -1) glColor3d(1,0,0);
    else glColor3d(1,1,1);
    const delaunay::HEVert& v0 = vs[es[i].vert];
    const delaunay::HEVert& v1 = vs[es[es[i].next].vert];
    glVertex3d(v0.x, v0.y, 0);
    glVertex3d(v1.x, v1.y, 0);
  }
  glEnd();

  //only boundary 
  glBegin(GL_LINES);
  glColor3d(1, 0, 0);
  for (int i = 0; i < es.size(); ++i)
  {
    if (es[i].oppo != -1) continue;  
    const delaunay::HEVert& v0 = vs[es[i].vert];
    const delaunay::HEVert& v1 = vs[es[es[i].next].vert];
    glVertex3d(v0.x, v0.y, 0.5);
    glVertex3d(v1.x, v1.y, 0.5);
  }
  glEnd();




}

void EventManager::LBtnDown(int x, int y, OglForCLI* ogl)
{
  m_isL = true;
  ogl->BtnDown_Trans(EVec2i(x, y)); // OpenGL‚ÌŽ‹“_‚ð‰ñ“]‚³‚¹‚é€”õ
}

void EventManager::MBtnDown(int x, int y, OglForCLI* ogl)
{
  m_isM = true;
  ogl->BtnDown_Zoom(EVec2i(x, y));
}

void EventManager::RBtnDown(int x, int y, OglForCLI* ogl)
{
  m_isR = true;
  ogl->BtnDown_Rot(EVec2i(x, y));
}

void EventManager::LBtnUp(int x, int y, OglForCLI* ogl)
{
  m_isL = false;
  ogl->BtnUp();
}

void EventManager::MBtnUp(int x, int y, OglForCLI* ogl)
{
  m_isM = false;
  ogl->BtnUp();
}

void EventManager::RBtnUp(int x, int y, OglForCLI* ogl)
{
  m_isR = false;
  ogl->BtnUp();
}

void EventManager::MouseMove(int x, int y, OglForCLI* ogl)
{
  if (!m_isL && !m_isR && !m_isM) return;
  ogl->MouseMove(EVec2i(x, y));
  DelaunayTriangulation::MainFormRedraw();
}
