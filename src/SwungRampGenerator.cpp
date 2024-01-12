#include "plugin.hpp"


struct SwungRampGenerator : Module {
	enum ParamId {
		SWING_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CV_INPUT,
		RAMPIN_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		RAMPOUT_OUTPUT,
		TRIG_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		BLINK_LIGHT,
		LIGHTS_LEN
	};

	SwungRampGenerator() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SWING_PARAM, 0.f, 10.f, 5.f, "");
		configInput(CV_INPUT, "");
		configInput(RAMPIN_INPUT, "");
		configOutput(RAMPOUT_OUTPUT, "");
		configOutput(TRIG_OUTPUT, "");
	}

	float prevOutput = 0.f;

	void process(const ProcessArgs& args) override {
		float rampin = rescale(inputs[RAMPIN_INPUT].getVoltage(), 0.f, 10.f, 0.f, 1.f);
    float swing = rescale(params[SWING_PARAM].getValue(), 0.f, 10.f, 0.f, 1.f)
      + rescale(inputs[CV_INPUT].getVoltage(), 0.f, 10.f, 0.f, 1.f);
    float outputA = rescale(rampin, 0.f, swing, 0.f, 1.f)
      * (rampin < swing ? 1.f : 0.f);
    float outputB = rescale(rampin, swing, 1.f, 0.f, 1.f)
      * (rampin >= swing ? 1.f : 0.f);
    float output = outputA + outputB;
    float trig = prevOutput > output ? 1.f : 0.f;
    outputs[RAMPOUT_OUTPUT].setVoltage(output * 10.f);
    outputs[TRIG_OUTPUT].setVoltage(trig * 10.f);
    lights[BLINK_LIGHT].setBrightness(output < 0.5f ? 1.f : 0.f);
    prevOutput = output;
	}
};


struct SwungRampGeneratorWidget : ModuleWidget {
	SwungRampGeneratorWidget(SwungRampGenerator* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SwungRampGenerator.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(9.948, 36.009)), module, SwungRampGenerator::SWING_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.948, 59.486)), module, SwungRampGenerator::CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.064, 78.996)), module, SwungRampGenerator::RAMPIN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(4.956, 100.775)), module, SwungRampGenerator::RAMPOUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.916, 100.77)), module, SwungRampGenerator::TRIG_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(9.948, 23.164)), module, SwungRampGenerator::BLINK_LIGHT));
	}
};


Model* modelSwungRampGenerator = createModel<SwungRampGenerator, SwungRampGeneratorWidget>("SwungRampGenerator");