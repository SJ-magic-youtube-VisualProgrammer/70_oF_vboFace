/************************************************************
************************************************************/
#include "ofApp.h"

/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp(int _Cam_id, int _CamWidth, int _CamHeight, int _CamFps, int _AppFps)
: Cam_id(_Cam_id)
, CamWidth(_CamWidth)
, CamHeight(_CamHeight)
, CamFps(_CamFps)
, AppFps(_AppFps){
}

/******************************
******************************/
ofApp::~ofApp(){
	delete Gui_Global;
	
	if(fp_Log)	fclose(fp_Log);
}


/******************************
******************************/
void ofApp::setup(){
	/********************
	********************/
	ofSetWindowTitle("vbo_Face");
	
	ofSetWindowShape( WINDOW_WIDTH, WINDOW_HEIGHT );
	ofSetVerticalSync(false);	// trueとどっちがいいんだろう？
	ofSetFrameRate(AppFps);
	
	ofSetEscapeQuitsApp(false);
	
	/********************
	********************/
	fp_Log = fopen("../../../data/Log.csv", "w");
	setup_Gui();
	
	/********************
	********************/
	setup_camera();
	
	EasyCam.setDistance(400);
	NUM_PARTICLES = int( webCam.getWidth() * webCam.getHeight() ); // size of webCam is fixed now.
	VboSet.setup( NUM_PARTICLES );
	Refresh_vboVerts();
	Refresh_vboColor();
	VboSet.update_vertex_color();
	
	img.load("img.png");
	
	fbo_WebCam.allocate(int(webCam.getWidth()), int(webCam.getHeight()), GL_RGBA);
	ClearFbo(fbo_WebCam);
	
	shader_Gray.load( "shader_sj/Gray.vert", "shader_sj/Gray.frag" );
}

/******************************
******************************/
void ofApp::ClearFbo(ofFbo& fbo)
{
	fbo.begin();
		ofClear(0, 0, 0, 0);
	fbo.end();
}

/******************************
description
	memoryを確保は、app start後にしないと、
	segmentation faultになってしまった。
******************************/
void ofApp::setup_Gui()
{
	/********************
	********************/
	Gui_Global = new GUI_GLOBAL;
	Gui_Global->setup("param", "gui.xml", 10, 10);
}

/******************************
******************************/
void ofApp::setup_camera()
{
	/********************
	********************/
	printf("> setup camera\n");
	fflush(stdout);
	
	/********************
	細かく情報出してもらう
	********************/
	ofSetLogLevel(OF_LOG_VERBOSE);
    webCam.setVerbose(true);
	
	vector< ofVideoDevice > Devices = webCam.listDevices();// 上 2行がないと、List表示されない.
	
	/********************
	********************/
	if(Cam_id == -1){
		ofExit();
		return;
	}else{
		if(Devices.size() <= Cam_id) { ERROR_MSG(); ofExit(); return; }
		
		webCam.setDeviceID(Cam_id);
		printf("\n> webCam set device id = %d\n", Cam_id);
		printf("> DeviceID     = %d\n", Devices[Cam_id].id);
		printf("> CamName      = %s\n", Devices[Cam_id].deviceName.c_str());
		printf("> HardwareName = %s\n", Devices[Cam_id].hardwareName.c_str());
		printf("> serialID     = %s\n", Devices[Cam_id].serialID.c_str());
		
		printf( "\n> Cam size asked  = (%d, %d)\n", int(CamWidth), int(CamHeight) );
		{
			bool b_CamSize_Changed = false;
			if(CAM_WIDTH_MAX < CamWidth)	{ CamWidth = CAM_WIDTH_MAX; b_CamSize_Changed = true; }
			if(CAM_HEIGHT_MAX < CamHeight)	{ CamHeight = CAM_HEIGHT_MAX; b_CamSize_Changed = true; }
			if(b_CamSize_Changed)			printf( "\n> Cam size changed  = (%d, %d)\n", int(CamWidth), int(CamHeight) );
		}
		/*
			https://openframeworks.cc/documentation/video/ofVideoGrabber/#show_setDesiredFrameRate
				Set's the desired frame rate of the grabber. This should be called before initGrabber(), which will try to initialize at the desired frame rate. 
				Not all frame rates will be supported, but this at least gives you some abilitity to try grab at different rates.
		*/
		webCam.setDesiredFrameRate(CamFps);
		webCam.setup(CamWidth, CamHeight, true); // bool ofVideoGrabber::initGrabber(int w, int h, bool setUseTexture){
		
		printf( "> Cam size actual = (%d, %d)\n\n", int(webCam.getWidth()), int(webCam.getHeight()) );
		fflush(stdout);
	}
}

/******************************
1--2
|  |
0--3
******************************/
void ofApp::Refresh_vboVerts()
{
	for(int i = 0; i < int(webCam.getWidth()); i++){
		for(int j = 0; j < int(webCam.getHeight()); j++){
			VboSet.set_vboVerts( j * int(webCam.getWidth()) + i, (i - int(webCam.getWidth())/2) * Gui_Global->ParticleSpace, (j - int(webCam.getHeight())/2) * Gui_Global->ParticleSpace, 0);
		}
	}
}

/******************************
******************************/
void ofApp::Refresh_vboColor()
{
	ofColor col(255, 255, 255, 255);
	VboSet.set_singleColor(col);
	
	return;
}

/******************************
******************************/
void ofApp::update(){
	webCam.update();
	if(webCam.isFrameNew()){
		/********************
		********************/
		fbo_WebCam.begin();
			if(Gui_Global->b_webCam_Gray) shader_Gray.begin();
				ofClear(0, 0, 0, 0);
				ofSetColor(255);
				webCam.draw(0, 0, fbo_WebCam.getWidth(), fbo_WebCam.getHeight());
			if(Gui_Global->b_webCam_Gray) shader_Gray.end();
		fbo_WebCam.end();
		
		/********************
		********************/
		// ofPixels& pixels = webCam.getPixels();
		ofPixels pixels;
		fbo_WebCam.readToPixels(pixels);
		
		for(int i = 0; i < int(webCam.getWidth()); i++){
			for(int j = 0; j < int(webCam.getHeight()); j++){
				// ofColor col = pixels.getColor((j * pixels.getWidth()) + i); // 何故か、こちらは動きがおかしい... : oFのbug??
				ofColor col = pixels.getColor(i, j);
				float r = (float)col.r / 255.0;
				float g = (float)col.g / 255.0;
				float b = (float)col.b / 255.0;
				
				float brightness = (r + g + b) / 3.0f;
				
				VboSet.set_vboVerts(j * int(webCam.getWidth()) + i, (i - int(webCam.getWidth())/2) * Gui_Global->ParticleSpace, (j - int(webCam.getHeight())/2) * Gui_Global->ParticleSpace, brightness * Gui_Global->ParticleHeight);
				VboSet.set_vboColor(j * int(webCam.getWidth()) + i, ofColor(col.r, col.g, col.b, Gui_Global->ParticleAlpha));
			}
		}
		
		VboSet.update_vertex_color();
	}
}

/******************************
******************************/
void ofApp::draw(){
	/********************
	********************/
	ofEnableAlphaBlending();
	// ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	// ofEnableSmoothing();
	
	/********************
	********************/
	ofBackground(0);
	
	/********************
	********************/
	ofDisableDepthTest();
	ofSetColor(Gui_Global->Alpba_img);
	img.draw(0, 0, ofGetWidth(), ofGetHeight());
	
	/********************
	********************/
	ofSetColor(Gui_Global->Alpba_webCam);
	ofPushMatrix();
		if(Gui_Global->b_Particle_Mirror)	{ ofTranslate(ofGetWidth(), 0, 0); ofScale(-1, 1, 1); }
		fbo_WebCam.draw(0, 0, ofGetWidth(), ofGetHeight());
	ofPopMatrix();
	
	/********************
	********************/
	ofEnableDepthTest();
	EasyCam.begin();
		ofPushMatrix();
			if(Gui_Global->b_Particle_Mirror)	ofScale(-1, -1, 1);
			else								ofScale(1, -1, 1);
			
			glPointSize(Gui_Global->ParticlePointSize);
			VboSet.draw(GL_POINTS);
		ofPopMatrix();
	EasyCam.end();
	
	/********************
	********************/
	ofDisableDepthTest(); // need this to draw gui.
	if(Gui_Global->b_Disp){
		Gui_Global->gui.draw();
	}
	
	ofSetColor(0, 100, 255, 255);
	string info;
	info += "Vertex num = " + ofToString(NUM_PARTICLES, 0) + "\n";
	info += "FPS = " + ofToString(ofGetFrameRate(), 2);
	ofDrawBitmapString(info, 30, 30);
}

/******************************
******************************/
void ofApp::keyPressed(int key){
	switch(key){
		case 'r':
			EasyCam.reset();
			EasyCam.setDistance(400);
			EasyCam.enableMouseInput();
			break;
			
		case 'd':
			Gui_Global->b_Disp = !Gui_Global->b_Disp;
			break;
	}

}

/******************************
******************************/
void ofApp::keyReleased(int key){

}

/******************************
******************************/
void ofApp::mouseMoved(int x, int y ){

}

/******************************
******************************/
void ofApp::mouseDragged(int x, int y, int button){

}

/******************************
******************************/
void ofApp::mousePressed(int x, int y, int button){

}

/******************************
******************************/
void ofApp::mouseReleased(int x, int y, int button){

}

/******************************
******************************/
void ofApp::mouseEntered(int x, int y){

}

/******************************
******************************/
void ofApp::mouseExited(int x, int y){

}

/******************************
******************************/
void ofApp::windowResized(int w, int h){

}

/******************************
******************************/
void ofApp::gotMessage(ofMessage msg){

}

/******************************
******************************/
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
