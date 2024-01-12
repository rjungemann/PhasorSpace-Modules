#include "plugin.hpp"


struct PitchedRampGenerator : Module {
	enum ParamId {
		VOCT_PARAM,
		PW_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		VOCTCV_INPUT,
		PWCV_INPUT,
		SOFT_INPUT,
		HARD_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		RAMP_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		BLINK_LIGHT,
		LIGHTS_LEN
	};

	PitchedRampGenerator() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(VOCT_PARAM, -54.f, 54.f, 0.f, "Frequency", " Hz", dsp::FREQ_SEMITONE, dsp::FREQ_C4);
		configParam(PW_PARAM, 0.f, 1.f, 1.f, "");
		configInput(VOCTCV_INPUT, "");
		configInput(PWCV_INPUT, "");
		configInput(SOFT_INPUT, "");
		configInput(HARD_INPUT, "");
		configOutput(RAMP_OUTPUT, "");
	}

	float lastSyncValue = 0.f;
	float freq = 0.f;
	float phase = 0.f;
	float syncDirection = 1.f;

	dsp::BooleanTrigger softTrigger;
	dsp::BooleanTrigger hardTrigger;
	dsp::PulseGenerator softPulseGenerator;
	dsp::PulseGenerator hardPulseGenerator;

	void process(const ProcessArgs& args) override {
		float pitch = (params[VOCT_PARAM].getValue() / 12.f) + inputs[VOCTCV_INPUT].getVoltage();
		float freq = dsp::FREQ_C4 * dsp::exp2_taylor5(pitch);
		float pw = params[PW_PARAM].getValue() + inputs[PWCV_INPUT].getVoltage();

		// Process soft reset input
		bool softTap = inputs[SOFT_INPUT].getVoltage() > 0.f;
		if (softTrigger.process(softTap)) {
			softPulseGenerator.trigger(1e-3f);
		}
		bool softPulse = softPulseGenerator.process(args.sampleTime);

		// Process hard reset input
		bool hardTap = inputs[HARD_INPUT].getVoltage() > 0.f;
		if (hardTrigger.process(hardTap)) {
			hardPulseGenerator.trigger(1e-3f);
		}
		bool hardPulse = hardPulseGenerator.process(args.sampleTime);

		// lights[BLINK_LIGHT].setBrightness(softPulse ? 1.f : 0.f);
		// lights[BLINK_LIGHT].setBrightness(1.f);

		// float sampleRate = APP->engine->getSampleRate();
		// float voct = params[VOCT_PARAM].getValue() + inputs[VOCTCV_INPUT].getVoltage();

		// Advance phase
		float deltaPhase = freq * args.sampleTime;
		if (softPulse) {
			// Reverse direction
			syncDirection = -syncDirection;
		}
		if (hardPulse) {
			phase = 0.f;
			syncDirection = 1.f;
		}
		phase += deltaPhase * syncDirection;
		// Wrap phase
		phase -= simd::floor(phase);

		float output = phase * (1.f / pw);
		output = output > 1.f ? 0.f : output;

		outputs[RAMP_OUTPUT].setVoltage(output * 10.f);
		lights[BLINK_LIGHT].setBrightness(output < 0.5 ? 1.f : 0.f);

		// outputs[RAMP_OUTPUT].setVoltage(phase * 10.f);
		// lights[BLINK_LIGHT].setBrightness(phase < 0.5 ? 1.f : 0.f);
	}
};


struct PitchedRampGeneratorWidget : ModuleWidget {
	PitchedRampGeneratorWidget(PitchedRampGenerator* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PitchedRampGenerator.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(4.902, 36.009)), module, PitchedRampGenerator::VOCT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.843, 36.009)), module, PitchedRampGenerator::PW_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(4.99, 59.486)), module, PitchedRampGenerator::VOCTCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(14.932, 59.486)), module, PitchedRampGenerator::PWCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(4.866, 78.395)), module, PitchedRampGenerator::SOFT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(14.864, 78.395)), module, PitchedRampGenerator::HARD_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.058, 100.775)), module, PitchedRampGenerator::RAMP_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(9.948, 23.164)), module, PitchedRampGenerator::BLINK_LIGHT));
	}
};


Model* modelPitchedRampGenerator = createModel<PitchedRampGenerator, PitchedRampGeneratorWidget>("PitchedRampGenerator");