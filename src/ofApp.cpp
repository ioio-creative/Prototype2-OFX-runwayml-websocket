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
    
    ofSetVerticalSync(true);
    
    
    record = false;
    

    //OSC SEND
    
    // open an outgoing connection to HOST:PORT
    sender.setup(HOST, SEND_PORT);
    
    
    //OSC RECEIVE
    cout << "listening for osc messages on port " << RECEIVE_PORT << "\n";
    receiver.setup(RECEIVE_PORT);
    

    
    current_msg_string = 0;
    mouseX = 0;
    mouseY = 0;
    mouseButtonState = "";
    
    //      This are the pair of body connections we want to form.
    //      Try creating new ones!
    connections = {
        {"Nose", "Left_Eye"},
        {"Left_Eye", "Left_Ear"},
        {"Nose", "Right_Eye"},
        {"Right_Eye", "Right_Ear"},
        {"Nose", "Neck"},
        {"Neck", "Right_Shoulder"},
        {"Neck", "Left_Shoulder"},
        {"Right_Shoulder", "Right_Elbow"},
        {"Right_Elbow", "Right_Wrist"},
        {"Left_Shoulder", "Left_Elbow"},
        {"Left_Elbow", "Left_Wrist"},
        {"Neck", "Right_Hip"},
        {"Right_Hip", "Right_Knee"},
        {"Right_Knee", "Right_Ankle"},
        {"Neck", "Left_Hip"},
        {"Left_Hip", "Left_Knee"},
        {"Left_Knee", "Left_Ankle"}
    };

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

    }
    
    
    
    
    //OSC
    
    // hide old messages
    for(int i = 0; i < NUM_MSG_STRINGS; i++){
        if(timers[i] < ofGetElapsedTimef()){
            msg_strings[i] = "";
        }
    }
    
    
    getHumanFromOSC();
    
   
    
}

void ofApp::getHumanFromOSC(){
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
        
        
        
        /*
         // get the next OSC message
         ofxOscMessage m;
         receiver.getNextMessage(m);
         //        grab the data
         string data = m.getArgAsString(0);
         //        parse it to JSON
         results.parse(data);
         //        grab the humans
         humans = results["results"]["humans"];
         
         */
        
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetHexColor(0xffffff);
    vidGrabber.draw(20, 20);

    
    
    
    
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
    
    
    
    //RUNWAY
    drawParts();
    drawConnections();
    
    

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


// FROM WEB SERVER


// A function to draw humans body parts as circles
void ofApp::drawParts(){
    for(int h = 0; h < humans.size(); h++) {
        ofxJSONElement human = humans[h];
        // Now that we have one human, let's draw its body parts
        for (int b = 0; b < human.size(); b++) {
            ofxJSONElement body_part = human[b];
            // Body parts are relative to width and weight of the input
            float x = body_part[1].asFloat();
            float y = body_part[2].asFloat();
            ofDrawEllipse(x * width, y * height, 10, 10);
        }
    }
}

// A function to draw connection between body parts
void ofApp::drawConnections(){
    for(int h = 0; h < humans.size(); h++) {
        ofxJSONElement human = humans[h];
        // Now that we have a human, let's draw its body
        // connections start by looping through all body
        // connections and matching only the ones we need.
        for(int c = 0; c < connections.size(); c++){
            ofxJSONElement start;
            ofxJSONElement end;
            // Check if we have a pair in the current body parts
            for(int b = 0; b < human.size(); b++) {
                ofxJSONElement body_part = human[b];
                string name = body_part[0].asString();
                if (name == connections[c][0]){
                    start = body_part;
                } else if (name == connections[c][1]){
                    end = body_part;
                }
            }
            if (start.size() > 0 && end.size() > 0){
                float x1 = start[1].asFloat() * width;
                float y1 = start[2].asFloat() * height;
                float x2 = end[1].asFloat() * width;
                float y2 = end[2].asFloat() * height;
                ofDrawLine(x1, y1, x2, y2);
            }
        }
    }
}




