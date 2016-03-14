#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	sphinx.setup();

	ofSoundStreamSettings settings;

	settings.setInListener(this);
	settings.sampleRate = 16000;
	settings.numOutputChannels = 0;
	settings.numInputChannels = 1;
	settings.bufferSize = BUFFER_SIZE;
	settings.setApi(ofSoundDevice::Api::PULSE);
	soundStream.setup(settings);

	ofSetFrameRate(60);

	font.load("Arial.ttf", 50);

	ofAddListener(sphinx.onResult, this, &ofApp::onResult);

	ofSetBackgroundColor(30);
}

//--------------------------------------------------------------
void ofApp::update(){

}


void ofApp::audioIn(ofSoundBuffer &input){
	sphinx.audioIn(input);
}


//--------------------------------------------------------------
void ofApp::draw(){
	sphinx.draw();

	if(wordCounter>0){
		font.drawString(currentResult, ofGetWidth()*.5-font.stringWidth(currentResult)*.5, ofGetHeight()*.6);
		wordCounter--;
	}
}

void ofApp::onResult(ofxPocketsphinx::EventArgs &args){
	currentResult = args.sentence;
	wordCounter = 100;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
