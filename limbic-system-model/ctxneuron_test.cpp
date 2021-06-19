#include "ctxneuron.h"
#include "filter.h"
#include <iostream>
#include <fstream>
#include <cmath>
			
using namespace std;

//Creating test inputs to be passed into ctxneuron: reward input, chosen visual stimuli input (rises and then spikes as agent runs towards it), alternative visual stimuli (rises and then falls as agent runs towards other stimuli). Variables on_contact_direction_green and on_contact_direction_blue are either set equal to the reward function or to 0, depending on the stage. In the full simulation, the reward input is "smeared" using a 2nd order low pass filter (f=0.1) and so it is not a perfect step function (reflecting biological transduction nature), however, as the test approximates it as a step function of the same size, this function is a good approximation of the contact variables which are actually step functions of the same width as the reward spikes.

double PolynomialFitChosen(int xg){ 
	double yg= (3.2237158889784859e-002)+((3.0670253391877969e-003)*(xg))+((-4.2808791788562532e-005)*(pow(xg,2)))+((3.2004943356100369e-006)*(pow(xg,3)))+((-9.1712058546080572e-008)*(pow(xg,4)))+((1.2049417989685665e-009)*(pow(xg,5)))+((-8.3188212195307858e-012)*(pow(xg,6)))+((2.8447725301565388e-014)*(pow(xg,7)))+((-1.2159647732298433e-017)*(pow(xg,8)))+((-2.8753630707559631e-019)*(pow(xg,9)))+((1.1842620988729465e-021)*(pow(xg,10)))+((-2.2421435153006369e-024)*(pow(xg,11)))+((2.1577152143963509e-027)*(pow(xg,12)))+((-8.5181952522742884e-031)*(pow(xg,13)));
	if (yg<0){
		yg= 0;
	} return yg;
}

double PolynomialFitAlt(int xb){ 
	double yb= (-3.0831225747954435e-002)+((1.1734471790796466e-002)*(xb))+((-4.6164786789105615e-004)*(pow(xb,2)))+((1.1818067803547473e-005)*(pow(xb,3)))+((-1.7479750166927092e-007)*(pow(xb,4)))+((1.5638991254951781e-009)*(pow(xb,5)))+((-8.9541519203471321e-012)*(pow(xb,6)))+((3.4220648454500508e-014)*(pow(xb,7)))+((-8.9507822906472904e-017)*(pow(xb,8)))+((1.6072331418440003e-019)*(pow(xb,9)))+((-1.9204642433495965e-022)*(pow(xb,10)))+((1.3867539847620216e-025)*(pow(xb,11)))+((-4.5854470675599626e-029)*(pow(xb,12)));
	if (yb<0){
		yb= 0;			
	} return yb;
}

int FitR(int xr){
	int yr = 0;
	if ((359<=xr)&&(xr<500)){          
		yr = 1;
	} return yr;
}

//The Reset function is needed to reset the input variables in stages 2-6 after every 502 steps. This is because the test input functions are based on one typical constellation from the full simulation that is of that size, compared to the 5020 steps each stage (2-6) lasts for.
int Reset(int _x){
	_x =_x%502;  
	return _x;
}   	

int main() {
	int step, i, x;  
	int NPInput;
	float serot;
	float InputG, InputB;
	float on_contact_direction_green, on_contact_direction_blue;
	const float learning_rate_OFC = 1;	
	float success;	
	int expected;
	float weightG, weightB;
	float OFC; 
  
  	//Unlike in the full simulation, in this test, the value for serotonin concentration is static. To aid testing of ctxneuron class behaviour with changng serotonin, the variable is input by the user during run-time.
	cout << "Enter 5ht conc: ";
	cin >> serot;

	//Logging results to data text file
	ofstream f("ctxneuron_test.dat");
	
	//Testing the behaviour of an OFC neuron specifically, the learning rates are 1 (LTP) and 0.1 (LTD), the decay rate (tLTD) goes to its default value and plastic (visual) inputs are added for the green and blue stimuli.
	CtxNeuron* TestCtxNeuron = new CtxNeuron(learning_rate_OFC,learning_rate_OFC * 0.1);
	TestCtxNeuron->addInput(InputG);
	TestCtxNeuron->addInput(InputB);
  
	//There are 7 test stages.
	for(i=0; i<27108; i++) {
		step = i;    
		// Stages 1 and 7. Stage 1 is "Zero Input" and stage 7 is "Zero Input2": All Inputs at zero.
		if (((0<=i)&&(i<1004))||((26104<=i)&&(i<27108))) { 	
			NPInput = 0;
			InputG = 0;
			InputB = 0;
			on_contact_direction_green = 0;
			on_contact_direction_blue = 0;  
		// Stages 2 and 6. Stage 2 is "Initial Learning" and stage 6 is "Re-Learning After LTD": 10 Rewards coincide with green stimuli. 
		} else if (((1004<=i)&&(i<6024))||((21084<=i)&&(i<26104))){
			x = Reset(i);
			InputG = PolynomialFitChosen(x);
			InputB = PolynomialFitAlt(x);
			NPInput = FitR(x);        
			on_contact_direction_green = FitR(x);
			on_contact_direction_blue = 0;  
		//Stage 3. "Reversal Learning 1": 10 Rewards coincide with blue stimuli.
		} else if ((6024<=i)&&(i<11044)){
			x = Reset(i);
			InputB = PolynomialFitChosen(x);
			InputG = PolynomialFitAlt(x);
			NPInput = FitR(x);        
			on_contact_direction_green = 0;
			on_contact_direction_blue = FitR(x);
		//Stage 4. "Reversal Learning 2": 10 Rewards coincide with green stimuli.     
		} else if ((11044<=i)&&(i<16064)){			
			x = Reset(i);
			InputG = PolynomialFitChosen(x);
			InputB = PolynomialFitAlt(x);
			NPInput = FitR(x);        
			on_contact_direction_green = FitR(x);
			on_contact_direction_blue = 0;      
		//Stage 5. "Association Decay- LTD": 10 Unrewarded green stimuli spikes  
		} else if ((16064<=i)&&(i<21084)){			
			x = Reset(i);
			InputG = PolynomialFitChosen(x);
			InputB = PolynomialFitAlt(x);
			NPInput =0;        
			on_contact_direction_green = FitR(x);
			on_contact_direction_blue = 0;
		}  
      
      		//Unlike in the full simulation, the reward signal is not filtered and is therefore suitable for the success variable.
		success = NPInput; 
      
      		//Passing generated inputs into class every step.
		OFC = TestCtxNeuron->doStep(NPInput, success, serot+0.1, on_contact_direction_green, on_contact_direction_blue);
		
		//Applying OFC limit.
		if (OFC > 0.25) {
			OFC = 0.25;
		}
		
		//Unexpected Reward Mechanism.
		expected = TestCtxNeuron->getExpected();
		if (expected == -1) {
			OFC += 0.2;
		}
	
		//Getting the weights of the synaptic connections of the plastic inputs for logging and plotting.
		weightG = TestCtxNeuron->getWeight(0);
		weightB = TestCtxNeuron->getWeight(1);

		//Logging data (per step) onto data text file.
		f <<step<<" "<<NPInput<<" "<<InputG<<" "<<InputB<<" "<<OFC<<" "<<weightG<<" "<<weightB<<" \n";  
 
	} f.close(); //Finished test, closing data file.
}
