# M0A21BSP_CAN_TX_RX
 M0A21BSP_CAN_TX_RX


update @ 2023/04/24

1. use PC.0 as CAN RX , PC.1 as CAN TX to test CAN bus function 

2. enable loop back and silent mode for test TX / RX

    CAN_EnterTestMode(CAN1, CAN_TEST_SILENT_Msk);
    
	CAN_EnterTestMode(CAN1, CAN_TEST_LBACK_Msk); 

3. below is log capture 

![image](https://github.com/released/M0A21BSP_CAN_TX_RX/blob/main/log.jpg)		
	
