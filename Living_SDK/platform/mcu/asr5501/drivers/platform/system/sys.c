#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////     

//////////////////////////////////////////////////////////////////////////////////  


  
__asm void WFI_SET(void)
{
    WFI;          
}

__asm void INTX_DISABLE(void)
{
    CPSID   I
    BX      LR      
}

__asm void INTX_ENABLE(void)
{
    CPSIE   I
    BX      LR  
}

__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0             //set Main Stack value
    BX r14
}


// below code is for no need to select microlib
#if 0
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
 struct __FILE 
{ 
    int handle; 
}; 

 FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
    x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{     
    while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
    return ch;
}
#endif













