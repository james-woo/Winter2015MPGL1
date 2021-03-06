/***********************************************************************************************************************
File: buttons.h                                                                
***********************************************************************************************************************/

#ifndef __BUTTONS_H
#define __BUTTONS_H

#include "configuration.h"

/***********************************************************************************************************************
Type Definitions
***********************************************************************************************************************/
typedef enum {RELEASED, PRESSED} ButtonStateType; 
typedef enum {BUTTON_PORTA = 0, BUTTON_PORTB = 0x80} ButtonPortType;  /* Offset between port registers (in 32 bit words) */
typedef enum {BUTTON_ACTIVE_LOW = 0, BUTTON_ACTIVE_HIGH = 1} ButtonActiveType;

typedef struct 
{
  ButtonActiveType eActiveState;
  ButtonPortType ePort;
}ButtonConfigType;


/***********************************************************************************************************************
Constants / Definitions
***********************************************************************************************************************/
#define BUTTON_INIT_MSG_TIMEOUT         (u32)1000     /* Time in ms for init message to send */
#define BUTTON_DEBOUNCE_TIME            (u32)25       /* Time in ms for button debouncing */


/***********************************************************************************************************************
Function Declarations
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/
bool IsButtonPressed(u32 u32Button_);
bool WasButtonPressed(u32 u32Button_);
void ButtonAcknowledge(u32 u32Button_);
bool IsButtonHeld(u32 u32Button_, u32 u32ButtonHeldTime_);

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
void ButtonInitialize(void);                        
u32 GetButtonBitLocation(u8 u8Button_, ButtonPortType ePort_);

/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void ButtonSM_Idle(void);                
static void ButtonSM_ButtonActive(void);        


#endif /* __BUTTONS_H */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
