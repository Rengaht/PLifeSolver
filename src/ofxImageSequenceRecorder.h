#pragma once
#ifndef OFXIMAGE_SEQUENCE_RECORDER_H
#define OFXIMAGE_SEQUENCE_RECORDER_H
#include "ofMain.h"    

typedef struct { 
    string fileName;    
    ofPixels image;
} QueuedImage;

class ofxImageSequenceRecorder : public ofThread {    
public:    
  
    int counter;  
    queue<QueuedImage> q;
    string prefix;
    string format;
    int numberWidth;
	
	string _user_id;
	int _juice_id;

	int _finish_frame;

	ofEvent<int> recordFinish,gifFinish;
	int _save_count;
      
    ofxImageSequenceRecorder(){  
        counter=0;  
        numberWidth=4;
        
    }  
    
    void setPrefix(string pre){
        prefix = pre;
    }
    
    void setFormat(string fmt){
        format = fmt;
    }
       
    void setCounter(int count){
        counter = count;
    }
       
    void setNumberWidth(int nbwidth){
        numberWidth = nbwidth;
    }
	void setUserID(string set_){
		_user_id=set_;
	}
	/*void setJuiceID(int set_){
		_juice_id=set_;
	}*/
       
    void threadedFunction() {    
        while(isThreadRunning()) {
            if(!q.empty()){
                QueuedImage i = q.front();
                ofSaveImage(i.image, i.fileName);
                q.pop();           
				_save_count++;
				
				if(counter>0 && _finish_frame==_save_count){
					
					counter=0;
					_save_count=0;
					ofNotifyEvent(recordFinish,counter,this);
				}
			}
        }
        
    }   
	void createGif(int set_){
		
		_juice_id=set_;

		string cmd=PParam::val()->FFmpegCmd
			+" -framerate "+ofToString(PParam::val()->GifFps)
			+" -i "+ofToDataPath("tmp/")+_user_id+"_%4d.png"
			+" -i "+ofToDataPath("juice/")+ofToString(_juice_id+1)+"/%4d.png "
			+PParam::val()->FFmpegFilter+" "
			+ofToDataPath("output/")+_user_id+".gif";
		ofLog()<<cmd;
		ofSystem(cmd);
		ofLog()<<"----------- finish create gif ------------";
		ofNotifyEvent(gifFinish,counter,this);
	}
    void addFrame(ofImage &img){
        addFrame(img.getPixelsRef());
    }
    
    void addFrame(ofVideoGrabber &cam){
        addFrame(cam.getPixelsRef());
    }
    
    void addFrame(ofVideoPlayer &player){
        addFrame(player.getPixelsRef());
    }
        
    void addFrame(ofPixels imageToSave) {  

        
        
        //char fileName[100]; 
        //snprintf(fileName,  "%s%.4i.%s" , prefix.c_str(), counter, format.c_str());     
        string fileName = prefix + ofToString(counter, numberWidth, '0') + "." + format;
        counter++;   
        
        QueuedImage qImage;
        
        qImage.fileName = fileName;    
        qImage.image = imageToSave; 
        
        q.push(qImage);
        
    }    
	void setFinishFrame(int set_){
		_finish_frame=set_;
	}
	
};  

#endif