//
//  DrawTuioForces.hpp
//  opticalGFlow
//
//  Created by Robert Albert on 17.02.16.
//
//

#ifndef DrawTuioForces_hpp
#define DrawTuioForces_hpp

#include <stdio.h>
#include "ftDrawMouseForces.h"

using namespace flowTools;

class DrawTuioForces {
public:
  
  void setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight);
  void tuioUpdated(float _x, float _y, int _ID);
  void				update(float _deltaTime);
  void				reset()  { for (int i=0; i<numDrawForces; i++) drawForces[i].reset(); }
  
  int					getNumForces() { return numDrawForces; }
  
  bool				didChange(int _index);
  ftDrawForceType		getType(int _index);
  ofTexture&			getTextureReference(int _index);
  float				getStrength(int _index);
  
private:
  ofParameter<bool>	doResetDrawForces;
  void resetDrawForcesListner(bool& _value) {
    if (_value) {
      reset();
    }
    doResetDrawForces.set(false);
  }
  
  int					numDrawForces;
  ftDrawForce*		drawForces;
  
  float				deltaTime;
  
  int					simulationWidth;
  int					simulationHeight;
  int					densityWidth;
  int					densityHeight;
  
  void				mouseMoved(ofMouseEventArgs & mouse);
  void				mouseDragged(ofMouseEventArgs & mouse);
  
  
  
  ofVec2f       last_touch_points[12];

};

#endif /* DrawTuioForces_hpp */
