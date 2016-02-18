//
//  DrawTuioForces.cpp
//  opticalGFlow
//
//  Created by Robert Albert on 17.02.16.
//
//

#include "DrawTuioForces.h"

void DrawTuioForces::setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight) {
  simulationWidth = _simulationWidth;
  simulationHeight = _simulationHeight;
  densityWidth = (!_densityWidth)? simulationWidth : _densityWidth;
  densityHeight = (!_densityHeight)? simulationHeight: _densityHeight;
  
  numDrawForces = 6;
  drawForces = new ftDrawForce[numDrawForces];
  drawForces[0].setup(densityWidth, densityHeight, FT_DENSITY, true);
  drawForces[0].setName("draw full res");
  drawForces[1].setup(simulationWidth, simulationHeight, FT_VELOCITY, true);
  drawForces[1].setName("draw flow res 1");
  drawForces[2].setup(simulationWidth, simulationHeight, FT_TEMPERATURE, true);
  drawForces[2].setName("draw flow res 2");
  drawForces[3].setup(densityWidth, densityHeight, FT_DENSITY, false);
  drawForces[3].setName("draw full res");
  drawForces[4].setup(simulationWidth, simulationHeight, FT_VELOCITY, false);
  drawForces[4].setName("draw flow res 1");
  drawForces[5].setup(simulationWidth, simulationHeight, FT_TEMPERATURE, false);
  drawForces[5].setName("draw flow res 2");
  
  for (ofVec2f vec : last_touch_points)
  {
    vec.set(0,0);
  }
}

void DrawTuioForces::update(float _deltaTime) {
		deltaTime = _deltaTime;
		
		for (int i=0; i<numDrawForces; i++) {
      drawForces[i].update();
    }
}

//--------------------------------------------------------------
bool DrawTuioForces::didChange(int _index) {
		if (_index < 0 || _index >= numDrawForces) {
      ofLogWarning("ftDrawMouseForces::getDrawForceType: index out of range");
      return false;
    }
    else
      return drawForces[_index].didChange();
}

//--------------------------------------------------------------
ftDrawForceType DrawTuioForces::getType(int _index) {
		if (_index < 0 || _index >= numDrawForces) {
      ofLogWarning("ftDrawTuioForces::getDrawForceType: index out of range");
      return FT_NONE;
    }
    else
      return drawForces[_index].getType();
}

//--------------------------------------------------------------
ofTexture& DrawTuioForces::getTextureReference(int _index) {
		if (_index < 0 || _index >= numDrawForces) {
      ofLogError("ftDrawTuioForces::getTexture: index out of range");
    }
    else
      return drawForces[_index].getTexture();
		
}

//--------------------------------------------------------------
float DrawTuioForces::getStrength(int _index) {
		if (_index < 0 || _index >= numDrawForces) {
      ofLogWarning("ftDrawTuioForces::getStrength: index out of range");
      return 0;
    }
    else {
      if (drawForces[_index].getIsTemporary()) {
        return drawForces[_index].getStrength();
      }
      else {
        return drawForces[_index].getStrength() * deltaTime;;
      }
    }
}

//--------------------------------------------------------------
void DrawTuioForces::tuioUpdated(float _x, float _y, int _ID) {
  ofVec2f this_point;
  
  this_point.set(_x, _y);
  ofVec2f velocity = this_point - last_touch_points[_ID];

  for (int i=0; i<3; i++) {
    if (drawForces[i].getType() == FT_VELOCITY)
      drawForces[i].setForce(velocity);
      drawForces[i].applyForce(this_point);
    }
  
  last_touch_points[_ID].set(this_point.x, this_point.y);
}


