#ifndef OFXPOCKETSPHINX_H
#define OFXPOCKETSPHINX_H

#include "ofMain.h"
#include <pocketsphinx.h>

class ofxPocketsphinx
{
public:
	class EventArgs: public ofEventArgs{
	public:
		EventArgs(std::string s){
			sentence = s;
		}

		std::string sentence;
	};

	ofxPocketsphinx();

	void setup(std::string hmm="/usr/share/pocketsphinx/model/en-us/en-us", std::string lm="/usr/share/pocketsphinx/model/en-us/en-us.lm.bin", std::string dict="/usr/share/pocketsphinx/model/en-us/cmudict-en-us.dict");
	void setup(std::map<std::string, std::string>& params);

	void audioIn(ofSoundBuffer& soundBuffer);
	void audioIn(float* data, int numSamples);
	void audioIn(short* data, int numSamples);

	std::string getContinuousText();
	int getContinuousProbability();

	std::string getResultText();
	int getResultProbability();

	void draw();

	ofEvent<EventArgs> onResult;

private:
	void process();

	ps_decoder_t *ps;
	cmd_ln_s* config;
	std::vector<short> buffer;
	bool uttStarted;
	std::string currentText;
	int currentProbability;
	std::string finalText;
	int finalProbability;
};

#endif // OFXPOCKETSPHINX_H
