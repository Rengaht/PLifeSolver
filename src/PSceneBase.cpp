#include "PSceneBase.h"
#include "ofApp.h"

ofEvent<int> SceneBase::sceneInFinish=ofEvent<int>();
ofEvent<int> SceneBase::sceneOutFinish=ofEvent<int>();
FrameTimer SceneBase::_timer_sleep=FrameTimer();
ofSoundPlayer SceneBase::SoundButton=ofSoundPlayer();

SceneBase::SceneBase(ofApp *set_ptr){
	_ptr_app=set_ptr;
	_timer_sleep=FrameTimer(SLEEP_TIME);

	
}

void SceneBase::setup(){
	for(int i=0;i<_mlayer;++i){
		_timer_in.push_back(FrameTimer(EASE_DUE,EASE_OFFSET*(i+1)));
		_timer_out.push_back(FrameTimer(EASE_DUE,EASE_OFFSET*(_mlayer-i)));
	}
    //_enable_button=new bool[_button.size()];
    //for(int i=0;i<_button.size();++i){
        //_enable_button.push_back(false);
    //    _enable_button[i]=false;
    //}
}


void SceneBase::draw(){
	ofPushStyle();
	//ofEnableAlphaBlending();


	for(int i=0;i<_mlayer;++i){

		ofPushStyle();
		ofPushMatrix();

		ofSetColor(255,255*_timer_in[i].valEaseInOut()*(1-_timer_out[i].valEaseInOut()));		
		
		drawLayer(i);

		ofPopMatrix();
		ofPopStyle();	
	}	

#ifdef DRAW_DEBUG_INFO
	ofPushStyle();
	ofSetColor(0,255,0);
	ofNoFill();
	int len=_button.size();
	for(int i=0;i<len;++i){
		if(_enable_button[i]) ofDrawRectangle(_button[i]);
	}

	ofPopStyle();
#endif


	ofPopStyle();
}
float SceneBase::getLayerAlpha(int i){
    return _timer_in[i].valEaseInOut()*(1-_timer_out[i].valEaseInOut());
}
void SceneBase::drawLayer(int i){
	
}
void SceneBase::init(){
    
    ofLog()<<"Scene "<<_order_scene<<" init!";
    
	for(int i=0;i<_mlayer;++i){
		_timer_out[i].reset();
		_timer_in[i].restart();
	}
	_status=SceneStatus::Init;
	_trigger_in=false;
	_trigger_out=false;

	//for(auto& en:_enable_button) en=false;
    //for(int i=0;i<_button.size();++i) _enable_button[i]=false;
}

void SceneBase::end(){
    
    ofLog()<<"Scene "<<_order_scene<<" end!";
    
	for(int i=0;i<_mlayer;++i) _timer_out[i].restart();
	_status=SceneStatus::End;
	//for(auto& en:_enable_button) en=false;
    //for(int i=0;i<_button.size();++i) _enable_button[i]=false;
}

void SceneBase::update(float dt_){
	bool fin_=true;
	switch(_status){
	case Init:
		for(int i=0;i<_mlayer;++i){
			_timer_in[i].update(dt_);
			if(!_timer_in[i].finish()) fin_=false;
		}
		if(fin_){
			_status=Due;
			if(!_trigger_in){
				ofNotifyEvent(sceneInFinish,_order_scene,this);
				_trigger_in=true;
                //for(auto& en:_enable_button) en=true;
                //for(int i=0;i<_button.size();++i) _enable_button[i]=true;
                
				_timer_sleep.restart();
			}
		}
		break;
	case Due:
		_timer_sleep.update(dt_);
		if(_order_scene!=0 && _timer_sleep.finish()){
			ofLog()<<"Back to sleep...";
			_ptr_app->prepareScene(ofApp::PStage::PSLEEP);
			_timer_sleep.reset();
		}
		break;
	case End:
		for(int i=0;i<_mlayer;++i){
			_timer_out[i].update(dt_);
			if(!_timer_out[i].finish()) fin_=false;
		}
		if(fin_){
			if(!_trigger_out){
				ofNotifyEvent(sceneOutFinish,_order_scene,this);		
				_trigger_out=true;
			}
		}
		break;	
	}
}

void SceneBase::reset(){
	for(int i=0;i<_mlayer;++i){
		_timer_in[i].reset();
		_timer_out[i].reset();
	}
}

//bool SceneBase::handleMouse(float mx_,float my_){
//	
//	bool b=false;
//
//	int len=_button.size();
//	for(int i=0;i<len;++i){
//
//		if(!_enable_button[i]) continue;
//
//		if(_button[i].inside(mx_,my_)){
//			buttonEvent(i);
//			b=true;
//			
//			SoundButton.play();	
//		}
//	}
//
//	return b;
//}
//void SceneBase::buttonEvent(int index_){
//
//}