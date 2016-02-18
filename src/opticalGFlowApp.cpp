#include "opticalGFlowApp.h"

//--------------------------------------------------------------
void opticalGFlowApp::setup(){
  ofSetVerticalSync(false);
  ofSetFullscreen(TRUE);
	ofSetLogLevel(OF_LOG_ERROR);
  
  drawWidth = ofGetWidth();
  drawHeight = ofGetHeight();
  ofSetWindowShape(drawWidth, drawHeight);
  
	// process all but the density on 16th resolution
	flowWidth = drawWidth / 2;
	flowHeight = drawHeight / 2;
	
	// FLOW & MASK
	opticalFlow.setup(flowWidth, flowHeight);
	velocityMask.setup(drawWidth, drawHeight);
	
	// FLUID & PARTICLES
#ifdef USE_FASTER_INTERNAL_FORMATS
	fluidSimulation.setup(flowWidth, flowHeight, drawWidth, drawHeight, true);
	particleFlow.setup(flowWidth, flowHeight, drawWidth, drawHeight, true);
#else
	fluidSimulation.setup(flowWidth, flowHeight, drawWidth, drawHeight, false);
	particleFlow.setup(flowWidth, flowHeight, drawWidth, drawHeight, false);
#endif
		
	// MOUSE DRAW
	mouseForces.setup(flowWidth, flowHeight, drawWidth, drawHeight);
  tuioForces.setup(flowWidth, flowHeight, drawWidth, drawHeight);
	
	canvasFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, 8);
	canvasFbo.clear();
    
  //obstacle
  fluidSimulation.addObstacle(canvasFbo.getTexture());
	
	// 3D MESH AND VIDEO STUFF
  the_car.loadModel("car_wireframe_2.obj");
  the_car_video.load("wireframe_car.mp4");
  the_car_video.setLoopState(OF_LOOP_NORMAL);
  the_car_video.play();
  
  // GUI
	setupGui();
  
  //LED Frame
  ledFrame.setup();
  
  //TUIO
  tuioClient.start(tuioPort);
  ofAddListener(tuioClient.cursorAdded, this, &opticalGFlowApp::tuioAdded);
  ofAddListener(tuioClient.cursorRemoved, this, &opticalGFlowApp::tuioRemoved);
  ofAddListener(tuioClient.cursorUpdated, this, &opticalGFlowApp::tuioUpdated);

  lastTime = ofGetElapsedTimef();

  use_mesh = false;
  }
void opticalGFlowApp::exit() {
  ledFrame.disconnect();
}
//--------------------------------------------------------------
void opticalGFlowApp::setupGui() {
	
	gui.setup("settings");
	gui.setDefaultBackgroundColor(ofColor(0, 0, 0, 127));
	gui.setDefaultFillColor(ofColor(160, 160, 160, 160));
	gui.add(guiFPS.set("average FPS", 0, 0, 60));
	gui.add(guiMinFPS.set("minimum FPS", 0, 0, 60));
	gui.add(doFullScreen.set("fullscreen (F)", true));
	doFullScreen.addListener(this, &opticalGFlowApp::setFullScreen);
	gui.add(toggleGuiDraw.set("show gui (G)", false));
  gui.add(use_mouse.set("use mouse (I)", false));
  gui.add(use_tuio.set("use tuio (T)", true));
	gui.add(doDrawWireframe.set("draw wireframe (C)", true));
  gui.add(use_mesh.set("use mesh (M)", false));
  gui.add(lineThickness.set("line thickness", 5.0f, 1.0f, 10.0f));
  gui.add(tuioPort.set("TUIO Port", 3333));
  gui.add(fluidColor.set("Fluid Color", ofColor(100, 100, 255, 255), ofColor(0, 0, 0, 255), ofColor(255, 255, 255, 255)));
		
	int guiColorSwitch = 0;
	ofColor guiHeaderColor[2];
	guiHeaderColor[0].set(160, 160, 80, 200);
	guiHeaderColor[1].set(80, 160, 160, 200);
	ofColor guiFillColor[2];
	guiFillColor[0].set(160, 160, 80, 200);
	guiFillColor[1].set(80, 160, 160, 200);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(opticalFlow.parameters);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(velocityMask.parameters);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(fluidSimulation.parameters);

	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(particleFlow.parameters);
  
  gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;

	// if the settings file is not present the parameters will not be set during this setup
	if (!ofFile("settings.xml"))
		gui.saveToFile("settings.xml");
	
	gui.loadFromFile("settings.xml");
	
	gui.minimizeAll();
	toggleGuiDraw = true;
}

//--------------------------------------------------------------
void opticalGFlowApp::update(){
  tuioClient.getMessage();
	deltaTime = ofGetElapsedTimef() - lastTime;
	lastTime = ofGetElapsedTimef();
  the_car_video.update();
  
  
  // DMX UPDATE
  if (ledFrame.getEnabled()) {
    ledFrame.updateLevel();
    ledFrame.setColor(fluidColor);
    ledFrame.update();
  }

  ofPushStyle();
  ofEnableBlendMode(OF_BLENDMODE_DISABLED);
  canvasFbo.begin();
  ofClear(0);
  
  if (use_mesh) {
    
    //draw wireframe cube
    ofPushMatrix();
    
    ofTranslate(canvasFbo.getWidth()/2, canvasFbo.getHeight()/2);
    float scale = 80.0f;
    ofScale(scale, scale, scale);
    ofRotate(-15.0f, 1.0f, 0.0f, 0.0f);
    ofRotate(180.0f, 0.0f, 0.0f, 1.0f);
    ofRotate(ofGetElapsedTimef()*10.0f, 0.0f, 1.0f, 0.0f);
    
    ofSetColor(255);
    //there seems to be a bug in the assimpModelLoader which does not allow to draw the wireframe via the drawWirefraem method
    ofSetLineWidth(lineThickness);
    the_car.getMesh(0).drawWireframe();
    the_car.getMesh(1).drawWireframe();
      
    ofPopMatrix();
  } else {
    float video_aspect = the_car_video.getWidth()/the_car_video.getHeight();
    the_car_video.draw(0,0, canvasFbo.getWidth(), canvasFbo.getWidth()/video_aspect);
  }
  
  canvasFbo.end();
  ofPopStyle();
  
  fluidSimulation.reset_obstacle();
  fluidSimulation.addObstacle(canvasFbo.getTexture());
  
  opticalFlow.setSource(canvasFbo.getTexture());
  opticalFlow.update(deltaTime);
  
  velocityMask.setDensity(canvasFbo.getTexture());
  velocityMask.setVelocity(opticalFlow.getOpticalFlow());
  velocityMask.update();
	
	fluidSimulation.addVelocity(opticalFlow.getOpticalFlowDecay());
	fluidSimulation.addDensity(velocityMask.getColorMask());
	fluidSimulation.addTemperature(velocityMask.getLuminanceMask());
	
  
  
	
  if (use_mouse) {
    mouseForces.update(deltaTime);
    for (int i=0; i<mouseForces.getNumForces(); i++) {
      if (mouseForces.didChange(i)) {
        switch (mouseForces.getType(i)) {
          case FT_DENSITY:
            fluidSimulation.addDensity(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
            break;
          case FT_VELOCITY:
            fluidSimulation.addVelocity(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
            particleFlow.addFlowVelocity(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
            break;
          case FT_TEMPERATURE:
            fluidSimulation.addTemperature(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
            break;
          case FT_PRESSURE:
            fluidSimulation.addPressure(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
            break;
          case FT_OBSTACLE:
            fluidSimulation.addTempObstacle(mouseForces.getTextureReference(i));
          default:
            break;
        }
      }
    }
  }
  
  if (use_tuio) {
    tuioForces.update(deltaTime);
    for (int i=0; i<tuioForces.getNumForces(); i++) {
      if (tuioForces.didChange(i)) {
        switch (tuioForces.getType(i)) {
          case FT_DENSITY:
            fluidSimulation.addDensity(tuioForces.getTextureReference(i), tuioForces.getStrength(i));
            break;
          case FT_VELOCITY:
            fluidSimulation.addVelocity(tuioForces.getTextureReference(i), tuioForces.getStrength(i));
            particleFlow.addFlowVelocity(tuioForces.getTextureReference(i), tuioForces.getStrength(i));
            break;
          case FT_TEMPERATURE:
            fluidSimulation.addTemperature(tuioForces.getTextureReference(i), tuioForces.getStrength(i));
            break;
          case FT_PRESSURE:
            fluidSimulation.addPressure(tuioForces.getTextureReference(i), tuioForces.getStrength(i));
            break;
          case FT_OBSTACLE:
            fluidSimulation.addTempObstacle(tuioForces.getTextureReference(i));
          default:
            break;
        }
      }
    }
  }

	fluidSimulation.update();
	
	if (particleFlow.isActive()) {
		particleFlow.setSpeed(fluidSimulation.getSpeed());
		particleFlow.setCellSize(fluidSimulation.getCellSize());
		particleFlow.addFlowVelocity(opticalFlow.getOpticalFlow());
		particleFlow.addFluidVelocity(fluidSimulation.getVelocity());
//		particleFlow.addDensity(fluidSimulation.getDensity());
		particleFlow.setObstacle(fluidSimulation.getObstacle());
	}
	particleFlow.update();
	
}

//--------------------------------------------------------------
void opticalGFlowApp::keyPressed(int key){
	switch (key) {
		case 'G':
		case 'g':
      toggleGuiDraw = !toggleGuiDraw;
      break;
		case 'f':
		case 'F':
      doFullScreen.set(!doFullScreen.get());
      break;
		case 'c':
		case 'C':
      doDrawWireframe.set(!doDrawWireframe.get());
      break;
    case 'm':
    case 'M':
      use_mesh.set(!use_mesh.get());
      break;
    case 'r':
		case 'R':
			fluidSimulation.reset();
			tuioForces.reset();
			break;
    case 't':
    case 'T':
      use_tuio.set(!use_tuio.get());
      break;
    case 'i':
    case 'I':
      use_mouse.set(!use_mouse.get());
      break;
		default:
      break;
	}
}

//--------------------------------------------------------------
void opticalGFlowApp::draw(){
	ofClear(0,0);
	
  if (doDrawWireframe.get()) {
		drawSource();
  }
  
  ofPushStyle();
  ofSetColor(fluidColor.get());
  drawComposite();
  ofPopStyle();
  
	if (toggleGuiDraw) {
    ofShowCursor();
    drawGui();
	} else {
		ofHideCursor();
  }
  
//  ofDrawCircle(tuioPoint, 10);
}

//--------------------------------------------------------------
void opticalGFlowApp::drawComposite(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	fluidSimulation.draw(_x, _y, _width, _height);
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	if (particleFlow.isActive())
		particleFlow.draw(_x, _y, _width, _height);
	
	ofPopStyle();
}

//--------------------------------------------------------------
void opticalGFlowApp::drawParticles(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	if (particleFlow.isActive())
		particleFlow.draw(_x, _y, _width, _height);
	ofPopStyle();
}

//--------------------------------------------------------------
void opticalGFlowApp::drawFluidObstacle(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_DISABLED);
	fluidSimulation.getObstacle().draw(_x, _y, _width, _height);
	ofPopStyle();
}

//--------------------------------------------------------------
void opticalGFlowApp::drawSource(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_DISABLED);
	canvasFbo.draw(_x, _y, _width, _height);
	ofPopStyle();
}

//--------------------------------------------------------------
void opticalGFlowApp::drawGui() {
	guiFPS = (int)(ofGetFrameRate() + 0.5);
	
	// calculate minimum fps
	deltaTimeDeque.push_back(deltaTime);
	
	while (deltaTimeDeque.size() > guiFPS.get())
		deltaTimeDeque.pop_front();
	
	float longestTime = 0;
	for (int i=0; i<deltaTimeDeque.size(); i++){
		if (deltaTimeDeque[i] > longestTime)
			longestTime = deltaTimeDeque[i];
	}
	
	guiMinFPS.set(1.0 / longestTime);
	
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	gui.draw();
	
	// HACK TO COMPENSATE FOR DISSAPEARING MOUSE
	ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
	ofDrawCircle(ofGetMouseX(), ofGetMouseY(), ofGetWindowWidth() / 300.0);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofDrawCircle(ofGetMouseX(), ofGetMouseY(), ofGetWindowWidth() / 600.0);
	ofPopStyle();
}

//--------------------------------------------------------------
void opticalGFlowApp::tuioAdded(ofxTuioCursor &cursor) {
  tuioForces.tuioUpdated(cursor.getX(), cursor.getY(), cursor.getFingerId());
  ledFrame.stopPulsing();
}

//--------------------------------------------------------------
void opticalGFlowApp::tuioRemoved(ofxTuioCursor &cursor) {}

//--------------------------------------------------------------
void opticalGFlowApp::tuioUpdated(ofxTuioCursor &cursor) {
//  tuioPoint.set(cursor.getX()*ofGetWidth(), cursor.getY()*ofGetHeight());
  tuioForces.tuioUpdated(cursor.getX(), cursor.getY(), cursor.getFingerId());
  ledFrame.stopPulsing();
}