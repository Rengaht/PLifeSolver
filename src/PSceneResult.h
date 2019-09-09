#pragma once
#ifndef PSCENE_RESULT_H
#define PSCENE_RESULT_H

#define CARD_TIME 5000
#define QRCODE_TIME 10000

#include "PSceneBase.h"


class PSceneResult:public SceneBase{

	FrameTimer _timer_scan;
	FrameTimer _timer_count;
	int _num_count;

	ofImage _img_card[4];
public:
	PSceneResult(ofApp* set_):SceneBase(set_){
		_mlayer=4;
		_order_scene=4;

		_timer_scan=FrameTimer(QRCODE_TIME);
		_timer_count=FrameTimer(1000);

		_img_card[0].loadImage("_img_ui/question_result.png");
		_img_card[1].loadImage("_img_ui/solution_result.png");
		_img_card[2].loadImage("_img_ui/takeaway_result.png");
		_img_card[3].loadImage("_img_ui/qrcode_result.png");
		
		setup();

		ofAddListener(_timer_in[3].finish_event,this,&PSceneResult::onTimerSceneInFinish);
		ofAddListener(_timer_scan.finish_event,this,&PSceneResult::onTimerScanFinish);
		ofAddListener(_timer_count.finish_event,this,&PSceneResult::onTimerCountFinish);
		
	}
	void drawLayer(int i){
		switch(i){
			case 0:
			case 1:
                _img_card[i].draw(0,0);
                break;
			case 2:
                _img_card[i].draw(0,0);
                ofPushStyle();
                ofSetColor(238,216,152,255*ofClamp(1.0-_timer_count.val(),0,1)*getLayerAlpha(0));
                    ofDrawBitmapString(ofToString(ofClamp(QRCODE_TIME/1000-_num_count,0,QRCODE_TIME/1000)),300,832);
                ofPopStyle();
                break;
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
	 void onTimerCountFinish(int &e){
        _num_count++;
        _timer_count.restart();
    }
};


#endif