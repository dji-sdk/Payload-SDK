/**
 ********************************************************************
 * @file    sys_monitor.c
 * @brief
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


/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "sys_monitor.h"
#include "dji_logger.h"
#include "utils/util_misc.h"

/* Private constants ---------------------------------------------------------*/
#define MONITOR_VMRSS_LINE      15
#define MONITOR_PROCESS_ITEM    14
#define MONITOR_CMD_BUF_SIZE    512

/* Private types -------------------------------------------------------------*/


/* Private functions declaration ---------------------------------------------*/
static const char *Monitor_GetItems(const char *buffer, int ie);

/* Private variables ---------------------------------------------------------*/


/* Exported functions definition ---------------------------------------------*/
int Monitor_GetPhyMem(pid_t p)
{
    int i;
    char file[64] = {0};
    FILE *fd;
    char lineBuf[256] = {0};
    char name[32];
    int vmrss = 0; //memory peak
    char *ret = NULL;

    sprintf(file, "/proc/%d/status", (int) p);
    fd = fopen(file, "r");
    if (fd == NULL) {
        USER_LOG_ERROR("open file fail.");
        return 0;
    }

    for (i = 0; i < MONITOR_VMRSS_LINE - 1; i++) {
        ret = fgets(lineBuf, sizeof(lineBuf), fd);
        USER_UTIL_UNUSED(ret);
    }

    ret = fgets(lineBuf, sizeof(lineBuf), fd);
    if (ret == NULL)
        goto out;

    sscanf(lineBuf, "%31s %d", name, &vmrss);

out:
    fclose(fd);

    return vmrss;
}

int Monitor_GetTotalMem(void)
{
    char *file = "/proc/meminfo";
    FILE *fd;
    char lineBuf[256] = {0};
    char name[32];
    int memtotal = 0;
    char *ret = NULL;

    fd = fopen(file, "r");
    if (fd == NULL) {
        USER_LOG_ERROR("open file fail.");
        return 0;
    }

    ret = fgets(lineBuf, sizeof(lineBuf), fd);
    if (ret == NULL)
        goto out;

    sscanf(lineBuf, "%31s %d", name, &memtotal);

out:
    fclose(fd);

    return memtotal;
}

float Monitor_GetPmem(pid_t p)
{
    int phy = Monitor_GetPhyMem(p);
    int total = Monitor_GetTotalMem();
    float occupy = (float) ((phy * 1.0) / (total * 1.0));

    return occupy;
}

unsigned int Monitor_GetCpuOccupyOfProcess(pid_t pid)
{
    char file[64] = {0};
    T_MonitorProcessCpuOccupy t = {0};
    FILE *fd;
    char lineBuf[1024] = {0};
    char *q = NULL;
    char *ret = NULL;

    sprintf(file, "/proc/%d/stat", (int) pid);
    fd = fopen(file, "r");
    if (fd == NULL) {
        USER_LOG_ERROR("open file fail.");
        return 0;
    }

    ret = fgets(lineBuf, sizeof(lineBuf), fd);
    if (ret == NULL)
        goto out;

    sscanf(lineBuf, "%u", (unsigned int *) &t.pid);
    q = (char *) Monitor_GetItems(lineBuf, MONITOR_PROCESS_ITEM);
    if (q == NULL) {
        USER_LOG_ERROR("get item fail.");
        goto out;
    }

    sscanf(q, "%u %u %u %u", &t.utime, &t.stime, &t.cutime, &t.cstime);

out:
    fclose(fd);

    return (t.utime + t.stime + t.cutime + t.cstime);
}

unsigned int Monitor_GetCpuOccupyOfThread(pid_t pid, pid_t tid)
{
    char file[64] = {0};
    T_MonitorProcessCpuOccupy t = {0};
    FILE *fd;
    char lineBuf[1024] = {0};
    char *q = NULL;
    char *ret = NULL;

    sprintf(file, "/proc/%d/task/%d/stat", (int) pid, (int) tid);
    fd = fopen(file, "r");
    if (fd == NULL) {
        USER_LOG_ERROR("open file fail.");
        return 0;
    }

    ret = fgets(lineBuf, sizeof(lineBuf), fd);
    if (ret == NULL)
        goto out;

    sscanf(lineBuf, "%u", (unsigned int *) &t.pid);
    q = (char *) Monitor_GetItems(lineBuf, MONITOR_PROCESS_ITEM);
    if (q == NULL) {
        USER_LOG_ERROR("get item fail.");
        goto out;
    }

    sscanf(q, "%u %u %u %u", &t.utime, &t.stime, &t.cutime, &t.cstime);

out:
    fclose(fd);

    return (t.utime + t.stime + t.cutime + t.cstime);
}

unsigned int Monitor_GetCpuTotalOccupy(void)
{
    FILE *fd;
    char buff[1024] = {0};
    T_MonitorTotalCpuOccupy t = {0};
    char name[16];
    char *ret = NULL;

    fd = fopen("/proc/stat", "r");
    if (fd == NULL) {
        USER_LOG_ERROR("open file fail.");
        return 0;
    }

    ret = fgets(buff, sizeof(buff), fd);
    if (ret == NULL)
        goto out;

    sscanf(buff, "%15s %u %u %u %u", name, &t.user, &t.nice, &t.system, &t.idle);

out:
    fclose(fd);

    return (t.user + t.nice + t.system + t.idle);
}

float Monitor_GetPcpuOfThread(pid_t pid, pid_t tid)
{
    FILE *fp;
    char cmdStr[MONITOR_CMD_BUF_SIZE];
    char lineBuf[256] = {0};
    pid_t tidInCommandLine = 0;
    float pcpuInCommandLine = 0.0f;
    int ret;
    char *q = NULL;

    snprintf(cmdStr, MONITOR_CMD_BUF_SIZE, "ps -mp %d -o tid,pcpu", (int) pid);
    fp = popen(cmdStr, "r");
    if (fp == NULL) {
        USER_LOG_ERROR("fp is null.");
        return 0;
    }

    while (fgets(lineBuf, sizeof(lineBuf), fp) != NULL) {
        q = (char *) Monitor_GetItems(lineBuf, 1);
        if (q == NULL) {
            USER_LOG_ERROR("get item fail.");
            goto out;
        }

        sscanf(q, "%u", (unsigned int *) &tidInCommandLine);

        if (tidInCommandLine == tid) {
            q = (char *) Monitor_GetItems(lineBuf, 2);
            if (q == NULL) {
                USER_LOG_ERROR("get item fail.");
                goto out;
            }

            ret = sscanf(q, "%f", &pcpuInCommandLine);
            if (ret <= 0) {
                USER_LOG_ERROR("get pcpu error.");
                pcpuInCommandLine = 0;
            }

            goto out;
        }
    }

    USER_LOG_DEBUG("not found thread.");

out:
    pclose(fp);

    return pcpuInCommandLine;
}

unsigned int Monitor_GetThreadCountOfProcess(pid_t pid)
{
    FILE *fp;
    char cmdStr[MONITOR_CMD_BUF_SIZE];
    unsigned int count;
    int ret;

    snprintf(cmdStr, MONITOR_CMD_BUF_SIZE, "ps -T -p %d | wc -l", (int) pid);
    fp = popen(cmdStr, "r");
    if (fp == NULL) {
        USER_LOG_ERROR("fp is null.");
        return 0;
    }

    ret = fscanf(fp, "%u", &count);
    if (ret <= 0) {
        USER_LOG_ERROR("get count error.");
        count = 0;
        goto out;
    }
    count--;

out:
    pclose(fp);

    return count;
}

void Monitor_GetTidListOfProcess(pid_t pid, pid_t *tidList, unsigned int size)
{
    int i = 0;
    FILE *fp;
    char cmdStr[MONITOR_CMD_BUF_SIZE];
    char lineBuf[256] = {0};
    int ret = 0;

    if (Monitor_GetThreadCountOfProcess(pid) > size) {
        USER_LOG_ERROR("size is too small.");
        return;
    }

    snprintf(cmdStr, MONITOR_CMD_BUF_SIZE, "ps -mp %d -o tid", (int) pid);
    fp = popen(cmdStr, "r");
    if (fp == NULL) {
        USER_LOG_ERROR("fp is null.");
        return;
    }

    while (fgets(lineBuf, sizeof(lineBuf), fp) != NULL && i <= size) {
        ret = sscanf(lineBuf, "%u", (unsigned int *) &tidList[i]);
        if (ret > 0)
            i++;
    }

    pclose(fp);
}

void Monitor_GetNameOfThread(pid_t pid, pid_t tid, char *name, unsigned int size)
{
    char file[64] = {0};
    FILE *fd;
    char lineBuf[32] = {0};
    char *ret = NULL;

    memset(name, 0, size);

    sprintf(file, "/proc/%d/task/%d/comm", (int) pid, (int) tid);
    fd = fopen(file, "r");
    if (fd == NULL) {
        USER_LOG_DEBUG("open file fail.");
        return;
    }

    ret = fgets(lineBuf, sizeof(lineBuf), fd);
    if (ret == NULL)
        goto out;

    if (lineBuf[strlen(lineBuf) - 1] == '\n')
        lineBuf[strlen(lineBuf) - 1] = '\0';
    strncpy(name, lineBuf, USER_UTIL_MIN(size - 1, sizeof(lineBuf)));

out:
    fclose(fd);
}

/**
 * @brief
 * @param pid
 * @return Unit: B.
 */
unsigned int Monitor_GetHeapUsed(pid_t pid)
{
    FILE *fp;
    char cmdStr[MONITOR_CMD_BUF_SIZE];
    char lineBuf[256] = {0};
    int ret = 0;
    unsigned int heapUsed = 0;
    char *q = NULL;
    char *rett = NULL;

    snprintf(cmdStr, MONITOR_CMD_BUF_SIZE, "cat /proc/%d/smaps | grep -A 18 heap | grep Private_Dirty", (int) pid);
    fp = popen(cmdStr, "r");
    if (fp == NULL) {
        USER_LOG_ERROR("fp is null.");
        return 0;
    }

    rett = fgets(lineBuf, sizeof(lineBuf), fp);
    if (rett == NULL) {
        goto out;
    }

    q = (char *) Monitor_GetItems(lineBuf, 2);
    if (q == NULL) {
        USER_LOG_ERROR("get item fail.");
        goto out;
    }

    ret = sscanf(q, "%u", &heapUsed);
    if (ret <= 0) {
        USER_LOG_ERROR("can not find heapUsed.");
        heapUsed = 0;
        goto out;
    }
    heapUsed *= 1024;

out:
    pclose(fp);

    return heapUsed;
}

/**
 * @brief
 * @param pid
 * @return Unit: B.
 */
unsigned int Monitor_GetStackUsed(pid_t pid)
{
    FILE *fp;
    char cmdStr[MONITOR_CMD_BUF_SIZE];
    char lineBuf[256] = {0};
    int ret = 0;
    unsigned int stackUsed = 0;
    char *q = NULL;
    char *rett = NULL;

    snprintf(cmdStr, MONITOR_CMD_BUF_SIZE, "cat /proc/%d/smaps | grep -A 18 stack | grep Private_Dirty", (int) pid);
    fp = popen(cmdStr, "r");
    if (fp == NULL) {
        USER_LOG_ERROR("fp is null.");
        return 0;
    }

    rett = fgets(lineBuf, sizeof(lineBuf), fp);
    if (rett == NULL)
        goto out;

    q = (char *) Monitor_GetItems(lineBuf, 2);
    if (q == NULL) {
        USER_LOG_ERROR("get item fail.");
        goto out;
    }

    ret = sscanf(q, "%u", &stackUsed);
    if (ret <= 0) {
        USER_LOG_ERROR("can not find stackUsed.");
        stackUsed = 0;
        goto out;
    }
    stackUsed *= 1024;

out:
    pclose(fp);

    return stackUsed;
}

/* Private functions definition-----------------------------------------------*/
static const char *Monitor_GetItems(const char *buffer, int ie)
{
    int i = 0;
    int j = 0;
    char *p = (char *) buffer;
    int len = (int) strlen(buffer);
    int count = 0;

    if (1 == ie || ie < 1) {
        return p;
    }

    while (1) {
        for (i = j; i < len; i++) {
            if (*(buffer + i) != ' ') {
                count++;

                if (count == ie)
                    return buffer + i;

                break;
            }
        }

        for (j = i; j < len; ++j) {
            if (*(buffer + j) == ' ')
                break;
        }

        if (i == len || j == len)
            break;
    }

    return NULL;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
