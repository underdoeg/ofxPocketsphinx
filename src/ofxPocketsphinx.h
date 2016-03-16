#ifndef OFXPOCKETSPHINX_H
#define OFXPOCKETSPHINX_H

#include "ofMain.h"
#include <pocketsphinx.h>
#include <limits>
#include "samplerate.h"


class ofxPocketsphinx
{
public:
	class EventArgs: public ofEventArgs{
	public:
		struct Word{
			std::string word;
			int32 acousticScore;
			int32 languageScore;
			int32 languageBackoffMode;
		};

		Word getHighestRatedWord(){
			int32 curScore = std::numeric_limits<int32>::max();
			Word ret;
			ret.acousticScore = curScore;
			for(auto& w: words){
				if(w.acousticScore < ret.acousticScore){
					//curScore = w.acousticScore;
					ret = w;
				}else if(w.acousticScore == ret.acousticScore){
					if(w.languageScore < ret.languageScore){
						ret = w;
					}
				}
			}
			return ret;
		}

		void printScore(){
			ofLog() << "--";
			for(auto& w: words){
				ofLog() << w.word << " AS:" << w.acousticScore << " LS:" << w.languageScore;
			}
		}

		std::string sentence;
		std::vector<Word> words;
		int32 probability;
	};

	ofxPocketsphinx();
	~ofxPocketsphinx();

	void setup(std::string hmm="/usr/share/pocketsphinx/model/en-us/en-us", std::string lm="/usr/share/pocketsphinx/model/en-us/en-us.lm.bin", std::string dict="/usr/share/pocketsphinx/model/en-us/cmudict-en-us.dict");
	void setup(std::map<std::string, std::string>& params);

	void audioIn(ofSoundBuffer& soundBuffer);
	void audioIn(float* data, int numSamples, long samplerate=16000);
	void audioIn(short* data, int numSamples, long samplerate=16000);


	std::string getContinuousText();
	int getContinuousProbability();

	std::string getResultText();
	int getResultProbability();

	void draw(bool drawAudioBuffer=true);

	ofEvent<EventArgs> onResult;

private:
	void process();

	ps_decoder_t *ps;
	cmd_ln_s* config;
	std::vector<short> buffer;
	long bufferSampleRate;

	bool uttStarted;
	std::string currentText;
	int currentProbability;
	std::string finalText;
	int finalProbability;

	SRC_STATE* srcState;
	int srcError;
};

#endif // OFXPOCKETSPHINX_H
