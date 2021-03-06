#include <STC15W4K60S4.H>
#include <USART.h>
#include <intrins.h>    
bit busy;
void Usart_Init()  //波特率115200
{
  SCON = 0x50;	
	AUXR |= 0x40;		
	AUXR &= 0xFE;		
	TMOD &= 0x0F;	
	TL1 = 0xC3;		
	TH1 = 0xFF;		
	ET1 = 0;		
	TR1 = 1;	
  EA = 1;	
}
void Uart() interrupt 4 using 1
{
    if (RI)
    {
        RI = 0;                       
    }
    if (TI)
    {
        TI = 0;                
        busy = 0;             
    }
}
void SendData(unsigned char dat)
{
    while (busy);            
    ACC = dat; 
    busy = 1;
    SBUF = ACC;               
}
void Send(int Ax,int Ay,int Az,int Gx,int Gy,int Gz)
{
	unsigned char sum=0;
	ES = 1;  //打开串口中断
	SendData(0xAA);   //帧头
	SendData(0xAA);   //帧头
	SendData(0x02);   //功能字
	SendData(12);     //发送的数据长度
	SendData(Ax);     //低8位
	SendData(Ax>>8);  //高8位
	SendData(Ay);
	SendData(Ay>>8);
	SendData(Az);
	SendData(Az>>8);
	SendData(Gx);
	SendData(Gx>>8);
	SendData(Gy);
	SendData(Gy>>8);
	SendData(Gz);
	SendData(Gz>>8);
	sum+=0xAA;sum+=0xAA;sum+=0x02;sum+=12;
	sum+=Ax>>8;sum+=Ax;sum+=Ay>>8;sum+=Ay;sum+=Az>>8;sum+=Az;
	sum+=Gx>>8;sum+=Gx;sum+=Gy>>8;sum+=Gy;sum+=Gz>>8;sum+=Gz;
	SendData(sum);  //校验和
	ES = 0;   //关闭串口中断
}

