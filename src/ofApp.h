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
		enum PJuice {RED_DRAGON,HONEY_LEMON,VEGETABLE,BEETROOT,CARROT,COCONUT,PINEAPPLE,ORANGE_PASSION};
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
		void parseFaceData(string data_);
		int getJuiceFromEmotion(ofxJSONElement emotion_);
		
		PJuice getJuice(string mood_);
		bool checkJuiceStorage(PJuice get_);



		ofEvent<int> _event_recieve_emotion;


		bool faceFound();
		void drawFaceFrame();

		vector<ofRectangle> _rect_face;
		ofxJSONElement _user_data;

		string _user_id;
		void createUserID();

		/* basic */

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void mouseReleased(int x, int y, int button);

		ofImage _img_number[5];

	private:

		float _millis_now;

		ofImage _img_frame,_img_mask;
		void drawOutFrame();

		ofVideoGrabber _camera;
        ofxCv::ObjectFinder _finder;

		void setStage(PStage set_);

};
