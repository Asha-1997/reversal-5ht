
//Comments in this file have been limited to updated sections for clarity. 

#ifndef _LIMBIC_SYSTEM
#define _LIMBIC_SYSTEM

#include <unistd.h>

#include <filter.h>

#include "limbic-system-model/ctxneuron.h"

extern float LHB_BIAS;
extern float DRN_SUPPRESSION;
extern float OFC_5HTR1_OFFSET;
extern float OFC_5HTR2_OFFSET;
extern float DRN_OFFSET;


class Limbic_system {
public:
	Limbic_system();
	
	~Limbic_system();
	
	void doStep(float _reward,
		    float _placefield_Green,
		    float _placefield_Blue,
		    float _on_contact_direction_Green,
		    float _on_contact_direction_Blue,
		    float _visual_direction_Green,
		    float _visual_direction_Blue,
		    float _visual_reward_Green,
		    float _visual_reward_Blue
	);
	
	enum ExploreStates {EXPLORE_STRAIGHT, EXPLORE_LEFT, EXPLORE_RIGHT, EXPLORE_STOP, EXPLORE_NUM_ITEMS};

	ExploreStates exploreState = EXPLORE_STRAIGHT;

	float getGreenOutput() {
		return CoreGreenOut;
	};
	
	float getBlueOutput() {
		return CoreBlueOut;
	};
	
	float getExploreLeft() {return mPFC2CoreExploreLeft;};

	float getExploreRight() {return mPFC2CoreExploreRight;};
	

	//Expected variable: for checking if an unexpected reward occurs (from a stimuli which was previously not rewarding) after switch.
	int expected;
	
	//Non filtered reward variable is used to define successful or noon successful interactions with stimuli using the variable "success"
	float non_filtered_reward = 0;
	
	//New variables to check workings of reinforcement mechanism
	float OFC_LTP_gainG;
	float OFC_LTD_gainG;
	float OFC_LTP_gainB;
	float OFC_LTD_gainB;
	float mPFCg_LTP_gainG;
	float mPFCg_LTD_gainG;
	float mPFCb_LTP_gainB;
	float mPFCb_LTD_gainB;	
	
private:
	long int step;

	float shunting_inhibition_factor = 200;

	float core_weight_lg2lg = 1;

	float core_weight_dg2dg = 1;

private:
	float CoreGreenOut = 0;
	float CoreBlueOut = 0;
	float mPFC2CoreExploreLeft = 0;
	float mPFC2CoreExploreRight = 0;

public:
	const float learning_rate_core = 0;

	float core_DA = 0;
	float core_plasticity = 0;

	int coreExploreCtr = 0;

	float lShell = 0;

	float shell_DA = 0;
	float shell_plasticity = 0;
	
	float lShell_weight_pflg = 0;
	float lShell_weight_pfdg = 0;

	const float learning_rate_lshell = 0.001;

	float VTA_baseline_activity = 0.10;
	float VTA_zero_val = 0.0505;

	float RMTg = 0;

	const float learning_rate_mPFC = 5;
	
	SecondOrderLowpassFilter* visual_direction_Green_mPFC_filter; 
	SecondOrderLowpassFilter* visual_direction_Blue_mPFC_filter; 

	float visual_direction_Green_trace = 0;
	float visual_direction_Blue_trace = 0;
	
	float mPFC_Green = 0;
	float mPFC_Blue = 0;

	CtxNeuron* mPFCneuronGreen = NULL;
	CtxNeuron* mPFCneuronBlue = NULL;
	
	float mPFC_receptor_5HT1 = 0;
	float mPFC_receptor_5HT2 = 0;

	int mPFCspontGreen = 0;
	int mPFCspontBlue = 0;

	float VTA = 0;

	SecondOrderLowpassFilter* VTA_forwardinhibition; 

	float dlVP = 0;

	float EP=0;

	float LHb = 0;

	float DRN = 0;

	CtxNeuron* OFCNeuron;
	float OFC = 0;

	const float learning_rate_OFC = 1;

	float pfLg2OFC = 0;
	float pfDg2OFC = 0;

	SecondOrderLowpassFilter* DRNto5HTrelease;

	float serotoninConcentration = 0;

private:
	void weightChange(float &w, float delta);

	SecondOrderLowpassFilter* on_contact_direction_Green_filter = NULL;
	SecondOrderLowpassFilter* on_contact_direction_Blue_filter = NULL;

	float reward = 0;
	SecondOrderLowpassFilter* reward_filter = NULL;

	float placefieldGreen = 0;
	float placefieldBlue = 0;

	SecondOrderLowpassFilter* placefield_Green_filter = NULL;
	SecondOrderLowpassFilter* placefield_Blue_filter = NULL;
	
	float on_contact_direction_Green = 0;
	float on_contact_direction_Blue = 0;
	float visual_direction_Green = 0;
	float visual_direction_Blue = 0;
	float visual_reward_Green = 0;
	float visual_reward_Blue = 0;

	FILE* flog = NULL;
	void logging();

	float ofc5HTreceptors(float x, float htR1, float htR2) {
		htR1 = htR1 + OFC_5HTR1_OFFSET;
		htR2 = htR2 + OFC_5HTR2_OFFSET;
		float r = (1-exp(-pow(x/htR1,htR1)))*htR2;
		if (r < 0.00001) return 0;
		return r;
	}
};
#endif
