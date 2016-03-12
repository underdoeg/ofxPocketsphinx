#pragma once

#include "ofMain.h"
#include "ofxPocketsphinx.h"

#define BUFFER_SIZE 256

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

	void onResult(ofxPocketsphinx::EventArgs& args);

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	void audioIn(ofSoundBuffer & input);

	ofxPocketsphinx sphinx;

	ofSoundStream soundStream;
	std::string currentResult;
	ofTrueTypeFont font;
	int wordCounter;
};
