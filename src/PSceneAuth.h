#pragma once
#ifndef PSCENE_AUTH_H
#define PSCENE_AUTH_H

#include "PSceneBase.h"

class PSceneAuth:public SceneBase{

	FrameTimer _timer_hint;
	int _num_timer;

	ofImage _img_hint;
	ofImage _img_text;

	PGlowLine _glow;

public:
	PSceneAuth(ofApp* set_):SceneBase(set_){
		_mlayer=2;
		_order_scene=1;

		_timer_hint=FrameTimer(1000);

		_img_hint.loadImage("ui_img/hint_auth.png");
		_img_text.loadImage("ui_img/text_auth.png");
		
		setup();

		ofAddListener(_timer_hint.finish_event,this,&PSceneAuth::onTimerHintFinish);
		ofAddListener(_timer_in[1].finish_event,this,&PSceneAuth::onSceneInFinish);

		_glow=PGlowLine(540,915,200,150,30);

	}
	void drawLayer(int i){		
		switch(i){
			case 0:
				_img_text.draw(0,0);
				// countdown
				ofPushStyle();
				ofSetColor(255,255*ofClamp(1.0-_timer_hint.val(),0,1)*getLayerAlpha(0));
					_ptr_app->_img_number[(int)ofClamp(2-_num_timer,0,2)].draw(373,771,53,74);
				ofPopStyle();
				break;
			case 1:
				_img_hint.draw(0,0);
				_glow.draw(getLayerAlpha(1));
				break;
		}

	}
	void update(float dt_){
		SceneBase::update(dt_);
		
		_timer_hint.update(dt_);		

		_glow.update(dt_);
	}

	void init(){
		SceneBase::init();
		_num_timer=0;
	}

	void onSceneInFinish(int &e){
		_timer_hint.restart();
	}
	void onTimerHintFinish(int &e){
		
		_num_timer++;
		if(_num_timer<3) _timer_hint.restart();		
		else{
			_timer_out[0].restart();
			_ptr_app->prepareScene(ofApp::PDETECT);
		}
	}
	void end(){
		ofLog()<<"Scene "<<_order_scene<<" end!";
    
		_timer_out[1].restart();
		_status=SceneStatus::End;
	}
};


#endif