/**
 ********************************************************************
 * @file    sys_monitor.h
 * @brief   This is the header file for "sys_monitor.c", defining the structure and
 * (exported) function prototypes.
 *
 * @copyright (c) 2021 DJI. All rights reserved.
 *
 * All information contained herein is, and remains, the property of DJI.
 * The intellectual and technical concepts contained herein are proprietary
 * to DJI and may be covered by U.S. and foreign patents, patents in process,
 * and protected by trade secret or copyright law.  Dissemination of this
 * information, including but not limited to data and other proprietary
 * material(s) incorporated within the information, in any form, is strictly
 * prohibited without the express written consent of DJI.
 *
 * If you receive this source code without DJI’s authorization, you may not
 * further disseminate the information, and you must immediately remove the
 * source code and notify DJI of its removal. DJI reserves the right to pursue
 * legal actions against you for any loss(es) or damage(s) caused by your
 * failure to do so.
 *
 *********************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SYS_MONITOR_H
#define SYS_MONITOR_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
typedef struct {
    unsigned int user;
    unsigned int nice;
    unsigned int system;
    unsigned int idle;
} T_MonitorTotalCpuOccupy;

typedef struct {
    pid_t pid;
    unsigned int utime;
    unsigned int stime;
    unsigned int cutime;
    unsigned int cstime;
} T_MonitorProcessCpuOccupy;

/* Exported functions --------------------------------------------------------*/
int Monitor_GetPhyMem(pid_t p);
int Monitor_GetTotalMem();
unsigned int Monitor_GetCpuTotalOccupy();
unsigned int Monitor_GetCpuOccupyOfProcess(pid_t pid);
unsigned int Monitor_GetCpuOccupyOfThread(pid_t pid, pid_t tid);
float Monitor_GetPcpuOfThread(pid_t pid, pid_t tid);
float Monitor_GetPmem(pid_t p);
unsigned int Monitor_GetThreadCountOfProcess(pid_t pid);
void Monitor_GetTidListOfProcess(pid_t pid, pid_t *tidList, unsigned int size);
void Monitor_GetNameOfThread(pid_t pid, pid_t tid, char *name, unsigned int size);
unsigned int Monitor_GetHeapUsed(pid_t pid);
unsigned int Monitor_GetStackUsed(pid_t pid);

#ifdef __cplusplus
}
#endif

#endif //SYS_MONITOR_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
