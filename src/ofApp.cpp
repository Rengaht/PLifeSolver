#include "ofApp.h"
#include "PSceneSleep.h"
#include "PSceneAuth.h"
#include "PSceneDetect.h"
#include "PSceneAnalysis.h"
#include "PSceneResult.h"


ofPath PDrop::_shape_drop;
ofVec2f PDrop::_size;

ofColor PFruitRain::_color[]={ofColor(236,152,152),ofColor(238,216,152),ofColor(152,236,200),ofColor(152,177,236),
						ofColor(219,152,236),ofColor(238,216,152),ofColor(152,236,200),ofColor(152,177,236)};

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
    //ofHideCursor();
	//ofSetFullscreen(true);


	loadScene();
	setupCamera();
	setupSerial();

	_stage_pre=PEMPTY;
	//setStage(PSLEEP);
	_stage=PSLEEP;
	_scene[_stage]->init();
	//setStage(PDETECT);


	ofAddListener(SceneBase::sceneInFinish,this,&ofApp::onSceneInFinish);
	ofAddListener(SceneBase::sceneOutFinish,this,&ofApp::onSceneOutFinish);

	ofAddListener(_http_utils.newResponseEvent,this,&ofApp::urlResponse);
    _http_utils.setTimeoutSeconds(60);
    _http_utils.setMaxRetries(1);
	_http_utils.start();

	_millis_now=ofGetElapsedTimeMillis();


	_fbo_save.allocate(GIF_HEIGHT,GIF_HEIGHT);
	_recorder.setFormat("png");
	_recording=false;
	_timer_record=FrameTimer(1000.0/(float)GIF_FPS);
	ofAddListener(_timer_record.finish_event,this,&ofApp::onRecordTimerFinish);
	
	ofAddListener(_recorder.recordFinish,this,&ofApp::onRecorderFinish);
	_recorder.startThread();

	_fbo_contour.allocate(ofGetWidth(),ofGetHeight(),GL_LUMINANCE);

	

	ofEnableSmoothing();


}

//--------------------------------------------------------------
void ofApp::update(){
	ofSetBackgroundColor(0);
	
	int dt_=ofGetElapsedTimeMillis()-_millis_now;
	_millis_now+=dt_;

	if(!_camera_paused){
		_camera.update();
	    if(_camera.isFrameNew()){
			_finder.update(_camera);
			//_contour_finder.findContours(_camera);	
			//drawContour();
		}		
		if(_recording) _timer_record.update(dt_);
	}

	if(_face_analysis_ready){
		_face_analysis_ready=false;
	}

	_fruit_rain.update(dt_);

	_scene[_stage]->update(dt_);
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofPushMatrix();
	float cam_scale=(float)ofGetHeight()/_camera.getHeight();
	float cam_wid=_camera.getWidth()*cam_scale;
	ofTranslate(ofGetWidth()/2-cam_wid/2,0);
	ofScale(cam_scale,cam_scale);
		
		_camera.draw(_camera.getWidth(),0,-_camera.getWidth(),_camera.getHeight());
		if(!_camera_paused && _stage>PSLEEP) drawFaceFrame();
		
	ofPopMatrix();

	//_img_mask.draw(0,0);
		
	float scale_=ofGetHeight()/(float)WIN_HEIGHT;

	ofPushMatrix();
	
	//ofPushMatrix();
	//ofTranslate(ofGetWidth()/2,0); //center		
	//ofScale(scale_,scale_);		
	//	if(_stage==PDETECT || _stage==PANALYSIS) _fruit_rain.draw();				
	//ofPopMatrix();

	ofPushMatrix();
	ofTranslate(ofGetWidth()/2-ofGetHeight()/2,0); //left-top
	ofScale(scale_,scale_);		
		_scene[_stage]->draw();			
	ofPopMatrix();
	
	float w_=ofGetHeight()/(float)WIN_HEIGHT*WIN_WIDTH;
	_img_frame.draw(ofGetWidth()/2-w_/2,0,w_,ofGetHeight());

	float margin_=(ofGetWidth()-w_)/2;
	if(margin_>0){
		ofPushStyle();
		ofSetColor(13,104,171);
			ofDrawRectangle(0,0,margin_,ofGetHeight());
			ofDrawRectangle(ofGetWidth()-margin_,0,margin_,ofGetHeight());
		ofPopStyle();
	}
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

	string cmd;

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
			//sendFaceRequest();
			uploadImage("test");
			break;
		case 'g':
			//createFruitImage();
			break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}
void ofApp::exit(){
	_recorder.waitForThread();
}



void ofApp::loadScene(){
	
	_scene[0]=new PSceneSleep(this);
	_scene[1]=new PSceneAuth(this);
	_scene[2]=new PSceneDetect(this);
	_scene[3]=new PSceneAnalysis(this);
	_scene[4]=new PSceneResult(this);


	_img_frame.loadImage("_img_ui/mask.png");
	//_img_mask.loadImage("_img_ui/mask-08.png");
	_img_share_frame.loadImage("_img_ui/frame_share.png");

	for(int i=0;i<MJUICE_RESULT;++i)
		_img_share_text[i].load("_img_ui/share/"+ofToString(i+1)+".png");
	
	_qrcode_gen.setColor(ofColor(238,216,152),ofColor(0,0,0,0));

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

	switch(set_){
		case PANALYSIS:
			//saveCameraFrame();
			break;
	}

}

void ofApp::setStage(PStage set_){

	_scene[set_]->init();

	switch(set_){
		case PDETECT:
			createUserID();
			_fruit_rain.reset();
			break;
        case PANALYSIS:
			setCameraPause(true);
			//_fruit_rain.reset();
            _fruit_rain.setSlow(false);
			_fruit_rain.start();	
            break;
        case PRESULT:
			_fruit_rain.reset();
			_fruit_rain.setAutoFruit(false);
			_fruit_rain.setFruit(_idx_user_juice);            
			_fruit_rain.start();			
            break;
		case PSLEEP:
			_fruit_rain.stop();
			setCameraPause(false);
			break;
	}

	_stage=set_;
}


void ofApp::setupCamera(){
	_camera.setup(CAM_WIDTH,CAM_HEIGHT);	
	
	_finder.setup("haarcascade_frontalface_default.xml");
	_finder.setPreset(ObjectFinder::Fast);
	_finder.setFindBiggestObject(true);


	_contour_finder.setMinAreaRadius(WIN_HEIGHT*.3);
	_contour_finder.setMaxAreaRadius(WIN_HEIGHT*.6);
	_contour_finder.setThreshold(120);
	_contour_finder.setSimplify(true);
	_contour_finder.setFindHoles(false);

	_camera_paused=false;
}

void ofApp::setCameraPause(bool set_){
	
	_camera_paused=set_;
}
void ofApp::createUserID(){
	_user_id="mm"+ofGetTimestampString();

}

void ofApp::onRecordTimerFinish(int &e){
	if(_recording) saveCameraFrame();
	_timer_record.restart();
}

void ofApp::drawFaceFrame(){
	ofPushStyle();
    ofNoFill();
    
    int len=_finder.size();
    for(int i=0;i<len;++i){
        //ofDrawRectangle(_finder.getObject(i));

        auto rec_=_finder.getObject(i);
        auto p_=rec_.getPosition();
        ofSetColor(238,216,152);
        ofPushMatrix();	
        ofTranslate(_camera.getWidth()-p_.x,p_.y);
			ofDrawRectangle(0,0,-rec_.getWidth(),rec_.getHeight());
        ofPopMatrix();
    }
    ofPopStyle();
}
bool ofApp::faceFound(){
    return _finder.size()>0;
}
void ofApp::drawContour(){
	
	_fbo_contour.begin();
	ofClear(0);
	ofPushStyle();
	ofFill();
	//ofSetColor(255,0,0);

    int len=_finder.size();
    for(int i=0;i<len;++i){
        auto rec_=_finder.getObject(i);

		auto pp=_contour_finder.getPolylines();
		for(auto&p:pp){
			if(rec_.intersects(p.getBoundingBox())){
				
				ofPath p_;
				p_.newSubPath();
				p_.moveTo(p.getVertices()[0]);
				for(auto& t:p.getVertices()) p_.lineTo(t);
				p_.close();
				p_.simplify();

				p_.draw();
			}
		}
	
	}
	ofPopStyle();
	_fbo_contour.end();

	_camera.getTexture().setAlphaMask(_fbo_contour.getTexture());

	//_contour_finder.draw();
}


void ofApp::sendFaceRequest(){
	ofLog()<<">>>>>> Send Face Requeset...";
	_face_analysis_ready=false;

	string url_="https://naturalbenefits-lifesolver.cognitiveservices.azure.com/face/v1.0/detect?returnFaceAttributes=age,gender,emotion,smile&returnFaceLandmarks=true";	
	
	/*ofImage tmp_;
    tmp_.setFromPixels(_camera.getPixels().getData(),_camera.getWidth(),_camera.getHeight(),OF_IMAGE_COLOR);
    tmp_.mirror(false, true);
    tmp_.save("raw/"+_user_id+".jpg");*/
    ofBuffer data_=ofBufferFromFile("tmp/"+_user_id+"_0000.png",true);

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
			//prepareScene(PRESULT);
        }
             
	}else if(resp_.url.find("mmlab.com.tw")!=-1){
		ofxJSONElement json_;
        json_.parse(resp_.responseBody);
		//ofLog()<<resp_.responseBody;

        if(json_["result"]=="success"){
			createQRcode(json_["url"].asString());
			prepareScene(PRESULT);
		}else{
			//TODO: something wron!!!
			prepareScene(PSLEEP);
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

            _idx_user_juice=getJuiceFromEmotion(json_[i]["faceAttributes"]["emotion"]);
			
			if(_idx_user_juice<0){
				prepareScene(PSLEEP);
				return;
			}

			_recorder.setJuiceID(_idx_user_juice);
			_face_analysis_ready=true;
		}
        _user_data["face"]=json_;
	}

}
int ofApp::getJuiceFromEmotion(ofxJSONElement emotion_){

	// find highest score
	float val_=0;
	string mood_;
	int juice_=-1;
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

void ofApp::setupSerial(){
	_serial.listDevices();
	vector <ofSerialDeviceInfo> deviceList=_serial.getDeviceList();
	for(auto& t:deviceList){
		ofLog()<<t.getDeviceID()<<"  "<<t.getDeviceName();
	}
	_serial.setup(0, 9600);
}

bool ofApp::checkJuiceStorage(PJuice get_){
	//TODO!!!
	return true;
}
void ofApp::sendJuiceSignal(int send_){
	if(!_serial.available()) return;

	char sig_[8]={'a','a','a','a','a','a','a','a'};
	_serial.writeByte(sig_[send_]);	
}


void ofApp::drawOutFrame(){
	
	_img_mask.draw(0,0);
	_img_frame.draw(0,0);
}


void ofApp::setRecord(bool set_){
	_recording=set_;
	if(set_){
		_idx_record=0;
		_recorder.setPrefix(ofToDataPath("tmp/"+_user_id+"_"));
		_recorder.setUserID(_user_id);
		
		_timer_record.restart();

		//_recorder.startThread();
		ofSystem("DEL /F/Q "+ofToDataPath("tmp\\")+"*.png");

		ofLog()<<"start recording.... "<<ofGetElapsedTimeMillis();
	}else{
		_timer_record.stop();
		ofLog()<<"end recording..."<<ofGetElapsedTimeMillis()<<" #fr= "<<_idx_record;
	}
}
void ofApp::saveCameraFrame(){
	float scale_=(float)GIF_HEIGHT/(float)CAM_HEIGHT;
	float w_=(float)CAM_WIDTH*scale_;

	_fbo_save.begin();
	ofClear(255);
		_camera.draw(GIF_HEIGHT/2-w_/2,0,w_,GIF_HEIGHT);		
	_fbo_save.end();
	

	ofPixels pix;	
    _fbo_save.readToPixels(pix);	
	pix.mirror(false,true);

	_recorder.addFrame(pix);
	//ofSaveImage(pix,"raw/"+_user_id+".png");

}
void ofApp::createFruitImage(){

	ofLog()<<"Create Fruit Image !!!";
	
	for(int x=0;x<MJUICE_RESULT;++x){
			
		_fruit_rain.reset();
		_fruit_rain.setAutoFruit(false);	
		
		_fruit_rain.setFruit(x);
		_fruit_rain.start();
		for(int i=0;i<180;++i)
			_fruit_rain.update(16);

		for(int i=0;i<GIF_LENGTH*GIF_FPS;++i){
			_fbo_save.begin();
			ofClear(255);
				ofPushMatrix();
				ofTranslate((float)GIF_HEIGHT/2,0);
				ofScale((float)GIF_HEIGHT/WIN_HEIGHT,(float)GIF_HEIGHT/WIN_HEIGHT);
					_fruit_rain.draw(1);
				ofPopMatrix();
				_img_share_frame.draw(0,0);
				_img_share_text[x].draw(0,0,_fbo_save.getWidth(),_fbo_save.getHeight());
			_fbo_save.end();

			ofPixels pix;
			_fbo_save.readToPixels(pix);
			ofSaveImage(pix,"juice/"+ofToString(x+1)+"/"+ofToString(i,4,'0')+".png");

			for(int i=0;i<12;++i)
				_fruit_rain.update(16);
			//_fruit_rain.update(1000/(float)GIF_FPS*20);
		}
	}
}

void ofApp::onRecorderFinish(int &e){
	ofLog()<<"Recorder finish !!!";
	
	uploadImage(_user_id);
		
}

void ofApp::uploadImage(string id_){
	
	ofLog()<<"Upload Image !!!";

	string filename_="output/"+id_+".gif";
    
    ofxHttpForm form_;
    form_.action="https://mmlab.com.tw/project/naturalbenefits/action.php";
    form_.method=OFX_HTTP_POST;
    form_.name="Face Upload "+ofToString(ofGetElapsedTimeMillis());
    form_.addFile("file",ofToDataPath(filename_));
    form_.addFormField("guid", _user_id);
    form_.addFormField("action","upload");
    
    _http_utils.addForm(form_);
}

void ofApp::createQRcode(string url_){

	_qrcode_gen.generate(url_);

}