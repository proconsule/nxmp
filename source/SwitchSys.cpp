//
// Created by cpasjuste on 26/11/18.
//


#include "SwitchSys.h"
#ifdef NXMP_SWITCH


using namespace c2d;

int SwitchSys::stock_cpu_clock = 0;
int SwitchSys::stock_gpu_clock = 0;
int SwitchSys::stock_emc_clock = 0;

int SwitchSys::getClock(const SwitchSys::Module &module, bool stockClocks) {

    u32 hz = 0;


    int bus_id = 0;
    bool is_old = hosversionBefore(8, 0, 0);

    if (stockClocks) {
        switch (module) {
            case SwitchSys::Module::Cpu:
                return stock_cpu_clock;
            case SwitchSys::Module::Gpu:
                return stock_gpu_clock;
            case SwitchSys::Module::Emc:
                return stock_emc_clock;
            default:
                return 0;
        }
    }

    switch (module) {
        case SwitchSys::Module::Cpu:
            bus_id = is_old ? (int) module : (int) PcvModuleId_CpuBus;
            break;
        case SwitchSys::Module::Gpu:
            bus_id = is_old ? (int) module : (int) PcvModuleId_GPU;
            break;
        case SwitchSys::Module::Emc:
            bus_id = is_old ? (int) module : (int) PcvModuleId_EMC;
            break;
        default:
            break;
    }

    if (hosversionBefore(8, 0, 0)) {
        pcvGetClockRate((PcvModule) bus_id, &hz);
    } else {
        ClkrstSession session = {0};
        clkrstOpenSession(&session, (PcvModuleId) bus_id, 3);
        clkrstGetClockRate(&session, &hz);
        clkrstCloseSession(&session);
    }


    return (int) hz;
}




bool SwitchSys::setClock(const SwitchSys::Module &module, int hz) {


    int new_hz = hz;
    int bus_id = (int) module;
    bool is_old = hosversionBefore(8, 0, 0);

    switch (module) {
        case SwitchSys::Module::Cpu:
            new_hz = new_hz <= 0 ? stock_cpu_clock : new_hz;
            bus_id = is_old ? (int) module : (int) PcvModuleId_CpuBus;
            break;
        case SwitchSys::Module::Gpu:
            new_hz = new_hz <= 0 ? stock_gpu_clock : new_hz;
            bus_id = is_old ? (int) module : (int) PcvModuleId_GPU;
            break;
        case SwitchSys::Module::Emc:
            new_hz = new_hz <= 0 ? stock_emc_clock : new_hz;
            bus_id = is_old ? (int) module : (int) PcvModuleId_EMC;
            break;
        default:
            break;
    }

    if (is_old) {
        if (R_SUCCEEDED(pcvSetClockRate((PcvModule) bus_id, (u32) new_hz))) {
            return true;
        }
    } else {
        ClkrstSession session = {0};
        clkrstOpenSession(&session, (PcvModuleId) bus_id, 3);
        clkrstSetClockRate(&session, hz);
        clkrstCloseSession(&session);
    }


    return false;
}

void SwitchSys::maxClock()
{ int clock_old = SwitchSys::getClock(SwitchSys::Module::Cpu);
        SwitchSys::setClock(SwitchSys::Module::Cpu, (int) SwitchSys::CPUClock::Max);
        SwitchSys::setClock(SwitchSys::Module::Gpu, (int) SwitchSys::GPUClock::Max);
        SwitchSys::setClock(SwitchSys::Module::Emc, (int) SwitchSys::EMCClock::Max);
        printf("setting max cpu speed (old: %i, new: %i)\n",
        clock_old, SwitchSys::getClock(SwitchSys::Module::Cpu));}

void SwitchSys::defaultClock(int stock_cpu_clock_temp,int stock_gpu_clock_temp,int stock_emc_clock_temp)
{
        int clock_old = SwitchSys::getClock(SwitchSys::Module::Cpu);
        SwitchSys::setClock(SwitchSys::Module::Cpu, (int) stock_cpu_clock_temp);
        SwitchSys::setClock(SwitchSys::Module::Gpu, (int) stock_gpu_clock_temp);
        SwitchSys::setClock(SwitchSys::Module::Emc, (int) stock_emc_clock_temp);
        printf("restoring cpu speed (old: %i, new: %i)\n",
        clock_old, SwitchSys::getClock(SwitchSys::Module::Cpu));
}

#endif