#ifndef SIGNAL_RECEIVE_HANDLER_H_
#define SIGNAL_RECEIVE_HANDLER_H_

#define DATA_BUF_TX_SIZE       		(2)
#define MODULATION_DATA_SIZE   		(20)
#define POWER_SUPPLY				(3300)  //3300
#define INPUT_AMPLITUDE				(3000)
#define TICKS_NUM					(5376)  //(32 + 5*8*16)*8 - количество тактов таймера на каждую ФРМ послыку
#define FFT_N						(256)

#include "main.h"
#include "fftM.h"
#include "fftM_terminate.h"
#include "rt_nonfinite.h"

void FlagsClear(void);
void SpectrumAnalysis(double*, uint8_t);
uint8_t ReadyToStartReceive(double);
uint8_t CatchPackageStart(double, double, uint64_t*);
void FasleActivationCatch(double, double, uint8_t*);
void FirstPeriodInit(double, double);
void HandlerVarReInit(void);
uint8_t GetStartSign(void);
void ReceiveStop(double*, double*, uint16_t*);
double AmlitudeAnalysis(double*, uint16_t);

#endif
