/*!
 * @file        TIM_lib.c
 *
 * @brief       Функции для работы с прерываниями таймеров
 *
 * @version     V1.0.0
 *
 * @date        02-10-2023
 */

#include "TIM_lib.h"

//#define DEBUG							//Раскомментировать для отправки по USART информации о принятом пакете в виде значений напряжения

//#define FILTER_TEST					//Раскомментировать для отправки по USART информации о работе частотного фильтра
										//(отправляется сигнал, пропущенный через частотный фильтр в виде значений напряжения)

uint8_t FalseActivation = 0;			/*!< Содержит информацию о том, что текущий приём пакета был определён
										 *   как ложный (см. #FasleActivationCatch) */

uint8_t LowLevel = 1;					/*!< Содержит информацию о том, что текущий приём АРС сигнала некачественный
										 *   (сигнал низкого уровня). Значение LowLevel = 1 соответсвует сигналу низкого уровня.
										 *   Значение LowLevel = 0 соответсвует сигналу высокого уровня. Значение LowLevel определяется
										 *   в #TMR3_EventCallback и используется при определении частоты принятого 
										 *   АРС сигнала (см. #SpectrumAnalysis)*/

uint16_t fft_pos = 0;					/*!< Счётчик позиции текущего элемента для записи в #FFT_Buff. Изменяется и используется
										 *   в #TMR3_EventCallback */

uint16_t EndOfSample[8*5+2+1] = {0};	/*!< Содержит информацию о временных промежутках (в тиках таймера), которые соотвутсвуют началу
										 *   и концу найденных фазовых переходов (конец предыдущего - начало следующего). 
										 *   Значения элементов EndOfSample определяются в [ZeroCrossAnalysis()](#ZeroCrossAnalysis) и
										 *   используются в [Demodulation()](#Demodulation)*/

uint64_t tick = 0;						/*!< Значение тика таймера (счётчик срабатываний прерывания) с начала приёма
										 *   текщуего АРСФ пакета*/

uint16_t voltageBuffCounter = 0;		/*!< Счётчик позиции текущего элемента для записи в #voltageBuff. Используется для определения
										 *   уровня напряжения для текущего приёма АРС сигнала. Изменяется и используется в #TMR3_EventCallback*/

#ifdef DEBUG
double VoltageTestArray[6000] = {0};
uint32_t c = 0;
#endif
#ifdef FILTER_TEST
int16_t FilterOutputArray[20000] = {0};
uint32_t q = 0;
#endif

double ZeroCrossTimings[700] = {0};			/*!< Содержит время переходов сигнала через 0V (снизу). Подробнее в #ZeroCrossAnalysis*/

double VoltagesData[(32+5*8*16)*8] = {0}; 	/*!< Содержит данные по принятом пакете в виде значений напряжения, полученное при каждом
											 *   прерывании таймера #TMR2_EventCallback. Используется для демодуляции полученного 
											 *   АРСФ пакета. Также используется в #AmlitudeAnalysis для определения амплитуды полученного пакета*/

double RealVoltage = 0;						/*!< Текущее значение полученного на АЦП "действительного" напряжения в милливольтах.
										     *   Подробнее в #GetRealVoltage*/

double PrevVoltage = 0;						/*!< Предыдущее значение напряжения (полученное при предшествующем прерывании таймера #TMR2_EventCallback)*/

double FFT_Buff[FFT_N] = {0};				/*!< Массив, хранящий выборку данных для частотного анализа АРС пакета. Заполняется в 
											 *   прерывании таймера #TMR3_EventCallback. Подробнее в #SpectrumAnalysis*/

double voltageBuff[VOL_BUF_LEN] = {0};		/*!< Массив, хранящий данные о принятом АРС сигнале в виде значений напряжения.
											 *   Заполняется в прерывании таймера #TMR3_EventCallback. Используется для определения
											 *   амплитуды принятого АРС сигнала в #AmlitudeAnalysis*/

/*!
 * @brief     Инициализация таймера TIM2
 *
 * Прерывание таймера TIM2 используется для приёма АРСФ пакетов. 
 * Таймер инициализируется со следующими параметрами: 
 * - Частота 26785,71 Гц
 * - Период	 3,7(3)е-5 с
 *
 * Частота таймера выбрана таким образом, чтобы на каждый период принимаемой 
 * АРСФ несущей приходилось ровно 8 прерываний таймера Так, частота АРСФ несущей
 * равна 3348,21 тогда количество прерываний на период: 26785,71/3348,21 = 8,00001.
 * Такая частота таймера была выбрана как оптимальное значение из рассчета на
 * количество измерений на период и вероятности возникновения ошибки т.е. остатка 
 * от деления частоты таймера на частоту несущей. Например, при выборе частоты, 
 * десятикратно превышающей несущую т.е. 10 прерываний на период, мы путём подбора
 * делителя сможем достигнуть наиближайшей к 3348,21 * 10 частоте в 33492,82 Гц. 
 * Тогда 33492,82/3348,21 = 10,003. Остаток отделения заметно больше, значит вероятность
 * "перескока" гораздо выше. Под "перескоком" подразумевается ситуация, когда 
 * измерения по текущему периоду происходят таким образом, что на период приходится
 * на одно измерение больше, чем подразумевается. Происходит это из-за неточной
 * кратности частоты и приводит к возникновению подобий фазовых переходов в местах,
 * где их быть не должно.
 * 
 * На количество прерываний на период завязана, по большей части, вся демодуляция 
 * АРСФ пакетов и их приём. 
 */

void TMR2_Init() {
    TMR_BaseConfig_T TMR_BaseConfigStruct;

    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR2);
    
    TMR_BaseConfigStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_BaseConfigStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_BaseConfigStruct.division = 1568-1;
    TMR_BaseConfigStruct.period = 1;
    TMR_BaseConfigStruct.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR2, &TMR_BaseConfigStruct);

    TMR_EnableInterrupt(TMR2, TMR_INT_UPDATE);
    NVIC_EnableIRQRequest(TMR2_IRQn, 1, 0);
			
    TMR_Enable(TMR2);
}

/*!
 * @brief     Инициализация таймера TIM3
 *
 * Прерывание таймера TIM3 используется для приёма АРС сигнала. 
 * Таймер инициализируется со следующими параметрами: 
 * - Частота 6399,51 Гц
 * - Период	 1,563е-4 с
 *
 * Частота таймера выбрана в согласовании с размером буфера #FFT_Buff
 * таким образом, чтобы шаг частоты частотного анализа составлял 25 Гц.
 * Благодаря этому, устройство может чётко отличить АРС код 75 Гц и 125 Гц, 
 * где разница в частотах составляет 50 Гц (справедливо и для других соседних кодов)
 */

void TMR3_Init() {
	
    TMR_BaseConfig_T TMR_BaseConfigStruct;

    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR3);
    
    TMR_BaseConfigStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_BaseConfigStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_BaseConfigStruct.division = 6563-1;
    TMR_BaseConfigStruct.period = 1;
    TMR_BaseConfigStruct.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR3, &TMR_BaseConfigStruct);

    TMR_EnableInterrupt(TMR3, TMR_INT_UPDATE);
    NVIC_EnableIRQRequest(TMR3_IRQn, 1, 0);

    TMR_Enable(TMR3);
}

/*!
 * @brief     Инициализация таймера TIM4
 *
 * Прерывание таймера TIM4 используется для определения времени
 * без приёма АРСФ пакета или АРС сигнала.
 *
 * Таймер инициализируется со следующими параметрами: 
 * - Частота 2 Гц
 * - Период	 0,5 с
 */

void TMR4_Init() {
	
    TMR_BaseConfig_T TMR_BaseConfigStruct;

    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR4);
    
    TMR_BaseConfigStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_BaseConfigStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_BaseConfigStruct.division = 42000-1;
    TMR_BaseConfigStruct.period = 1000;
    TMR_BaseConfigStruct.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR4, &TMR_BaseConfigStruct);

    TMR_EnableInterrupt(TMR4, TMR_INT_UPDATE);
    NVIC_EnableIRQRequest(TMR4_IRQn, 1, 0);

	NoResultCounter = 3;
    TMR_Enable(TMR4);
}

/*!
 * @brief     Прерывание таймера TIM2
 *
 * Прерывание таймера TIM2 обеспечивает приём АРСФ пакетов и запуск демодуляции
 * полученного пакета.
 *
 * Принцип работы функции следующий: 
 * - Считывется текущее значение "подлинного" напряжения на линии АЦП (см. #GetRealVoltage)
 * - Производится проверка готовности системы к приёму нового АРСФ пакета 
 *   (см. #ReadyToStartReceive). Если в настоящий момент прозводится приём 
 *   пакета, то проверка не произвоится
 * - Определение текущего момента времени на соответсвие началу нового АРСФ пакета
 *   (см. #CatchPackageStart). Если в настоящий момент прозводится приём 
 *   пакета, то определение начала пакета не производится 
 * - Если предыдущие проверки не инициировали выход из функции прерывания, то
 *   считается, что в настоящий момент производится приём АРСФ пакета. В действительности,
 *   это не всегда так, поэтому производится проверка текущего приёма на
 *   ложное срабатывание (см. #FasleActivationCatch)
 * - Если в течении заданного времени (5376 тиков или 201 мс), что соответсвует
 *   полному АРСФ пакету, не был определён ложный приём, то функция инициирует
 *   анализ принятого пакета. Полученные в течении времени приёма данные в #VoltagesData
 *   используются для анализа амплитуды в [AmlitudeAnalysis()](#AmlitudeAnalysis) и
 *   для демодуляции сигнала в [ReceiveStop()](#ReceiveStop)
 */

void TMR2_EventCallback() {
	if(TMR_ReadIntFlag(TMR2, TMR_INT_UPDATE) == SET) {		
		if (ADC_ReadStatusFlag(ADC1, ADC_FLAG_EOC)) {
			RealVoltage = GetRealVoltage();
			if (tick < VOL_BUF_LEN) voltageBuff[tick] = RealVoltage;
			RealVoltage = HighPassFilter(RealVoltage);
			
			#ifdef FILTER_TEST
				if (tick < 20000) FilterOutputArray[tick] = (int) RealVoltage;
				else {
					for (int i = 0; i < 20000; i++) USART_Tx_Number(USART, FilterOutputArray[i]);
					while(1);
				}
				tick++;
				FlagsClear();
				return;
			#endif
			
			if (!ReadyToStartReceive(RealVoltage))
			{
				FlagsClear();
				return;
			}
			
			if (!CatchPackageStart(RealVoltage, PrevVoltage, &tick))
			{
				FlagsClear();
				return;
			}
						
			#ifdef DEBUG			
				if (c < 6000) {
					VoltageTestArray[c] = RealVoltage;
					c++;
				}
			#endif
			
			VoltagesData[tick] = RealVoltage;
			FasleActivationCatch(RealVoltage, PrevVoltage, &FalseActivation);
			PrevVoltage = RealVoltage;
			tick++;
		}
		
		FlagsClear();
		
		if (FalseActivation) {
			
			#ifdef DEBUG
				for(int i = 0; i < c; i++) {
					USART_Tx_Number(USART, (int) VoltageTestArray[i]);
				}
				USART_Tx_Char(USART, 13);
			#endif
			
			VarReInit();
			return;
		}
		
		if (tick == TICKS_NUM) {
			AverageVoltage = AmlitudeAnalysis(voltageBuff, VOL_BUF_LEN);
			ReceiveStop(VoltagesData, ZeroCrossTimings, EndOfSample);
		}
   }
 }

/*!
 * @brief     Прерывание таймера TIM3
 *
 * Прерывание таймера TIM3 обеспечивает приём АРС сигнала и запуск частотного анализа
 * полученного сигнала.
 *
 * Принцип работы функции следующий: 
 * - Считывется текущее значение "подлинного" напряжения на линии АЦП (см. #GetRealVoltage)
 * - Производится проверка полученного сигнала на соответсвие сигналу высокого уровня
 *   (см. #LowLevel)
 * - Заполнение массивов #voltageBuff и #FFT_Buff
 * - После приёма VOL_BUF_LEN значений, функция инициирует спектральный
 *   анализ полученного сигнала (см. #SpectrumAnalysis)
 */
 
 void TMR3_EventCallback(void) {
	 if(TMR_ReadIntFlag(TMR3, TMR_INT_UPDATE) == SET) {
		 if (ADC_ReadStatusFlag(ADC1, ADC_FLAG_EOC)) {
			 RealVoltage = GetRealVoltage();
			 if (LowLevel && (RealVoltage > 500)) LowLevel = 0;
			 voltageBuff[voltageBuffCounter] = RealVoltage;
			 voltageBuffCounter++;
			 if (voltageBuffCounter >= VOL_BUF_LEN-256) {
				 FFT_Buff[fft_pos] = RealVoltage;
				 fft_pos++;
			 }
			 if (fft_pos == FFT_N) SpectrumAnalysis(FFT_Buff, LowLevel, voltageBuff);
		 }
	 }
	 FlagsClear();
 }
 
/*!
 * @brief     Прерывание таймера TIM4
 *
 * Прерывание таймера TIM4 обеспечивает информирование системы об отсутсвии приёма
 * сигнала на линии.
 *
 * Принцип работы функции следующий: 
 * - Если #NoResultCounter == 0, то инициируется отображение отсутсвия приёма на 
 *   LCD экране (см. #LCD_Show_No_Result)
 * - Если #NoResultCounter != 0, то #NoResultCounter уменьшается на единицу
 *
 * При каждом новом успешном приёме сигнала (АРС или АРСФ), переменной
 * #NoResultCounter присваевается значение 3, происходит это в #LCD_DQPSK_Result
 * и #LCD_Freq_Result для АРСФ и АРС режимов соответсвенно. Таким образом, если 
 * в течении трёх стабатываний прерывания таймера TMR4 не было произведено удачного
 * приёма, то вызывается [LCD_Show_No_Result()](#LCD_Show_No_Result)
 */
 
 void TMR4_EventCallback(void) {
	 if(TMR_ReadIntFlag(TMR4, TMR_INT_UPDATE) == SET) {
		 if (NoResultCounter == 0) {
			 MovingAverageClear();
			 LCD_Result_Reset();
			 LCD_Show_No_Result();
		 } else NoResultCounter--;
		 TMR_ClearIntFlag(TMR4, TMR_INT_UPDATE);
	 }
 }
 
/*!
 * @brief     Обнуление переменных и массивов, используемых в АРС режиме
 *
 * Обнуляемые переменные и массивы:
 * - #FFT_Buff
 * - #voltageBuff
 * - #voltageBuffCounter
 * - #fft_pos
 * - #LowLevel
 *
 * Используется в [KEY1_IRQHandler()](#KEY1_IRQHandler) при переключении 
 * режимов работы устройства и в [SpectrumAnalysis()](#SpectrumAnalysis)
 * после окончания приёма АРС сигнала
 */
 
 void SpectrumVarReInit() {
	 for (uint16_t i = 0; i < FFT_N; i++) FFT_Buff[i] = 0;
	 for (uint16_t i = 0; i < VOL_BUF_LEN; i++) voltageBuff[i] = 0;
	 voltageBuffCounter = 0;
	 fft_pos = 0;
	 LowLevel = 1;
 }
 
/*!
 * @brief     Обнуление переменных и массивов, используемых в АРСФ режиме
 *
 * Обнуляемые переменные и массивы:
 * - #tick
 * - #PrevVoltage
 * - #FalseActivation
 * - #fft_pos
 * - #ZeroCrossTimings
 * - #EndOfSample
 * - #VoltagesData
 * - #voltageBuff
 *
 * Также запускает [HandlerVarReInit()](#HandlerVarReInit). Используется в 
 * [KEY1_IRQHandler()](#KEY1_IRQHandler) при переключении режимов работы 
 * устройства и в [ReceiveStop()](#ReceiveStop) после окончания приёма АРСФ пакетов
 */

void VarReInit() {
	tick = 0;
	PrevVoltage = 0;
	FalseActivation = 0;
	for (int i = 0; i < 600; i++) 			ZeroCrossTimings[i] = 0;
	for (int i = 0; i < 42; i++)			EndOfSample[i] = 0;
	for (int i = 0; i < 5376; i++)			VoltagesData[i] = 0;
	for (int i = 0; i < VOL_BUF_LEN; i++)	voltageBuff[i] = 0;
	HandlerVarReInit();
	#ifdef DEBUG
		USART_Tx_Char(USART, 13);
		for(int i = 0; i < c; i++) {
			USART_Tx_Number(USART, (int) VoltageTestArray[i]);
			VoltageTestArray[i] = 0;
		}
		USART_Tx_Char(USART, 13);
		c = 0;
	#endif
}

/*!
* @brief     Очистка флагов перефирии
 *
 * Очищает флаги прерываний таймеров TIM2 и TIM3, а также флаг
 * готовности АЦП к передачи текущего принятого значения напряжения
 */

void FlagsClear() {
	TMR_ClearIntFlag(TMR2, TMR_INT_UPDATE);
	TMR_ClearIntFlag(TMR3, TMR_INT_UPDATE);
	ADC_ClearStatusFlag(ADC1, ADC_FLAG_EOC);
}
