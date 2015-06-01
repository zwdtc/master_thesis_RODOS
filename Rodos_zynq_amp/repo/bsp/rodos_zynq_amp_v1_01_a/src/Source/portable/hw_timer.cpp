/**
 * @file hw_timer.cpp(v2.0) modified based on the v1.0.
 * @data 2014/10/30 7:50
 * @anther Wei Zhang
 * @Copyright 2014 DLR
 *
 ************************************************/
/*
 * @file hw_timer.cpp(v1.0)
 * @date 2012/08/22
 * @author Michael Ruffer, modified by Johannes Freitag
 *
 * Copyright 2012 University of Wuerzburg
 *
 * @brief Timer for system time and preemption
 */
#include "rodos.h"
#include "hw_specific.h"
#include "params.h"
#include "xscutimer.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xtime_l.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

#define XSCUTIMER_CLOCK_HZ XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ/2

//timer info
#define TIMER_DEVICE_ID		XPAR_XSCUTIMER_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_IRPT_INTR		XPAR_SCUTIMER_INTR

int64_t Timer::microsecondsInterval = PARAM_TIMER_INTERVAL;

static XScuTimer timer;						/* A9 timer counter */
static XScuGic InterruptController;         /* Interrupt controller instance */
static XScuGic Intc;
long timerTickCounter = 0;                     /* counter the time tick, initilize to 0*/


static void SetupInterruptSystem(XScuGic *GicInstancePtr, XScuTimer *TimerInstancePtr, u16 TimerIntrId);
static void TimerIntrHandler(void *CallBackRef);

static void SetupInterruptSystem(XScuGic *GicInstancePtr,
		XScuTimer *TimerInstancePtr, u16 TimerIntrId)
{

		XScuGic_Connect(GicInstancePtr, TimerIntrId,
						(Xil_ExceptionHandler)TimerIntrHandler,
						(void *)TimerInstancePtr);
}
	
void Timer::init() {
	//the code below is copied from Freertos port.c code, without the interrupt controller code.
	int Status;
	XScuGic_Config *IntcConfig; /* The configuration parameters of the
									interrupt controller */
	/*
	 * Initialize the interrupt controller driver
	 */
	IntcConfig = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if (IntcConfig == NULL) {
		return;
	}

	Status = XScuGic_CfgInitialize(&Intc, IntcConfig,
									IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return;
	}
	
	//int Status;
	XScuTimer_Config *ScuConfig;

	ScuConfig = XScuTimer_LookupConfig(TIMER_DEVICE_ID);

	Status = XScuTimer_CfgInitialize(&timer, ScuConfig,
									ScuConfig->BaseAddr);
	if (Status != XST_SUCCESS) {
		return;
	}
	
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (IntcConfig == NULL) {
		return;
	}
		
	//enable the interrupt for the Timer at GIC
	XScuGic_Enable(&Intc, TIMER_IRPT_INTR);
	/*
	 * set prescaler value.
	 */
	XScuTimer_SetPrescaler(&timer, 9);
	/*
	 * Enable Auto reload mode.
	 */
	 
	XScuTimer_EnableAutoReload(&timer);

	/*
	 * Load the timer counter register.
	 */
	//XScuTimer_LoadTimer(&timer, 0x0000ffff);
	//enable interrupt on the timer
	//XScuTimer_EnableInterrupt(&timer);
	//XScuTimer_Start(&timer);
}

static void TimerIntrHandler(void *CallBackRef)
{

	//XScuTimer_DisableInterrupt(&timer);
	XScuTimer *TimerInstancePtr = (XScuTimer *) CallBackRef;
	XScuTimer_ClearInterruptStatus(TimerInstancePtr);
	xprintf("the Prescaler value = %d\n\r", XScuTimer_GetPrescaler(TimerInstancePtr));
	timerTickCounter++;
	//for testing purpose
	if(100 == timerTickCounter){
		xprintf("****Timer Event!!!!!!!!!!!!!****\n\r");
		timerTickCounter = 0;
	}
	//XScuTimer_EnableInterrupt(&timer);

}	

/**
 * start timer
 * not necessary in this port because interrupts are disabled in swi routine contextSwitch
 */
void Timer::start() {
}

/**
 * stop timer
 * not necessary in this port because interrupt are disabled in swi routine contextSwitch
 */
void Timer::stop() {
}

/**
 * set timer interval
 */
void Timer::setInterval(const int64_t microsecondsInterval) {
	Timer::microsecondsInterval = microsecondsInterval;
}

/**
 * time at bootup
 */
uint64_t initNanoTime = 0;

/**
 * RODOS time
 */
uint64_t hwGetNanoseconds() {
	//return (((uint64_t) GET32(CLOL))
	//		| ((uint64_t) GET32(CLOH) << 32)) * 1000 - initNanoTime;
	//invoke the Micro to get the current timer counter register value.
	XTime Global_time;
	XTime_GetTime(&Global_time);
	return (uint64_t)(Global_time);
}

/**
 * set time at bootup
 */
void hwInitTime() {
	//initNanoTime = (((uint64_t) GET32(CLOL))
	//		| ((uint64_t) GET32(CLOH) << 32)) * 1000;
}

uint64_t hwGetAbsoluteNanoseconds(void) {
	return hwGetNanoseconds(); // + timeAtStartup;
}

#ifndef NO_RODOS_NAMESPACE
}
#endif

