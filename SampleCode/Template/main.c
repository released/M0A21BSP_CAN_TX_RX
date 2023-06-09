/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

#include "misc_config.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

struct flag_32bit flag_PROJ_CTL;
#define FLAG_PROJ_TIMER_PERIOD_1000MS                 	(flag_PROJ_CTL.bit0)
#define FLAG_PROJ_TIMER_PERIOD_2000MS                   (flag_PROJ_CTL.bit1)
#define FLAG_PROJ_TIMER_PERIOD_4000MS                 	(flag_PROJ_CTL.bit2)
#define FLAG_PROJ_TIMER_PERIOD_8000MS                   (flag_PROJ_CTL.bit3)
#define FLAG_PROJ_REVERSE4                              (flag_PROJ_CTL.bit4)
#define FLAG_PROJ_REVERSE5                              (flag_PROJ_CTL.bit5)
#define FLAG_PROJ_REVERSE6                              (flag_PROJ_CTL.bit6)
#define FLAG_PROJ_REVERSE7                              (flag_PROJ_CTL.bit7)


/*_____ D E F I N I T I O N S ______________________________________________*/

volatile unsigned int counter_systick = 0;
volatile uint32_t counter_tick = 0;

#define CAN_MASK_MSG_DIR    (0x1ul << 30) /*!< CAN mask direction bit \hideinitializer */
#define CAN_MASK_EXT_ID_BIT (0x1ul << 31) /*!< CAN mask extended id bit \hideinitializer */


/**
 * @brief Specifies the standard identifier mask used for acceptance filtering
 *
 * @param[in] mask_bit The standard id mask bits.
 *
 * @return Mask ID bit.
 *
 *  \hideinitializer
 */
#define CAN_STD_ID_MASK(mask_bit) (mask_bit << 18)

/**
 * @brief Specifies the extended identifier mask used for acceptance filtering
 *
 * @param[in] mask_bit The extended id mask bits.
 *
 * @return Mask ID bit.
 *
*  \hideinitializer
 */
#define CAN_EXT_ID_MASK(mask_bit) (mask_bit)

#define CAN_DEVICE_1                                    ((uint32_t)0x601101) 
#define CAN_DEVICE_1_MsgID_r                            (MSG(0))
#define CAN_DEVICE_1_MsgID_t                            (MSG(31))

#define CAN_DEVICE_2                                    ((uint32_t)0x600) 
#define CAN_DEVICE_2_MsgID_r                            (MSG(1))
#define CAN_DEVICE_2_MsgID_t                            (MSG(30))

#define CAN_DEVICE_3                                    ((uint32_t)0x628) 
#define CAN_DEVICE_3_MsgID_r                            (MSG(1))    //similiar device ID , use RX mask to saving msg obj usage
#define CAN_DEVICE_3_MsgID_t                            (MSG(29))

#define CAN_DEVICE_4                                    ((uint32_t)0x7FFFE)
#define CAN_DEVICE_4_MsgID_r                            (MSG(2))
#define CAN_DEVICE_4_MsgID_t                            (MSG(28))

#define CAN_DEVICE_5                                    ((uint32_t)0x7FFF2) 
#define CAN_DEVICE_5_MsgID_r                            (MSG(2))    //similiar device ID , use RX mask to saving msg obj usage
#define CAN_DEVICE_5_MsgID_t                            (MSG(27))


typedef struct
{
    uint32_t  IdType;       
    uint32_t  FrameType;    
    uint32_t  Id;    
    uint32_t  rMsgID;    
    uint32_t  tMsgID;           
} CANMSG_T;

CANMSG_T deviceA;
CANMSG_T deviceB;
CANMSG_T deviceC;
CANMSG_T deviceD;
CANMSG_T deviceE;
/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

unsigned int get_systick(void)
{
	return (counter_systick);
}

void set_systick(unsigned int t)
{
	counter_systick = t;
}

void systick_counter(void)
{
	counter_systick++;
}

void SysTick_Handler(void)
{

    systick_counter();

    if (get_systick() >= 0xFFFFFFFF)
    {
        set_systick(0);      
    }

    // if ((get_systick() % 1000) == 0)
    // {
       
    // }

    #if defined (ENABLE_TICK_EVENT)
    TickCheckTickEvent();
    #endif    
}

void SysTick_delay(unsigned int delay)
{  
    
    unsigned int tickstart = get_systick(); 
    unsigned int wait = delay; 

    while((get_systick() - tickstart) < wait) 
    { 
    } 

}

void SysTick_enable(unsigned int ticks_per_second)
{
    set_systick(0);
    if (SysTick_Config(SystemCoreClock / ticks_per_second))
    {
        /* Setup SysTick Timer for 1 second interrupts  */
        printf("Set system tick error!!\n");
        while (1);
    }

    #if defined (ENABLE_TICK_EVENT)
    TickInitTickEvent();
    #endif
}

uint32_t get_tick(void)
{
	return (counter_tick);
}

void set_tick(uint32_t t)
{
	counter_tick = t;
}

void tick_counter(void)
{
	counter_tick++;
    if (get_tick() >= 60000)
    {
        set_tick(0);
    }
}


uint8_t CAN0_transmit5(void)
{
    uint8_t can_data[8] = {0};
    STR_CANMSG_T msg1;
    uint8_t i = 0;
    __IO static uint8_t cnt = 0;
    uint8_t dlc = 8;

    can_data[0] = 0x5E;
    can_data[1] = 0x5E;

    can_data[2] = cnt;

    can_data[6] = 0xE5;
    can_data[7] = 0xE5;

    msg1.FrameType= deviceE.FrameType;
    msg1.IdType   = deviceE.IdType;
    msg1.Id       = deviceE.Id;
    msg1.DLC      = dlc;

    for(i = 0 ; i < dlc; i++)
    {
        msg1.Data[i] = can_data[i];
    }
    
    if (CAN_Transmit(CAN, deviceE.tMsgID , &msg1) == FALSE)
    {
        printf("set tx msg failed\r\n");
        return 0;
    }

    cnt++;

    return 1;
}

uint8_t CAN0_transmit4(void)
{
    uint8_t can_data[8] = {0};
    STR_CANMSG_T msg1;
    uint8_t i = 0;
    __IO static uint8_t cnt = 0;
    uint8_t dlc = 8;

    can_data[0] = 0x5D;
    can_data[1] = 0x5D;

    can_data[2] = cnt;

    can_data[6] = 0xD5;
    can_data[7] = 0xD5;

    msg1.FrameType= deviceD.FrameType;
    msg1.IdType   = deviceD.IdType;
    msg1.Id       = deviceD.Id;
    msg1.DLC      = dlc;

    for(i = 0 ; i < dlc; i++)
    {
        msg1.Data[i] = can_data[i];
    }
    
    if (CAN_Transmit(CAN, deviceD.tMsgID , &msg1) == FALSE)
    {
        printf("set tx msg failed\r\n");
        return 0;
    }

    cnt++;

    return 1;
}

uint8_t CAN0_transmit3(void)
{
    uint8_t can_data[8] = {0};
    STR_CANMSG_T msg1;
    uint8_t i = 0;
    __IO static uint8_t cnt = 0;
    uint8_t dlc = 8;

    can_data[0] = 0x5C;
    can_data[1] = 0x5C;

    can_data[2] = cnt;

    can_data[6] = 0xC5;
    can_data[7] = 0xC5;

    msg1.FrameType= deviceC.FrameType;
    msg1.IdType   = deviceC.IdType;
    msg1.Id       = deviceC.Id;
    msg1.DLC      = dlc;

    for(i = 0 ; i < dlc; i++)
    {
        msg1.Data[i] = can_data[i];
    }
    
    if (CAN_Transmit(CAN, deviceC.tMsgID , &msg1) == FALSE)
    {
        printf("set tx msg failed\r\n");
        return 0;
    }

    cnt++;

    return 1;
}

uint8_t CAN0_transmit2(void)
{
    uint8_t can_data[8] = {0};
    STR_CANMSG_T msg1;
    uint8_t i = 0;
    __IO static uint8_t cnt = 0;
    uint8_t dlc = 8;

    can_data[0] = 0x5B;
    can_data[1] = 0x5B;

    can_data[2] = cnt;

    can_data[6] = 0xB5;
    can_data[7] = 0xB5;

    msg1.FrameType= deviceB.FrameType;
    msg1.IdType   = deviceB.IdType;
    msg1.Id       = deviceB.Id;
    msg1.DLC      = dlc;

    for(i = 0 ; i < dlc; i++)
    {
        msg1.Data[i] = can_data[i];
    }
    
    if (CAN_Transmit(CAN, deviceB.tMsgID , &msg1) == FALSE)
    {
        printf("set tx msg failed\r\n");
        return 0;
    }

    cnt++;

    return 1;
}

void CAN0_Sender(uint32_t can_id , uint8_t* can_data , uint8_t dlc)
{
    STR_CANMSG_T msg1;
    uint8_t i = 0;
 
    msg1.FrameType= deviceA.FrameType;
    msg1.IdType   = deviceA.IdType;
    msg1.Id       = can_id;
    msg1.DLC      = dlc;

    for(i = 0 ; i < dlc; i++)
    {
        msg1.Data[i] = can_data[i];
    }
    
    if (CAN_Transmit(CAN, deviceA.tMsgID , &msg1) == FALSE)
    {
        printf("set tx msg failed\r\n");
        return;
    }
}

uint8_t CAN0_transmit1(void)
{
    uint8_t can_data[8] = {0};

    __IO static uint8_t cnt = 0;

    can_data[0] = 0x5A;
    can_data[1] = 0x5A;

    can_data[2] = cnt;

    can_data[6] = 0xA5;
    can_data[7] = 0xA5;

    CAN0_Sender(deviceA.Id,can_data,8);

    cnt++;

    return 1;
}

void CAN0_StateMachine(uint8_t state)
{
	uint8_t DoneCheck = 0;
    printf("%s:%d\r\n" , __FUNCTION__ , state );

    switch(state)
    {
        case 1:
            while(!DoneCheck)
            {
                DoneCheck = CAN0_transmit1();
            }
            DoneCheck = 0;
            break;
        case 2:
            while(!DoneCheck)
            {
                DoneCheck = CAN0_transmit2();
            }
            DoneCheck = 0;
            break;
        case 3:
            while(!DoneCheck)
            {
                DoneCheck = CAN0_transmit3();
            }
            DoneCheck = 0;
            break;
        case 4:
            while(!DoneCheck)
            {
                DoneCheck = CAN0_transmit4();
            }
            DoneCheck = 0;
            break;
        case 5:
            while(!DoneCheck)
            {
                DoneCheck = CAN0_transmit5();
            }
            DoneCheck = 0;
            break;
    }
}

void CAN0_SetRxMsg(void)
{
    CAN_T *tCAN = CAN;

    if(CAN_SetRxMsg(tCAN, deviceA.rMsgID,deviceA.IdType, deviceA.Id) == FALSE)
    {
        printf("Set Rx Msg Object failed\n");
        return;
    }

    if(CAN_SetRxMsg(tCAN, deviceB.rMsgID,deviceB.IdType, deviceB.Id) == FALSE)
    {
        printf("Set Rx Msg Object failed\n");
        return;
    }

    #if 1   //similiar device ID , use RX mask to saving msg obj usage

    if( CAN_SetRxMsgAndMsk(tCAN, deviceC.rMsgID, deviceC.IdType, deviceC.Id, CAN_STD_ID_MASK(0x700) | CAN_MASK_MSG_DIR) == FALSE)
    {
        printf("Set Rx Msg Object failed\n");
        return;
    }

    #else
    if(CAN_SetRxMsg(tCAN, deviceC.rMsgID,deviceC.IdType, deviceC.Id) == FALSE)
    {
        printf("Set Rx Msg Object failed\n");
        return;
    }
    #endif

    if(CAN_SetRxMsg(tCAN, deviceD.rMsgID,deviceD.IdType, deviceD.Id) == FALSE)
    {
        printf("Set Rx Msg Object failed\n");
        return;
    }

    #if 1   //similiar device ID , use RX mask to saving msg obj usage

    if( CAN_SetRxMsgAndMsk(tCAN, deviceE.rMsgID, deviceE.IdType, deviceE.Id, CAN_EXT_ID_MASK(0x1FFFFFF0) | CAN_MASK_MSG_DIR | CAN_MASK_EXT_ID_BIT) == FALSE)
    {
        printf("Set Rx Msg Object failed\n");
        return;
    }
    
    #else   // use different msg obj num. 
    if(CAN_SetRxMsg(tCAN, deviceE.rMsgID,deviceE.IdType, deviceE.Id) == FALSE)
    {
        printf("Set Rx Msg Object failed\n");
        return;
    }
    #endif
}

void CAN_ShowMsg(STR_CANMSG_T* Msg)
{
    uint8_t i;

    printf("(aa)Read ID=0x%8X, Type=%s, DLC=%d,Data=",Msg->Id,Msg->IdType?"EXT":"STD",Msg->DLC);
    // printf("(bb)Read ID=0x%8X, Type=%s, DLC=%d,Data=",Msg->Id,Msg->IdType?"EXT":"STD",Msg->DLC);

    for(i=0; i<Msg->DLC; i++)
        printf("0x%02X,",Msg->Data[i]);
    printf("\r\n");
}

void CAN_MsgInterrupt(CAN_T *tCAN, uint32_t u32IIDR)
{
    #if 0

    switch(u32IIDR) {
        case 0x1:
        case 0x2:
        case 0x3:
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:
        case 0x8:
            //printf("Msg-<%d> INT and Callback\n", u32IIDR-1);
            // printf("\n\nMsg-<%d> Rx INT\n\n", u32IIDR-1);
            CAN_Receive(tCAN, u32IIDR-1, &rrMsg);
            CAN_ShowMsg(&rrMsg);
            break; //Rx

        case 0x9:
        case 0xA:
        case 0xB:
        case 0xC:
        case 0xD:
        case 0xE:
        case 0xF:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
            // printf("\n\nMsg-<%d> Tx INT\n\n", u32IIDR-1);
            CAN_CLR_INT_PENDING_BIT(tCAN, (u32IIDR -1));      /* Clear Interrupt Pending */
            break; //Tx

        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F:
        case 0x20:
        default:
            CAN_CLR_INT_PENDING_BIT(tCAN, (u32IIDR -1));      /* Clear Interrupt Pending */
            break; //No used
    }
    #else
    uint8_t msgID = u32IIDR-1;
    STR_CANMSG_T rrMsg;

    // printf("msgID : %d\r\n",msgID);
    if ((msgID == deviceA.rMsgID) ||
        (msgID == deviceB.rMsgID) ||
        (msgID == deviceC.rMsgID) ||
        (msgID == deviceD.rMsgID) ||
        (msgID == deviceE.rMsgID) )
    {
        printf("Msg-%d INT and Callback\r\n" , msgID);
        CAN_Receive(tCAN, MSG(msgID), &rrMsg);
        CAN_ShowMsg(&rrMsg);

        printf("\r\n");
    }

    // if(u32IIDR==0+1)
    // {
    //     printf("Msg-0 INT and Callback\n");
    //     CAN_Receive(tCAN, MSG(0), &rrMsg);
    //     CAN_ShowMsg(&rrMsg);
    // }

    // if(u32IIDR==5+1)
    // {
    //     printf("Msg-5 INT and Callback\n");
    //     CAN_Receive(tCAN, MSG(5), &rrMsg);
    //     CAN_ShowMsg(&rrMsg);
    // }

    // if(u32IIDR==8+1)
    // {
    //     printf("Msg-8 INT and Callback \n");
    //     CAN_Receive(tCAN, MSG(8), &rrMsg);
    //     CAN_ShowMsg(&rrMsg);
    // }
    
    // if(u32IIDR==14+1)
    // {
    //     printf("Msg-14 INT and Callback \n");
    //     CAN_Receive(tCAN, MSG(14), &rrMsg);
    //     CAN_ShowMsg(&rrMsg);
    // }

    // if(u32IIDR==15+1)
    // {
    //     printf("Msg-15 INT and Callback \n");
    //     CAN_Receive(tCAN, MSG(15), &rrMsg);
    //     CAN_ShowMsg(&rrMsg);
    // }

    #endif
}

void CAN0_IRQHandler(void)
{
    uint32_t u8IIDRstatus;

    u8IIDRstatus = CAN->IIDR;

    if(u8IIDRstatus == 0x00008000)        /* Check Status Interrupt Flag (Error status Int and Status change Int) */
    {
        /**************************/
        /* Status Change interrupt*/
        /**************************/
        if(CAN->STATUS & CAN_STATUS_RXOK_Msk)
        {
            CAN->STATUS &= ~CAN_STATUS_RXOK_Msk;   /* Clear Rx Ok status*/

            printf("RX OK INT\n") ;
        }

        if(CAN->STATUS & CAN_STATUS_TXOK_Msk)
        {
            CAN->STATUS &= ~CAN_STATUS_TXOK_Msk;    /* Clear Tx Ok status*/

            printf("TX OK INT\n") ;
        }

        /**************************/
        /* Error Status interrupt */
        /**************************/
        if(CAN->STATUS & CAN_STATUS_EWARN_Msk)
        {
            printf("EWARN INT\n") ;
        }

        if(CAN->STATUS & CAN_STATUS_BOFF_Msk)
        {
            printf("BOFF INT\n") ;

            /* Do Init to release busoff pin */
            CAN->CON = (CAN_CON_INIT_Msk | CAN_CON_CCE_Msk);
            CAN->CON &= (~(CAN_CON_INIT_Msk | CAN_CON_CCE_Msk));
            while(CAN->CON & CAN_CON_INIT_Msk);
        }
    }
    else if (u8IIDRstatus!=0)
    {
        printf("=> Interrupt Pointer = %d\n",CAN->IIDR -1);
 
        CAN_MsgInterrupt(CAN, u8IIDRstatus);

        CAN_CLR_INT_PENDING_BIT(CAN, ((CAN->IIDR) -1));  /* Clear Interrupt Pending */
    }
    else if(CAN->WU_STATUS == 1)
    {
        printf("Wake up\n");

        CAN->WU_STATUS = 0;                       /* Write '0' to clear */
    }

}

void CAN0_Device_Int(void)
{
    deviceA.FrameType   = CAN_DATA_FRAME;
    deviceA.IdType      = CAN_EXT_ID;
    deviceA.Id          = CAN_DEVICE_1;
    deviceA.rMsgID      = CAN_DEVICE_1_MsgID_r;
    deviceA.tMsgID      = CAN_DEVICE_1_MsgID_t;

    deviceB.FrameType   = CAN_DATA_FRAME;
    deviceB.IdType      = CAN_STD_ID;
    deviceB.Id          = CAN_DEVICE_2;
    deviceB.rMsgID      = CAN_DEVICE_2_MsgID_r;
    deviceB.tMsgID      = CAN_DEVICE_2_MsgID_t;
    
    deviceC.FrameType   = CAN_DATA_FRAME;
    deviceC.IdType      = CAN_STD_ID;
    deviceC.Id          = CAN_DEVICE_3;
    deviceC.rMsgID      = CAN_DEVICE_3_MsgID_r;
    deviceC.tMsgID      = CAN_DEVICE_3_MsgID_t;
    
    deviceD.FrameType   = CAN_DATA_FRAME;
    deviceD.IdType      = CAN_EXT_ID;
    deviceD.Id          = CAN_DEVICE_4;
    deviceD.rMsgID      = CAN_DEVICE_4_MsgID_r;
    deviceD.tMsgID      = CAN_DEVICE_4_MsgID_t;
    
    deviceE.FrameType   = CAN_DATA_FRAME;
    deviceE.IdType      = CAN_EXT_ID;
    deviceE.Id          = CAN_DEVICE_5;
    deviceE.rMsgID      = CAN_DEVICE_5_MsgID_r;
    deviceE.tMsgID      = CAN_DEVICE_5_MsgID_t;

}

void CAN0_Init(void)
{
    CAN_Open(CAN,  500000, CAN_NORMAL_MODE);

    #if 1
    CAN_EnterTestMode(CAN, CAN_TEST_SILENT_Msk);
    CAN_EnterTestMode(CAN, CAN_TEST_LBACK_Msk);
    #endif

    CAN_EnableInt(CAN, CAN_CON_IE_Msk|CAN_CON_SIE_Msk | CAN_CON_EIE_Msk);
    NVIC_SetPriority(CAN0_IRQn, (1<<__NVIC_PRIO_BITS) - 2);
    NVIC_EnableIRQ(CAN0_IRQn);    
    
    printf("%s (aa) \r\n",__FUNCTION__);
    // printf("%s (bb) \r\n",__FUNCTION__);
}


// void delay_ms(uint16_t ms)
// {
// 	TIMER_Delay(TIMER0, 1000*ms);
// }

void TMR1_IRQHandler(void)
{
	
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        TIMER_ClearIntFlag(TIMER1);
		tick_counter();

		if ((get_tick() % 1000) == 0)
		{
            FLAG_PROJ_TIMER_PERIOD_1000MS = 1;
		}

		if ((get_tick() % 2000) == 0)
		{
            FLAG_PROJ_TIMER_PERIOD_2000MS = 1;
		}

		if ((get_tick() % 4000) == 0)
		{
            FLAG_PROJ_TIMER_PERIOD_4000MS = 1;
		}

		if ((get_tick() % 8000) == 0)
		{
            FLAG_PROJ_TIMER_PERIOD_8000MS = 1;
		}

		if ((get_tick() % 50) == 0)
		{

		}	
    }
}

void TIMER1_Init(void)
{
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);	
    TIMER_Start(TIMER1);
}

void loop(void)
{
	// static uint32_t LOG1 = 0;
	// static uint32_t LOG2 = 0;
    static uint8_t cnt = 1;

    if ((get_systick() % 1000) == 0)
    {
        // printf("%s(systick) : %4d\r\n",__FUNCTION__,LOG2++);    
    }

    if (FLAG_PROJ_TIMER_PERIOD_1000MS)//(is_flag_set(flag_timer_period_1000ms))
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 0;//set_flag(flag_timer_period_1000ms ,DISABLE);

        // printf("%s(timer) : %4d\r\n",__FUNCTION__,LOG1++);
		PA2 ^= 1; 

        CAN0_StateMachine(cnt);

        cnt = (cnt >= 5) ? ( 1 ) : ( cnt + 1) ;
    }
}

void UARTx_Process(void)
{
	uint8_t res = 0;
	res = UART_READ(UART0);

	if (res > 0x7F)
	{
		printf("invalid command\r\n");
	}
	else
	{
		printf("press : %c\r\n" , res);
		switch(res)
		{
			case '1':
				break;

			case 'X':
			case 'x':
			case 'Z':
			case 'z':
                SYS_UnlockReg();
				// NVIC_SystemReset();	// Reset I/O and peripherals , only check BS(FMC_ISPCTL[1])
                // SYS_ResetCPU();     // Not reset I/O and peripherals
                SYS_ResetChip();    // Reset I/O and peripherals ,  BS(FMC_ISPCTL[1]) reload from CONFIG setting (CBS)	
				break;
		}
	}
}

void UART0_IRQHandler(void)
{

    if(UART_GET_INT_FLAG(UART0, UART_INTSTS_RDAINT_Msk | UART_INTSTS_RXTOINT_Msk))     /* UART receive data available flag */
    {
        while(UART_GET_RX_EMPTY(UART0) == 0)
        {
            UARTx_Process();
        }
    }

    if(UART0->FIFOSTS & (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk))
    {
        UART_ClearIntFlag(UART0, (UART_INTSTS_RLSINT_Msk| UART_INTSTS_BUFERRINT_Msk));
    }	
}

void UART0_Init(void)
{
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
    UART_EnableInt(UART0, UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk);
    NVIC_EnableIRQ(UART0_IRQn);
	
	#if (_debug_log_UART_ == 1)	//debug
	printf("\r\nCLK_GetCPUFreq : %8d\r\n",CLK_GetCPUFreq());
	printf("CLK_GetHCLKFreq : %8d\r\n",CLK_GetHCLKFreq());
	printf("CLK_GetHXTFreq : %8d\r\n",CLK_GetHXTFreq());
	printf("CLK_GetLXTFreq : %8d\r\n",CLK_GetLXTFreq());	
	printf("CLK_GetPCLK0Freq : %8d\r\n",CLK_GetPCLK0Freq());
	printf("CLK_GetPCLK1Freq : %8d\r\n",CLK_GetPCLK1Freq());	
	#endif	

    #if 0
    printf("FLAG_PROJ_TIMER_PERIOD_1000MS : 0x%2X\r\n",FLAG_PROJ_TIMER_PERIOD_1000MS);
    printf("FLAG_PROJ_REVERSE1 : 0x%2X\r\n",FLAG_PROJ_REVERSE1);
    printf("FLAG_PROJ_REVERSE2 : 0x%2X\r\n",FLAG_PROJ_REVERSE2);
    printf("FLAG_PROJ_REVERSE3 : 0x%2X\r\n",FLAG_PROJ_REVERSE3);
    printf("FLAG_PROJ_REVERSE4 : 0x%2X\r\n",FLAG_PROJ_REVERSE4);
    printf("FLAG_PROJ_REVERSE5 : 0x%2X\r\n",FLAG_PROJ_REVERSE5);
    printf("FLAG_PROJ_REVERSE6 : 0x%2X\r\n",FLAG_PROJ_REVERSE6);
    printf("FLAG_PROJ_REVERSE7 : 0x%2X\r\n",FLAG_PROJ_REVERSE7);
    #endif

}

void GPIO_Init (void)
{
    SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~(SYS_GPA_MFP0_PA2MFP_Msk)) | (SYS_GPA_MFP0_PA2MFP_GPIO);
    SYS->GPC_MFP0 = (SYS->GPC_MFP0 & ~(SYS_GPC_MFP0_PC2MFP_Msk)) | (SYS_GPC_MFP0_PC2MFP_GPIO);
		
	// EVM LED_R
    GPIO_SetMode(PA, BIT2, GPIO_MODE_OUTPUT);

	// EVM button
    GPIO_SetMode(PC, BIT2, GPIO_MODE_OUTPUT);	
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

//    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

//	CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);	

//	CLK_EnableXtalRC(CLK_PWRCTL_LXTEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);	

    /* Select HCLK clock source as HIRC and HCLK source divider as 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    CLK_EnableModuleClock(TMR0_MODULE);
  	CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, 0);

    CLK_EnableModuleClock(TMR1_MODULE);
  	CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HIRC, 0);

    CLK_EnableModuleClock(CAN0_MODULE);

    /* Set PB multi-function pins for UART0 RXD=PB.6 and TXD=PB.4 */
    SYS->GPB_MFP1 = (SYS->GPB_MFP1 & ~(SYS_GPB_MFP1_PB4MFP_Msk | SYS_GPB_MFP1_PB6MFP_Msk)) |        \
                    (SYS_GPB_MFP1_PB4MFP_UART0_TXD | SYS_GPB_MFP1_PB6MFP_UART0_RXD);

    SYS->GPC_MFP0 = (SYS->GPC_MFP0 & ~(SYS_GPC_MFP0_PC0MFP_Msk | SYS_GPC_MFP0_PC1MFP_Msk)) |
                    (SYS_GPC_MFP0_PC0MFP_CAN0_RXD | SYS_GPC_MFP0_PC1MFP_CAN0_TXD);


   /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Lock protected registers */
    SYS_LockReg();
}

int main()
{
    SYS_Init();

	GPIO_Init();
	UART0_Init();
	TIMER1_Init();

    SysTick_enable(1000);
    #if defined (ENABLE_TICK_EVENT)
    TickSetTickEvent(1000, TickCallback_processA);  // 1000 ms
    TickSetTickEvent(5000, TickCallback_processB);  // 5000 ms
    #endif

	CAN0_Init();
    CAN0_Device_Int();
    CAN0_SetRxMsg();


    /* Got no where to go, just loop forever */
    while(1)
    {
        loop();

    }
}

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
