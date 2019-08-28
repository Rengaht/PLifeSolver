#pragma once

#define DRAW_DEBUG
#define CAM_WIDTH 1920
#define CAM_HEIGHT 1080

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#include "ofMain.h"

#include "ofxCv.h"
#include "ofxHttpUtils.h"
#include "ofxJSON.h"
#include "PSceneBase.h"

using namespace cv;
using namespace ofxCv;

class ofApp : public ofBaseApp{

	public:

		/* scene */
		enum PStage {PSLEEP,PAUTH,PDETECT,PANALYSIS,PRESULT,PEMPTY};
		PStage _stage,_stage_pre,_stage_next;
		SceneBase* _scene[5];

		void loadScene();
		void onSceneInFinish(int &e);
		void onSceneOutFinish(int &e);
		void prepareScene(PStage set_);

		/* camera & face */
		void setupCamera();
		void setCameraPause(bool set_);
		bool _camera_paused;

		void sendFaceRequest();
		ofxHttpUtils _http_utils;
		void urlResponse(ofxHttpResponse & response);

		bool faceFound();
		void drawFaceFrame();


		/* basic */

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void mouseReleased(int x, int y, int button);


	private:

		float _millis_now;

		ofImage _img_frame,_img_mask;
		void drawOutFrame();

		ofVideoGrabber _camera;
        ofxCv::ObjectFinder _finder;

		void setStage(PStage set_);

};
