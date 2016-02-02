#include "ofApp.h"
#include <math.h>
#include "string.h"

double camPosx;
double camPosy;
double camPosz;

double camA;
double camB;
double camC;

double worldCenterX;
double worldCenterY;
double worldCenterZ;

double longitude;
double latitude;

ofVec3f testPoint, newTestPoint; //willbecome a vector of original points

int currentIndex;

struct longlat {
    double longitude;
    double latitude;
    bool cali;
    
};

longlat temp;

vector <longlat> longlatVector;

vector <ofVec3f> earthquakePoints;


//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofBackground( 255, 255, 255);
	
    camPosx = 0;
    camPosy = -7;
    camPosz = -10;
    
    camA = 0;
    camB = 0;
    camC = 0;
    
    
    
	camera.setPosition(ofVec3f(camPosx, camPosy, camPosz));
	camera.lookAt(ofVec3f(camA, camB, camC), ofVec3f(0, -1, 0));

	world.setup();
	world.enableGrabbing();
	world.enableDebugDraw();
	world.setCamera(&camera);
	
	sphere = new ofxBulletSphere();
	sphere->create(world.world, ofVec3f(0, 0, 0), 0.1, 3);
	sphere->add();
	
		
	ground.create( world.world, ofVec3f(0., 5.5, 0.), 0., 100.f, 1.f, 100.f );
	ground.setProperties(.25, .95);
	ground.add();
    
    
    
    
    //CSV parse
    currentIndex = 0;
    
    //Path to the comma delimited file
    string filePath = "earthquakes.csv";
    
    //Load file placed in bin/data
    ofFile file(filePath);
    
    if(!file.exists()){
        ofLogError("The file " + filePath + " is missing");
    }
    ofBuffer buffer(file);
    
    //Read file line by line
    
    for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
        string line = *it;
        
        //Split line into strings
        vector<string> words = ofSplitString(line, ",");
        
        //add elements to longlatVector
        temp.longitude = ofToDouble(words[1]);
        temp.latitude = ofToDouble(words[2]);
//        cout << words[14].substr(0,11) <<endl;
        if ((words[14].substr(0,11)).compare(" California")){
            
            temp.cali = true;
        }
        else {
            temp.cali = false;
        }
        
        
        longlatVector.push_back(temp);
        
    }
    
    cout << longlatVector.size() << endl;
    
    for(int i = 0; i < longlatVector.size(); i++){
        //cout << longlatVector[i].longitude << " longitude" << endl;
        testPoint = placePoint(longlatVector[i].latitude, longlatVector[i].longitude, sphere->getRadius());
        //cout << testPoint << " testPoint" << endl;
        earthquakePoints.push_back(testPoint); //vector of 3d vectors
        
    }
    
    //now we can draw them

    
    
    
}

//--------------------------------------------------------------
void ofApp::update() {
	world.update();
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 0));
    
    camera.setPosition(ofVec3f(camPosx, camPosy, camPosz));
    //camera.lookAt(ofVec3f(0, 0, 0), ofVec3f(camA, camB, camC));
    camera.lookAt(ofVec3f(camA, camB, camC), ofVec3f(0, -1, 0));
    
}

//--------------------------------------------------------------
void ofApp::draw() {
	glEnable( GL_DEPTH_TEST );
	camera.begin();
	
	ofSetLineWidth(1.f);
	ofSetColor(255, 0, 200);
	world.drawDebug();
	
	ofSetColor(5, 100, 100);
	ground.draw();
	
	ofSetColor(225, 225, 225);
	sphere->draw();
    
    worldCenterX = sphere->getPosition().x;
    worldCenterY = sphere->getPosition().y;
    worldCenterZ = sphere->getPosition().z;
    
    ofSetColor(100, 0, 0);
    
    //placement
    
    for (int i = 0; i < earthquakePoints.size(); i++){
        //cout << earthquakePoints[i] << endl;
        ofDrawSphere(earthquakePoints[i].x + worldCenterX, earthquakePoints[i].y + worldCenterY,
                     earthquakePoints[i].z + worldCenterZ, 0.03);
    }
    
    
    //movement
    
    
    
	
	camera.end();
}

//ofVec3f ofApp::rotatedPoint(ofVec3f originalPoint, float r){
//    
//    ofVec3f newPoint;
//    
//    double xRot = sphere->getRotationAxis().x;
//    double yRot = sphere->getRotationAxis().y;
//    double zRot = sphere->getRotationAxis().z;
//    
//    
//    
//    
//    double xdiff, ydiff, zdiff;
//    
//    //xtriangle
//    xdiff = sqrt((2*r*r)*(1-cos(xRot)));
//    
//    //ytriangle
//    ydiff = sqrt((2*r*r)*(1-cos(yRot)));
//    
//    //ztriangle
//    zdiff = sqrt((2*r*r)*(1-cos(zRot)));
//    
//
//    
//    //checking to see which way we rotate
//    if (xRot < 0) xdiff = xdiff * (-1);
//    if (yRot < 0) ydiff = ydiff * (-1);
//    if (zRot < 0) zdiff = zdiff * (-1);
//    
//    
//    
//    newPoint = ofVec3f(originalPoint.x + zdiff, originalPoint.y + ydiff, originalPoint.z + xdiff);
//    
//    
//    return newPoint;
//    
//}

ofVec3f ofApp::placePoint(double latitude, double longitude, float r){
    double theta;
    double fi;
    double x, y, z;
    double constantA, constantB;
    
    //establish theta and fi in degrees
    theta = ((90 - latitude)* M_PI / 180);
    fi = (longitude * M_PI / 180);
    
    
    constantA = tan(fi);
    
    //calculate z
    z = cos(theta)*r;
    
    constantB = (r*r) - (z*z);
    
    //calculate x
    
    x = sqrt((constantB * constantA * constantA) / (1+ (constantA * constantA)));
    
    if (longitude < 1) x = x * (-1);
    
    
    //calculate y
    
    y = x / (tan(fi));
    
    
    
    return ofVec3f(x,-y,z);
    
}



//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    switch(key) {
        //adjusting gravity
            
            
        //adjusting camera
        case (OF_KEY_UP):
            camPosy += 1;
            break;
            
        case (OF_KEY_DOWN):
            camPosy -= 1;
            break;
        
        case (OF_KEY_RIGHT):
            camPosx += 1;
            break;
            
        case (OF_KEY_LEFT):
            camPosx -= 1;
            break;
            
        case ('l'):
            camPosz += 1;
            break;
        case ('k'):
            camPosz -= 1;
            break;
        case ('g'):
            cout << sphere->getPosition() << endl;
            break;

            
    }

	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
	
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) { 
	
}