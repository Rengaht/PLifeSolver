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
	setStage(_stage);


	ofAddListener(SceneBase::sceneInFinish,this,&ofApp::onSceneInFinish);
	ofAddListener(SceneBase::sceneOutFinish,this,&ofApp::onSceneOutFinish);

	ofAddListener(_http_utils.newResponseEvent,this,&ofApp::urlResponse);
    _http_utils.setTimeoutSeconds(60);
    _http_utils.setMaxRetries(1);
	_http_utils.start();

	_millis_now=ofGetElapsedTimeMillis();


	_fbo_save.allocate(PParam::val()->GifSize,PParam::val()->GifSize);
	_recorder.setFormat("png");
	_recording=false;
	_timer_record=FrameTimer(1000.0/PParam::val()->GifFps);
	ofAddListener(_timer_record.finish_event,this,&ofApp::onRecordTimerFinish);
	
	ofAddListener(_recorder.recordFinish,this,&ofApp::onRecorderFinish);
	_recorder.startThread();

	_fbo_contour.allocate(ofGetWidth(),ofGetHeight(),GL_RGB);
		

	_fbo_bgd_tmp.allocate(_camera.getWidth()*PParam::val()->BgdDetectScale,_camera.getHeight()*PParam::val()->BgdDetectScale,GL_RGB);
	_fbo_threshold_tmp.allocate(_camera.getWidth(),_camera.getHeight(),GL_RGB);
	_fbo_bgd.allocate(_camera.getWidth(),_camera.getHeight(),GL_RGB);

	_masker.setup(_camera.getWidth(),_camera.getHeight());
	_masker.newLayer();

	ofEnableSmoothing();


}

//--------------------------------------------------------------
void ofApp::update(){
	ofSetBackgroundColor(255);
	
	int dt_=ofGetElapsedTimeMillis()-_millis_now;
	_millis_now+=dt_;

	if(!_camera_paused){
		_camera.update();
	    if(_camera.isFrameNew()){
			_finder.update(_camera);
			//_contour_finder.findContours(_camera);	
			//drawContour();
#ifdef USE_BACKGROUND_SUB
			updateBackground();
#endif
			

		}		
		if(_recording) _timer_record.update(dt_);
	}

	if(_face_analysis_ready){
		_face_analysis_ready=false;
	}

	_fruit_rain.update(dt_);

	_scene[_stage]->update(dt_);

	parseChannelStatus();
	updateSound(dt_);

}

//--------------------------------------------------------------
void ofApp::draw(){

	ofPushMatrix();
	float cam_scale=(float)ofGetHeight()/_camera.getHeight();
	float cam_wid=_camera.getWidth()*cam_scale;
	ofTranslate(ofGetWidth()/2-cam_wid/2,0);
	ofScale(cam_scale,cam_scale);			
	

	if(_camera.isInitialized()) 
		_camera.draw(_camera.getWidth(),0,-_camera.getWidth(),_camera.getHeight());

	if(!_camera_paused && _stage>PSLEEP) drawFaceFrame();
	ofPopMatrix();

		
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
	
	if(!_camera_paused && _stage>PSLEEP){
		ofPushMatrix();
		ofTranslate(ofGetWidth()/2-cam_wid/2,0);
		ofScale(cam_scale,cam_scale);				
			 drawFaceFrame();
		ofPopMatrix();
	}


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

	ofDrawBitmapString("learning_time= "+ofToString(PParam::val()->BgdLearningTime),ofGetWidth()-200,20);
	ofDrawBitmapString("threhold= "+ofToString(PParam::val()->BgdThreshold),ofGetWidth()-200,30);
	

	/*ofNoFill();
	float s_=1.0/PParam::val()->BgdDetectScale*cam_scale;
	ofPushMatrix();
	ofTranslate(ofGetWidth()/2-cam_wid/2,0);
	ofScale(s_,s_);
	for(auto& r_:_rect_contour){
		ofDrawRectangle(r_.getLeft(),r_.getTop(),r_.getWidth(),r_.getHeight());
	}
	ofPopMatrix();*/

	ofPopStyle();

#ifdef USE_BACKGROUND_SUB
	float tw_=_camera.getWidth()*PParam::val()->BgdDetectScale;
	float th_=_camera.getHeight()*PParam::val()->BgdDetectScale;
	_fbo_bgd_tmp.draw(0,0,tw_,th_);
	if(_img_threshold.isAllocated()) _img_threshold.draw(0,th_,tw_,th_);
	_fbo_threshold_tmp.draw(0,th_*2,tw_,th_);
#endif

#endif

}
void ofApp::updateBackground(){
	_fbo_bgd_tmp.begin();
	ofClear(0);
		if(_camera.isInitialized()) 
			_camera.draw(0,0,_fbo_bgd_tmp.getWidth(),_fbo_bgd_tmp.getHeight());
	_fbo_bgd_tmp.end();
			
	ofPixels pix;	
	_fbo_bgd_tmp.readToPixels(pix);

	_background.setLearningTime(PParam::val()->BgdLearningTime);
	_background.setThresholdValue(PParam::val()->BgdThreshold);
	_background.update(pix,_img_threshold);
	_img_threshold.update();

	_contour_finder.findContours(_img_threshold);
	_rect_contour.clear();


	_fbo_threshold_tmp.begin();
	ofClear(0);
//		_img_threshold.draw(0,0,_fbo_threshold_tmp.getWidth(),_fbo_threshold_tmp.getHeight());
	ofPushMatrix();
	ofScale(1.0/PParam::val()->BgdDetectScale,1.0/PParam::val()->BgdDetectScale);
		ofPushStyle();
		ofFill();
		//ofSetColor(255,0,0);


		auto pp=_contour_finder.getPolylines();
		for(auto&p:pp){
		//	if(rec_.intersects(p.getBoundingBox())){
				auto r_=p.getBoundingBox();
				r_.x=_img_threshold.getWidth()-r_.x;
				_rect_contour.push_back(r_);

				ofPath p_;
				p_.newSubPath();
				p_.moveTo(p.getVertices()[0]);
				for(auto& t:p.getVertices()) p_.lineTo(t);
				p_.close();				
				p_.simplify(2.0);				
				p_.draw();

				/*ofPushStyle();
				ofSetColor(255,0,0);
				ofNoFill();
					ofDrawRectangle(p.getBoundingBox());
				ofPopStyle();*/
		//	}
		}
	
		
		ofPopStyle();
	ofPopMatrix();
	_fbo_threshold_tmp.end();		

	_fruit_rain._rect_contour=&_rect_contour;


	_fbo_bgd.begin();
	ofClear(0);
		_camera.draw(0,0);
	_fbo_bgd.end();
}
void ofApp::drawForeground(){
	

	_masker.beginMask();
		ofClear(0);
		_fbo_threshold_tmp.draw(_camera.getWidth(),0,-_camera.getWidth(),_camera.getHeight());
	_masker.endMask();

	_masker.beginLayer();
		ofClear(0);
		_camera.draw(_camera.getWidth(),0,-_camera.getWidth(),_camera.getHeight());
	_masker.endLayer();

	float scale_=ofGetHeight()/(float)WIN_HEIGHT;
	ofPushMatrix();	
	ofScale(1.0/scale_,1.0/scale_);
	ofTranslate(-ofGetWidth()/2+ofGetHeight()/2,0);

	float cam_scale=(float)ofGetHeight()/_camera.getHeight();
	float cam_wid=_camera.getWidth()*cam_scale;
	ofTranslate(ofGetWidth()/2-cam_wid/2,0);
	ofScale(cam_scale,cam_scale);	

		_masker.draw();
	ofPopMatrix();

	//if(_img_threshold.isAllocated()){
	//	//_img_threshold.draw(_camera.getWidth(),0,-_camera.getWidth(),_camera.getHeight());
	//	_fbo_threshold_tmp.getTexture().setSwizzle(GL_TEXTURE_SWIZZLE_A,GL_RED);
	//	_fbo_bgd.getTexture().setAlphaMask(_fbo_threshold_tmp.getTexture());
	//}
	//_fbo_bgd.draw(_camera.getWidth(),0,-_camera.getWidth(),_camera.getHeight());
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
			createFruitImage();
			break;
		case 'b':
		case 'B':
#ifdef USE_BACKGROUND_SUB
			_background.reset();
#endif
			break;
		case '-':
			PParam::val()->BgdLearningTime=min(PParam::val()->BgdLearningTime+1,15.0f);
			break;
		case '=':
			PParam::val()->BgdLearningTime=max(PParam::val()->BgdLearningTime-1,0.0f);
			break;
		case '[':
			PParam::val()->BgdThreshold=min(PParam::val()->BgdThreshold+1,255.0f);
			break;
		case ']':
			PParam::val()->BgdThreshold=max(PParam::val()->BgdThreshold-1,0.0f);
			break;
		/*default:
			_idx_channel=floor(ofRandom(10));
			sendJuiceSignal();
			break;*/
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

	for(int i=0;i<FRUIT_GROUP;++i)
		_img_share_text[i].load("_img_ui/share/"+ofToString(i+1)+".png");
	
	_qrcode_gen.setColor(ofColor(238,216,152),ofColor(0,0,0,0));

	_soundfx[0].loadSound("sound/check.wav");
	_soundfx[1].loadSound("sound/count.wav");
	_soundfx[2].loadSound("sound/finish.wav");
	_soundfx[3].loadSound("sound/result.wav");
	_soundfx[4].loadSound("sound/shutter.wav");

	_sound_bgm.loadSound("sound/Summer_Smile.mp3");
	_sound_bgm.setLoop(true);
	_sound_bgm.setVolume(.4);

	_timer_bgm_in=FrameTimer(500);	
	_timer_bgm_out=FrameTimer(2000);	

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
			stopBgm();
			break;
	}

	_stage=set_;
}


void ofApp::setupCamera(){
	_camera.setup(CAM_WIDTH,CAM_HEIGHT);	
	
	_finder.setup("haarcascade_frontalface_default.xml");
	_finder.setPreset(ObjectFinder::Fast);
	_finder.setFindBiggestObject(true);


	_contour_finder.setMinAreaRadius(_camera.getWidth()*PParam::val()->BgdDetectScale*.05);
	_contour_finder.setMaxAreaRadius(_camera.getWidth()*PParam::val()->BgdDetectScale*.7);
	_contour_finder.setThreshold(180);
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

			_recorder.createGif(_idx_user_juice);
			_face_analysis_ready=true;
				
			 uploadImage(_user_id);
		}
        _user_data["face"]=json_;
	}

}
int ofApp::getJuiceFromEmotion(ofxJSONElement emotion_){

	ofLog()<<emotion_.toStyledString();

	// find highest score
	float val_=0;
	string mood_;
	int juice_=-1;
	auto name_=emotion_.getMemberNames();
	
	for(auto& n:name_){
		int t=getJuice(n);
		if(emotion_[n].asFloat()>=val_){
            int cc=checkJuiceStorage((PParam::PJuice)t);
            if(cc>-1){
                mood_=n;
                juice_=t;
                val_=emotion_[n].asFloat();

                _idx_channel=cc;
            }
		}
	}
	
	ofLog()<<mood_<<" ---> "<<juice_<<"  channel= "<<_idx_channel;
	return juice_;
}
PParam::PJuice ofApp::getJuice(string mood_){

	if(mood_=="disgust") return PParam::RED_DRAGON;
	if(mood_=="neutral") return PParam::HONEY_LEMON;
	if(mood_=="sadness") return PParam::VEGETABLE;
	if(mood_=="fear") return PParam::BEETROOT;
	if(mood_=="anger") return PParam::COCONUT;
	if(mood_=="contempt") return PParam::PINEAPPLE;
	if(mood_=="happiness") return PParam::ORANGE_PASSION;
	if(mood_=="surprise") return PParam::CARROT;
	else return PParam::EMPTY;
}

void ofApp::setupSerial(){
	_serial.listDevices();
	vector <ofSerialDeviceInfo> deviceList=_serial.getDeviceList();
	for(auto& t:deviceList){
		ofLog()<<t.getDeviceID()<<"  "<<t.getDeviceName();
	}
	_serial.setup(PParam::val()->SerialPort, 9600);

	for(int i=0;i<PParam::val()->ChannelCmd.size()+1;++i) _status_channel.push_back(1);
	_parse_index=0;
}
void ofApp::parseChannelStatus(){

	if(!_serial.isInitialized()) return;

	while(_serial.available()){
		char byte_=_serial.readByte();
		//ofLog()<<byte_;
		if(byte_=='a'){
			//ofLog()<<"--------- parse start! ---------";
			_parse_index=0;
		}else if(byte_=='1'){
			
			if(_parse_index<0 || _parse_index>=PParam::val()->ChannelCmd.size()) continue;
			
			_status_channel[_parse_index+1]=1;
			_parse_index++;
			//ofLog()<<"--------- channel= "<<_parse_index<<" status= 1 ---------";
		}else if(byte_=='0'){

			if(_parse_index<0 || _parse_index>=PParam::val()->ChannelCmd.size()) continue;

			_status_channel[_parse_index+1]=0;
			_parse_index++;
			//ofLog()<<"--------- channel= "<<_parse_index<<" status= 0 ---------";
		}
		
	}

}
int ofApp::checkJuiceStorage(PParam::PJuice get_){

#ifdef WEEKAND_OFF
	int day_=ofGetWeekday();
	if(day_==0 || day_==6) return 0;
#endif

	if(get_==PParam::EMPTY) return -1;

	int available_channel_=-1;	
	for(auto& p:PParam::val()->JuiceChannel[get_]){
		if(_status_channel[p]==1) available_channel_=p;
	}

	return available_channel_;
}
void ofApp::sendJuiceSignal(){
	if(!_serial.isInitialized()) return;
	if(_idx_channel<0) return;

#ifdef WEEKAND_OFF
	int day_=ofGetWeekday();
	if(day_==0 || day_==6) return;
#endif

	auto c_=PParam::val()->ChannelCmd[_idx_channel].c_str();
	_serial.writeByte(c_[0]);	
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
		ofSystem(PParam::val()->DeleteCmd+" "+ofToDataPath("tmp\\")+"*.png");

		ofLog()<<"start recording.... "<<ofGetElapsedTimeMillis();
	}else{
		_timer_record.stop();
		ofLog()<<"end recording..."<<ofGetElapsedTimeMillis()<<" #fr= "<<_idx_record;
	}
}
void ofApp::saveCameraFrame(){
	float scale_=(float)PParam::val()->GifSize/(float)CAM_HEIGHT;
	float w_=(float)CAM_WIDTH*scale_;

	_fbo_save.begin();
	ofClear(255);
		_camera.draw(PParam::val()->GifSize/2-w_/2,0,w_,PParam::val()->GifSize);		
	_fbo_save.end();
	

	ofPixels pix;	
    _fbo_save.readToPixels(pix);	
	pix.mirror(false,true);
 
	_recorder.addFrame(pix);
	//ofSaveImage(pix,"raw/"+_user_id+".png");

}
void ofApp::createFruitImage(){

	ofLog()<<"Create Fruit Image !!!";
	
	for(int x=0;x<FRUIT_GROUP;++x){
			
		_fruit_rain.reset();
		_fruit_rain.setAutoFruit(false);	
		
		_fruit_rain.setFruit(x);
		_fruit_rain.start();
		for(int i=0;i<180;++i)
			_fruit_rain.update(16);

		for(int i=0;i<PParam::val()->GifLength*PParam::val()->GifFps;++i){
			_fbo_save.begin();
			ofClear(255);
				ofPushMatrix();
				ofTranslate((float)PParam::val()->GifSize/2,0);
				ofScale((float)PParam::val()->GifSize/WIN_HEIGHT,(float)PParam::val()->GifSize/WIN_HEIGHT);
					_fruit_rain.drawBack(1);
					_fruit_rain.drawFront(1);
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

void ofApp::playSound(PSound p_){
	_soundfx[p_].play();
}

void ofApp::updateSound(float dt_){
	_timer_bgm_in.update(dt_);	
	_timer_bgm_out.update(dt_);	
	_sound_bgm.setVolume(_timer_bgm_in.val()*(1.0-_timer_bgm_out.val()));
	float v=_sound_bgm.getVolume();
}
void ofApp::startBgm(){
	_timer_bgm_out.reset();
	_timer_bgm_in.restart();

	_sound_bgm.play();
}
void ofApp::stopBgm(){
	_timer_bgm_out.restart();
}