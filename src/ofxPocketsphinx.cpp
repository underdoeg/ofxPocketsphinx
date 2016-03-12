#include "ofxPocketsphinx.h"

ofxPocketsphinx::ofxPocketsphinx(){

}

void ofxPocketsphinx::setup(std::string hmm, std::string lm, std::string dict){
	std::map<std::string, std::string> params;
	params["-hmm"] = hmm;
	params["-lm"] = lm;
	params["-dict"] = dict;
	setup(params);
}

void ofxPocketsphinx::setup(std::map<std::string, std::string>& params){
	config = cmd_ln_init(NULL, ps_args(), True, "-samprate", "16000", "-logfn", "/dev/null", "-rawlogdir", ofToDataPath("log/").c_str(), NULL);



	for(auto param: params){
		cmd_ln_set_str_extra_r(config, param.first.c_str(), param.second.c_str());
	}

	ps = ps_init(config);
	if(ps == nullptr){
		ofLogFatalError("ofxPocketSphinx") << "Could not create pocketsphinx recognizer";
	}

	ps_start_utt(ps);

	uttStarted = false;
}

void ofxPocketsphinx::audioIn(ofSoundBuffer &soundBuffer){
	soundBuffer.toShortPCM(buffer);
	process();
}

void ofxPocketsphinx::audioIn(float *data, int numSamples){
	if(buffer.size()!=numSamples)
		buffer.resize(numSamples);

	float f = 0.f;
	for(int i=0;i<numSamples; i++){
		f = data[i] * 32768 ;
		if( f > 32767 ) f = 32767;
		if( f < -32768 ) f = -32768;
		buffer[i] = (short)f;
	}
	process();
}

void ofxPocketsphinx::audioIn(short *data, int numSamples){
	if(buffer.size()!=numSamples)
		buffer.resize(numSamples);

	for(int i=0;i<numSamples; i++){
		buffer[i] = data[i];
	}

	process();
}

void ofxPocketsphinx::process(){
	int res = ps_process_raw(ps, buffer.data(), buffer.size(), FALSE, FALSE);
	if(res<0){
		ofLogWarning("ofxPocketsphinx") << "Error processing";
		return;
	}

	uint8 inSpeech = ps_get_in_speech(ps);

	if(inSpeech){
		const char* ct = ps_get_hyp(ps, &currentProbability);
		if(ct){
			currentText = ct;
		}

		if(!uttStarted){
			uttStarted = true;
		}
	}else{
		if(uttStarted){
			ps_end_utt(ps);

			finalText = ps_get_hyp_final(ps, &finalProbability);

			if (ps_start_utt(ps) < 0)
				ofLogFatalError("ofxPocketsphinx") << "Failed to start utterance";
			uttStarted = false;
		}
	}
}

string ofxPocketsphinx::getContinuousText(){
	return currentText;
}

int ofxPocketsphinx::getContinuousProbability(){
	return currentProbability;
}

string ofxPocketsphinx::getResultText(){
	return finalText;
}

int ofxPocketsphinx::getResultProbability(){
	return finalProbability;
}

void ofxPocketsphinx::draw(){
	ofPushStyle();
	std::string curText = "CURRENT: "+getContinuousText()+"\nCURRENT PROBABILITY: "+ofToString(getContinuousProbability());
	curText += "\n\n";
	curText += "RESULT: "+getResultText()+"\nRESULT PROBABILITY: "+ofToString(getResultProbability());
	ofDrawBitmapStringHighlight(curText, ofVec2f(30, 30));
	ofPopStyle();
}


