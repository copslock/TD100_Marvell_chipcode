/*****************************************************************************
* Utility Library
*
* Singly linked-list utilities header file
*
*****************************************************************************/

#ifndef _UTL_LINKED_LIST_INCLUDED
#define _UTL_LINKED_LIST_INCLUDED


/*--- Data Types ------------------------------------------------------------*/

typedef struct utlLinkedListNode_S *utlLinkedListNode_P;
typedef struct utlLinkedListNode_S {
	utlLinkedListNode_P next_p;
	unsigned char data[0];    /* variable-sized array */
}             utlLinkedListNode_T;
typedef const utlLinkedListNode_T *utlLinkedListNode_P2c;

#define utlNULL_LINKED_LIST_NODE { NULL, { 0 } }


typedef unsigned int utlNodeCount_T;

typedef struct utlLinkedList_S {
	utlLinkedListNode_P head_p;
	utlLinkedListNode_P tail_p;
	utlNodeCount_T node_count;
}             utlLinkedList_T, *utlLinkedList_P;
typedef const utlLinkedList_T  *utlLinkedList_P2c;

#define utlEMPTY_LINKED_LIST { NULL, NULL, 0 }


/*--- Macros ----------------------------------------------------------------*/

#define utlIsListEmpty(list) ((list).head_p == (utlLinkedListNode_P)NULL)

#define utlQueryPrevNode(list_p, node_type, node_p)         (node_type *)utlDoQueryPrevNode(list_p, (utlLinkedListNode_P)node_p)
#define   utlGetHeadNode(list_p, node_type)                 (node_type *)utlDoGetHeadNode(list_p)
#define       utlGetNode(list_p, node_type, prev_p, node_p) (node_type *)utlDoGetNode(list_p, (utlLinkedListNode_P)prev_p, (utlLinkedListNode_P)node_p)
#define   utlGetTailNode(list_p, node_type)                 (node_type *)utlDoGetTailNode(list_p)
#define   utlPutHeadNode(list_p, node_type, node_p)                       utlDoPutHeadNode(list_p, (utlLinkedListNode_P)node_p)
#define       utlPutNode(list_p, node_type, here_p, node_p)               utlDoPutNode(list_p, (utlLinkedListNode_P)here_p, (utlLinkedListNode_P)node_p)
#define   utlPutTailNode(list_p, node_type, node_p)                       utlDoPutTailNode(list_p, (utlLinkedListNode_P)node_p)

/*--- Prototypes ------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern void                utlInitLinkedList(const utlLinkedList_P list_p);
extern utlLinkedListNode_P utlDoQueryPrevNode(const utlLinkedList_P list_p,
					      const utlLinkedListNode_P node_p);
extern utlLinkedListNode_P utlDoGetHeadNode(const utlLinkedList_P list_p);
extern utlLinkedListNode_P utlDoGetNode(const utlLinkedList_P list_p,
					const utlLinkedListNode_P prev_p,
					const utlLinkedListNode_P node_p);
extern utlLinkedListNode_P utlDoGetTailNode(const utlLinkedList_P list_p);
extern void                utlDoPutHeadNode(const utlLinkedList_P list_p,
					    const utlLinkedListNode_P node_p);
extern void                utlDoPutNode(const utlLinkedList_P list_p,
					const utlLinkedListNode_P here_p,
					const utlLinkedListNode_P node_p);
extern void                utlDoPutTailNode(const utlLinkedList_P list_p,
					    const utlLinkedListNode_P node_p);
extern void                utlPutHeadList(const utlLinkedList_P list1_p,
					  const utlLinkedList_P list2_p);
extern void                utlPutTailList(const utlLinkedList_P list1_p,
					  const utlLinkedList_P list2_p);

#ifdef __cplusplus
}
#endif

#endif /* _UTL_LINKED_LIST_INCLUDED */

