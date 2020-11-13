/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"
#include <osdep_api.h>
#include "hal_i2c.h"

//---------------------------------------------------------------------------------------------------
//External functions
//---------------------------------------------------------------------------------------------------
extern HAL_TIMER_OP HalTimerOp;

#define I2C_STATIC_ALLOC      1
/* I2C SAL global variables declaration when kernel disabled */
#ifdef I2C_STATIC_ALLOC
    HAL_I2C_OP HalI2COpSAL;
#endif
 
#if I2C0_USED   /*#if I2C0_USED*/    
#ifdef I2C_STATIC_ALLOC  
    SAL_I2C_MNGT_ADPT   SalI2C0MngtAdpt;
        
    SAL_I2C_HND_PRIV    SalI2C0HndPriv;
    
    HAL_I2C_INIT_DAT    HalI2C0InitData;
   
    IRQ_HANDLE          I2C0IrqHandleDat;
    
    HAL_GDMA_ADAPTER    HalI2C0TxGdmaAdpt;
   
    HAL_GDMA_ADAPTER    HalI2C0RxGdmaAdpt;
     
    HAL_GDMA_OP         HalI2C0GdmaOp;

    IRQ_HANDLE          I2C0TxGdmaIrqHandleDat;
    
    IRQ_HANDLE          I2C0RxGdmaIrqHandleDat;
        
    SAL_I2C_USER_CB     SalI2C0UserCB;
   
    SAL_I2C_USERCB_ADPT SalI2C0UserCBAdpt[SAL_USER_CB_NUM];
    
    SAL_I2C_DMA_USER_DEF    SalI2C0DmaUserDef;
#endif    
#endif          /*#if I2C0_USED*/
 
#if I2C1_USED   /*#if I2C1_USED*/
#ifdef I2C_STATIC_ALLOC   
    SAL_I2C_MNGT_ADPT   SalI2C1MngtAdpt;
       
    SAL_I2C_HND_PRIV    SalI2C1HndPriv;
   
    HAL_I2C_INIT_DAT    HalI2C1InitData;
  
    IRQ_HANDLE          I2C1IrqHandleDat;
 
    HAL_GDMA_ADAPTER    HalI2C1TxGdmaAdpt;

    HAL_GDMA_ADAPTER    HalI2C1RxGdmaAdpt;
    
    HAL_GDMA_OP         HalI2C1GdmaOp;

    IRQ_HANDLE          I2C1TxGdmaIrqHandleDat;

    IRQ_HANDLE          I2C1RxGdmaIrqHandleDat;

    SAL_I2C_USER_CB     SalI2C1UserCB;

    SAL_I2C_USERCB_ADPT SalI2C1UserCBAdpt[SAL_USER_CB_NUM];

    SAL_I2C_DMA_USER_DEF    SalI2C1DmaUserDef;
#endif
#endif          /*#if I2C1_USED*/
 
#if I2C2_USED   /*#if I2C2_USED*/    
#ifdef I2C_STATIC_ALLOC

    SAL_I2C_MNGT_ADPT   SalI2C2MngtAdpt;
    
    SAL_I2C_HND_PRIV    SalI2C2HndPriv;

    HAL_I2C_INIT_DAT    HalI2C2InitData;

    IRQ_HANDLE          I2C2IrqHandleDat;

    HAL_GDMA_ADAPTER    HalI2C2TxGdmaAdpt;
    
    HAL_GDMA_ADAPTER    HalI2C2RxGdmaAdpt;
    
    HAL_GDMA_OP         HalI2C2GdmaOp;

    IRQ_HANDLE          I2C2TxGdmaIrqHandleDat;

    IRQ_HANDLE          I2C2RxGdmaIrqHandleDat;

    SAL_I2C_USER_CB     SalI2C2UserCB;

    SAL_I2C_USERCB_ADPT SalI2C2UserCBAdpt[SAL_USER_CB_NUM];

    SAL_I2C_DMA_USER_DEF    SalI2C2DmaUserDef;
#endif    
#endif          /*#if I2C2_USED*/
 
#if I2C3_USED   /*#if I2C3_USED*/    
#ifdef I2C_STATIC_ALLOC

    SAL_I2C_MNGT_ADPT   SalI2C3MngtAdpt;

    SAL_I2C_HND_PRIV    SalI2C3HndPriv;
 
    HAL_I2C_INIT_DAT    HalI2C3InitData;

    IRQ_HANDLE          I2C3IrqHandleDat;

    HAL_GDMA_ADAPTER    HalI2C3TxGdmaAdpt;

    HAL_GDMA_ADAPTER    HalI2C3RxGdmaAdpt;
    
    HAL_GDMA_OP         HalI2C3GdmaOp;

    IRQ_HANDLE          I2C3TxGdmaIrqHandleDat;

    IRQ_HANDLE          I2C3RxGdmaIrqHandleDat;

    SAL_I2C_USER_CB     SalI2C3UserCB;

    SAL_I2C_USERCB_ADPT SalI2C3UserCBAdpt[SAL_USER_CB_NUM];

    SAL_I2C_DMA_USER_DEF    SalI2C3DmaUserDef;
#endif    
#endif          /*#if I2C3_USED*/

/* Used only for A~C Version */
#ifndef CONFIG_CHIP_E_CUT



VOID 
HalI2COpInit_Patch(
    IN  VOID *Data
)
{
    PHAL_I2C_OP pHalI2COp = (PHAL_I2C_OP) Data;

    pHalI2COp->HalI2CInit       =   HalI2CInit8195a_Patch;
    DBG_I2C_INFO("HalOpInit->HalI2CInit:%x\n",pHalI2COp->HalI2CInit);

    pHalI2COp->HalI2CDeInit     =   HalI2CDeInit8195a;
    DBG_I2C_INFO("HalOpInit->HalI2CDeInit:%x\n",pHalI2COp->HalI2CDeInit);

    pHalI2COp->HalI2CSend       =   HalI2CSendRtl8195a_Patch;
    DBG_I2C_INFO("HalOpInit->HalI2CSend:%x\n",pHalI2COp->HalI2CSend);
    
    pHalI2COp->HalI2CReceive    =   HalI2CReceiveRtl8195a;    
    DBG_I2C_INFO("HalOpInit->HalI2CReceive:%x\n",pHalI2COp->HalI2CReceive);
    
    pHalI2COp->HalI2CEnable     =   HalI2CEnableRtl8195a_Patch;    
    DBG_I2C_INFO("HalOpInit->HalI2CEnable:%x\n",pHalI2COp->HalI2CEnable);
    
    pHalI2COp->HalI2CIntrCtrl   =   HalI2CIntrCtrl8195a; 
    DBG_I2C_INFO("HalOpInit->HalI2CIntrCtrl:%x\n",pHalI2COp->HalI2CIntrCtrl);
    
    pHalI2COp->HalI2CReadReg    =   HalI2CReadRegRtl8195a;
    DBG_I2C_INFO("HalOpInit->HalI2CReadReg:%x\n",pHalI2COp->HalI2CReadReg);

    pHalI2COp->HalI2CWriteReg    =   HalI2CWriteRegRtl8195a;
    DBG_I2C_INFO("pHalI2COp->HalI2CWriteReg:%x\n",pHalI2COp->HalI2CWriteReg);

    pHalI2COp->HalI2CSetCLK     =   HalI2CSetCLKRtl8195a_Patch;
    DBG_I2C_INFO("HalOpInit->HalI2CSetCLK:%x\n",pHalI2COp->HalI2CSetCLK);
    
    pHalI2COp->HalI2CMassSend   =   HalI2CMassSendRtl8195a_Patch;
    DBG_I2C_INFO("HalOpInit->HalI2CMassSend:%x\n",pHalI2COp->HalI2CMassSend);
    
    pHalI2COp->HalI2CClrIntr    =   HalI2CClrIntrRtl8195a;
    DBG_I2C_INFO("HalOpInit->HalI2CClrIntr:%x\n",pHalI2COp->HalI2CClrIntr);
    
    pHalI2COp->HalI2CClrAllIntr =   HalI2CClrAllIntrRtl8195a;
    DBG_I2C_INFO("HalOpInit->HalI2CClrAllIntr:%x\n",pHalI2COp->HalI2CClrAllIntr);
    
    pHalI2COp->HalI2CDMACtrl    =   HalI2CDMACtrl8195a;
    DBG_I2C_INFO("HalOpInit->HalI2CDMACtrl:%x\n",pHalI2COp->HalI2CDMACtrl);
}


//---------------------------------------------------------------------------------------------------
//Function Name:
//		I2CISRHandle
//
// Description:
//         I2C Interrupt Service Routine.
//		According to the input pointer to SAL_I2C_HND, all the rest pointers will be 
//         found and be used to the rest part of this servie routine.
//         The following types of interrupt will be taken care:
//              - General Call  (providing General Call Callback). Slave receives a general call.
//              - STOP Bit       (NOT providing General Call Callback)
//              - START Bit     (NOTproviding General Call Callback)
//              - I2C Activity   (NOTproviding General Call Callback)
//              - RX Done       (providing Error Callback). The slave transmitter does NOT
//                                     receive a proper NACK for the end of whole transfer.
//              - TX Abort       (providing Error Call Callback). The Master/Slave 
//                                    transmitting is terminated.
//              - RD Req         (providing TX and TXC Callback). Slave gets a Read Request
//                                    and starts a slave-transmitter operation. The slave transmit
//                                    data will be written into slave TX FIFO from user data buffer.
//              - TX Empty      (providing TX and TXC Callback). Master TX FIFO is empty.
//                                    The user transmit data will be written into master TX FIFO
//                                    from user data buffer.
//              - TX Over        (providing Error Callback). Master TX FIFO is Overflow.
//              - RX Full         (providing RX and RXC Callback). Master/Slave RX FIFO contains
//                                    data. And the received data will be put into Master/Slave user
//                                    receive data buffer.
//              - RX Over        (providing Error Callback). Master/Slave RX FIFO is Overflow.
//              - RX Under      (providing Error Callback). Master/Slave RX FIFO is Underflow.
//
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		NA
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-02.
//
//----------------------------------------------------------------------------------------------------
VOID
I2CISRHandle_Patch(
    IN  VOID *Data
){
    PSAL_I2C_HND        pSalI2CHND          = (PSAL_I2C_HND) Data;
    PSAL_I2C_HND_PRIV   pSalI2CHNDPriv      = NULL;
    PSAL_I2C_MNGT_ADPT  pSalI2CMngtAdpt     = NULL;
    PHAL_I2C_INIT_DAT   pHalI2CInitDat      = NULL;
    PHAL_I2C_OP         pHalI2COP           = NULL;    
    PSAL_I2C_USER_CB    pSalI2CUserCB       = NULL;
    u32 I2CLocalTemp    = 0;
    u32 I2CInTOTcnt     = 0;
    u32 I2CIrqIdx       = 0;
    u32 InTimeoutCount  = 0;
    u32 InStartCount    = 0;
    volatile u32 I2CLocalRawSts  = 0;
    u32 I2CStsTmp       = 0;
   
    /* To get the SAL_I2C_MNGT_ADPT pointer, and parse the rest pointers */
    pSalI2CHNDPriv  = CONTAINER_OF(pSalI2CHND, SAL_I2C_HND_PRIV, SalI2CHndPriv);
    pSalI2CMngtAdpt = CONTAINER_OF(pSalI2CHNDPriv->ppSalI2CHnd, SAL_I2C_MNGT_ADPT, pSalHndPriv);
    pHalI2CInitDat  = pSalI2CMngtAdpt->pHalInitDat;
    pHalI2COP       = pSalI2CMngtAdpt->pHalOp;
    I2CInTOTcnt     = pSalI2CMngtAdpt->InnerTimeOut;
    I2CIrqIdx       = pHalI2CInitDat->I2CIdx;
    pSalI2CUserCB   = pSalI2CHND->pUserCB;

    /* I2C General Call Intr*/
    if (pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_INTR_STAT) &
                                BIT_CTRL_IC_INTR_STAT_R_GEN_CALL(1)) {

        DBG_I2C_WARN("I2C%d INTR_GEN_CALL\n",I2CIrqIdx);

        /* Clear I2C interrupt */
        pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_GEN_CALL;
        pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);

        /* Invoke I2C General Call callback if available*/
        if (pSalI2CUserCB->pGENCALLCB->USERCB != NULL) {
            pSalI2CUserCB->pGENCALLCB->USERCB((void *)pSalI2CUserCB->pGENCALLCB->USERData);
        }            
    }

    /* I2C START DET Intr */
    if (pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_INTR_STAT) &
                                BIT_CTRL_IC_INTR_STAT_R_START_DET(1)) {

        DBG_I2C_WARN("I2C%d INTR_START_DET\n",I2CIrqIdx);

        /* Clear I2C interrupt */
        pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_START_DET;
        pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);
    }

    /* I2C STOP DET Intr */
    if (pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_INTR_STAT) &
                                BIT_CTRL_IC_INTR_STAT_R_STOP_DET(1)) {

        DBG_I2C_WARN("I2C%d INTR_STOP_DET\n",I2CIrqIdx);

        /* Clear I2C interrupt */
        pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_STOP_DET;
        pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);
    }

    /* I2C Activity Intr */
    if (pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_INTR_STAT) &
                                BIT_CTRL_IC_INTR_STAT_R_ACTIVITY(1)) {

        DBG_I2C_WARN("I2C%d INTR_ACTIVITY\n",I2CIrqIdx);

        /* Clear I2C interrupt */
        pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_ACTIVITY;
        pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);
    }
    
    /* I2C RX Done Intr */
    if (pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_INTR_STAT) &
                                BIT_CTRL_IC_INTR_STAT_R_RX_DONE(1)) {
        if ((pSalI2CHND->DevSts == I2C_STS_TX_READY) || (pSalI2CHND->DevSts == I2C_STS_TX_ING)) {
            /* Disable I2C TX Related Interrupts */
            I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_INTR_MASK);
            I2CLocalTemp &= ~(BIT_IC_INTR_MASK_M_TX_ABRT |
                              BIT_IC_INTR_MASK_M_TX_OVER |
                              BIT_IC_INTR_MASK_M_RX_DONE |
                              BIT_IC_INTR_MASK_M_RD_REQ);
            pHalI2CInitDat->I2CIntrMSK = I2CLocalTemp;
            pHalI2COP->HalI2CIntrCtrl(pHalI2CInitDat);
            pHalI2COP->HalI2CClrAllIntr(pHalI2CInitDat);

            /* Update I2C device status */
            pSalI2CHND->DevSts = I2C_STS_IDLE;

            /* Invoke I2C TX complete callback if available */
            if (pSalI2CUserCB->pTXCCB->USERCB != NULL)
                pSalI2CUserCB->pTXCCB->USERCB((void *)pSalI2CUserCB->pTXCCB->USERData);
        } else {
            DBG_I2C_ERR("I2C%d INTR_RX_DONE\n",I2CIrqIdx);
            DBG_I2C_ERR("I2C%d IC_TXFLR:%2x\n",I2CIrqIdx,
            pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_TXFLR));

            /* Clear I2C interrupt */
            pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_RX_DONE;
            pHalI2COP->HalI2CClrIntr(pHalI2CInitDat); 

            /* Update I2C device status */
            pSalI2CHND->DevSts  = I2C_STS_ERROR;

            /* Update I2C error type */
            pSalI2CHND->ErrType |= I2C_ERR_SLV_TX_NACK;

            /* Invoke I2C error callback if available */
            if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
        }
    }

    /* I2C TX Abort Intr */
    if (pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_INTR_STAT) & 
                                BIT_CTRL_IC_INTR_STAT_R_TX_ABRT(1)) {
        I2CStsTmp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_TX_ABRT_SOURCE);
        DBG_I2C_ERR("!!!I2C%d INTR_TX_ABRT!!!\n",I2CIrqIdx);
        DBG_I2C_ERR("I2C%d IC_TX_ABRT_SOURCE[%2x]: %x\n", I2CIrqIdx, REG_DW_I2C_IC_TX_ABRT_SOURCE, I2CStsTmp);
        DBG_I2C_ERR("Dev Sts:%x\n",pSalI2CHND->DevSts);
        DBG_I2C_ERR("rx len:%x\n",pSalI2CHND->pRXBuf->DataLen);
        DBG_I2C_ERR("tx len:%x\n",pSalI2CHND->pTXBuf->DataLen);
        DBG_I2C_ERR("raw sts:%x\n",pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_RAW_INTR_STAT));
        DBG_I2C_ERR("ic sts:%x\n",pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS));
        /* Clear I2C Interrupt */
        pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_TX_ABRT;
        pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);
        I2CLocalTemp = pSalI2CHND->DevSts;
                
        /* Update I2C device status */
        pSalI2CHND->DevSts  = I2C_STS_ERROR;

        /* Update I2C error type */
        if ((I2CStsTmp & (BIT_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK | 
                          BIT_IC_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK |
                          BIT_IC_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK)) != 0) {
            pSalI2CHND->ErrType |= I2C_ERR_MST_A_NACK;
        }
        else if ((I2CStsTmp & BIT_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK) != 0){
            pSalI2CHND->ErrType |= I2C_ERR_MST_D_NACK;
        }
        else {
            pSalI2CHND->ErrType |= I2C_ERR_TX_ABRT;
        }
        

        /* Invoke I2C error callback */
        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
            pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
			
		if (pSalI2CHND->I2CExd & I2C_EXD_MTR_ADDR_RTY) {
	        if (pSalI2CHND->I2CMaster == I2C_MASTER_MODE) {
	            if ((I2CLocalTemp == I2C_STS_RX_READY) || (I2CLocalTemp == I2C_STS_RX_ING)) {
	                /* Clear Abort source */
	                pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_CLR_TX_ABRT);
                
	                /* Update I2C device status */
	                pSalI2CHND->DevSts  = I2C_STS_RX_ING;
                                    
                    if (pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS)
                                  & BIT_IC_STATUS_TFNF) {
                        if (pSalI2CMngtAdpt->MstRDCmdCnt > 0) {
                                pHalI2CInitDat->I2CCmd    = I2C_READ_CMD;
                            pHalI2CInitDat->I2CDataLen= 1;
                            pHalI2CInitDat->I2CRWData = pSalI2CHND->pRXBuf->pDataBuf;
                            pHalI2CInitDat->I2CStop   = I2C_STOP_DIS;
                            if ((pSalI2CMngtAdpt->MstRDCmdCnt == 1) && ((pSalI2CHND->I2CExd & I2C_EXD_MTR_HOLD_BUS) == 0))
                                pHalI2CInitDat->I2CStop   = I2C_STOP_EN;
                            pSalI2CMngtAdpt->MstRDCmdCnt--;
                            pHalI2COP->HalI2CMassSend(pHalI2CInitDat);
                        }
                        
                    }
	            }
	            else if ((I2CLocalTemp == I2C_STS_TX_READY) || (I2CLocalTemp == I2C_STS_TX_ING)){
	                /* Clear Abort source */
	                pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_CLR_TX_ABRT);
                              
	                /* Update I2C device status */
	                pSalI2CHND->DevSts  = I2C_STS_TX_ING;

	                /* Return to the former transfer status */                
	                pSalI2CHND->pTXBuf->pDataBuf--;
	                pSalI2CHND->pTXBuf->DataLen++;
                
	                if (pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS)
	                        & BIT_IC_STATUS_TFNF) {
	                    pHalI2CInitDat->I2CCmd    = I2C_WRITE_CMD;
	                    pHalI2CInitDat->I2CDataLen= 1;
	                    pHalI2CInitDat->I2CRWData = pSalI2CHND->pTXBuf->pDataBuf;
	                    pHalI2CInitDat->I2CStop   = I2C_STOP_DIS;
	                    if ((pSalI2CHND->pTXBuf->DataLen == 1) && ((pSalI2CHND->I2CExd & I2C_EXD_MTR_HOLD_BUS) == 0))
	                        pHalI2CInitDat->I2CStop   = I2C_STOP_EN;

	                    pHalI2COP->HalI2CMassSend(pHalI2CInitDat);

	                    pSalI2CHND->pTXBuf->pDataBuf++;
	                    pSalI2CHND->pTXBuf->DataLen--;
	                }                
	            }
	        }
		}
    }

    /* I2C RD REQ Intr */
    if (pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_INTR_STAT) &
                                BIT_CTRL_IC_INTR_STAT_R_RD_REQ(1)) {
        /* Confirm it's slave mode */
        if (pSalI2CHND->I2CMaster == I2C_SLAVE_MODE) {
            if (pSalI2CHND->DevSts == I2C_STS_IDLE) {
                /* Disable I2C RD REQ Interrupts first */
                I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_INTR_MASK);
                I2CLocalTemp &= ~(BIT_IC_INTR_MASK_M_RD_REQ);
                pHalI2CInitDat->I2CIntrMSK = I2CLocalTemp;
                pHalI2COP->HalI2CIntrCtrl(pHalI2CInitDat);
                
                /* Invoke I2C rd req callback if available */
                if (pSalI2CUserCB->pRDREQCB->USERCB != NULL)
                    pSalI2CUserCB->pRDREQCB->USERCB((void *)pSalI2CUserCB->pRDREQCB->USERData);
            } else if ((pSalI2CHND->DevSts == I2C_STS_TX_READY) || (pSalI2CHND->DevSts == I2C_STS_TX_ING)) {
                if (pSalI2CHND->pTXBuf->DataLen>0) {
                    /* Update I2C device status */
                    pSalI2CHND->DevSts = I2C_STS_TX_ING;
                    
                    /* Invoke I2C TX callback if available */
                    if (pSalI2CUserCB->pTXCB->USERCB != NULL)
                        pSalI2CUserCB->pTXCB->USERCB((void *)pSalI2CUserCB->pTXCB->USERData);
                        
                    /* I2C Slave transmits data to Master. If the TX FIFO is NOT full,
                                write one byte from slave TX buffer to TX FIFO. */    
                    if ((pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_STATUS)
                                & (BIT_IC_STATUS_TFNF)) == BIT_IC_STATUS_TFNF) {
                        pHalI2CInitDat->I2CCmd    = I2C_WRITE_CMD;
                        pHalI2CInitDat->I2CDataLen= 1;
                        pHalI2CInitDat->I2CRWData = pSalI2CHND->pTXBuf->pDataBuf;
                        pHalI2CInitDat->I2CStop   = I2C_STOP_DIS;
                        if ((pSalI2CHND->pTXBuf->DataLen == 1) && ((pSalI2CHND->I2CExd & I2C_EXD_MTR_HOLD_BUS) == 0))
                            pHalI2CInitDat->I2CStop   = I2C_STOP_EN;

                        pHalI2COP->HalI2CMassSend(pHalI2CInitDat);

                        pSalI2CHND->pTXBuf->pDataBuf++;
                        pSalI2CHND->pTXBuf->DataLen--;
                    }
                }
                
                /* To clear Read Request Intr */
                pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_RD_REQ;
                pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);            

                /* To check I2C slave TX data length. If all the data are transmitted,
                          mask all the interrupts and invoke the user callback */
                if (!pSalI2CHND->pTXBuf->DataLen) {
                    /* This is a software patch */
                    pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_RAW_INTR_STAT);
                    HalDelayUs(1000);
                    
                    /* Disable I2C TX Related Interrupts */
                    I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_INTR_MASK);
                    I2CLocalTemp &= ~(BIT_IC_INTR_MASK_M_TX_ABRT |
                                      BIT_IC_INTR_MASK_M_TX_OVER |
                                      BIT_IC_INTR_MASK_M_RX_DONE |
                                      BIT_IC_INTR_MASK_M_RD_REQ);
                    pHalI2CInitDat->I2CIntrMSK = I2CLocalTemp;
                    pHalI2COP->HalI2CIntrCtrl(pHalI2CInitDat);
                    pHalI2COP->HalI2CClrAllIntr(pHalI2CInitDat);

                    /* Update I2C device status */
                    pSalI2CHND->DevSts = I2C_STS_IDLE;

                    /* Invoke I2C TX complete callback if available */
                    if (pSalI2CUserCB->pTXCCB->USERCB != NULL)
                        pSalI2CUserCB->pTXCCB->USERCB((void *)pSalI2CUserCB->pTXCCB->USERData);
                }
            }            
        }
    } 

    /* I2C TX Empty Intr */
    if (pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_INTR_STAT) & 
                                BIT_CTRL_IC_INTR_STAT_R_TX_EMPTY(1)) {
        /* Confirm it's master mode */
        if (pSalI2CHND->I2CMaster == I2C_MASTER_MODE) {
        
            /* To check I2C master TX data length. If all the data are transmitted,
                      mask all the interrupts and invoke the user callback */
            if (!pSalI2CHND->pTXBuf->DataLen) {
                /* I2C Disable TX Related Interrupts */
                I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_INTR_MASK);
                I2CLocalTemp &= ~(BIT_IC_INTR_MASK_M_TX_ABRT  | 
                                  BIT_IC_INTR_MASK_M_TX_EMPTY |
                                  BIT_IC_INTR_MASK_M_TX_OVER);
                pHalI2CInitDat->I2CIntrMSK = I2CLocalTemp;
                pHalI2COP->HalI2CIntrCtrl(pHalI2CInitDat);

                /* Clear all I2C pending interrupts */
                pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);
                /* Update I2C device status */
                pSalI2CHND->DevSts = I2C_STS_IDLE;

                /* Invoke I2C TX Complete callback */
                if (pSalI2CUserCB->pTXCCB->USERCB != NULL)
                    pSalI2CUserCB->pTXCCB->USERCB((void *)pSalI2CUserCB->pTXCCB->USERData);
            }
            
            if (pSalI2CHND->pTXBuf->DataLen > 0) {
                /* Update I2C device status */
                pSalI2CHND->DevSts = I2C_STS_TX_ING;

                /* Invoke I2C TX callback if available */
                if (pSalI2CUserCB->pTXCB->USERCB != NULL)
                    pSalI2CUserCB->pTXCB->USERCB((void *)pSalI2CUserCB->pTXCB->USERData);
                    
                /* Check I2C TX FIFO status. If it's not full, one byte data will be written into it. */
                if ((pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_STATUS)
                            & (BIT_IC_STATUS_TFNF)) == BIT_IC_STATUS_TFNF) {
                    pHalI2CInitDat->I2CCmd    = I2C_WRITE_CMD;
                    pHalI2CInitDat->I2CDataLen= 1;
                    pHalI2CInitDat->I2CRWData = pSalI2CHND->pTXBuf->pDataBuf;
                    pHalI2CInitDat->I2CStop   = I2C_STOP_DIS;
                    pHalI2CInitDat->I2CReSTR  = 1;
                    if ((pSalI2CHND->pTXBuf->DataLen == 1) && ((pSalI2CHND->I2CExd & I2C_EXD_MTR_HOLD_BUS) == 0))
                        pHalI2CInitDat->I2CStop   = I2C_STOP_EN;

                    pHalI2COP->HalI2CMassSend(pHalI2CInitDat);

                    pSalI2CHND->pTXBuf->pDataBuf++;
                    pSalI2CHND->pTXBuf->DataLen--;

                } 
            }            
        }/*if (pSalI2CHND->I2CMaster == I2C_MASTER_MODE)*/   
    }

    /* I2C TX Over Run Intr */
    if (pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_INTR_STAT) & 
                                BIT_CTRL_IC_INTR_STAT_R_TX_OVER(1)) {

        DBG_I2C_ERR("!!!I2C%d INTR_TX_OVER!!!\n",I2CIrqIdx);

        /* Clear I2C interrupt */
        pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_TX_OVER;
        pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);

        /* Update I2C device status */
        pSalI2CHND->DevSts  = I2C_STS_ERROR;

        /* Update I2C error type */
        pSalI2CHND->ErrType |= I2C_ERR_TX_OVER;

        /* Invoke I2C error callback if available */
        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
            pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
    }

    /* I2C RX Full Intr */
    if (pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_INTR_STAT) & 
                                BIT_CTRL_IC_INTR_STAT_R_RX_FULL(1)) {
        /* Check if it's Master */
        if (pSalI2CHND->I2CMaster == I2C_MASTER_MODE){
            
            //DBG_8195A("full\n");
            /* Check if the receive transfer is NOT finished. If it is not, check if there 
                    is data in the RX FIFO and move the data from RX FIFO to user data buffer*/
            if (pSalI2CHND->pRXBuf->DataLen > 0) {
                               
                /* Update I2C device status */
                pSalI2CHND->DevSts = I2C_STS_RX_ING;

                /* Invoke I2C RX callback if available */
                if (pSalI2CUserCB->pRXCB->USERCB != NULL)
                    pSalI2CUserCB->pRXCB->USERCB((void *)pSalI2CUserCB->pRXCB->USERData);
                                
                I2CInTOTcnt     = (u32)pSalI2CMngtAdpt->InnerTimeOut;
                InTimeoutCount  = 0;
                /* Calculate internal time out parameters */
                if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
                    InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
                    InStartCount = HalTimerOp.HalTimerReadCount(1);
                }

                while (1) {                  
                    I2CLocalRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS);
                    if ((I2CLocalRawSts & (BIT_IC_STATUS_RFNE | BIT_IC_STATUS_RFF)) != 0) {                    
                        *(pSalI2CHND->pRXBuf->pDataBuf) = 
                                        pHalI2COP->HalI2CReceive(pHalI2CInitDat);
                        //DBG_8195A("rx:%x\n",*(pSalI2CHND->pRXBuf->pDataBuf));
                        pSalI2CHND->pRXBuf->pDataBuf++;
                        pSalI2CHND->pRXBuf->DataLen--;
						
                        if ((pSalI2CHND->pRXBuf->DataLen) == 0)
                            break;
                    }
                    else if ((pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_RAW_INTR_STAT)
                                & (BIT_IC_RAW_INTR_STAT_RX_OVER | BIT_IC_RAW_INTR_STAT_RX_UNDER)) != 0) {
                        break;                        
                    }
                    else {
                        if ((pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS) & BIT_IC_STATUS_RFNE)
                                            == 0){
                            break;
                        }                    	
                    }

                    if (InTimeoutCount > 0) {
                        if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
                            pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                            pSalI2CHND->ErrType = I2C_ERR_RX_FF_TO;
                            DBG_I2C_ERR("RX Full Timeout, I2C%2x,1\n",pSalI2CHND->DevNum);
                            DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                            break;
                        }
                    }
                    else {
                        if (I2CInTOTcnt == 0) {
                            pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                            pSalI2CHND->ErrType = I2C_ERR_RX_FF_TO;
                            DBG_I2C_ERR("RX Full Timeout, I2C%2x,2\n",pSalI2CHND->DevNum);
                            DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                            break;
                        }
                    }
                }
            }

            /* To check I2C master RX data length. If all the data are received,
                    mask all the interrupts and invoke the user callback.
                    Otherwise, the master should send another Read Command to slave for
                    the next data byte receiving. */
            if (!pSalI2CHND->pRXBuf->DataLen) {
                /* I2C Disable RX Related Interrupts */
                I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_INTR_MASK);
                I2CLocalTemp &= ~(BIT_IC_INTR_MASK_M_RX_FULL |
                                  BIT_IC_INTR_MASK_M_RX_OVER |
                                  BIT_IC_INTR_MASK_M_RX_UNDER|
                                  BIT_IC_INTR_MASK_M_TX_ABRT);

                pHalI2CInitDat->I2CIntrMSK = I2CLocalTemp;
                pHalI2COP->HalI2CIntrCtrl(pHalI2CInitDat);

                /* Clear all I2C pending interrupts */
                pHalI2COP->HalI2CClrAllIntr(pHalI2CInitDat);
                /* Update I2C device status */
                pSalI2CHND->DevSts = I2C_STS_IDLE;

                /* Invoke I2C RX complete callback if available */
                if (pSalI2CUserCB->pRXCCB->USERCB != NULL)
                    pSalI2CUserCB->pRXCCB->USERCB((void *)pSalI2CUserCB->pRXCCB->USERData);            
            }
            else {
                /* If TX FIFO is not full, another Read Command is written into it. */
                if (pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS)
                        & BIT_IC_STATUS_TFNF) {
                    if (pSalI2CMngtAdpt->MstRDCmdCnt > 0) {
                        pHalI2CInitDat->I2CCmd    = I2C_READ_CMD;
                        pHalI2CInitDat->I2CDataLen= 1;
                        pHalI2CInitDat->I2CRWData = pSalI2CHND->pRXBuf->pDataBuf;
                        pHalI2CInitDat->I2CStop   = I2C_STOP_DIS;
                        pHalI2CInitDat->I2CReSTR  = 1;
                        if ((pSalI2CMngtAdpt->MstRDCmdCnt == 1) && ((pSalI2CHND->I2CExd & I2C_EXD_MTR_HOLD_BUS) == 0)){
                            pHalI2CInitDat->I2CStop   = I2C_STOP_EN;
                        }
                        pSalI2CMngtAdpt->MstRDCmdCnt--;
                        pHalI2COP->HalI2CMassSend(pHalI2CInitDat);                        
                    }
                }
            }

        }/*(pSalI2CHND->I2CMaster == I2C_MASTER_MODE)*/
        else{
            /* To check I2C master RX data length. If all the data are received,
                    mask all the interrupts and invoke the user callback.
                    Otherwise, if there is data in the RX FIFO and move the data from RX 
                    FIFO to user data buffer*/
            if (pSalI2CHND->pRXBuf->DataLen > 0){

                /* Update I2C device status */
                pSalI2CHND->DevSts = I2C_STS_RX_ING;

                /* Invoke I2C RX callback if available */
                if (pSalI2CUserCB->pRXCB->USERCB != NULL)
                    pSalI2CUserCB->pRXCB->USERCB((void *)pSalI2CUserCB->pRXCB->USERData);
                    
                if ((pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS)
                            & (BIT_IC_STATUS_RFNE | BIT_IC_STATUS_RFF)) != 0) {                    
                    *(pSalI2CHND->pRXBuf->pDataBuf) = 
                                    pHalI2COP->HalI2CReceive(pHalI2CInitDat);
                    pSalI2CHND->pRXBuf->pDataBuf++;
                    pSalI2CHND->pRXBuf->DataLen--;
                }
            }

            /* All data are received. Mask all related interrupts. */
            if (!pSalI2CHND->pRXBuf->DataLen){
                /*I2C Disable RX Related Interrupts*/
                I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_INTR_MASK);
                I2CLocalTemp &= ~(BIT_IC_INTR_MASK_M_RX_FULL |
                                  BIT_IC_INTR_MASK_M_RX_OVER |
                                  BIT_IC_INTR_MASK_M_RX_UNDER);

                pHalI2CInitDat->I2CIntrMSK = I2CLocalTemp;
                pHalI2COP->HalI2CIntrCtrl(pHalI2CInitDat);

                /* Update I2C device status */
                pSalI2CHND->DevSts = I2C_STS_IDLE;

                /* Invoke I2C RX complete callback if available */
                if (pSalI2CUserCB->pRXCCB->USERCB != NULL)
                    pSalI2CUserCB->pRXCCB->USERCB((void *)pSalI2CUserCB->pRXCCB->USERData);            
            }
        }
    }

    /*I2C RX Over Run Intr*/
    if (pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_INTR_STAT) & 
                                BIT_CTRL_IC_INTR_STAT_R_RX_OVER(1)) {   

        DBG_I2C_ERR("I2C%d INTR_RX_OVER\n",I2CIrqIdx);

        pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_RX_OVER;
        pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);

        /* Update I2C device status */
        pSalI2CHND->DevSts  = I2C_STS_ERROR;

        /* Update I2C error type */
        pSalI2CHND->ErrType |= I2C_ERR_RX_OVER;

        /* Invoke I2C error callback if available */
        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
            pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
    }

    /*I2C RX Under Run Intr*/
    if (pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_INTR_STAT) &
                                BIT_CTRL_IC_INTR_STAT_R_RX_UNDER(1)) {

        DBG_I2C_ERR("!!!I2C%d INTR_RX_UNDER!!!\n",I2CIrqIdx);

        pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_RX_UNDER;
        pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);

        /* Update I2C device status */
        pSalI2CHND->DevSts  = I2C_STS_ERROR;

        /* Update I2C error type */
        pSalI2CHND->ErrType |= I2C_ERR_RX_UNDER;

        /* Invoke I2C error callback if available */
        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
            pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
    }
}
#endif

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CSend
//
// Description:
//         To execute Master-Transmitter and Slave-Transmitter operation.
//         There are 3 operation mode in this function which are separated by compile-time
//          flag.
//         For Master-Transmitter, the User Register Address flag is checked first.
//         User Register Address may be sent before any formal transfer, no matter in
//         Poll-, Intr- or DMA- Mode.
//
//         In Poll-Mode, no matter it's master or slave mode, the transfer will be done in 
//         this function by checking the transfer length.
//              -Master in Poll-Mode: 
//                a. Send the User Register Address if needed.
//                b. Check if all the data are transmitted. If it's NOT, checking the TX FIFO
//                    status is done for writing data from user TX buffer to I2C TX FIFO when
//                    TX FIFO is NOT full.
//                    TX data length  decrements one after writing one byte into TX FIFO.
//                c. b is executed circularly till the TX buffer data length is zero.
//
//              -Slave in Poll-Mode: 
//                    Slave could send data only when it received a Read Commmand matched 
//                    with its own I2C address from other I2C master. Once a slave correctly
//                    received a Read Command matched with its own addr., a Read-Request
//                    flag is set at the same time.
//                    In this Poll-Mode, the slave checks the Read-Request flag to decide
//                    if it could send its TX buffer data.
//                a. Check if the Read-Request flag is set or not. If the flag is set, it should
//                    check if TX buffer data length is zero. If it's NOT, 
//                    the I2C TX FIFO status will be checked for the following operation.
//                b. If the TX FIFO is NOT empty, slave will write one byte data from TX data
//                    buffer to TX FIFO.
//                c. a and b are executed circularly till the TX buffer data length is zero.
//----------------------------------------------------------------------
//         In Intr-Mode, this function is used to unmask the realted I2C interrupt for 
//         the following interrupt operations.
//              -Master in Intr-Mode:
//                a. Send the User Register Address if needed.
//                b. Unmask the TX-Empty and realted error interrupts.
//
//              -Slave in Intr-Mode:
//                a. Unmask the RD-Req and realted error interrupts.
//
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C send process.
//          _EXIT_SUCCESS if the RtkI2CSend succeeded.
//          _EXIT_FAILURE if the RtkI2CSend failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
HAL_Status
RtkI2CSend_Patch(
    IN  VOID *Data
){
    PSAL_I2C_HND        pSalI2CHND          = (PSAL_I2C_HND) Data;
    PSAL_I2C_HND_PRIV   pSalI2CHNDPriv      = NULL;
    PSAL_I2C_MNGT_ADPT  pSalI2CMngtAdpt     = NULL;
    PHAL_I2C_INIT_DAT   pHalI2CInitDat      = NULL;
    PHAL_I2C_OP         pHalI2COP           = NULL;    

    PHAL_GDMA_ADAPTER   pHalI2CTxGdmaAdpt   = NULL;
    PHAL_GDMA_OP        pHalI2CGdmaOp       = NULL;
    PSAL_I2C_USER_CB    pSalI2CUserCB       = NULL;
    
    u32 I2CLocalTemp = 0;
    u32 I2CInTOTcnt = 0;
    u32 InTimeoutCount = 0;
    u32 InStartCount = 0;
    u32 I2CChkRawSts = 0;
    u32 I2CChkRawSts2 = 0;
    u32 I2CDataLenBak = 0;
    u32 I2CDataPtrBak = 0;
    //u32 I2CInTOTcntIntr = 0;
    //u32 InTimeoutCountIntr = 0;
    //u32 InStartCountIntr = 0;
    u32 I2CMtrTtyCnt    = 0;
    
    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalI2CHNDPriv  = CONTAINER_OF(pSalI2CHND, SAL_I2C_HND_PRIV, SalI2CHndPriv);
    pSalI2CMngtAdpt = CONTAINER_OF(pSalI2CHNDPriv->ppSalI2CHnd, SAL_I2C_MNGT_ADPT, pSalHndPriv);

    
    
    pHalI2CInitDat  = pSalI2CMngtAdpt->pHalInitDat;

    pHalI2COP       = pSalI2CMngtAdpt->pHalOp;
    pSalI2CUserCB   = pSalI2CHND->pUserCB;

    pHalI2CTxGdmaAdpt   = pSalI2CMngtAdpt->pHalTxGdmaAdp;
    pHalI2CGdmaOp       = pSalI2CMngtAdpt->pHalGdmaOp;

    /* Check if it's Master Mode */
    if (pSalI2CHND->I2CMaster == I2C_MASTER_MODE) {   
        I2CMtrTtyCnt = 0;
        
        /* Master run-time update  target address */
        if (pSalI2CHND->I2CExd & I2C_EXD_MTR_ADDR_UPD) {

            /* Calculate user time out parameters */
            I2CInTOTcnt = pSalI2CHND->TimeOut;
            if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
                InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
                InStartCount = HalTimerOp.HalTimerReadCount(1);
            }

            /* Check Master activity status */
            while ((pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS)) & BIT_IC_STATUS_MST_ACTIVITY) {                
                /* Time-Out check */
                if (InTimeoutCount > 0) {
                    if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
                        pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                        pSalI2CHND->ErrType = I2C_ERR_TX_ADD_TO;
                        DBG_I2C_ERR("RtkI2CSend Timeout, I2C%2x,1\n",pSalI2CHND->DevNum);
                        DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                        return HAL_TIMEOUT;
                    }
                }
                else {
                    if (I2CInTOTcnt == 0) {
                        pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                        pSalI2CHND->ErrType = I2C_ERR_TX_ADD_TO;
                        DBG_I2C_ERR("RtkI2CSend Timeout, I2C%2x,2\n",pSalI2CHND->DevNum);
                        DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                        return HAL_TIMEOUT;
                    }
                }
            }
            /* Calculate user time out parameters */
            I2CInTOTcnt = pSalI2CHND->TimeOut;
            if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
                InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
                InStartCount = HalTimerOp.HalTimerReadCount(1);
            }

            /* Check TX FIFO status */
            while (!((pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS)) & BIT_IC_STATUS_TFE)) {                
                /* Time-Out check */
                if (InTimeoutCount > 0) {
                    if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
                        pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                        pSalI2CHND->ErrType = I2C_ERR_TX_ADD_TO;
                        DBG_I2C_ERR("RtkI2CSend Timeout, I2C%2x,3\n",pSalI2CHND->DevNum);
                        DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                        return HAL_TIMEOUT;
                    }
                }
                else {
                    if (I2CInTOTcnt == 0) {
                        pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                        pSalI2CHND->ErrType = I2C_ERR_TX_ADD_TO;
                        DBG_I2C_ERR("RtkI2CSend Timeout, I2C%2x,4\n",pSalI2CHND->DevNum);
                        DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                        return HAL_TIMEOUT;
                    }
                }
            }

            I2CLocalTemp = 0;
            I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_TAR);
            I2CLocalTemp &= (~BIT_MASK_IC_TAR);
            I2CLocalTemp |= BIT_CTRL_IC_TAR(pSalI2CHND->pTXBuf->TargetAddr);
            /* Update Master Target address */
            pHalI2COP->HalI2CWriteReg(pHalI2CInitDat, REG_DW_I2C_IC_TAR, I2CLocalTemp);
        }

        RtkI2CSendUserAddr(pSalI2CHND, 0);      

        /* #if I2C_POLL_OP_TYPE */
        if (pSalI2CHND->OpType == I2C_POLL_TYPE) { /* if (pSalI2CHND->OpType == I2C_POLL_TYPE) */
            /* I2C Device Status Update */
            pSalI2CHND->DevSts  = I2C_STS_TX_READY;

            /* Calculate user time out parameters */
            I2CInTOTcnt = pSalI2CHND->TimeOut;
            if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
                InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
                InStartCount = HalTimerOp.HalTimerReadCount(1);
            }
            
            /* Send data till the TX buffer data length is zero */
            for (;pSalI2CHND->pTXBuf->DataLen>0;) {
                /* I2C Device Status Update */
                pSalI2CHND->DevSts  = I2C_STS_TX_ING;
                
                /* Check I2C TX FIFO status */
                if ((pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_STATUS)
                            & (BIT_IC_STATUS_TFNF)) == BIT_IC_STATUS_TFNF) {
                    /* Wrtie data into I2C TX FIFO */
                    pHalI2CInitDat->I2CCmd    = I2C_WRITE_CMD;
                    pHalI2CInitDat->I2CDataLen= 1;
                    pHalI2CInitDat->I2CRWData = pSalI2CHND->pTXBuf->pDataBuf;
                    pHalI2CInitDat->I2CStop   = I2C_STOP_DIS;
                    if ((pSalI2CHND->pTXBuf->DataLen == 1) && ((pSalI2CHND->I2CExd & I2C_EXD_MTR_HOLD_BUS) == 0))
                        pHalI2CInitDat->I2CStop   = I2C_STOP_EN;
                    pHalI2COP->HalI2CMassSend(pHalI2CInitDat);

                    pSalI2CHND->pTXBuf->pDataBuf++;
                    pSalI2CHND->pTXBuf->DataLen--;
                }
                else {
                    /* Time-Out check */
                    if (InTimeoutCount > 0) {
                        if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
                            pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                            pSalI2CHND->ErrType = I2C_ERR_TX_CMD_TO;
                            DBG_I2C_ERR("RtkI2CSend Timeout, I2C%2x,5\n",pSalI2CHND->DevNum);
                            DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                            if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                            return HAL_TIMEOUT;
                        }
                    }
                    else {
                        if (I2CInTOTcnt == 0) {
                            pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                            pSalI2CHND->ErrType = I2C_ERR_TX_CMD_TO;
                            DBG_I2C_ERR("RtkI2CSend Timeout, I2C%2x,6\n",pSalI2CHND->DevNum);
                            DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                            if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                            return HAL_TIMEOUT;
                        }
                    }
                }
				
				if (pSalI2CHND->I2CExd & I2C_EXD_MTR_ADDR_RTY) {
					HalDelayUs(((1000*30)/pHalI2CInitDat->I2CClk));    //the 10 is for ten bit time                  
                    I2CLocalTemp = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_RAW_INTR_STAT);
	                if (I2CLocalTemp & BIT_IC_RAW_INTR_STAT_TX_ABRT) {
                        I2CMtrTtyCnt++;
                        I2CLocalTemp = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_TX_ABRT_SOURCE);
	                    if (I2CMtrTtyCnt > I2C_MTR_RTY_CNT) {
                            if ((I2CLocalTemp & (BIT_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK | 
                                              BIT_IC_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK |
                                              BIT_IC_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK)) != 0) {
                                pSalI2CHND->ErrType |= I2C_ERR_MST_A_NACK;
                            }
                            else if ((I2CLocalTemp & BIT_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK) != 0){
                                pSalI2CHND->ErrType |= I2C_ERR_MST_D_NACK;
                            }
                            else {
                                pSalI2CHND->ErrType |= I2C_ERR_TX_ABRT;
                            }
                            
                            pSalI2CHND->DevSts  = I2C_STS_ERROR;
                            pSalI2CHND->pTXBuf->pDataBuf--;
                            pSalI2CHND->pTXBuf->DataLen++;
                            if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                            return HAL_ERR_HW;
	                    }
	                    pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_CLR_TX_ABRT);
	                    pSalI2CHND->pTXBuf->pDataBuf--;
	                    pSalI2CHND->pTXBuf->DataLen++;
	                }
				}           
            }

            /* Calculate user time out parameters */
            I2CInTOTcnt = pSalI2CMngtAdpt->InnerTimeOut;
            if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
                InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
                InStartCount = HalTimerOp.HalTimerReadCount(1);
            }
            /* I2C Wait TX FIFO Empty */
            while (1) {
                if ((pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_STATUS)
                        & (BIT_IC_STATUS_TFE | BIT_IC_STATUS_TFNF)) ==
                          (BIT_IC_STATUS_TFE | BIT_IC_STATUS_TFNF)){
					break;  	  
				}
				else {
					/* Time-Out check */
	                if (InTimeoutCount > 0) {
	                    if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
	                        pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
	                        pSalI2CHND->ErrType = I2C_ERR_TX_FF_TO;
	                        DBG_I2C_ERR("RtkI2CSend Timeout, I2C%2x,7\n",pSalI2CHND->DevNum);
	                        DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
	                        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
	                        return HAL_TIMEOUT;
	                    }
	                }
	                else {
	                    if (I2CInTOTcnt == 0) {
	                        pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
	                        pSalI2CHND->ErrType = I2C_ERR_TX_FF_TO;
	                        DBG_I2C_ERR("RtkI2CSend Timeout, I2C%2x,8\n",pSalI2CHND->DevNum);
	                        DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
	                        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
	                        return HAL_TIMEOUT;
	                    }
	                }
				}                
            }

            /* Invoke I2C TX complete callback if available */
            if (pSalI2CUserCB->pTXCCB->USERCB != NULL)
                pSalI2CUserCB->pTXCCB->USERCB((void *)pSalI2CUserCB->pTXCCB->USERData);
                
            /* I2C Device Status Update */
            pSalI2CHND->DevSts  = I2C_STS_IDLE;
        }/* if (pSalI2CHND->OpType == I2C_POLL_TYPE) */
        /* #if I2C_POLL_OP_TYPE */

#if I2C_INTR_OP_TYPE
        if (pSalI2CHND->OpType == I2C_INTR_TYPE) {  /* if (pSalI2CHND->OpType == I2C_INTR_TYPE) */
			/* Calculate user time out parameters */
			InTimeoutCount = 0;
			InStartCount = 0;
            I2CInTOTcnt = pSalI2CHND->TimeOut;

            //InTimeoutCountIntr = 0;
            //InStartCountIntr = 0;            
            //I2CInTOTcntIntr = pSalI2CHND->AddRtyTimeOut;
            
            if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
                InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
                InStartCount = HalTimerOp.HalTimerReadCount(1);
            }

            //InTimeoutCountIntr = (I2CInTOTcntIntr*1000/TIMER_TICK_US);
            //InStartCountIntr = HalTimerOp.HalTimerReadCount(1);

            
            I2CDataLenBak = (u32)(pSalI2CHND->pTXBuf->DataLen);
            I2CDataPtrBak = (u32)(pSalI2CHND->pTXBuf->pDataBuf);
            /* I2C Device Status Update */
            pSalI2CHND->DevSts = I2C_STS_IDLE;
            pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_TX_ABRT;
            pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);
            pHalI2COP->HalI2CClrAllIntr(pHalI2CInitDat);
            
            /* Send data till the TX buffer data length is zero */
            for (;;) {
SEND_I2C_WR_CMD_INTR:

                /* I2C Device Status Update */
                pSalI2CHND->DevSts  = I2C_STS_TX_ING;

                /* Check I2C TX FIFO status */
                /* Fill TX FIFO only when it's completely empty */
                I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS);
                if ((I2CChkRawSts & BIT_IC_STATUS_TFE) == BIT_IC_STATUS_TFE) {
                    if (pSalI2CHND->pTXBuf->DataLen > 0) {
                        /* Wrtie data into I2C TX FIFO */
                        pHalI2CInitDat->I2CCmd    = I2C_WRITE_CMD;
                        pHalI2CInitDat->I2CDataLen= 1;
                        pHalI2CInitDat->I2CRWData = pSalI2CHND->pTXBuf->pDataBuf;
                        pHalI2CInitDat->I2CStop     = I2C_STOP_DIS;
                            
                        if ((pSalI2CHND->pTXBuf->DataLen == 1) && ((pSalI2CHND->I2CExd & I2C_EXD_MTR_HOLD_BUS) == 0))
                            pHalI2CInitDat->I2CStop   = I2C_STOP_EN;

                        if ((pHalI2CInitDat->I2CReSTR == 1) && (pSalI2CHND->pTXBuf->DataLen == I2CDataLenBak)){
                            pHalI2CInitDat->RSVD0 |= BIT0;
                        }
                        
                        pHalI2COP->HalI2CMassSend(pHalI2CInitDat);

                        pSalI2CHND->pTXBuf->pDataBuf++;
                        pSalI2CHND->pTXBuf->DataLen--;
                        
                        if (pHalI2CInitDat->I2CReSTR == 1) {
                            pHalI2CInitDat->RSVD0 &= (~BIT0);
                        }
                    }
                }
                				
				if (pSalI2CHND->I2CExd & I2C_EXD_MTR_ADDR_RTY) {
                    u32 I2CInTOTcntRty      = 0;
                    u32 InTimeoutCountRty   = 0;
                    u32 InStartCountRty     = 0;
                    
#if 0
                    /* SEND_I2C_WR_CMD_INTR Time-Out check */
                    if (InTimeoutCountIntr > 0) {
                        if (HAL_TIMEOUT == I2CIsTimeout(InStartCountIntr, InTimeoutCountIntr)) {
                            /* Reset the data count before status return */
                            pSalI2CHND->pTXBuf->pDataBuf--;
                            pSalI2CHND->pTXBuf->DataLen++;
                            return HAL_TIMEOUT;
                        }
                    }
#endif
                    /* Calculate user master retry local time out parameters */
        			InTimeoutCountRty   = 0;
        			InStartCountRty     = 0;
                    I2CInTOTcntRty      = pSalI2CHND->TimeOut;          
                    
                    if ((I2CInTOTcntRty != 0) && (I2CInTOTcntRty!= I2C_TIMEOOUT_ENDLESS)) {
                        InTimeoutCountRty= (I2CInTOTcntRty*1000/TIMER_TICK_US);
                        InStartCountRty= HalTimerOp.HalTimerReadCount(1);
                    }
                    I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS);
                    while ((I2CChkRawSts & BIT_IC_STATUS_TFE) == 0) {
                        I2CChkRawSts2 = pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_RAW_INTR_STAT);
	                    if ((I2CChkRawSts2 & BIT_IC_RAW_INTR_STAT_TX_ABRT) != 0){
                            break;
	                    }
	                    
                        /* Time-Out check */
                        if (InTimeoutCountRty > 0) {
                            if (HAL_TIMEOUT == I2CIsTimeout(InStartCountRty, InTimeoutCountRty)) {
                                break;
                            }
                        }
                        else {
                            if (I2CInTOTcntRty == 0) {
                                break;
                            }
                        }
                        
                        /* Read I2C IC status again */
	                    I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS);
                    }

                    HalDelayUs((u32)((1000*30)/pHalI2CInitDat->I2CClk));    //the 10 is for ten bit time                  
                    
                    I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_RAW_INTR_STAT);
	                if (I2CChkRawSts & BIT_IC_RAW_INTR_STAT_TX_ABRT) {
	                    I2CMtrTtyCnt++;
	                    I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_TX_ABRT_SOURCE);
	                    if (I2CMtrTtyCnt > I2C_MTR_RTY_CNT) {
                            if ((I2CChkRawSts & (BIT_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK | 
                                              BIT_IC_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK |
                                              BIT_IC_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK)) != 0) {
                                pSalI2CHND->ErrType |= I2C_ERR_MST_A_NACK;
                            }
                            else if ((I2CChkRawSts & BIT_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK) != 0){
                                pSalI2CHND->ErrType |= I2C_ERR_MST_D_NACK;
                            }
                            else {
                                pSalI2CHND->ErrType |= I2C_ERR_TX_ABRT;
                            }
                            pSalI2CHND->DevSts  = I2C_STS_ERROR;
                            pSalI2CHND->pTXBuf->pDataBuf--;
                            pSalI2CHND->pTXBuf->DataLen++;
                            if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                            return HAL_ERR_HW;
	                    }
	                    
						/* use InTimeoutCount to backup tar here */
                        InTimeoutCount = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_TAR);
                        InTimeoutCount = BIT_GET_IC_TAR(InTimeoutCount);
						
	                    RtkI2CDeInitForPS(pSalI2CHND);
                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        while((I2CChkRawSts & BIT_IC_ENABLE_STATUS_IC_EN) == BIT_IC_ENABLE_STATUS_IC_EN){
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        }
                        
                        
                        RtkI2CInitForPS(pSalI2CHND);

                        pSalI2CHND->pInitDat->I2CAckAddr = InTimeoutCount;
                        HalI2CSetTarRtl8195a(pSalI2CHND->pInitDat);
                        
                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        while((I2CChkRawSts & BIT_IC_ENABLE_STATUS_IC_EN) != BIT_IC_ENABLE_STATUS_IC_EN){
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        }

                        pSalI2CHND->DevSts  = I2C_STS_TX_READY;
                        pSalI2CHND->ErrType = 0;
                        pSalI2CHND->pTXBuf->DataLen = (u16)I2CDataLenBak;
                        pSalI2CHND->pTXBuf->pDataBuf= (u8*)I2CDataPtrBak;
                        /* Calculate user time out parameters */
            			InTimeoutCount = 0;
            			InStartCount = 0;
                        I2CInTOTcnt = pSalI2CHND->TimeOut;          
                        if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
                            InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
                            InStartCount = HalTimerOp.HalTimerReadCount(1);
                        }
                        goto SEND_I2C_WR_CMD_INTR;

	                }
	                else if (((u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS) & BIT_IC_STATUS_TFE) != BIT_IC_STATUS_TFE) {
                        I2CMtrTtyCnt++;
                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_TX_ABRT_SOURCE);
	                    if (I2CMtrTtyCnt > I2C_MTR_RTY_CNT) {
                            if ((I2CChkRawSts & (BIT_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK | 
                                          BIT_IC_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK |
                                          BIT_IC_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK)) != 0) {
                                pSalI2CHND->ErrType |= I2C_ERR_MST_A_NACK;
                            }
                            else if ((I2CChkRawSts & BIT_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK) != 0){
                                pSalI2CHND->ErrType |= I2C_ERR_MST_D_NACK;
                            }
                            else {
                                pSalI2CHND->ErrType |= I2C_ERR_TX_ABRT;
                            }
                            pSalI2CHND->DevSts  = I2C_STS_ERROR;
                            pSalI2CHND->pTXBuf->pDataBuf--;
                            pSalI2CHND->pTXBuf->DataLen++;
                            if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                            return HAL_ERR_HW;
	                    }
	                    /* use InTimeoutCount to backup tar here */
                        InTimeoutCount = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_TAR);
                        InTimeoutCount = BIT_GET_IC_TAR(InTimeoutCount);
                        
                        RtkI2CDeInitForPS(pSalI2CHND);
                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        while((I2CChkRawSts & BIT_IC_ENABLE_STATUS_IC_EN) == BIT_IC_ENABLE_STATUS_IC_EN){
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        }

                        
                        
                        RtkI2CInitForPS(pSalI2CHND);

                        pSalI2CHND->pInitDat->I2CAckAddr = InTimeoutCount;
                        HalI2CSetTarRtl8195a(pSalI2CHND->pInitDat);

                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        while((I2CChkRawSts & BIT_IC_ENABLE_STATUS_IC_EN) != BIT_IC_ENABLE_STATUS_IC_EN){
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        }

                        pSalI2CHND->DevSts  = I2C_STS_TX_READY;
                        pSalI2CHND->ErrType = 0;
                        pSalI2CHND->pTXBuf->DataLen = (u16)I2CDataLenBak;
                        pSalI2CHND->pTXBuf->pDataBuf= (u8 *)I2CDataPtrBak;
                        /* Calculate user time out parameters */
            			InTimeoutCount = 0;
            			InStartCount = 0;
                        I2CInTOTcnt = pSalI2CHND->TimeOut;          
                        if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
                            InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
                            InStartCount = HalTimerOp.HalTimerReadCount(1);
                        }
                        goto SEND_I2C_WR_CMD_INTR;        
	               	}      
	               	else {                 
			            /* I2C Enable TX Related Interrupts */
			            I2CLocalTemp = 0;
			            I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_INTR_MASK);
			            I2CLocalTemp |= (BIT_IC_INTR_MASK_M_TX_ABRT  | 
			                             BIT_IC_INTR_MASK_M_TX_EMPTY |
			                             BIT_IC_INTR_MASK_M_TX_OVER);
			            pHalI2CInitDat->I2CIntrMSK = I2CLocalTemp;
			            pHalI2COP->HalI2CIntrCtrl(pHalI2CInitDat);
			                                        
                        break;
                    }
				} 
				else {
				    I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS);
                    while ((I2CChkRawSts & BIT_IC_STATUS_TFE) == 0) {
                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS);
                    }
                    
				    I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS);
	                if (I2CChkRawSts & BIT_IC_STATUS_TFE) {

	                    if (pSalI2CHND->pTXBuf->DataLen > 0) {
    			            /* I2C Enable TX Related Interrupts */
    			            I2CLocalTemp = 0;
    			            I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_INTR_MASK);
    			            I2CLocalTemp |= (BIT_IC_INTR_MASK_M_TX_ABRT  | 
    			                             BIT_IC_INTR_MASK_M_TX_EMPTY |
    			                             BIT_IC_INTR_MASK_M_TX_OVER);
    			            pHalI2CInitDat->I2CIntrMSK = I2CLocalTemp;
    			            pHalI2COP->HalI2CIntrCtrl(pHalI2CInitDat);                            
                        }
                        else {
                            /* Invoke I2C TX complete callback if available */
                            if (pSalI2CUserCB->pTXCCB->USERCB != NULL)
                                pSalI2CUserCB->pTXCCB->USERCB((void *)pSalI2CUserCB->pTXCCB->USERData);
							
							/* I2C Device Status Update */
							pSalI2CHND->DevSts  = I2C_STS_IDLE;
                        }
                        
                        break;  //end of send process in INTR mode
	                }
				}

				/* Time-Out check */
                if (InTimeoutCount > 0) {
                    if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
                        pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                        pSalI2CHND->ErrType = I2C_ERR_TX_FF_TO;
                        //DBG_I2C_ERR("RtkI2CSend Timeout, I2C%2x,7\n",pSalI2CHND->DevNum);
                        //DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);

                        /* use InTimeoutCount to backup tar here */
                        InTimeoutCount = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_TAR);
                        InTimeoutCount = BIT_GET_IC_TAR(InTimeoutCount);
                        
                        RtkI2CDeInitForPS(pSalI2CHND);
                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        while((I2CChkRawSts & BIT_IC_ENABLE_STATUS_IC_EN) == BIT_IC_ENABLE_STATUS_IC_EN){
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        }

                        
                        
                        RtkI2CInitForPS(pSalI2CHND);

                        pSalI2CHND->pInitDat->I2CAckAddr = InTimeoutCount;
                        HalI2CSetTarRtl8195a(pSalI2CHND->pInitDat);

                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        while((I2CChkRawSts & BIT_IC_ENABLE_STATUS_IC_EN) != BIT_IC_ENABLE_STATUS_IC_EN){
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        }
                        return HAL_TIMEOUT;
                    }
                }
                else {
                    if (I2CInTOTcnt == 0) {
                        pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                        pSalI2CHND->ErrType = I2C_ERR_TX_FF_TO;
                        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);

                        /* use InTimeoutCount to backup tar here */
                        InTimeoutCount = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_TAR);
                        InTimeoutCount = BIT_GET_IC_TAR(InTimeoutCount);
                        
                        RtkI2CDeInitForPS(pSalI2CHND);
                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        while((I2CChkRawSts & BIT_IC_ENABLE_STATUS_IC_EN) == BIT_IC_ENABLE_STATUS_IC_EN){
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        }

                        
                        
                        RtkI2CInitForPS(pSalI2CHND);

                        pSalI2CHND->pInitDat->I2CAckAddr = InTimeoutCount;
                        HalI2CSetTarRtl8195a(pSalI2CHND->pInitDat);

                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        while((I2CChkRawSts & BIT_IC_ENABLE_STATUS_IC_EN) != BIT_IC_ENABLE_STATUS_IC_EN){
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        }
                        return HAL_TIMEOUT;
                    }
                }
            }
        }                                           /* if (pSalI2CHND->OpType == I2C_INTR_TYPE) */
#endif

        /* if (pSalI2CHND->OpType == I2C_DMA_TYPE) */
        if (pSalI2CHND->OpType == I2C_DMA_TYPE) {  
            /* I2C Device Status Update */
            pSalI2CHND->DevSts  = I2C_STS_TX_READY;
            
            /* I2C Enable TX Related Interrupts */
            I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_INTR_MASK);
            I2CLocalTemp |= (BIT_IC_INTR_MASK_M_TX_ABRT  |
                             BIT_IC_INTR_MASK_M_TX_OVER);
            pHalI2CInitDat->I2CIntrMSK = I2CLocalTemp;
            pHalI2COP->HalI2CIntrCtrl(pHalI2CInitDat);

            //HalGdmaOpInit(pHalI2CGdmaOp);
            pSalI2CMngtAdpt->pHalGdmaOpInit(pHalI2CGdmaOp);
            pHalI2CTxGdmaAdpt->GdmaCtl.BlockSize    = pSalI2CHND->pTXBuf->DataLen;
            pHalI2CTxGdmaAdpt->ChSar                = (u32)pSalI2CHND->pTXBuf->pDataBuf;
            pHalI2CTxGdmaAdpt->ChDar                = (u32)(I2C0_REG_BASE+REG_DW_I2C_IC_DATA_CMD+
                                                                pSalI2CHND->DevNum*0x400);
            pHalI2CGdmaOp->HalGdmaChSeting(pHalI2CTxGdmaAdpt);
            pHalI2CGdmaOp->HalGdmaChEn(pHalI2CTxGdmaAdpt);
            pSalI2CHND->DevSts  = I2C_STS_TX_ING;
            pHalI2CInitDat->I2CDMACtrl = BIT_CTRL_IC_DMA_CR_TDMAE(1);
            pHalI2COP->HalI2CDMACtrl(pHalI2CInitDat);
            
        }                                           
        /* if (pSalI2CHND->OpType == I2C_DMA_TYPE) */

    }/* if (pSalI2CHND->I2CMaster == I2C_MASTER_MODE) */
    else{ 
        /* #if I2C_POLL_OP_TYPE */
        if (pSalI2CHND->OpType == I2C_POLL_TYPE) {
            /* Calculate user time out parameters */
            I2CInTOTcnt = pSalI2CHND->TimeOut;
            if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
                InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
                InStartCount = HalTimerOp.HalTimerReadCount(1);
            }
            
            /* I2C Device Status Update */
            pSalI2CHND->DevSts  = I2C_STS_TX_READY;
            
            /* Send data till the TX buffer data length is zero */
            for (;pSalI2CHND->pTXBuf->DataLen>0;) {
                /* I2C Device Status Update */
                pSalI2CHND->DevSts  = I2C_STS_TX_ING;
                
                /* Check I2C RD Request flag */
                if ((pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_RAW_INTR_STAT))
                        & BIT_IC_RAW_INTR_STAT_RD_REQ) {
                        
                    /* Check I2C TX FIFO status */
                    if ((pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_STATUS)
                            & (BIT_IC_STATUS_TFNF)) == BIT_IC_STATUS_TFNF) {
                        pHalI2CInitDat->I2CDataLen= 1;
                        pHalI2CInitDat->I2CRWData = pSalI2CHND->pTXBuf->pDataBuf;
                        pHalI2COP->HalI2CMassSend(pHalI2CInitDat);
                        pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_RD_REQ;
                        pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);
                        pSalI2CHND->pTXBuf->pDataBuf++;
                        pSalI2CHND->pTXBuf->DataLen--;
                    }
                }
                else {
                    /* Time-Out check */
                    if (InTimeoutCount > 0) {
                        if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
                            pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                            pSalI2CHND->ErrType = I2C_ERR_TX_FF_TO;
                            DBG_I2C_ERR("RtkI2CSend Timeout, I2C%2x,9\n",pSalI2CHND->DevNum);
                            DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                            if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                            return HAL_TIMEOUT;
                        }
                    }
                    else {
                        if (I2CInTOTcnt == 0) {
                            pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                            pSalI2CHND->ErrType = I2C_ERR_TX_FF_TO;
                            DBG_I2C_ERR("RtkI2CSend Timeout, I2C%2x,10\n",pSalI2CHND->DevNum);
                            DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                            if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                            return HAL_TIMEOUT;
                        }
                    }
                }               
            }

            /* I2C Device Status Update */
            pSalI2CHND->DevSts  = I2C_STS_IDLE;
        }
        /* #if I2C_POLL_OP_TYPE */

        /* #if I2C_INTR_OP_TYPE */
        if (pSalI2CHND->OpType == I2C_INTR_TYPE) {
            /* I2C Device Status Update */
            pSalI2CHND->DevSts = I2C_STS_IDLE;
#if 0
            pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_TX_ABRT;
            pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);
            pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_TX_OVER;
            pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);
            pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_RD_REQ;
            pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);
            pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_ACTIVITY;
            pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);
            pHalI2COP->HalI2CClrAllIntr(pHalI2CInitDat);
#endif
            pSalI2CHND->DevSts  = I2C_STS_TX_READY;
            
            /* I2C Enable TX Related Interrupts. In Slave-Transmitter, the below 
                    interrupts should be enabled. */
            I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_INTR_MASK);
            I2CLocalTemp |= (BIT_IC_INTR_MASK_M_TX_ABRT |
                             BIT_IC_INTR_MASK_M_TX_OVER |
                             BIT_IC_INTR_MASK_M_RX_DONE |
                             BIT_IC_INTR_MASK_M_RD_REQ);
            pHalI2CInitDat->I2CIntrMSK = I2CLocalTemp;
            pHalI2COP->HalI2CIntrCtrl(pHalI2CInitDat);
        }
        /* #if I2C_INTR_OP_TYPE */

        /* #if I2C_DMA_OP_TYPE */
            ;
        /* #if I2C_DMA_OP_TYPE */
    }

    return HAL_OK;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CReceive
//
// Description:
//         To execute Master-Receiver and Slave-Receiver operation.
//         There are 3 operation mode in this function which are separated by compile-time
//         flag.
//         For Master-Receiver, the User Register Address flag is checked first.
//         User Register Address may be sent before any formal transfer, no matter in
//         Poll-, Intr- or DMA- Mode.
//
//         For Master-Receiver, the I2C master have to send a Read Command for receiving 
//         one byte from the other I2C slave. 
//
//         In Poll-Mode, no matter it's master or slave mode, the transfer will be done in 
//         this function by checking the transfer length.
//              -Master in Poll-Mode: 
//                a. Send the User Register Address if needed.
//                b. Check if all the data are received. If it's NOT, checking the TX FIFO
//                    status will be done. If the TX FIFO it's full, a Read Command will be
//                    wirtten into the TX FIFO.
//                c. After b, the I2C master contineously polls the RX FIFO status to see
//                   if there is a received data. If it received one, it will move the data from
//                   I2C RX FIFO into user RX data buffer.
//                d. b and c are executed circularly till the RX buffer data length is zero.
//
//              -Slave in Poll-Mode: 
//                a. Check if all the data are received.
//                b. The I2C slave contineously polls the RX FIFO status to see
//                    if there is a received data. If it received one, it will move the data from
//                    I2C RX FIFO into user RX data buffer.
//                c. a and b are executed circularly till the RX buffer data length is zero.
//
//----------------------------------------------------------------------
//         In Intr-Mode, this function is used to unmask the realted I2C interrupt for 
//         the following interrupt operations.
//              -Master in Intr-Mode:
//                a. Send the User Register Address if needed.
//                b. Unmask the RX-Full and realted error interrupts.
//                c. Write one or two Read Command into master TX FIFO for requesting 
//                   another slave providing data.
//
//              -Slave in Intr-Mode:
//                a. Unmask the RX-Full and realted error interrupts.
//
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C receive process.
//          _EXIT_SUCCESS if the RtkI2CReceive succeeded.
//          _EXIT_FAILURE if the RtkI2CReceive failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
HAL_Status
RtkI2CReceive_Patch(
    IN  VOID *Data
){
    PSAL_I2C_HND        pSalI2CHND          = (PSAL_I2C_HND) Data;
    PSAL_I2C_HND_PRIV   pSalI2CHNDPriv      = NULL;
    PSAL_I2C_MNGT_ADPT  pSalI2CMngtAdpt     = NULL;
    PHAL_I2C_INIT_DAT   pHalI2CInitDat      = NULL;
    PHAL_I2C_OP         pHalI2COP           = NULL;   
#if I2C_DMA_OP_TYPE
    PHAL_GDMA_ADAPTER   pHalI2CRxGdmaAdpt   = NULL;
    PHAL_GDMA_OP        pHalI2CGdmaOp       = NULL;
#endif
    PSAL_I2C_USER_CB    pSalI2CUserCB       = NULL;
    
    u32 I2CLocalTemp = 0;
    u32 I2CInTOTcnt = 0;
    u32 InTimeoutCount = 0;
    u32 InStartCount = 0;
    u32 I2CLocalLen = 0;
    u32 I2CChkRawSts = 0;
    u32 I2CChkRawSts2 = 0;
    u32 I2CDataLenBak = 0;
    u32 I2CDataPtrBak = 0;
    u32 I2CInTOTcntRty      = 0;
    u32 InTimeoutCountRty   = 0;
    u32 InStartCountRty     = 0;
    //u32 I2CInTOTcntIntr = 0;
    //u32 InTimeoutCountIntr = 0;
    //u32 InStartCountIntr = 0;
    u32 I2CMtrTtyCnt    = 0;
    
    /*To Get the SAL_I2C_MNGT_ADPT Pointer*/
    pSalI2CHNDPriv  = CONTAINER_OF(pSalI2CHND, SAL_I2C_HND_PRIV, SalI2CHndPriv);
    pSalI2CMngtAdpt = CONTAINER_OF(pSalI2CHNDPriv->ppSalI2CHnd, SAL_I2C_MNGT_ADPT, pSalHndPriv);
    pHalI2CInitDat  = pSalI2CMngtAdpt->pHalInitDat;
    pHalI2COP       = pSalI2CMngtAdpt->pHalOp;
    pSalI2CUserCB   = pSalI2CHND->pUserCB;
#if I2C_DMA_OP_TYPE
    pHalI2CRxGdmaAdpt   = pSalI2CMngtAdpt->pHalRxGdmaAdp;
    pHalI2CGdmaOp       = pSalI2CMngtAdpt->pHalGdmaOp;
#endif
    
    if (pSalI2CHND->I2CMaster == I2C_MASTER_MODE)/*if (pSalI2CHND->I2CMaster == I2C_MASTER_MODE)*/
    {
        I2CMtrTtyCnt = 0;
        /* Master run-time update  target address */
        if (pSalI2CHND->I2CExd & I2C_EXD_MTR_ADDR_UPD) {
            /* Calculate user time out parameters */
            I2CInTOTcnt = pSalI2CHND->TimeOut;
            if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
                InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
                InStartCount = HalTimerOp.HalTimerReadCount(1);
            }
            
            /* Check Master activity status */
            while ((pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS)) & BIT_IC_STATUS_MST_ACTIVITY) {  
                pHalI2COP->HalI2CClrAllIntr(pHalI2CInitDat);
                /* Time-Out check */
                if (InTimeoutCount > 0) {
                    if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
                        pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                        pSalI2CHND->ErrType = I2C_ERR_RX_ADD_TO;
                        DBG_I2C_ERR("RtkI2CReceive Timeout, I2C%2x,1\n",pSalI2CHND->DevNum);
                        DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                        return HAL_TIMEOUT;
                    }
                }
                else {
                    if (I2CInTOTcnt == 0) {
                        pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                        pSalI2CHND->ErrType = I2C_ERR_RX_ADD_TO;
                        DBG_I2C_ERR("RtkI2CReceive Timeout, I2C%2x,2\n",pSalI2CHND->DevNum);
                        DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                        return HAL_TIMEOUT;
                    }
                }
            }

            /* Calculate user time out parameters */
            I2CInTOTcnt = pSalI2CHND->TimeOut;
            if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
                InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
                InStartCount = HalTimerOp.HalTimerReadCount(1);
            }

            /* Check TX FIFO status */
            while (!((pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS)) & BIT_IC_STATUS_TFE)) {                
                /* Time-Out check */
                if (InTimeoutCount > 0) {
                    if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
                        pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                        pSalI2CHND->ErrType = I2C_ERR_RX_ADD_TO;
                        DBG_I2C_ERR("RtkI2CReceive Timeout, I2C%2x,3\n",pSalI2CHND->DevNum);
                        DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                        return HAL_TIMEOUT;
                    }
                }
                else {
                    if (I2CInTOTcnt == 0) {
                        pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                        pSalI2CHND->ErrType = I2C_ERR_RX_ADD_TO;
                        DBG_I2C_ERR("RtkI2CReceive Timeout, I2C%2x,4\n",pSalI2CHND->DevNum);
                        DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                        return HAL_TIMEOUT;
                    }
                }
            }

            I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_TAR);
            I2CLocalTemp &= (~BIT_MASK_IC_TAR);
            I2CLocalTemp |= BIT_CTRL_IC_TAR(pSalI2CHND->pRXBuf->TargetAddr);
            /* Update Master Target address */
            pHalI2COP->HalI2CWriteReg(pHalI2CInitDat, REG_DW_I2C_IC_TAR, I2CLocalTemp);
        }


#if I2C_USER_REG_ADDR   /*I2C_USER_REG_ADDR*/ 
        RtkI2CSendUserAddr(pSalI2CHND, 1);        
#endif                  /*I2C_USER_REG_ADDR*/

#if I2C_POLL_OP_TYPE/*I2C_POLL_OP_TYPE*/
        if (pSalI2CHND->OpType == I2C_POLL_TYPE)
        {
            /* I2C Device Status Update */
            pSalI2CHND->DevSts  = I2C_STS_RX_READY;
            
            pSalI2CMngtAdpt->MstRDCmdCnt = pSalI2CHND->pRXBuf->DataLen;
            I2CLocalTemp = pSalI2CHND->pRXBuf->DataLen;
            
            /* Calculate user time out parameters */
            I2CInTOTcnt = pSalI2CHND->TimeOut;
            if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
                InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
                InStartCount = HalTimerOp.HalTimerReadCount(1);
            }
            
            /* Receive data till the RX buffer data length is zero */
            for ( ;pSalI2CHND->pRXBuf->DataLen>0; ) {
SEND_I2C_RD_CMD:
                /* I2C Device Status Update */
                pSalI2CHND->DevSts  = I2C_STS_RX_ING;
                /* Check I2C TX FIFO status. If it's NOT full, a Read command is written
                            into the TX FIFO.*/
                if (pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS)
                        & BIT_IC_STATUS_TFNF) {
                    if (pSalI2CMngtAdpt->MstRDCmdCnt > 0) {
                        pHalI2CInitDat->I2CCmd    = I2C_READ_CMD;
                        pHalI2CInitDat->I2CDataLen= 1;
                        pHalI2CInitDat->I2CRWData = pSalI2CHND->pRXBuf->pDataBuf;
                        pHalI2CInitDat->I2CStop   = I2C_STOP_DIS;
                        if ((pSalI2CMngtAdpt->MstRDCmdCnt == 1) && ((pSalI2CHND->I2CExd & I2C_EXD_MTR_HOLD_BUS) == 0)){
                            pHalI2CInitDat->I2CStop   = I2C_STOP_EN; 
                        }
                        pHalI2COP->HalI2CMassSend(pHalI2CInitDat);
                        if (pSalI2CMngtAdpt->MstRDCmdCnt > 0)
                            pSalI2CMngtAdpt->MstRDCmdCnt--;
                    }                    
                }

                if (I2CLocalTemp == pSalI2CHND->pRXBuf->DataLen){
                  if (pSalI2CHND->I2CExd & I2C_EXD_MTR_ADDR_RTY) {
                        HalDelayUs(((1000*30)/pHalI2CInitDat->I2CClk));    //the 10 is for ten bit time      
                        I2CChkRawSts = pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_RAW_INTR_STAT);
                        if (I2CChkRawSts & BIT_IC_RAW_INTR_STAT_TX_ABRT) {
                            I2CMtrTtyCnt++;
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_TX_ABRT_SOURCE);
    	                    if (I2CMtrTtyCnt > I2C_MTR_RTY_CNT) {
                                if ((I2CChkRawSts & (BIT_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK | 
                                              BIT_IC_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK |
                                              BIT_IC_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK)) != 0) {
                                    pSalI2CHND->ErrType |= I2C_ERR_MST_A_NACK;
                                }
                                else if ((I2CChkRawSts & BIT_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK) != 0){
                                    pSalI2CHND->ErrType |= I2C_ERR_MST_D_NACK;
                                }
                                else {
                                    pSalI2CHND->ErrType |= I2C_ERR_TX_ABRT;
                                }
                                pSalI2CHND->DevSts  = I2C_STS_ERROR;
                                if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                                return HAL_ERR_HW;
    	                    }
                            pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_CLR_TX_ABRT); 
                            pSalI2CMngtAdpt->MstRDCmdCnt++;
                            goto SEND_I2C_RD_CMD;
                        }
                  }                    
                }
                               
                /* Contineously poll the I2C RX FIFO status */
                while (1) {
                    if ((pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS)
                        & (BIT_IC_STATUS_RFNE | BIT_IC_STATUS_RFF)) != 0) {
                        *(pSalI2CHND->pRXBuf->pDataBuf) = 
                                        pHalI2COP->HalI2CReceive(pHalI2CInitDat);
                        
                        pSalI2CHND->pRXBuf->pDataBuf++;
                        pSalI2CHND->pRXBuf->DataLen--;

                        if (!pSalI2CHND->pRXBuf->DataLen) {
                            break;
                        }
                    }
                    else {
                        break;
                    }
                }

                /* Time-Out check */
                if (InTimeoutCount > 0) {
                    if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
                        pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                        pSalI2CHND->ErrType = I2C_ERR_RX_FF_TO;
                        DBG_I2C_ERR("RtkI2CReceive Timeout, I2C%2x,5\n",pSalI2CHND->DevNum);
                        DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                        return HAL_TIMEOUT;
                    }
                }
                else {
                    if (I2CInTOTcnt == 0) {
                        pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                        pSalI2CHND->ErrType = I2C_ERR_RX_FF_TO;
                        DBG_I2C_ERR("RtkI2CReceive Timeout, I2C%2x,6\n",pSalI2CHND->DevNum);
                        DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                        return HAL_TIMEOUT;
                    }
                }

            }   

            /* I2C Device Status Update */
            pSalI2CHND->DevSts = I2C_STS_IDLE;
        }       
#endif/*I2C_POLL_OP_TYPE*/    

#if I2C_INTR_OP_TYPE/*I2C_INTR_OP_TYPE*/
        if (pSalI2CHND->OpType == I2C_INTR_TYPE) {
            /* Calculate user time out parameters */
            InTimeoutCount= 0;
            InStartCount  = 0;
            I2CInTOTcnt = pSalI2CMngtAdpt->InnerTimeOut;

            //InTimeoutCountIntr = 0;
            //InStartCountIntr = 0;            
            //I2CInTOTcntIntr = pSalI2CHND->AddRtyTimeOut;
            
            if ((I2CInTOTcnt!= 0) && (I2CInTOTcnt!= I2C_TIMEOOUT_ENDLESS)) {
                InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
                InStartCount= HalTimerOp.HalTimerReadCount(1);
            }

            //InTimeoutCountIntr = (I2CInTOTcntIntr*1000/TIMER_TICK_US);
            //InStartCountIntr = HalTimerOp.HalTimerReadCount(1);
            
            I2CDataLenBak = (u32)(pSalI2CHND->pRXBuf->DataLen);
            I2CDataPtrBak = (u32)(pSalI2CHND->pRXBuf->pDataBuf);
            
            /* I2C Device Status Update */
            pSalI2CHND->DevSts = I2C_STS_RX_READY;
            pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_TX_ABRT;
            pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);
            pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_ACTIVITY;
            pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);
            pHalI2COP->HalI2CClrAllIntr(pHalI2CInitDat);

            /* Clear RX FIFO */
            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_RXFLR);
            while (I2CChkRawSts > 0){
                I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_RXFLR);
            }
            
            /* To fill the Master Read Command into TX FIFO */
            pSalI2CMngtAdpt->MstRDCmdCnt = pSalI2CHND->pRXBuf->DataLen;
            I2CLocalLen = 2;//pSalI2CHND->pRXBuf->DataLen;
            pSalI2CHND->DevSts = I2C_STS_RX_READY;

            
            while (1) {
SEND_I2C_RD_CMD_INTR:
                
                /* Calculate user time out parameters */
    			InTimeoutCountRty   = 0;
    			InStartCountRty     = 0;
                I2CInTOTcntRty      = pSalI2CHND->TimeOut;          
                
                if ((I2CInTOTcntRty != 0) && (I2CInTOTcntRty!= I2C_TIMEOOUT_ENDLESS)) {
                    InTimeoutCountRty= (I2CInTOTcntRty*1000/TIMER_TICK_US);
                    InStartCountRty= HalTimerOp.HalTimerReadCount(1);
                }
                I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS);

                if ((pSalI2CMngtAdpt->MstRDCmdCnt > 0) && (I2CLocalLen > 0)){
                    pHalI2CInitDat->I2CCmd    = I2C_READ_CMD;
                    pHalI2CInitDat->I2CDataLen= 1;
                    pHalI2CInitDat->I2CRWData = pSalI2CHND->pRXBuf->pDataBuf;
                    pHalI2CInitDat->I2CStop   = I2C_STOP_DIS;

                    if ((pSalI2CMngtAdpt->MstRDCmdCnt == 1) && ((pSalI2CHND->I2CExd & I2C_EXD_MTR_HOLD_BUS) == 0)){
                        pHalI2CInitDat->I2CStop   = I2C_STOP_EN;
                    }

                    if ((pHalI2CInitDat->I2CReSTR == 1) && (I2CLocalLen == 2)) {
                        pHalI2CInitDat->RSVD0 |= BIT0;
                    }
                    
                    pHalI2COP->HalI2CMassSend(pHalI2CInitDat);               

                    if ((pHalI2CInitDat->I2CReSTR == 1) && (I2CLocalLen == 2)) {
                        pHalI2CInitDat->RSVD0 &= (~BIT0);
                    }
                }                  

                if (pSalI2CHND->I2CExd & I2C_EXD_MTR_ADDR_RTY) {

                    /* SEND_I2C_WR_CMD_INTR Time-Out check */
                    //if (InTimeoutCountIntr > 0) {
                    //    if (HAL_TIMEOUT == I2CIsTimeout(InStartCountIntr, InTimeoutCountIntr)) {
                    //        return HAL_TIMEOUT;
                    //    }
                    //}
                    
                    I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS);
                    while ((I2CChkRawSts & BIT_IC_STATUS_TFE) == 0) {
                        I2CChkRawSts2 = pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_RAW_INTR_STAT);
                        if ((I2CChkRawSts2 & BIT_IC_RAW_INTR_STAT_TX_ABRT) != 0){
                            break;
                        }

                        /* Time-Out check */
                        if (InTimeoutCountRty > 0) {
                            if (HAL_TIMEOUT == I2CIsTimeout(InStartCountRty, InTimeoutCountRty)) {
                                break;
                            }
                        }
                        else {
                            if (I2CInTOTcntRty == 0) {
                                break;
                            }
                        }
                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS);
                        
                    }
                    
                    HalDelayUs(((1000*30)/pHalI2CInitDat->I2CClk));    //the 10 is for ten bit time
                    
                    I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_RAW_INTR_STAT);
                    I2CChkRawSts2 = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS);
	                if (I2CChkRawSts & BIT_IC_RAW_INTR_STAT_TX_ABRT) {
                        I2CMtrTtyCnt++;
	                    I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_TX_ABRT_SOURCE);
	                    if (I2CMtrTtyCnt > I2C_MTR_RTY_CNT) {
                            if ((I2CChkRawSts & (BIT_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK | 
                                              BIT_IC_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK |
                                              BIT_IC_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK)) != 0) {
                                pSalI2CHND->ErrType |= I2C_ERR_MST_A_NACK;
                            }
                            else if ((I2CChkRawSts & BIT_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK) != 0){
                                pSalI2CHND->ErrType |= I2C_ERR_MST_D_NACK;
                            }
                            else {
                                pSalI2CHND->ErrType |= I2C_ERR_TX_ABRT;
                            }

                            pSalI2CHND->DevSts  = I2C_STS_ERROR;
                            if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                            return HAL_ERR_HW;
	                    }
                        
	                    /* use InTimeoutCount to backup tar here */
                        InTimeoutCount = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_TAR);
                        InTimeoutCount = BIT_GET_IC_TAR(InTimeoutCount);
                        
                        RtkI2CDeInitForPS(pSalI2CHND);
                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        while((I2CChkRawSts & BIT_IC_ENABLE_STATUS_IC_EN) == BIT_IC_ENABLE_STATUS_IC_EN){
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        }

                        
                        
                        RtkI2CInitForPS(pSalI2CHND);

                        pSalI2CHND->pInitDat->I2CAckAddr = InTimeoutCount;
                        HalI2CSetTarRtl8195a(pSalI2CHND->pInitDat);

                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        while((I2CChkRawSts & BIT_IC_ENABLE_STATUS_IC_EN) != BIT_IC_ENABLE_STATUS_IC_EN){
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        }
                        pSalI2CHND->DevSts  = I2C_STS_RX_READY;
                        pSalI2CHND->ErrType = 0;
                        pSalI2CHND->pRXBuf->DataLen = (u16)I2CDataLenBak;
                        pSalI2CHND->pRXBuf->pDataBuf= (u8 *)I2CDataPtrBak;

                        /* Calculate user time out parameters */
                        InTimeoutCount  = 0;
                        InStartCount    = 0;
                        I2CInTOTcnt = pSalI2CMngtAdpt->InnerTimeOut;
                        if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
                            InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
                            InStartCount = HalTimerOp.HalTimerReadCount(1);
                        }
                        goto SEND_I2C_RD_CMD_INTR;
                    }
                    else if ((I2CChkRawSts2 & BIT_IC_STATUS_TFE) != BIT_IC_STATUS_TFE){
                        I2CMtrTtyCnt++;
	                    I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_TX_ABRT_SOURCE);
	                    if (I2CMtrTtyCnt > I2C_MTR_RTY_CNT) {
                            if ((I2CChkRawSts & (BIT_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK | 
                                              BIT_IC_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK |
                                              BIT_IC_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK)) != 0) {
                                pSalI2CHND->ErrType |= I2C_ERR_MST_A_NACK;
                            }
                            else if ((I2CChkRawSts & BIT_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK) != 0){
                                pSalI2CHND->ErrType |= I2C_ERR_MST_D_NACK;
                            }
                            else {
                                pSalI2CHND->ErrType |= I2C_ERR_TX_ABRT;
                            }
                            pSalI2CHND->DevSts  = I2C_STS_ERROR;
                            if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                            return HAL_ERR_HW;
	                    }

                        /* use InTimeoutCount to backup tar here */
                        InTimeoutCount = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_TAR);
                        InTimeoutCount = BIT_GET_IC_TAR(InTimeoutCount);
                        
                        RtkI2CDeInitForPS(pSalI2CHND);
                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        while((I2CChkRawSts & BIT_IC_ENABLE_STATUS_IC_EN) == BIT_IC_ENABLE_STATUS_IC_EN){
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        }

                        
                        
                        RtkI2CInitForPS(pSalI2CHND);

                        pSalI2CHND->pInitDat->I2CAckAddr = InTimeoutCount;
                        HalI2CSetTarRtl8195a(pSalI2CHND->pInitDat);

                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        while((I2CChkRawSts & BIT_IC_ENABLE_STATUS_IC_EN) != BIT_IC_ENABLE_STATUS_IC_EN){
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        }
                        pSalI2CHND->DevSts  = I2C_STS_RX_READY;
                        pSalI2CHND->ErrType = 0;
                        pSalI2CHND->pRXBuf->DataLen = (u16)I2CDataLenBak;
                        pSalI2CHND->pRXBuf->pDataBuf= (u8 *)I2CDataPtrBak;

                        /* Calculate user time out parameters */
                        InTimeoutCount  = 0;
                        InStartCount    = 0;
                        I2CInTOTcnt = pSalI2CMngtAdpt->InnerTimeOut;
                        if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
                            InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
                            InStartCount = HalTimerOp.HalTimerReadCount(1);
                        }
                        goto SEND_I2C_RD_CMD_INTR;
                    }
                    else {
                        I2CChkRawSts2 = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS);
                        {
                            if (I2CLocalLen>0){
                                I2CLocalLen--;
                                pSalI2CMngtAdpt->MstRDCmdCnt --;
                            }
                        }
                    }
                }               
                else {
                    if (I2CLocalLen>0) {
                        I2CLocalLen--;  
                        pSalI2CMngtAdpt->MstRDCmdCnt --;
                    }
                }

                if ((I2CLocalLen == 0) || (pSalI2CHND->pRXBuf->DataLen == 1)){
                    pHalI2CInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_TX_ABRT;
                    pHalI2COP->HalI2CClrIntr(pHalI2CInitDat);
                    I2CLocalTemp = 0;
                    I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_INTR_MASK);
                    I2CLocalTemp |= (BIT_IC_INTR_MASK_M_RX_FULL |
                                     BIT_IC_INTR_MASK_M_RX_OVER |
                                     BIT_IC_INTR_MASK_M_RX_UNDER|BIT_IC_INTR_MASK_M_TX_ABRT);
                    pHalI2CInitDat->I2CIntrMSK = I2CLocalTemp;
                    pHalI2COP->HalI2CIntrCtrl(pHalI2CInitDat);  
                    break;
                }

                /* Time-Out check */
                if (InTimeoutCount > 0) {
                    if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
                        pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                        pSalI2CHND->ErrType = I2C_ERR_TX_FF_TO;
                        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);

                        /* use InTimeoutCount to backup tar here */
                        InTimeoutCount = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_TAR);
                        InTimeoutCount = BIT_GET_IC_TAR(InTimeoutCount);
                        
                        RtkI2CDeInitForPS(pSalI2CHND);
                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        while((I2CChkRawSts & BIT_IC_ENABLE_STATUS_IC_EN) == BIT_IC_ENABLE_STATUS_IC_EN){
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        }

                        
                        
                        RtkI2CInitForPS(pSalI2CHND);

                        pSalI2CHND->pInitDat->I2CAckAddr = InTimeoutCount;
                        HalI2CSetTarRtl8195a(pSalI2CHND->pInitDat);
                        
                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        while((I2CChkRawSts & BIT_IC_ENABLE_STATUS_IC_EN) != BIT_IC_ENABLE_STATUS_IC_EN){
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        }
                        return HAL_TIMEOUT;
                    }
                }
                else {
                    if (I2CInTOTcnt == 0) {
                        pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                        pSalI2CHND->ErrType = I2C_ERR_TX_FF_TO;
                        if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);

                        /* use InTimeoutCount to backup tar here */
                        InTimeoutCount = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_TAR);
                        InTimeoutCount = BIT_GET_IC_TAR(InTimeoutCount);
                        
                        RtkI2CDeInitForPS(pSalI2CHND);
                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        while((I2CChkRawSts & BIT_IC_ENABLE_STATUS_IC_EN) == BIT_IC_ENABLE_STATUS_IC_EN){
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        }

                        
                        
                        RtkI2CInitForPS(pSalI2CHND);

                        pSalI2CHND->pInitDat->I2CAckAddr = InTimeoutCount;
                        HalI2CSetTarRtl8195a(pSalI2CHND->pInitDat);

                        I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        while((I2CChkRawSts & BIT_IC_ENABLE_STATUS_IC_EN) != BIT_IC_ENABLE_STATUS_IC_EN){
                            I2CChkRawSts = (u32)pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_ENABLE_STATUS);
                        }
                        return HAL_TIMEOUT;
                    }
                }
            }

        }
#endif/*I2C_INTR_OP_TYPE*/
    }/*if (pSalI2CHND->I2CMaster == I2C_MASTER_MODE)*/
    else
    {
#if I2C_POLL_OP_TYPE
        if (pSalI2CHND->OpType == I2C_POLL_TYPE) {
            /* Calculate user time out parameters */
            I2CInTOTcnt = pSalI2CHND->TimeOut;
            if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
                InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
                InStartCount = HalTimerOp.HalTimerReadCount(1);
            }
            
            /* Receive data till the RX buffer data length is zero */
            for (;pSalI2CHND->pRXBuf->DataLen>0; ) {
                if ((pHalI2COP->HalI2CReadReg(pHalI2CInitDat,REG_DW_I2C_IC_STATUS)
                        & (BIT_IC_STATUS_RFNE | BIT_IC_STATUS_RFF)) != 0) {                    
                    *(pSalI2CHND->pRXBuf->pDataBuf) = 
                                    pHalI2COP->HalI2CReceive(pHalI2CInitDat);
                    pSalI2CHND->pRXBuf->pDataBuf++;
                    pSalI2CHND->pRXBuf->DataLen--;
                }
                else {
                    /* Time-Out check */
                    if (InTimeoutCount > 0) {
                        if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
                            pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                            pSalI2CHND->ErrType = I2C_ERR_RX_FF_TO;
                            DBG_I2C_ERR("RtkI2CReceive Timeout, I2C%2x,9\n",pSalI2CHND->DevNum);
                            DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                            if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                            return HAL_TIMEOUT;
                        }
                    }
                    else {
                        if (I2CInTOTcnt == 0) {
                            pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                            pSalI2CHND->ErrType = I2C_ERR_RX_FF_TO;
                            DBG_I2C_ERR("RtkI2CReceive Timeout, I2C%2x,10\n",pSalI2CHND->DevNum);
                            DBG_I2C_ERR("DevSts:%x, ErrType:%x\n", pSalI2CHND->DevSts, pSalI2CHND->ErrType);
                            if (pSalI2CUserCB->pERRCB->USERCB != NULL)
                                    pSalI2CUserCB->pERRCB->USERCB((void *)pSalI2CUserCB->pERRCB->USERData);
                            return HAL_TIMEOUT;
                        }
                    }
                }
            }

            /* I2C Device Status Update */
            pSalI2CHND->DevSts = I2C_STS_IDLE;
        }
#endif   

#if I2C_INTR_OP_TYPE/*I2C_INTR_OP_TYPE*/
        if (pSalI2CHND->OpType == I2C_INTR_TYPE) {
            pSalI2CHND->DevSts = I2C_STS_RX_READY;

            /*I2C Enable RX Related Interrupts*/
            I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_INTR_MASK);
            I2CLocalTemp |= (BIT_IC_INTR_MASK_M_RX_FULL |
                             BIT_IC_INTR_MASK_M_RX_OVER |
                             BIT_IC_INTR_MASK_M_RX_UNDER);
            pHalI2CInitDat->I2CIntrMSK = I2CLocalTemp;
            pHalI2COP->HalI2CIntrCtrl(pHalI2CInitDat);
        }
#endif/*I2C_INTR_OP_TYPE*/

#if I2C_DMA_OP_TYPE/*I2C_INTR_OP_TYPE*/
        if (pSalI2CHND->OpType == I2C_DMA_TYPE) {
            pSalI2CHND->DevSts = I2C_STS_RX_READY;

            /*I2C Enable RX Related Interrupts*/
            I2CLocalTemp = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_INTR_MASK);
            I2CLocalTemp |= (BIT_IC_INTR_MASK_M_RX_OVER |
                             BIT_IC_INTR_MASK_M_RX_UNDER);
            pHalI2CInitDat->I2CIntrMSK = I2CLocalTemp;
            pHalI2COP->HalI2CIntrCtrl(pHalI2CInitDat);

            //HalGdmaOpInit(pHalI2CGdmaOp);
            pSalI2CMngtAdpt->pHalGdmaOpInit(pHalI2CGdmaOp);

            pHalI2CRxGdmaAdpt->GdmaCtl.BlockSize    = pSalI2CHND->pRXBuf->DataLen;
            pHalI2CRxGdmaAdpt->ChSar                = (u32)(I2C0_REG_BASE+REG_DW_I2C_IC_DATA_CMD+
                                                                pSalI2CHND->DevNum*0x400);
            pHalI2CRxGdmaAdpt->ChDar                = (u32)pSalI2CHND->pRXBuf->pDataBuf;

            pHalI2CGdmaOp->HalGdmaChSeting(pHalI2CRxGdmaAdpt);
            pHalI2CGdmaOp->HalGdmaChEn(pHalI2CRxGdmaAdpt);
            pSalI2CHND->DevSts  = I2C_STS_RX_ING;
            pHalI2CInitDat->I2CDMACtrl = BIT_CTRL_IC_DMA_CR_RDMAE(1);
            pHalI2COP->HalI2CDMACtrl(pHalI2CInitDat);            
        }
#endif/*I2C_INTR_OP_TYPE*/

    }
    
    return HAL_OK;
}



//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CInitForPS
//
// Description:
//         Add power state registeration for I2C initail process.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C initialization process.
//          HAL_OK if the RtkI2CInitForPS succeeded.
//          HAL_ERR_UNKNOWN if the RtkI2CInitForPS failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2015-05-31.
//
//----------------------------------------------------------------------------------------------------
HAL_Status
RtkI2CInitForPS(
    IN  VOID *Data
){

    u8 i2cInitSts = HAL_OK;
    PSAL_I2C_HND    pSalI2CHND  =   (PSAL_I2C_HND) Data;
#ifdef CONFIG_SOC_PS_MODULE
    REG_POWER_STATE i2cPwrState;
#endif

    /* Check the input I2C index first */
    if (FunctionChk((I2C0 + pSalI2CHND->DevNum), pSalI2CHND->PinMux) == _FALSE)
        return HAL_ERR_UNKNOWN;
        
    i2cInitSts = RtkI2CInit(pSalI2CHND);
    
#ifdef CONFIG_SOC_PS_MODULE
    // To register a new peripheral device power state
    if (i2cInitSts == HAL_OK){
        i2cPwrState.FuncIdx = I2C0 + pSalI2CHND->DevNum;
        i2cPwrState.PwrState = ACT;
        RegPowerState(i2cPwrState);
    }    
#endif

    return i2cInitSts;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CDeInitForPS
//
// Description:
//         Add power state registeration for I2C deinitail process.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C deinitialization process.
//          HAL_OK if the RtkI2CDeInitForPS succeeded.
//          HAL_ERR_UNKNOWN if the RtkI2CDeInitForPS failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2015-05-31.
//
//----------------------------------------------------------------------------------------------------
HAL_Status
RtkI2CDeInitForPS(
    IN  VOID *Data
){
    u8 i2cInitSts = HAL_OK;
    PSAL_I2C_HND    pSalI2CHND  =   (PSAL_I2C_HND) Data;
#ifdef CONFIG_SOC_PS_MODULE
    REG_POWER_STATE i2cPwrState;
    u8 HwState;
#endif

    if (pSalI2CHND == NULL)
        return HAL_ERR_UNKNOWN;
        
    /* Check the input I2C index first */
    if (RtkI2CIdxChk(pSalI2CHND->DevNum))
        return HAL_ERR_UNKNOWN;

#ifdef CONFIG_SOC_PS_MODULE
    i2cPwrState.FuncIdx = I2C0 + pSalI2CHND->DevNum;
        
    QueryRegPwrState(i2cPwrState.FuncIdx, &(i2cPwrState.PwrState), &HwState);
        
    // if the power state isn't ACT, then switch the power state back to ACT first
    if ((i2cPwrState.PwrState != ACT) && (i2cPwrState.PwrState != INACT)) {
        RtkI2CEnablePS(Data);
        QueryRegPwrState(i2cPwrState.FuncIdx, &(i2cPwrState.PwrState), &HwState);
    }

    if (i2cPwrState.PwrState == ACT) {
        i2cPwrState.PwrState = INACT;
        RegPowerState(i2cPwrState);
    }         
#endif

    i2cInitSts = RtkI2CDeInit(pSalI2CHND);

    return i2cInitSts;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CDisablePS
//
// Description:
//         I2C disable opertion by setting clock disable.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C disable process.
//          HAL_OK if the RtkI2CDisablePS succeeded.
//          HAL_ERR_PARA if the RtkI2CDisablePS failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2015-05-31.
//
//----------------------------------------------------------------------------------------------------
HAL_Status
RtkI2CDisablePS(
    IN  VOID *Data
){

    PSAL_I2C_HND    pSalI2CHND  =   (PSAL_I2C_HND) Data;
    u8 i2cIdx = pSalI2CHND->DevNum;
#ifdef CONFIG_SOC_PS_MODULE
    REG_POWER_STATE i2cPwrState;
#endif

    if (RtkI2CIdxChk(i2cIdx))
        return HAL_ERR_UNKNOWN;
        
    switch (i2cIdx) {
        case 0:
        {
            /* I2C 0 */
            ACTCK_I2C0_CCTRL(OFF);
            SLPCK_I2C0_CCTRL(OFF);
            break;
        }
        case 1:
        {
            /* I2C 1 */
            ACTCK_I2C1_CCTRL(OFF);
            SLPCK_I2C1_CCTRL(OFF);
            break;
        }
        case 2:
        {
            /* I2C 2 */
            ACTCK_I2C2_CCTRL(OFF);
            SLPCK_I2C2_CCTRL(OFF);
            break;
        }
        case 3:
        {
            /* I2C 3 */
            ACTCK_I2C3_CCTRL(OFF);
            SLPCK_I2C3_CCTRL(OFF);
            break;
        }
        default:
        {
            return HAL_ERR_PARA;
        }
    }

#ifdef CONFIG_SOC_PS_MODULE
    // To register a new peripheral device power state
    i2cPwrState.FuncIdx = I2C0 + pSalI2CHND->DevNum;
    i2cPwrState.PwrState = SLPCG;   
    RegPowerState(i2cPwrState);
#endif

    return HAL_OK;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CEnablePS
//
// Description:
//         I2C enable opertion by setting clock enable.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C enable process.
//          HAL_OK if the RtkI2CEnablePS succeeded.
//          HAL_ERR_PARA if the RtkI2CEnablePS failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2015-05-31.
//
//----------------------------------------------------------------------------------------------------
HAL_Status
RtkI2CEnablePS(
    IN  VOID *Data
){

    PSAL_I2C_HND    pSalI2CHND  =   (PSAL_I2C_HND) Data;
    u8 i2cIdx = pSalI2CHND->DevNum;
#ifdef CONFIG_SOC_PS_MODULE
    REG_POWER_STATE i2cPwrState;
#endif

    if (RtkI2CIdxChk(i2cIdx))
        return HAL_ERR_UNKNOWN;
        
    switch (i2cIdx) {
        case 0:
        {
            /* I2C 0 */
            ACTCK_I2C0_CCTRL(ON);
            SLPCK_I2C0_CCTRL(ON);
            break;
        }
        case 1:
        {
            /* I2C 1 */
            ACTCK_I2C1_CCTRL(ON);
            SLPCK_I2C1_CCTRL(ON);
            break;
        }
        case 2:
        {
            /* I2C 2 */
            ACTCK_I2C2_CCTRL(ON);
            SLPCK_I2C2_CCTRL(ON);
            break;
        }
        case 3:
        {
            /* I2C 3 */
            ACTCK_I2C3_CCTRL(ON);
            SLPCK_I2C3_CCTRL(ON);
            break;
        }
        default:
        {
            return HAL_ERR_PARA;
        }
    }

#ifdef CONFIG_SOC_PS_MODULE
    // To register a new peripheral device power state
    i2cPwrState.FuncIdx = I2C0 + pSalI2CHND->DevNum;
    i2cPwrState.PwrState = ACT;   
    RegPowerState(i2cPwrState);
#endif

    return HAL_OK;
}


#ifndef CONFIG_MBED_ENABLED
//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CGetMngtAdpt
//
// Description:
//		According to the input index, all the memory space are allocated and all the 
//          related pointers are assigned. The management adapter pointer will be 
//          returned.
//
// Arguments:
//		[in] u8  I2CIdx -
//			I2C module index
//
// Return:
//		PSAL_I2C_MNGT_ADPT
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-02.
//
//--------------------------------------------------------------------------------------------------- 
PSAL_I2C_MNGT_ADPT
RtkI2CGetMngtAdpt(
    IN  u8  I2CIdx
){
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_USERCB_ADPT    pSalI2CUserCBAdpt   = NULL;

    /* If the kernel is available, Memory-allocation is used. */
#ifndef I2C_STATIC_ALLOC

    pSalI2CMngtAdpt = (PSAL_I2C_MNGT_ADPT)RtlZmalloc(sizeof(SAL_I2C_MNGT_ADPT));
    pSalI2CMngtAdpt->pSalHndPriv    = (PSAL_I2C_HND_PRIV)RtlZmalloc(sizeof(SAL_I2C_HND_PRIV));
    pSalI2CMngtAdpt->pHalInitDat    = (PHAL_I2C_INIT_DAT)RtlZmalloc(sizeof(HAL_I2C_INIT_DAT));
    pSalI2CMngtAdpt->pHalOp         = (PHAL_I2C_OP)RtlZmalloc(sizeof(HAL_I2C_OP));
    pSalI2CMngtAdpt->pIrqHnd        = (PIRQ_HANDLE)RtlZmalloc(sizeof(IRQ_HANDLE));
    pSalI2CMngtAdpt->pHalTxGdmaAdp  = (PHAL_GDMA_ADAPTER)RtlZmalloc(sizeof(HAL_GDMA_ADAPTER));
    pSalI2CMngtAdpt->pHalRxGdmaAdp  = (PHAL_GDMA_ADAPTER)RtlZmalloc(sizeof(HAL_GDMA_ADAPTER));
    pSalI2CMngtAdpt->pHalGdmaOp     = (PHAL_GDMA_OP)RtlZmalloc(sizeof(HAL_GDMA_OP));
    pSalI2CMngtAdpt->pIrqTxGdmaHnd  = (PIRQ_HANDLE)RtlZmalloc(sizeof(IRQ_HANDLE));
    pSalI2CMngtAdpt->pIrqRxGdmaHnd  = (PIRQ_HANDLE)RtlZmalloc(sizeof(IRQ_HANDLE));
    pSalI2CMngtAdpt->pUserCB        = (PSAL_I2C_USER_CB)RtlZmalloc(sizeof(SAL_I2C_USER_CB));
    pSalI2CMngtAdpt->pDMAConf       = (PSAL_I2C_DMA_USER_DEF)RtlZmalloc(sizeof(SAL_I2C_DMA_USER_DEF));
    pSalI2CUserCBAdpt               = (PSAL_I2C_USERCB_ADPT)RtlZmalloc((sizeof(SAL_I2C_USERCB_ADPT)*SAL_USER_CB_NUM));
#else
    switch (I2CIdx){
        case I2C0_SEL:
        {
            pSalI2CMngtAdpt = &SalI2C0MngtAdpt;
            pSalI2CMngtAdpt->pSalHndPriv    = &SalI2C0HndPriv;
            pSalI2CMngtAdpt->pHalInitDat    = &HalI2C0InitData;
            pSalI2CMngtAdpt->pHalOp         = &HalI2COpSAL;
            pSalI2CMngtAdpt->pIrqHnd        = &I2C0IrqHandleDat;
            pSalI2CMngtAdpt->pHalTxGdmaAdp  = &HalI2C0TxGdmaAdpt;
            pSalI2CMngtAdpt->pHalRxGdmaAdp  = &HalI2C0RxGdmaAdpt;
            pSalI2CMngtAdpt->pHalGdmaOp     = &HalI2C0GdmaOp;
            pSalI2CMngtAdpt->pIrqTxGdmaHnd  = &I2C0TxGdmaIrqHandleDat;
            pSalI2CMngtAdpt->pIrqRxGdmaHnd  = &I2C0RxGdmaIrqHandleDat;
            pSalI2CMngtAdpt->pUserCB        = &SalI2C0UserCB;
            pSalI2CMngtAdpt->pDMAConf       = &SalI2C0DmaUserDef;
            pSalI2CUserCBAdpt               = (PSAL_I2C_USERCB_ADPT)&SalI2C0UserCBAdpt;
            break;
        }
        
        case I2C1_SEL:
        {
            pSalI2CMngtAdpt = &SalI2C1MngtAdpt;
            pSalI2CMngtAdpt->pSalHndPriv    = &SalI2C1HndPriv;
            pSalI2CMngtAdpt->pHalInitDat    = &HalI2C1InitData;
            pSalI2CMngtAdpt->pHalOp         = &HalI2COpSAL;
            pSalI2CMngtAdpt->pIrqHnd        = &I2C1IrqHandleDat;
            pSalI2CMngtAdpt->pHalTxGdmaAdp  = &HalI2C1TxGdmaAdpt;
            pSalI2CMngtAdpt->pHalRxGdmaAdp  = &HalI2C1RxGdmaAdpt;
            pSalI2CMngtAdpt->pHalGdmaOp     = &HalI2C1GdmaOp;
            pSalI2CMngtAdpt->pIrqTxGdmaHnd  = &I2C1TxGdmaIrqHandleDat;
            pSalI2CMngtAdpt->pIrqRxGdmaHnd  = &I2C1RxGdmaIrqHandleDat;
            pSalI2CMngtAdpt->pUserCB        = &SalI2C1UserCB;
            pSalI2CMngtAdpt->pDMAConf       = &SalI2C1DmaUserDef;
            pSalI2CUserCBAdpt               = (PSAL_I2C_USERCB_ADPT)&SalI2C1UserCBAdpt;
            break;
        }
        
        case I2C2_SEL:
        {
            pSalI2CMngtAdpt = &SalI2C2MngtAdpt;
            pSalI2CMngtAdpt->pSalHndPriv    = &SalI2C2HndPriv;
            pSalI2CMngtAdpt->pHalInitDat    = &HalI2C2InitData;
            pSalI2CMngtAdpt->pHalOp         = &HalI2COpSAL;
            pSalI2CMngtAdpt->pIrqHnd        = &I2C2IrqHandleDat;
            pSalI2CMngtAdpt->pHalTxGdmaAdp  = &HalI2C2TxGdmaAdpt;
            pSalI2CMngtAdpt->pHalRxGdmaAdp  = &HalI2C2RxGdmaAdpt;
            pSalI2CMngtAdpt->pHalGdmaOp     = &HalI2C2GdmaOp;
            pSalI2CMngtAdpt->pIrqTxGdmaHnd  = &I2C2TxGdmaIrqHandleDat;
            pSalI2CMngtAdpt->pIrqRxGdmaHnd  = &I2C2RxGdmaIrqHandleDat;
            pSalI2CMngtAdpt->pUserCB        = &SalI2C2UserCB;
            pSalI2CMngtAdpt->pDMAConf       = &SalI2C2DmaUserDef;
            pSalI2CUserCBAdpt               = (PSAL_I2C_USERCB_ADPT)&SalI2C2UserCBAdpt;
            break;
        }
        
        case I2C3_SEL:
        {
            pSalI2CMngtAdpt = &SalI2C3MngtAdpt;
            pSalI2CMngtAdpt->pSalHndPriv    = &SalI2C3HndPriv;
            pSalI2CMngtAdpt->pHalInitDat    = &HalI2C3InitData;
            pSalI2CMngtAdpt->pHalOp         = &HalI2COpSAL;
            pSalI2CMngtAdpt->pIrqHnd        = &I2C3IrqHandleDat;
            pSalI2CMngtAdpt->pHalTxGdmaAdp  = &HalI2C3TxGdmaAdpt;
            pSalI2CMngtAdpt->pHalRxGdmaAdp  = &HalI2C3RxGdmaAdpt;
            pSalI2CMngtAdpt->pHalGdmaOp     = &HalI2C3GdmaOp;
            pSalI2CMngtAdpt->pIrqTxGdmaHnd  = &I2C3TxGdmaIrqHandleDat;
            pSalI2CMngtAdpt->pIrqRxGdmaHnd  = &I2C3RxGdmaIrqHandleDat;
            pSalI2CMngtAdpt->pUserCB        = &SalI2C3UserCB;
            pSalI2CMngtAdpt->pDMAConf       = &SalI2C3DmaUserDef;
            pSalI2CUserCBAdpt               = (PSAL_I2C_USERCB_ADPT)&SalI2C3UserCBAdpt;            
            break;
        }
        
        default:
            break;
    }
#endif

    /*To assign user callback pointers*/
    pSalI2CMngtAdpt->pUserCB->pTXCB     = pSalI2CUserCBAdpt;
    pSalI2CMngtAdpt->pUserCB->pTXCCB    = (pSalI2CUserCBAdpt+1);
    pSalI2CMngtAdpt->pUserCB->pRXCB     = (pSalI2CUserCBAdpt+2);
    pSalI2CMngtAdpt->pUserCB->pRXCCB    = (pSalI2CUserCBAdpt+3);
    pSalI2CMngtAdpt->pUserCB->pRDREQCB  = (pSalI2CUserCBAdpt+4);
    pSalI2CMngtAdpt->pUserCB->pERRCB    = (pSalI2CUserCBAdpt+5);
    pSalI2CMngtAdpt->pUserCB->pDMATXCB  = (pSalI2CUserCBAdpt+6);
    pSalI2CMngtAdpt->pUserCB->pDMATXCCB = (pSalI2CUserCBAdpt+7);
    pSalI2CMngtAdpt->pUserCB->pDMARXCB  = (pSalI2CUserCBAdpt+8);
    pSalI2CMngtAdpt->pUserCB->pDMARXCCB = (pSalI2CUserCBAdpt+9);
    pSalI2CMngtAdpt->pUserCB->pGENCALLCB= (pSalI2CUserCBAdpt+10);
    
    /*To assign the rest pointers*/
    pSalI2CMngtAdpt->MstRDCmdCnt    = 0;
    pSalI2CMngtAdpt->InnerTimeOut   = 2000; // inner time-out count, 2000 ms
    pSalI2CMngtAdpt->pSalHndPriv->ppSalI2CHnd = (void**)&(pSalI2CMngtAdpt->pSalHndPriv);

    /* To assign the default (ROM) HAL OP initialization function */
#if  defined(CONFIG_CHIP_A_CUT) || defined(CONFIG_CHIP_B_CUT) || defined(CONFIG_CHIP_C_CUT)
    pSalI2CMngtAdpt->pHalOpInit     = HalI2COpInit_Patch;
#elif defined(CONFIG_CHIP_E_CUT)
    // TODO: E-Cut
    pSalI2CMngtAdpt->pHalOpInit     = HalI2COpInit_V04;
#endif

    /* To assign the default (ROM) HAL GDMA OP initialization function */
    pSalI2CMngtAdpt->pHalGdmaOpInit = HalGdmaOpInit;

    /* To assign the default (ROM) SAL interrupt function */
#if defined(CONFIG_CHIP_A_CUT) || defined(CONFIG_CHIP_B_CUT) || defined(CONFIG_CHIP_C_CUT)
    pSalI2CMngtAdpt->pSalIrqFunc    = I2CISRHandle_Patch;
#elif defined(CONFIG_CHIP_E_CUT)
    pSalI2CMngtAdpt->pSalIrqFunc    = I2CISRHandle_V04;
#endif

    /* To assign the default (ROM) SAL DMA TX interrupt function */
    pSalI2CMngtAdpt->pSalDMATxIrqFunc   = I2CTXGDMAISRHandle;

    /* To assign the default (ROM) SAL DMA RX interrupt function */
    pSalI2CMngtAdpt->pSalDMARxIrqFunc   = I2CRXGDMAISRHandle;
    
    return pSalI2CMngtAdpt;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CFreeMngtAdpt
//
// Description:
//		Free all the previous allocated memory space.
//
// Arguments:
//		[in] PSAL_I2C_MNGT_ADPT  pSalI2CMngtAdpt -
//			I2C SAL management adapter pointer
//         
//
// Return:
//		The status of the enable process.
//          _EXIT_SUCCESS if the RtkI2CFreeMngtAdpt succeeded.
//          _EXIT_FAILURE if the RtkI2CFreeMngtAdpt failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-02.
//
//---------------------------------------------------------------------------------------------------
HAL_Status
RtkI2CFreeMngtAdpt(
    IN  PSAL_I2C_MNGT_ADPT  pSalI2CMngtAdpt
){
#ifndef I2C_STATIC_ALLOC
    RtlMfree((u8 *)pSalI2CMngtAdpt->pUserCB->pTXCB, (sizeof(SAL_I2C_USERCB_ADPT)*SAL_USER_CB_NUM));
    RtlMfree((u8 *)pSalI2CMngtAdpt->pDMAConf, (sizeof(SAL_I2C_DMA_USER_DEF)));
    RtlMfree((u8 *)pSalI2CMngtAdpt->pIrqRxGdmaHnd, (sizeof(IRQ_HANDLE)));
    RtlMfree((u8 *)pSalI2CMngtAdpt->pIrqTxGdmaHnd, (sizeof(IRQ_HANDLE)));
    RtlMfree((u8 *)pSalI2CMngtAdpt->pHalGdmaOp, (sizeof(HAL_GDMA_OP)));
    RtlMfree((u8 *)pSalI2CMngtAdpt->pHalRxGdmaAdp, (sizeof(HAL_GDMA_ADAPTER)));
    RtlMfree((u8 *)pSalI2CMngtAdpt->pHalTxGdmaAdp, (sizeof(HAL_GDMA_ADAPTER)));
    RtlMfree((u8 *)pSalI2CMngtAdpt->pUserCB, sizeof(SAL_I2C_USER_CB));
    RtlMfree((u8 *)pSalI2CMngtAdpt->pIrqHnd, sizeof(IRQ_HANDLE));
    RtlMfree((u8 *)pSalI2CMngtAdpt->pHalOp, sizeof(HAL_I2C_OP));
    RtlMfree((u8 *)pSalI2CMngtAdpt->pHalInitDat, sizeof(HAL_I2C_INIT_DAT));
    RtlMfree((u8 *)pSalI2CMngtAdpt->pSalHndPriv, sizeof(SAL_I2C_HND_PRIV));    
    RtlMfree((u8 *)pSalI2CMngtAdpt, sizeof(SAL_I2C_MNGT_ADPT));
#else
    ;
#endif

    return HAL_OK;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CGetSalHnd
//
// Description:
//         Allocation of lower layer memory spaces will be done by invoking RtkI2CGetMngtAdpt
//         in this function and return a SAL_I2C_HND pointer to upper layer.
//         According to the given I2C index, RtkI2CGetMngtAdpt will allocate all the memory
//         space such as SAL_I2C_HND, HAL_I2C_INIT_DAT, SAL_I2C_USER_CB etc.
//         
//
// Arguments:
//		[in] u8 I2CIdx -
//			I2C Index
//
// Return:
//		PSAL_I2C_HND
//               A pointer to SAL_I2C_HND which is allocated in the lower layer.
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
PSAL_I2C_HND
RtkI2CGetSalHnd(
    IN  u8  I2CIdx
){
    PSAL_I2C_MNGT_ADPT  pSalI2CMngtAdpt = NULL;
    PSAL_I2C_HND        pSalI2CHND      = NULL;

    /* Check the user define setting and the given index */
    if (RtkI2CIdxChk(I2CIdx)) {
        return (PSAL_I2C_HND)NULL;
    }

    /* Invoke RtkI2CGetMngtAdpt to get the I2C SAL management adapter pointer */
    pSalI2CMngtAdpt = RtkI2CGetMngtAdpt(I2CIdx);

    /* Assign the private SAL handle to public SAL handle */
    pSalI2CHND      = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);

    /* Assign the internal HAL initial data pointer to the SAL handle */
    pSalI2CHND->pInitDat    = pSalI2CMngtAdpt->pHalInitDat;

    /* Assign the internal user callback pointer to the SAL handle */
    pSalI2CHND->pUserCB     = pSalI2CMngtAdpt->pUserCB;

    /* Assign the internal user define DMA configuration to the SAL handle */
    pSalI2CHND->pDMAConf    = pSalI2CMngtAdpt->pDMAConf;
    
    return &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
}


//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CFreeSalHnd
//
// Description:
//         Based on the given pSalI2CHND, the top layer management adapter pointer could
//         be reversely parsed. And free memory space is done by RtkI2CFreeMngtAdpt.
//         
//
// Arguments:
//		[in] PSAL_I2C_HND    pSalI2CHND -
//			SAL I2C handle
//
// Return:
//		The status of the free SAL memory space process.
//          _EXIT_SUCCESS if the RtkI2CFreeSalHnd succeeded.
//          _EXIT_FAILURE if the RtkI2CFreeSalHnd failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
HAL_Status
RtkI2CFreeSalHnd(
    IN  PSAL_I2C_HND    pSalI2CHND
){
    PSAL_I2C_MNGT_ADPT  pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND_PRIV   pSalI2CHNDPriv      = NULL;

    /* To get the SAL_I2C_MNGT_ADPT pointer */
    pSalI2CHNDPriv  = CONTAINER_OF(pSalI2CHND, SAL_I2C_HND_PRIV, SalI2CHndPriv);
    pSalI2CMngtAdpt = CONTAINER_OF(pSalI2CHNDPriv->ppSalI2CHnd, SAL_I2C_MNGT_ADPT, pSalHndPriv);

    /* Invoke RtkI2CFreeMngtAdpt to free all the lower layer memory space */
    return (RtkI2CFreeMngtAdpt(pSalI2CMngtAdpt));
}

#endif  // end of "#ifndef CONFIG_MBED_ENABLED"

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkSalI2CSts
//
// Description:
//		Get i2c status
//
// Arguments:
//         A SAL operation adapter pointer
//
// Return:
//		NA
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//---------------------------------------------------------------------------------------------------
u32 
RtkSalI2CSts(
    IN  VOID *Data
){
    PSAL_I2C_HND        pSalI2CHND          = (PSAL_I2C_HND) Data;
    PSAL_I2C_HND_PRIV   pSalI2CHNDPriv      = NULL;
    PSAL_I2C_MNGT_ADPT  pSalI2CMngtAdpt     = NULL;
    PHAL_I2C_INIT_DAT   pHalI2CInitDat      = NULL;
    PHAL_I2C_OP         pHalI2COP           = NULL;
    u32 I2CLocalTemp;
    
    
    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalI2CHNDPriv  = CONTAINER_OF(pSalI2CHND, SAL_I2C_HND_PRIV, SalI2CHndPriv);
    pSalI2CMngtAdpt = CONTAINER_OF(pSalI2CHNDPriv->ppSalI2CHnd, SAL_I2C_MNGT_ADPT, pSalHndPriv);
    
    pHalI2CInitDat  = pSalI2CMngtAdpt->pHalInitDat;
    pHalI2COP       = pSalI2CMngtAdpt->pHalOp;

    I2CLocalTemp    = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_RAW_INTR_STAT);

    if (I2CLocalTemp & BIT_IC_RAW_INTR_STAT_GEN_CALL) {
        return 2;
    }
    else if (I2CLocalTemp & BIT_IC_RAW_INTR_STAT_RD_REQ) {
        return 1;
    }

    I2CLocalTemp    = pHalI2COP->HalI2CReadReg(pHalI2CInitDat, REG_DW_I2C_IC_STATUS);

    if (I2CLocalTemp & BIT_IC_STATUS_RFNE) {
        return 3;
    }

    return 0;
}
