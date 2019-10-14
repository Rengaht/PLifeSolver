#pragma once
#ifndef PSCENE_RESULT_H
#define PSCENE_RESULT_H

//#define CARD_TIME 2000
//#define QRCODE_TIME 20000


#include "PSceneBase.h"


class PSceneResult:public SceneBase{

	FrameTimer _timer_scan;
	FrameTimer _timer_count;
	FrameTimer _timer_page_disappear;

	int _num_count;

	ofImage _img_question[FRUIT_GROUP],_img_solution[FRUIT_GROUP];
	ofImage _img_qrcode,_img_takeaway,_img_lalamove;
	ofImage _img_number[10];

	PGlowLine _glow;
public:
	PSceneResult(ofApp* set_):SceneBase(set_){
		_mlayer=6;
		_order_scene=4;

		_timer_scan=FrameTimer(PParam::val()->QrcodeTime);
		_timer_count=FrameTimer(1000);
		_timer_page_disappear=FrameTimer(EASE_DUE,PParam::val()->CardTime*.5);

		for(int i=0;i<FRUIT_GROUP;++i){
			_img_question[i].loadImage("_img_ui/question/"+ofToString(i+1)+".png");
			_img_solution[i].loadImage("_img_ui/solution/"+ofToString(i+1)+".png");
		}
		_img_takeaway.loadImage("_img_ui/takeaway_result.png");
		_img_qrcode.loadImage("_img_ui/qrcode_result.png");
		_img_lalamove.loadImage("_img_ui/lalamove_result.png");
		
		for(int i=0;i<10;++i) _img_number[i].loadImage("_img_ui/result_count/"+ofToString(i)+".png");

		setup();
		_timer_in[1]=FrameTimer(EASE_DUE,PParam::val()->CardTime*.3);
		_timer_in[2]=FrameTimer(EASE_DUE,PParam::val()->CardTime*1.5);
		_timer_in[3]=FrameTimer(EASE_DUE,PParam::val()->CardTime*2.5+EASE_DUE);
		_timer_in[4]=FrameTimer(EASE_DUE,PParam::val()->CardTime*2.5+EASE_DUE);
		_timer_in[5]=FrameTimer(EASE_DUE,PParam::val()->CardTime*3+EASE_DUE);


		ofAddListener(_timer_in[1].finish_event,this,&PSceneResult::onCardInFinish);
		ofAddListener(_timer_in[2].finish_event,this,&PSceneResult::onSolutionInFinish);

		ofAddListener(_timer_in[3].finish_event,this,&PSceneResult::onTimerSceneInFinish);
		ofAddListener(_timer_scan.finish_event,this,&PSceneResult::onTimerScanFinish);
		ofAddListener(_timer_count.finish_event,this,&PSceneResult::onTimerCountFinish);
		
		_glow=PGlowLine(540,680,374,150,30);
	}
	void drawLayer(int i){

		int num_=ofClamp(PParam::val()->QrcodeTime/1000-_num_count,0,PParam::val()->QrcodeTime/1000);
		switch(i){
			case 0:
				_ptr_app->_fruit_rain.drawBack(getLayerAlpha(0));
#ifdef USE_BACKGROUND_SUB
				_ptr_app->drawForeground();
#endif
				_ptr_app->_fruit_rain.drawFront(getLayerAlpha(0));
				ofPushStyle();
				ofSetColor(0,160*getLayerAlpha(i));
					ofDrawRectangle(0,0,WIN_HEIGHT,WIN_HEIGHT);
				ofPopStyle();
				break;
			case 1:
				ofPushStyle();
				ofSetColor(255,255*getLayerAlpha(i)*(1.0-_timer_page_disappear.valEaseInOut()));
					_img_question[_ptr_app->_idx_user_juice].draw(0,146,WIN_HEIGHT,WIN_HEIGHT);
				ofPopStyle();
                break;
			case 2:
                ofPushStyle();
				ofSetColor(255,255*getLayerAlpha(i)*(1.0-_timer_page_disappear.valEaseInOut()));
					_img_solution[_ptr_app->_idx_user_juice].draw(0,146,WIN_HEIGHT,WIN_HEIGHT);
				ofPopStyle();
                break;
			case 3:
                _img_takeaway.draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
                ofPushStyle();
                //ofSetColor(255,255*ofClamp(1.0-_timer_count.val(),0,1)*getLayerAlpha(0));
                    _img_number[(int)ofClamp(floor(num_/10),0,9)].draw(290,520);
					_img_number[(int)ofClamp(num_%10,0,9)].draw(312,520);
                ofPopStyle();
                break;
			case 4:
				_img_qrcode.draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
				ofPushMatrix();
				ofTranslate(682,484);
					_ptr_app->_qrcode_gen.draw(117,255*getLayerAlpha(i));
				ofPopMatrix();
				break;
			case 5:
				_img_lalamove.draw(0,0,WIN_HEIGHT,WIN_HEIGHT);
				_glow.draw(getLayerAlpha(i));
				break;
		}

	}
	void update(float dt_){
		SceneBase::update(dt_);		
		_timer_scan.update(dt_);
		_timer_count.update(dt_);

		_timer_page_disappear.update(dt_);
		_glow.update(dt_);
	}

	void init(){
		SceneBase::init();
		_timer_scan.reset();

		_timer_count.reset();
		_num_count=0;
	}
	void onCardInFinish(int &e){
		_ptr_app->playSound(ofApp::PSound::SRESULT);
	}
	void onSolutionInFinish(int &e){
		_ptr_app->playSound(ofApp::PSound::SRESULT);
		_timer_page_disappear.restart();
	}
	void onTimerSceneInFinish(int &e){
		_timer_scan.restart();
		_timer_count.restart();

		_ptr_app->playSound(ofApp::PSound::SFINISH);
		_ptr_app->sendJuiceSignal();
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