#pragma once
#ifndef PGLOW_LINE_H
#define PGLOW_LINE_H

#define ANIM_VEL 800
#define LINE_ANGLE 6
#define LBORDER 1.0
#define LWIDTH 5.0

#include "ofMain.h"
#include "FrameTimer.h"


class PGlowLine{

	FrameTimer _timer,_timer2;
	ofVec2f _pos;
	ofVec2f _size;
	
	float _ang;
	int _mline;

public:

	float _length;

	PGlowLine(){}
	PGlowLine(float x, float y, float radx, float rady, float ang_){
		_pos=ofVec2f(x,y);
		_size=ofVec2f(radx,rady);
		_ang=ofDegToRad(ang_);
		_length=.75;
		
		_mline=ang_/(float)LINE_ANGLE;

		_timer=FrameTimer(ANIM_VEL);
		
		_timer2=FrameTimer(ANIM_VEL);
		_timer2.setContinuous(true);

		_timer.restart();
		_timer2.restart();
	}
	void update(float dt_){
		_timer.update(dt_);
		_timer2.update(dt_);
		if(_timer.val()==1) _timer.restart();
		//if(_timer2.val()==1) _timer2.restart();
	}
	void draw(float alpha_=1.0){

		ofPushMatrix();
		ofTranslate(_pos);
		
		float ang_per_line=ofDegToRad(LINE_ANGLE);
		float ang_start=-_ang/2.0;
		
		float anim_=_timer.valEaseInOut();
		float anim2_=_timer2.valEaseInOut();

		float stretch_start_=_length+(1.0-_length)*valStart(anim_);
		float stretch_end_=_length+(1.0-_length)*valEnd(anim_);
		
		float shrink_start_=_length+(1.0-_length)*.3*valStart(anim2_);
		float shrink_end_=_length+(1.0-_length)*.3*valEnd(anim2_);

		for(int i=0;i<_mline;++i){
			float a_=ang_per_line*i+ang_start;
			if(i%2==0){
				
				ofPushMatrix();
				ofTranslate(_size.x*stretch_start_*cos(a_),_size.y*stretch_start_*sin(a_));
				ofRotate(ofRadToDeg(a_));
					drawLine(_size.x*cos(a_)*(stretch_end_-stretch_start_),alpha_);
				ofPopMatrix();
				
				ofPushMatrix();
				ofTranslate(_size.x*stretch_start_*cos(a_+PI),_size.y*stretch_start_*sin(a_+PI));
				ofRotate(ofRadToDeg(a_+PI));
					drawLine(_size.x*cos(a_)*(stretch_end_-stretch_start_),alpha_);				
				ofPopMatrix();

				
			}else{
				ofPushMatrix();
				ofTranslate(_size.x*shrink_start_*cos(a_),_size.y*shrink_start_*sin(a_));
				ofRotate(ofRadToDeg(a_));
					drawLine(_size.x*cos(a_)*(shrink_end_-shrink_start_),alpha_);
				ofPopMatrix();
				
				ofPushMatrix();
				ofTranslate(_size.x*shrink_start_*cos(a_+PI),_size.y*shrink_start_*sin(a_+PI));
				ofRotate(ofRadToDeg(a_+PI));
					drawLine(_size.x*cos(a_)*(shrink_end_-shrink_start_),alpha_);				
				ofPopMatrix();
			
			}

		}

		ofPopMatrix();

	}
	void drawLine(float len_,float alpha_=1.0){
		ofPushStyle();
		ofSetColor(229,202,133,alpha_*255);
			ofDrawRectangle(0,0,len_,LWIDTH);
		ofSetColor(29,79,144,alpha_*255);
			ofDrawRectangle(LBORDER,LBORDER,max(0.0,len_-2*LBORDER),LWIDTH-2*LBORDER);
		ofPopStyle();
	}

	float valEnd(float val_){
		if(val_<.66) return val_/2*3;
		return 1;
	}
	float valStart(float val_){
		if(val_<.66) return 0;
		return (val_-.66)*3;
	}
	void setWidth(float set_){
		_size.x=set_;
	}
	void restart(){
		_timer.restart();
		_timer2.restart();
	}
	
};

#endif