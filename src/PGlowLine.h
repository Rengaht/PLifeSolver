#pragma once
#ifndef PGLOW_LINE_H
#define PGLOW_LINE_H

#define ANIM_VEL 800
#define LINE_ANGLE 6

#include "ofMain.h"
#include "FrameTimer.h"


class PGlowLine{

	FrameTimer _timer,_timer2;
	ofVec2f _pos;
	ofVec2f _size;
	
	float _length;
	float _ang;
	int _mline;

public:
	PGlowLine(){}
	PGlowLine(float x, float y, float radx, float rady, float ang_){
		_pos=ofVec2f(x,y);
		_size=ofVec2f(radx,rady);
		_ang=ofDegToRad(ang_);
		_length=.8;
		
		_mline=ang_/(float)LINE_ANGLE;

		_timer=FrameTimer(ANIM_VEL);
		_timer2=FrameTimer(ANIM_VEL*1.2);
		_timer.restart();
		_timer2.restart();
	}
	void update(float dt_){
		_timer.update(dt_);
		_timer2.update(dt_);
		if(_timer.val()==1) _timer.restart();
		if(_timer2.val()==1) _timer2.restart();
	}
	void draw(){

		ofPushStyle();
		ofSetColor(29,79,144);
		ofSetLineWidth(8);

		ofPushMatrix();
		ofTranslate(_pos);
		
		float ang_per_line=ofDegToRad(LINE_ANGLE);
		float ang_start=-_ang/2.0;
		float anim_=_timer.valEaseInOut();
		float stretch_=_length+(1.0-_length)*sin(anim_*PI);
		//float shrink_=_length+(1.0-_length)*(1.0-sin(anim_*PI));
		float shrink_=_length+(1.0-_length)*sin(_timer2.valEaseInOut()*PI);

		for(int i=0;i<_mline;++i){
			float a_=ang_per_line*i+ang_start;
			if(i%2==0){
				ofLine(_size.x*_length*cos(a_),_size.y*_length*sin(a_),
						_size.x*stretch_*cos(a_),_size.y*stretch_*sin(a_));
				ofLine(-_size.x*_length*cos(a_),_size.y*_length*sin(a_),
						-_size.x*stretch_*cos(a_),_size.y*stretch_*sin(a_));
			}else{
				ofLine(_size.x*_length*cos(a_),_size.y*_length*sin(a_),
						_size.x*shrink_*cos(a_),_size.y*shrink_*sin(a_));
				ofLine(-_size.x*_length*cos(a_),_size.y*_length*sin(a_),
						-_size.x*shrink_*cos(a_),_size.y*shrink_*sin(a_));
			}

		}

		ofPopMatrix();

		ofPopStyle();
	}



};

#endif