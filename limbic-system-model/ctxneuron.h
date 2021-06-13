#ifndef _CTXNEURON
#define _CTXNEURON

#include <math.h>
#include "filter.h"

// Based on Sjostrom (sjostrom_2002_plast_ca_ltp_ltd.pdf) and
// Ciranna (ciranna_2006_5ht_glu_receptors_review.pdf): Ca based conincidence detector
// Dayan / Iigaya 2018: 5HT controls plasticity, learning rate increased
// Nutt: 5HTR2 "Flexibilit of the mind" / plastcicity
// Kraus 2017: 5HTR2 predominantly in the PFC



class CtxNeuron {

	static const int maxInputs = 10;
	static const int max_contacts = 2; //There can only be two contact streams passed in from simulation: green contact or blue contact

public:

	CtxNeuron(float _learningRateLTP = 0.0, float _learningRateLTD = 0.0, float tLTD = 0.01);

				
	void addInput(float& anInput, float initWeight = 0) {
		if (nInputs == maxInputs) throw "No more inputs.";
		inputs[nInputs] = &anInput;
		weights[nInputs] = initWeight;
		if(nInputs < max_contacts){ 
			Reinf_Gain_LTP[nInputs] = min_gain; //Holds LTP Reinforcement gains for added inputs
			Reinf_Gain_LTD[nInputs] = min_gain; //Holds LTD Reinforcement gains for added inputs
			Past[nInputs] = 0; //Holds the nature (rewarding or not) of every interaction with a stimuli
			Contacts[nInputs] = 0; //Array for whether a stimuli has been chosen (1) or is not currently chosen (0)
			Past_contact[nInputs] = 0; //Holds Contacts[] inputs of last step - used so that reinforcement and unexpected reward 
		}			           // mechanisms only take effect once per decision (on the first step)
    	    	nInputs++;
				
	}
	
	
	//Function Overloading
	//Applying doStep slightly differently depending on neuron type
	float doStep(float nonPlasticInput, float success, float serot, float primary_contact, float secondary_contact);
	float doStep(float nonPlasticInput, float success, float serot, float primary_contact);
	float doStep(float nonPlasticInput, float success, float serot);
	
	
	//Reinforcement Mechanism: LTP
	float ReinforcementLTP(float gain, float past){
		if(past==1){ //To start LTP reinforcement gain building, a stimuli must be found to be rewarding at least twice in a row. 
			gain += LTP_Reinf_gain_rate; //Reinforcement LTP gain building
			if(gain > max_LTP_Reinf){
				gain = max_LTP_Reinf;	//Setting max Reinforcement LTP gain
			}
		}else{
			gain = min_gain; //If a stimuli is rewarding but wasn't in the last interaction (past == 0 (initial) or past == -1 (LTD),
		}			 // Reinforcement LTP gain is maintained at the minimum value. 
		return gain;		 //Regardless of the last past value, the past value is then updated to equal 1 (in ctxneuron.cpp).
	}
	
	
	//Reinforcement Mechanism: LTD
	float ReinforcementLTD(float gain, float past){
		if(past == -1){ //To start LTD reinforcement gain building, a stimuli must be found to be not rewarding at least twice in a row. 
			gain += LTD_Reinf_gain_rate; //Reinforcement LTD gain building
			if(gain > max_LTD_Reinf){
				gain = max_LTD_Reinf;	//Setting max Reinforcement LTD gain
			}
		}else{
			gain = min_gain;//If a stimuli is not rewarding and either: it was rewarding in the last interaction, or this is the first interaction, 
		}			 //then Reinforcement LTD gain is maintained at the minimum value. 
		return gain;		 //Regardless of the last past value, the past value is then updated to equal -1 (in ctxneuron.cpp).
	}


	//Functions to get and set values from outside the class. The "getters" are used for logging and plotting important values.	
	void setWeight(int i, float w) {
	  weights[i] = w;
	}	
	float getWeight(int i) {
	  return weights[i];
	}
	int getExpected() {
	  return expected;
	}
	float getLTP_gain(int i) {
	  return Reinf_Gain_LTP[i];
	}
	float getLTD_gain(int i) {
	  return Reinf_Gain_LTD[i];
	}


private:

	float output;
	float past_output;
	float* inputs[maxInputs];
	float weights[maxInputs];
	int nInputs = 0;
	float learningRateLTP;
	float learningRateLTD;
	float tLTD;
	SecondOrderLowpassFilter* slowCaDetector;
	
	int expected;	
	float Reinf_Gain_LTP[maxInputs], Reinf_Gain_LTD[maxInputs];
	float Contacts[maxInputs], Past_contact[maxInputs];
	float Past[maxInputs];
	float max_reinf_inputs;
	const float min_gain = 1;  //min increase in LTD or LTP speed is 0%
	const float max_LTP_Reinf = 2;  //max increase in LTP speed is 100%
	const float max_LTD_Reinf = 2;  //max increase in LTD speed is 100%
	const float LTP_Reinf_gain_rate = 0.2;
	const float LTD_Reinf_gain_rate = 0.2;
	
	void weightChange(float &w, float delta) {
	  w += delta;
	  if (w>1) w = 1;
	  if (w<0) w = 0;
	}
	

public:
	static float ofc5HTreceptors(float x, float htR1, float htR2) { //need to find out what static float means
	  htR1 = 1 + htR1;
	  htR2 = 2 + htR2;
	  float r = (1-exp(-pow(x/htR1,htR1)))*htR2;
	  if (r < 0.00001) return 0;
	  return r;
	}

};

#endif
