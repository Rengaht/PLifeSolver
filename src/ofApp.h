#pragma once

//#define FORCE_MODE
//#define FORCE_JUICE 2  //VEGETABLE
//#define FORCE_JUICE 6  //PINEAPPLE
//#define FORCE_JUICE 0  //RED_DRAGON

#define DRAW_DEBUG
//#define USE_BACKGROUND_SUB
#define MIRROR_CAMERA
//#define WEEKAND_OFF

#define CAM_WIDTH 1920
#define CAM_HEIGHT 1080

#define WIN_WIDTH 1920
#define WIN_HEIGHT 1080
#define FRUIT_GROUP 8

#define CHANNEL_UPDATE_RATE 20

#include "ofMain.h"

#include "ofxCv.h"
#include "ofxHttpUtils.h"
#include "ofxJSON.h"
#include "ofxQRCodeGenerator.h"
#include "ofxLayerMask.h"

#include "Parameter.h"
#include "ofxImageSequenceRecorder.h"
#include "PSceneBase.h"
#include "PFruitRain.h"



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

		enum PSound {SCHECK,SCOUNT,SFINISH,SRESULT,SSHUTTER};
		void playSound(PSound i);

		/* camera & face */
		void setupCamera();
		void setCameraPause(bool set_);
		bool _camera_paused;

		void sendFaceRequest();
		ofxHttpUtils _http_utils;
		void urlResponse(ofxHttpResponse & response);
		bool parseFaceData(string data_);
		int getJuiceFromEmotion(ofxJSONElement emotion_);
		
		PParam::PJuice getJuice(string mood_);
		int checkJuiceStorage(PParam::PJuice get_);
		
		void sendJuiceSignal();
		void sendSleepLight();
		ofSerial _serial;
		void setupSerial();


		ofEvent<int> _event_recieve_emotion;
		ofEvent<int> _event_receive_result;


		bool faceFound();
		void drawFaceFrame();
		//ofFbo _fbo_contour;

		list<ofRectangle> _rect_face;
		ofxJSONElement _user_data;

		string _user_id;
		int _idx_user_juice,_idx_channel;
		string _user_gender;
		int _user_age;

		void createUserID();

		
		ofFbo _fbo_camera;
		ofPixels _pixel_camera;
		void drawCameraFrame();

		PFruitRain _fruit_rain;
		void drawForeground();
		list<ofRectangle> _rect_contour;

		bool _recording;
		FrameTimer _timer_record;
		int _idx_record;
		void setRecord(bool set_);
		bool _recorder_save_finish;

		
		bool _channel_all_empty;

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
	
		void stopBgm();
		void startBgm();

		void sendJandiMessage(string message_);

	private:

		float _millis_now;

		ofImage _img_frame,_img_mask;
		

		ofVideoGrabber _camera;
        ofxCv::ObjectFinder _finder;
		ofxCv::ContourFinder _contour_finder;
		
		ofxCv::RunningBackground _background;
		ofImage _img_threshold;
		//float _bgd_learning_time,_bgd_threshold;
		ofFbo _fbo_bgd_tmp,_fbo_threshold_tmp,_fbo_bgd;
		ofxLayerMask _masker;
		void updateBackground();
		


		void setStage(PStage set_);

		
		ofImage _img_share_frame,_img_share_text[FRUIT_GROUP];
		ofFbo _fbo_save;
		ofxImageSequenceRecorder _recorder;    
		void saveCameraFrame();
		void onRecorderFinish(int &e);
		void onRecordTimerFinish(int &e);
		void onGifFinish(int &e);


		void createFruitImage();

		ofSoundPlayer _soundfx[5];
		ofSoundPlayer _sound_bgm;
		FrameTimer _timer_bgm_in,_timer_bgm_out;

		void updateSound(float dt_);

		vector<int> _status_channel;
		void parseChannelStatus();
		int _parse_index;
		
		int _count_send_channel;
		void sendChannelStatus();
		void getJuiceMapping();
		map<string,vector<PParam::PJuice>> _emotion_map;
		PParam::PJuice getJuiceFromName(string name_);

};
