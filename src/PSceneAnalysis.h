#pragma once
#ifndef PSCENE_ANALYSIS_H
#define PSCENE_ANALYSIS_H

#include "PSceneBase.h"


class PSceneAnalysis:public SceneBase{

	FrameTimer _timer_hint;
	ofImage _img_hint;

public:
	PSceneAnalysis(ofApp* set_):SceneBase(set_){
		_mlayer=1;
		_order_scene=3;

		_timer_hint=FrameTimer(3000);

		_img_hint.loadImage("ui_img/hint_analysis.png");

		setup();
	}
	void drawLayer(int i){
		switch(i){
			case 0:
				_img_hint.draw(0,0);
				break;
		}

	}
	void update(float dt_){
		SceneBase::update(dt_);
	}

	void init(){
		SceneBase::init();
	}
};


#endif