#pragma once
#ifndef PSCENE_RESULT_H
#define PSCENE_RESULT_H

#define CARD_TIME 5000
#define QRCODE_TIME 10000

#include "PSceneBase.h"


class PSceneResult:public SceneBase{

	FrameTimer _timer_scan;
	ofImage _img_card[4];
public:
	PSceneResult(ofApp* set_):SceneBase(set_){
		_mlayer=4;
		_order_scene=4;

		_timer_scan=FrameTimer(QRCODE_TIME);

		_img_card[0].loadImage("ui_img/question_result.png");
		_img_card[1].loadImage("ui_img/solution_result.png");
		_img_card[2].loadImage("ui_img/takeaway_result.png");
		_img_card[3].loadImage("ui_img/qrcode_result.png");
		
		setup();

		ofAddListener(_timer_in[3].finish_event,this,&PSceneResult::onTimerSceneInFinish);
		ofAddListener(_timer_scan.finish_event,this,&PSceneResult::onTimerScanFinish);

		
	}
	void drawLayer(int i){
		switch(i){
			case 0:
			case 1:
			case 2:
			case 3:
				_img_card[i].draw(0,0);
				break;
		}

	}
	void update(float dt_){
		SceneBase::update(dt_);		
		_timer_scan.update(dt_);
	}

	void init(){
		SceneBase::init();
		_timer_scan.reset();
	}

	void onTimerSceneInFinish(int &e){
		_timer_scan.restart();
	}
	void onTimerScanFinish(int &e){
		_ptr_app->prepareScene(ofApp::PSLEEP);
	}
};


#endif