#include "configuration.h"

extern volatile u32 G_u32SystemTime1ms;

volatile fnCode_type G_LcdStateMachine;

u8 Test_Buffer[20];
u32 u32Timer;
u32 u32Return;
TWIConfigurationType Request;
TWIPeripheralType* Control;

void TestInitialize(void)
{
  AT91C_BASE_PIOB->PIO_CODR = PB_09_LCD_RST;
  G_TestStateMachine = TestSM_Idle;
}

void TestSM_Idle(void)
{
  

  
  static u8 u8Bar = 0;
  static u8 au8Data1[] = { 0x00 };
  static u8 au8Data2[] = { 0x38, 0x39, 0x14, 0x72, 0x5E, 0x6D };
  static u8 au8Data3[] = { 0x0F };
  static u8 au8Data4[] = { 0x40, 0x48, 0x65, 0x6C, 0x6C, 0x6F };
  
  if(u8Bar == 0)
  {  
    
    AT91C_BASE_PIOB->PIO_SODR |= PB_09_LCD_RST;
     
    Request.TWIPeripheral = TWI0;
    Request.u8DeviceAddress = 0x3C;
    Request.pu8RxBufferAddress = &Test_Buffer[0];
    Request.u32RxBufferSize = 20;
    Request.pu8RxNextByte = &Request.pu8RxBufferAddress;
    
    Control = TWIRequest(&Request);
    u32Timer = G_u32SystemTime1ms;
    u8Bar++;
  }
  
  if( (u8Bar == 1) && IsTimeUp(&u32Timer, 50) )
  {
    u32Return = TWIWriteData(Control, 1, au8Data1);
    u32Timer = G_u32SystemTime1ms;
    u8Bar++;
  }
  
  if( (u8Bar == 2) && /*IsTimeUp(&u32Timer, 1)*/ )
  {
    u32Return = TWIWriteData(Control, 1, &au8Data2[0]);
    u32Timer = G_u32SystemTime1ms;
    u8Bar++;
  }
  
  if( (u8Bar == 3) && /*IsTimeUp(&u32Timer, 1)*/ )
  {
    u32Return = TWIWriteData(Control, 1, &au8Data2[1]);
    u32Timer = G_u32SystemTime1ms;
    u8Bar++;
  }
  
  if( (u8Bar == 4) && /*IsTimeUp(&u32Timer, 1)*/ )
  {
    u32Return = TWIWriteData(Control, 1, &au8Data2[2]);
    u32Timer = G_u32SystemTime1ms;
    u8Bar++;
  }
  
  if( (u8Bar == 5) && IsTimeUp(&u32Timer, 1) )
  {
    u32Return = TWIWriteData(Control, 1, &au8Data2[3]);
    u32Timer = G_u32SystemTime1ms;
    u8Bar++;
  }
  
  if( (u8Bar == 6) && IsTimeUp(&u32Timer, 1) )
  {
    u32Return = TWIWriteData(Control, 1, &au8Data2[4]);
    u32Timer = G_u32SystemTime1ms;
    u8Bar++;
  }
  
  if( (u8Bar == 7) && IsTimeUp(&u32Timer, 1) )
  {
    u32Return = TWIWriteData(Control, 1, &au8Data2[5]);
    u32Timer = G_u32SystemTime1ms;
    u8Bar++;
  }
  
  if( (u8Bar == 8) && IsTimeUp(&u32Timer, 200))
  {
    u32Return = TWIWriteData(Control, 1, au8Data3 );
    u32Timer = G_u32SystemTime1ms;
    u8Bar++;
  }
       
  if( (u8Bar == 9) && IsTimeUp(&u32Timer, 1))
  {
    SendStop(Control);
    u32Timer = G_u32SystemTime1ms;
    u8Bar++;
  }
       
  if( (u8Bar == 10) && IsTimeUp(&u32Timer, 1))
  {
    u32Return = TWIWriteData(Control, 1, au8Data4 );
    u32Timer = G_u32SystemTime1ms;
    u8Bar++;
  }
  
  if( (u8Bar == 11) && IsTimeUp(&u32Timer, 1000))
  {
    SendStop(Control);
    u32Timer = G_u32SystemTime1ms;
    u8Bar++;
  }
}