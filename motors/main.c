/*
 * main.c
 */

#include "memCopy.h"
#include "gpioDriver.h"
#include "adcDriver.h"
#include "clkDriver.h"
#include "mcbspDriver.h"
#include "spiDriverMAX.h"
#include "sciDriver.h"
#include "systemDriver.h"
#include "interruptDriver.h"
#include "timerDriver.h"
#include "pwmDriver.h"
#include "scidata.h"
#include "crs8.h"
#include "F2806x_SysCtrl.h"

#include "main.h"

long j, jj;
long i;

#define angArrAvr 10
short angArr[angArrAvr];
short angArrCnt;

//#ifdef BETA
float alphaF(float a)
{
    return
    		-1.3268
    		+41.0484*a
    		+2.04995*a*a
    		-55.02655*a*a*a
    		-37.36914*a*a*a*a
    		+443.73994*a*a*a*a*a
    		+91.25246*a*a*a*a*a*a
    		-880.77533*a*a*a*a*a*a*a;
}
//#endif

    void main(void) {

    systemInit();
    systemSetup();

    for(i = 0; i<4000;i++)
    {
//        for(j = 0; j<10000;j++)
            for(jj = 0; jj<1000;)jj++;
    }



    interrupsInit();
    interrupsCtrlInit();

    EALLOW;  // This is needed to write to EALLOW protected registers
    interrupSVectTable->TINT0 = &cpu_timer0_isr;
    interrupSVectTable->TINT1 = &cpu_timer1_isr;
    interrupSVectTable->TINT2 = &cpu_timer2_isr;
    interrupSVectTable->SCIRXINTA = &SCI_RX_isr;
    //  interrupSVectTable->SPIRXINTA = &SPI_RX_isr;
    interrupSVectTable->XINT2 = &GYRO_X_isr;
    interrupSVectTable->ADCINT1 = &adc_X_ISR;
    EDIS;


#ifdef FLASH
    flashInit();
    flashSetup();
#endif

    gpioInit();
    setupGPIOs();
    setupGPIOsInterrupt();

    mcbspaInit();
    mcbspaSetup();
    mcbspSetupSpi();

    AMCconf();

    setSPIData(&spiModule);
    spiInit();
    spiSetup();
    spiGyroConfL2G2IS();
    gyroVerifyL2G2IS();
    spiSetupFF();

    sciaInit();
    sciaSetup();

    timerInit();
    timerSConfig();

    pwmSInit();
    pwmSetupWTest();

    adcInit();
    ADC_config();

    zeroStart();

    interruptSEnable();
    interrupsCpuSetup();

    adcRegs->ADCINTFLGCLR.bit.ADCINT1 = 1;      //Clear ADCINT1 flag reinitialize for next SOC

    angArrCnt = 0;


    for(;;)
    {
        adcRes[0] = adcRead(0);
        adcRes[1] = adcRead(1);
        adcRes[2] = adcRead(2);

        adcRes[3] = adcRead(3);
        adcRes[4] = adcRead(4);
        adcRes[6] = adcRead(6);

        adcRes[8] = adcRead(8);
        adcRes[9] = adcRead(9);
        adcRes[10] = adcRead(10);

        adcRes[11] = adcRead(11);
        adcRes[12] = adcRead(12);
        adcRes[14] = adcRead(14);
        //      gyroVerify();

        makeTest();

        if(mode == GoTestMode) //<<<<<<<<<<<<<<<<<<<TEST>>>>>>>>>>>>>>>>
        {
            vel = 30;
            moveMode = MoveNotDef;
            motor0.position = motor0.leftPos;

            if((alpha<motor0.position)&(moveMode == MoveNotDef))
            {
                motor0.velocity = +vel;
                moveMode = MoveUpStable;
            }
            if((alpha>motor0.position)&(moveMode == MoveNotDef))
            {
                motor0.velocity = -vel;
                moveMode = MoveDownStable;
            }
            for(;;)
                if((moveMode == MoveUpZero)|(moveMode == MoveDownZero))
                    break;

            moveMode = MoveNotDef;
            motor0.position = motor0.rightPos;
            if((alpha<motor0.position)&(moveMode == MoveNotDef))
            {
                motor0.velocity = +vel;
                moveMode = MoveUpStable;
            }
            if((alpha>motor0.position)&(moveMode == MoveNotDef))
            {
                motor0.velocity = -vel;
                moveMode = MoveDownStable;
            }
            for(;;)
                if((moveMode == MoveUpZero)|(moveMode == MoveDownZero))
                    break;

            mode = GoZeroMode;
            moveMode = MoveNotDef;

            vel = 60;
        }

        //////////<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<go Zero>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        {
            if(((fabs(motor0.position-alpha)<2.)|(SAAD_CTRL_ALL.CTRL.bit.LOCK_DEV==0))&(mode == GoIdleMode)&(lockDevEn == 1))
                lockDevStart();
            if(((fabs(motor0.position-alpha)<2.)|(SAAD_CTRL_ALL.CTRL.bit.LOCK_DEV==1))&(mode == GoZeroMode)&(lockDevEn == 1))
                lockDevStart();
        }

    }

}

__interrupt void cpu_timer0_isr(void)
{
    GPIO_setHigh(gpioS,LED3);
    GPIO_setHigh(gpioS,LED2);

    if((mode == GoZeroMode))
        motor0.position = 0.50;
    if((mode == GoScanMode))
    {
        if((alpha > motor0.leftPosScan-0.05))//-
        {
            if((moveMode == MoveNotDef))
                moveMode = MoveDownStable;
            motor0.velocity = -vel;
        }
        else
            if((alpha < motor0.rightPosScan+0.05))//+
        {
            if((moveMode == MoveNotDef))
                moveMode = MoveUpStable;
            motor0.velocity = +vel;
        }
        else
            moveMode = MoveNotDef;
    }

    if((mode == GoPositionMode)|(mode == GoZeroMode)|(mode == GoTestMode))
    {
        if(moveMode != MoveModeStab)
        {
            if((fabs(alpha-motor0.position)<0.05))
            {
                motor0.velocity = 0;
                if(mode == GoTestMode)
                {
                    if(moveMode == MoveDownStable)
                        moveMode = MoveDownZero;
                    if(moveMode == MoveUpStable)
                        moveMode = MoveUpZero;
                }
                if((mode == GoZeroMode)&(lockDevEn == 0))
                {
                    lockDevSend = 1;
                }
                if(SAAD_CTRL_ALL.CTRL.bit.AVTO == 1)
                    moveMode = MoveModeStab;
            }
            else
                if(alpha<motor0.position-0.05)
                {
                    if(mode == GoTestMode)
                        motor0.velocity = +vel;
                    else
                        moveMode = MoveUpAcel;
                }
                else
                    if(alpha>motor0.position+0.05)
                    {
                        if(mode == GoTestMode)
                            motor0.velocity = -vel;
                        else
                            moveMode = MoveDownAcel;
                    }
            if(((mode == GoPositionMode)|(mode == GoZeroMode))&(moveMode != MoveModeStab))
                calcSpeed();
        }
        if(moveMode == MoveModeStab)
        {
            motor0.velocity = 0;
        }
    }

    if(mode == NoNameMode)
    {
        //      alpha = 0;
        motor1.phaseZero += motor1.phasePosStep;
        if(motor1.phaseZero > 360.)
            motor1.phaseZero = 0;
        motor0.phaseZero += motor0.phasePosStep;
        if(motor0.phaseZero > 360.)
            motor0.phaseZero = 0;
    }
    //==========================================================0============================================

    abc[0].data[0] = (double)((adcRead(0)-2044)) /240.;
    abc[0].data[1] = (double)((adcRead(1)-2051)) /240.;//1819.
    abc[0].data[2] = (double)((adcRead(2)-2051)) /240.;
    abc[1].data[0] = (double)((adcRead(8)-2039)) /240.;
    abc[1].data[1] = (double)((adcRead(9)-2044)) /240.;
    abc[1].data[2] = (double)((adcRead(10)-2045)) /240.;

//    GPIO_setLow(gpioS,LED3);

    if((mode == GoIdleMode))
    {
        pidQ[0].Ui = 0;
        pidQ[1].Ui = 0;
        pidQ[0].Ref = 0;
        pidQ[1].Ref = 0;
    }
    else
    {
        iTot[0] = abc[0].data[0]+abc[0].data[1]+abc[0].data[2];
        alBe[0] = clarke(abc[0]);
        dq[0] = park(alBe[0],(alpha+motor0.phaseZero)*DEG2RAD*motor0.polesCount);

        pidD[0].Fdb = dq[0].data[0];
        pidD[0].Ref = refD[0];
        pid_reg3_calc(&pidD[0]);
        dqP[0].data[0] = pidD[0].Out;

        pidVel[0].Ref = motor0.velocity;
        pidVel[0].Fdb = ((float)saadFrameSend.VELOCITY.all)*0.01-motor0.velCorrection;
        pid_reg3_calc(&pidVel[0]);
        if(mode != NoNameMode)
            refQ[0] = pidVel[0].Out;


        pidQ[0].Fdb = dq[0].data[1];
        pidQ[0].Ref = refQ[0];
        pid_reg3_calc(&pidQ[0]);
        dqP[0].data[1] = pidQ[0].Out;
        alBeI[0] = parkInv(dqP[0], (alpha+motor0.phaseZero)*DEG2RAD*motor0.polesCount);
        abcI[0] = clarkeInv(alBeI[0]);
        //==========================================================1============================================
        iTot[1] = abc[1].data[0]+abc[1].data[1]+abc[1].data[2];
        alBe[1] = clarke(abc[1]);
        dq[1] = park(alBe[1],(alpha+motor1.phaseZero)*DEG2RAD*motor1.polesCount);
        pidD[1].Fdb = dq[1].data[0];
        pidD[1].Ref = refD[1];
        pid_reg3_calc(&pidD[1]);
        dqP[1].data[0] = pidD[1].Out;

        pidVel[1].Ref = motor0.velocity;
        pidVel[1].Fdb = ((float)saadFrameSend.VELOCITY.all)*0.01-motor0.velCorrection;
        pid_reg3_calc(&pidVel[1]);
        if(mode != NoNameMode)
            refQ[1] = pidVel[1].Out;


        pidQ[1].Fdb = dq[1].data[1];
        pidQ[1].Ref = refQ[1];
        pid_reg3_calc(&pidQ[1]);
        dqP[1].data[1] = pidQ[1].Out;
        alBeI[1] = parkInv(dqP[1], (alpha+motor1.phaseZero)*DEG2RAD*motor1.polesCount);
        abcI[1] = clarkeInv(alBeI[1]);
    }

    motor0.pwmValues.pwmV[0] = (abcI[0].data[0])*motor0.pwmData;
    motor0.pwmValues.pwmV[1] = (abcI[0].data[1])*motor0.pwmData;
    motor0.pwmValues.pwmV[2] = (abcI[0].data[2])*motor0.pwmData;

    motor1.pwmValues.pwmV[0] = (abcI[1].data[0])*motor1.pwmData;
    motor1.pwmValues.pwmV[1] = (abcI[1].data[1])*motor1.pwmData;
    motor1.pwmValues.pwmV[2] = (abcI[1].data[2])*motor1.pwmData;

    GPIO_setLow(gpioS,LED2);

    if(((SAAD_CTRL_ALL.POWER==1)
            |((mode == GoZeroMode)&(SAAD_CTRL_ALL.CTRL.bit.LOCK_DEV == 1))
            |(mode == NoNameMode))
            &(mode!=GoIdleMode)
//            &(saadTests.all == 0x00)
            )
    {
        for(i = 0; i < 3; i++)
        {
            if(motor0.pwmValues.pwmV[i]<0)
                motor0.pwmValues.pwmV[i] = 0;
            if(motor1.pwmValues.pwmV[i]<0)
                motor1.pwmValues.pwmV[i] = 0;

            if(motor0.pwmValues.pwmV[i]>getMaxPWMVal(i))
                motor0.pwmValues.pwmV[i] = getMaxPWMVal(i);
            if(motor1.pwmValues.pwmV[i]>getMaxPWMVal(i))
                motor1.pwmValues.pwmV[i] = getMaxPWMVal(i);
        }
        setPWMValuesArr(motor0.pwmValues);
        setPWMValuesArr(motor1.pwmValues);
    }
    else
    {
        for(i = 0; i<6; i++)
            setPWMValues(i, 0);
    }
    GPIO_setLow(gpioS,LED3);
//    if(SAAD_CTRL_ALL.POWER)
//        GPIO_setLow(gpioS,LED1);
//    else
//        GPIO_setHigh(gpioS,LED1);
//    if(SAAD_CTRL_ALL.CTRL.bit.LOCK_DEV)
//        GPIO_setLow(gpioS,LED2);
//    else
//        GPIO_setHigh(gpioS,LED2);
//    if(mode == GoScanMode)
//        GPIO_setLow(gpioS,LED3);
//    else
//        GPIO_setHigh(gpioS,LED3);

}

__interrupt void cpu_timer2_isr(void)
{
//    GPIO_setHigh(gpioS,LED2);

    if(GPIO_read(gpioS, PS_ACK))
    {
//        mcbspData = mcbsp_read(0x00);
//        if(mcbspData.data1&0x1000)
            apsL = mcbsp_read(0x1D).data1;
//        if(mcbspData.data1&0x2000)
            apsR = mcbsp_read(0x1F).data1;

        alpha = (apsL*1.-apsR*1.)/1./(apsL*1.+apsR*1.);
        alpha1 = alpha;
        alpha = alphaF(alpha);
        if(angArrCnt<angArrAvr)
        {
            angArr[angArrCnt] = alpha*100;
            angArrCnt++;
        }
        else
        {
            for(i = 0; i<angArrAvr-1; i++)
                angArr[i] = angArr[i+1];
            angArr[angArrAvr-1] = alpha*100;
        }
        if(angArrCnt<angArrAvr)
            saadFrameSend.POSITION.all = (alpha*100);

        else
        {
            saadFrameSend.POSITION.all = 0;
            for(i = 0; i<angArrAvr; i++)
                saadFrameSend.POSITION.all += angArr[i];
            saadFrameSend.POSITION.all /= angArrAvr;
        }
    }
//    saadFrameSend.POSITION.all = (alpha1*10000);

    if(spiaRegs->SPIFFRX.bit.RXFFST == 4)
    {
        SPI_RX_isr();
        gyroUpdateData();
    }

    interrupS->PIEACK.all = PIEACK_GROUP1;
}

__interrupt void cpu_timer1_isr(void)
{

    if((sciaRegs->SCIRXST.bit.RXERROR)
            |(sciaRegs->SCIRXST.bit.BRKDT)
            |(sciaRegs->SCIRXST.bit.FE))
    {
        sciaSetup();
    }

    if(sciaRegs->SCIFFRX.bit.RXFFOVF)
    {
        sciaRead();sciaRead();sciaRead();sciaRead();
        sciaRegs->SCIFFRX.bit.RXFFOVRCLR = 1;
    }

    timer1cnt++;
    if((sciFramePart == 1)&
            (sciaRegs->SCIFFRX.bit.RXFFST<4))
    {
        sciaRead();sciaRead();sciaRead();sciaRead();
        sciFramePart = 0;
    }

    if((sciaRegs->SCIFFRX.bit.RXFFST>0)&
            (sciaRegs->SCIFFRX.bit.RXFFST<4))
    {
        sciFramePart = 1;
    }

    if(sciaRegs->SCIFFRX.bit.RXFFST==4)
    {
        sciFramePart = 0;

        saadFrameResive.COMMAND_BYTE.bit.COMMAND_H = (0x00FF)&sciaRead();
        saadFrameResive.DATA.bit.DATA_L = (0x00FF)&sciaRead();
        saadFrameResive.DATA.bit.DATA_H = (0x00FF)&sciaRead();
        saadFrameResive.CTRLSUM.bit.CTRLSUM_H = (0x00FF)&sciaRead();

        if((saadFrameResive.COMMAND_BYTE.bit.COMMAND_H == 0x10)
                |(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H == 0x20)
                |(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H == 0xC0)
                |(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H == 0x80)
                |(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H == 0x30)
                |(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H == 0x40))
            saadFrameSend.DATA.all = saadFrameResive.DATA.all;

        saadFrameSend.COMMAND_BYTE.bit.COMMAND_H = saadFrameResive.COMMAND_BYTE.bit.COMMAND_H+1;

//        if(saadFrameResive.CTRLSUM.bit.CTRLSUM_H==crc8DATA(saadFrameResive))
        if(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x52)
        {
            motor0.velCorrection = (((float)saadFrameResive.DATA.all)*0.01);
        }
        else
            if(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x50)
            {
                mode = GoPositionMode;
                if(SAAD_CTRL_ALL.CTRL.bit.SCAN)
                    vel = motor0.scanVel;
                else
                    vel = ((float)60.);
                if(saadFrameResive.POSITION.all != saadFrameResive.DATA.all)
                {
                    saadFrameResive.POSITION.all = saadFrameResive.DATA.all;
                    if(SAAD_CTRL_ALL.CTRL.bit.SCAN)
                    {
                        pidQ[0].OutIMax = 0.8;
                        pidQ[0].OutIMin = -0.8;
                        pidQ[1].OutIMax = 0.8;
                        pidQ[1].OutIMin = -0.8;
                        acel = 800.;
                    }
                    else
                    {
                        pidQ[0].OutIMax = 0.1;
                        pidQ[0].OutIMin = -0.1;
                        pidQ[1].OutIMax = 0.1;
                        pidQ[1].OutIMin = -0.1;
                        acel = 200.;
                    }
                    mode = GoPositionMode;
                    moveMode = MoveUpZero;
                    motor0.position =  (((float)saadFrameResive.DATA.all)*0.01);
                    if(motor0.position > motor0.leftPos)
                        motor0.position = motor0.leftPos;
                    if(motor0.position < motor0.rightPos)
                        motor0.position = motor0.rightPos;

                    motor0.rightPosScan = motor0.position;
                    motor0.leftPosScan = motor0.rightPosScan+motor0.scanDelta;

                    //                                          motor0.velocity = vel;
                }
                setSci = 1;
                sendSCI = 1;
            }
            else
                if(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x54)
                {
                    saadFrameResive.POSITION.all = saadFrameResive.DATA.all;
                    float ang =  (((float)saadFrameResive.DATA.all)*0.01);
                    if(motor0.rightPosScan+ang > motor0.leftPos)
                    {
                        motor0.leftPosScan = motor0.leftPos;
                        motor0.scanDelta = motor0.rightPosScan+ang - motor0.leftPos;
                    }
                    else
                        if(motor0.rightPosScan+ang < motor0.rightPos)
                        {
                            motor0.leftPosScan = motor0.rightPos;
                            motor0.scanDelta = motor0.rightPos - motor0.rightPosScan+ang;
                        }
                        else
                        {
                            motor0.leftPosScan = motor0.rightPosScan+ang;
                            motor0.scanDelta = ang;
                        }
                    setSci = 1;
                    sendSCI = 1;
                }
                else
                    if(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0xC0)
                    {
                        if(saadFrameResive.DATA.all==0xAAAA)
                        {
                            if(SAAD_CTRL_ALL.CTRL.bit.LOCK_DEV != 1)
                            {
                                SAAD_CTRL_ALL.CTRL.bit.LOCK_DEV = 1;
                                mode = GoZeroMode;
                                moveMode = MoveZeroBegin;
                                sendSCI = 1;
                                lockDevEn = 1;
                                lockDevSend = 0;
                                pidQ[0].OutIMax = 0.3;
                                pidQ[0].OutIMin = -0.3;
                                pidQ[1].OutIMax = 0.3;
                                pidQ[1].OutIMin = -0.3;
                                lockDevStepCount = 220;
                            }
                            else
                            {
                                lockDevSend = 1;
                                sendSCI = 1;
                            }
                        }
                        else
                            if(saadFrameResive.DATA.all==0xBBBB)
                            {
                                if(SAAD_CTRL_ALL.CTRL.bit.LOCK_DEV != 0)
                                {
                                    SAAD_CTRL_ALL.CTRL.bit.LOCK_DEV = 0;
                                    mode = GoIdleMode;
                                    sendSCI = 1;
                                    lockDevEn = 1;
                                    lockDevSend = 0;
                                    pidQ[0].OutIMax = 0.1;
                                    pidQ[0].OutIMin = -0.1;
                                    pidQ[1].OutIMax = 0.1;
                                    pidQ[1].OutIMin = -0.1;
                                    lockDevStepCount = 220;
                                }
                                else
                                {
                                    lockDevSend = 1;
                                    sendSCI = 1;
                                }
                            }
                            else
                                if(saadFrameResive.DATA.all==0xCCCC)
                                {
                                    sendSCI = 1;
                                    lockDevSend = 1;
                                }
                    }
                    else
                        if(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x80)
                        {
                            if(saadFrameResive.DATA.all==0xAAAA)
                                SAAD_CTRL_ALL.CTRL.bit.AVTO = 1;
                            else
                                if(saadFrameResive.DATA.all==0xBBBB)
                                {
                                    SAAD_CTRL_ALL.CTRL.bit.AVTO = 0;
                                    moveMode = MoveUpZero;
                                }
                            sendSCI = 1;
                        }
                        else
                            if(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x40)
                            {
                                if(saadFrameResive.DATA.all==0xAAAA)
                                    SAAD_CTRL_ALL.CTRL.bit.SCAN = 1;
                                else
                                    if(saadFrameResive.DATA.all==0xBBBB)
                                        SAAD_CTRL_ALL.CTRL.bit.SCAN = 0;
                                sendSCI = 1;
                            }
                            else
                                if(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x10)
                                {
                                    if(saadFrameResive.DATA.all==0xAAAA)
                                        SAAD_CTRL_ALL.POWER = 1;
                                    else
                                        if(saadFrameResive.DATA.all==0xBBBB)
                                            SAAD_CTRL_ALL.POWER = 0;
                                    sendSCI = 1;
                                }
                                else
                                    if(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x30)
                                    {
                                        if(saadFrameResive.DATA.all==0xAAAA)
                                        {
                                            if(testSciSend == 1)
                                            {
                                                makeTest();
                                                mode = GoTestMode;
                                                testSciSend = 0;
                                            }
                                        }
                                        if(saadFrameResive.DATA.all==0xBBBB)
                                        {
                                            mode = GoZeroMode;
                                            testSciSend = 1;
                                        }
                                        sendSCI = 1;
                                    }
                                    else
                                        if(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0xA0)
                                        {
                                            vel = motor0.scanVel;
                                            mode = GoScanMode;
                                            if(mode != GoScanMode)
                                                motor0.velocity = vel;

                                            pidQ[0].OutIMax = 0.8;
                                            pidQ[0].OutIMin = -0.8;
                                            pidQ[1].OutIMax = 0.8;
                                            pidQ[1].OutIMin = -0.8;
                                            setSci = 1;
                                            sendSCI = 1;
                                        }
                                        else
                                            if(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x70)
                                            {
                                                if((saadFrameResive.VELOCITY.all != saadFrameResive.DATA.all)|(mode == GoIdleMode))
                                                {
                                                    vel = ((float)saadFrameResive.DATA.all)*0.01;
                                                    motor0.scanVel = vel;
                                                    motor0.velocity = vel;
                                                    saadFrameResive.VELOCITY.all = saadFrameResive.DATA.all;
                                                    pidQ[0].OutIMax = 0.8;
                                                    pidQ[0].OutIMin = -0.8;
                                                    pidQ[1].OutIMax = 0.8;
                                                    pidQ[1].OutIMin = -0.8;
                                                }
                                                setSci = 1;
                                                sendSCI = 1;
                                            }
                                            else

                                                if(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x20)
                                                {
                                                    if(saadFrameResive.DATA.all==0xAAAA)
                                                        SAAD_CTRL_ALL.CTRL.bit.WORK = 1;
                                                    else
                                                        if(saadFrameResive.DATA.all==0xBBBB)
                                                        {
                                                            SAAD_CTRL_ALL.CTRL.bit.WORK = 0;
                                                            mode = GoZeroMode;
                                                            moveMode = MoveZeroBegin;
                                                        }
                                                    sendSCI = 1;
                                                }
    }

    if((timer1cnt>2)|(sendSCI == 1)|(sendSCI == 2)|(sendDubSCI == 1))
    {
        timer1cnt = 0;
        if((sendSCI>0)|(sendDubSCI == 1))
            if(sciaRegs->SCIFFTX.bit.TXFFST == 0)
            {
                if((saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x70)|(sendDubSCI == 1))
                {
                    sendDubSCI = 0;
                    saadFrameSend.DATA.all = saadFrameSend.VELOCITY.all;
                    saadFrameSend.COMMAND_BYTE.bit.COMMAND_H = 0x71;

                    sciaWrite(saadFrameSend.COMMAND_BYTE.bit.COMMAND_H);
                    sciaWrite(saadFrameSend.DATA.bit.DATA_L);
                    sciaWrite(saadFrameSend.DATA.bit.DATA_H);
                }
                else
                    if(((saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x50)|
                            (saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x52)|
                            (saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0xA0))
                            &(sendDubSCI == 0))
                    {
                        saadFrameSend.COMMAND_BYTE.bit.COMMAND_H = 0x51;
                        sendDubSCI = 1;
                        saadFrameSend.DATA.all = saadFrameSend.POSITION.all;
                        sciaWrite(saadFrameSend.COMMAND_BYTE.bit.COMMAND_H);
                        sciaWrite(saadFrameSend.DATA.bit.DATA_L);
                        sciaWrite(saadFrameSend.DATA.bit.DATA_H);
                    }
                    else
                        if((saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x54))
                        {
                            sciaWrite(saadFrameSend.COMMAND_BYTE.bit.COMMAND_H);
                            sciaWrite(saadFrameSend.DATA.bit.DATA_L);
                            sciaWrite(saadFrameSend.DATA.bit.DATA_H);
                        }
                        else
                            if((saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0xC0)&(lockDevSend == 1))
                            {
                                sciaWrite(saadFrameSend.COMMAND_BYTE.bit.COMMAND_H);
                                sciaWrite(saadFrameSend.DATA.bit.DATA_L);
                                sciaWrite(saadFrameSend.DATA.bit.DATA_H);
                            }
                            else
                                if(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x80)
                                {
                                    sciaWrite(saadFrameSend.COMMAND_BYTE.bit.COMMAND_H);
                                    sciaWrite(saadFrameSend.DATA.bit.DATA_L);
                                    sciaWrite(saadFrameSend.DATA.bit.DATA_H);
                                }
                                else
                                    if(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x10)
                                    {
                                        saadFrameSend.DATA.bit.DATA_L = SAAD_CTRL_ALL.CTRL.all;
                                        sciaWrite(saadFrameSend.COMMAND_BYTE.bit.COMMAND_H);
                                        sciaWrite(saadFrameSend.DATA.bit.DATA_L);
                                        sciaWrite(saadFrameSend.DATA.bit.DATA_H);
                                    }else
                                        if(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x40)
                                        {
                                            sciaWrite(saadFrameSend.COMMAND_BYTE.bit.COMMAND_H);
                                            sciaWrite(saadFrameSend.DATA.bit.DATA_L);
                                            sciaWrite(saadFrameSend.DATA.bit.DATA_H);
                                        }
                                        else
                                            if((saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x30))
                                            {
                                                if(mode != GoTestMode)
                                                {
                                                    if(saadTests.all == 0x00)
                                                        saadFrameSend.COMMAND_BYTE.bit.COMMAND_H = 0x31;
                                                    else
                                                    {
                                                        saadFrameSend.COMMAND_BYTE.bit.COMMAND_H = 0x32;
                                                        saadFrameSend.DATA.bit.DATA_L = saadTests.all;
                                                    }
                                                    sciaWrite(saadFrameSend.COMMAND_BYTE.bit.COMMAND_H);
                                                    sciaWrite(saadFrameSend.DATA.bit.DATA_L);
                                                    sciaWrite(saadFrameSend.DATA.bit.DATA_H);
                                                    testSciSend = 1;
                                                }
                                            }
                                            else
                                                if((saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0xA0)&
                                                        (saadFrameResive.DATA.all == 0xBBBB))

                                                {
                                                    sciaWrite(saadFrameSend.COMMAND_BYTE.bit.COMMAND_H);
                                                    sciaWrite(saadFrameSend.DATA.bit.DATA_L);
                                                    sciaWrite(saadFrameSend.DATA.bit.DATA_H);
                                                }
                                                else
                                                    if(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x20)
                                                    {
                                                        sciaWrite(saadFrameSend.COMMAND_BYTE.bit.COMMAND_H);
                                                        sciaWrite(saadFrameSend.DATA.bit.DATA_L);
                                                        sciaWrite(saadFrameSend.DATA.bit.DATA_H);
                                                    }
                if((saadFrameResive.COMMAND_BYTE.bit.COMMAND_H!=0x30)
                        &(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H!=0xC0))
                    sciaWrite(crc8DATA(saadFrameSend));

                if((sendSCI == 1)
                        &(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H!=0x30)
                        &(saadFrameResive.COMMAND_BYTE.bit.COMMAND_H!=0xC0))
                    sendSCI = 0;

                if((saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0x30)&(mode == GoZeroMode))
                {
                    sciaWrite(crc8DATA(saadFrameSend));
                    sendSCI = 0;
                }

                if((saadFrameResive.COMMAND_BYTE.bit.COMMAND_H==0xC0)&(lockDevSend == 1))
                {
                    sciaWrite(crc8DATA(saadFrameSend));
                    sendSCI = 0;
                    lockDevSend = 0;
                }


            }

    }
}

__interrupt void SCI_RX_isr(void)
{
    interrupS->PIEACK.all = PIEACK_GROUP9;
}

//__interrupt
void SPI_RX_isr(void)
{
	spiData->xH = 0x00FF&spiaRegs->SPIRXBUF;
	spiData->xL = 0x00FF&spiaRegs->SPIRXBUF;
	spiData->yH = 0x00FF&spiaRegs->SPIRXBUF;
	spiData->yL = 0x00FF&spiaRegs->SPIRXBUF;

	spiData->xData = (spiData->xH<<8)|spiData->xL;
	xData = (int)spiData->xData;
	spiData->yData = (spiData->yH<<8)|spiData->yL;
	yData = (int)spiData->yData;
	saadFrameSend.VELOCITY.all = (spiData->xData+183)/(1.31);

    spiaRegs->SPIFFRX.bit.RXFFINTCLR = 1;
    interrupS->PIEACK.all = PIEACK_GROUP6;
}

__interrupt void GYRO_X_isr(void)
{
    interrupS->PIEACK.all = PIEACK_GROUP1;
}

void adc_X_ISR()
{
    adcRegs->ADCINTFLGCLR.bit.ADCINT1 = 1;      //Clear ADCINT1 flag reinitialize for next SOC
    interrupS->PIEACK.all = PIEACK_GROUP1;

}

void lockDevStart()
{
    int iii, jjj;
//    GPIO_setHigh(gpioS,LD_SLEEP);
    for(iii = 1000;iii>0;)iii--;
    GPIO_read(gpioS,LD_FAULT);
//    if(SAAD_CTRL_ALL.CTRL.bit.LOCK_DEV == 1)
//        GPIO_setHigh(gpioS, LD_DIR);
//    else
//        GPIO_setLow(gpioS, LD_DIR);
    for(iii = 1000;iii>0;)iii--;
    for(jjj = lockDevStepCount; jjj>0;jjj--)
    {
//        GPIO_setHigh(gpioS, LD_STEP);
        for(iii = 4300;iii>0;)iii--;
//        GPIO_setLow(gpioS, LD_STEP);
        for(iii = 4300;iii>0;)iii--;
    }
//    GPIO_setLow(gpioS, LD_SLEEP);
//    GPIO_setLow(gpioS, LD_DIR);
    if(SAAD_CTRL_ALL.CTRL.bit.LOCK_DEV == 1)
    {
    	GPIO_setHigh(gpioS, LD_STEP);
    	for(jjj = 200;jjj>0;jjj--)
    		for(iii = 10000;iii>0;)iii--;
    }
    else
    {
    	GPIO_setHigh(gpioS, LD_DIR);
    	for(jjj = 400;jjj>0;jjj--)
    		for(iii = 10000;iii>0;)iii--;
    }
    GPIO_setLow(gpioS, LD_STEP);
    GPIO_setLow(gpioS, LD_DIR);

    moveMode = MoveZeroEnd;
    lockDevEn = 0;
    lockDevSend = 1;
    if(SAAD_CTRL_ALL.CTRL.bit.LOCK_DEV == 1)
        mode = GoIdleMode;
    else
        mode = GoZeroMode;
}


void calcSpeed()
{
	motor0.velocity = (motor0.position-alpha)/velCoef;
	if(motor0.velocity>vel)
		motor0.velocity = vel;
	if(motor0.velocity<-vel)
		motor0.velocity = -vel;

//	if((motor0.velocity < vel)&(moveMode == MoveUpAcel))
//	{
//		motor0.aceleration = +acel;
//		motor0.velocity = motor0.velocity + motor0.aceleration*dt;
//	}
//	else
//	{
//		if(moveMode == MoveUpAcel)
//			moveMode = MoveUpStable;
//		if(moveMode == MoveUpStable)
//		{
//			motor0.velocity = vel;
//			motor0.aceleration = -acel;
//		}
//	}
//	if((alpha>=(motor0.position-motor0.velocity*motor0.velocity*0.5/acel))&(motor0.velocity>0))
//	{
//		if((moveMode == MoveUpStable)|(moveMode == MoveUpAcel))
//			moveMode = MoveUpDecel;
//		if(moveMode == MoveUpDecel)
//		{
//			motor0.aceleration = -acel;
//			motor0.velocity = motor0.velocity + motor0.aceleration*dt;
//		}
//	}
//	if((motor0.velocity<=0))//|(alpha>=motor0.position))
//		{
//		if((moveMode == MoveUpDecel))
//			moveMode = MoveUpZero;
//		if(moveMode == MoveUpZero)
//		{
//			motor0.velocity = 0;
//		}
//		}
//
//	if((motor0.velocity > -vel)&(moveMode == MoveDownAcel))
//	{
//		motor0.aceleration = -acel;
//		motor0.velocity = motor0.velocity + motor0.aceleration*dt;
//	}
//	else
//	{
//		if(moveMode == MoveDownAcel)
//			moveMode = MoveDownStable;
//		if(moveMode == MoveDownStable)
//		{
//			motor0.velocity = -vel;
//			motor0.aceleration = +acel;
//		}
//	}
//	if((alpha<=(motor0.position+motor0.velocity*motor0.velocity*0.5/acel))&(motor0.velocity<0))
//	{
//		if((moveMode == MoveDownStable)|(moveMode == MoveDownAcel))
//			moveMode = MoveDownDecel;
//		if(moveMode == MoveDownDecel)
//		{
//			motor0.aceleration = +acel;
//			motor0.velocity = motor0.velocity + motor0.aceleration*dt;
//		}
//	}
//	if((motor0.velocity>=0))//|(alpha<=motor0.position))
//		{
//		if(moveMode == MoveDownDecel)
//			moveMode = MoveDownZero;
//		if(moveMode == MoveDownZero)
//		{
//			motor0.velocity = 0;
//		}
//		}
}
