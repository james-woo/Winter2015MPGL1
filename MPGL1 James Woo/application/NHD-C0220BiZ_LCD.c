#include "configuration.h"

extern volatile u32 G_u32SystemTime1ms;

volatile fnCode_type G_LcdStateMachine;

static u32 Lcd_u32Timer;

/*------------------------------------------------------------------------------
Function LCDCommand

Description:
Queues a command char to be sent to the LCD using the TWI messaging function. 

Requires:
	- u8Command_ is an acceptable command value for the LCD as taken from the
    "LCD Commands" list in lcd_nhd_c0220biz.h header file.  The command must
    be complete and include any optional bits.

Promises:
  - The command is queued and will be sent to the LCD at the next
    available time.
*/
void LCDCommand(u8 u8Command_)
{
  static u8 au8LCDWriteCommand[] = {LCD_CONTROL_COMMAND, 0x00};

  /* $$$$ Update the command paramter into the command array */
  au8LCDWriteCommand[1] = u8Command_;
    
  /* $$$$ Queue the command to the I²C application */
  TWI0WriteData(LCD_ADDRESS, sizeof(au8LCDWriteCommand), &au8LCDWriteCommand[0], STOP);

  
} /* end LCDCommand() */

/*------------------------------------------------------------------------------
Function: LCDMessage

Description:
Sends a text message to the LCD to be printed at the address specified.  

Requires:
  - LCD is initialized
  - u8Message_ is a pointer to a NULL-terminated C-string
	- The message to display is no more than (40 - the selected display location) 
    characters in length
  - Any characters not desired on screen that will not be overwritten need to 
    be erased first

Promises:
  - Message to set cursor address in the LCD is queued, then message data 
    is queued to the LCD to be displayed. 
*/
void LCDMessage(u8 u8Address_, u8 *u8Message_)
{ 
  u8 u8Index; 
  static u8 au8LCDMessage[LCD_MESSAGE_OVERHEAD_SIZE + LCD_MAX_MESSAGE_SIZE] = 
                           {LCD_CONTROL_DATA};
  
  /* Set the cursor to the correct address */
  LCDCommand(LCD_ADDRESS_CMD | u8Address_);
  
  /* Fill the message */
  u8Index = 1;
  while(*u8Message_ != '\0')
  {
    au8LCDMessage[u8Index++] = *u8Message_++;
  }
    
  /* Queue the message */
  TWI0WriteData(LCD_ADDRESS, u8Index, au8LCDMessage, STOP);

} /* end LCDMessage() */


/*------------------------------------------------------------------------------
Function: LCDClearChars

Description:
Clears a number of chars starting from the address specified.  This function is
not meant to span rows.

Requires:
  - LCD is initialized
  - u8Address_ is the starting address where the first character will be cleared
	- u8CharactersToClear_ is the number of characters to clear and does not cause 
    the cursor to go past the available data RAM.

Promises:
  - Message to set cursor address in the LCD is queued, then message data 
    consisting of all ' ' characters is queued to the LCD to be displayed. 
*/
void LCDClearChars(u8 u8Address_, u8 u8CharactersToClear_)
{ 
  /* $$$$ Add this function */
  u8 u8Index; 
  static u8 au8LCDMessage[LCD_MESSAGE_OVERHEAD_SIZE + LCD_MAX_MESSAGE_SIZE] = 
                           {LCD_CONTROL_DATA};
  
  /* Set the cursor to the correct address */
  LCDCommand(LCD_ADDRESS_CMD | u8Address_);
  
  /* $$$$ Fill the message characters with ' ' */
  for(u8Index = 0; u8Index < u8CharactersToClear_; u8Index++)
  {
    au8LCDMessage[u8Index + 1] = ' ';
  }
      
  /* Queue the message */
  TWI0WriteData(LCD_ADDRESS, u8Index, au8LCDMessage, STOP);
      	
} /* end LCDClearChars() */


/*------------------------------------------------------------------------------
Function: LcdInitialize

Description:
Initializes the LCD task and manually sends a message to the LCD

Requires:
  - 

Promises:
  - LCD task Setup and LCD functions can now be called
*/
void LcdInitialize(void)
{
  u8 au8Commands[] = 
  {
    LCD_FUNCTION_CMD, LCD_FUNCTION2_CMD, LCD_BIAS_CMD, 
    LCD_CONTRAST_CMD, LCD_DISPLAY_SET_CMD, LCD_FOLLOWER_CMD 
  };
  
  u8 au8Welcome[] = "PARTY TIME!!!         ";
  
  /* State to Idle */
  G_LcdStateMachine = LcdSM_Idle;
  
  /* Turn on LCD wait 40 ms for it to setup */
  AT91C_BASE_PIOB->PIO_SODR = PB_09_LCD_RST;
  Lcd_u32Timer = G_u32SystemTime1ms;
  while( !IsTimeUp(&Lcd_u32Timer, LCD_STARTUP_DELAY) );
  
  /* Send Control Command */
  TWI0WriteByte(LCD_ADDRESS, LCD_CONTROL_COMMAND, NO_STOP);
  
  /* Send Control Commands */
  TWI0WriteData(LCD_ADDRESS, NUM_CONTROL_CMD, &au8Commands[0], NO_STOP);
  
  /* Wait for 200 ms */
  Lcd_u32Timer = G_u32SystemTime1ms;
  while( !IsTimeUp(&Lcd_u32Timer, LCD_CONTROL_COMMAND_DELAY) );
  
  /* Send Final Command to turn it on */
  TWI0WriteByte(LCD_ADDRESS, LCD_DISPLAY_CMD | LCD_DISPLAY_ON /*| LCD_DISPLAY_CURSOR | LCD_DISPLAY_BLINK*/, STOP);

  /* Blacklight - White */
  LedOn(LCD_RED);
  LedOn(LCD_GREEN);
  LedOn(LCD_BLUE);
    
  LedOn(WHITE);
  LedOn(PURPLE);
  LedOn(BLUE);
  LedOn(CYAN);
  LedOn(GREEN);
  LedOn(YELLOW);
  LedOn(ORANGE);
  LedOn(RED);
  
  TWI0WriteByte(LCD_ADDRESS, LCD_CONTROL_DATA, NO_STOP);
  TWI0WriteData(LCD_ADDRESS, 20, &au8Welcome[0], STOP);
  
  Lcd_u32Timer = G_u32SystemTime1ms;
  while( !IsTimeUp(&Lcd_u32Timer, LCD_INIT_MSG_DISP_TIME) );
  
  Lcd_u32Timer = G_u32SystemTime1ms;

}


/*------------------------------------------------------------------------------
Function: LcdSM_Idle

Description:
Takes two messages and shifts them circularly around the LCD screen
Requires:
  - LCD is initialized
  - the two msgs are 20 char long and are null terminated

Promises:
  - 
*/
void LcdSM_Idle(void)
{
  /* Msgs */
  static const u8 au8Eng[] = "BUTTON2:Little lamb    ";
  static const u8 au8MPG[] = "BUTTON3:Fur Elise      ";
  u8 au8Temp[20];
  
  /* Indexes */
  static u8 u8ResetIndex = 0;
  static u8 u8Index = 0;
  
  
  if(IsTimeUp(&Lcd_u32Timer, 500))
  {
    
    u8Index = u8ResetIndex;
    for(u8 i = 0; i < 20; i++, u8Index++)
    { 
      if( u8Index == 20)
      {
        u8Index = 0; 
      }
      au8Temp[u8Index] = au8Eng[i];
    }
    
    LCDMessage(LINE1_START_ADDR, au8Temp);
    
    u8Index = u8ResetIndex;
    for(u8 i = 0; i < 20; i++, u8Index++)
    { 
      if( u8Index == 20)
      {
        u8Index = 0; 
      }
      au8Temp[u8Index] = au8MPG[i];
    }
    
    LCDMessage(LINE2_START_ADDR, au8Temp);
    u8ResetIndex++;
    
    if(u8ResetIndex == 20)
    {
      u8ResetIndex = 0;
    }
    
    Lcd_u32Timer = G_u32SystemTime1ms;
  }
  
}