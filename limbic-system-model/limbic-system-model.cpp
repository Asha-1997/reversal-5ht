
//Comments in this file have been limited to updated sections for clarity. 

#include"limbic-system-model.h"

#include <stdio.h>
#include <iostream>
#include <filter.h>

float LHB_BIAS = 0;
float DRN_SUPPRESSION = 0;
float OFC_5HTR1_OFFSET = 0;
float OFC_5HTR2_OFFSET = 0;
float DRN_OFFSET = 0;

Limbic_system::Limbic_system()
{
	flog = fopen("log.dat","wt");

	const float tLTD = 0.05;

	on_contact_direction_Green_filter = new SecondOrderLowpassFilter(0.1);
	on_contact_direction_Blue_filter = new SecondOrderLowpassFilter(0.1);

	visual_direction_Green_mPFC_filter = new SecondOrderLowpassFilter(0.01);
	visual_direction_Blue_mPFC_filter = new SecondOrderLowpassFilter(0.01);

	VTA_forwardinhibition = new SecondOrderLowpassFilter(0.01);

	placefield_Green_filter = new SecondOrderLowpassFilter(0.1);
	placefield_Blue_filter = new SecondOrderLowpassFilter(0.1);

	reward_filter = new SecondOrderLowpassFilter(0.1);

	DRNto5HTrelease = new SecondOrderLowpassFilter(0.001);
	

//Due to the functioning of the reinforcement mechanism, although up to 10 inputs can be added, primary inputs must be added first and contact streams in doStep must be passed in the same order. For the OFC, the first two added inputs will go through the reinforcement mechanism paired with the 2 passed in contact streams. Added inputs and contact streams are paired using their order: if the primary green plastic input is added first (e.g., visual_direction_Green_trace) and the primary blue input is second, then the green contact variable should be passed in before the blue contact variable in the OFC neuron doStep () function (as seen on line 96). This rule is less important for mPFC instances, as only one contact is passed in (blue or green) which is paired with the first added primary input (blue or green). However, it is still important to add whichever plastic input you would like reinforcement to affect (e.g., visual_direction_green_trace or visual_reward_Blue) first. Examples of this are in lines 147 and 150. As in lines 45, 46, 51 and 55, there is an option to add more plastic inputs to the ctxneuron objects- reinforcement will not be applied but normal weight change will.

	OFCNeuron = new CtxNeuron(learning_rate_OFC,learning_rate_OFC * 0.1);
	OFCNeuron->addInput(visual_direction_Green_trace);
	OFCNeuron->addInput(visual_direction_Blue_trace);
//	OFCNeuron->addInput(placefieldGreen);
//	OFCNeuron->addInput(placefieldBlue);

	const float w = 1;
	mPFCneuronGreen = new CtxNeuron(learning_rate_mPFC,learning_rate_mPFC*0.01,tLTD);
	mPFCneuronGreen->addInput(visual_direction_Green_trace,w*0.25);
//	mPFCneuronGreen->addInput(visual_reward_Green,w);
	
	mPFCneuronBlue = new CtxNeuron(learning_rate_mPFC,learning_rate_mPFC*0.01,tLTD);
	mPFCneuronBlue->addInput(visual_direction_Blue_trace,w*0.25);
//      mPFCneuronBlue->addInput(visual_reward_Blue,w);	
		
	step = 0;
};

Limbic_system::~Limbic_system()
{
	fclose(flog);
}

void Limbic_system::weightChange(float &w, float delta) {
	w += delta;
	if (w>1) w = 1;
	if (w<0) w = 0;
}
void Limbic_system::doStep(float _reward,
		float _placefieldGreen,
		float _placefieldBlue,
		float _on_contact_direction_Green,
		float _on_contact_direction_Blue,
		float _visual_direction_Green,
		float _visual_direction_Blue,
		float _visual_reward_Green,
		float _visual_reward_Blue ) {
		
	// Used to create success variable.
	non_filtered_reward = _reward; 
	
	reward = reward_filter->filter(_reward);
	placefieldGreen = placefield_Green_filter->filter(_placefieldGreen);
	placefieldBlue = placefield_Blue_filter->filter(_placefieldBlue);
	on_contact_direction_Green = _on_contact_direction_Green;
	on_contact_direction_Blue = _on_contact_direction_Blue;
	visual_direction_Green = _visual_direction_Green;
	visual_direction_Blue = _visual_direction_Blue;
	visual_reward_Green = _visual_reward_Green;
	visual_reward_Blue = _visual_reward_Blue;
	visual_direction_Green_trace = visual_direction_Green_mPFC_filter->filter(visual_direction_Green);
	visual_direction_Blue_trace = visual_direction_Blue_mPFC_filter->filter(visual_direction_Blue);

	//New OFC Neuron doStep
	OFC = OFCNeuron->doStep(reward, non_filtered_reward, serotoninConcentration+0.1, on_contact_direction_Green, on_contact_direction_Blue);
	
	if (OFC > 0.25) {
		OFC = 0.25;
	}
	
	//Unexpected Reward Mechanism: used for checking if an unexpected reward occurs (from a stimuli which was previously not rewarding) after a reversal switch. If so, a transient OFC boost of 0.1 is applied.
	expected = OFCNeuron->getExpected();
	if (expected == -1) {
		OFC += 0.1;
	}	
	
	DRN = (OFC * 5) / (1+RMTg * shunting_inhibition_factor + DRN_SUPPRESSION) + DRN_OFFSET;

	serotoninConcentration = DRNto5HTrelease->filter(DRN);

	VTA = (OFC + VTA_baseline_activity) / (1+(RMTg + VTA_forwardinhibition->filter(OFC*0.1)) * shunting_inhibition_factor);

	switch (exploreState) {
	case EXPLORE_LEFT:
		mPFC2CoreExploreLeft = 0.1;
		mPFC2CoreExploreRight = 0;
		if (((float)random()/((float)RAND_MAX))<0.03) {
			exploreState = (ExploreStates)floor((float)random()/(float)RAND_MAX*EXPLORE_NUM_ITEMS);
		}
		break;
	case EXPLORE_RIGHT:
		mPFC2CoreExploreLeft = 0;
		mPFC2CoreExploreRight = 0.1;
		if (((float)random()/((float)RAND_MAX))<0.03) {
			exploreState = (ExploreStates)floor((float)random()/(float)RAND_MAX*EXPLORE_NUM_ITEMS);
		}
		break;
	case EXPLORE_STOP:
		mPFC2CoreExploreLeft = 0;
		mPFC2CoreExploreRight = 0;
		if (((float)random()/((float)RAND_MAX))<0.01) {
			exploreState = (ExploreStates)floor((float)random()/(float)RAND_MAX*EXPLORE_NUM_ITEMS);
		}
		break;
	case EXPLORE_STRAIGHT:
	default:
		mPFC2CoreExploreLeft = 0.1;
		mPFC2CoreExploreRight = 0.1;
		if (((float)random()/((float)RAND_MAX))<0.05) {
			exploreState = (ExploreStates)floor((float)random()/(float)RAND_MAX*EXPLORE_NUM_ITEMS);
		}
		break;
	}

	//New Green mPFC Neuron doStep
	mPFC_Green = mPFCneuronGreen->doStep(visual_reward_Green + OFC, non_filtered_reward, serotoninConcentration, on_contact_direction_Green);
	
	//New Blue mPFC Neuron doStep
	mPFC_Blue = mPFCneuronBlue->doStep(visual_reward_Blue + OFC, non_filtered_reward, serotoninConcentration, on_contact_direction_Blue);

	//Code for logging new variables 	
	mPFCg_LTP_gainG = mPFCneuronGreen->getLTP_gain(0);
	mPFCg_LTD_gainG = mPFCneuronGreen->getLTD_gain(0);	
	mPFCb_LTP_gainB = mPFCneuronBlue->getLTP_gain(0);
	mPFCb_LTD_gainB = mPFCneuronBlue->getLTD_gain(0);		
	OFC_LTP_gainG = OFCNeuron->getLTP_gain(0);
	OFC_LTD_gainG = OFCNeuron->getLTD_gain(0);
	OFC_LTP_gainB = OFCNeuron->getLTP_gain(1);
	OFC_LTD_gainB = OFCNeuron->getLTD_gain(1);

	lShell = placefieldGreen * lShell_weight_pflg + placefieldBlue * lShell_weight_pfdg;

	shell_DA = VTA;
	
	shell_plasticity = shell_DA - VTA_zero_val;
	weightChange(lShell_weight_pflg, learning_rate_lshell * shell_plasticity * placefieldGreen);
	weightChange(lShell_weight_pfdg, learning_rate_lshell * shell_plasticity * placefieldBlue);

	dlVP = 1/(1+lShell * shunting_inhibition_factor);

	EP = 1/(1+dlVP * shunting_inhibition_factor);

	LHb = EP + LHB_BIAS;

	RMTg = LHb;

	CoreGreenOut= (mPFC_Green * core_weight_lg2lg);
	
	CoreBlueOut= (mPFC_Blue * core_weight_dg2dg);

	float core_threshold = 0.01;
	if (CoreGreenOut < core_threshold) CoreGreenOut = 0;
	if (CoreBlueOut < core_threshold) CoreBlueOut = 0;

	if (CoreGreenOut > 1) CoreGreenOut = 1;
	if (CoreBlueOut > 1) CoreBlueOut = 1;

	core_DA = VTA;
	core_plasticity = core_DA - VTA_zero_val;
	weightChange(core_weight_lg2lg, learning_rate_core * core_plasticity * mPFC_Green);
	weightChange(core_weight_dg2dg, learning_rate_core * core_plasticity * mPFC_Blue);

	if ((CoreGreenOut > 0.05)||(CoreBlueOut > 0.05)) {
		mPFC2CoreExploreLeft = 0; 
		mPFC2CoreExploreRight = 0; 
	}

	logging();
	
	step++;  
}	

void Limbic_system::logging() {
	fprintf(flog,"%ld %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f "
		"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d\n",
		step, //0
		reward, //1
		placefieldGreen, //2
		placefieldBlue, //3
		on_contact_direction_Green, //4
		on_contact_direction_Blue, //5
		visual_direction_Green_trace, //6
		visual_direction_Blue_trace, //7
		core_weight_lg2lg, //8
		mPFC_Green, //9
		mPFC_Blue, //10
		core_weight_dg2dg, //11
		CoreGreenOut, //12
		CoreBlueOut, //13
		VTA,//14
		core_plasticity,//15
		shell_plasticity,//16
		lShell_weight_pflg,//17
		lShell_weight_pfdg,//18
		dlVP,//19
		EP,//20
		LHb,//21
		RMTg,//22
		pfLg2OFC,//23
		pfDg2OFC, //24
		DRN, //25
		visual_reward_Green, // 26
		visual_reward_Blue, //27
		OFC, // 28
		serotoninConcentration, // 29
		OFCNeuron->getWeight(0), // 30
		OFCNeuron->getWeight(1), // 31
		mPFCneuronGreen->getWeight(0), // 32
		mPFCneuronBlue->getWeight(0), // 33
		);
	fflush(flog);
}


/*

// Logging all variables including those relevant to the reinforcement and unexpected reward mechansisms. However, can not be used with the python plotting file plot_reversal.py

void Limbic_system::logging() {
	fprintf(flog,"%ld %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f"
		"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d %f"
		" %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
		step, //0
		reward, //1
		placefieldGreen, //2
		placefieldBlue, //3
		on_contact_direction_Green, //4
		on_contact_direction_Blue, //5
		visual_direction_Green_trace, //6
		visual_direction_Blue_trace, //7
		core_weight_lg2lg, //8
		mPFC_Green, //9
		mPFC_Blue, //10
		core_weight_dg2dg, //11
		CoreGreenOut, //12
		CoreBlueOut, //13
		VTA,//14
		core_plasticity,//15
		shell_plasticity,//16
		lShell_weight_pflg,//17
		lShell_weight_pfdg,//18
		dlVP,//19
		EP,//20
		LHb,//21
		RMTg,//22
		pfLg2OFC,//23
		pfDg2OFC, //24
		DRN, //25
		visual_reward_Green, // 26
		visual_reward_Blue, //27
		OFC, // 28
		serotoninConcentration, // 29
		OFCNeuron->getWeight(0), // 30
		OFCNeuron->getWeight(1), // 31
		mPFCneuronGreen->getWeight(0), // 32
		mPFCneuronBlue->getWeight(0), // 33
		expected, //34
		mPFCg_LTP_gainG, //35
		mPFCg_LTD_gainG, //36
		mPFCb_LTP_gainB, //37
		mPFCb_LTD_gainB, //38
		OFC_LTP_gainG, //39
		OFC_LTD_gainG, //40
		OFC_LTP_gainB, //41
		OFC_LTD_gainB,  //42
		OFC_green_past, //43
		OFC_blue_past,  //44
		mPFCg_green_past, //45
		mPFCg_blue_past, //46
		mPFCb_green_past, //47
		mPFCb_blue_past, //48
		);
	fflush(flog);
}

*/
