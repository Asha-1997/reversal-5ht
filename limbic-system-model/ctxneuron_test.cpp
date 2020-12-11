#include "ctxneuron.h"
#include "filter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

//To run this program, need: log_26.dat, log_6.dat and log_7.dat in working directory- all can be found in limbic-system-model folder 

using namespace std; 

int main() {
    const int nSteps = 27000;    	
    int NPInput; 
    float serot, InputG, InputB, OFC = 0, weightG = 0, weightB = 0;
    const float learning_rate_OFC = 1;
      
    cout<< "Enter 5ht conc: ";
    cin>> serot;

    string line, R[nSteps+1], IG[nSteps+1], IB[nSteps+1];
    int i=0,j=0,k=0;
    ifstream f1("log_26.dat");
    while ( getline (f1,line) ) {
      R[i] = line;
      i++;
      } f1.close();
    ifstream f2("log_6.dat");
    while ( getline (f2,line) ) {
      IG[j] = line;
      j++;
      } f2.close();
    ifstream f3("log_7.dat");
    while ( getline (f3,line) ) {
      IB[k] = line;
      k++;
      } f3.close();
    
    ofstream f("ctxneuron_test.dat");

    CtxNeuron* TestCtxNeuron = new CtxNeuron(learning_rate_OFC,learning_rate_OFC * 0.1);
    TestCtxNeuron->addInput(InputG);
    TestCtxNeuron->addInput(InputB);    
   
    for(int i = 0; i<nSteps+1; i++) {
	    
      stringstream temp1(R[i]); 
      temp1>>NPInput; 
      stringstream temp2(IG[i]);
      temp2>>InputG;
      stringstream temp3(IB[i]);
      temp3>>InputB;
    
      OFC = TestCtxNeuron->doStep(NPInput,serot+0.1);
      weightG = TestCtxNeuron->getWeight(0);
      weightB = TestCtxNeuron->getWeight(1);
        if (OFC > 0.25) {
	  OFC = 0.25;
        }
      f <<i<<" "<<NPInput<<" "<<InputG<<" "<<InputB<<" "<<OFC<<" "<<weightG<<" "<<weightB<<" \n";
    }  
    f.close();
  }


//gnuplot
//plot "ctxneuron_test.dat" using 1:2 w l, "ctxneuron_test.dat" using 1:3 w l, "ctxneuron_test.dat" using 1:4 w l,"ctxneuron_test.dat" using 1:5 w l, "ctxneuron_test.dat" using 1:6 w l, "ctxneuron_test.dat" using 1:7 w l
//--
//python plot


