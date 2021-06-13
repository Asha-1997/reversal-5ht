//Including header file
#include "ctxneuron.h"

//Class Constructor
CtxNeuron::CtxNeuron(float _learningRateLTP, float _learningRateLTD, float _tLTD) : 
	learningRateLTP(_learningRateLTP),
	learningRateLTD(_learningRateLTD),
	tLTD(_tLTD),
	slowCaDetector(new SecondOrderLowpassFilter(_tLTD))
{
}

//Function Overloading: distinguishing between OFC neurons and mPFC neurons to apply rules specific to them
//For OFC object: two contact streams for two sources, max of 2 added inputs which work with reinforcement
float CtxNeuron::doStep(float nonPlasticInput, float success, float serot, float primary_contact, float secondary_contact) {	
	Contacts[0] = primary_contact;
	Contacts[1] = secondary_contact;
	max_reinf_inputs = 2;
	float t = doStep(nonPlasticInput, success, serot);
	Past_contact[0] = Contacts[0];
	Past_contact[1] = Contacts[1];
	return t;
}

//For mPFC objects: only one contact stream pass in & used for one source only, max of 1 added input which works with reinforcement	
float CtxNeuron::doStep(float nonPlasticInput, float success, float serot, float primary_contact) {
	Contacts[0] = primary_contact;
	Contacts[1] = 0;
	max_reinf_inputs = 1;
	float t = doStep(nonPlasticInput, success, serot);
	Past_contact[0] = Contacts[0];
	Past_contact[1] = 0;
	return t;
}

//New success variable defines whether a decision was rewarding or not. It is used for building effective mechanisms in this program, but an alternative method would be welcome as "success" exists as a step function- using it in this model fails to acknowledge the time it takes for biological systems to recieve, process and transmit information.
float CtxNeuron::doStep(float nonPlasticInput, float success, float serot) {   		   
        		   
	output = nonPlasticInput;

	for(int i=0;i<nInputs;i++) {
		output += weights[i] * *(inputs[i]);
	}
	if (output < 0) output = 0;
	if (output > 1) output = 1;
	if (serot < 0) serot = 0;
	if (serot > 1) serot = 1;
	output = ofc5HTreceptors(output,serot,serot*2);
	
	float dOutput = output - past_output;
	if (dOutput < 0) dOutput = 0;

	float slowCa = slowCaDetector->filter(output);
	slowCa = fabs(slowCa);
	
//UNEXPECTED REWARD MECHANISM // OFC boost when a stimuli found to be previously unrewarding is now rewarding
	expected = 0;
	if(success>0){ //using success variable (non-filtered) in place of nonPlasticInput to isolate event which mechanism is based around
		if(((Past_contact[0] < Contacts[0]) && (Past[0] ==-1)) || ((Past_contact[1] < Contacts[1]) && (Past[1] ==-1))) {
			expected = -1;
		}else {
			expected = 1;
		}
	}

//REINFORCEMENT MECHANISM // The more times in a sequence that a rule is found to be true, the more significant each experience is on learning or unlearning (until a maximum). Also, unless there is LTD in-between, then the reinforcement LTP gain associated with a stimuli is carried on to the next interaction with that stimuli- this explains the increased speed at which a rule which was once learned, and not specifically unlearned, can be learned again- shown best in the ctxneuron_test.
	for(int j=0;j<max_reinf_inputs;j++){
		if(Past_contact[j] < Contacts[j]){ //only happens once per new decision
			if(success>0){ //using success variable in place of nonPlasticInput to isolate event which mechanism is based around
				Reinf_Gain_LTP[j] = ReinforcementLTP(Reinf_Gain_LTP[j], Past[j]);
				Reinf_Gain_LTD[j] = 1;
				Past[j] = 1; //logs the nature of every new decision: 1 = rewarding 
			}else{
				Reinf_Gain_LTP[j] = 1;
				Reinf_Gain_LTD[j] = ReinforcementLTD(Reinf_Gain_LTD[j], Past[j]);;
				Past[j] = -1; //logs the nature of every new decision: -1 = not rewarding 
			}
		}	
	}   
			    
//WEIGHT CHANGE MECHANISM
	for(int i=0;i<nInputs;i++) {
		//Reinforcement mechanism uses two contact inputs (g/b), where a ratio of 1:1 contact inputs to added inputs must be observed. Due to this shortcoming, the effects of reinforcement are not applied to more than the first two added inputs (visG and visB) for an OFC neuron, or to the first added input (visG OR visB) for an mPFC neuron.
		if(i < max_reinf_inputs){ //maximum added inputs which will be affected by reinforcement: 1 for mPFCs, 2 for OFC	 
			if (nonPlasticInput> 0.25) // could do (success > 0) but would further undermine lack of step functions in real biology
				//LTP
				weightChange(weights[i], learningRateLTP * serot * *(inputs[i]) * dOutput * Reinf_Gain_LTP[i]);
			//LTD
			weightChange(weights[i], - tLTD * learningRateLTD * slowCa * serot * Reinf_Gain_LTD[i]);			
		}else{	//until maximum amount of added inputs (10)
			if (nonPlasticInput> 0.25)
				//LTP
				weightChange(weights[i], learningRateLTP * serot * *(inputs[i]) * dOutput);
			//LTD
			weightChange(weights[i], - tLTD * learningRateLTD * slowCa * serot);
		}
	}
	
	past_output = output; //changed name for readability in derivative calculation
	return output;
}
