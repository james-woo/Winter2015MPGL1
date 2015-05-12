/***********************************************************************************************************************
File: main.c                                                                

Description:
Container for the MPG firmware.  
***********************************************************************************************************************/

#include "configuration.h"
#include "music.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32SystemFlags = 0;                     /* Global system flags */
volatile u32 G_u32ApplicationFlags = 0;                /* Global applications flags: set when application is successfully initialized */

/*--------------------------------------------------------------------------------------------------------------------*/
/* External global variables defined in other files (must indicate which file they are defined in) */
extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern volatile fnCode_type G_ButtonStateMachine;      /* From buttons.c           */
extern volatile fnCode_type G_UartStateMachine;        /* From sam3u_uart.c        */
extern volatile fnCode_type G_MessagingStateMachine;   /* From messaging.c         */
extern volatile fnCode_type G_DebugStateMachine;       /* From debug.c             */
extern volatile fnCode_type G_AudioTestStateMachine;   /* From mpgl1-audio-test    */
extern volatile fnCode_type G_PlayAudioStateMachine;   /* From mpgl1-audio-test    */

extern volatile fnCode_type G_LcdStateMachine;
extern volatile fnCode_type G_TWIStateMachine;

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Main_" and be declared as static.
***********************************************************************************************************************/


/***********************************************************************************************************************
Main Program
Main has two sections:

1. Initialization which is run once on power-up or reset.  All drivers and applications are setup here without timing
contraints but must complete execution regardless of success or failure of starting the application. 

2. Super loop which runs infinitely giving processor time to each application.  The total loop time should not exceed
1ms of execution time counting all application execution.  SystemSleep() will execute to complete the remaining time in
the 1ms period.
***********************************************************************************************************************/

void playSong(u32 music_notes[], u16 music_length[], int speedDivisor, int musLen){
  u32 u32Timer;
  u8 au8LedStartupMsg[] = "LED functions ready\n\r";
  u8* pu8Parser; 
  
  // This is to remember what the old maximum value for the LEDs was
  // With this value, we can ensure that the following set of displayed LEDs
  // isn't the exact same, even if the note was close enough in pitch 
  
  int oldMax = 0; 
  
  // The main loop that plays the song 
  for(u8 i = 0; i < musLen; i++)
  {
    // Configure Buzzer to play appropriate note 
    PWMAudioSetFrequency(AT91C_PWMC_CHID0, music_notes[i]);
    PWMAudioOn(AT91C_PWMC_CHID0);
    
    //Calculations that provide a value between 0 and 7
    //This number is used to know how many LEDs to display
    //The calculations makes it such that the higher the frequency, the 
    //more LEDs are displayed 
    int calculated = (int)((music_notes[i]-130)/55); 
    int max = calculated>7?7:calculated; 
    
    if (i>1)
      if (max==oldMax)
        if (music_notes[i-1]>music_notes[i])
          max--; 
        else
          max++; 
    
    //Turn on all the requested LEDs 
    for (int i = 0; i <=max; i++)
      LedOn(i); 
    
    // Spend the required amount of length for each note 
    for(u16 j = 0; j < music_length[i]/speedDivisor; j++)
    {
      u32Timer = G_u32SystemTime1ms;
      while( !IsTimeUp(&u32Timer, 1) );
    }
    
    //Turn off all the LEDs we turned on 
    for (int i = 0; i <=max; i++)
      LedOff(i);
    
    oldMax = max; 
  }

  /* Final update to set last state, hold for a short period */
  LedUpdate();
  while( !IsTimeUp(&u32Timer, 200) );
  
  /* Turn off the buzzers */
  PWMAudioOff(AT91C_PWMC_CHID0);

  /* Report that LED system is ready */
  pu8Parser = &au8LedStartupMsg[0];
  while(*pu8Parser != NULL)
  {
    /* Attempt to queue the character */
    if( Uart_putc(*pu8Parser) )
    {
      /* Advance only if character has been sent */
      pu8Parser++;
    }
  }
}

void main(void)
{
  G_u32SystemFlags |= _SYSTEM_INITIALIZING;
  // Check for watch dog restarts

  /* Low level initialization */
  WatchDogSetup(); /* During development, set to not reset processor if timeout */
  GpioSetup();
  ClockSetup();
  InterruptSetup();
  SysTickSetup();

  /* Driver initialization */
  MessagingInitialize();
  UartInitialize();
  LedInitialize();
  ButtonInitialize();
  TWIInitialize();
  
  /* Application initialization */
  DebugInitialize();
  LcdInitialize();
  
  /* Exit initialization */
  G_u32SystemFlags &= ~_SYSTEM_INITIALIZING;
  
  /* "Mary had a little lamb" notes and their length*/
  u32 maryNotes[] = { B4, A4, G4, A4, B4, B4, B4, A4, A4, A4, B4,\
                      D4, D4, B4, A4, G4, A4, B4, B4, B4, B4, A4,\
                      A4, B4, A4, G4};
  u16 maryLength[] =  {QUARTER_NOTE,QUARTER_NOTE,QUARTER_NOTE,\
                       QUARTER_NOTE,QUARTER_NOTE,QUARTER_NOTE,\
                       HALF_NOTE,\
                       QUARTER_NOTE,QUARTER_NOTE,HALF_NOTE,\
                       QUARTER_NOTE,QUARTER_NOTE,HALF_NOTE,\
                       QUARTER_NOTE,QUARTER_NOTE,QUARTER_NOTE,\
                       QUARTER_NOTE,QUARTER_NOTE,QUARTER_NOTE,\
                       QUARTER_NOTE,QUARTER_NOTE,QUARTER_NOTE,\
                       QUARTER_NOTE,QUARTER_NOTE,QUARTER_NOTE,\
                       FULL_NOTE};  

  /* "Fur Elise" notes and their length */
  u32 fuerNotes[] = {B4, C4, D4, E4, G3, F4, E4, D4, F3, E4, D4,\
                     C4, E3, D4, C4, B4, NO,  E4, D4S,E4, D4S,E4,\
                     B4, D4, C4, A4, C3, E3, A4, B4, E3, G3S,B4,\
                     C4, NO,  E4, D4S,E4, D4S,E4, B4, D4, C4, A4,\
                     C3, E3, A4, B4, E3, C4, B4, A4};
  u16 fuerLength[] ={QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,\
      QUARTER_NOTE+HALF_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,\
      QUARTER_NOTE+HALF_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,\
      QUARTER_NOTE+HALF_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,\
      QUARTER_NOTE+HALF_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,\
      QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,\
      QUARTER_NOTE+HALF_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,\
      QUARTER_NOTE+HALF_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,\
      QUARTER_NOTE+HALF_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,\
      QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,\
      QUARTER_NOTE+HALF_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,\
      QUARTER_NOTE+HALF_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,\
      QUARTER_NOTE+HALF_NOTE};
  
  /* Super loop */  
  while(1)
  {
    WATCHDOG_BONE();
    
    /* Drivers */
    LedUpdate();
    G_ButtonStateMachine();
    G_MessagingStateMachine();
    G_UartStateMachine();
    G_DebugStateMachine();
    G_TWIStateMachine();
   
    /* Applications */
    G_LcdStateMachine();
    
    /* System sleep*/
    AT91C_BASE_PIOA->PIO_SODR = PA_31_HEARTBEAT;
    SystemSleep();
    AT91C_BASE_PIOA->PIO_CODR = PA_31_HEARTBEAT;
    
      //If the second button was pressed, play Mary had a little lamb
     if( WasButtonPressed(BUTTON1) )
    {
       ButtonAcknowledge(BUTTON1);
       LedOn(LCD_BLUE);
       playSong(maryNotes, maryLength,2, sizeof(maryNotes)/sizeof(maryNotes[0]));
    }
    
    //If the third button was pressed, play Fur Elise
    if( WasButtonPressed(BUTTON2) )
    {
       LedOn(LCD_RED);
       ButtonAcknowledge(BUTTON2);
       playSong(fuerNotes, fuerLength,2, sizeof(fuerNotes)/sizeof(fuerNotes[0]));
    }
  } /* end while(1) main super loop */
  
} /* end main() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/
