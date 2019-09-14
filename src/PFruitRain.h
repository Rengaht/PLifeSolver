#pragma once
#ifndef PFRUIT_RAIN_H
#define PFRUIT_RAIN_H

#define FRUIT_GROUP 8
#define FRUIT_BORDER 50
#define FRUIT_VEL 3
#define FRUIT_ACC 0.01
#define FRUIT_DENSITY 8
#define FRUIT_SCALE 1

#define FRUIT_RAIN_INTERVAL 600
#define FRUIT_GROUP_INTERVAL 5000

#include "ofMain.h"
#include "FrameTimer.h"
#include "PJuice.h"




class PFruit{

public:
	int _index;
	ofVec2f _pos,_vel,_acc;
	float _ang;
	ofImage* _img;

	PFruit(int idx_,ofImage* img_,ofVec2f pos_,float ang_){
		
		_index=idx_;
		_img=img_;
		_ang=ang_;

		_pos=pos_;
		/*_vel=ofVec2f(0,ofRandom(.8,1.2)*FRUIT_VEL);
		_acc=ofVec2f(FRUIT_ACC*ofRandom(-.2,.2),FRUIT_ACC*ofRandom(0.9,1.1));*/
		_vel=ofVec2f(0,FRUIT_VEL);
		_acc=ofVec2f(0,FRUIT_ACC);
	}
	void update(float dt_){
		_pos+=_vel;
		_vel+=_acc;

		if(ofRandom(25)<1) _acc.x+=ofRandom(-.01,.01)*FRUIT_ACC;
	}
	void draw(){
		ofPushStyle();
		ofPushMatrix();
		ofTranslate(_pos);
		ofRotate(_ang+_vel.angle(ofVec2f(0,1)));

		ofScale(FRUIT_SCALE,FRUIT_SCALE);
			_img->draw(0,0);
		ofPopMatrix();
		ofPopStyle();
	}
	bool isDead(){
		return (_pos.y>ofGetHeight()+FRUIT_BORDER*2);
	}

};

class PFruitRain{

	FrameTimer _timer_rain,_timer_group;
	list<PFruit> _fruit;

	PJuice _juice;

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
		addNewFruit();
		_timer_rain=FrameTimer(FRUIT_RAIN_INTERVAL);
		//_timer_rain.setContinuous(true);
		//ofAddListener(_timer_rain.finish_event,this,&PFruitRain::onRainFinish);


		_timer_group=FrameTimer(FRUIT_GROUP_INTERVAL);
		
	}
	void loadImage(){
		/*_idx_fruit[0]=list<int>({5,6,15});
		_idx_fruit[1]=list<int>({8,16});
		_idx_fruit[2]=list<int>({10,11});
		_idx_fruit[3]=list<int>({18});
		_idx_fruit[4]=list<int>({2,9,17});
		_idx_fruit[5]=list<int>({1,3,4});
		_idx_fruit[6]=list<int>({7,12});
		_idx_fruit[7]=list<int>({1,2,3,4});*/
		
		_idx_fruit[0]=list<int>({1});
		_idx_fruit[1]=list<int>({9,10});
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
		_idx_group=0;
		
		_fruit.clear();		
		_juice.reset();

        setSlow(true);
		setAutoFruit(true);
	}
	void start(){
		_timer_rain.restart();
		_timer_group.restart();
		_playing=true;
	}
	void stop(){
		_timer_rain.stop();
		_timer_group.stop();
		_playing=false;
	}
	void addNewFruit(){

		float grid_=(ofGetHeight()+FRUIT_BORDER*2)/(float)FRUIT_DENSITY;
		

		for(int i=0;i<=FRUIT_DENSITY;++i){

			if(_timer_rain.num()%2==0){
				if(i%2==0) continue;
			}else{
				if(i%2==1) continue;
			}
			int idx_=i%_idx_fruit[_idx_group].size();//floor(ofRandom(_idx_fruit[_idx_group].size()));
			auto it=_img_fruit[_idx_group].begin();
			advance(it,idx_);
			/*_fruit.push_back(PFruit(idx_,&(*it),
									ofVec2f((i+ofRandom(-.1,.1))*grid_-FRUIT_BORDER,
											-(*it).getHeight()*FRUIT_SCALE*ofRandom(1,3))));*/
			_fruit.push_back(PFruit(idx_,&(*it),
									ofVec2f((i)*grid_-FRUIT_BORDER,
											-(*it).getHeight()),(i%2==0?45:-45)));
		}
	}

	void update(float dt_){

		for(auto& f:_fruit) f.update(dt_);
		_fruit.erase(remove_if(_fruit.begin(),_fruit.end(),
								[&](PFruit p)->bool {return p.isDead();}),_fruit.end());

		_juice.update(dt_);
		_timer_rain.update(dt_);
		if(_timer_rain.val()==1){
			addNewFruit();
			_timer_rain.restart();
		}


		if(_auto_fruit){
			_timer_group.update(dt_);
			if(_timer_group.val()==1){
				(++_idx_group)%=FRUIT_GROUP;
				_timer_group.restart();

				if(!_slow){
					_juice.addWave(ofColor(ofRandom(255),ofRandom(255),ofRandom(255),200));
				}
			}
		}

		
	}
	

	void draw(){
		
		for(auto& f:_fruit) f.draw();
		if(!_slow) _juice.draw();
	}
	void onRainFinish(int &e){
		addNewFruit();
	}
	void setSlow(bool set_){
        _slow=set_;
        if(set_){
            _timer_rain.setDue(FRUIT_RAIN_INTERVAL*2);
			_juice.addWave(ofColor(_color[_idx_group],120));
        }else _timer_rain.setDue(FRUIT_RAIN_INTERVAL);
    }

	void setAutoFruit(bool set_){
		_auto_fruit=set_;
	}
	void setFruit(int set_){
		_idx_group=set_;
	}
};



#endif
