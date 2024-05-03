#pragma once


#pragma unmanaged
#include "OglForCLI.h"

#include "tmath.h"
#include <vector>
#include "delauney.h"

class EventManager
{
private:
  bool m_isL, m_isR, m_isM; 
  EventManager();

  
  delaunay::DelaunayMesh m_mesh;

public:
  static EventManager* GetInst() {
    static EventManager p;
    return &p;
  }

  void DrawScene();
  void LBtnDown(int x, int y, OglForCLI* ogl);
  void MBtnDown(int x, int y, OglForCLI* ogl);
  void RBtnDown(int x, int y, OglForCLI* ogl);
  void LBtnUp(int x, int y, OglForCLI* ogl);
  void MBtnUp(int x, int y, OglForCLI* ogl);
  void RBtnUp(int x, int y, OglForCLI* ogl);
  void MouseMove(int x, int y, OglForCLI* ogl);


};



#pragma managed
