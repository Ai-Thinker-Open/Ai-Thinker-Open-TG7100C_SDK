#ifndef __RANDOM_H__
#define __RANDOM_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//  maintainer: Halley
#include "type.h"


//����ģ���������������������ʱ�����Դﵽ�����Ч������������Ŀ��Ϊn
//1. ����һ�׸������������������β���
//2. ����һ�׸��������β���֮����ഩ��(2*n - 2)���������
//3. ����һ�׸�������������n�׸����У���౻��������

//���1--MaxNum֮��Ĳ��ظ�α������С�
//ĳ�ε��ñ�����ʱ�����MaxNum������ǰһ�η����仯��
//���ʾҪ����һ���µ�α������У��ײ�����³�ʼ��α������з�������
uint16_t GetRandomNum(uint16_t seed, uint16_t MaxNum);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
