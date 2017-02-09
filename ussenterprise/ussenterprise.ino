
#include "SegmentScreen.h"
#include "EnableInterrupt.h"

#define ONOFF A8
#define ENOUGH_WATER A13
#define WHEN_WATER_IS_OUT A10

#define HANA 50
#define PUMPPU 51
#define LINKOUS 52
#define PESU 53

#define TAKING_WATER_MAX_TIME 3000
#define WASHING_MAX_TIME 9000
#define DIRTY_WATER_PUMPING_MAX_TIME 3000
#define TAKING_WATER2_MAX_TIME 3000
#define RINGSING_MAX_TIME 3000
#define PUMPING_OUT_MAX_TIME 3000
#define CERTIFUGING_MAX_TIME 5000

#define WASHING_PROGRAM_TIME 30000

long takingWaterTime = 0;
long washingTime = 0;
long dirtyWaterPmpingTime = 0;
long takingWaterTime2 = 0;
long rinsingTime = 0;
long pumpingOutTime = 0;
long certifugingTime = 0;

long inputTime = 0;

bool TAKING_WATER = false;
bool WASHING = false;
bool DIRTY_WATER_PUMPING = false;
bool TAKING_WATER2 = false;
bool RINGSING = false;
bool PUMPING_OUT = false;
bool CERTIFUGING = false;
bool MACHINE_ON = false;

bool SAMPLING_INPUT = false;

bool ONOFF_PRESSED = false;
bool ENOUGH_WATER_PRESSED = false;
bool WHEN_WATER_IS_OUT_PRESSED = false;


int mseconds = 0;


long takingWaterStartTime = 0;
long pumpingWaterStartTime = 0;

char cathodePins[7] = {
    4, 5, 6, 7, 8, 9, 10
};

SegmentScreen *screens[] = {
    new SegmentScreen(cathodePins, 2),
    new SegmentScreen(cathodePins, 3),
    new SegmentScreen(cathodePins, 11),
    new SegmentScreen(cathodePins, 12),
};

SegmentLayout layout(screens, 4);

void setup()
{
    pinMode(ONOFF, INPUT);
    pinMode(ENOUGH_WATER, INPUT);
    pinMode(WHEN_WATER_IS_OUT, INPUT);

    enableInterrupt(ONOFF, buttonClicked, RISING);
    enableInterrupt(ENOUGH_WATER, buttonClicked, RISING);
    enableInterrupt(WHEN_WATER_IS_OUT, buttonClicked, RISING);

    pinMode(HANA, OUTPUT);
    pinMode(PUMPPU, OUTPUT);
    pinMode(LINKOUS, OUTPUT);
    pinMode(PESU, OUTPUT);

	initTimer();
    initTimer2();

    layout.init();
}

void loop()
{
    long currentTime = millis();
	if (TAKING_WATER)
	{
	    if (currentTime - takingWaterTime > 3000)
	    {
			digitalWrite(HANA, LOW);
			TAKING_WATER = false;
			WASHING = true;
			washingTime = millis();
			digitalWrite(PESU, HIGH);
	    }
	}
	if (WASHING)
	{
	    if (currentTime - washingTime > 9000)
	    {
			digitalWrite(PESU, LOW);
			WASHING = false;
			DIRTY_WATER_PUMPING = true;
			dirtyWaterPmpingTime = millis();
			digitalWrite(PUMPPU, HIGH);
	    }
	}
	if (DIRTY_WATER_PUMPING)
	{
	    if (currentTime - dirtyWaterPmpingTime > 3000)
	    {
			digitalWrite(PUMPPU, LOW);
			DIRTY_WATER_PUMPING = false;
			TAKING_WATER2 = true;
			takingWaterTime2 = millis();
			digitalWrite(HANA, HIGH);
	    }
	}
	if (TAKING_WATER2)
	{
	    if (currentTime - takingWaterTime2 > 3000)
	    {
			digitalWrite(HANA, HIGH);
			TAKING_WATER2 = false;
			RINGSING = true;
			rinsingTime = millis();
			digitalWrite(PESU, HIGH);
	    }
	}
	if (RINGSING)
	{
	    if (currentTime - rinsingTime > 3000)
	    {
			digitalWrite(PESU, LOW);
			RINGSING = false;
			PUMPING_OUT = true;
			pumpingOutTime = millis();
			digitalWrite(PUMPPU, HIGH);
	    }
	}
	if (PUMPING_OUT)
	{
	    if (currentTime - pumpingOutTime > 3000)
	    {
			digitalWrite(PUMPPU, LOW);
			PUMPING_OUT = false;
			CERTIFUGING = true;
			certifugingTime = millis();
			digitalWrite(LINKOUS, HIGH);
	    }
	}
	if (CERTIFUGING)
	{
	    if (currentTime - certifugingTime > 5000)
	    {
			digitalWrite(LINKOUS, LOW);
			CERTIFUGING = false;
	    }
	}
    if (ONOFF_PRESSED)
    {
		ONOFF_PRESSED = false;
	    mseconds = WASHING_PROGRAM_TIME;
	    TAKING_WATER = true;	    
		takingWaterTime = millis();
		digitalWrite(HANA, HIGH);
    }
    if (ENOUGH_WATER_PRESSED)
    {
		if (TAKING_WATER)
		{
			mseconds -= TAKING_WATER_MAX_TIME - (millis() - takingWaterTime);
			digitalWrite(HANA, LOW);
			TAKING_WATER = false;
			WASHING = true;
			washingTime = millis();
			digitalWrite(PESU, HIGH);
		}
		if (TAKING_WATER2)
		{
			mseconds -= TAKING_WATER2_MAX_TIME - (millis() - takingWaterTime2);
			digitalWrite(HANA, LOW);
			TAKING_WATER = false;
			WASHING = true;
			washingTime = millis();
			digitalWrite(PESU, HIGH);
		}
		ENOUGH_WATER_PRESSED = false;
    }
    if (WHEN_WATER_IS_OUT_PRESSED)
    {
		if (DIRTY_WATER_PUMPING)
		{
			mseconds -= DIRTY_WATER_PUMPING_MAX_TIME - (millis() - dirtyWaterPmpingTime);
			digitalWrite(PUMPPU, LOW);
			DIRTY_WATER_PUMPING = false;
			TAKING_WATER2 = true;
			takingWaterTime2 = millis();
			digitalWrite(HANA, HIGH);
		}
		if (PUMPING_OUT)
		{
			mseconds -= PUMPING_OUT_MAX_TIME - (millis() - pumpingOutTime);
			digitalWrite(PUMPPU, LOW);
			PUMPING_OUT = false;
			CERTIFUGING = true;
			certifugingTime = millis();
			digitalWrite(LINKOUS, HIGH);
		}
		WHEN_WATER_IS_OUT_PRESSED = false;
    }

    String s;
    if (mseconds/1000 < 10)
	s = String("000") + String(mseconds/1000);
    else
	s = String("00") + String(mseconds/1000);
    layout.setText(s);

    layout.refresh();
}

void buttonClicked()
{
    startTimer2();
}

ISR(TIMER2_COMPA_vect)
{
    stopTimer2();
    ONOFF_PRESSED = digitalRead(ONOFF);
    ENOUGH_WATER_PRESSED = digitalRead(ENOUGH_WATER);
    WHEN_WATER_IS_OUT_PRESSED = digitalRead(WHEN_WATER_IS_OUT);
}

void startTimer2()
{
    TCNT2 = 0;
    TIMSK2 |= (1 << OCIE2A);
}

void stopTimer2()
{
    TIMSK2 &= ~(1 << OCIE2A);
}

void initTimer2()
{
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;
    OCR2A = 46;
    TCCR2A |= (1 << WGM21);
    TCCR2B |= (1 << CS12) | (1 << CS10);
    TIMSK2 &= ~(1 << OCIE2A);
}


void initTimer() {
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;
	OCR1A = 781;
	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1 << CS12) | (1 << CS10);
	TIMSK1 |= (1 << OCIE1A);	
}

void startTimer() {
	TCNT1  = 0;
	TIMSK1 |= (1 << OCIE2A);
}

void stopTimer() {
	TIMSK1 &= ~(1 << OCIE2A);
}

ISR(TIMER1_COMPA_vect){
	mseconds-=50;
	if (mseconds < 0) mseconds = 0;
}
