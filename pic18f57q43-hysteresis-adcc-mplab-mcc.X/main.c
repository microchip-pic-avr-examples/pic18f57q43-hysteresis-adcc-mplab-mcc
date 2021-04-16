/*
Copyright (c) [2012-2020] Microchip Technology Inc.  

    All rights reserved.

    You are permitted to use the accompanying software and its derivatives 
    with Microchip products. See the Microchip license agreement accompanying 
    this software, if any, for additional info regarding your rights and 
    obligations.
    
    MICROCHIP SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT 
    WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT 
    LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT 
    AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP OR ITS
    LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT 
    LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE 
    THEORY FOR ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT 
    LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, 
    OR OTHER SIMILAR COSTS. 
    
    To the fullest extend allowed by law, Microchip and its licensors 
    liability will not exceed the amount of fees, if any, that you paid 
    directly to Microchip to use this software. 
    
    THIRD PARTY SOFTWARE:  Notwithstanding anything to the contrary, any 
    third party software accompanying this software is subject to the terms 
    and conditions of the third party's license agreement.  To the extent 
    required by third party licenses covering such third party software, 
    the terms of such license will apply in lieu of the terms provided in 
    this notice or applicable license.  To the extent the terms of such 
    third party licenses prohibit any of the restrictions described here, 
    such restrictions will not apply to such third party software.
*/
#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"

//Should the UART print? Set by the ISR, cleared in main
static bool printResult = false;

//ISR called when the threshold interrupt is triggered
void onThreshold(void)
{
    printResult = true;
}

#define getUpperThreshold()  (ADUTH + ADSTPT)
#define getLowerThreshold()  (ADLTH + ADSTPT)
#define setpointADCC ADSTPT

//Set the setpoint register of the ADCC
#define setSetpointADCC(X) do { setpointADCC = X; } while (0)

//Set the Sampling Channel of the ADCC without starting a conversion
#define setSamplingChannelADCC(X) do { ADPCH = X; } while (0)

int main(void)
{
    SYSTEM_Initialize();

    //Select RA0 as the sampling input
    setSamplingChannelADCC(channel_ANA0);
    
    //Setup Threshold Interrupt
    ADCC_SetADTIInterruptHandler(&onThreshold);
    
    //Enable all Interrupts
    INTERRUPT_GlobalInterruptEnable();
    
    while(1)
    {
        if (printResult)
        {
            printResult = false;
            printf("Threshold Exceeded!\n\r");
            printf("ADCC Result: %d\n\r", ADCC_GetConversionResult());
            printf("Prev. Thresholds: %d to %d\n\r", 
                    getLowerThreshold(), 
                    getUpperThreshold());
            
            //Update the setpoint
            setSetpointADCC(ADCC_GetConversionResult());
            
            printf("New Thresholds: %d to %d\n\r\n\r", 
                    getLowerThreshold(), 
                    getUpperThreshold());
            
        }
    }    
}