#pragma once
#ifndef PARMAMETR_H
#define PARAMETER_H

#include <codecvt>

#include "ofMain.h"
#include "ofxXmlSettings.h"

const string PARAMETER_PATH="param.xml";
const string CVVAL_PATH="cv_val.xml";

class PParam{

	
public:
	static PParam* _instance;	
	
	string FolderExport;
	enum PJuice {RED_DRAGON,
				HONEY_LEMON,
				VEGETABLE,
				BEETROOT,
				CARROT,
				COCONUT,
				PINEAPPLE,
				ORANGE_PASSION,
				EMPTY};
		
	
	int GifLength;
	int GifFps;
	int GifSize;

	//int JuiceCount;
	float FruitStartVel;
	float FruitDensity;
	float FruitScale;
	int FruitRowPerGroup;

	int JuiceWaveRow;
	int JuiceTime;
	int JuiceDropTime;

	int QrcodeTime;
	int CardTime;

	float BgdDetectScale;
	float BgdLearningTime;
	float BgdThreshold;
	float ContourThreshold;
	float ContourSmoothed;

	string DeleteCmd;
	string FFmpegCmd;
	string FFmpegFilter;

	list<int> JuiceChannel[FRUIT_GROUP];
	map<int,string> ChannelCmd;

	string JandiWebhookIn;
	string JandiWebhookOut;

	int SerialPort;

	float CameraScale;
	ofVec2f CameraPosition;

	float ScreenScale;
	ofVec2f ScreenPosition;

	PParam(){
		readParam();
		readCVParam();

	}
	static PParam* val(){
		if(!_instance)
			_instance=new PParam();
		return _instance;
	}
	void readParam(){


		ofxXmlSettings _param;
		bool file_exist=true;
		if(_param.loadFile(ofToDataPath(PARAMETER_PATH))){
			ofLog()<<PARAMETER_PATH<<" loaded!";
		}else{
			ofLog()<<"Unable to load "<<PARAMETER_PATH<<" check data/ folder";
			file_exist=false;
		}
        
		GifLength=_param.getValue("GifLength",5);
		GifFps=_param.getValue("GifFps",12);
		GifSize=_param.getValue("GifSize",700);
		
		//JuiceCount=_param.getValue("JuiceCount",8);
		FruitStartVel=_param.getValue("FruitStartVel",5);
		FruitDensity=_param.getValue("FruitDensity",5);
		FruitScale=_param.getValue("FruitScale",.8);
		FruitRowPerGroup=_param.getValue("FruitRowPerGroup",8);

		JuiceWaveRow=_param.getValue("JuiceWaveRow",5);
		JuiceTime=_param.getValue("JuiceTime",5);
		JuiceDropTime=_param.getValue("JuiceDropTime",5);

		QrcodeTime=_param.getValue("QrcodeTime",20000);
		CardTime=_param.getValue("CardTime",2000);

		BgdDetectScale=_param.getValue("BgdDetectScale",0.2);
		BgdLearningTime=_param.getValue("BgdLearningTime",5);
		BgdThreshold=_param.getValue("BgdThreshold",10);

		DeleteCmd=_param.getValue("DelCmd","");
		FFmpegCmd=_param.getValue("FFmpegCmd","");
		FFmpegFilter=_param.getValue("FFmpegFilter","");

		SerialPort=_param.getValue("SerialPort",0);
		JandiWebhookIn=_param.getValue("JandiWebhookIn","");
		JandiWebhookOut=_param.getValue("JandiWebhookOut","");


		_param.pushTag("SerialCmd");
		readChannel(_param,"RedDragon",RED_DRAGON);
		readChannel(_param,"HoneyLemon",HONEY_LEMON);
		readChannel(_param,"Vegetable",VEGETABLE);
		readChannel(_param,"Beetroot",BEETROOT);
		readChannel(_param,"Carrot",CARROT);
		readChannel(_param,"Coconut",COCONUT);
		readChannel(_param,"Pineapple",PINEAPPLE);
		readChannel(_param,"OrangePassion",ORANGE_PASSION);

		
		//if(!file_exist) saveParameterFile();

	
	}	
	void readCVParam(){
			ofxXmlSettings _param;
		bool file_exist=true;
		if(_param.loadFile(ofToDataPath(CVVAL_PATH))){
			ofLog()<<CVVAL_PATH<<" loaded!";
		}else{
			ofLog()<<"Unable to load "<<CVVAL_PATH<<" check data/ folder";
			file_exist=false;
		}
        
		BgdLearningTime=_param.getValue("BgdLearningTime",5);
		BgdThreshold=_param.getValue("BgdThreshold",10);
		ContourThreshold=_param.getValue("ContourThreshold",200);
		ContourSmoothed=_param.getValue("ContourSmoothed",200);

		CameraScale=_param.getValue("CameraScale",1.0);
		CameraPosition.x=_param.getValue("CameraPositionX",0.0);
		CameraPosition.y=_param.getValue("CameraPositionY",0.0);

		ScreenScale=_param.getValue("ScreenScale",1.0);
		ScreenPosition.x=_param.getValue("ScreenPositionX",0.0);
		ScreenPosition.y=_param.getValue("ScreenPositionY",0.0);
		
		if(!file_exist) saveParameterFile();
	}
	void readChannel(ofxXmlSettings p_,string tag_,int index_){
		int m=p_.getNumTags(tag_);
		for(int i=0;i<m;++i){
			int channel_=p_.getValue(tag_,0,i);
			JuiceChannel[index_].push_back(channel_);

			if(ChannelCmd.find(channel_)==ChannelCmd.end()){			
				ChannelCmd[channel_]=p_.getAttribute(tag_,"cmd","",i);
			}
		}
	}

	void saveParameterFile(){


		ofxXmlSettings _param;
      
  
		_param.setValue("BgdLearningTime",BgdLearningTime);
		_param.setValue("BgdThreshold",BgdThreshold);
		_param.setValue("ContourThreshold",ContourThreshold);
		_param.setValue("ContourSmoothed",ContourSmoothed);
		
		_param.setValue("CameraPositionX",CameraPosition.x);
		_param.setValue("CameraPositionY",CameraPosition.y);
		_param.setValue("CameraScale",CameraScale);


		_param.setValue("ScreenPositionX",ScreenPosition.x);
		_param.setValue("ScreenPositionY",ScreenPosition.y);
		_param.setValue("ScreenScale",ScreenScale);


		_param.save(CVVAL_PATH);


	}
	
};





#endif