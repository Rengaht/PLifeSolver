#pragma once
#ifndef PFRUIT_RAIN_H
#define PFRUIT_RAIN_H

#define FRUIT_GROUP 8
#define FRUIT_BORDER 50
#define FRUIT_VEL 5
//#define FRUIT_ACC 0
#define FRUIT_DENSITY 5
#define FRUIT_SCALE .8

//#define FRUIT_RAIN_INTERVAL 600
//#define FRUIT_GROUP_INTERVAL 5000

#define MFRUIT_GROUP_ROW 8

#include "ofMain.h"
#include "FrameTimer.h"
#include "PJuice.h"



class PFruit{

public:
	//int _index;
	ofVec2f _pos,_vel,_acc;
	float _ang;
	ofImage* _img;
	ofVec2f _size;

	PFruit(ofImage* img_,ofVec2f pos_,float ang_){
		
		//_index=idx_;
		_img=img_;
		_size=ofVec2f(img_->getWidth(),img_->getHeight());
		_ang=ang_;

		_pos=pos_;
		_vel=ofVec2f(0,FRUIT_VEL);
		//_acc=ofVec2f(0,FRUIT_ACC);
	}
	void update(float dy_){
		
		_pos.y+=dy_;
		//_pos+=_vel;
		//_vel+=_acc;

		//if(ofRandom(25)<1) _acc.x+=ofRandom(-.01,.01)*FRUIT_ACC;
	}
	void draw(){
		ofPushStyle();
		ofPushMatrix();
		ofTranslate(_pos);
		
		ofTranslate(_size.x/2,_size.y/2);
		ofRotate(_ang+_vel.angle(ofVec2f(0,1)));
		ofTranslate(-_size.x/2,-_size.y/2);

		ofScale(FRUIT_SCALE,FRUIT_SCALE);
			_img->draw(0,0);
		ofPopMatrix();
		ofPopStyle();
	}
	bool isDead(){
		return (_pos.y>ofGetHeight()+_size.y);
	}
	bool isDropped(){
		return (_pos.y>_size.y);
	}

};

class PFruitRain{

	FrameTimer _timer_rain,_timer_group;
	list<PFruit> _fruit;

	PJuice _juice;
	float _vel_drop;

public:
	static ofColor _color[];
	list<int> _idx_fruit[FRUIT_GROUP];
	list<ofImage> _img_fruit[FRUIT_GROUP];
	
	int _idx_group;
	bool _playing;
	bool _slow;
	bool _auto_fruit;

	PFruitRain(){
		
		loadImage();

		reset();
		//_timer_rain=FrameTimer(FRUIT_RAIN_INTERVAL);
		//_timer_rain.setContinuous(true);
		//ofAddListener(_timer_rain.finish_event,this,&PFruitRain::onRainFinish);


		//_timer_group=FrameTimer(FRUIT_GROUP_INTERVAL);
		
	}
	void loadImage(){
		
		_idx_fruit[0]=list<int>({1});
		_idx_fruit[1]=list<int>({10});
		_idx_fruit[2]=list<int>({3});
		_idx_fruit[3]=list<int>({4});
		_idx_fruit[4]=list<int>({5});
		_idx_fruit[5]=list<int>({6});
		_idx_fruit[6]=list<int>({7});
		_idx_fruit[7]=list<int>({8});

		for(int i=0;i<FRUIT_GROUP;++i){
			for(auto &t:_idx_fruit[i]){
				_img_fruit[i].push_back(ofImage("_img_fruit/j"+ofToString(t)+".png"));
			}
		}

		ofxSVG svg_;
		svg_.load("_img_ui/drop.svg");		
		PDrop::_shape_drop.append(svg_.getPathAt(0));
		PDrop::_shape_drop.setUseShapeColor(false);
		PDrop::_shape_drop.setFilled(true);

		PDrop::_size=ofVec2f(svg_.getWidth(),svg_.getHeight());
	}
	void reset(){
		_idx_group=floor(ofRandom(MJUICE_RESULT));
		_vel_drop=FRUIT_VEL;

		_fruit.clear();		
		_juice.reset();

        setSlow(true);
		setAutoFruit(true);
	}
	void start(){
		/*_timer_rain.restart();
		_timer_group.restart();*/
		_playing=true;

		addFruit(_idx_group);
	}
	void stop(){
		/*_timer_rain.stop();
		_timer_group.stop();*/
		_playing=false;
	}
	void addFruit(int index_){

		float grid_=(ofGetHeight()+FRUIT_BORDER*2)/(float)FRUIT_DENSITY;
		
		auto it=_img_fruit[_idx_group].begin();
		float fw_=(*it).getWidth();
		float fh_=(*it).getHeight();
		
		for(int j=0;j<MFRUIT_GROUP_ROW;++j){
			for(int i=0;i<=FRUIT_DENSITY;++i){
				
				if(_slow && ofRandom(2)<1)  continue;
				if(!_slow && ofRandom(5)<1) continue;

				_fruit.push_back(PFruit(&(*it),ofVec2f(-WIN_HEIGHT/2+(i)*grid_-FRUIT_BORDER+(j%2==0?-fw_:0),
												-(fh_*FRUIT_SCALE)*(j+(j+1)*.2)),(ofRandom(2)<1?45:-45)));
			}
		}
	}

	void update(float dt_){

		for(auto& f:_fruit) f.update(_vel_drop);
		_fruit.erase(remove_if(_fruit.begin(),_fruit.end(),
								[&](PFruit p)->bool {return p.isDead();}),_fruit.end());

		_juice.update(dt_);

		if(!_slow){
			if(_vel_drop<FRUIT_VEL*2) _vel_drop+=.5;
		}

		//check if need new fruit
		if(_fruit.size()>0 && _fruit.back().isDropped()){
			if(_auto_fruit){
				(++_idx_group)%=FRUIT_GROUP;
				if(!_slow){
					_juice.addWave(ofColor(_color[_idx_group],200));
				}
			}
			addFruit(_idx_group);
		}		
	}
	

	void draw(){
		
		for(auto& f:_fruit) f.draw();
		if(!_slow) _juice.draw();
	}
	/*void onRainFinish(int &e){
		addNewFruit();
	}*/
	void setSlow(bool set_){
        _slow=set_;
        if(!set_){
            _juice.addWave(ofColor(_color[_idx_group],120));			
        }
    }

	void setAutoFruit(bool set_){
		_auto_fruit=set_;
	}
	void setFruit(int set_){
		_idx_group=set_;
	}
};



#endif
