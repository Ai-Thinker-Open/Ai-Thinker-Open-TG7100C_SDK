/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTL8195A_OTG_H_
#define _RTL8195A_OTG_H_

#include "rtl8195a.h"
#define OTG_FAST_INIT   1

#define HAL_OTG_READ32(addr)            HAL_READ32(USB_OTG_REG_BASE, (u32)addr)
#define HAL_OTG_WRITE32(addr, value)    HAL_WRITE32(USB_OTG_REG_BASE, (u32)addr, value)

#define HAL_OTG_MODIFY32(addr, clrmsk, setmsk)     HAL_WRITE32(USB_OTG_REG_BASE,(u32)addr,\
                                                   ((HAL_READ32(USB_OTG_REG_BASE, (u32)addr) & (~clrmsk)) | setmsk))

#define DWC_READ_REG32(_reg_)   HAL_OTG_READ32((u32)_reg_)
#define DWC_WRITE_REG32(_reg_, _val_)   HAL_OTG_WRITE32((u32)_reg_,_val_)
#define DWC_MODIFY_REG32(_reg_,_cmsk_,_smsk_)   HAL_OTG_MODIFY32((u32)_reg_,_cmsk_,_smsk_)


//This part is added for RTK power sequence

//3 SYS_ON reg

//#define REG_SYS_FUNC_EN                     0x08
#define BIT_SHIFT_SOC_SYSPEON_EN   4
#define BIT_MASK_SOC_SYSPEON_EN    0x1
#define BIT_SOC_SYSPEON_EN_OTG(x)(((x) & BIT_MASK_SOC_SYSPEON_EN) << BIT_SHIFT_SOC_SYSPEON_EN)
#define BIT_INVC_SOC_SYSPEON_EN (~(BIT_MASK_SOC_SYSPEON_EN << BIT_SHIFT_SOC_SYSPEON_EN))


//3 Peri_ON reg
#define REG_OTG_PWCSEQ_OFFSET_OTG               0x40000000
#define REG_OTG_PWCSEQ_PWC_OTG                  0x200
#define REG_OTG_PWCSEQ_ISO_OTG                  0x204
#define REG_SOC_HCI_COM_FUNC_EN_OTG             0x214
#define REG_PESOC_HCI_CLK_CTRL0_OTG             0x240

//#define REG_PON_ISO_CTRL                        0x204


#define REG_OTG_PWCSEQ_IP_OFF               0x30004     //This is in OTG IP
#define REG_OTG_PS_INTR_STS                 0x30008     //This is in OTG IP
#define REG_OTG_PS_INTR_MSK                 0x3000C     //This is in OTG IP


//4 REG_OTG_PWCSEQ_PWC
#define BIT_SHIFT_PWC_USBD_EN   0
#define BIT_MASK_PWC_USBD_EN    0x1
#define BIT_PWC_USBD_EN(x)(((x) & BIT_MASK_PWC_USBD_EN) << BIT_SHIFT_PWC_USBD_EN)
#define BIT_INVC_PWC_USBD_EN (~(BIT_MASK_PWC_USBD_EN << BIT_SHIFT_PWC_USBD_EN))

#define BIT_SHIFT_PWC_UPLV_EN   1
#define BIT_MASK_PWC_UPLV_EN    0x1
#define BIT_PWC_UPLV_EN(x)(((x) & BIT_MASK_PWC_UPLV_EN) << BIT_SHIFT_PWC_UPLV_EN)
#define BIT_INVC_PWC_UPLV_EN (~(BIT_MASK_PWC_UPLV_EN << BIT_SHIFT_PWC_UPLV_EN))

#define BIT_SHIFT_PWC_UPHV_EN   2
#define BIT_MASK_PWC_UPHV_EN    0x1
#define BIT_PWC_UPHV_EN(x)(((x) & BIT_MASK_PWC_UPHV_EN) << BIT_SHIFT_PWC_UPHV_EN)
#define BIT_INVC_PWC_UPHV_EN (~(BIT_MASK_PWC_UPHV_EN << BIT_SHIFT_PWC_UPHV_EN))

//4 REG_OTG_PWCSEQ_ISO
#define BIT_SHIFT_ISO_USBD_EN   0
#define BIT_MASK_ISO_USBD_EN    0x1
#define BIT_ISO_USBD_EN(x)(((x) & BIT_MASK_ISO_USBD_EN) << BIT_SHIFT_ISO_USBD_EN)
#define BIT_INVC_ISO_USBD_EN (~(BIT_MASK_ISO_USBD_EN << BIT_SHIFT_ISO_USBD_EN))

#define BIT_SHIFT_ISO_USBA_EN   1
#define BIT_MASK_ISO_USBA_EN    0x1
#define BIT_ISO_USBA_EN(x)(((x) & BIT_MASK_ISO_USBA_EN) << BIT_SHIFT_ISO_USBA_EN)
#define BIT_INVC_ISO_USBA_EN (~(BIT_MASK_ISO_USBA_EN << BIT_SHIFT_ISO_USBA_EN))

//4 REG_SOC_HCI_COM_FUNC_EN
#define BIT_SHIFT_SOC_HCI_OTG_EN   4
#define BIT_MASK_SOC_HCI_OTG_EN    0x1
#define BIT_SOC_HCI_OTG_EN_OTG(x)(((x) & BIT_MASK_SOC_HCI_OTG_EN) << BIT_SHIFT_SOC_HCI_OTG_EN)
#define BIT_INVC_SOC_HCI_OTG_EN (~(BIT_MASK_SOC_HCI_OTG_EN << BIT_SHIFT_SOC_HCI_OTG_EN))

//4 REG_PESOC_HCI_CLK_CTRL0
#define BIT_SHIFT_SOC_ACTCK_OTG_EN   4
#define BIT_MASK_SOC_ACTCK_OTG_EN    0x1
#define BIT_SOC_ACTCK_OTG_EN_OTG(x)(((x) & BIT_MASK_SOC_ACTCK_OTG_EN) << BIT_SHIFT_SOC_ACTCK_OTG_EN)
#define BIT_INVC_SOC_ACTCK_OTG_EN (~(BIT_MASK_SOC_ACTCK_OTG_EN << BIT_SHIFT_SOC_ACTCK_OTG_EN))


//4 REG_OTG_PWCSEQ_OTG
#define BIT_SHIFT_USBOTG_PS_EN   0
#define BIT_MASK_USBOTG_PS_EN    0x1
#define BIT_USBOTG_PS_EN(x)(((x) & BIT_MASK_USBOTG_PS_EN) << BIT_SHIFT_USBOTG_PS_EN)
#define BIT_INVC_USBOTG_PS_EN (~(BIT_MASK_USBOTG_PS_EN << BIT_SHIFT_USBOTG_PS_EN))

#define BIT_SHIFT_USBOTG_DIS_SUSB   1
#define BIT_MASK_USBOTG_DIS_SUSB    0x1
#define BIT_USBOTG_DIS_SUSB(x)(((x) & BIT_MASK_USBOTG_DIS_SUSB) << BIT_SHIFT_USBOTG_DIS_SUSB)
#define BIT_INVC_USBOTG_DIS_SUSB (~(BIT_MASK_USBOTG_DIS_SUSB << BIT_SHIFT_USBOTG_DIS_SUSB))

#define BIT_SHIFT_USBOTG_SUSBM   4
#define BIT_MASK_USBOTG_SUSBM    0x1
#define BIT_USBOTG_SUSBM(x)(((x) & BIT_MASK_USBOTG_SUSBM) << BIT_SHIFT_USBOTG_SUSBM)
#define BIT_INVC_USBOTG_SUSBM (~(BIT_MASK_USBOTG_SUSBM << BIT_SHIFT_USBOTG_SUSBM))

#define BIT_SHIFT_UPLL_CKRDY   5
#define BIT_MASK_UPLL_CKRDY    0x1
#define BIT_UPLL_CKRDY(x)(((x) & BIT_MASK_UPLL_CKRDY) << BIT_SHIFT_UPLL_CKRDY)
#define BIT_INVC_UPLL_CKRDY (~(BIT_MASK_UPLL_CKRDY << BIT_SHIFT_UPLL_CKRDY))

#define BIT_SHIFT_USB_LS        6
#define BIT_MASK_USB_LS    0x3
#define BIT_USB_LS(x)(((x) & BIT_MASK_USB_LS) << BIT_SHIFT_USB_LS)
#define BIT_INVC_USB_LS (~(BIT_MASK_USB_LS << BIT_SHIFT_USB_LS))

#define BIT_SHIFT_USBOTG_EN   8
#define BIT_MASK_USBOTG_EN    0x1
#define BIT_USBOTG_EN(x)(((x) & BIT_MASK_USBOTG_EN) << BIT_SHIFT_USBOTG_EN)
#define BIT_INVC_USBOTG_EN (~(BIT_MASK_USBOTG_EN << BIT_SHIFT_USBOTG_EN))

#define BIT_SHIFT_USBPHY_EN   9
#define BIT_MASK_USBPHY_EN    0x1
#define BIT_USBPHY_EN(x)(((x) & BIT_MASK_USBPHY_EN) << BIT_SHIFT_USBPHY_EN)
#define BIT_INVC_USBPHY_EN (~(BIT_MASK_USBPHY_EN << BIT_SHIFT_USBPHY_EN))

#define BIT_SHIFT_USB_GT_LS_EN   10
#define BIT_MASK_USB_GT_LS_EN    0x1
#define BIT_USB_GT_LS_EN(x)(((x) & BIT_MASK_USB_GT_LS_EN) << BIT_SHIFT_USB_GT_LS_EN)
#define BIT_INVC_USB_GT_LS_EN (~(BIT_MASK_USB_GT_LS_EN << BIT_SHIFT_USB_GT_LS_EN))

#endif
