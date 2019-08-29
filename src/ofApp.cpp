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
		case 't':
			sendFaceRequest();
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

	switch(set_){
		case PDETECT:
			createUserID();
			break;
	}

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
void ofApp::createUserID(){
	_user_id="mm"+ofGetTimestampString();

}

void ofApp::drawFaceFrame(){
	ofPushStyle();
    ofSetColor(255,0,0);
    ofNoFill();
    
    int len=_finder.size();
    for(int i=0;i<len;++i){
        ofDrawRectangle(_finder.getObject(i));

        auto rec_=_finder.getObject(i);
        
        ofSetColor(255);
        ofPushMatrix();
        ofTranslate(rec_.getPosition());
        _img_frame.draw(0,0,rec_.getWidth(),rec_.getHeight());
        ofPopMatrix();
    }
    ofPopStyle();
}
bool ofApp::faceFound(){
    return _finder.size()>0;
}

void ofApp::sendFaceRequest(){
	ofLog()<<">>>>>> Send Face Requeset...";

	string url_="https://naturalbenefits-lifesolver.cognitiveservices.azure.com/face/v1.0/detect?returnFaceAttributes=age,gender,emotion,smile&returnFaceLandmarks=true";	
	
	ofImage tmp_;
    tmp_.setFromPixels(_camera.getPixels().getData(),_camera.getWidth(),_camera.getHeight(),OF_IMAGE_COLOR);
    tmp_.mirror(false, true);
    tmp_.save("raw/"+_user_id+".jpg");
    ofBuffer data_=ofBufferFromFile("raw/"+_user_id+".jpg",true);

	ofxHttpForm form_;
    form_.action=url_;
    form_.method=OFX_HTTP_POST;
    form_.name="Face Request "+ofToString(ofGetElapsedTimeMillis());
    form_.addHeaderField("Ocp-Apim-Subscription-Key", "2ff65a4e2b5e4ca989c288f502738d63");
    form_.contentType="application/octet-stream";
    form_.data=data_;
    
    _http_utils.addForm(form_);

	//_http_utils.postData(url_,data_,"application/octet-stream");

}
void ofApp::urlResponse(ofxHttpResponse &resp_){
	if(resp_.status != 200){
        ofLog()<<"Requeset error: "<<resp_.reasonForStatus;
        //prepareStatus(PSLEEP);
        return;
    }

	if(resp_.url.find("azure.com")!=-1){
        //ofLog()<<"receive: "<<resp_.responseBody;
        parseFaceData(resp_.responseBody);
        
        if(_stage==PANALYSIS){
            int event=_stage;
			ofNotifyEvent(_event_recieve_emotion,event);
			prepareScene(PRESULT);
        }
       
             
	}
}
void ofApp::parseFaceData(string data_){
	//ofLog()<<"get face data: "<<data_;
    
	ofxJSONElement json_;
	if(json_.parse(data_)){
		int len=json_.size();
		for(int i=0;i<1;++i){
			ofRectangle rect_(json_[i]["faceRectangle"]["left"].asInt(),
								json_[i]["faceRectangle"]["top"].asInt(),
								json_[i]["faceRectangle"]["width"].asInt(),
								json_[i]["faceRectangle"]["height"].asInt());
			_rect_face.push_back(rect_);

            int idx_=getJuiceFromEmotion(json_[i]["faceAttributes"]["emotion"]);

		}
        _user_data["face"]=json_;
	}

}
int ofApp::getJuiceFromEmotion(ofxJSONElement emotion_){

	// find highest score
	float val_=0;
	string mood_;
	int juice_;
	auto name_=emotion_.getMemberNames();
	
	for(auto& n:name_){
		int t=getJuice(n);
		if(emotion_[n].asFloat()>val_ && checkJuiceStorage((PJuice)t)){
			mood_=n;
			juice_=t;
			val_=emotion_[n].asFloat();
		}
	}
	
	ofLog()<<mood_<<" ---> "<<juice_;
	return juice_;
}
ofApp::PJuice ofApp::getJuice(string mood_){

	if(mood_=="disgust") return RED_DRAGON;
	if(mood_=="neutral") return HONEY_LEMON;
	if(mood_=="sadness") return VEGETABLE;
	if(mood_=="fear") return BEETROOT;
	if(mood_=="anger") return COCONUT;
	if(mood_=="contempt") return PINEAPPLE;
	if(mood_=="happiness") return ORANGE_PASSION;
	
}
bool ofApp::checkJuiceStorage(PJuice get_){
	//TODO!!!
	return true;
}

void ofApp::drawOutFrame(){
	
	_img_mask.draw(0,0);
	_img_frame.draw(0,0);
}




