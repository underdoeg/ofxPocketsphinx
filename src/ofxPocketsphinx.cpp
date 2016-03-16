#include "ofxPocketsphinx.h"

ofxPocketsphinx::ofxPocketsphinx(){
	srcState = nullptr;
}

ofxPocketsphinx::~ofxPocketsphinx(){
	if(srcState)
		src_delete(srcState);
}

void ofxPocketsphinx::setup(std::string hmm, std::string lm, std::string dict){
	std::map<std::string, std::string> params;
	params["-hmm"] = ofToDataPath(hmm);
	params["-lm"] = ofToDataPath(lm);
	params["-dict"] = ofToDataPath(dict);
	setup(params);
}

void ofxPocketsphinx::setup(std::map<std::string, std::string>& params){
	config = cmd_ln_init(NULL, ps_args(), True, "-samprate", "16000", "-logfn", "/dev/null", NULL); //"-rawlogdir", ofToDataPath("log/").c_str(),


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
	audioIn(soundBuffer.getBuffer().data(), soundBuffer.size(), soundBuffer.getSampleRate());
}

void ofxPocketsphinx::audioIn(float *data, int numSamples, long sampleRate){

	bufferSampleRate = 16000;

	if(sampleRate != bufferSampleRate){

		float dataOut[numSamples];

		if(srcState == nullptr){
			srcState = src_new(SRC_SINC_BEST_QUALITY, 1, &srcError);
			ofLogNotice("ofxPocketsphinx") << "create new sample rate converter";
		}

		SRC_DATA srcData;
		srcData.data_in = data;
		srcData.data_out = dataOut;
		srcData.input_frames = numSamples;
		srcData.output_frames = numSamples;//bufferSampleRate / sampleRate;
		srcData.end_of_input = 0;

		srcData.src_ratio = (1.0 * bufferSampleRate) / sampleRate;

		if((srcError = src_process(srcState, &srcData))){
			ofLogError("ofxPocketsphinx") << src_strerror(srcError);
		}

		numSamples = srcData.output_frames_gen;
		data = srcData.data_out;
	}

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

void ofxPocketsphinx::audioIn(short *data, int numSamples, long sampleRate){
	if(buffer.size()!=numSamples)
		buffer.resize(numSamples);

	for(int i=0;i<numSamples; i++){
		buffer[i] = data[i];
	}
	bufferSampleRate = sampleRate;
	process();
}

void ofxPocketsphinx::process(){
	int res = ps_process_raw(ps, buffer.data(), buffer.size(), FALSE, FALSE);
	if(res<0){
		ofLogWarning("ofxPocketsphinx") << "Error processing";
		return;
	}

	if(bufferSampleRate != 16000){
		ofLogError("ofxPocketsphinx") << "Wrong samplerate, should be 16000. Automatic conversion not yet supported for short buffer types";
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

			const char* res = ps_get_hyp_final(ps, &finalProbability);

			if(res){
				finalText = res;
				ps_seg_t* iter = ps_seg_iter(ps);

				std::vector<EventArgs::Word> words;
				while(ps_seg_next(iter)){
					EventArgs::Word w;
					w.word =  ps_seg_word(iter);
					if(w.word[0] != '<'){
						ps_seg_prob(iter, &w.acousticScore, &w.languageScore, &w.languageBackoffMode);
						words.push_back(w);
					}
				}

				static EventArgs args;
				args.sentence = finalText;
				args.words = words;
				args.probability = ps_get_prob(ps);
				ofNotifyEvent(onResult, args);
			}

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

	ofPushMatrix();
	ofTranslate(30, 100);
	ofNoFill();
	ofBeginShape();
	for (unsigned int i = 0; i < buffer.size(); i++){
		ofVertex(i, 100 - buffer[i]*.005f);
	}
	ofEndShape(false);
	ofPopMatrix();

	ofPopStyle();
}


