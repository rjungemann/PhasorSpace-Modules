#include "plugin.hpp"


struct PhasedRampDistorter : Module {
	enum ParamId {
		TILT_PARAM,
		RES_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		TILTCV_INPUT,
		RESCV_INPUT,
		RAMP_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		TRI_OUTPUT,
		SAW_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		BLINK_LIGHT,
		LIGHTS_LEN
	};

	PhasedRampDistorter() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(TILT_PARAM, 0.f, 10.f, 0.f, "");
		configParam(RES_PARAM, 0.f, 10.f, 0.f, "");
		configInput(TILTCV_INPUT, "");
		configInput(RESCV_INPUT, "");
		configInput(RAMP_INPUT, "");
		configOutput(TRI_OUTPUT, "");
		configOutput(SAW_OUTPUT, "");
	}

	const float tau = 2.f * M_PI;

	void process(const ProcessArgs& args) override {
		// r
		float res = rescale(params[RES_PARAM].getValue() + inputs[RESCV_INPUT].getVoltage(), 0.f, 10.f, 1.f, 8.f);
		// o
		float tilt = rescale(params[TILT_PARAM].getValue() + inputs[TILTCV_INPUT].getVoltage(), 0.f, 10.f, 0.f, 1.f);
		// t
		float ramp = rescale(inputs[RAMP_INPUT].getVoltage(), 0.f, 10.f, 0.f, 1.f);
		float u = fmod(ramp * res, 1.f);
		// v
		float triangleWindow = 1.f - (2.f * (ramp < 0.5 ? ramp : 1.f - ramp));
		float k = pow(1.f - u, 1.f - tilt);
		float tri = (sin(-k * tau) * (1.f - triangleWindow)) * -1.f;
		float saw = (sin(-k * tau) * (1.f - ramp)) * -1.f;
		outputs[TRI_OUTPUT].setVoltage(tri * 5.f);
		outputs[SAW_OUTPUT].setVoltage(saw * 5.f);
		lights[BLINK_LIGHT].setBrightness(ramp < 0.5 ? 1.f : 0.f);
	}
};


struct PhasedRampDistorterWidget : ModuleWidget {
	PhasedRampDistorterWidget(PhasedRampDistorter* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PhasedRampDistorter.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(4.902, 36.009)), module, PhasedRampDistorter::TILT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.843, 36.009)), module, PhasedRampDistorter::RES_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(4.99, 59.486)), module, PhasedRampDistorter::TILTCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(14.932, 59.486)), module, PhasedRampDistorter::RESCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.064, 78.996)), module, PhasedRampDistorter::RAMP_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(4.968, 100.775)), module, PhasedRampDistorter::TRI_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.99, 100.838)), module, PhasedRampDistorter::SAW_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(9.948, 23.164)), module, PhasedRampDistorter::BLINK_LIGHT));
	}
};


Model* modelPhasedRampDistorter = createModel<PhasedRampDistorter, PhasedRampDistorterWidget>("PhasedRampDistorter");