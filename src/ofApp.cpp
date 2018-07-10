#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    camWidth = 640;  // try to grab at this size.
    camHeight = 480;
    
    //we can now get back a list of devices.
    vector<ofVideoDevice> devices = vidGrabber.listDevices();
    
    for(int i = 0; i < devices.size(); i++){
        if(devices[i].bAvailable){
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
        }else{
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
        }
    }
    
    vidGrabber.setDeviceID(1);
    vidGrabber.setDesiredFrameRate(60);
    vidGrabber.initGrabber(camWidth, camHeight);
    
    videoInverted.allocate(camWidth, camHeight, OF_PIXELS_RGB);
    videoTexture.allocate(videoInverted);
    ofSetVerticalSync(true);
    
    
    record = false;
    
    
    
    
    
    //OSC
    
    cout << "listening for osc messages on port " << RECEIVE_PORT << "\n";
    receiver.setup(RECEIVE_PORT);
    
    current_msg_string = 0;
    mouseX = 0;
    mouseY = 0;
    mouseButtonState = "";
    
    
    
    // open an outgoing connection to HOST:PORT
    sender.setup(HOST, SEND_PORT);
}




//--------------------------------------------------------------
void ofApp::update(){
    ofBackground(100, 100, 100);
    vidGrabber.update();
    
    string path = "nodeGetImage/public/";
    if(vidGrabber.isFrameNew()){
        ofPixels & pixels = vidGrabber.getPixels();
        if(record){
                ofSaveImage(pixels,path+"1.jpg");
    //ofSaveImage(pixels,ofToString(ofGetFrameNum())+".jpg");
            record = false;
        }
        for(int i = 0; i < pixels.size(); i++){
            videoInverted[i] = 255 - pixels[i];
        }
        videoTexture.loadData(videoInverted);
    }
    
    
    
    
    //OSC
    
    // hide old messages
    for(int i = 0; i < NUM_MSG_STRINGS; i++){
        if(timers[i] < ofGetElapsedTimef()){
            msg_strings[i] = "";
        }
    }
    
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        
        // check for mouse moved message
        if(m.getAddress() == "/mouse/position"){
            // both the arguments are int32's
            mouseX = m.getArgAsInt32(0);
            mouseY = m.getArgAsInt32(1);
        }
        // check for mouse button message
        else if(m.getAddress() == "/mouse/button"){
            // the single argument is a string
            mouseButtonState = m.getArgAsString(0);
        }
        // check for an image being sent (note: the size of the image depends greatly on your network buffer sizes - if an image is too big the message won't come through )
        else if(m.getAddress() == "/image" ){
            ofBuffer buffer = m.getArgAsBlob(0);
            receivedImage.load(buffer);
        }
        else{
            // unrecognized message: display on the bottom of the screen
            string msg_string;
            msg_string = m.getAddress();
            msg_string += ": ";
            for(int i = 0; i < m.getNumArgs(); i++){
                // get the argument type
                msg_string += m.getArgTypeName(i);
                msg_string += ":";
                // display the argument - make sure we get the right type
                if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                    msg_string += ofToString(m.getArgAsInt32(i));
                }
                else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                    msg_string += ofToString(m.getArgAsFloat(i));
                }
                else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
                    msg_string += m.getArgAsString(i);
                }
                else{
                    msg_string += "unknown";
                }
            }
            // add to the list of strings to display
            msg_strings[current_msg_string] = msg_string;
            timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
            current_msg_string = (current_msg_string + 1) % NUM_MSG_STRINGS;
            // clear the next line
            msg_strings[current_msg_string] = "";
        }
        
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetHexColor(0xffffff);
    vidGrabber.draw(20, 20);
    videoTexture.draw(20 + camWidth, 20, camWidth, camHeight);
    
    
    
    
    
    //OSC
    
    string buf;
    buf = "listening for osc messages on port" + ofToString(RECEIVE_PORT);
    ofDrawBitmapString(buf, 10, 20);
    
    if(receivedImage.getWidth() > 0){
        ofDrawBitmapString("Image:", 10, 160);
        receivedImage.draw(10, 180);
    }
    
    // draw mouse state
    buf = "mouse: " + ofToString(mouseX, 4) +  " " + ofToString(mouseY, 4);
    ofDrawBitmapString(buf, 430, 20);
    ofDrawBitmapString(mouseButtonState, 580, 20);
    
    for(int i = 0; i < NUM_MSG_STRINGS; i++){
        ofDrawBitmapString(msg_strings[i], 10, 40 + 15 * i);
    }
    
    

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    // in fullscreen mode, on a pc at least, the
    // first time video settings the come up
    // they come up *under* the fullscreen window
    // use alt-tab to navigate to the settings
    // window. we are working on a fix for this...
    
    // Video settings no longer works in 10.7
    // You'll need to compile with the 10.6 SDK for this
    // For Xcode 4.4 and greater, see this forum post on instructions on installing the SDK
    // http://forum.openframeworks.cc/index.php?topic=10343
    if(key == 's' || key == 'S'){
        vidGrabber.videoSettings();
    }
    if(key=='r'){
        record = !record;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    ofLog() << "send Mouse X : " << x;
    ofxOscMessage m;
    m.setAddress("/test");
    m.addIntArg(x);
    m.addIntArg(y);
    sender.sendMessage(m, true);
}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
}

