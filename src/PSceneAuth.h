#pragma once
#ifndef PSCENE_AUTH_H
#define PSCENE_AUTH_H

#define MAUTH_COUNT 3

#include "PSceneBase.h"

class PSceneAuth:public SceneBase{

	FrameTimer _timer_hint;
	//FrameTimer _timer_blink;

	int _num_timer;

	ofImage _img_hint;
	ofImage _img_text;
	ofImage _img_confirm;

	PGlowLine _glow;
	//bool _sound_confirm_played;

public:
	PSceneAuth(ofApp* set_):SceneBase(set_){
		_mlayer=2;
		_order_scene=1;

		_timer_hint=FrameTimer(2000);
		//_timer_blink=FrameTimer(1200);
		//_timer_blink.setContinuous(true);

		_img_hint.loadImage("_img_ui/hint_auth.png");
		_img_text.loadImage("_img_ui/text_auth.png");
		/*for(int i=0;i<MAUTH_COUNT;++i)
			_img_text[i].loadImage("_img_ui/text_auth/"+ofToString(i+1)+".png");

		_img_confirm.loadImage("_img_ui/hint_confirm.png");*/
		
		setup();

		ofAddListener(_timer_hint.finish_event,this,&PSceneAuth::onTimerHintFinish);
		//ofAddListener(_timer_confirm.finish_event,this,&PSceneAuth::onTimerConfirmFinish);
		ofAddListener(_timer_in[0].finish_event,this,&PSceneAuth::onSceneInFinish);

		_glow=PGlowLine(540,915,240,150,30);

	}
	void drawLayer(int i){		
		switch(i){
			case 1:
				// countdown
				//ofPushStyle();
				//ofSetColor(255,255*ofClamp(1.0-_timer_hint.val(),0,1)*getLayerAlpha(0));
				//	//_ptr_app->_img_number[(int)ofClamp(2-_num_timer,0,2)].draw(373,771,53,74);
				//	_img_text[(int)ofClamp(MAUTH_COUNT-1-_num_timer,0,MAUTH_COUNT-1)].draw(0,0,WIN_HEIGHT,WIN_HEIGHT);				
				//ofPopStyle();
				_img_hint.draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
				_glow.draw(getLayerAlpha(0));
				break;
			case 0:
				//ofPushStyle();
				//ofSetColor(255,255*ofClamp(1.0-_timer_blink.val()*.8,0,1)*getLayerAlpha(i));				
					_img_text.draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
				//ofPopStyle();
				break;
			/*case 1:
				ofPushStyle();
								
				ofSetColor(255,255*ofClamp(1.0-3*_timer_confirm.val(),0,1)*getLayerAlpha(1));
					_img_hint.draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
				ofSetColor(255,255*ofClamp(3*(_timer_confirm.val()-.33),0,1)*getLayerAlpha(1));
					_img_confirm.draw(0,0,WIN_HEIGHT,WIN_HEIGHT);				
				
				ofPopStyle();
				
				_glow.draw(getLayerAlpha(1));
				break;*/
		}

	}
	void update(float dt_){
		SceneBase::update(dt_);
		
		_timer_hint.update(dt_);		
		/*_timer_confirm.update(dt_);
		if(_ptr_app->faceFound()){
			if(!_sound_confirm_played && _timer_confirm.valEaseInOut()*3>2){
				_sound_confirm_played=true;
				_ptr_app->playSound(ofApp::PSound::SCHECK);
			}
		}*/
		//_timer_blink.update(dt_);
		_glow.update(dt_);
	}

	void init(){
		SceneBase::init();
		_num_timer=0;
		//_sound_confirm_played=false;
	}

	void onSceneInFinish(int &e){
		_timer_hint.restart();
		//_timer_blink.restart();
		//_ptr_app->playSound(ofApp::PSound::SCHECK);
	}
	void onTimerHintFinish(int &e){
		
		_ptr_app->prepareScene(ofApp::PDETECT);

		/*_num_timer++;
		if(_num_timer<3){
			_timer_hint.restart();		
			_ptr_app->playSound(ofApp::PSound::SCOUNT);
		}else{
			if(_ptr_app->faceFound()) _timer_confirm.restart();
			else _ptr_app->prepareScene(ofApp::PSLEEP);
		}*/
	}
	void onTimerConfirmFinish(int &e){
		_ptr_app->prepareScene(ofApp::PDETECT);
		
	}

	/*void end(){
		ofLog()<<"Scene "<<_order_scene<<" end!";
    
		_timer_out[1].restart();
		_status=SceneStatus::End;
	}*/
};


#endif