


//KEYBOARD TYPE
#define TW_CANON_TYPESTAR_7 1

#include "KeyboardMap.h"

// CONFIGURATION OPTIONS
#define KB_POLLRATE_MS 100
#define KEY_TIME_US 100
//#define DEBUG_ON 1

// HARDWARE/PIN OPTIONS
#define GPIO_ROW_START 22
#define GPIO_COL_START A8
#define SHIFT_PIN 30

uint8_t uchDataIn[NUM_ROWS] = {0};
#ifdef DEBUG_ON
volatile boolean matrixStatus[NUM_ROWS][NUM_COLS] = {0};
#endif
volatile uint8_t txBuffer[32] = {0};
volatile uint8_t txBufferPos = 0;
volatile uint8_t currentRow = 0;
volatile uint8_t currentCol = 0;
int timeSinceLastChar = 0;
char lastChar = 0;
char currChar = 0;
volatile boolean currentLedState = 0;




void setupInterrupts()
{
    //enable Pin change interrupts 16-23(A8-A15)
    PCMSK2 = 0xFF; 
    PCICR = 0x04;
}


void setup() {
    Serial.begin(9600);
    Serial3.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
  // put your setup code here, to run once:
    for(int i = 0; i < NUM_ROWS; i++){
        pinMode(i + GPIO_ROW_START, OUTPUT);
        digitalWrite(i + GPIO_ROW_START, LOW);
    }
    for(int i = 0; i < NUM_COLS; i++){
        pinMode((i + GPIO_COL_START), INPUT);
    }
    pinMode(SHIFT_PIN, INPUT);
    setupInterrupts();
}

#ifdef DEBUG_ON
void printKBMatrix(){
    for(int i = 0; i< NUM_ROWS; i++)
    {
        for(int j = 0; j<NUM_COLS; j++)
        {
            Serial.print(matrixStatus[i][j]);
            matrixStatus[i][j] = 0;
        }
        Serial.print('\n');
    }
}
#endif

void loop() {
    for (currentRow = 0; currentRow < NUM_ROWS; currentRow++)
    {   
        digitalWrite(currentRow + GPIO_ROW_START, HIGH);

        delayMicroseconds(KEY_TIME_US);
        digitalWrite(currentRow + GPIO_ROW_START, LOW);
    }
    if(txBufferPos > 0)
    {
        for(uint8_t i = 1; i <= txBufferPos; i++){
            Serial.write((uint8_t)txBuffer[i]);
            txBuffer[i] = 0;
        }
        txBufferPos = 0;
    }
    #ifdef DEBUG_ON
    printKBMatrix();
    #endif
    delay(KB_POLLRATE_MS);
}


ISR(PCINT2_vect)
{
    currentLedState = !currentLedState;
    digitalWrite(LED_BUILTIN, currentLedState);
    for(uint8_t i = 0; i< NUM_COLS; i++){
        #ifdef TW_CANON_TYPESTAR_7
        if((digitalRead(i+GPIO_COL_START) == 1) & !((currentRow == 6)&(i==2)) & !((currentRow == 6)&(i==3))) 
        #else
        if(digitalRead(i+GPIO_COL_START) == 1)
        #endif
        {
            char nextKey;
            if(digitalRead(SHIFT_PIN) == 0)
            {
                nextKey = uchKeyboardMap[currentRow][i];
            }
            else {
                nextKey = uchKeyboardShiftMap[currentRow][i];
            }
            if(txBuffer[txBufferPos] != nextKey)
            {
                txBufferPos++;
                txBuffer[txBufferPos] = nextKey;
            }
            #ifdef DEBUG_ON
            matrixStatus[currentRow][i] = 1;
            #endif
        }
    }
}
