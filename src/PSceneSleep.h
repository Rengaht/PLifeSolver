#pragma once
#ifndef PSCENE_SLEEP_H
#define PSCENE_SLEEP_H

#include "PSceneBase.h"
#include "PGlowLine.h"

#define FACE_DETECT_FRAME 4000

class PSceneSleep:public SceneBase{

	FrameTimer _timer_hint;
	FrameTimer _timer_confirm;

	ofImage _img_hint[2];
	ofImage _img_text[2];
	ofImage _img_face;
	
	int _index_face_start;
	
	int _idx_hint;
	bool _confirm_face;	

	PGlowLine _glow;

	bool _sound_confirm_played;

public:
	PSceneSleep(ofApp* set_):SceneBase(set_){
		_mlayer=2;
		_order_scene=0;

		_timer_hint=FrameTimer(8000);
		_timer_hint.restart();

		_timer_confirm=FrameTimer(2000);

		_img_hint[0].loadImage("_img_ui/hint_sleep-1.png");
		_img_hint[1].loadImage("_img_ui/hint_sleep-2.png");

		_img_text[0].loadImage("_img_ui/text_sleep-1.png");
		_img_text[1].loadImage("_img_ui/text_sleep-2.png");

		_img_face.loadImage("_img_ui/face_sleep.png");

		_glow=PGlowLine(540,540,420,420,60);

		ofAddListener(_timer_hint.finish_event,this,&PSceneSleep::onTimerHintFinish);
		ofAddListener(_timer_confirm.finish_event,this,&PSceneSleep::onTimerConfirmFinish);

		setup();
	}
	void drawLayer(int i){
		switch(i){
			case 0:
				ofPushStyle();
				ofSetColor(255,255*_timer_hint.valFade()*getLayerAlpha(i));
					_img_text[_idx_hint].draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
				ofPopStyle();
				
				_glow.draw(getLayerAlpha(0));
				
				break;
			case 1:
				ofPushStyle();
				ofSetColor(255,255*(1.0-ofClamp(_timer_confirm.val()*5.0,0,1))*getLayerAlpha(i));
					_img_hint[0].draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
				ofPopStyle();

				if(_confirm_face){
					ofPushStyle();
					ofSetColor(255,255*ofClamp(_timer_confirm.valEaseInOut()*5.0-2.0,0,1)*getLayerAlpha(i));
						_img_hint[1].draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
					ofPopStyle();
				}
				
				_img_face.draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
				break;
		}

	}
	void update(float dt_){
		SceneBase::update(dt_);
		_glow.update(dt_);
		
		_timer_hint.update(dt_);
		
		_timer_confirm.update(dt_);
		if(!_sound_confirm_played &&_timer_confirm.val()*5>2){
			_sound_confirm_played=true;
			_ptr_app->playSound(ofApp::PSound::SCHECK);
		}


		 if(_ptr_app->faceFound()){
            if(_index_face_start<0){
                _index_face_start=0;
                ofLog()<<"---------- detect face ---------- ";
            }else{
                if(!_confirm_face && _index_face_start>=FACE_DETECT_FRAME){
					_confirm_face=true;				
					_timer_confirm.restart();		
					
					

				}
				_index_face_start+=dt_;
            }
        }else{
            if(_index_face_start>0)
                ofLog()<<"----------  no face ---------- "<<_index_face_start;
            _index_face_start=-1;
            
        }
	}

	void init(){
		SceneBase::init();
		_idx_hint=0;
		_index_face_start=-1;

		_confirm_face=false;
		_timer_confirm.reset();
		_timer_hint.restart();

		_sound_confirm_played=false;

	}
	void onTimerHintFinish(int &e){
		
		(++_idx_hint)%=2;
		_timer_hint.restart();
	}

	void onTimerConfirmFinish(int &e){		
		_ptr_app->prepareScene(ofApp::PAUTH);			 		
	}
};


#endif