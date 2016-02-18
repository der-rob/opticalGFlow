#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxFlowTools.h"
#include "ofxSyphon.h"
#include "ofxAssimpModelLoader.h"
#include "ofxTuio.h"
#include "DrawTuioForces.h"
#include "LEDFrame.h"

//#define USE_PROGRAMMABLE_GL					// Maybe there is a reason you would want to
#define USE_FASTER_INTERNAL_FORMATS			// about 15% faster but gives errors from ofGLUtils

using namespace flowTools;

class opticalGFlowApp : public ofBaseApp{
public:
	void	setup();
	void	update();
	void	draw();
  void exit();
	
	// Camera
	ftFbo				canvasFbo;
  ofxSyphonServer syphonMainOut;
	
	// Time
	float				lastTime;
	float				deltaTime;
	
	// FlowTools
	int					flowWidth;
	int					flowHeight;
	int					drawWidth;
	int					drawHeight;
	
	ftOpticalFlow		opticalFlow;
	ftVelocityMask		velocityMask;
	ftFluidSimulation	fluidSimulation;
	ftParticleFlow		particleFlow;
	
	ftVelocitySpheres	velocityDots;
	
	// MouseDraw
	ftDrawMouseForces	mouseForces;
  // TUIO Draw
  DrawTuioForces tuioForces;
	
	// GUI
	ofxPanel			gui;
	void				setupGui();
	void				keyPressed(int key);
	void				drawGui();
	ofParameter<bool>	toggleGuiDraw;
	ofParameter<float>	guiFPS;
	ofParameter<float>	guiMinFPS;
	deque<float>		deltaTimeDeque;
	ofParameter<bool>	doFullScreen;
  ofParameter<bool> use_mouse;
  ofParameter<bool> use_tuio;
	void				setFullScreen(bool& _value) { ofSetFullscreen(_value);}
	
	// DRAW
	ofParameter<bool>	doDrawWireframe;
  ofParameter<ofColor> fluidColor;
		
	void				drawComposite()			{ drawComposite(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
	void				drawComposite(int _x, int _y, int _width, int _height);
	void				drawParticles()			{ drawParticles(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
	void				drawParticles(int _x, int _y, int _width, int _height);
  void				drawFluidObstacle()		{ drawFluidObstacle(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
	void				drawFluidObstacle(int _x, int _y, int _width, int _height);
	void				drawSource()			{ drawSource(0, 0, ofGetWindowWidth(), ofGetWindowHeight()); }
	void				drawSource(int _x, int _y, int _width, int _height);

  //TUIO
  ofxTuioClient tuioClient;
  void tuioAdded(ofxTuioCursor &cursor);
  void tuioRemoved(ofxTuioCursor &cursor);
  void tuioUpdated(ofxTuioCursor &cursor);
  ofParameter<int> tuioPort;
  ofVec2f tuioPoint;
  
  // 3D assets and video
  ofxAssimpModelLoader the_car;
  ofMesh the_car_mesh;
  vector<string> meshnames;
  ofVideoPlayer the_car_video;
  ofParameter<bool> use_mesh;
  ofParameter<float> lineThickness;
  
  LEDFrame ledFrame;
};
