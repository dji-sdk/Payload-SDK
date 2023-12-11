/**
 ********************************************************************
 * @file    link_list.h
 * @brief   This is the header file for "link_list.c", defining the structure and
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
#ifndef UTIL_LINK_LIST_H
#define UTIL_LINK_LIST_H

/* Includes ------------------------------------------------------------------*/
#include "dji_platform.h"

#ifdef UTIL_LINK_LIST_C
    #define UTIL_LINK_LIST_EXT
#else
    #define UTIL_LINK_LIST_EXT          extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
#define UTIL_REPORT_ERROR( exp... )

#define UTIL_OSAL_MEMRY_ALLOC(size)     ( { T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler(); osalHandler->Malloc( size ); } )
#define UTIL_OSAL_MEMRY_FREE(ptr)       ( { T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler(); osalHandler->Free( ptr ); } )

#define UTIL_OSAL_SEM_FREE_MAY_RETURN(sem)          ( { T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler(); if( DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != osalHandler->SemaphoreDestroy( sem ) ) { return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER; } } )
#define UTIL_OSAL_SEM_INIT_MAY_RETURN(sem)          ( { T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler(); if( DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != osalHandler->SemaphoreCreate( 0, sem ) ) { return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER; } } )
#define UTIL_OSAL_SEM_POST_MAY_RETURN(sem)          ( { T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler(); if( DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != osalHandler->SemaphorePost( sem ) ) { return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER; } } )
#define UTIL_OSAL_SEM_WAIT_MAY_RETURN(sem)          ( { T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler(); if( DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != osalHandler->SemaphoreWait( sem ) ) { return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER; } } )
#define UTIL_OSAL_SEM_WAIT_FOR(sem,ms)              ( { T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler(); if( DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != osalHandler->SemaphoreTimedWait( sem, ms ) ) { return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER; } } )

#define UTIL_OSAL_MUTEX_FREE_MAY_RETURN(mutex)      ( { T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler(); if( DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != osalHandler->MutexDestroy( mutex ) ) { return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER; } } )
#define UTIL_OSAL_MUTEX_INIT_MAY_RETURN(mutex)      ( { T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler(); if( DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != osalHandler->MutexCreate( mutex ) ) { return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER; } } )
#define UTIL_OSAL_MUTEX_LOCK_MAY_RETURN(mutex)      ( { T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler(); if( DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != osalHandler->MutexLock( mutex ) ) { return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER; } } )
#define UTIL_OSAL_MUTEX_UNLOCK_MAY_RETURN(mutex)    ( { T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler(); if( DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != osalHandler->MutexUnlock( mutex ) ) { return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER; } } )

#define UTIL_LINKLIST_IS_EMPTY(l)       ( ((l).first == NULL)  ? true : false )

/* Exported types ------------------------------------------------------------*/
typedef struct tagT_UtilListNode {
    struct tagT_UtilListNode        *next;
    struct tagT_UtilListNode        *prev;
    void                            *data;
} T_UtilListNode;

typedef struct tagT_UtilLinkList {
    T_UtilListNode                  *first;
    T_UtilListNode                  *last;
    uint32_t                        count;
} T_UtilLinkList;

/* Exported functions --------------------------------------------------------*/
UTIL_LINK_LIST_EXT void DjiUserUtil_ListNodeDeleteDataOnly( T_UtilListNode *node );
UTIL_LINK_LIST_EXT void DjiUserUtil_ListNodeDeleteNodeSelf( T_UtilListNode *node );
UTIL_LINK_LIST_EXT void DjiUserUtil_InitListNode( T_UtilListNode *node, void *data );
UTIL_LINK_LIST_EXT T_UtilListNode *DjiUserUtil_NewListNode( void *data );

UTIL_LINK_LIST_EXT void DjiUserUtil_LinkListDestory( T_UtilLinkList *linkList );
UTIL_LINK_LIST_EXT void DjiUserUtil_LinkListAddNodeFirst( T_UtilLinkList *linkList, T_UtilListNode *node );
UTIL_LINK_LIST_EXT void DjiUserUtil_LinkListAddNodeLast( T_UtilLinkList *linkList, T_UtilListNode *node );
UTIL_LINK_LIST_EXT void DjiUserUtil_InitLinkList( T_UtilLinkList *linkList );
UTIL_LINK_LIST_EXT T_UtilLinkList *DjiUserUtil_NewLinkList( void );

UTIL_LINK_LIST_EXT void DjiUserUtil_LinkListRemoveNodeOnly( T_UtilLinkList *linkList, T_UtilListNode *node );

#ifdef __cplusplus
}
#endif

#endif // UTIL_LINK_LIST_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
