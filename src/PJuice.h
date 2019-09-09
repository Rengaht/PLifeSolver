#pragma once
#ifndef PJUICE_H
#define PJUICE_H

#include "ofxSvg.h"

#define DROP_VEL 0
#define DROP_ACC .5

#define JUICE_RESOLUTION 150
#define JUICE_WAVE_COUNT 3
#define JUICE_WAVE_AMP 0.2
#define JUICE_DROP_INTERVAL 1000



class PDrop{
    
    ofColor _color;    
    ofVec2f _pos,_vel,_acc;
    float _hei;
    
public:
	
	static ofPath _shape_drop;
	static ofVec2f _size;

    PDrop(ofColor col_,float hei_){
        _color=col_;

        float w_=ofGetHeight();
        _pos=ofVec2f(w_/2,-_size.y*1.5);
        //_size=ofVec2f(w_/15,w_/10);
        
        _hei=hei_;
        
        _vel=ofVec2f(0,DROP_VEL);
        _acc=ofVec2f(0,DROP_ACC);
    }
    void draw(){
        ofPushStyle();
        ofSetColor(_color);

		
        ofPushMatrix();
        ofTranslate(_pos.x,70);
		
		float dw_=0;
		float dh_=0;

		float src1=-_size.y*1.5;
		float src2=-_size.y;
		float dest=_size.y*.2;

		if(_pos.y<src2){			
			float pp_=(_pos.y-src1)/(src2-src1);
			dw_=_size.x*ofLerp(1.2,1.5,pp_);
			dh_=_size.y*ofLerp(0,.5,pp_);
		}else if(_pos.y<dest){			
			float pp_=(_pos.y-src2)/(dest-src2);
			dw_=_size.x*ofLerp(1.5,0,pp_);
			dh_=_size.y*ofLerp(.5,1.5,pp_);
		}
		ofDrawEllipse(0,0,dw_,dh_);
		
		ofTranslate(-_size.x/2,_pos.y);
		_shape_drop.draw();       

        ofPopMatrix();
        ofPopStyle();
    }
    void update(float dt_){
         _pos+=_vel;
        _vel+=_acc;
    }
    bool isDead(){
        //return _timer.val()==1;
        return _pos.y>_hei;
    }
    
};
class PJuice{
    
    //ofColor _color;
    FrameTimer _timer_fill,_timer_move,_timer_drop;
    
    //ofMesh _mesh;
    ofVec2f _pos,_size;
    int _mwave;
    list<ofColor> _color;
    list<PDrop> _drop;
    
public:
	


    PJuice(){
        _pos=ofVec2f(0,ofGetHeight());
        
        _timer_fill=FrameTimer(FRUIT_GROUP_INTERVAL);
//        _timer_fill.setContinuous(true);
        _timer_fill.restart();
        
        _timer_drop=FrameTimer(JUICE_DROP_INTERVAL);
//        _timer_drop.setContinuous(true);
        _timer_drop.restart();
        
        _timer_move=FrameTimer(FRUIT_RAIN_INTERVAL*4);
        _timer_move.setContinuous(true);
        _timer_move.restart();
        
        _size=ofVec2f(ofGetHeight(),ofGetHeight()/8);
    }
    void reset(){
        _mwave=0;
    }
    void addWave(ofColor col_){
        
        if(_mwave>=8) return;
            
        _color.push_back(col_);
        _timer_fill.restart();
        _mwave++;
        
        if(_mwave>8) _timer_move.restart();
    }
    void draw(){
        ofPushStyle();
        
        ofPushMatrix();
        ofTranslate(_pos);
        if(_mwave>8) ofTranslate(0,_size.y*(max(_mwave-9,0)+_timer_move.valEaseOut()));
        
        list<ofPoint> tmp_;
        tmp_.push_back(ofPoint(0,_size.y));
        tmp_.push_back(ofPoint(_size.x,_size.y));
        
        auto it_color=_color.begin();
        
        for(int x=0;x<_mwave;++x){
            
            ofSetColor(*it_color);
            
//            ofTranslate(0,-_size.y);
            ofBeginShape();
            
                for(auto it_=tmp_.rbegin();it_!=tmp_.rend();++it_){
                    ofVertex((*it_).x,(*it_).y);
                }
                tmp_.clear();
            
                float len_=_size.x/(float)JUICE_RESOLUTION;
                float ang_=TWO_PI*JUICE_WAVE_COUNT/(float)JUICE_RESOLUTION;
                float start_ang_=(_timer_move.num()+_timer_move.valEaseInOut())*TWO_PI*(x%2==0?1:-1);
                float h_=(x==_mwave-1)?_size.y*(_timer_fill.valEaseInOut()):_size.y;
            
                for(int i=0;i<=JUICE_RESOLUTION;++i){
                    ofPoint pt_(i*len_,-_size.y*x-h_+sin(start_ang_+ang_*i)*_size.y*JUICE_WAVE_AMP);
                    ofVertex(pt_.x,pt_.y);
                    tmp_.push_back(pt_);
                }
            
            ofEndShape();
            
            it_color++;
        }
        
        ofPopMatrix();
        
        for(auto& d:_drop) d.draw();
        
        ofPopStyle();
    }
    void update(float dt_){
        _timer_fill.update(dt_);
        _timer_move.update(dt_);
        _timer_drop.update(dt_);
        
        for(auto& d:_drop) d.update(dt_);
        if(_color.size()>0 && _timer_drop.val()==1){
            _drop.push_back(PDrop(_color.back(),ofGetHeight()-_mwave*_size.y));
            _timer_drop.restart();
        }
        _drop.erase(remove_if(_drop.begin(),_drop.end(),
                               [&](PDrop p)->bool {return p.isDead();}),_drop.end());
        
    }
    
};

#endif