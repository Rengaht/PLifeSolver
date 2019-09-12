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

	ofEvent<int> recordFinish;
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
	void setJuiceID(int set_){
		_juice_id=set_;
	}
       
    void threadedFunction() {    
        while(isThreadRunning()) {
            if(!q.empty()){
                QueuedImage i = q.front();
                ofSaveImage(i.image, i.fileName);
                q.pop();           
				_save_count++;
				
				if(counter==_save_count){
					
					counter=0;
					_save_count=0;
					//stopThread();
					string cmd="\"C:\\Program Files\\ImageMagick-7.0.8-Q16\\magick.exe\" "+ofToDataPath("tmp/")+"*.png "
							+"-reverse "+ofToDataPath("tmp/")+"*.png -loop 0 "
							+ofToDataPath("output/")+_user_id+".gif";
					//ofLog()<<cmd;
					ofSystem(cmd);

					string cmd2="\"C:\\Program Files\\ImageMagick-7.0.8-Q16\\magick.exe\" convert "+ofToDataPath("output/")+_user_id+".gif"
								+" -coalesce null: "+ofToDataPath("_img_ui/share/")+ofToString(_juice_id+1)+".png"
								+" -gravity center -layers composite -layers optimize "
								+ofToDataPath("output/")+_user_id+".gif";
					ofLog()<<cmd2;
					ofSystem(cmd2);

					ofNotifyEvent(recordFinish,counter,this);
				}
			}
        }
        
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
};  

#endif