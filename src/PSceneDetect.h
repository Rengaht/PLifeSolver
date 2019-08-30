#pragma once
#ifndef PSCENE_DETECT_H
#define PSCENE_DETECT_H

#include "PSceneBase.h"


class PSceneDetect:public SceneBase{

	FrameTimer _timer_detect;
	ofImage _img_hint;
	ofImage _img_text;

	PGlowLine _glow;
public:
	PSceneDetect(ofApp* set_):SceneBase(set_){
		_mlayer=2;
		_order_scene=2;

		_timer_detect=FrameTimer(5000);

		_img_text.loadImage("ui_img/text_detect.png");
		_img_hint.loadImage("ui_img/hint_detect.png");

		setup();

		ofAddListener(_timer_detect.finish_event,this,&PSceneDetect::onTimerDetectFinish);
		ofAddListener(_timer_in[1].finish_event,this,&PSceneDetect::onSceneInFinish);

		_glow=PGlowLine(540,915,200,150,30);
	}
	void drawLayer(int i){
		switch(i){
			case 0:
				_img_text.draw(0,0);
				break;
			case 1:
				_img_hint.draw(0,0);
				_glow.draw(getLayerAlpha(1));
				break;
		}

	}
	void update(float dt_){
		SceneBase::update(dt_);

		_timer_detect.update(dt_);
		_glow.update(dt_);
	}

	void init(){
		SceneBase::init();
	}
	void onTimerDetectFinish(int &e){
		_ptr_app->prepareScene(ofApp::PANALYSIS);
	}
	void onSceneInFinish(int &e){
		_timer_detect.restart();
	}
};

#endif