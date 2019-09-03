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
       
    void threadedFunction() {    
        while(isThreadRunning()) {
            if(!q.empty()){
                QueuedImage i = q.front();
                ofSaveImage(i.image, i.fileName);
                q.pop();           
				_save_count++;
				
				if(counter==_save_count){
					ofNotifyEvent(recordFinish,counter,this);
					counter=0;
					_save_count=0;
					//stopThread();
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