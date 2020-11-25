//At the moment, static 5ht conc. - need to update + will sort into functions for appearance if needed
#include "ctxneuron.h"
#include "filter.h"
#include <iostream>
#include <stdio.h>
#include <math.h>
SecondOrderLowpassFilter* slowCaDetector;

int main() {
  int nInputs = 5000; //5000 simulated steps
  int NonPlasticInputs[nInputs];
  float inputs[nInputs];
  float weights[nInputs];
  float serot;
  float initWeight = 1;  // value: 1 (when weight change function working -> 0)
  std::cout<< "Enter 5ht conc: ";
  std::cin>> serot;
/*Filling weight array (until working)
  Generating smooth plastic (inputs[i]) test input, for either green or blue, using positive only sin function, max 0.7, double period (max every 720 steps)
  Generating non-plastic (NonPlasticInputs[i]) test input, simulating irregular stepwise function: R: 0->1 
  Moveable reward spikes to test different characteristics*/
  for(int i = 0; i<nInputs; i++) {
    weights[i]={initWeight};  
    inputs[i]= 0.7*sin(0.5*i);
    if (inputs[i] < 0) {
        inputs[i] = 0;
    }
    if (((0<=i)&&(i <100)) || ((500<=i)&&(i <600)) || ((1000<=i)&&(i<1100)) || ((1300<=i)&&(i<1400)) || ((1600<= i)&&(i<1700)) || ((4000<=i)&&(i<4100))) {
      NonPlasticInputs[i] = 1;
    } else {
      NonPlasticInputs[i] = 0;
    }	
  }
  //Opening write-only file for outputs to be logged to
  FILE* f = fopen("test_ctxneuron.dat","wt");
//CtxNeuron processing: No simulation- instead of DoStep, processing in loop per inputs array entry 
  float output[nInputs];
  float output2[nInputs];
  float doutput[nInputs];
  float slowCa[nInputs]; //won't work if no resulting entry for high pass 
   for(int i = 0; i<nInputs; i++) {
       output[i] = NonPlasticInputs[i] + (inputs[i]*weights[i]);  
       output[i] = CtxNeuron::ofc5HTreceptors(output[i],serot,serot*2); 
       doutput[i]= output[i] - output2[i];
       if (doutput[i] < 0) {
          doutput[i] = 0;
       }
//       slowCa[i]= slowCaDetector->filter(output[i]); //only will work if filter based on instantaneous scaler
//       slowCa[i]= fabs(slowCa[i]);
//       if (NonPlasticInputs[i] > 0.25) { 
      
       /*Main Issue here (1): have taken out learningRateLTP, tLTD, learningRateLTD for now (just to see if it was running) because I couldn't find their origins/how to
         simulate them- just need some info on how they're formulated
      
         Main Issue here (2): lines 57-58: recieving "error: qualified-id in declaration before ‘(’ token" when trying to make- have made the weightchange function 
         public in my git test branch but no luck, the other solutions I saw was to take lines 57 & 58 out of int main() but I'm not sure I can do that with 
         my looping set-up/ I don't know why it works for ofc5htreceptor and not weight change */
      
//       void CtxNeuron::weightChange(weights[i], serot * inputs[i] * doutput[i]);      
//       }
//       else {
//       void CtxNeuron::weightChange(weights[i], slowCa[i]);
//       }
       fprintf(f,"%d %d %f %f %f\n",i,NonPlasticInputs[i],inputs[i],weights[i],doutput[i]); //checking doutput until weight change working
       output2[i] = output[i];
   }	
  fclose(f);
}


//Of this version: No compile-, link- or run- time errors


