#include "stats.h"
#include "i2c.h"



#define NVGPU_GPU_IOCTL_PMU_GET_GPU_LOAD 0x80044715

void BatteryChecker(void*arg);

Result psmGetBatteryChargeInfoFields(Service* psmService, BatteryChargeInfoFields *out) {
    return serviceDispatchOut(psmService, 17, *out);
}

Result getNvChannelClockRate(NvChannel *channel, u32 module_id, u32 *clock_rate) {
	struct nvhost_clk_rate_args {
	    uint32_t rate;
	    uint32_t moduleid;
	} args = {
		.rate     = 0,
		.moduleid = module_id,
	};

	u32 id = hosversionBefore(8,0,0) ? _NV_IOWR(0, 0x14, args) : _NV_IOWR(0, 0x23, args);
	Result rc = nvIoctl(channel->fd, id, &args);
	if (R_SUCCEEDED(rc) && clock_rate)
		*clock_rate = args.rate;

	return rc;
}


CStats::CStats(){
	
}

CStats::~CStats(){
	clkrstExit();
	pcvExit();

 	i2cExit();
	psmExit();
	nvClose(fd);
	nvExit();
}

void CheckCore0(void *arg) {
	CStats * ctx = (CStats *)arg;
	while (!ctx->threadexit) {
		static uint64_t idletick_a0 = 0;
		static uint64_t idletick_b0 = 0;
		svcGetInfo(&idletick_b0, InfoType_IdleTickCount, INVALID_HANDLE, 0);
		svcSleepThread(1'000'000'000 / ctx->refreshrate);
		svcGetInfo(&idletick_a0, InfoType_IdleTickCount, INVALID_HANDLE, 0);
		ctx->idletick0 = idletick_a0 - idletick_b0;
	}
}

void CheckCore1(void *arg) {
	CStats * ctx = (CStats *)arg;
	while (!ctx->threadexit) {
		static uint64_t idletick_a1 = 0;
		static uint64_t idletick_b1 = 0;
		svcGetInfo(&idletick_b1, InfoType_IdleTickCount, INVALID_HANDLE, 1);
		svcSleepThread(1'000'000'000 / ctx->refreshrate);
		svcGetInfo(&idletick_a1, InfoType_IdleTickCount, INVALID_HANDLE, 1);
		ctx->idletick1 = idletick_a1 - idletick_b1;
	}
}

void CheckCore2(void *arg) {
	CStats * ctx = (CStats *)arg;
	while (!ctx->threadexit) {
		static uint64_t idletick_a2 = 0;
		static uint64_t idletick_b2 = 0;
		svcGetInfo(&idletick_b2, InfoType_IdleTickCount, INVALID_HANDLE, 2);
		svcSleepThread(1'000'000'000 / ctx->refreshrate);
		svcGetInfo(&idletick_a2, InfoType_IdleTickCount, INVALID_HANDLE, 2);
		ctx->idletick2 = idletick_a2 - idletick_b2;
	}
}

void Misc(void*arg) {
	CStats * ctx = (CStats *)arg;
	//Service* psmService = 0;
	//Result nvCheck = 1;
	
	//ctx->clkrstCheck = clkrstInitialize();
	
	
	while (!ctx->threadexit) {
		
		// CPU, GPU and RAM Frequency
		//if (R_SUCCEEDED(ctx->clkrstCheck)) {
			ClkrstSession session = {0};
			clkrstOpenSession(&session, PcvModuleId_CpuBus, 3);
			clkrstGetClockRate(&session, &ctx->CPU_Hz);
			clkrstCloseSession(&session);
			
			clkrstOpenSession(&session, PcvModuleId_GPU, 3);
			clkrstGetClockRate(&session, &ctx->GPU_Hz);
			clkrstCloseSession(&session);
			
			clkrstOpenSession(&session, PcvModuleId_EMC, 3);
			clkrstGetClockRate(&session, &ctx->RAM_Hz);
			clkrstCloseSession(&session);
			
			
		
		//GPU Load
		if(R_SUCCEEDED(ctx->nvCheck)){
			nvIoctl(ctx->fd, NVGPU_GPU_IOCTL_PMU_GET_GPU_LOAD, &ctx->GPU_Load_u);
		}
		if(R_SUCCEEDED(ctx->nvdecCheck)){
			getNvChannelClockRate(&ctx->nvdecChannel, 0x75, &ctx->NVDEC_Hz_u);
		}
		
		// Interval
		ctx->UpdateStats();
		svcSleepThread(1'000'000'000);
	}
}


void CStats::StartThreads() {

	if(!emuoverrides){

		psmCheck = psmInitialize();
			if (R_SUCCEEDED(psmCheck)) {
				psmService = psmGetServiceSession();
				i2cInitialize();
				NXLOG::DEBUGLOG("I2C INIT\n");
		}

		if (R_SUCCEEDED(nvInitialize())){
			nvCheck = nvOpen(&fd, "/dev/nvhost-ctrl-gpu");
		}else{
			NXLOG::ERRORLOG("Cannot Init ctrl-gpu");
		}
		
		if (R_SUCCEEDED(nvMapInit())){
			nvdecCheck = nvChannelCreate(&nvdecChannel, "/dev/nvhost-nvdec");
		}else{
			NXLOG::ERRORLOG("Cannot Init nvhost-nvdec");
		}

	}
	
	threadCreate(&t0, CheckCore0, this, NULL, 0x1000, 0x3B, 0);
	threadCreate(&t1, CheckCore1, this, NULL, 0x1000, 0x3B, 1);
	threadCreate(&t2, CheckCore2, this, NULL, 0x1000, 0x3B, 2);
	
	threadCreate(&t4, Misc, this, NULL, 0x1000, 0x3B, -2);
	threadCreate(&t7, BatteryChecker, this, NULL, 0x1000, 0x3B, -2);
	
	
	
	
	threadStart(&t0);
	threadStart(&t1);
	threadStart(&t2);
	
	threadStart(&t4);
	
	threadStart(&t7);
	
	
	
}

void CStats::CloseThreads() {
	threadexit = true;
	threadWaitForExit(&t0);
	threadWaitForExit(&t1);
	threadWaitForExit(&t2);
	
	threadWaitForExit(&t4);
	threadWaitForExit(&t7);
	
	
	
	threadClose(&t0);
	threadClose(&t1);
	threadClose(&t2);
	
	threadClose(&t4);
	
	threadClose(&t7);
	
	
}

void CStats::UpdateStats(){
	
	if (idletick0 > systemtickfrequency) idletick0 = systemtickfrequency;
	if (idletick1 > systemtickfrequency) idletick1 = systemtickfrequency;
	if (idletick2 > systemtickfrequency) idletick2 = systemtickfrequency;
	if (idletick3 > systemtickfrequency) idletick3 = systemtickfrequency;
	
	snprintf(CPU_Hz_c, sizeof CPU_Hz_c, "Frequency: %.1f MHz", (float)CPU_Hz / 1000000);
	snprintf(CPU_Usage0, sizeof CPU_Usage0, "Core #0: %.2f%s", ((double)systemtickfrequency - (double)idletick0) / (double)systemtickfrequency * 100, "%");
	snprintf(CPU_Usage1, sizeof CPU_Usage1, "Core #1: %.2f%s", ((double)systemtickfrequency - (double)idletick1) / (double)systemtickfrequency * 100, "%");
	snprintf(CPU_Usage2, sizeof CPU_Usage2, "Core #2: %.2f%s", ((double)systemtickfrequency - (double)idletick2) / (double)systemtickfrequency * 100, "%");
	snprintf(GPU_Hz_c, sizeof GPU_Hz_c, "Frequency: %.1f MHz", (float)GPU_Hz / 1000000);
	snprintf(GPU_Load_c, sizeof GPU_Load_c, "Load: %.1f%s", (float)GPU_Load_u / 10, "%");
	snprintf(NVDEC_Hz_c, sizeof NVDEC_Hz_c, "Frequency: %.1f MHz", (float)NVDEC_Hz_u / 1000000);

	snprintf(RAM_Hz_c, sizeof RAM_Hz_c, "Frequency: %.1f MHz", (float)RAM_Hz / 1000000);
	
	batpercentage = _batteryChargeInfoFields.RawBatteryCharge / 1000;
	
	if (_batteryChargeInfoFields.ChargerType)
			snprintf(Battery_c, sizeof Battery_c,
				"Battery Temperature: %.1f\u00B0C\n"
				"Battery Raw Charge: %.1f%s\n"
				"Battery Age: %.1f%s\n"
				"Battery Voltage (5s AVG): %.0f mV\n"
				"Battery Current Flow (5s AVG): %+.0f mA\n"
				"Battery Power Flow (5s AVG): %+.3f W\n"
				"Charger Type: %u\n"
				"Charger Max Voltage: %u mV\n"
				"Charger Max Current: %u mA",
				(float)_batteryChargeInfoFields.BatteryTemperature / 1000,
				(float)_batteryChargeInfoFields.RawBatteryCharge / 1000, "%",
				(float)_batteryChargeInfoFields.BatteryAge / 1000, "%",
				batVoltageAvg,
				batCurrentAvg,
				PowerConsumption,
				_batteryChargeInfoFields.ChargerType,
				_batteryChargeInfoFields.ChargerVoltageLimit,
				_batteryChargeInfoFields.ChargerCurrentLimit
			);
		else
			snprintf(Battery_c, sizeof Battery_c,
				"Battery Temperature: %.1f\u00B0C\n"
				"Battery Raw Charge: %.1f%s\n"
				"Battery Age: %.1f%s\n"
				"Battery Voltage (5s AVG): %.0f mV\n"
				"Battery Current Flow (5s AVG): %.0f mA\n"
				"Battery Power Flow (5s AVG): %+.3f W",
				(float)_batteryChargeInfoFields.BatteryTemperature / 1000,
				(float)_batteryChargeInfoFields.RawBatteryCharge / 1000, "%",
				(float)_batteryChargeInfoFields.BatteryAge / 1000, "%",
				batVoltageAvg,
				batCurrentAvg,
				PowerConsumption
			);
			
		time_t unixTime = time(NULL);
        struct tm* timeStruct = localtime((const time_t *)&unixTime);//Gets UTC time. If you want local-time use localtime().

        int hours = timeStruct->tm_hour;
        int minutes = timeStruct->tm_min;
        int seconds = timeStruct->tm_sec;
        int day = timeStruct->tm_mday;
        int month = timeStruct->tm_mon;
        int year = timeStruct->tm_year +1900;
        int wday = timeStruct->tm_wday;
		sprintf(currentTime,"%02i:%02i", hours, minutes);
}

void BatteryChecker(void*arg) {
	CStats * ctx = (CStats *)arg;
	
	if (R_SUCCEEDED(ctx->psmCheck)){
		u16 data = 0;
		float tempV = 0;
		float tempA = 0;
		size_t ArraySize = 10;
		float readingsAmp[ArraySize] = {0};
		float readingsVolt[ArraySize] = {0};

		if (Max17050ReadReg(MAX17050_Current, &data)) {
			tempA = (1.5625 / (max17050SenseResistor * max17050CGain)) * (s16)data;
			for (size_t i = 0; i < ArraySize; i++) {
				readingsAmp[i] = tempA;
			}
		}
		if (Max17050ReadReg(MAX17050_VCELL, &data)) {
			tempV = 0.625 * (data >> 3);
			for (size_t i = 0; i < ArraySize; i++) {
				readingsVolt[i] = tempV;
			}
		}
		
		size_t i = 0;
		while (!ctx->threadexit) {
			psmGetBatteryChargeInfoFields(ctx->psmService, &ctx->_batteryChargeInfoFields);
			// Calculation is based on Hekate's max17050.c
			// Source: https://github.com/CTCaer/hekate/blob/master/bdk/power/max17050.c
			if (!Max17050ReadReg(MAX17050_Current, &data))
				continue;
			tempA = (1.5625 / (max17050SenseResistor * max17050CGain)) * (s16)data;
			if (!Max17050ReadReg(MAX17050_VCELL, &data))
				continue;
			tempV = 0.625 * (data >> 3);

			readingsAmp[i] = tempA;
			readingsVolt[i] = tempV;
			if (i+1 < ArraySize) {
				i += 1;
			}
			else i = 0;
			
			float batCurrent = readingsAmp[0];
			float batVoltage = readingsVolt[0];
			float batPowerAvg = (readingsAmp[0] * readingsVolt[0]) / 1'000;
			for (size_t x = 1; x < ArraySize; x++) {
				batCurrent += readingsAmp[x];
				batVoltage += readingsVolt[x];
				batPowerAvg += (readingsAmp[x] * readingsVolt[x]) / 1'000;
			}
			batCurrent /= ArraySize;
			batVoltage /= ArraySize;
			ctx->batCurrentAvg = batCurrent;
			ctx->batVoltageAvg = batVoltage;
			batPowerAvg /= ArraySize * 1000;
			ctx->PowerConsumption = batPowerAvg;
			
			svcSleepThread(500'000'000);
		}
		ctx->_batteryChargeInfoFields = {0};
	}
}
