#include "ofApp.h"
#include "PSceneSleep.h"
#include "PSceneAuth.h"
#include "PSceneDetect.h"
#include "PSceneAnalysis.h"
#include "PSceneResult.h"


//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
    //ofHideCursor();
	//ofSetFullscreen(true);


	loadScene();
	setupCamera();

	_stage_pre=PEMPTY;
	_stage=PSLEEP;
	_scene[_stage]->init();

	ofAddListener(SceneBase::sceneInFinish,this,&ofApp::onSceneInFinish);
	ofAddListener(SceneBase::sceneOutFinish,this,&ofApp::onSceneOutFinish);

	ofAddListener(_http_utils.newResponseEvent,this,&ofApp::urlResponse);
    _http_utils.setTimeoutSeconds(60);
    _http_utils.setMaxRetries(1);
	_http_utils.start();

	_millis_now=ofGetElapsedTimeMillis();

	ofEnableSmoothing();
}

//--------------------------------------------------------------
void ofApp::update(){
	ofSetBackgroundColor(0);
	
	int dt_=ofGetElapsedTimeMillis()-_millis_now;
	_millis_now+=dt_;

	if(!_camera_paused){
		_camera.update();
	    if(_stage==PSLEEP && _camera.isFrameNew()){
			_finder.update(_camera);
		}		
	}

	_scene[_stage]->update(dt_);
}

//--------------------------------------------------------------
void ofApp::draw(){

	_camera.draw(0,0);
	_img_mask.draw(0,0);

	ofPushMatrix();
	ofTranslate(420,0);
		_img_frame.draw(0,0);
		_scene[_stage]->draw();			
	ofPopMatrix();

#ifdef DRAW_DEBUG
	ofPushStyle();
	ofSetColor(255,0,0);
	ofDrawBitmapString("fps= "+ofToString(ofGetFrameRate()),ofGetWidth()-100,10);
	ofPopStyle();
#endif

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case 'f':
		case 'F':
			ofToggleFullscreen();
			break;
		case 'q':
		case 'Q':
			prepareScene(PSLEEP);
			break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}



void ofApp::loadScene(){
	
	_scene[0]=new PSceneSleep(this);
	_scene[1]=new PSceneAuth(this);
	_scene[2]=new PSceneDetect(this);
	_scene[3]=new PSceneAnalysis(this);
	_scene[4]=new PSceneResult(this);


	_img_frame.loadImage("ui_img/frame_sleep.png");
	_img_mask.loadImage("ui_img/mask-08.png");
}

void ofApp::onSceneInFinish(int &e){
	ofLog()<<"Scene "<<e<<" In finish!";
}

void ofApp::onSceneOutFinish(int &e){
	ofLog()<<"Scene "<<e<<" Out finish!";
	setStage(_stage_next);	
}

void ofApp::prepareScene(PStage set_){
	
	if(set_==_stage_next) return;

	ofLog()<<"Preparing for stage "<<set_;
	_stage_next=set_;
	if(_stage!=PEMPTY) _scene[_stage]->end();

}

void ofApp::setStage(PStage set_){

	_scene[set_]->init();

	_stage=set_;
}


void ofApp::setupCamera(){
	_camera.setup(CAM_WIDTH,CAM_HEIGHT);
	_finder.setup("haarcascade_frontalface_default.xml");
	_finder.setPreset(ObjectFinder::Fast);

	_camera_paused=false;
}

void ofApp::setCameraPause(bool set_){

}

void ofApp::drawFaceFrame(){

}
bool ofApp::faceFound(){
    return _finder.size()>0;
}

void ofApp::sendFaceRequest(){


}
void ofApp::urlResponse(ofxHttpResponse &resp){

}


void ofApp::drawOutFrame(){
	
	_img_mask.draw(0,0);
	_img_frame.draw(0,0);
}




