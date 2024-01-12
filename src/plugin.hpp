#pragma once
#include <rack.hpp>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
extern Model* modelStochasticRampFilter;
extern Model* modelSwungRampGenerator;
extern Model* modelPhasedRampDistorter;
extern Model* modelPitchedRampGenerator;
extern Model* modelAnalogSwizzlingRegister;
