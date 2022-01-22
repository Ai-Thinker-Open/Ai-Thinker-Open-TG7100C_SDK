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
static spi_data_cb_t *g_spi_cb_data = NULL; 

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

    ret = aos_event_new(&spi_event, EVT_GROUP_SPI_DMA_TR);
    if (ret != 0) {
        printf("create event failed. \r\n");
    }
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

        if (i == count - 1) {
            dmactrl.I = 1;
        } else {
            dmactrl.I = 0;
        }

        if (ptx_data) {
            dmactrl.SI = DMA_MINC_ENABLE;
            dmactrl.DI = DMA_MINC_DISABLE;
            (*pptxlli)[i].srcDmaAddr = (uint32_t)(ptx_data + i * LLI_BUFF_SIZE);
            (*pptxlli)[i].destDmaAddr = (uint32_t)(SPI_BASE+SPI_FIFO_WDATA_OFFSET);
            (*pptxlli)[i].dmaCtrl = dmactrl;
            if (i != 0) {
                (*pptxlli)[i-1].nextLLI = (uint32_t)&(*pptxlli)[i];
            }
            (*pptxlli)[i].nextLLI = 0;
        } else {
            aos_free(*pptxlli);
            *pptxlli = NULL;
        }

        if (prx_data) {
            dmactrl.SI = DMA_MINC_DISABLE;
            dmactrl.DI = DMA_MINC_ENABLE;
            (*pprxlli)[i].srcDmaAddr = (uint32_t)(SPI_BASE+SPI_FIFO_RDATA_OFFSET);
            (*pprxlli)[i].destDmaAddr = (uint32_t)(prx_data + i * LLI_BUFF_SIZE);
            (*pprxlli)[i].dmaCtrl = dmactrl;
            if (i != 0) {
                (*pprxlli)[i-1].nextLLI = (uint32_t)&(*pprxlli)[i];
            }
            (*pprxlli)[i].nextLLI = 0;

        } else {
            aos_free(*pprxlli);
            *pprxlli = NULL;
        }
    }

    return 0;
}

struct __prescale_and_count_cal_ctx {
    uint64_t   desired_ticks;
    uint64_t   count_max;
    uint64_t   error;
    uint64_t   count;
    uint32_t   prescale;
    uint8_t    update;
};

static void __error_check_with_new_prescale (
    struct __prescale_and_count_cal_ctx *p_ctx, uint32_t prescale_new)
{
    uint64_t count = p_ctx->desired_ticks / prescale_new;
    uint64_t error;

    if (count > p_ctx->count_max) {
        count = p_ctx->count_max;
    }

    error = p_ctx->desired_ticks - count * prescale_new;

    if (p_ctx->error > error) {
        p_ctx->error    = error;
        p_ctx->count    = count;
        p_ctx->prescale = prescale_new;
        p_ctx->update   = 1;
    }
}

static int __prescale_and_count_cal (uint32_t               counter_width,
                                     uint32_t               prescale_max,
                                     size_t                 ticks,
                                     uint32_t              *p_prescale,
                                     size_t                *p_count)
{
    struct __prescale_and_count_cal_ctx  ctx;

    uint32_t prescale_min;
    uint32_t prescale;

    size_t count_max;

    count_max = ( (uint64_t)1ull << counter_width ) - 1;

    if (ticks <= count_max) {

         *p_prescale = 1;
         *p_count    = ticks;

         return 0;
    }

    prescale_min = ticks / count_max;

    ctx.count_max     = count_max;
    ctx.desired_ticks = ticks;
    ctx.error         = ticks;
    ctx.count         = count_max;
    ctx.prescale      = 1;
    ctx.update        = 0;

    if (prescale_max < prescale_min) {
        return -1;
    }

    for (prescale = prescale_min; prescale <= prescale_max; prescale++) {
        __error_check_with_new_prescale(&ctx, prescale);
        if (ctx.error == 0) {
            break;
        }
    }

    if (ctx.update) {

         *p_prescale = ctx.prescale;
         *p_count    = ctx.count;

        return 0;
    }

    return -1;
}

void hal_spi_dma_init(spi_hw_t *arg)
{
    spi_hw_t *hw_arg = arg;
    SPI_CFG_Type spicfg;
    SPI_ClockCfg_Type clockcfg;
    SPI_FifoCfg_Type fifocfg;
    SPI_ID_Type spi_id;
    uint32_t clk_div;
    uint32_t ticks;
    size_t count;

    //spi_id = hw_arg->ssp_id;
    spi_id = 0;
    /* clock */
    /*1  --->  40 Mhz
     *2  --->  20 Mhz
     *5  --->  8  Mhz
     *6  --->  6.66 Mhz
     *10 --->  4 Mhz
     * */
    ticks = 40000000 / hw_arg->freq;
    if (__prescale_and_count_cal(8, 0xff, ticks, &clk_div, &count) != 0) {
        printf("--------spi div clk error!!!!--------\r\n");
        return;
    }

    g_spi_cb_data = (spi_data_cb_t *)aos_malloc(sizeof(spi_data_cb_t));
    if (NULL == g_spi_cb_data) {
        blog_error("malloc g_spi_cb_data failed. \r\n");
    }

    memset(g_spi_cb_data, 0, sizeof(spi_data_cb_t));
    GLB_Set_SPI_CLK(ENABLE, clk_div - 1);
    clockcfg.startLen = count;
    clockcfg.stopLen = count;
    clockcfg.dataPhase0Len = count;
    clockcfg.dataPhase1Len = count;
    clockcfg.intervalLen = count;
    SPI_ClockConfig(0, &clockcfg);

    /* spi config */
    spicfg.deglitchEnable = DISABLE;
    spicfg.continuousEnable = ENABLE;

    if (hw_arg->data_shift == HAL_SPI_DATA_SHIFT_MSB) {
        spicfg.byteSequence = SPI_BYTE_INVERSE_BYTE0_FIRST;
        spicfg.bitSequence = SPI_BIT_INVERSE_MSB_FIRST;   
    } else if (hw_arg->data_shift == HAL_SPI_DATA_SHIFT_LSB) {
        spicfg.byteSequence = SPI_BYTE_INVERSE_BYTE0_FIRST;
        spicfg.bitSequence = SPI_BIT_INVERSE_LSB_FIRST;
    } else {
        printf("data_shift is not support:%d\r\n", hw_arg->data_shift);    
    }
   
    if (hw_arg->data_width == HAL_SPI_DATA_WIDTH_8B) {
        spicfg.frameSize = SPI_FRAME_SIZE_8;
    } else if (hw_arg->data_width == HAL_SPI_DATA_WIDTH_16B) {
        printf("data_width is not support:%d\r\n", hw_arg->data_width);
    } else {
        printf("data_width is not support:%d\r\n", hw_arg->data_width);
    }

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

//    if (hw_arg->mode == HAL_SPI_MODE_MASTER)
//    {
//        SPI_Disable(0, SPI_WORK_MODE_MASTER);
//    } else {
//        SPI_Disable(0, SPI_WORK_MODE_SLAVE);
//    }

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

int hal_spi_dma_deinit(spi_hw_t *arg)
{
    if (NULL == arg) {
        printf("arg is NULL\r\n");
        return -1;
    }

    aos_free(g_spi_cb_data);
    bl_dma_irq_unregister(arg->tx_dma_ch);
    bl_dma_irq_unregister(arg->rx_dma_ch);
    
    return 0;
}

int hal_spi_dma_trans(spi_hw_t *arg, uint8_t *TxData, uint8_t *RxData, uint32_t Len, uint32_t timeout)
{
    DMA_LLI_Cfg_Type txllicfg;
    DMA_LLI_Cfg_Type rxllicfg;
    DMA_LLI_Ctrl_Type *ptxlli;
    DMA_LLI_Ctrl_Type *prxlli;
    int ret = 0;
    unsigned int  actl_flags;
    
    if (!arg) {
        blog_error("arg err.\r\n");
        return -1;
    }
    if (g_spi_cb_data->cb) {
        if (g_spi_cb_data->ptxlli) {
            ret = aos_event_get(&spi_event, EVT_GROUP_SPI_DMA_TX, RHINO_AND_CLEAR, &actl_flags, 0);
            if (ret) {
                return -1;
            }
        }

        if (g_spi_cb_data->prxlli) {
            ret = aos_event_get(&spi_event, EVT_GROUP_SPI_DMA_RX, RHINO_AND_CLEAR, &actl_flags, 0);
            if (ret) {
                return -1;
            }
        }
    } else {
        aos_event_set(&spi_event, 0, RHINO_OR);
        aos_event_set(&spi_event, 0, RHINO_OR);
    }

    txllicfg.dir = DMA_TRNS_M2P;
    txllicfg.srcPeriph = DMA_REQ_NONE; 
    txllicfg.dstPeriph = DMA_REQ_SPI_TX;

    rxllicfg.dir = DMA_TRNS_P2M;
    rxllicfg.srcPeriph = DMA_REQ_SPI_RX;
    rxllicfg.dstPeriph = DMA_REQ_NONE;

    DMA_Channel_Disable(arg->tx_dma_ch);
    DMA_Channel_Disable(arg->rx_dma_ch);
    DMA_Enable();

    if (arg->mode == HAL_SPI_MODE_MASTER) {
        SPI_Enable(0, SPI_WORK_MODE_MASTER);
    } else {
        SPI_Enable(0, SPI_WORK_MODE_SLAVE);
    }

    ret = lli_list_init(&ptxlli, &prxlli, TxData, RxData, Len);
    if (ret < 0) {
        blog_error("init lli failed. \r\n");

        return ret;
    }

    g_spi_cb_data->ptxlli = ptxlli;
    g_spi_cb_data->prxlli = prxlli;
    g_spi_cb_data->RxData = RxData;
    g_spi_cb_data->length = Len;

    DMA_LLI_Init(arg->tx_dma_ch, &txllicfg);
    DMA_LLI_Init(arg->rx_dma_ch, &rxllicfg);

    if (prxlli) {
        DMA_LLI_Update(arg->rx_dma_ch,(uint32_t)prxlli);
        DMA_Channel_Enable(arg->rx_dma_ch);
    }

    if (ptxlli) {
        DMA_LLI_Update(arg->tx_dma_ch,(uint32_t)ptxlli);
        DMA_Channel_Enable(arg->tx_dma_ch);
    }

    if (NULL == g_spi_cb_data->cb) {
        if (ptxlli) {
            ret = aos_event_get(&spi_event, EVT_GROUP_SPI_DMA_TX, RHINO_AND_CLEAR, &actl_flags, timeout);
            if (ret != 0) {
                printf("spi transfer failed. \r\n");
            }
        }
        if (prxlli) {
            ret = aos_event_get(&spi_event, EVT_GROUP_SPI_DMA_RX, RHINO_AND_CLEAR, &actl_flags, timeout);
            if (ret != 0) {
                printf("spi transfer failed. \r\n");
            }
        }

        aos_free(ptxlli);
        aos_free(prxlli);
        g_spi_cb_data->ptxlli = NULL;
        g_spi_cb_data->prxlli = NULL;
    }

    return ret;

}
void bl_spi0_dma_int_handler_tx(void)
{
    int ret;

    if (NULL != g_hal_buf) {

        ret = aos_event_set(&spi_event, EVT_GROUP_SPI_DMA_TX, RHINO_OR);
        if (ret != 0) {
            printf("set event failed. \r\n");
        }
        if (g_spi_cb_data->cb) {
            g_spi_cb_data->cb(SPI_INT_DMA_TC, NULL, 0);
            aos_free(g_spi_cb_data->ptxlli);
            g_spi_cb_data->ptxlli = NULL;
        }
    }

    return;
}

void bl_spi0_dma_int_handler_rx(void)
{
    int ret;
    uint8_t *data;
    uint32_t size;

    if (NULL != g_hal_buf) {

        ret = aos_event_set(&spi_event, EVT_GROUP_SPI_DMA_RX, RHINO_OR);
        if (ret != 0) {
            printf("set event failed. \r\n");
        }
        if (NULL == g_spi_cb_data->cb) {
            data = g_spi_cb_data->RxData;
            size = g_spi_cb_data->length;
            g_spi_cb_data->cb(SPI_INT_DMA_TC, data, size);
            aos_free(g_spi_cb_data->prxlli);
            g_spi_cb_data->prxlli = NULL;
        }
    }

    return;
}

spi_data_cb_t *bl_spi_get_cb_ctx(void)
{
    if (NULL == g_spi_cb_data) {
        return NULL;
    }

    return g_spi_cb_data;
 
}
