/*
 * spiDriverMAX.h
 *
 *  Created on: 14 ����. 2016
 *      Author: Mishko
 *      This file contains the structures
 *      and functions for the SPI interface settings
 *      and gyroscope as angular velocity sensor.
 */
#include "F2806x_Spi.h"

#define SPIA_BASE_ADDR (0x00007040)
#define SPIB_BASE_ADDR (0x00007740)


SPI_REGS *spiaRegs;


typedef struct SPIData_{

	long addr, writeData;
	int xH, xL, yH, yL, tH, tL;
	int xData, yData, tData;
	double xAngle, yAngle, temperature;

	int spiI;
	float temp;

	int spiID;
	int selfTestRes;
	int selfTestData[4];

	long POWER_CFG, SENSE_CFG1, SENSE_CFG2, SENSE_CFG3, DR_CFG;


}SPIData;

//extern
SPIData *spiData;
int yData, xData;
int delay = 200, counter;

int gyroVerifyL2G2IS();
void spiGyroConfL2G2IS();



void spiInit()
{
	spiaRegs = (SPI_REGS*) SPIA_BASE_ADDR;
}


static inline void spiSetup()
{
	spiaRegs->SPICCR.all =0x004F;	             // Reset on, rising edge, 16-bit char bits
	spiaRegs->SPICTL.all =0x0006;    		     // Enable master mode, normal phase,
												// enable talk, and SPI int disabled.
//	spiaRegs->SPIBRR =0x0018;
	spiaRegs->SPIBRR =0x0070;

	spiaRegs->SPIPRI.bit.TRIWIRE = 0;			//4-wire mode
	spiaRegs->SPIPRI.bit.STEINV = 0;
	spiaRegs->SPIPRI.bit.FREE = 1;                // Set so breakpoints don't disturb xmission
	spiaRegs->SPICTL.bit.TALK = 1;
	spiaRegs->SPICCR.all =0x00CF;		         // Relinquish SPI from Reset
}

void spiSetupFF()
{
	spiaRegs->SPIFFTX.all=0xE440;
	spiaRegs->SPIFFRX.all=0x2024;
	spiaRegs->SPIFFCT.all=0x08;
}

void spiDisableFF()
{
	spiaRegs->SPIFFTX.all=0xA000;
	spiaRegs->SPIFFRX.all=0x201F;
	spiaRegs->SPIFFCT.all=0x00;
}

static inline void spiWait()
{
//	while((spiaRegs->SPISTS.bit.INT_FLAG)==0){};
	for(counter = delay; counter>0;) counter--;
}

static inline void setSPIData(SPIData* spiD)
{
	spiData = spiD;
}

static inline Uint16 spiWrite(int addr, int byte)
{
	addr = (addr<<8);
	addr = addr|byte;
	spiaRegs->SPITXBUF = addr;
	spiaRegs->SPISTS.bit.BUFFULL_FLAG = 1;
	spiWait();
	return spiaRegs->SPIRXBUF;
}

static inline void spiWriteFF(int addr, int byte)
{

	addr = (addr<<8)|0x4000;
	addr = addr|byte;
	spiaRegs->SPITXBUF = addr;
}

static inline void spiReadWriteFF(int addr)
{

	addr = (addr<<8)|0x8000;
	spiaRegs->SPITXBUF = addr;
}

static inline Uint16 spiRead(int addr)
{
	Uint16 byte = 0;
	addr = (addr<<8)|0x8000;//odd parity
	byte = spiaRegs->SPIRXBUF;
	spiaRegs->SPITXBUF = addr;
//	spiaRegs->SPISTS.bit.BUFFULL_FLAG = 1;
	spiWait();
//	while((spiaRegs->SPISTS.bit.INT_FLAG)!=1){}
	byte = spiaRegs->SPIRXBUF;

	byte = byte&0x00FF;
	return byte;
}

static inline Uint16 spiReadFF()
{
	return spiaRegs->SPIRXBUF;
}

static inline float readGyroX()
{
	spiData->xH = spiRead( 0x04);
	spiData->xL = spiRead( 0x03);
	spiData->xData = (spiData->xH<<8)|spiData->xL;
//	spiData->xAngle = spiData->xData;
	return spiData->xData;
}

static inline float readGyroY()
{
	spiData->yH = spiRead( 0x06);
	spiData->yL = spiRead( 0x05);
	spiData->yData = (spiData->yH<<8)|spiData->yL;
//	spiData->yAngle = spiData->yData/(1000.0/(2^((unsigned)spiData->POWER_CFG>>6)));
	return spiData->yData;
}

static inline float readGyroT()
{
	spiData->tH = spiRead( 0x02);
	spiData->tL = spiRead( 0x01);
	spiData->tData = ((spiData->tH<<8)|spiData->tL)>>4;
	return spiData->tData;
}

static inline void gyroUpdateData()
{
	for(;;)
	{
		if(spiaRegs->SPIFFRX.bit.RXFFST==0)
			break;
		int a = spiaRegs->SPIRXBUF;
	}
	spiReadWriteFF(0x04);
	spiReadWriteFF(0x03);
	spiReadWriteFF(0x06);
	spiReadWriteFF(0x05);
//	spiWrite( 0x13, 0x00);
}



static inline int gyroVerifyMAX()
{
//	spiDisableFF();

	spiData->spiID = spiRead( 0x20);

	spiData->POWER_CFG = spiRead( 0x00);
	if(spiData->POWER_CFG==0)
		spiWrite( 0x00, 0x0F);
	spiData->SENSE_CFG1 = spiRead( 0x01);
	spiData->SENSE_CFG2 = spiRead( 0x02);
	spiData->SENSE_CFG3 = spiRead( 0x03);
	spiData->DR_CFG = spiRead( 0x13);
//	spiSetupFF();
	return spiData->spiID;
}

int gyroVerifyL2G2IS()
{
	spiData->spiID = spiRead( 0x00);

	spiData->SENSE_CFG1 = spiRead( 0x0B);
	spiData->SENSE_CFG2 = spiRead( 0x0C);
	spiData->SENSE_CFG3 = spiRead( 0x0D);
	spiData->DR_CFG = spiRead( 0x1F);
//	spiSetupFF();
	return spiData->spiID;
}

static inline int gyroSelfTest()
{
	spiData->selfTestRes = 0;
	spiWrite( 0x00, 0x0F);//power mode
	spiWrite( 0x01, 0x68);//self test enable, direct mode, optical stabilization disable
	spiData->temperature = readGyroT()/256.0;
	readGyroX();
	readGyroY();
	spiData->selfTestData[0] = spiData->xData;
	spiData->selfTestData[1] = spiData->yData;
//	if(!((spiData->xAngle>250.0)&(spiData->xAngle<750.0)))
//		spiData->selfTestRes += 0x01;
//	if(!((spiData->yAngle<-250.0)&(spiData->yAngle>-750.0)))
//			spiData->selfTestRes += 0x02;
//	if(spiData->selfTestRes==0)
	{
//		spiWait(spiHandle);
		spiWrite( 0x01, 0xC8);//self test enable, back mode, optical stabilization disable
		readGyroX();
		readGyroY();
		spiData->temp = readGyroT()/256.0;
		spiData->selfTestData[2] = spiData->xData;
		spiData->selfTestData[3] = spiData->yData;
//		if(!((spiData->xAngle<-250.0)&(spiData->xAngle>-750.0)))
//			spiData->selfTestRes += 0x10;
//		if(!((spiData->yAngle>250.0)&(spiData->yAngle<750.0)))
//			spiData->selfTestRes += 0x20;
//		if(!((spiData->temp>spiData->temperature-1.0)&(spiData->temp<spiData->temperature+1.0)))
//			spiData->selfTestRes += 0x80;
	}
	return spiData->selfTestRes;
}

void spiGyroConfMAX()
{
	spiWrite( 0x21, 0x00);//bank select
	spiWrite( 0x00, 0x07);//power mode
	spiWrite( 0x01, 0x28);//filter enable/0x28-100Hz optical stabilization disable
	spiWrite( 0x02, 0x02);//data update freq //!!!!!!!
	spiWrite( 0x03, 0x00);//filtrer enabling
	spiWrite( 0x13, 0x00);//data reset & temperature sensor enabling
	spiWrite( 0x14, 0x20);//interrupt INT1 enabling
	spiWrite( 0x16, 0x00);//4wire spi, endian bit=0

	spiWrite( 0x00, 0x07);//power mode
	spiWrite( 0x01, 0x29);//self tedt dis, low freq filter 10Hz, optical stab en, max speed 125dps
//	spiWrite( 0x01, 0x3D);//self tedt dis, low freq filter 400Hz, optical stab en, max speed 125dps
//	spiWrite( 0x01, 0x29);//self tedt dis, low freq filter 100Hz, optical stab en, max speed 125dps
	spiWrite( 0x00, 0x0F);//power mode
}

void spiGyroConfL2G2IS()
{
	spiWrite(0x0B, 0x03); //power on comand
	spiWrite(0x0C, 0x80); //config LPF-O for 1 and disable high-pass filter
	spiWrite(0x0D, 0x01); //disable data RDY signal and enable high-pass filter
	spiWrite(0x10, 0x00); //orientation config
	spiWrite(0x1F, 0x08); //set scale 200 dgps


}
