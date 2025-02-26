#pragma once
#ifndef PSCENE_ANALYSIS_H
#define PSCENE_ANALYSIS_H

#include "PSceneBase.h"
#define NUM_HINT_ANALYSIS 5

class PSceneAnalysis:public SceneBase{

	FrameTimer _timer_hint;
	ofImage _img_hint[NUM_HINT_ANALYSIS];
	PGlowLine _glow;

	int _idx_hint;
	int _count_hint;
	bool _receive_result;

public:
	PSceneAnalysis(ofApp* set_):SceneBase(set_){
		_mlayer=1;
		_order_scene=3;

		_timer_hint=FrameTimer(2000);

		for(int i=0;i<5;++i)
			_img_hint[i].loadImage("_img_ui/hint_analysis-"+ofToString(i+1)+".png");
		
		setup();

		_timer_out[0]=FrameTimer(5000);

		ofAddListener(_timer_in[0].finish_event,this,&PSceneAnalysis::onSceneInFinish);
		ofAddListener(_timer_hint.finish_event,this,&PSceneAnalysis::onTimerHintFinish);
		ofAddListener(_ptr_app->_event_receive_result,this,&PSceneAnalysis::onResultFinish);

		_glow=PGlowLine(540,915,200,150,30);
		_timer_sleep=FrameTimer(SLEEP_TIME*3);

	}
	void drawLayer(int i){
		switch(i){
			case 0:
				_ptr_app->_fruit_rain.drawBack(1.0-_timer_out[0].valEaseInOut());
#ifdef USE_BACKGROUND_SUB
				_ptr_app->drawForeground();
#endif
				_ptr_app->_fruit_rain.drawFront(1.0-_timer_out[0].valEaseInOut());

				ofPushStyle();
				ofSetColor(255,255*_timer_hint.valFade()*getLayerAlpha(0));
					_img_hint[_idx_hint].draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
				ofPopStyle();

				_glow.draw(getLayerAlpha(0));
				break;
		}

	}
	void update(float dt_){
		SceneBase::update(dt_);
		_glow.update(dt_);
		_timer_hint.update(dt_);

		if(_receive_result && _count_hint>=NUM_HINT_ANALYSIS-1){
			_ptr_app->prepareScene(ofApp::PRESULT);
			_receive_result=false;
		}
	}

	void init(){
		SceneBase::init();
		_idx_hint=0;
		_count_hint=0;
		_timer_hint.restart();
		_receive_result=false;
	}
	void onSceneInFinish(int &e){
		//_ptr_app->sendFaceRequest();
	}
	void onTimerHintFinish(int &e){
		
		(++_idx_hint)%=NUM_HINT_ANALYSIS;		
		_timer_hint.restart();
		_count_hint++;

		if(_idx_hint==2) _glow.setWidth(280);
		else if(_idx_hint>2) _glow.setWidth(220);
		else _glow.setWidth(150);

		//_glow.restart();
	}
	void onResultFinish(int &e){
		_receive_result=true;
	}
};


#endif