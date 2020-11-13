/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"
#include "soc_init.h"
#include "hal.h"
#include "stm32l4xx_hal.h"
#include "hal_rtc_stm32l4.h"
#include "stm32l4xx_hal_rtc.h"

/* Init and deInit function for rtc */
static int32_t rtc_Init(rtc_dev_t *rtc);
static int32_t rtc_DeInit(void);

/* function used to transform hal para to stm32l4 para */
static int32_t rtc_format_transform(uint16_t format_hal, uint16_t *format_stm32l4);

/* handle for rtc */
RTC_HandleTypeDef rtc_handle;

int32_t hal_rtc_init(rtc_dev_t *rtc)
{
    int32_t ret = -1;

    if (rtc == NULL) {
        return -1;
    }

    /*init rtc handle*/
    memset(&rtc_handle, 0, sizeof(rtc_handle));

    rtc->priv = &rtc_handle;
    ret = rtc_Init(rtc);

    return ret;
}

int32_t hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time)
{
    uint16_t format = 0;
    int32_t ret1 = -1;
    int32_t ret2 = -1;	
    int32_t ret = -1;
    RTC_TimeTypeDef time_st;
    RTC_DateTypeDef data_st;

    if ((rtc == NULL) || (time == NULL)) {
        return -1;
    }

    ret = rtc_format_transform(rtc->config.format, &format);
    if (ret == 0) {
        ret1 = HAL_RTC_GetTime((RTC_HandleTypeDef *)rtc->priv, &time_st, format);
        ret2 = HAL_RTC_GetDate((RTC_HandleTypeDef *)rtc->priv, &data_st, format);

        if ((ret1 == 0) && (ret2 == 0)) {
            time->year = data_st.Year;
            time->month = data_st.Month;
            time->date = data_st.Date;
            time->weekday = data_st.WeekDay;
            time->hr = time_st.Hours;
            time->min = time_st.Minutes;
            time->sec = time_st.Seconds;

            ret = 0;
        } else {
            ret = -1;
        }
    }

    return ret;
}

int32_t hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time)
{
    uint16_t format = 0;
    int32_t ret1 = -1;
    int32_t ret2 = -1;	
    int32_t ret = -1;
    RTC_TimeTypeDef time_st;
    RTC_DateTypeDef data_st;

    if ((rtc == NULL) || (time == NULL)) {
		    return -1;
		}

    memset(&time_st, 0, sizeof(time_st));
    memset(&data_st, 0, sizeof(data_st));
		
    ret = rtc_format_transform(rtc->config.format, &format);
    if (ret == 0) {
        data_st.Year = time->year;
        data_st.Month = time->month;
        data_st.Date = time->date;
        data_st.WeekDay = time->weekday;
        time_st.Hours = time->hr;
        time_st.Minutes = time->min;
        time_st.Seconds = time->sec;
			
        ret1 = HAL_RTC_SetTime((RTC_HandleTypeDef *)rtc->priv, &time_st, format);
        ret2 = HAL_RTC_SetDate((RTC_HandleTypeDef *)rtc->priv, &data_st, format);

        if ((ret1 == 0) && (ret2 == 0)) {
            ret = 0;
        } else {
            ret = -1;
        }
    }

    return ret;
}

int32_t hal_rtc_finalize(rtc_dev_t *rtc)
{
    int32_t ret = -1;

    if (rtc == NULL) {
        return -1;
    }

    ret = rtc_DeInit();

    return ret;
}

int32_t rtc_Init(rtc_dev_t *rtc)
{
    int32_t ret = 0;

    rtc_handle.Instance = RTC;
	
    rtc_handle.Init.HourFormat     = RTC_INIT_HOUR_FORMAT;
    rtc_handle.Init.AsynchPrediv   = RTC_INIT_ASYNC_PREDIV;
    rtc_handle.Init.SynchPrediv    = RTC_INIT_SYNC_PREDIV;
    rtc_handle.Init.OutPut         = RTC_INIT_OUTPUT;
    rtc_handle.Init.OutPutPolarity = RTC_INIT_OUTPUT_PRI;
    rtc_handle.Init.OutPutType     = RTC_INIT_OUTPUT_TYPE;

    ret = HAL_RTC_Init(&rtc_handle);

    return ret;
}

int32_t rtc_DeInit(void)
{
    int32_t ret = -1;

    /* adc1 deinitialization */
    ret = HAL_RTC_DeInit(&rtc_handle);

    return ret;
}

static int32_t rtc_format_transform(uint16_t format_hal, uint16_t *format_stm32l4)
{
    uint16_t format = 0;
    int32_t	ret = 0;

    if(format_hal == HAL_RTC_FORMAT_DEC)
    {
        format = RTC_FORMAT_BIN;
    }
    else if(format_hal == HAL_RTC_FORMAT_BCD)
    {
        format = RTC_FORMAT_BCD;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *format_stm32l4 = format;
    }

    return ret;
}
