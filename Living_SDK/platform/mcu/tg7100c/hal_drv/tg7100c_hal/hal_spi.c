#include <string.h>
#include <stdio.h>
#include <hal/soc/spi.h>
#include <aos/kernel.h>

#include <bl_dma.h>
#include <bl_gpio.h>
#include <tg7100c_spi.h>
#include <tg7100c_gpio.h>
#include <tg7100c_glb.h>
#include <tg7100c_dma.h>
#include <bl_irq.h>
#include <bl_dma.h>
#include <hal_spi.h>


#include <libfdt.h>
#include <blog.h>
#include <k_api.h>
#include <aos/kernel.h>

#define SPI_NUM_MAX         1 /* only support spi0 */
#define LLI_BUFF_SIZE       2048

#define EVT_GROUP_SPI_DMA_TX    (1<<0)
#define EVT_GROUP_SPI_DMA_RX    (1<<1)
#define EVT_GROUP_SPI_DMA_TR    (EVT_GROUP_SPI_DMA_TX | EVT_GROUP_SPI_DMA_RX)

spi_hw_t *g_hal_buf = NULL;
static aos_event_t  spi_event;

void bl_spi0_dma_int_handler_tx(void);
void bl_spi0_dma_int_handler_rx(void);

void hal_spi_hw_init(spi_hw_t *arg)
{
    uint8_t gpiopins[4];
    int ret = 0;

    if (!arg) {
        blog_error("arg err.\r\n");
        return;
    }
    g_hal_buf = arg;
    gpiopins[0] = arg->pin_cs;
    gpiopins[1] = arg->pin_clk;
    gpiopins[2] = arg->pin_mosi;
    gpiopins[3] = arg->pin_miso;
    
    GLB_GPIO_Func_Init(GPIO_FUN_SPI,gpiopins,sizeof(gpiopins)/sizeof(gpiopins[0]));

    if (arg->mode == HAL_SPI_MODE_MASTER) {
        GLB_Set_SPI_0_ACT_MOD_Sel(GLB_SPI_PAD_ACT_AS_MASTER);
    } else {
        GLB_Set_SPI_0_ACT_MOD_Sel(GLB_SPI_PAD_ACT_AS_SLAVE);
    }

    ret = aos_event_new(&spi_event, 0);
    if (ret != 0) {
        printf("create event failed. \r\n");
    }

    return ret;
}

void hal_spi_hw_deinit(spi_hw_t *arg)
{
    DMA_Channel_Disable(arg->tx_dma_ch);
    DMA_Channel_Disable(arg->rx_dma_ch);
    aos_event_free(&spi_event);
    g_hal_buf = NULL;

    return;
}

static int lli_list_init(DMA_LLI_Ctrl_Type **pptxlli, DMA_LLI_Ctrl_Type **pprxlli, uint8_t *ptx_data, uint8_t *prx_data, uint32_t length)
{
    uint32_t i = 0;
    uint32_t count;
    uint32_t remainder;
    struct DMA_Control_Reg dmactrl;

    count = length / LLI_BUFF_SIZE;
    remainder = length % LLI_BUFF_SIZE;

    if (remainder != 0) {
        count = count + 1;
    }

    dmactrl.SBSize = DMA_BURST_SIZE_1;
    dmactrl.DBSize = DMA_BURST_SIZE_1;
    dmactrl.SWidth = DMA_TRNS_WIDTH_8BITS;
    dmactrl.DWidth = DMA_TRNS_WIDTH_8BITS;
    dmactrl.Prot = 0;
    dmactrl.SLargerD = 0;

    *pptxlli = aos_malloc(sizeof(DMA_LLI_Ctrl_Type) * count);
    if (*pptxlli == NULL) {
        blog_error("malloc lli failed. \r\n");

        return -1;
    }

    *pprxlli = aos_malloc(sizeof(DMA_LLI_Ctrl_Type) * count);
    if (*pprxlli == NULL) {
        blog_error("malloc lli failed.");
        aos_free(*pptxlli);

        return -1;
    }

    for (i = 0; i < count; i++) {
        if (remainder == 0) {
            dmactrl.TransferSize = LLI_BUFF_SIZE;
        } else {
            if (i == count - 1) {
                dmactrl.TransferSize = remainder;
            } else {
                dmactrl.TransferSize = LLI_BUFF_SIZE;
            }
        }

        dmactrl.SI = DMA_MINC_ENABLE;
        dmactrl.DI = DMA_MINC_DISABLE;
            
        if (i == count - 1) {
            dmactrl.I = 1;
        } else {
            dmactrl.I = 0;
        }

        (*pptxlli)[i].srcDmaAddr = (uint32_t)(ptx_data + i * LLI_BUFF_SIZE);
        (*pptxlli)[i].destDmaAddr = (uint32_t)(SPI_BASE+SPI_FIFO_WDATA_OFFSET);
        (*pptxlli)[i].dmaCtrl = dmactrl;

        dmactrl.SI = DMA_MINC_DISABLE;
        //dmactrl.DI = DMA_MINC_ENABLE;
        dmactrl.DI = DMA_MINC_DISABLE;
        (*pprxlli)[i].srcDmaAddr = (uint32_t)(SPI_BASE+SPI_FIFO_RDATA_OFFSET);
        (*pprxlli)[i].destDmaAddr = (uint32_t)(prx_data + i * LLI_BUFF_SIZE);
        (*pprxlli)[i].dmaCtrl = dmactrl;

        if (i != 0) {
            (*pptxlli)[i-1].nextLLI = (uint32_t)&(*pptxlli)[i];
            (*pprxlli)[i-1].nextLLI = (uint32_t)&(*pprxlli)[i];
        }

        (*pptxlli)[i].nextLLI = 0;
        (*pprxlli)[i].nextLLI = 0;
    }

    return 0;
}

void hal_spi_dma_init(spi_hw_t *arg)
{
    spi_hw_t *hw_arg = arg;
    SPI_CFG_Type spicfg;
    SPI_ClockCfg_Type clockcfg;
    SPI_FifoCfg_Type fifocfg;
    SPI_ID_Type spi_id;
    uint8_t clk_div;
    
    //spi_id = hw_arg->ssp_id;
    spi_id = 0;
    /* clock */
    /*1  --->  40 Mhz
     *2  --->  20 Mhz
     *5  --->  8  Mhz
     *6  --->  6.66 Mhz
     *10 --->  4 Mhz
     * */
    clk_div = (uint8_t)(40000000 / hw_arg->freq);
    GLB_Set_SPI_CLK(ENABLE,0);
    clockcfg.startLen = clk_div;
    clockcfg.stopLen = clk_div;
    clockcfg.dataPhase0Len = clk_div;
    clockcfg.dataPhase1Len = clk_div;
    clockcfg.intervalLen = clk_div;
    SPI_ClockConfig(0, &clockcfg);

    /* spi config */
    spicfg.deglitchEnable = DISABLE;
    spicfg.continuousEnable = ENABLE;
    spicfg.byteSequence = SPI_BYTE_INVERSE_BYTE0_FIRST,
    spicfg.bitSequence = SPI_BIT_INVERSE_MSB_FIRST,
    spicfg.frameSize = SPI_FRAME_SIZE_8;

    if (hw_arg->polar_phase == 0) {
        spicfg.clkPhaseInv = SPI_CLK_PHASE_INVERSE_0;
        spicfg.clkPolarity = SPI_CLK_POLARITY_LOW;
    } else if (hw_arg->polar_phase == 1) {
        spicfg.clkPhaseInv = SPI_CLK_PHASE_INVERSE_1;
        spicfg.clkPolarity = SPI_CLK_POLARITY_LOW;
    } else if (hw_arg->polar_phase == 2) {
        spicfg.clkPhaseInv = SPI_CLK_PHASE_INVERSE_0;
        spicfg.clkPolarity = SPI_CLK_POLARITY_HIGH;
    } else if (hw_arg->polar_phase == 3) {
        spicfg.clkPhaseInv = SPI_CLK_PHASE_INVERSE_1;
        spicfg.clkPolarity = SPI_CLK_POLARITY_HIGH;
    } else {
        blog_error("node support polar_phase \r\n");
    }
    SPI_Init(0,&spicfg);

    if (hw_arg->mode == HAL_SPI_MODE_MASTER)
    {
        SPI_Disable(0, SPI_WORK_MODE_MASTER);
    } else {
        SPI_Disable(0, SPI_WORK_MODE_SLAVE);
    }

    SPI_IntMask(spi_id,SPI_INT_ALL,MASK);

    /* fifo */
    fifocfg.txFifoThreshold = 1;
    fifocfg.rxFifoThreshold = 1;
    fifocfg.txFifoDmaEnable = ENABLE;
    fifocfg.rxFifoDmaEnable = ENABLE;
    SPI_FifoConfig(spi_id,&fifocfg);

    DMA_Disable();
    DMA_IntMask(hw_arg->tx_dma_ch, DMA_INT_ALL, MASK);
    DMA_IntMask(hw_arg->tx_dma_ch, DMA_INT_TCOMPLETED, UNMASK);
    DMA_IntMask(hw_arg->tx_dma_ch, DMA_INT_ERR, UNMASK);

    DMA_IntMask(hw_arg->rx_dma_ch, DMA_INT_ALL, MASK);
    DMA_IntMask(hw_arg->rx_dma_ch, DMA_INT_TCOMPLETED, UNMASK); 
    DMA_IntMask(hw_arg->rx_dma_ch, DMA_INT_ERR, UNMASK);

    bl_irq_enable(DMA_ALL_IRQn);
    bl_dma_irq_register(hw_arg->tx_dma_ch, bl_spi0_dma_int_handler_tx, NULL, NULL);
    bl_dma_irq_register(hw_arg->rx_dma_ch, bl_spi0_dma_int_handler_rx, NULL, NULL);

    return;
}

int hal_spi_dma_trans(spi_hw_t *arg, uint8_t *TxData, uint8_t *RxData, uint32_t Len, uint32_t timeout)
{
    DMA_LLI_Cfg_Type txllicfg;
    DMA_LLI_Cfg_Type rxllicfg;
    DMA_LLI_Ctrl_Type *ptxlli;
    DMA_LLI_Ctrl_Type *prxlli;
    int ret = 0;
    uint32_t actl_flags;

    if (!arg) {
        blog_error("arg err.\r\n");
        return;
    }

    txllicfg.dir = DMA_TRNS_M2P;
    txllicfg.srcPeriph = DMA_REQ_NONE; 
    txllicfg.dstPeriph = DMA_REQ_SPI_TX;

    rxllicfg.dir = DMA_TRNS_P2M;
    rxllicfg.srcPeriph = DMA_REQ_SPI_RX;
    rxllicfg.dstPeriph = DMA_REQ_NONE;

    DMA_Channel_Disable(arg->tx_dma_ch);
    DMA_Channel_Disable(arg->rx_dma_ch);
    bl_dma_int_clear(arg->tx_dma_ch);
    bl_dma_int_clear(arg->rx_dma_ch);
    DMA_Enable();

    if (arg->mode == HAL_SPI_MODE_MASTER) {
        SPI_Enable(0, SPI_WORK_MODE_MASTER);
    } else {
        SPI_Enable(0, SPI_WORK_MODE_SLAVE);
    }

    ret = lli_list_init(&ptxlli, &prxlli, TxData, RxData, Len);
    if (ret < 0) {
        blog_error("init lli failed. \r\n");

        return;
    }

    DMA_LLI_Init(arg->tx_dma_ch, &txllicfg);
    DMA_LLI_Init(arg->rx_dma_ch, &rxllicfg);
    DMA_LLI_Update(arg->tx_dma_ch,(uint32_t)ptxlli);
    DMA_LLI_Update(arg->rx_dma_ch,(uint32_t)prxlli);
    DMA_Channel_Enable(arg->tx_dma_ch);
    DMA_Channel_Enable(arg->rx_dma_ch);

    ret = aos_event_get(&spi_event, EVT_GROUP_SPI_DMA_TR, RHINO_AND_CLEAR, &actl_flags, timeout);
    if (ret == 0) {
        printf("spi transfer success. \r\n");
    } else {
        printf("spi transfer failed. \r\n");
    }

    aos_free(ptxlli);
    aos_free(prxlli);

    return ret;
}

void bl_spi0_dma_int_handler_tx(void)
{
    int ret;
    
    if (NULL != g_hal_buf) {
        bl_dma_int_clear(g_hal_buf->tx_dma_ch);
        ret = aos_event_set(&spi_event, EVT_GROUP_SPI_DMA_TX, RHINO_OR);
        if (ret != 0) {
            printf("set event failed. \r\n");
        }
    }

    return;
}

void bl_spi0_dma_int_handler_rx(void)
{
    int ret;
    if (NULL != g_hal_buf) {
        bl_dma_int_clear(g_hal_buf->rx_dma_ch);
        ret = aos_event_set(&spi_event, EVT_GROUP_SPI_DMA_RX, RHINO_OR);
        if (ret != 0) {
            printf("set event failed. \r\n");
        }
    }

    return;
}
