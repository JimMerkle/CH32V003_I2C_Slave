/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/08
 * Description        : Main program body.
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/*
 *@Note
  SDA:  PC1
  SCL:  PC2

*/

#include "debug.h"
#include "queue.h"
#include "ch32v00x_i2c.h"
#include "ch32v00x_rcc.h"
#include "ch32v00x_gpio.h"

/* I2C Mode Definition */
#define HOST_MODE   0
#define SLAVE_MODE   1

/* I2C Communication Mode Selection */
//#define I2C_MODE   HOST_MODE
#define I2C_MODE   SLAVE_MODE

/* Global define */
#define Size   7
#define RXAddress   0x12
//#define TxAddress   0x02

/* Global Variable */
//u8 TxData[Size] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
u8 RxData[Size];

/*********************************************************************
 * @fn      GPIO_Blink_Init
 *
 * @brief   Initializes GPIO D6 for LED blinking
 *
 * @return  none
 */
void GPIO_Blink_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      IIC_Init
 *
 * @brief   Initializes the IIC peripheral.
 *
 * @return  none
 */
void IIC_Init(u32 bound, u16 address)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    I2C_InitTypeDef I2C_InitTSturcture={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1, ENABLE );

    // Configure PC2 for SCL
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    // Configure PC1 for SDA
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    I2C_InitTSturcture.I2C_ClockSpeed = bound;
    I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
    I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2C_InitTSturcture.I2C_OwnAddress1 = address;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init( I2C1, &I2C_InitTSturcture );

    I2C_Cmd( I2C1, ENABLE );

#if (I2C_MODE == HOST_MODE)
    I2C_AcknowledgeConfig( I2C1, ENABLE );

#endif
}

// 200ms ON, 200ms OFF
void short_blink(void)
{
    GPIO_WriteBit(GPIOD,GPIO_Pin_6,Bit_SET);
    Delay_Ms(200);
    GPIO_WriteBit(GPIOD,GPIO_Pin_6,Bit_RESET);
    Delay_Ms(200);
}

typedef struct {
    uint32_t time;
    uint32_t event;
    uint8_t  data;
} TIME_EVENT;

#define MAX_EVENTS  100
TIME_EVENT table[MAX_EVENTS];

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 *
 * Notes: During the do-while loop, we send 0xAB just fine, then we send 0x25 ?
 */
int main(void)
{
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n",SystemCoreClock);

    // Continuously monitor (poll) the I2C status registers,
    //   watching as a byte is received (as a slave), what bits become set as
    //   events / internal states change.
    printf("I2C Slave mode\r\n");

    // Attempting to force a software reset doesn't appear to this a more content slave device
    //I2C1->CTLR1 = 0x8000; // Software Reset
    //I2C1->CTLR1 = 0;

    IIC_Init( 10000, RXAddress<<1); // 100Kbps
    I2C1->CTLR1 |= 0x0080; // CTLR1_NOSTRETCH_Set - Disable clock stretching
    I2C1->CTLR1 |= 0x0400; // CTLR1_ACK_Set - Enable ACK following each byte received - This also stops the clock stretching for each character received.

    // Use LED to indicate Starting....
    GPIO_Blink_Init();
    // Short Blink, Short Blink, Go....
    short_blink();
    short_blink();

    // For 10 seconds, record changes (with time-stamp)
    // Reset and start SysTick timer running (normally disabled) - see debug.c
    SysTick->CTLR &= ~1;  // force SysTick stopped
    SysTick->CNT = 0;     // Clear counter/timer
    SysTick->CTLR |= 1;   // Start timer/counter

    uint32_t previousevent = 0;
    unsigned count =0;
    do {
        uint32_t lastevent = ((I2C1->STAR2 << 16) | I2C1->STAR1) & 0x00FFFFFF; // Combine two 16-bit status registers into a 32-bit value
        if(previousevent != lastevent) {
            // event flags changed
            table[count].time = SysTick->CNT;
            table[count].event = lastevent;
            previousevent = lastevent;
            // If Master writes a byte of data to us...
            //if(lastevent & 0x00000004) // is BTF (0x00000004) flag set? - Wrong flag to use.  This one is set when 2nd byte needs to transfer to DATAR.
            if(lastevent & 0x00000040) // is RXNE (0x00000040 flag set?
                table[count].data = I2C1->DATAR; // This clears the BTF/RXNE flag(s)
            else table[count].data = 0xFF;
            // The following clears the STOPF, but prevents receiving additional transfers
            //if(lastevent & 0x10) // is STOPF set?
            //    I2C1->CTLR1 |= 0x200; // Set Bit 9 STOP. Release the SCL and SDA lines after the current byte transfer.
            // If Master attemps to read a byte of data from us...
            if(lastevent & 0x80) // is TXE set?
                I2C1->DATAR = 0xAB; // If master tries to read from us, send constant 0xAB
            // The AF bit, once set, should be cleared so we can see if/when it gets set again
            if(lastevent & 0x400) // is AF set?
                I2C1->STAR1 &= ~0x400; // Write zero to AF bit

            count++;
        }
    } while(SysTick->CNT < 60000000 && count < MAX_EVENTS); // 4-seconds of 6 ticks / us
    SysTick->CTLR &= ~1;  // force SysTick stopped

    // Generate report - We have all the time in the world.  Provide annotations
    printf("Indx  Time   Event bits  Data   Annotations\r\n");

    for(unsigned i=0;i<count;i++){
        int8_t buf[120] = {0};
        uint8_t indx =0;
        printf("%03u  %07u  0x%06X   ",i,(table[i].time + 3) / 6,table[i].event,table[i].data); // round time to closest us

        if(table[i].data != 0xFF) {
            printf("0x%02X   ",table[i].data); // show data
        }
        else {
            printf("----   "); // don't show data
        }
        // Add annotations
        if(table[i].event & 0x040000) indx += sprintf(&buf[indx],"TRA ");
        if(table[i].event & 0x020000) indx += sprintf(&buf[indx],"BUSY ");
        if(table[i].event & 0x400) indx += sprintf(&buf[indx],"AF ");
        if(table[i].event & 0x80) indx += sprintf(&buf[indx],"TXE ");
        if(table[i].event & 0x40) indx += sprintf(&buf[indx],"RXNE ");
        if(table[i].event & 0x10) indx += sprintf(&buf[indx],"STOPF ");
        if(table[i].event & 0x04) indx += sprintf(&buf[indx],"BTF ");
        if(table[i].event & 0x02) indx += sprintf(&buf[indx],"ADDR ");
        printf("%s\r\n",buf); // New line
    }
    //printf("Data Byte: 0x%02X\r\n",(uint8_t)I2C1->DATAR);

#if 0
    extern QUEUE_ITEM data_queue;
    queue_init(&data_queue);

    // Test the data queues
    queue_fill(&data_queue);
    queue_remove_all(&data_queue);

    while( !I2C_CheckEvent( I2C1, I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED ) );
    int i=0;

    while( i<6 )
    {
        if( I2C_GetFlagStatus( I2C1, I2C_FLAG_RXNE ) !=  RESET )
        {
            RxData[i] = I2C_ReceiveData( I2C1 );
            i++;
        }
    }

    printf( "RxData:\r\n" );
    for( i=0; i<6; i++ )
    {
        printf( "%02x\r\n", RxData[i] );
    }
#endif



    while(1);
    return 0;
}
