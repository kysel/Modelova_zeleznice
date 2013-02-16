/*
 * servo.h
 *
 * Created: 26.1.2013 18:09:51
 *  Author: jirka1
 */ 

#define SERVO_PORT  PORTB
#define SERVO_DDR   DDRB

// Upto 8 servos (since pulses are generated in
// sequence + only one port is used).
#define N_SERVOS    8

static const uint16_t polohyPrestavniku[N_SERVOS][2]={
//jedna	druha
{1000,	2000},	//0
{1000,	2000},	//1
{1000,	2000},	//2
{1000,	2000},	//3
{1000,	2000},	//4
{1000,	2000},	//5
{1000,	2000}	//6
};

// Servo times (this is Futaba timing).
#define SERVO_MIN    920 // microseconds
#define SERVO_MAX   2120 // microseconds
#define SERVO_MID   (SERVO_MIN + SERVO_MAX) / 2

// Time between servo pulses.
#define SERVO_FRAME 20000 // microseconds (50Hz)

// Time slot available for each servo.
#define SERVO_TIME_DIV (SERVO_FRAME / N_SERVOS)

#if (SERVO_TIME_DIV < SERVO_MAX + 50)
#warning "Output fewer servo signals or increase SERVO_FRAME"
#endif
#if ((SERVO_TIME_DIV * (F_CPU / 1000000UL)) >= 0xFF00)
#warning "Output more servo signals or decrease SERVO_FRAME (or use the prescaler)"
#endif

// Computing timer ticks given microseconds.
// Note, this version works out ok with even MHz F_CPU (e.g., 1, 2, 4, 8, 16 MHz).
// (Not a good idea to have this end up as a floating point operation)
#define US2TIMER1(us) ((us) * (uint16_t)(F_CPU / 1E6))

// Servo times - to be entered as timer1 ticks (using US2TIMER1).
// This must be updated with interrupts disabled.
volatile uint16_t servoTime[N_SERVOS];

// Servo output allocation (on a single port currently).
const static uint8_t servoOutMask[N_SERVOS] = {
	0b00000001, // PX0
	0b00000010, // PX1
	0b00000100, // PX2
	0b00001000, // PX3
	0b00010000, // PX4
	0b00100000, // PX5
	0b01000000, // PX6
	0b10000000, // PX7
};
// Servo mask is just the above masks ored.
#define SERVO_MASK 0xff

void servoStart(void)
{
	// Outputs
	SERVO_DDR |= SERVO_MASK;
	// Setupt a first compare match
	OCR1A = TCNT1 + US2TIMER1(100);
	// start timer 1 with no prescaler
	TCCR1B = (1 << CS10);
	// Enable interrupt
	TIMSK |= (1 << OCIE1A);
}

void servoSet(uint8_t servo, uint16_t time /* microseconds */)
{
	uint16_t ticks = US2TIMER1(time);
	cli();
	servoTime[servo] = ticks;
	sei();
}


