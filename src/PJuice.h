#pragma once
#ifndef PJUICE_H
#define PJUICE_H

#include "ofxSvg.h"

//#define DROP_VEL 0
//#define DROP_ACC .5

//#define MJUICE_ROW 5
//#define JUICE_INTERVAL 3000
#define JUICE_MOVE_INTERVAL 1000

#define JUICE_RESOLUTION 150
#define JUICE_WAVE_COUNT 4
#define JUICE_WAVE_AMP 0.1
//#define JUICE_DROP_INTERVAL 500



class PDrop{
    
    ofColor _color;    
    ofVec2f _pos,_vel,_acc;
    float _hei;
    
public:
	
	static ofPath _shape_drop;
	static ofVec2f _size;

    PDrop(ofColor col_,float hei_){
        _color=col_;

        _pos=ofVec2f(0,-_size.y*1.5);
        //_size=ofVec2f(w_/15,w_/10);
        
        _hei=hei_;
        
        _vel=ofVec2f(0);
        _acc=ofVec2f(0);
    }
    void draw(float alpha_){
        ofPushStyle();
        ofSetColor(_color,180*alpha_);

		
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
    FrameTimer _timer_fill,_timer_move_wave,_timer_drop,_timer_move;
    
    //ofMesh _mesh;
    ofVec2f _pos,_size;
    int _mwave;
    list<ofColor> _color;
    list<PDrop> _drop;
	
	FrameTimer _timer_fade_in,_timer_fade_out;
    
public:
	


    PJuice(){
        _pos=ofVec2f(-WIN_HEIGHT/2,WIN_HEIGHT);
        
        _timer_fill=FrameTimer(PParam::val()->JuiceTime);
        _timer_fill.restart();
        
        _timer_drop=FrameTimer(PParam::val()->JuiceDropTime);
        _timer_drop.restart();
        
		_timer_move_wave=FrameTimer(JUICE_MOVE_INTERVAL);
		_timer_move_wave.setContinuous(true);
		_timer_move_wave.restart();

        _timer_move=FrameTimer(PParam::val()->JuiceTime*.4);
       

        _size=ofVec2f(WIN_HEIGHT,WIN_HEIGHT/PParam::val()->JuiceWaveRow);
    }
    void reset(){
        _mwave=0;
    }
    void addWave(ofColor col_){
        
        if(_mwave>=8) return;
            
        _color.push_back(col_);
        _timer_fill.restart();
        _mwave++;
        
        if(_mwave>PParam::val()->JuiceWaveRow-1){
			_timer_move.restart();
		}
    }
    void draw(float alpha_){
        ofPushStyle();
		
        
        ofPushMatrix();
        ofTranslate(_pos);
        ofTranslate(0,_size.y*_timer_move.valEaseOut());
        
        list<ofPoint> tmp_;
        tmp_.push_back(ofPoint(0,_size.y));
        tmp_.push_back(ofPoint(_size.x,_size.y));
        
        auto it_color=_color.begin();
        
        for(int x=0;x<_mwave;++x){
            			
            ofSetColor(*it_color,alpha_*180);
            
//            ofTranslate(0,-_size.y);
            ofBeginShape();
            
                for(auto it_=tmp_.rbegin();it_!=tmp_.rend();++it_){
                    ofVertex((*it_).x,(*it_).y);
                }
                tmp_.clear();
            
                float len_=_size.x/(float)JUICE_RESOLUTION;
                float ang_=TWO_PI*JUICE_WAVE_COUNT/(float)JUICE_RESOLUTION;
                float start_ang_=(_timer_move_wave.valEaseInOut())*TWO_PI*(x%2==0?1:-1);
                float h_=(x==_mwave-1)?_size.y*(_timer_fill.valEaseOut()):_size.y;
            
                for(int i=0;i<=JUICE_RESOLUTION;++i){
                    ofPoint pt_(i*len_,-_size.y*x-h_+sin(start_ang_+ang_*i)*_size.y*JUICE_WAVE_AMP);
                    ofVertex(pt_.x,pt_.y);
                    tmp_.push_back(pt_);
                }
            
            ofEndShape();
            
            it_color++;
        }
        
        ofPopMatrix();
        
        for(auto& d:_drop) d.draw(alpha_);
        
        ofPopStyle();
    }
    void update(float dt_){
        _timer_fill.update(dt_);
        
		_timer_move_wave.update(dt_);

		_timer_move.update(dt_);
		if(_timer_move.val()==1){
			//_pos.y+=_size.y;
			_timer_move.reset();
			_color.pop_front();
			_mwave--;
		}

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