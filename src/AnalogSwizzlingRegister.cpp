#include "plugin.hpp"


struct AnalogSwizzlingRegister : Module {
	enum ParamId {
		PRESET_PARAM,
		BUTTON1_PARAM,
		BUTTON2_PARAM,
		BUTTON3_PARAM,
		BUTTON4_PARAM,
		BUTTON5_PARAM,
		BUTTON6_PARAM,
		BUTTON7_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		INDEX_INPUT,
		NEXT_INPUT,
		RESET_INPUT,
		FREQ_INPUT,
		INPUT_INPUT,
		CLOCK_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		CV1_OUTPUT,
		CV2_OUTPUT,
		CV3_OUTPUT,
		CV4_OUTPUT,
		CV5_OUTPUT,
		CV6_OUTPUT,
		CV7_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		PRESETA_LIGHT,
		PRESETB_LIGHT,
		PRESETC_LIGHT,
		BLINK1A_LIGHT,
		BLINK1B_LIGHT,
		BLINK1C_LIGHT,
		BLINK2A_LIGHT,
		BLINK2B_LIGHT,
		BLINK2C_LIGHT,
		BLINK3A_LIGHT,
		BLINK3B_LIGHT,
		BLINK3C_LIGHT,
		BLINK4A_LIGHT,
		BLINK4B_LIGHT,
		BLINK4C_LIGHT,
		BLINK5A_LIGHT,
		BLINK5B_LIGHT,
		BLINK5C_LIGHT,
		BLINK6A_LIGHT,
		BLINK6B_LIGHT,
		BLINK6C_LIGHT,
		BLINK7A_LIGHT,
		BLINK7B_LIGHT,
		BLINK7C_LIGHT,
		LIGHTS_LEN
	};

	int prevPresetPressed;
	int prevPreset1Pressed;
	int prevPreset2Pressed;
	int prevPreset3Pressed;
	int prevPreset4Pressed;
	int prevPreset5Pressed;
	int prevPreset6Pressed;
	int prevPreset7Pressed;
	int presetIndex;
	std::vector<std::vector<int64_t>> presets;

	int index;
	std::vector<float> values;

	dsp::BooleanTrigger nextTrigger;
	dsp::PulseGenerator nextPulseGenerator;
	dsp::BooleanTrigger resetTrigger;
	dsp::PulseGenerator resetPulseGenerator;
	dsp::BooleanTrigger clockTrigger;
	dsp::PulseGenerator clockPulseGenerator;

	AnalogSwizzlingRegister() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PRESET_PARAM, 0.f, 1.f, 0.f, "");
		configParam(BUTTON1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(BUTTON2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(BUTTON3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(BUTTON4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(BUTTON5_PARAM, 0.f, 1.f, 0.f, "");
		configParam(BUTTON6_PARAM, 0.f, 1.f, 0.f, "");
		configParam(BUTTON7_PARAM, 0.f, 1.f, 0.f, "");
		configInput(INDEX_INPUT, "");
		configInput(NEXT_INPUT, "");
		configInput(RESET_INPUT, "");
		configInput(FREQ_INPUT, "");
		configInput(INPUT_INPUT, "");
		configInput(CLOCK_INPUT, "");
		configOutput(CV1_OUTPUT, "");
		configOutput(CV2_OUTPUT, "");
		configOutput(CV3_OUTPUT, "");
		configOutput(CV4_OUTPUT, "");
		configOutput(CV5_OUTPUT, "");
		configOutput(CV6_OUTPUT, "");
		configOutput(CV7_OUTPUT, "");

		prevPresetPressed = 0;
		prevPreset1Pressed = 0;
		presetIndex = 0;
		presets = {
			{ 1, 2, 3, 4, 5, 6, 7 },
			{ 7, 6, 5, 4, 3, 2, 1 },
			{ 0, 0, 0, 0, 0, 0, 0 },
			{ 1, 7, 2, 6, 3, 5, 4 },
			{ 4, 5, 3, 6, 2, 7, 1 },
			{ 1, 1, 3, 3, 5, 5, 7 },
			{ 2, 2, 4, 4, 6, 6, 7 }
		};

		index = 0;
		values = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	}

	// TODO: Index, Freq, and Input inputs
	// TODO: Persisting preset data
	void process(const ProcessArgs& args) override {
		// Process "next" input
		bool nextTap = inputs[NEXT_INPUT].getVoltage() > 0.f;
		if (nextTrigger.process(nextTap)) {
			nextPulseGenerator.trigger(1e-3f);
		}
		bool nextPulse = nextPulseGenerator.process(args.sampleTime);
		if (nextPulse) {
			presetIndex++;
			if (presetIndex >= 7) {
				presetIndex = 0;
			}
		}

		// Process "reset" input
		bool resetTap = inputs[RESET_INPUT].getVoltage() > 0.f;
		if (resetTrigger.process(resetTap)) {
			resetPulseGenerator.trigger(1e-3f);
		}
		bool resetPulse = resetPulseGenerator.process(args.sampleTime);
		if (resetPulse) {
			presetIndex = 0;
		}

		// Process clock input
		bool clockTap = inputs[CLOCK_INPUT].getVoltage() > 0.f;
		if (clockTrigger.process(clockTap)) {
			clockPulseGenerator.trigger(1e-3f);
		}
		bool clockPulse = clockPulseGenerator.process(args.sampleTime);
		if (clockPulse) {
			values[index] = inputs[INPUT_INPUT].getVoltage();

			index++;
			if (index >= 7) {
				index = 0;
			}
		}

		// Switch to next preset
		float presetPressed = params[PRESET_PARAM].getValue() > 0.f;
		if ( presetPressed > 0 && prevPresetPressed != presetPressed ) {
				presetIndex++;
				if (presetIndex >= 7) {
					presetIndex = 0;
				}
		}
		prevPresetPressed = presetPressed;

		// For input tap 1, map it from current output to next output
		float preset1Pressed = params[BUTTON1_PARAM].getValue() > 0.f;
		if ( preset1Pressed > 0 && prevPreset1Pressed != preset1Pressed ) {
				presets[presetIndex][0]++;
				if (presets[presetIndex][0] >= 8) {
					presets[presetIndex][0] = 0;
				}
		}
		prevPreset1Pressed = preset1Pressed;

		// For input tap 2, map it from current output to next output
		float preset2Pressed = params[BUTTON2_PARAM].getValue() > 0.f;
		if ( preset2Pressed > 0 && prevPreset2Pressed != preset2Pressed ) {
				presets[presetIndex][1]++;
				if (presets[presetIndex][1] >= 8) {
					presets[presetIndex][1] = 0;
				}
		}
		prevPreset2Pressed = preset2Pressed;

		// For input tap 3, map it from current output to next output
		float preset3Pressed = params[BUTTON3_PARAM].getValue() > 0.f;
		if ( preset3Pressed > 0 && prevPreset3Pressed != preset3Pressed ) {
				presets[presetIndex][2]++;
				if (presets[presetIndex][2] >= 8) {
					presets[presetIndex][2] = 0;
				}
		}
		prevPreset3Pressed = preset3Pressed;

		// For input tap 4, map it from current output to next output
		float preset4Pressed = params[BUTTON4_PARAM].getValue() > 0.f;
		if ( preset4Pressed > 0 && prevPreset4Pressed != preset4Pressed ) {
				presets[presetIndex][3]++;
				if (presets[presetIndex][3] >= 8) {
					presets[presetIndex][3] = 0;
				}
		}
		prevPreset4Pressed = preset4Pressed;

		// For input tap 5, map it from current output to next output
		float preset5Pressed = params[BUTTON5_PARAM].getValue() > 0.f;
		if ( preset5Pressed > 0 && prevPreset5Pressed != preset5Pressed ) {
				presets[presetIndex][4]++;
				if (presets[presetIndex][4] >= 8) {
					presets[presetIndex][4] = 0;
				}
		}
		prevPreset5Pressed = preset5Pressed;

		// For input tap 6, map it from current output to next output
		float preset6Pressed = params[BUTTON6_PARAM].getValue() > 0.f;
		if ( preset6Pressed > 0 && prevPreset6Pressed != preset6Pressed ) {
				presets[presetIndex][5]++;
				if (presets[presetIndex][5] >= 8) {
					presets[presetIndex][5] = 0;
				}
		}
		prevPreset6Pressed = preset6Pressed;

		// For input tap 7, map it from current output to next output
		float preset7Pressed = params[BUTTON7_PARAM].getValue() > 0.f;
		if ( preset7Pressed > 0 && prevPreset7Pressed != preset7Pressed ) {
				presets[presetIndex][6]++;
				if (presets[presetIndex][6] >= 8) {
					presets[presetIndex][6] = 0;
				}
		}
		prevPreset7Pressed = preset7Pressed;

		bool cv1Active = presets[presetIndex][0] == 0;
		bool cv2Active = presets[presetIndex][1] == 0;
		bool cv3Active = presets[presetIndex][2] == 0;
		bool cv4Active = presets[presetIndex][3] == 0;
		bool cv5Active = presets[presetIndex][4] == 0;
		bool cv6Active = presets[presetIndex][5] == 0;
		bool cv7Active = presets[presetIndex][6] == 0;
		int cv1Index = (index + presets[presetIndex][0] - 1) % 7 + 1;
		int cv2Index = (index + presets[presetIndex][1] - 1) % 7 + 1;
		int cv3Index = (index + presets[presetIndex][2] - 1) % 7 + 1;
		int cv4Index = (index + presets[presetIndex][3] - 1) % 7 + 1;
		int cv5Index = (index + presets[presetIndex][4] - 1) % 7 + 1;
		int cv6Index = (index + presets[presetIndex][5] - 1) % 7 + 1;
		int cv7Index = (index + presets[presetIndex][6] - 1) % 7 + 1;

		outputs[CV1_OUTPUT].setVoltage(cv1Active ? 0.f : cv1Index);
		outputs[CV2_OUTPUT].setVoltage(cv2Active ? 0.f : cv2Index);
		outputs[CV3_OUTPUT].setVoltage(cv3Active ? 0.f : cv3Index);
		outputs[CV4_OUTPUT].setVoltage(cv4Active ? 0.f : cv4Index);
		outputs[CV5_OUTPUT].setVoltage(cv5Active ? 0.f : cv5Index);
		outputs[CV6_OUTPUT].setVoltage(cv6Active ? 0.f : cv6Index);
		outputs[CV7_OUTPUT].setVoltage(cv7Active ? 0.f : cv7Index);

		lights[PRESETA_LIGHT].setBrightness((presetIndex + 1) & 1 ? 1.f : 0.f);
		lights[PRESETB_LIGHT].setBrightness((presetIndex + 1) & 2 ? 1.f : 0.f);
		lights[PRESETC_LIGHT].setBrightness((presetIndex + 1) & 4 ? 1.f : 0.f);
		lights[BLINK1A_LIGHT].setBrightness(presets[presetIndex][0] & 1 ? 1.f : 0.f);
		lights[BLINK1B_LIGHT].setBrightness(presets[presetIndex][0] & 2 ? 1.f : 0.f);
		lights[BLINK1C_LIGHT].setBrightness(presets[presetIndex][0] & 4 ? 1.f : 0.f);
		lights[BLINK2A_LIGHT].setBrightness(presets[presetIndex][1] & 1 ? 1.f : 0.f);
		lights[BLINK2B_LIGHT].setBrightness(presets[presetIndex][1] & 2 ? 1.f : 0.f);
		lights[BLINK2C_LIGHT].setBrightness(presets[presetIndex][1] & 4 ? 1.f : 0.f);
		lights[BLINK3A_LIGHT].setBrightness(presets[presetIndex][2] & 1 ? 1.f : 0.f);
		lights[BLINK3B_LIGHT].setBrightness(presets[presetIndex][2] & 2 ? 1.f : 0.f);
		lights[BLINK3C_LIGHT].setBrightness(presets[presetIndex][2] & 4 ? 1.f : 0.f);
		lights[BLINK4A_LIGHT].setBrightness(presets[presetIndex][3] & 1 ? 1.f : 0.f);
		lights[BLINK4B_LIGHT].setBrightness(presets[presetIndex][3] & 2 ? 1.f : 0.f);
		lights[BLINK4C_LIGHT].setBrightness(presets[presetIndex][3] & 4 ? 1.f : 0.f);
		lights[BLINK5A_LIGHT].setBrightness(presets[presetIndex][4] & 1 ? 1.f : 0.f);
		lights[BLINK5B_LIGHT].setBrightness(presets[presetIndex][4] & 2 ? 1.f : 0.f);
		lights[BLINK5C_LIGHT].setBrightness(presets[presetIndex][4] & 4 ? 1.f : 0.f);
		lights[BLINK6A_LIGHT].setBrightness(presets[presetIndex][5] & 1 ? 1.f : 0.f);
		lights[BLINK6B_LIGHT].setBrightness(presets[presetIndex][5] & 2 ? 1.f : 0.f);
		lights[BLINK6C_LIGHT].setBrightness(presets[presetIndex][5] & 4 ? 1.f : 0.f);
		lights[BLINK7A_LIGHT].setBrightness(presets[presetIndex][6] & 1 ? 1.f : 0.f);
		lights[BLINK7B_LIGHT].setBrightness(presets[presetIndex][6] & 2 ? 1.f : 0.f);
		lights[BLINK7C_LIGHT].setBrightness(presets[presetIndex][6] & 4 ? 1.f : 0.f);
	}

	// json_t* dataToJson() override {
	// 	json_t* rootJ = json_object();
	// 	json_object_set_new(rootJ, "mode", json_integer(mode));
	// 	return rootJ;
	// }

	// void dataFromJson(json_t* rootJ) override {
	// 	json_t* modeJ = json_object_get(rootJ, "mode");
	// 	if (modeJ)
	// 		mode = json_integer_value(modeJ);
	// }
};


struct AnalogSwizzlingRegisterWidget : ModuleWidget {
	AnalogSwizzlingRegisterWidget(AnalogSwizzlingRegister* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/AnalogSwizzlingRegister.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<VCVButton>(mm2px(Vec(14.711, 20.908)), module, AnalogSwizzlingRegister::PRESET_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(14.711, 55.262)), module, AnalogSwizzlingRegister::BUTTON1_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(14.711, 64.257)), module, AnalogSwizzlingRegister::BUTTON2_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(14.711, 73.253)), module, AnalogSwizzlingRegister::BUTTON3_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(14.711, 82.249)), module, AnalogSwizzlingRegister::BUTTON4_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(14.711, 91.245)), module, AnalogSwizzlingRegister::BUTTON5_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(14.711, 100.24)), module, AnalogSwizzlingRegister::BUTTON6_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(14.711, 109.236)), module, AnalogSwizzlingRegister::BUTTON7_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.851, 37.848)), module, AnalogSwizzlingRegister::INDEX_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.996, 37.848)), module, AnalogSwizzlingRegister::NEXT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(32.115, 37.85)), module, AnalogSwizzlingRegister::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.01, 55.262)), module, AnalogSwizzlingRegister::FREQ_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.009, 73.253)), module, AnalogSwizzlingRegister::INPUT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.009, 91.245)), module, AnalogSwizzlingRegister::CLOCK_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.553, 55.262)), module, AnalogSwizzlingRegister::CV1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.553, 64.257)), module, AnalogSwizzlingRegister::CV2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.553, 73.253)), module, AnalogSwizzlingRegister::CV3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.553, 82.249)), module, AnalogSwizzlingRegister::CV4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.553, 91.245)), module, AnalogSwizzlingRegister::CV5_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.553, 100.241)), module, AnalogSwizzlingRegister::CV6_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.553, 109.237)), module, AnalogSwizzlingRegister::CV7_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.29, 20.908)), module, AnalogSwizzlingRegister::PRESETA_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(24.251, 20.908)), module, AnalogSwizzlingRegister::PRESETB_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(28.128, 20.908)), module, AnalogSwizzlingRegister::PRESETC_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.29, 55.262)), module, AnalogSwizzlingRegister::BLINK1A_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(24.25, 55.262)), module, AnalogSwizzlingRegister::BLINK1B_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(28.128, 55.262)), module, AnalogSwizzlingRegister::BLINK1C_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.29, 64.257)), module, AnalogSwizzlingRegister::BLINK2A_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(24.25, 64.257)), module, AnalogSwizzlingRegister::BLINK2B_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(28.128, 64.257)), module, AnalogSwizzlingRegister::BLINK2C_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.29, 73.253)), module, AnalogSwizzlingRegister::BLINK3A_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(24.25, 73.253)), module, AnalogSwizzlingRegister::BLINK3B_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(28.128, 73.253)), module, AnalogSwizzlingRegister::BLINK3C_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.29, 82.249)), module, AnalogSwizzlingRegister::BLINK4A_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(24.25, 82.249)), module, AnalogSwizzlingRegister::BLINK4B_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(28.128, 82.249)), module, AnalogSwizzlingRegister::BLINK4C_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.29, 91.245)), module, AnalogSwizzlingRegister::BLINK5A_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(24.25, 91.245)), module, AnalogSwizzlingRegister::BLINK5B_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(28.128, 91.245)), module, AnalogSwizzlingRegister::BLINK5C_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.29, 100.24)), module, AnalogSwizzlingRegister::BLINK6A_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(24.25, 100.24)), module, AnalogSwizzlingRegister::BLINK6B_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(28.128, 100.24)), module, AnalogSwizzlingRegister::BLINK6C_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.29, 109.236)), module, AnalogSwizzlingRegister::BLINK7A_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(24.25, 109.236)), module, AnalogSwizzlingRegister::BLINK7B_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(28.128, 109.236)), module, AnalogSwizzlingRegister::BLINK7C_LIGHT));
	}
};


Model* modelAnalogSwizzlingRegister = createModel<AnalogSwizzlingRegister, AnalogSwizzlingRegisterWidget>("AnalogSwizzlingRegister");