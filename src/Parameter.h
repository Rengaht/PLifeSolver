#pragma once
#ifndef PARMAMETR_H
#define PARAMETER_H

#include <codecvt>

#include "ofMain.h"
#include "ofxXmlSettings.h"

const string PARAMETER_PATH="data\\param.xml";

class PParam{

	
public:
	static PParam* _instance;	
	
	string FolderExport;
	
	
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

	string FFmpegCmd;
	string FFmpegFilter;

	PParam(){
		readParam();

	}
	static PParam* val(){
		if(!_instance)
			_instance=new PParam();
		return _instance;
	}
	void readParam(){


		ofxXmlSettings _param;
		bool file_exist=true;
		if(_param.loadFile(PARAMETER_PATH) ){
			ofLog()<<PARAMETER_PATH<<" loaded!";
		}else{
			ofLog()<<"Unable to load xml check data/ folder";
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

		FFmpegCmd=_param.getValue("FFmpegCmd","");
		FFmpegFilter=_param.getValue("FFmpegFilter","");

		if(!file_exist) saveParameterFile();

	
	}	
	void saveParameterFile(){


		ofxXmlSettings _param;
      
  
		
		_param.setValue("GifLength",GifLength);
		_param.setValue("GifFps",GifFps);
		_param.setValue("GifSize",GifSize);

		//_param.setValue("JuiceCount",JuiceCount);
		_param.setValue("FruitStartVel",FruitStartVel);
		_param.setValue("FruitDensity",FruitDensity);
		_param.setValue("FruitScale",FruitScale);
		_param.setValue("FruitRowPerGroup",FruitRowPerGroup);

		_param.setValue("JuiceWaveRow",JuiceWaveRow);
		_param.setValue("JuiceTime",JuiceTime);
		_param.setValue("JuiceDropTime",JuiceDropTime);


		_param.setValue("QrcodeTime",QrcodeTime);
		_param.setValue("CardTime",CardTime);


		_param.setValue("BgdDetectScale",BgdDetectScale);
		_param.setValue("BgdLearningTime",BgdLearningTime);
		_param.setValue("BgdThreshold",BgdThreshold);

		_param.setValue("FFmpegCmd",FFmpegCmd);
		_param.setValue("FFmpegFilter",FFmpegFilter);


		_param.save(PARAMETER_PATH);


	}
	
};





#endif