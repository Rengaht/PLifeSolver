#pragma once
#ifndef PSCENE_AUTH_H
#define PSCENE_AUTH_H

#include "PSceneBase.h"

class PSceneAuth:public SceneBase{

	FrameTimer _timer_hint;
	ofImage _img_hint;
	ofImage _img_text;

public:
	PSceneAuth(ofApp* set_):SceneBase(set_){
		_mlayer=2;
		_order_scene=1;

		_timer_hint=FrameTimer(3000);

		_img_hint.loadImage("ui_img/hint_auth.png");
		_img_text.loadImage("ui_img/text_auth.png");
		
		setup();

		ofAddListener(_timer_hint.finish_event,this,&PSceneAuth::onTimerHintFinish);
		ofAddListener(_timer_in[1].finish_event,this,&PSceneAuth::onSceneInFinish);

	}
	void drawLayer(int i){
		switch(i){
			case 0:
				_img_text.draw(0,0);
				break;
			case 1:
				_img_hint.draw(0,0);
				break;
		}

	}
	void update(float dt_){
		SceneBase::update(dt_);
		
		_timer_hint.update(dt_);

	}

	void init(){
		SceneBase::init();
	}

	void onSceneInFinish(int &e){
		_timer_hint.restart();
	}
	void onTimerHintFinish(int &e){
		_ptr_app->prepareScene(ofApp::PDETECT);
	}
};


#endif