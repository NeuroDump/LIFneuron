/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include <InterruptIn.h>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <math.h>
#include <stdbool.h>
#include <vector>

#define MAX_CONNECTIONS 1 // maximum number of connections per neuron
// Blinking rate in milliseconds
#define BLINKING_RATE 250ms
// Define a neuron
struct Neuron {
  double excitation;
  double inhibition;
  double leakage;
  double threshold;
  double refractoryCounter;
  double refractoryPeriod;
  bool fired;
  int numConnections = 0;
  struct Neuron
      *connections[MAX_CONNECTIONS]; // references to connected neurons
  double weights[MAX_CONNECTIONS];   // weights of each connection
};

//InterruptIn button(BUTTON1);
Timer Epoch;
DigitalOut led(PA_5);
DigitalIn button(BUTTON1);
int buttonCount = 0;


// Initialize a neuron
void init_neuron(struct Neuron *neuron, double excitation, double inhibition,
                 double leakage, double threshold, double refractoryPeriod) {
  neuron->excitation = excitation;
  neuron->inhibition = inhibition;
  neuron->leakage = leakage;
  neuron->threshold = threshold;
  neuron->refractoryPeriod = refractoryPeriod;
  neuron->refractoryCounter = 0;
  neuron->fired = false;
  neuron->numConnections = 0; // initially no connections
}

// Update a neuron
void update_neuron(struct Neuron* neuron, double dt) {
    double net_excitation = neuron->excitation - neuron->inhibition;

    if (neuron->refractoryCounter > 0) {
        neuron->excitation -= neuron->refractoryCounter * dt; // decrease the excitation during the refractory period
        neuron->refractoryCounter -= dt; // decrease the refractory counter over time
    }

    neuron->excitation -= neuron->leakage * dt;
    if (neuron->excitation < 0) neuron->excitation = 0;
    neuron->inhibition -= neuron->leakage * dt;
    if (neuron->inhibition < 0) neuron->inhibition = 0;

    if (net_excitation > neuron->threshold && neuron->refractoryCounter <= 0) {
        neuron->fired = true;
        printf("excite count = %d\r\n",buttonCount);
        printf("threshold = %lf\r\n",neuron->threshold);
        buttonCount = 0;
        led = 1;
        neuron->excitation = 0;
        neuron->inhibition = 0;
        neuron->refractoryCounter = neuron->refractoryPeriod; // set the refractory counter
    } else {
        neuron->fired = false;
        led = 0;
    }

    if (neuron->fired && neuron->numConnections > 0) {
        // If the neuron fired, send a signal to all connected neurons
        for (int i = 0; i < neuron->numConnections; i++) {
            struct Neuron* outputNeuron = neuron->connections[i];
            outputNeuron->excitation += neuron->weights[i];
        }
    }
}


int main() {
  Epoch.start();
  
  // neuron stuff
  struct Neuron neuron1, neuron2;
  init_neuron(&neuron1, 0.0, 0.0, 0.05, 0.5, 2.0);

  while (1) {

     if(button == 0){
         
         neuron1.excitation += 0.3;
        buttonCount++;
         while(button == 0);
     } 

    if (Epoch.elapsed_time() >= std::chrono::microseconds(100000)) {
    //led = 0;
      //trig();
      // update neuron here
    update_neuron(&neuron1, 0.2);
    printf("refractory Counter = %lf ",neuron1.refractoryCounter);
    printf("excitation = %lf\r\n",neuron1.excitation);
      Epoch.reset();
      Epoch.start();
    }
  }

  // Initialise the digital pin LED1 as an output
}



