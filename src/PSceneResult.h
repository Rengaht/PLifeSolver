#pragma once
#ifndef PSCENE_RESULT_H
#define PSCENE_RESULT_H

#define CARD_TIME 5000
#define QRCODE_TIME 20000


#include "PSceneBase.h"


class PSceneResult:public SceneBase{

	FrameTimer _timer_scan;
	FrameTimer _timer_count;
	int _num_count;

	ofImage _img_question[MJUICE_RESULT],_img_solution[MJUICE_RESULT];
	ofImage _img_qrcode,_img_takeaway;
	ofImage _img_number[10];
public:
	PSceneResult(ofApp* set_):SceneBase(set_){
		_mlayer=4;
		_order_scene=4;

		_timer_scan=FrameTimer(QRCODE_TIME);
		_timer_count=FrameTimer(1000);

		for(int i=0;i<MJUICE_RESULT;++i){
			_img_question[i].loadImage("_img_ui/question/"+ofToString(i+1)+".png");
			_img_solution[i].loadImage("_img_ui/solution/"+ofToString(i+1)+".png");
		}
		_img_takeaway.loadImage("_img_ui/takeaway_result.png");
		_img_qrcode.loadImage("_img_ui/qrcode_result.png");
		
		for(int i=0;i<10;++i) _img_number[i].loadImage("_img_ui/result_count/"+ofToString(i)+".png");

		setup();

		ofAddListener(_timer_in[3].finish_event,this,&PSceneResult::onTimerSceneInFinish);
		ofAddListener(_timer_scan.finish_event,this,&PSceneResult::onTimerScanFinish);
		ofAddListener(_timer_count.finish_event,this,&PSceneResult::onTimerCountFinish);
		
	}
	void drawLayer(int i){

		int num_=ofClamp(QRCODE_TIME/1000-_num_count,0,QRCODE_TIME/1000);
		switch(i){
			case 0:
				_img_question[_ptr_app->_idx_user_juice].draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
                break;
			case 1:
                _img_solution[_ptr_app->_idx_user_juice].draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
                break;
			case 2:
                _img_takeaway.draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
                ofPushStyle();
                //ofSetColor(255,255*ofClamp(1.0-_timer_count.val(),0,1)*getLayerAlpha(0));
                    _img_number[(int)ofClamp(floor(num_/10),0,9)].draw(297,797);
					_img_number[(int)ofClamp(num_%10,0,9)].draw(314,797);
                ofPopStyle();
                break;
			case 3:
				_img_qrcode.draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
				break;
		}

	}
	void update(float dt_){
		SceneBase::update(dt_);		
		_timer_scan.update(dt_);
		_timer_count.update(dt_);
	}

	void init(){
		SceneBase::init();
		_timer_scan.reset();

		_timer_count.reset();
		_num_count=0;
	}

	void onTimerSceneInFinish(int &e){
		_timer_scan.restart();
		_timer_count.restart();
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