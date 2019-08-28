#pragma once
#ifndef PSCENE_RESULT_H
#define PSCENE_RESULT_H

#include "PSceneBase.h"


class PSceneResult:public SceneBase{

	FrameTimer _timer_hint;
	ofImage _img_card[4];
public:
	PSceneResult(ofApp* set_):SceneBase(set_){
		_mlayer=4;
		_order_scene=4;

		_timer_hint=FrameTimer(3000);

		_img_card[0].loadImage("ui_img/question_result.png");
		_img_card[1].loadImage("ui_img/solution_result.png");
		_img_card[2].loadImage("ui_img/takeaway_result.png");
		_img_card[3].loadImage("ui_img/qrcode_result.png");
		
		setup();
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

	}

	void init(){
		SceneBase::init();
	}
};


#endif