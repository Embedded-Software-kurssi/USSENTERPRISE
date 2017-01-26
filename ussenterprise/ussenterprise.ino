
#include "SegmentScreen.h"
#include "EnableInterrupt.h"

#define ONOFF A8
#define ENOUGH_WATER A9
#define WHEN_WATER_IS_OUT A10

#define HANA 50
#define PUMPPU 51
#define LINKOUS 52
#define PESU 53

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
    Serial.begin(9600);
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

    initTimer2();
    layout.init();
    layout.setText("0145");
}

void loop()
{
    long currentTime = millis();
    if (MACHINE_ON)
    {
	if (TAKING_WATER)
	{
	    if (currentTime - takingWaterTime > 3000)
	    {
		digitalWrite(HANA, LOW);
		TAKING_WATER = false;
		WASHING = true;
		washingTime = millis();
	    }
	    else
	    {
		digitalWrite(HANA, HIGH);
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
	    }
	    else
	    {
		digitalWrite(PESU, HIGH);
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
	    }
	    else
	    {
		digitalWrite(PUMPPU, HIGH);
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
	    }
	    else
	    {
		digitalWrite(HANA, HIGH);
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
	    }
	    else
	    {
		digitalWrite(PESU, HIGH);
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
	    }
	    else
	    {
		digitalWrite(PUMPPU, HIGH);
	    }
	}
	if (CERTIFUGING)
	{
	    if (currentTime - certifugingTime > 5000)
	    {
		digitalWrite(LINKOUS, LOW);
		CERTIFUGING = false;
	    }
	    else
	    {
		digitalWrite(LINKOUS, HIGH);
	    }
	}
    }
    if (ONOFF_PRESSED)
    {
	MACHINE_ON = true;
	TAKING_WATER = true;
	takingWaterTime = millis();
	ONOFF_PRESSED = false;
    }
    if (ENOUGH_WATER_PRESSED)
    {
	if (TAKING_WATER)
	{
	    digitalWrite(HANA, LOW);
	    TAKING_WATER = false;
	    WASHING = true;
	    washingTime = millis();
	}
	if (TAKING_WATER2)
	{
	    digitalWrite(HANA, HIGH);
	    TAKING_WATER2 = false;
	    RINGSING = true;
	    rinsingTime = millis();
	}
	ENOUGH_WATER_PRESSED = false;
    }
    if (WHEN_WATER_IS_OUT_PRESSED)
    {
	if (DIRTY_WATER_PUMPING)
	{
	    digitalWrite(PUMPPU, LOW);
	    DIRTY_WATER_PUMPING = false;
	    TAKING_WATER2 = true;
	    takingWaterTime2 = millis();
	}
	if (PUMPING_OUT)
	{
	    digitalWrite(PUMPPU, LOW);
	    PUMPING_OUT = false;
	    CERTIFUGING = true;
	    certifugingTime = millis();
	}
	WHEN_WATER_IS_OUT_PRESSED = false;
    }

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
    ENOUGH_WATER_PRESSED = digitalRead(ENOUGH_WATER_PRESSED);
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
    //set timer0 interrupt at 500Hz
    TCCR2A = 0; // set entire TCCR0A register to 0
    TCCR2B = 0; // same for TCCR0B
    TCNT2 = 0;  //initialize counter value to 0
    // set compare match register for 500Hz increments
    OCR2A = 46; // = (16*10^6) / (500*64) - 1 (must be <256)
    // turn on CTC mode
    TCCR2A |= (1 << WGM21);
    // Set CS02 for 256 prescaler
    TCCR2B |= (1 << CS12) | (1 << CS10);
    // enable timer compare interrupt
    TIMSK2 &= ~(1 << OCIE2A);
}
