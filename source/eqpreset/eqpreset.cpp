#include "eqpreset.h"

EQPreset::EQPreset(std::string _filename){
	presetsfilePath = _filename;
	ini = new CSimpleIniA(true,true);
	ini->SetUnicode();
	ini->LoadFile(_filename.c_str());
	
	
	CSimpleIniA::TNamesDepend sections;
	ini->GetAllSections(sections);
	
	CSimpleIniA::TNamesDepend::const_iterator it;
	int i;
	preset_struct tmp_presetdefault;
	tmp_presetdefault.name = "Flat (Default)";
	for(int y=0;y<18;y++){
		tmp_presetdefault.eqvals[y] = 1.0;
	}
	presets.push_back(tmp_presetdefault);
	for (i = 0, it = sections.begin(); it != sections.end(); ++i, ++it) {
		preset_struct tmp_preset;
		tmp_preset.name = it->pItem;
		for(int y=0;y<18;y++){
			char tmpkey[12];
			sprintf(tmpkey,"f%d",y);
			std::string tmpf = ini->GetValue(it->pItem,tmpkey);
			tmp_preset.eqvals[y] = std::stof(tmpf);
			
		}
		presets.push_back(tmp_preset);
	}
}

std::vector<EQPreset::preset_struct> EQPreset::getPresets(){
	return presets;
}

float * EQPreset::getPresetNum(int presetidx){
	return presets[presetidx].eqvals; 
}