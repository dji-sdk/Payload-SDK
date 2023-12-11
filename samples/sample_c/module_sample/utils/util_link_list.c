/**
 ********************************************************************
 * @file    util_link_list.c
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

#define UTIL_LINK_LIST_C

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "util_link_list.h"

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Private values ------------------------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
void DjiUserUtil_ListNodeDeleteDataOnly( T_UtilListNode *node )
{
    if ( NULL == node) {
        UTIL_REPORT_ERROR( "null pointer" );
        return;
    }

    if ( NULL != node->data ) {
        UTIL_OSAL_MEMRY_FREE( node->data );
        node->data = NULL;
    }
    return;
}

void DjiUserUtil_ListNodeDeleteNodeSelf( T_UtilListNode *node )
{
    if ( NULL == node) {
        UTIL_REPORT_ERROR( "null pointer" );
        return;
    }

    UTIL_OSAL_MEMRY_FREE( node );
    return;
}

void DjiUserUtil_InitListNode( T_UtilListNode *node, void *data )
{
    if ( NULL == node ) {
        UTIL_REPORT_ERROR( "null pointer" );
        return;
    }

    node->prev = NULL;
    node->next = NULL;
    node->data = data;
    return;
}

T_UtilListNode *DjiUserUtil_NewListNode( void *data )
{
    T_UtilListNode *node = NULL;

    node = (T_UtilListNode *)UTIL_OSAL_MEMRY_ALLOC( sizeof(T_UtilListNode) );
    if ( NULL == node ) {
        UTIL_REPORT_ERROR( "null pointer" );
        return NULL;
    }

    DjiUserUtil_InitListNode( node, data );
    return node;
}

void DjiUserUtil_LinkListDestory( T_UtilLinkList *linkList )
{
    T_UtilListNode *node = NULL;
    T_UtilListNode *next = NULL;

    if ( NULL == linkList ) {
        UTIL_REPORT_ERROR( "null pointer" );
        return;
    }

    for ( node = linkList->first; NULL != node ; node = next ) {
        next = node->next;
        DjiUserUtil_ListNodeDeleteDataOnly( node );
        DjiUserUtil_ListNodeDeleteNodeSelf( node );
    }
}
void DjiUserUtil_LinkListAddNodeFirst( T_UtilLinkList *linkList, T_UtilListNode *node )
{
    if ( ( NULL == linkList ) || ( NULL == node ) ) {
        UTIL_REPORT_ERROR( "null pointer" );
        return;
    }

    if ( UTIL_LINKLIST_IS_EMPTY( *linkList ) ) {
        node->prev = NULL;
        node->next = NULL;
        linkList->first = node;
        linkList->last  = node;
    } else {
        node->prev = NULL;
        node->next = linkList->first;
        linkList->first = node;
        node->next->prev = node;
    }
    linkList->count ++;
    return;
}

void DjiUserUtil_LinkListAddNodeLast( T_UtilLinkList *linkList, T_UtilListNode *node )
{
    if ( ( NULL == linkList ) || ( NULL == node ) ) {
        UTIL_REPORT_ERROR( "null pointer" );
        return;
    }

    if ( UTIL_LINKLIST_IS_EMPTY( *linkList ) ) {
        node->prev = NULL;
        node->next = NULL;
        linkList->first = node;
        linkList->last  = node;
    } else {
        node->next = NULL;
        node->prev = linkList->last;
        linkList->last = node;
        node->prev->next = node;
    }
    linkList->count ++;
    return;
}

void DjiUserUtil_InitLinkList( T_UtilLinkList *linkList )
{
    if ( NULL == linkList ) {
        UTIL_REPORT_ERROR( "null pointer" );
        return;
    }

    linkList->first = NULL;
    linkList->last  = NULL;
    linkList->count = 0;
    return;
}

T_UtilLinkList *DjiUserUtil_NewLinkList( void )
{
    T_UtilLinkList *linkList = NULL;

    linkList = (T_UtilLinkList *)UTIL_OSAL_MEMRY_ALLOC( sizeof( T_UtilLinkList ) );
    if ( NULL == linkList ) {
        UTIL_REPORT_ERROR( "null pointer" );
        return NULL;
    }

    DjiUserUtil_InitLinkList( linkList );
    return linkList;
}

void DjiUserUtil_LinkListRemoveNodeOnly( T_UtilLinkList *linkList, T_UtilListNode *node )
{
    if ( ( NULL == linkList ) || ( NULL == node ) ) {
        UTIL_REPORT_ERROR( "null pointer" );
        return;
    }

    if ( node == linkList->first ) {
        linkList->first = linkList->first->next;
    }

    if ( node == linkList->last ) {
        linkList->last  = linkList->last->prev;
    }

    if ( NULL != node->next ) {
        node->next->prev = node->prev;
    }

    if ( NULL != node->prev ) {
        node->prev->next = node->next;
    }

    if ( 0 != linkList->count ) {
        linkList->count --;
    }

    DjiUserUtil_ListNodeDeleteNodeSelf( node );
    return;
}
/* Private functions definition-----------------------------------------------*/

#ifdef __cplusplus
}
#endif

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
