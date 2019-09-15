#pragma once

#define DRAW_DEBUG
#define CAM_WIDTH 1920
#define CAM_HEIGHT 1080

#define WIN_WIDTH 1920
#define WIN_HEIGHT 1080

#define FRUIT_COUNT 18
#define MJUICE_RESULT 8

#define GIF_FPS 12
#define GIF_HEIGHT 700
#define GIF_LENGTH 5

#include "ofMain.h"

#include "ofxCv.h"
#include "ofxHttpUtils.h"
#include "ofxJSON.h"
#include "ofxQRCodeGenerator.h"

#include "ofxImageSequenceRecorder.h"
#include "PSceneBase.h"
#include "PFruitRain.h"


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
		void sendJuiceSignal(int send_);
		ofSerial _serial;
		void setupSerial();


		ofEvent<int> _event_recieve_emotion;


		bool faceFound();
		void drawFaceFrame();
		void drawContour();
		ofFbo _fbo_contour;

		vector<ofRectangle> _rect_face;
		ofxJSONElement _user_data;

		string _user_id;
		int _idx_user_juice;

		void createUserID();

		
		PFruitRain _fruit_rain;
		

		bool _recording;
		FrameTimer _timer_record;
		int _idx_record;
		void setRecord(bool set_);

		/* share */
		bool _face_analysis_ready;
		
		void uploadImage(string id_);
		void createQRcode(string url_);
		ofxQRCodeGenerator _qrcode_gen;

		/* basic */

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void mouseReleased(int x, int y, int button);
		void exit();
	

	private:

		float _millis_now;

		ofImage _img_frame,_img_mask;
		void drawOutFrame();

		ofVideoGrabber _camera;
        ofxCv::ObjectFinder _finder;
		ofxCv::ContourFinder _contour_finder;

		void setStage(PStage set_);

		
		ofImage _img_share_frame,_img_share_text[MJUICE_RESULT];
		ofFbo _fbo_save;
		ofxImageSequenceRecorder _recorder;    
		void saveCameraFrame();
		void onRecorderFinish(int &e);
		void onRecordTimerFinish(int &e);



		void createFruitImage();
};
