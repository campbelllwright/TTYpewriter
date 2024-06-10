

#define TW_CANON_TYPESTAR_7 1

// CONFIGURATION OPTIONS
#define KB_POLLRATE 1000

// HARDWARE OPTIONS
#define NUM_ROWS 8
#define NUM_COLS 8
#define GPIO_ROW_START 22
#define GPIO_COL_START A8

#define SHIFT_PIN 30

uint8_t uchDataIn[NUM_ROWS] = {0};
volatile boolean matrixStatus[NUM_ROWS][NUM_COLS] = {0};
volatile uint8_t txBuffer[32] = {0};
volatile uint8_t txBufferPos = 0;
volatile uint8_t currentRow = 0;
volatile uint8_t currentCol = 0;
int timeSinceLastChar = 0;
char lastChar = 0;
char currChar = 0;
volatile boolean currentLedState = 0;

#ifdef TW_CANON_TYPESTAR_7
uint8_t uchKeyboardMap[NUM_COLS][NUM_ROWS] = 
{ 
	{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' },
	{ 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p' },
	{ 'q', 'r', 's', 't', 'u', 'v', 'w', 'x' },
	{ 'y', 'z', '0', '1', '2', '3', '4', '5' },
	{ '6', '7', '8', '9', '=', '?', ';', '\'' },
	{ ',', '.', '?', '?', ' ', '\n', 'R', 'B' },
	{ 'C', 'D', 0, 0, 'G', 'H', 'I', 'J' },
	{ 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'S' },
};
uint8_t uchKeyboardShiftMap[NUM_COLS][NUM_ROWS] = 
{ 
	{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H' },
	{ 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P' },
	{ 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X' },
	{ 'Y', 'Z', ')', '!', '@', '#', '$', '%' },
	{ '^', '&', '&', '(', '+', '?', ':', '"' },
	{ '<', '>', '~', '~', ' ', '\n', 'R', 'B' },
	{ '~', '~', '~', '~', '~', '~', '~', '~' },
	{ '~', '~', '~', '~', '~', '~', '~', '~' },
};
#else
uint8_t uchKeyboardMap[NUM_COLS][NUM_ROWS] = 
{ 
	{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' },
	{ 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p' },
	{ 'q', 'r', 's', 't', 'u', 'v', 'w', 'x' },
	{ 'y', 'z', '0', '1', '2', '3', '4', '5' },
	{ '6', '7', '8', '9', '=', '?', ';', '\'' },
	{ ',', '.', '?', '?', ' ', '\n', 'R', 'B' },
	{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' },
	{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' },
};
uint8_t uchKeyboardShiftMap[NUM_COLS][NUM_ROWS] = 
{ 
	{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H' },
	{ 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P' },
	{ 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X' },
	{ 'Y', 'Z', ')', '!', '@', '#', '$', '%' },
	{ '^', '&', '&', '(', '+', '?', ':', '"' },
	{ '<', '>', '~', '~', ' ', '\n', 'R', 'B' },
	{ '~', '~', '~', '~', '~', '~', '~', '~' },
	{ '~', '~', '~', '~', '~', '~', '~', '~' },
};
#endif


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

uint8_t getColFromRow(uint8_t uchRowNum)
{
	uint8_t uchRowData = 0xFF;
    digitalWrite(uchRowNum + GPIO_ROW_START, HIGH);
	for (uint8_t uchCount = 0; uchCount < NUM_COLS; uchCount++)
	{
		if (digitalRead((uchCount + GPIO_COL_START)) == 1)
		{
			uchRowData = uchCount;
		}
	}
	digitalWrite(uchRowNum + GPIO_ROW_START, LOW);
	return uchRowData;
}

char pollKB()
{
    uint8_t uchCol = 0xFF;
    for (uint8_t uchRowCounter = 0; uchRowCounter < NUM_ROWS; uchRowCounter++)
    {   
		uchCol = getColFromRow(uchRowCounter);
	    if (uchCol != 0xFF)
	    {
		    if (digitalRead(SHIFT_PIN) == 1)
		    {
			    return (char)uchKeyboardShiftMap[uchCol][uchRowCounter];
		    }
		    else
		    {
				return (char)uchKeyboardMap[uchCol][uchRowCounter]; 
		    }
	    }
    }
    return NULL;
}

//lastChar = currChar
//poll updatingCurrChar
//if ((lastChar != currChar) and (currChar != 0xFF(none))) or ((timeSinceLastPrint > KB_POLLRATE) and (currChar != 0xFF)), print. (also check for changes in shift)

/*void loop() {
    // put your main code here, to run repeatedly:
    lastChar = currChar;
    currChar = pollKB();
	if (((lastChar != currChar) and (currChar != NULL)))// or (((timeSinceLastChar + KB_POLLRATE) < millis()) and (currChar != NULL))) and currChar
	{
		Serial.print(currChar);
        Serial3.print(currChar);
        timeSinceLastChar = millis();
	}
    //printMapData('a');
    //delay(KB_POLLRATE);
    delay(1);
}*/


    //this works but polling is finicky
/*void loop() {
    // put your main code here, to run repeatedly:
    uint8_t uchCol = 0xFF;
    for (uint8_t uchRowCounter = 0; uchRowCounter < NUM_ROWS; uchRowCounter++)
    {   
		uchCol = getColFromRow(uchRowCounter);
	    if (uchCol != 0xFF)
	    {
		    if (digitalRead(SHIFT_PIN) == 1)
		    {
			    Serial.print((char)uchKeyboardShiftMap[uchCol][uchRowCounter]);
                Serial3.print((char)uchKeyboardShiftMap[uchCol][uchRowCounter]);
                Serial.write()
		    }
		    else
		    {
				Serial.print((char)uchKeyboardMap[uchCol][uchRowCounter]); 
                Serial3.print((char)uchKeyboardMap[uchCol][uchRowCounter]);
		    }
	    }
	    uchCol = 0xFF;
    }
    delay(KB_POLLRATE);
}*/


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

void loop() {
    // put your main code here, to run repeatedly:
    //uint8_t uchCol = 0xFF;
    for (currentRow = 0; currentRow < NUM_ROWS; currentRow++)
    {   
        digitalWrite(currentRow + GPIO_ROW_START, HIGH);

        delayMicroseconds(100);
        digitalWrite(currentRow + GPIO_ROW_START, LOW);
    }
    
    if(txBufferPos > 0)
    {
        txBuffer[txBufferPos] = '\n';
        txBufferPos++;
        for(uint8_t i = 0; i < txBufferPos; i++){
            Serial.write(txBuffer[i]);
            txBuffer[i] = 0;
        }
        txBufferPos = 0;
    }
    printKBMatrix();
    delay(KB_POLLRATE);
}


ISR(PCINT2_vect)
{
    currentLedState = !currentLedState;
    digitalWrite(LED_BUILTIN, currentLedState);
    for(uint8_t i = 0; i< NUM_COLS; i++){
        if((digitalRead(i+GPIO_COL_START) == 1) & !((currentRow == 6)&(i==2)) & !((currentRow == 6)&(i==3))) 
        {
            txBuffer[txBufferPos] = uchKeyboardMap[currentRow][i];
            matrixStatus[currentRow][i] = 1;
            txBufferPos++;
        }
        //else {
        //    matrixStatus[currentRow][i] = 0;
        //}
    }
    /*switch (PINK) {
        case 0x01: //A8
            txBuffer[txBufferPos] = uchKeyboardMap[currentRow][0];
            txBufferPos++;
            break;
        case 0x02: //A9
            txBuffer[txBufferPos] = uchKeyboardMap[currentRow][1];
            txBufferPos++;
            break;
        case 0x04: //A10
            txBuffer[txBufferPos] = uchKeyboardMap[currentRow][2];
            txBufferPos++;
            break;
        case 0x08: //A11
            txBuffer[txBufferPos] = uchKeyboardMap[currentRow][3];
            txBufferPos++;
            break;
        case 0x10: //A12
            txBuffer[txBufferPos] = uchKeyboardMap[currentRow][4];
            txBufferPos++;
            break;
        case 0x20: //A13
            txBuffer[txBufferPos] = uchKeyboardMap[currentRow][5];
            txBufferPos++;
            break;
        case 0x40: //A14
            txBuffer[txBufferPos] = uchKeyboardMap[currentRow][6];
            txBufferPos++;
            break;
        case 0x80: //A15
            txBuffer[txBufferPos] = uchKeyboardMap[currentRow][7];
            txBufferPos++;
            break;
        default:
            break;
    }*/
}
