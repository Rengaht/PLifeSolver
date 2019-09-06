#pragma once
#ifndef PSCENE_ANALYSIS_H
#define PSCENE_ANALYSIS_H

#include "PSceneBase.h"


class PSceneAnalysis:public SceneBase{

	FrameTimer _timer_hint;
	ofImage _img_hint;
	PGlowLine _glow;
public:
	PSceneAnalysis(ofApp* set_):SceneBase(set_){
		_mlayer=1;
		_order_scene=3;

		_timer_hint=FrameTimer(3000);

		_img_hint.loadImage("_img_ui/hint_analysis.png");

		setup();

		ofAddListener(_timer_in[0].finish_event,this,&PSceneAnalysis::onSceneInFinish);

		_glow=PGlowLine(540,915,200,150,30);


	}
	void drawLayer(int i){
		switch(i){
			case 0:
				_img_hint.draw(0,0);
				_glow.draw(getLayerAlpha(0));
				break;
		}

	}
	void update(float dt_){
		SceneBase::update(dt_);
		_glow.update(dt_);
	}

	void init(){
		SceneBase::init();
	}
	void onSceneInFinish(int &e){
		//_ptr_app->sendFaceRequest();
	}
};


#endif