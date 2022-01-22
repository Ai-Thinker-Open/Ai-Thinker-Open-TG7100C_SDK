 /*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Generic lora driver implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis, Gregory Cristian and Wael Guibene
*/
/******************************************************************************
  * @file    timeserver.c
  * @author  MCD Application Team
  * @version V1.1.1
  * @date    01-June-2017
  * @brief   Time server infrastructure
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <time.h>
#include "hw.h"
#include "timeServer.h"
#include "lorawan_port.h"

/*!
 * safely execute call back
 */
#define exec_cb( _callback_ )   \
do                              \
{                               \
    if( _callback_ == NULL )    \
    {                           \
        while(1);               \
    }                           \
    else                        \
    {                           \
        _callback_( );          \
    }                           \
} while(0);                   



/*!
 * Timers list head pointer
 */
static TimerEvent_t *TimerListHead = NULL;

/*!
 * \brief Adds or replace the head timer of the list.
 *
 * \remark The list is automatically sorted. The list head always contains the
 *     next timer to expire.
 *
 * \param [IN]  obj Timer object to be become the new head
 * \param [IN]  remainingTime Remaining time of the previous head to be replaced
 */
static void TimerInsertNewHeadTimer( TimerEvent_t *obj );

/*!
 * \brief Adds a timer to the list.
 *
 * \remark The list is automatically sorted. The list head always contains the
 *     next timer to expire.
 *
 * \param [IN]  obj Timer object to be added to the list
 * \param [IN]  remainingTime Remaining time of the running head after which the object may be added
 */
static void TimerInsertTimer( TimerEvent_t *obj );

/*!
 * \brief Sets a timeout with the duration "timestamp"
 * 
 * \param [IN] timestamp Delay duration
 */
static void TimerSetTimeout( TimerEvent_t *obj );

/*!
 * \brief Check if the Object to be added is not already in the list
 * 
 * \param [IN] timestamp Delay duration
 * \retval true (the object is already in the list) or false  
 */
static bool TimerExists( TimerEvent_t *obj );



void TimerInit( TimerEvent_t *obj, void ( *callback )( void ) )
{
  obj->Timestamp = 0;
  obj->ReloadValue = 0;
  obj->IsRunning = false;
  obj->Callback = callback;
  obj->Next = NULL;
}

void TimerStart( TimerEvent_t *obj )
{
    uint32_t elapsedTime = 0;
    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    if( ( obj == NULL ) || ( TimerExists( obj ) == true ) )
    {
        RHINO_CPU_INTRPT_ENABLE();
        return;
    }

    obj->Timestamp = obj->ReloadValue;
    obj->IsRunning = false;

    if( TimerListHead == NULL )
    {
        aos_lrwan_time_itf.set_timer_context();
        TimerInsertNewHeadTimer( obj ); // insert a timeout at now+obj->Timestamp
    }
    else 
    {
        elapsedTime = aos_lrwan_time_itf.get_timer_elapsed_time();
        obj->Timestamp += elapsedTime;

        if( obj->Timestamp < TimerListHead->Timestamp )
        {
            TimerInsertNewHeadTimer( obj);
        }
        else
        {
            TimerInsertTimer( obj);
        }
    }
    RHINO_CPU_INTRPT_ENABLE();
}

static void TimerInsertTimer( TimerEvent_t *obj)
{
  TimerEvent_t* cur = TimerListHead;
  TimerEvent_t* next = TimerListHead->Next;

  while (cur->Next != NULL )
  {  
    if( obj->Timestamp  > next->Timestamp )
    {
        cur = next;
        next = next->Next;
    }
    else
    {
        cur->Next = obj;
        obj->Next = next;
        return;

    }
  }
  cur->Next = obj;
  obj->Next = NULL;
}

static void TimerInsertNewHeadTimer( TimerEvent_t *obj )
{
  TimerEvent_t* cur = TimerListHead;

  if( cur != NULL )
  {
    cur->IsRunning = false;
  }

  obj->Next = cur;
  TimerListHead = obj;
  TimerSetTimeout( TimerListHead );
}

void TimerIrqHandler( void )
{
    TimerEvent_t* cur;
    TimerEvent_t* next;

    uint32_t old =  aos_lrwan_time_itf.get_timer_context(); 
    uint32_t now =  aos_lrwan_time_itf.set_timer_context(); 
    uint32_t DeltaContext = now - old; //intentionnal wrap around

    /* update timeStamp based upon new Time Reference*/
    /* beacuse delta context should never exceed 2^32*/
    if ( TimerListHead != NULL )
    {
        for (cur=TimerListHead; cur->Next != NULL; cur= cur->Next)
        {
          next =cur->Next;
          if (next->Timestamp > DeltaContext)
          {
                next->Timestamp -= DeltaContext;
          }
          else
          {
                next->Timestamp = 0 ;
          }
        }
    }

    /* execute imediately the alarm callback */
    if ( TimerListHead != NULL )
    {
        cur = TimerListHead;
        TimerListHead = TimerListHead->Next;
        exec_cb( cur->Callback );
    }


    // remove all the expired object from the list
    while( ( TimerListHead != NULL ) && ( TimerListHead->Timestamp < aos_lrwan_time_itf.get_timer_elapsed_time(  )  ))
    {
        cur = TimerListHead;
        TimerListHead = TimerListHead->Next;
        exec_cb( cur->Callback );
    }

    /* start the next TimerListHead if it exists AND NOT running */
    if(( TimerListHead != NULL ) && (TimerListHead->IsRunning == false))
    {
        TimerSetTimeout( TimerListHead );
    }
}

void TimerStop( TimerEvent_t *obj ) 
{
    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    TimerEvent_t* prev = TimerListHead;
    TimerEvent_t* cur = TimerListHead;

    // List is empty or the Obj to stop does not exist 
    if( ( TimerListHead == NULL ) || ( obj == NULL ) )
    {
        RHINO_CPU_INTRPT_ENABLE();
        return;
    }

    if( TimerListHead == obj ) // Stop the Head                  
    {
        if( TimerListHead->IsRunning == true ) // The head is already running 
        {    
            if( TimerListHead->Next != NULL )
            {
                TimerListHead->IsRunning = false;
                TimerListHead = TimerListHead->Next;
                TimerSetTimeout( TimerListHead );
            }
            else
            {
                aos_lrwan_time_itf.stop_alarm( );
                TimerListHead = NULL;
            }
        }
        else // Stop the head before it is started
        {   
            if( TimerListHead->Next != NULL )   
            {
                TimerListHead = TimerListHead->Next;
            }
            else
            {
                TimerListHead = NULL;
            }
        }
    }
    else // Stop an object within the list
    {      
        while( cur != NULL )
        {
            if( cur == obj )
            {
                if( cur->Next != NULL )
                {
                    cur = cur->Next;
                    prev->Next = cur;
                }
                else
                {
                    cur = NULL;
                    prev->Next = cur;
                }
                break;
            }
            else
            {
                prev = cur;
                cur = cur->Next;
            }
        }   
    }

    RHINO_CPU_INTRPT_ENABLE();
}  
  
static bool TimerExists( TimerEvent_t *obj )
{
    TimerEvent_t* cur = TimerListHead;

    while( cur != NULL )
    {
        if( cur == obj )
        {
            return true;
        }
        cur = cur->Next;
    }
    return false;  
}

void TimerReset( TimerEvent_t *obj )
{
  TimerStop( obj );
  TimerStart( obj );
}

void TimerSetValue( TimerEvent_t *obj, uint32_t value )
{
    TimerStop( obj );
    
    aos_lrwan_time_itf.set_timer_val(obj, value);  
}

TimerTime_t TimerGetCurrentTime( void )
{
    return aos_lrwan_time_itf.get_current_time();   
}

TimerTime_t TimerGetElapsedTime( TimerTime_t past )
{
    return aos_lrwan_time_itf.compute_elapsed_time(past);    
}

static void TimerSetTimeout( TimerEvent_t *obj )
{
    aos_lrwan_time_itf.set_timeout(obj);   
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
