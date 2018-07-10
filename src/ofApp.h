#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

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
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofVideoGrabber vidGrabber;
    ofPixels videoInverted;
    ofTexture videoTexture;
    int camWidth;
    int camHeight;
    
    
    bool record;
    
    
    
    ofxOscReceiver receiver;
    
    
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    
    int mouseX, mouseY;
    string mouseButtonState;
    
    ofImage receivedImage;
    
    
    
    
    
    
    //ofxOscSender
    
    ofxOscSender sender;
};

