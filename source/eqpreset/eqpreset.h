#ifndef NXMP_EQPRESET_H
#define NXMP_EQPRESET_H

#include <string>
#include <vector>
#include "SimpleIni.h"

class EQPreset{

struct preset_struct{
	std::string name;
	float eqvals[18];
};

public:
	EQPreset(std::string _filename);
	
	std::vector<preset_struct> getPresets();
	float * getPresetNum(int presetidx);
	
private:
	CSimpleIniA *ini;
	std::string presetsfilePath;
	std::vector<preset_struct> presets;
	
};



#endif