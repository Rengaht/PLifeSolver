#pragma once
#ifndef PSCENE_DETECT_H
#define PSCENE_DETECT_H

#include "PSceneBase.h"


class PSceneDetect:public SceneBase{

	FrameTimer _timer_count;
	int _num_count;
	
	ofImage _img_hint;
	ofImage _img_text;

	PGlowLine _glow;
public:
	PSceneDetect(ofApp* set_):SceneBase(set_){
		_mlayer=2;
		_order_scene=2;

		_timer_count=FrameTimer(1000);


		_img_text.loadImage("_img_ui/text_detect.png");
		_img_hint.loadImage("_img_ui/hint_detect.png");

		setup();

		ofAddListener(_timer_count.finish_event,this,&PSceneDetect::onTimerCountFinish);
		ofAddListener(_timer_in[1].finish_event,this,&PSceneDetect::onSceneInFinish);

		_glow=PGlowLine(540,915,200,150,30);
	}
	void drawLayer(int i){
		switch(i){
			case 0:
				ofPushStyle();
				ofSetColor(255,255*ofClamp(1.0-_timer_count.val(),0,1)*getLayerAlpha(0));
					_ptr_app->_img_number[(int)ofClamp(4-_num_count,0,4)].draw(492,613,96,134);					
				ofPopStyle();

				_img_text.draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
				break;
			case 1:
				_img_hint.draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
				_glow.draw(getLayerAlpha(1));
				break;
		}

	}
	void update(float dt_){
		SceneBase::update(dt_);

		_timer_count.update(dt_);
		_glow.update(dt_);
	}

	void init(){
		SceneBase::init();

		_num_count=0;
	}
	void onTimerCountFinish(int &e){

		_num_count++;
		if(_num_count<5){
			_timer_count.restart();
			if(_num_count==3) _ptr_app->setRecord(true);
		}else{
			_ptr_app->setRecord(false);
			_ptr_app->prepareScene(ofApp::PANALYSIS);			
		}
	}
	void onSceneInFinish(int &e){
		_timer_count.restart();
	}
};

#endif