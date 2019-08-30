#pragma once
#ifndef PSCENE_SLEEP_H
#define PSCENE_SLEEP_H

#include "PSceneBase.h"
#include "PGlowLine.h"

#define FACE_DETECT_FRAME 4000

class PSceneSleep:public SceneBase{

	FrameTimer _timer_hint;
	ofImage _img_hint;
	ofImage _img_text;
	
	int _index_face_start;

	PGlowLine _glow;

public:
	PSceneSleep(ofApp* set_):SceneBase(set_){
		_mlayer=2;
		_order_scene=0;

		_timer_hint=FrameTimer(3000);

		_img_hint.loadImage("ui_img/hint_sleep.png");
		_img_text.loadImage("ui_img/text_sleep.png");

		_glow=PGlowLine(540,540,420,420,60);

		setup();
	}
	void drawLayer(int i){
		switch(i){
			case 0:
				_img_text.draw(0,0);
				_glow.draw(getLayerAlpha(0));
				break;
			case 1:
				_img_hint.draw(0,0);
				break;
		}

	}
	void update(float dt_){
		SceneBase::update(dt_);
		_glow.update(dt_);
		
		 if(_ptr_app->faceFound()){
            if(_index_face_start<0){
                _index_face_start=0;
                ofLog()<<"---------- detect face ---------- ";
            }else{
                if(_index_face_start>=FACE_DETECT_FRAME) _ptr_app->prepareScene(ofApp::PAUTH);
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
		
		_index_face_start=-1;
	}
};


#endif