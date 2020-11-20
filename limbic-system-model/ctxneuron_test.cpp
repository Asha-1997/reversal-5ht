//At the moment, static 5ht conc. - need to update + will sort into functions for appearance if needed
#include "ctxneuron.h"
#include "filter.h"
#include<stdio.h>
#include<math.h>
SecondOrderLowpassFilter* slowCaDetector;
int argc
char **argv 

int main() {
  if (argc < 2) {
    fprintf(stderr,"We need the 5ht conc\n");
    exit(1);
  }
  float serot = atoi(argv[1]);	
//Generating smooth plastic (inputs[i]) test input, for either green or blue, using positive only sin function, max 0.7
//Generating non-plastic test input, simulating irregular stepwise function: R: 0->1 
  int nInputs = 5000;
  float NonPlasticInputs[nInputs];
  float inputs[nInputs];
  for(int i = 0; i<nInputs; i++) {
      NonPlasticInputs[i]= i;
      inputs[i]= sin(i);
  if (inputs[i] < 0) inputs[i] = 0;
  if (inputs[i] > 0.7) inputs[i] = 0.7;
//moveable reward spikes to test different characteristics
  if (0<= i <100 || 500<= i <600 || 1000<= i <1100 || 1300<= i <1400 || 1600<= i <1700 || 4000<= i <4100) NonPlasticInputs[i] = 1;
  else NonPlasticInputs[i] = 0;
  }	  
//No addInput Function, need to intialise weight and create storage array
  float initWeight = 0;
  float weights[nInputs] = {initWeight}; 
  	
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
      if (doutput[i] < 0) doutput[i] = 0;
      slowCa[i]= slowCaDetector->filter(output[i]); //only will work if filter based on instantaneous scaler
      slowCa[i]= fabs(slowCa[i]);
      if (NonPlasticInputs[i] > 0.25) 
      
      // Issue here (1): have taken out learningRateLTP, tLTD, learningRateLTD for now (just to see if it was running) because I couldn't find their origins/how to
      // simulate them- just need some info on how they're formulated
      
      // Issue here (2): lines 57-58: recieving "error: qualified-id in declaration before ‘(’ token" when trying to make- have made the weightchange function public in my git
      // test branch but no luck, the other solutions I saw was to take lines 57 & 58 out of int main() but I'm not sure I can do that with my looping
      // set-up/ I don't know why it works for ofc5htreceptor and not weight change
      
      void CtxNeuron::weightChange(weights[i], serot * inputs[i] * doutput[i]);      
      else void CtxNeuron::weightChange(weights[i], slowCa[i]);
      fprintf(f,"%d %f %f %f %f\n",i,NonPlasticInputs[i],inputs[i],weights[i],serot);
      output2[i] = output[i];
  }	
  fclose(f);
}

  
