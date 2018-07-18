#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxJSON.h"


#define RECEIVE_PORT 3334
#define NUM_MSG_STRINGS 20


#define HOST "localhost"
#define SEND_PORT 12000

class ofApp : public ofBaseApp{
    
public:
    
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);

    ofVideoGrabber vidGrabber;
    int camWidth;
    int camHeight;
    
    
    bool record;
    
    //ofxOscReceiver receiver;
    
    
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    
    int mouseX, mouseY;
    string mouseButtonState;
    
    ofImage receivedImage;

    //ofxOscSender
    
    ofxOscSender sender;
    
    
    //osc Receiver
    
    
    void drawParts();
    void drawConnections();
    
    void getHumanFromOSC();
    
    ofxOscReceiver receiver;
    
    // This array will hold the results parsed from the OSC message string
    ofxJSONElement results;
    // This array will hold all the humans detected
    ofxJSONElement humans;
    
    vector<vector<string>> connections;;
    int width,height;
    
    
    
    
    
};

