#ifndef NXMP_STATS_H
#define NXMP_STATS_H

#include <switch.h>
#include <string>
#include <chrono>

#include "max17050.h"
#include "logger.h"


typedef enum {
    NoHub  = BIT(0),  //If hub is disconnected
    Rail   = BIT(8),  //At least one Joy-con is charging from rail
    SPDSRC = BIT(12), //OTG
    ACC    = BIT(16)  //Accessory
} BatteryChargeInfoFieldsFlags;

typedef enum {
    NewPDO               = 1, //Received new Power Data Object
    NoPD                 = 2, //No Power Delivery source is detected
    AcceptedRDO          = 3  //Received and accepted Request Data Object
} BatteryChargeInfoFieldsPDControllerState; //BM92T series

typedef enum {
    None         = 0,
    PD           = 1,
    TypeC_1500mA = 2,
    TypeC_3000mA = 3,
    DCP          = 4,
    CDP          = 5,
    SDP          = 6,
    Apple_500mA  = 7,
    Apple_1000mA = 8,
    Apple_2000mA = 9
} BatteryChargeInfoFieldsChargerType;

typedef enum {
    Sink         = 1,
    Source       = 2
} BatteryChargeInfoFieldsPowerRole;

typedef struct {
    int32_t InputCurrentLimit;                                  //Input (Sink) current limit in mA
    int32_t VBUSCurrentLimit;                                   //Output (Source/VBUS/OTG) current limit in mA
    int32_t ChargeCurrentLimit;                                 //Battery charging current limit in mA (512mA when Docked, 768mA when BatteryTemperature < 17.0 C)
    int32_t ChargeVoltageLimit;                                 //Battery charging voltage limit in mV (3952mV when BatteryTemperature >= 51.0 C)
    int32_t unk_x10;                                            //Possibly an emum, getting the same value as PowerRole in all tested cases
    int32_t unk_x14;                                            //Possibly flags
    BatteryChargeInfoFieldsPDControllerState PDControllerState; //Power Delivery Controller State
    int32_t BatteryTemperature;                                 //Battery temperature in milli C
    int32_t RawBatteryCharge;                                   //Raw battery charged capacity per cent-mille (i.e. 100% = 100000 pcm)
    int32_t VoltageAvg;                                         //Voltage avg in mV (more in Notes)
    int32_t BatteryAge;                                         //Battery age (capacity full / capacity design) per cent-mille (i.e. 100% = 100000 pcm)
    BatteryChargeInfoFieldsPowerRole PowerRole;
    BatteryChargeInfoFieldsChargerType ChargerType;
    int32_t ChargerVoltageLimit;                                //Charger and external device voltage limit in mV
    int32_t ChargerCurrentLimit;                                //Charger and external device current limit in mA
    BatteryChargeInfoFieldsFlags Flags;                         //Unknown flags
} BatteryChargeInfoFields;

//CODE FROM https://github.com/masagrator/Status-Monitor-Overlay


typedef struct{
	std::string hwdec;
	std::string codec;
	std::string pixelformat;
	std::string hwpixelformat;
	std::string videoformat;
	std::string colormatrix;
	
	double fps;
	double bitrate;
	int width;
	int height;
	int framedropcount;
	
	
}videodecstats_struct;

typedef struct{
	std::string codec;
	int64_t samplerate;
	int64_t channels;
	std::string hrchannels;
	double bitrate;
	
}audiodecstats_struct;


typedef struct{
	videodecstats_struct videodecstats;
	audiodecstats_struct audiodecstats;
	
}decodingstats_struct;



class CStats{
public:
	CStats();
	~CStats();
	void StartThreads();
	void CloseThreads();
	void UpdateStats();
	void CalcDelay();


	bool threadexit=false;
	
	
	Thread t0;
	Thread t1;
	Thread t2;
	Thread t3;
	
	Thread t4;
	Thread t7;
	
	Thread updatethread;
	
	
	//NVDEC
	uint32_t NVDEC_Hz = 0;
	char NVDEC_Hz_c[32];
	
	uint64_t systemtickfrequency = 19200000;
	
	uint64_t idletick0 = systemtickfrequency;
	uint64_t idletick1 = systemtickfrequency;
	uint64_t idletick2 = systemtickfrequency;
	uint64_t idletick3 = systemtickfrequency;
	char CPU_Usage0[32];
	char CPU_Usage1[32];
	char CPU_Usage2[32];
	char CPU_Usage3[32];
	char CPU_compressed_c[160];

	//Frequency
	///CPU
	uint32_t CPU_Hz = 0;
	char CPU_Hz_c[32];
	///GPU
	uint32_t GPU_Hz = 0;
	char GPU_Hz_c[32];
	///RAM
	uint32_t RAM_Hz = 0;
	char RAM_Hz_c[32];
	
	//GPU Usage
	NvChannel nvdecChannel;
	uint32_t fd = 0;
	uint32_t GPU_Load_u = 0;
	char GPU_Load_c[32];
	uint32_t NVDEC_Hz_u = 0;
	//uint32_t NVDEC_Load_u = 0;
	

	//RAM Size
	char RAM_all_c[64];
	char RAM_application_c[64];
	char RAM_applet_c[64];
	char RAM_system_c[64];
	char RAM_systemunsafe_c[64];
	char RAM_compressed_c[320];
	char RAM_var_compressed_c[320];
	uint64_t RAM_Total_all_u = 0;
	uint64_t RAM_Total_application_u = 0;
	uint64_t RAM_Total_applet_u = 0;
	uint64_t RAM_Total_system_u = 0;
	uint64_t RAM_Total_systemunsafe_u = 0;
	uint64_t RAM_Used_all_u = 0;
	uint64_t RAM_Used_application_u = 0;
	uint64_t RAM_Used_applet_u = 0;
	uint64_t RAM_Used_system_u = 0;
	uint64_t RAM_Used_systemunsafe_u = 0;


	uint32_t batpercentage = 0;
	
	Service* psmService = 0;
	
	BatteryChargeInfoFields _batteryChargeInfoFields = {0};
	char Battery_c[320];
	char BatteryDraw_c[64];
	float batCurrentAvg = 0;
	float batVoltageAvg = 0;
	float PowerConsumption = 0;
	

	
	Result clkrstCheck = 1;
	Result nvCheck = 1;
	Result pcvCheck = 1;
	Result tsCheck = 1;
	Result fanCheck = 1;
	Result tcCheck = 1;
	Result Hinted = 1;
	Result pmdmntCheck = 1;
	Result psmCheck = 1;
	Result audsnoopCheck = 1;
	Result nvdecCheck = 1;
	Result nifmCheck = 1;

	decodingstats_struct decodingstats;
	
	uint64_t refreshrate = 1;
	
	char currentTime[32];
	
	
	
	char loopstat_c[320];
	std::chrono::time_point<std::chrono::system_clock> starttime;
	std::chrono::time_point<std::chrono::system_clock> eventtime;
	std::chrono::time_point<std::chrono::system_clock> layertime;
	std::chrono::time_point<std::chrono::system_clock> rendertime;
	std::chrono::time_point<std::chrono::system_clock> endtime;
	
	int event_time_delay;
	int layer_time_delay;
	int render_time_delay;
	
	
};


#endif