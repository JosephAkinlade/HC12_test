#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "system.h"
#include "hc12.h"
#include "button.h"
#include "lcd.h"

Button_t BUTTON = {BUTTON1, BUTTON_NOT_PRESSED, false};
uint8_t dataToSend[5] = "Hello";
uint8_t rxBuffer[5] = {0};
uint32_t tick1 = 0;
uint32_t tick2 = 0;
uint32_t diff = 0;

int main(void)
{
	System_Init();
	Button_Init();
	HC12_Init();
	HC12_RxBufferInit(rxBuffer, 5);
	LCD_Init();
	LCD_Set_Cursor(0,0);
	LCD_Write_String("Time taken: ");
	LCD_Set_Cursor(1,0);
	
	while(1)
	{
		Button_Poll(&BUTTON);
		
		if(BUTTON.isDebounced && !BUTTON.prevPressed)
		{
			BUTTON.prevPressed = true;
			GPIO_OutputWrite(GPIOC, GPIO_PIN13, true);
			HC12_TransmitBytes(dataToSend, 5);
			tick1 = System_GetTick();	
		}
		
		else if(!BUTTON.isDebounced && BUTTON.prevPressed)
		{
			GPIO_OutputWrite(GPIOC, GPIO_PIN13, false);
			BUTTON.prevPressed = false;
		}
		
		if(HC12_Rx_BufferFull())
		{
			System_TimerDelayMs(100);
			tick2 = System_GetTick();
			diff = tick2 - tick1;
			LCD_Write_Int(diff);
		}
		
		if(HC12_IncompleteRxData() || (rxBuffer[0] == IDLE_CHARACTER))
		{
			HC12_RxBufferInit(rxBuffer, 5);
		}		
		
	}
}

