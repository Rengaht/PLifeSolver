#pragma once
#ifndef PSCENE_DETECT_H
#define PSCENE_DETECT_H

#define MDETECT_COUNT 5
#define FRUIT_TIME_START 1
#include "PSceneBase.h"


class PSceneDetect:public SceneBase{

	FrameTimer _timer_count,_timer_delay;
	FrameTimer _timer_flash;
	int _num_count;
	
	ofImage _img_hint;
	ofImage _img_text[3];
	int _idx_text;
	FrameTimer _timer_text;
	
	PGlowLine _glow;

	ofImage _img_number[5];
	bool _flash;
public:
	PSceneDetect(ofApp* set_):SceneBase(set_){
		_mlayer=2;
		_order_scene=2;

		_timer_delay=FrameTimer(1000);
		_timer_count=FrameTimer(1000);
		_timer_flash=FrameTimer(100);

		_idx_text=0;
		_timer_text=FrameTimer(2000);
		for(int i=0;i<3;++i)
			_img_text[i].loadImage("_img_ui/text_detect-"+ofToString(i+1)+".png");

		_img_hint.loadImage("_img_ui/hint_detect.png");

		for(int i=1;i<=MDETECT_COUNT;++i){
			_img_number[i-1].loadImage("_img_ui/detect_count/"+ofToString(i)+".png");
		}

		setup();

		ofAddListener(_timer_count.finish_event,this,&PSceneDetect::onTimerCountFinish);
		ofAddListener(_timer_flash.finish_event,this,&PSceneDetect::onTimerFlashFinish);
		ofAddListener(_timer_in[1].finish_event,this,&PSceneDetect::onSceneInFinish);

		ofAddListener(_timer_delay.finish_event,this,&PSceneDetect::onTimerDelayFinish);
		ofAddListener(_timer_text.finish_event,this,&PSceneDetect::onTimerTextFinish);

		_glow=PGlowLine(WIN_HEIGHT/2,915,160,150,30);
	}
	void drawLayer(int i){
		switch(i){
			case 0:

				_ptr_app->_fruit_rain.drawBack(1);
#ifdef USE_BACKGROUND_SUB
				_ptr_app->drawForeground();
				_ptr_app->_fruit_rain.drawFront(1);
#endif				
				if(_timer_count.isStart()){
					ofPushStyle();
					ofSetColor(255,255*ofClamp(1.0-_timer_count.valEaseIn(),0,1)*getLayerAlpha(0));
						_img_number[(int)ofClamp(MDETECT_COUNT-1-_num_count,0,MDETECT_COUNT-1)].draw(0,0,WIN_HEIGHT,WIN_HEIGHT);					
					ofPopStyle();
				}

				ofPushStyle();
				ofSetColor(255,255*ofClamp(1.0-_timer_text.valEaseIn(),0,1)*getLayerAlpha(0));
					_img_text[_idx_text].draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
				ofPopStyle();
				break;
			case 1:
				if(_timer_count.isStart()){
					ofPushStyle();
					ofSetColor(255,255*ofClamp(1.0-_timer_count.valEaseIn(),0,1)*getLayerAlpha(1));
						_img_hint.draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
					ofPopStyle();
					_glow.draw(getLayerAlpha(1));
				}
				ofPushStyle();
				ofSetColor(255,255*getLayerAlpha(0)*_timer_flash.val());
					ofDrawRectangle(0,0,WIN_HEIGHT,WIN_HEIGHT);
				ofPopStyle();
				break;
		}

	}
	void update(float dt_){
		SceneBase::update(dt_);

		_timer_delay.update(dt_);
		_timer_count.update(dt_);
		_timer_flash.update(dt_);
		_glow.update(dt_);

		_timer_text.update(dt_);
	}

	void init(){
		SceneBase::init();

		_num_count=0;
		_timer_flash.reset();
		_timer_count.reset();
		_timer_text.reset();
		_timer_delay.reset();
	}
	void onTimerCountFinish(int &e){

		_num_count++;
		if(_num_count<5){
			_timer_count.restart();
			_ptr_app->playSound(ofApp::PSound::SCOUNT);
			if(_num_count==FRUIT_TIME_START) _ptr_app->_fruit_rain.start();			
		}else{
			_ptr_app->playSound(ofApp::PSound::SSHUTTER);
			_ptr_app->setRecord(false);
			_ptr_app->setCameraPause(true);
			_ptr_app->_fruit_rain.stop();
			_timer_flash.restart();
		}
	}
	void onTimerFlashFinish(int &e){
		_ptr_app->prepareScene(ofApp::PANALYSIS);			
	}
	void onSceneInFinish(int &e){
		
		_timer_text.restart();
		_timer_delay.restart();		
	}
	void onTimerTextFinish(int &e){				

		(++_idx_text)%=3;
		_timer_text.restart();
	}
	void onTimerDelayFinish(int &e){
		_timer_count.restart();
		_ptr_app->setRecord(true);
		_ptr_app->playSound(ofApp::PSound::SCOUNT);
	}
};

#endif