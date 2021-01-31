#include "ctxneuron.h"
#include "filter.h"
#include <iostream>
#include <fstream>
#include <cmath>
			
using namespace std;

double PolynomialFitRewarded(int xg){ 
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

int Reset(int _x){
  _x =_x%502;  
  return _x;
} 
  	
int main() {
  int NPInput,step,i,x; 
  float serot,InputG,InputB,OFC = 0,weightG = 0,weightB = 0;
  const float learning_rate_OFC = 1;
  cout<< "Enter 5ht conc: ";
  cin>> serot;
  ofstream f("ctxneuron_test.dat");
  CtxNeuron* TestCtxNeuron = new CtxNeuron(learning_rate_OFC,learning_rate_OFC * 0.1);
  TestCtxNeuron->addInput(InputG);
  TestCtxNeuron->addInput(InputB);   
    for(i=0; i<22088; i++) {
    step = i;
      if (((0<=i)&&(i<1004))||((21084<=i)&&(i<22088))){ 	// Stage 1 & 6
        NPInput = 0;
        InputG = 0;
        InputB = 0;
      } else if ((1004<=i)&&(i<6024)){			        // Stage 2 
        x = Reset(i);
        InputG = PolynomialFitRewarded(x);
        InputB = PolynomialFitAlt(x);
        NPInput = FitR(x);
      } else if ((6024<=i)&&(i<11044)){			        // Stage 3
        x = Reset(i);
        InputB = PolynomialFitRewarded(x);
        InputG = PolynomialFitAlt(x);
        NPInput = FitR(x);
      } else if ((11044<=i)&&(i<16064)){			// Stage 4
        x = Reset(i);
        InputG = PolynomialFitRewarded(x);
        InputB = PolynomialFitAlt(x);
        NPInput = FitR(x);
      } else if ((16064<=i)&&(i<21084)){			// Stage 5
        x = Reset(i);
        InputG = PolynomialFitRewarded(x);
        InputB = PolynomialFitAlt(x);
        NPInput =0;
      }  
      OFC = TestCtxNeuron->doStep(NPInput,serot+0.1);
      weightG = TestCtxNeuron->getWeight(0);
      weightB = TestCtxNeuron->getWeight(1);
      if (OFC > 0.25) {
      OFC = 0.25;
      } f <<step<<" "<<NPInput<<" "<<InputG<<" "<<InputB<<" "<<OFC<<" "<<weightG<<" "<<weightB<<" \n";
    } f.close();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
This program is designed to test the Ctx Neuron class with stereotypical environmental RL paradigm inputs (Rewards and Associated Stimuli(G/B)) and log the 
outputs. This test will allow for monitoring of alterations made to the class. Outputs can be plotted with [./plot_test.py].

The program tests 6 stages: 
	1. Zero Input: All Inputs at zero- All outputs should be zero.
	2. LN->LE: 10 Rewards coincide with green stimuli- Green OFC synapse weight should increase, OFC value should respond to rewards and 
	   should also
	   increasingly respond to green stimuli. Highlights LTP speed. 
	3. RN->RE: Switch1, 10 Rewards coincide with blue stimuli- Blue OFC synapse weight should increase, green OFC synapse weight should decrease,
	   OFC value should respond to rewards and should also increasingly respond to blue visual stimuli + decreasingly respond to green. LTP and LTD. 
	4. R2N->R2E: Switch 2, 10 Rewards coincide with green stimuli- Inverse of stage 3 effects + possible increased rates of change (task familiarity)
	5. Association Decay: 10 Unrewarded green stimuli spikes- Inverse of stage 2 effects. Highlights LTD speed.
	6. Zero Input2: All inputs at zero- Zero OFC activation, weights should remain constant.	

Timing of Inputs:
  As the agent moves forward to approach the placefields, there is an increase in both green and blue visual stimuli. The agent then  moves toward + turns
  toward the rewarding visual stimulus- this further increases the rewarding stimulus input and, as the agent is no longer looking at the non-rewarding
  stimulus, the non-rewarding visual stimulus input falls to zero. The agent now enters the reawrding stimulus placefield and collects the reward- this prompts
  the rewarding visual stimulus input to peak at ~0.6 and the reward stimulus (indicating collection of reward by agent) goes to 1. After reward collection,
  the agent is returned to the starting point and all inputs return to zero. 

Inputs:
  In the polynomial fits, there will be some error due to limitation in computer arithmetic which uses shifting mantissas to add and subtract values (in this 
  case, of large magnitude range)- due to multiplication by increasing x exponents (of increasing x) within each term, the terms affected are difficult to 
  narrow down and avoid => trade off within accuracy. Therefore, the least degree of polynomials that would reliably present a peak >0.6 for rewarded stimuli
  and would affect the cortical neuron program most similarly to the selected, average input constellation were used (alt:deg12, rewarded:deg13). The average
  constellation selected for input modelling was chosen from ouput data from a full-run reversal learning simulation using a method based on beat-typing. 
  There is also discrepancy in the peak value of the rewarded visual stimulus: in this test the peak value is 0.603613 whereas in the average constellatio,
  the peak value is 0.67753. This will also negatively affect the accuracy and usefulness of the test. 

  As the agent moves forward to approach the placefields, there is an increase in both green and blue visual stimuli. The agent then  moves toward + turns
  toward the rewarding visual stimulus- this further increases the rewarding stimulus input and, as the agent is no longer looking at the non-rewarding
  stimulus, the non-rewarding visual stimulus input falls to zero. The agent now enters the reawrding stimulus placefield and collects the reward- this
  prompts the rewarding visual stimulus input to peak at ~0.6 and the reward stimulus (indicating collection of reward by agent) goes to 1. After reward
  collection, the agent is returned to the starting point and all inputs return to zero. 

Inputs:
  In the polynomial fits, there will be some error due to limitation in computer arithmetic which uses shifting mantissas to add and subtract values (in this 
  case, of large magnitude range)- due to multiplication by increasing x exponents (of increasing x) within each term, the terms affected are difficult to 
  narrow down and avoid => trade off within accuracy. Therefore, the least degree of polynomials that would reliably present a peak >0.6 for rewarded stimuli 
  and would affect the cortical neuron program most similarly to the selected, average input constellation were used (alt:deg12, rewarded:deg13). The average
  constellation selected for input modelling was chosen from ouput data from a full-run reversal learning simulation using a method based on beat-typing. 
  There is also discrepancy in the peak value of the rewarded visual stimulus: in this test the peak value is 0.603613 whereas in the average constellatio, the
  peak value is 0.67753. This will also negatively affect the accuracy and usefulness of the test. 

	Polynomial Regression Visual Stimulus rewarded Fit			Polynomial Regression Visual Stimulus alt Fit
	Mode: normal x,y analysis						Mode: normal x,y analysis
	Polynomial degree 13, 503 x,y data pairs.				Polynomial degree 12, 503 x,y data pairs.
	Correlation coefficient = 0.9825837594639034				Correlation coefficient = 0.9839663659104874
	Standard error = 0.01850759098776909					Standard error = 0.013506565373283928
*/
