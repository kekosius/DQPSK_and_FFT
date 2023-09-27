/*!
 * @file        SignalReceiveHandler.c
 *
 * @brief       Функции, обеспечивающие приём АРСФ пакетов 
 * 				(обнаружение пакетов, регистрация и обработка ложных срабатываний)
 *
 * @version     V1.0.0
 *
 * @date        12-09-2023
 */

#include "SignalReceiveHandler.h"

//#define DEBUG

uint8_t ZeroVoltageCounter = 0;			/*!< Содержит информацию о количестве приёмов слабого (|RealVoltage| < InterferenceAmplitude) сигнала
										 *   подряд. Значение ZeroVoltageCounter определяется в [ReadyToStartReceive()](#ReadyToStartReceive).
										 *   При достижении ZeroVoltageCounter определённого значения (16) считается, что система готова к приёму
										 *   нового АРСФ пакета (#ReceiverReady = 1) */

uint8_t ReceiveStarted = 0;				/*!< Содержит информацию о том, что система в данный момент принимает новый АРСФ пакет.
										 *   Значение 1 соответсвует приёму нового пакета в настоящий момент.
										 *   Значение 0 соответсвует отсутсвию приёма пакета в настоящий момент.
										 *   Значение ReceiveStarted определяется в [CatchPackageStart()](#CatchPackageStart) */
										 
uint8_t ReceiverReady = 0;				/*!< Содержит информацию о том, что система готова к приёму нового АРСФ пакета.
										 *   Значение 1 соответсвует готовности к приёму нового пакета.
										 *   Значение 0 соответсвует занятости линии приёма (система не готова к приёму пакета).
										 *   Значение ReceiverReady определяется в [ReadyToStartReceive()](#ReadyToStartReceive) */
										 
uint16_t ExitCounter = 0;				/*!< Содержит информацию о количестве принятых "помех" (уровень принятого сигнала
										 *	 меньше #InterferenceAmplitude) подряд. Значение ExitCounter определяется в 
										 *   [FasleActivationCatch()](#FasleActivationCatch). При ExitCounter = 32 система
										 *   инициирует сброс приёма (см. #FalseActivation)*/
										 
int8_t StartSign = 0;					/*!< Содержит информацию о стартовом напрявлении входного сигнала. То есть,
										 *	 значение -1 сообтветсвует стартовому периоду, который находится ниже нуля,
										 *	 значение +1 сообтветсвует стартовому периоду, который находится выше нуля.
										 *	 Значение StartSign определяется в [CatchPackageStart()](#CatchPackageStart).
										 *   Используется для анализа принятого пакета в [ZeroCrossAnalysis()](#ZeroCrossAnalysis)*/

double InterferenceAmplitude = (INPUT_AMPLITUDE/2)*0.5; /*!< Амплитуда колебаний, которая будет считаться как паразитный сигнал (помеха) */

/*!
 * @brief     Содержит информацию о максимальном и минимальном уровне сигнала
 *
 * Используется для создания #PeriodAmplitude
 */

struct MinMaxStruct { 
	double max;
	double min;
};

/*!
 * @brief     Содержит информацию об амплитуде принимаемого сигнала
 *
 * Информация об амплитуде сигнала используется в [CatchPackageStart()](#CatchPackageStart) для определения 
 * начала пакета, а также в [FasleActivationCatch()](#FasleActivationCatch) для определения ложного срабатывания приёма
 */

struct MinMaxStruct PeriodAmplitude;

/*!
 * @brief     Определяет готовность системы к приёму нового АРСФ пакета
 *
 * @param	  RealVoltage Напряжение с поправкой на добавленную константу (см. #GetRealVoltage)
 *
 * Если на линии отсутсвует полезный сигнал (|RealVoltage| < InterferenceAmplitude) на протяжении достаточно
 * продолжительного времени (16 измерений), то считается, что система готова к приёму АРСФ пакета (#ReceiverReady = 1).
 * Такой метод необходим для того, чтобы не начать приём и обработку пакета с его середины или конца.
 * Если в текущий момент времени производится приём нового АРСФ пакета (#ReceiveStarted == 1), то проверка готовности
 * системы к приёму нового пакета не производится.
 */
 
uint8_t ReadyToStartReceive(double RealVoltage) {
	if (ReceiveStarted || ReceiverReady) return 1;
	if (fabs(RealVoltage) > InterferenceAmplitude) {
		ZeroVoltageCounter = 0;
		return 0;
	}
	if (ZeroVoltageCounter > 8*2) {
		ReceiverReady = 1;
		ZeroVoltageCounter = 0;
		PeriodAmplitude.max = 0;
		PeriodAmplitude.min = 0;
		return 1;
	}
	ZeroVoltageCounter++;
	return 0;
}

/*!
 * @brief     Определяет начало нового АРСФ пакета
 *
 * @param[in]	  RealVoltage Напряжение с поправкой на добавленную константу (см. #GetRealVoltage)
 * @param[in]	  PrevVoltage Предыдущее значение принятого напряжение
 * @param[out]	  tick Ссылка на значение текущего тика таймера
 *
 * Если на линии присутсвует сигнал, превышающий InterferenceAmplitude, то этот момент считается
 * за начало пакета (заведомо известно, что на линии достаточно продолжительное время отсутсвует
 * сигнал помех или полезный сигнал, т.е. #ReceiverReady == 1). Функция сигнализирует об определении
 * начала приёма новго пакета путём присваивания #ReceiveStarted = 1. Функция также обнуляет tick
 * и определяет #StartSign
 */

uint8_t CatchPackageStart(double RealVoltage, double PrevVoltage, uint64_t* tick) {
	if (ReceiveStarted) return 1;
	if (RealVoltage > PrevVoltage) {
		PeriodAmplitude.max = RealVoltage;
	} else PeriodAmplitude.min = RealVoltage;
	
	if ((PeriodAmplitude.max - PeriodAmplitude.min) > InterferenceAmplitude) {
		ReceiveStarted = 1;
		*tick = 0; //8
		PrevVoltage = 0;
		if (RealVoltage > 0) StartSign = 1;
		else StartSign = -1;
		APM_MINI_LEDOn(LED2);
		return 1;
	}
	return 0;
}

/*!
 * @brief     Определяет ложный приём пакета
 *
 * @param[in]	  RealVoltage Напряжение с поправкой на добавленную константу (см. #GetRealVoltage)
 * @param[in]	  PrevVoltage Предыдущее значение принятого напряжение
 * @param[out]	  FalseActivation Ссылка на переменную #FalseActivation
 *
 * Если на линии продолжительное время (32 измерения) отсутсвует сигнал, превышающий 
 * InterferenceAmplitude, то текущий приём считается ложным (система срагировала на случайный
 * всплеск на линии или на паразитный сигнал). Функция сигнализирует о ложном срабатывании путём
 * присваивания #FalseActivation = 1
 */

void FasleActivationCatch(double RealVoltage, double PrevVoltage, uint8_t* FalseActivation) {
	if (RealVoltage > PrevVoltage) PeriodAmplitude.max = RealVoltage;
	else PeriodAmplitude.min = RealVoltage;

	if (PeriodAmplitude.max - PeriodAmplitude.min < InterferenceAmplitude) ExitCounter++;
	else ExitCounter = 0;

	if (ExitCounter > 8*4) *FalseActivation = 1;
}

/*!
 * @brief     Обнуляет перемнные, определяющие приём АРСФ пакетов
 *
 * Используется при окончании приёма (#ReceiveStop) и обнаружении ложного приёма (#FasleActivationCatch)
 * 
 * Обнуляемые переменные:
 * - #ReceiverReady
 * - #ReceiveStarted
 * - #ExitCounter
 * - #ZeroVoltageCounter
 * - #PeriodAmplitude
 */

void HandlerVarReInit() {
	ReceiverReady = 0;
	ReceiveStarted = 0;
	ExitCounter = 0;
	ZeroVoltageCounter = 0;
	PeriodAmplitude.max = 0;
	PeriodAmplitude.min = 0;
}

/*!
 * @brief     Окончание приёма пакета. Демодуляция и обнуление переменных
 *
 * @param[in]	  VoltagesData 			Массив, содержащий значения напряжений для всего принятого пакета
 * @param[out]	  ZeroCrossTimings		Массив, в который записываются значения времени, когда несущий
 *										сигнал пересекает 0 V снизу
 * @param[out]	  EndOfSample 			Массив, в который записываются временные промежутки для всех фазовых переходов
 *
 * Функция вызывается при определении конца текущего пакета. Описание последовательности работы:
 * - Переключение светодиодов (LED2 - off, LED3 - on), выставляется High напряжение
 *   на ЦАП (для визуального и сигнального информирования о состоянии системы)
 * - Вызывается [ZeroCrossAnalysis()](#ZeroCrossAnalysis)
 * - Заполненные #ZeroCrossTimings и #EndOfSample используются в [Demodulation()](#Demodulation)
 * - Обнуление (возврат к значениям при инициализации) всех переменных и 
 *   переинициализация используемой перефирии
 * - Отключение LED3, выставляется Low напряжение на ЦАП
 */

void ReceiveStop(double* VoltagesData, double* ZeroCrossTimings, uint16_t* EndOfSample){
	
	APM_MINI_LEDOff(LED2);
	APM_MINI_LEDOn(LED3);
	DAC_SetHigh();
	
	ZeroCrossAnalysis(VoltagesData, ZeroCrossTimings, StartSign, EndOfSample);
	
	uint8_t HighBinData[40] = {0};
	uint8_t LowBinData[40] = {0};
	
	if(Demodulation(ZeroCrossTimings, EndOfSample, HighBinData, LowBinData)){
		#ifdef DEBUG
			USART_Tx_Char(USART, 13);
			for (int i = 0; i < 43; i++) USART_Tx_Number(USART, EndOfSample[i]);
			USART_Tx_Char(USART, 13);
			for (int i = 1; i < 43; i++) {
				for (int j = EndOfSample[i-1]+1; j<=EndOfSample[i]; j++) {
					USART_Tx_Float(USART, ZeroCrossTimings[j], 5);
				}
				USART_Tx_Char(USART, 13);
			}
			for (int i = 0; i < 40; i++){
				USART_Tx_Number(USART, HighBinData[i]);
				if ((i+1)%4 == 0) USART_Tx_Char(USART, ' ');
			}
			USART_Tx_Char(USART, 13);
			for (int i = 0; i < 40; i++){
				USART_Tx_Number(USART, LowBinData[i]);
				if ((i+1)%4 == 0) USART_Tx_Char(USART, ' ');
			}
			USART_Tx_Char(USART, 13);
			while(1);
		#endif
	}
	
	VarReInit();
	FilterWipe();
	TMR_Reset(TMR2);
	ADC_Init();
	TMR2_Init();
	DAC_SetLow();
	APM_MINI_LEDOff(LED3);
}
