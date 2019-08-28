#pragma once
#ifndef SCENE_BASE_H
#define SCENE_BASE_H

#define SLEEP_TIME 30000
#define EASE_DUE 1500
#define EASE_OFFSET 300

#include "ofMain.h"
#include "FrameTimer.h"

class ofApp;


class SceneBase{

public:

	static ofEvent<int> sceneInFinish;
	static ofEvent<int> sceneOutFinish;

	static FrameTimer _timer_sleep;
	static ofSoundPlayer SoundButton;


	int _order_scene;

	enum SceneStatus {Init, Due, End};
	SceneStatus _status;

	ofApp* _ptr_app;

	SceneBase(ofApp *set_ptr);
	~SceneBase(){};

	void setup();

	virtual void draw();
	virtual void drawLayer(int i);

	virtual void init();
	virtual void end();
	virtual void update(float dt_);
	virtual void reset();
	bool _trigger_in,_trigger_out;

	//virtual bool handleMouse(float mx_,float my_);
	//virtual void buttonEvent(int index);

	//vector<ofRectangle> _button;
	//bool* _enable_button;

	vector<FrameTimer> _timer_in;
	vector<FrameTimer> _timer_out;
	int _mlayer;
    
    float getLayerAlpha(int layer_);


};



#endif