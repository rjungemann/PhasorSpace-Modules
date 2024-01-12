#include "plugin.hpp"


struct StochasticRampFilter : Module {
	enum ParamId {
		PROB_PARAM,
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

	StochasticRampFilter() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PROB_PARAM, 0.f, 10.f, 5.f, "");
		configInput(CV_INPUT, "");
		configInput(RAMPIN_INPUT, "");
		configOutput(RAMPOUT_OUTPUT, "");
		configOutput(TRIG_OUTPUT, "");
	}

	float prevRampin = 0.f;
	bool isTriggered = false;

	void process(const ProcessArgs& args) override {
		// If incoming ramp is going down, recalculate probability
		float rampin = inputs[RAMPIN_INPUT].getVoltage();
		if (prevRampin > rampin) {
			float prob = params[PROB_PARAM].getValue();
			prob += inputs[CV_INPUT].getVoltage();
			float v = 10.f * random::uniform();

			isTriggered = false;
			if (v < prob) {
				isTriggered = true;
			}

			outputs[TRIG_OUTPUT].setVoltage(5.f);
		} else {
			outputs[TRIG_OUTPUT].setVoltage(0.f);
		}
		// If triggered, output incoming ramp, otherwise output nothing
		if (isTriggered) {
			outputs[RAMPOUT_OUTPUT].setVoltage(rampin);
			lights[BLINK_LIGHT].setBrightness(1.f);
		} else {
			outputs[RAMPOUT_OUTPUT].setVoltage(0.f);
			lights[BLINK_LIGHT].setBrightness(0.f);
		}

		prevRampin = rampin;
	}
};


struct StochasticRampFilterWidget : ModuleWidget {
	StochasticRampFilterWidget(StochasticRampFilter* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/StochasticRampFilter.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(9.948, 36.009)), module, StochasticRampFilter::PROB_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.948, 59.486)), module, StochasticRampFilter::CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.064, 78.996)), module, StochasticRampFilter::RAMPIN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(4.956, 100.775)), module, StochasticRampFilter::RAMPOUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.916, 100.77)), module, StochasticRampFilter::TRIG_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(9.948, 23.164)), module, StochasticRampFilter::BLINK_LIGHT));
	}
};


Model* modelStochasticRampFilter = createModel<StochasticRampFilter, StochasticRampFilterWidget>("StochasticRampFilter");