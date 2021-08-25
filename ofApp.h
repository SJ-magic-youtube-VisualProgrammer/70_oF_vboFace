/************************************************************
■Media Art II 2013 第7回 : openFrameworks 3Dグラフィクス、OpenGL
	https://www.slideshare.net/tado/media-art-ii-2013
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofxVboSet.h"

#include "sj_common.h"

/************************************************************
************************************************************/
class ofApp : public ofBaseApp{
private:
	/****************************************
	****************************************/
	enum{
		WINDOW_WIDTH	= 1280,
		WINDOW_HEIGHT 	= 720,
	};
	enum{
		CAM_WIDTH_MAX	= 640,
		CAM_HEIGHT_MAX 	= 480,
	};
	
	/****************************************
	****************************************/
	const int AppFps;
	
	ofEasyCam EasyCam;
	// ofLight light;
	
	ofx__VBO_SET VboSet;
	int NUM_PARTICLES;
	
	ofVideoGrabber webCam;
	const int Cam_id;
	int CamWidth;
	int CamHeight;
	const int CamFps;
	
	ofImage img;
	
	ofShader shader_Gray;
	ofFbo fbo_WebCam;
	
	/****************************************
	****************************************/
	void Refresh_vboVerts();
	void Refresh_vboColor();
	void setup_camera();
	void setup_Gui();
	void ClearFbo(ofFbo& fbo);
	
public:
	/****************************************
	****************************************/
	ofApp(int _Cam_id, int _CamWidth, int _CamHeight, int _CamFps, int _AppFps);
	~ofApp();

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
};
