/*****************************************************************************
* Utility Library
*
* DESCRIPTION
*	Functions to support generic linked list operations.
*
*      Each linked list may contain zero or more nodes.  Nodes are
*      singly linked to each other.  For each linked list a head
*      pointer, tail pointer and node count are maintained.
*
*      A typical linked list might look like this:
*
*        List Header
*        .------------.
*        |     head_p |------------>NODE1
*        |     tail_p |----.         |
*        | node_count |=3   \        V
*        `------------'      \      NODE2
*                             \      |
*                              \     V
*                               `-->NODE3
*                                    |
*                                    V
*                                   NULL
*
*      Linked-list node structures are linked together like this:
*
*        .-----------.   .-----------.   .-----------.
*        |     next_p|-->|     next_p|-->|     next_p|-->NULL
*        | data area |   | data area |   | data area |
*        `-----------'   `-----------'   `-----------'
*
* EXAMPLE USAGE
*
*     typedef struct myNode_S *myNode_P;
*     typedef struct myNode_S {
*         myNode_P next_p;
*         int      a;
*         char     b;
*     } myNode_T;
*
*     {
*     utlLinkedList_t list;
*     myNode_T        node1;
*     myNode_T        node2;
*     myNode_T        node3;
*     myNode_P        node_p;
*
*     utlLinkedList_t list;
*
*         utlInitLinkedList(&list);
*
*         node1.next_p = NULL;
*         node1.a      = 42;
*         node1.b      = 'a';
*
*         node2.next_p = NULL;
*         node2.a      = 100;
*         node2.b      = 'b';
*
*         node3.next_p = NULL;
*         node3.a      = 88;
*         node3.b      = 'c';
*
*         utlPutTailNode(&list, myNode_T, &node1);
*         utlPutHeadNode(&list, myNode_T, &node2);
*
*         --- insert node3 immediately after node1 ---
*         utlPutNode(&list, myNode_T, &node1, &node3);
*
*         --- fetch the node immediately following node1 ---
*         node_p = utlGetNode(&list, myNode_T, NULL, &node1);
*         <use "node_p" here>
*
*         node_p = utlGetHeadNode(&list, myNode_T);
*         <use "node_p" here>
*
*         node_p = utlGetTailNode(&list, myNode_T);
*         <use "node_p" here>
*
*         if (utlIsListEmpty(&list))
*             (void) printf("Linked list is empty.\n");
*         else
*             (void) printf("Linked list is NOT empty.\n");
*     }
*****************************************************************************/

#include <stdio.h>

#include <utlTypes.h>
#include <utlError.h>
#include <utlLinkedList.h>


/*---------------------------------------------------------------------------*
* FUNCTION
*	utlInitLinkedList(list_p)
* INPUT
*	list_p == pointer to an uninitialized utlLinkedList_T
* OUTPUT
*	*list_p == an initialized linked list header
* RETURNS
*	nothing
* DESCRIPTION
*      Initailizes the linked list header specified by `list_p'.
*---------------------------------------------------------------------------*/
void utlInitLinkedList(const utlLinkedList_P list_p)
{
	list_p->head_p     = NULL;
	list_p->tail_p     = NULL;
	list_p->node_count = 0;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDoQueryPrevNode(list_p, node_p)
* INPUT
*	list_p == pointer to a filled utlLinkedList_T
*      node_p == pointer to a utlLinkedListNode_T currently in `list_p'
* OUTPUT
*	none
* RETURNS
*	A pointer to the node preceding `node_p' in `list_p'
* DESCRIPTION
*      Fetches the node that precedes `node_p' in `list_p'.
*---------------------------------------------------------------------------*/
utlLinkedListNode_P utlDoQueryPrevNode(const utlLinkedList_P list_p,
				       const utlLinkedListNode_P node_p)
{
	utlLinkedListNode_P prev_p;
	utlLinkedListNode_P this_p;

	utlAssert(list_p != NULL);
	utlAssert(node_p != NULL);

	utlAssert(!utlIsListEmpty(*list_p));

	/*--- `list_p' cannot be empty since the node count was non-zero ---*/
	utlAssert(list_p->head_p != NULL);
	utlAssert(list_p->tail_p != NULL);

	prev_p = NULL;
	for (this_p = list_p->head_p; this_p != NULL; )
	{
		if (this_p == node_p)
			break;

		prev_p = this_p;
		this_p = this_p->next_p;
	}

	return prev_p;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDoGetHeadNode(list_p)
* INPUT
*	list_p == pointer to a filled utlLinkedList_T
* OUTPUT
*	none
* RETURNS
*	A pointer to the node that was unlinked.  If `list_p' is empty, NULL
*      is returned.
* DESCRIPTION
*      Unlinks the first node from the linked list specified by `list_p'.
*
*	Note: This routine is carefully crafted so that it can interrupt
*      utlGetHeadNode() (assuming it is executing in a signal handler) at
*      any point, and `list_p' will not be corrupted.
*---------------------------------------------------------------------------*/
utlLinkedListNode_P utlDoGetHeadNode(const utlLinkedList_P list_p)
{
	utlLinkedListNode_P node_p;

	utlAssert(list_p != NULL);

	if (utlIsListEmpty(*list_p))
		return NULL;

	/*--- `list_p' cannot be empty since the node count was non-zero ---*/
	utlAssert(list_p->head_p != NULL);
	utlAssert(list_p->tail_p != NULL);

	/*--- grab first node... ---*/
	node_p         = list_p->head_p;
	list_p->head_p = list_p->head_p->next_p;

	/*--- `list_p' now empty? then fix tail pointer ---*/
	if (list_p->head_p == NULL)
		list_p->tail_p = NULL;

	node_p->next_p = NULL;

	list_p->node_count--;

	return node_p;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDoGetNode(list_p, prev_p, node_p)
* INPUT
*	list_p == pointer to a filled utlLinkedList_T
*      prev_p == pointer to the node preceding `node_p', or NULL
*      node_p == pointer to a utlLinkedListNode_T currently in `list_p'
* OUTPUT
*	none
* RETURNS
*	A pointer to the node that was unlinked.  If the list was empty, NULL
*      is returned.
* DESCRIPTION
*      Removes the node pointed to by `node_p' from the linked list specified
*      by `list_p'.  `prev_p' must point to the node preceeding `node_p', or
*      be NULL.  For maximum run-time performance, "prev_p" should be non-NULL,
*      unless `node_p' points to the first node in the list.
*---------------------------------------------------------------------------*/
utlLinkedListNode_P utlDoGetNode(const utlLinkedList_P list_p,
				 const utlLinkedListNode_P prev_p,
				 const utlLinkedListNode_P node_p)
{
	utlLinkedListNode_P nprev_p;

	utlAssert(list_p != NULL);
	utlAssert(node_p != NULL);

	/*--- nothing to get? ---*/
	if (utlIsListEmpty(*list_p))
		return NULL;

	/*--- is `node_p' the head node in `list_p'? ---*/
	if (node_p == list_p->head_p)
		return utlDoGetHeadNode(list_p);

	/*--- `list_p' cannot be empty since the node count was non-zero ---*/
	utlAssert(list_p->head_p != NULL);
	utlAssert(list_p->tail_p != NULL);

	/*--- if `prev_p' node was not specified, find it ---*/
	if (prev_p == NULL) nprev_p = utlDoQueryPrevNode(list_p, node_p);
	else nprev_p = prev_p;

	/*--- since `node_p' was not the head node in `list_p', `nprev_p' must be
	      known, and must be the node preceeding `node_p' */
	utlAssert(nprev_p         != NULL);
	utlAssert(nprev_p->next_p == node_p);

	/*--- unlink `node_p' from `list_p' ---*/
	nprev_p->next_p = node_p->next_p;
	node_p->next_p  = NULL;

	/*--- if `node_p' was the last node in `list_p', fix up tail pointer ---*/
	if (node_p == list_p->tail_p)
		list_p->tail_p = nprev_p;

	list_p->node_count--;

	return node_p;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDoGetTailNode(list_p)
* INPUT
*	list_p == pointer to a filled utlLinkedList_T
* OUTPUT
*	none
* RETURNS
*	A pointer to the node that was unlinked.  If `list_p' is empty,
*      NULL is returned.
* DESCRIPTION
*      Unlinks the last node from the linked list specified by `list_p'.
*---------------------------------------------------------------------------*/
utlLinkedListNode_P utlDoGetTailNode(const utlLinkedList_P list_p)
{
	utlLinkedListNode_P prev_p;
	utlLinkedListNode_P this_p;
	utlLinkedListNode_P node_p;

	utlAssert(list_p != NULL);

	if (utlIsListEmpty(*list_p))
		return NULL;

	/*--- `list_p' cannot be empty since the node count was non-zero ---*/
	utlAssert(list_p->head_p != NULL);
	utlAssert(list_p->tail_p != NULL);

	/*--- find node preceding current tail node ---*/
	prev_p = NULL;
	for (this_p = list_p->head_p; this_p != NULL; )
	{
		if (this_p == list_p->tail_p)
			break;

		prev_p = this_p;
		this_p = this_p->next_p;
	}

	/*--- grab last node... ---*/
	node_p         = list_p->tail_p;
	list_p->tail_p = prev_p;

	/*--- fix up new `last node' ---*/
	if (prev_p != NULL)
		prev_p->next_p = NULL;

	/*--- `list_p' now empty? then fix tail pointer ---*/
	if (list_p->tail_p == NULL)
		list_p->head_p = NULL;

	node_p->next_p = NULL;

	list_p->node_count--;

	return node_p;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDoPutHeadNode(list_p, node_p)
* INPUT
*	list_p == pointer to a filled utlLinkedList_T
*      node_p == pointer to a filled utlLinkedListNode_T
* OUTPUT
*	none
* RETURNS
*	nothing
* DESCRIPTION
*      Attachs the node pointed to by `node_p' to the head of the linked list
*      specified by `list_p'.
*---------------------------------------------------------------------------*/
void utlDoPutHeadNode(const utlLinkedList_P list_p,
		      const utlLinkedListNode_P node_p)
{
	utlAssert(list_p != NULL);
	utlAssert(node_p != NULL);

	node_p->next_p = list_p->head_p;

	/*--- `list_p' empty? then fix tail pointer ---*/
	if (utlIsListEmpty(*list_p))
		list_p->tail_p = node_p;

	/*--- attach `node_p' to `list_p' ---*/
	list_p->head_p = node_p;

	list_p->node_count++;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDoPutNode(list_p, here_p, node_p)
* INPUT
*	list_p == pointer to a filled utlLinkedList_T
*      here_p == pointer to a node in the above list
*      node_p == pointer to a filled utlLinkedListNode_T
* OUTPUT
*	none
* RETURNS
*	nothing
* DESCRIPTION
*      Inserts the node pointed to by `node_p' after the node pointed to by
*      `ptr_p' in the list specified by `list_p'.  If `ptr_p' equals NULL
*      then the node is linked to the head of `list_p'.
*---------------------------------------------------------------------------*/
void utlDoPutNode(const utlLinkedList_P list_p,
		  const utlLinkedListNode_P here_p,
		  const utlLinkedListNode_P node_p)
{
	utlAssert(list_p != NULL);
	utlAssert(node_p != NULL);

	/*--- link `node_p' to head of list? ---*/
	if (here_p == NULL)
	{
		utlDoPutHeadNode(list_p, node_p);
		return;
	}

	/*--- `list_p' cannot be empty since `here_p' is non-NULL ---*/
	utlAssert(list_p->head_p != NULL);
	utlAssert(list_p->tail_p != NULL);
	utlAssert(!utlIsListEmpty(*list_p));

	/*--- insert `node_p' after `here_p'... ---*/
	node_p->next_p = here_p->next_p;
	here_p->next_p = node_p;

	/*--- is `node_p' the new tail node? ---*/
	if (here_p == list_p->tail_p)
		list_p->tail_p = node_p;

	list_p->node_count++;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlDoPutTailNode(list_p, node_p)
* INPUT
*	list_p == pointer to a filled utlLinkedList_T
*      node_p == pointer to a filled utlLinkedListNode_T
* OUTPUT
*	none
* RETURNS
*	nothing
* DESCRIPTION
*      Attachs the node pointed to by `node_p' to the tail of the linked list
*      specified by `list_p'.
*
*	Note: This routine is carefully crafted so that it can be interrupted
*      at any point and `list_p' processed by utlGetHeadNode() (which would
*      be executed via a signal handler) without corruption.
*---------------------------------------------------------------------------*/
void utlDoPutTailNode(const utlLinkedList_P list_p,
		      const utlLinkedListNode_P node_p)
{
	utlAssert(list_p != NULL);
	utlAssert(node_p != NULL);

	node_p->next_p = NULL;

	/*--- `list_p' empty? then fix head pointer ---*/
	if (utlIsListEmpty(*list_p))
	{
		list_p->tail_p = node_p;
		list_p->head_p = node_p;
	}
	else
	{
		list_p->tail_p->next_p = node_p;
		list_p->tail_p         = node_p;
	}

	list_p->node_count++;
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlPutHeadList(list1_p, list2_p)
* INPUT
*	list1_p == pointer to a utlLinkedList_T
*	list2_p == pointer to a utlLinkedList_T
* OUTPUT
*	*list1_p == a full linked list header
*	*list2_p == an empy linked list header
* RETURNS
*	nothing
* DESCRIPTION
*      Merges two lists by moving all the nodes in `list2_p' onto the head
*      of `list1_p'.
*---------------------------------------------------------------------------*/
void utlPutHeadList(const utlLinkedList_P list1_p,
		    const utlLinkedList_P list2_p)
{
	utlAssert(list1_p != NULL);
	utlAssert(list2_p != NULL);

	/*--- is `list2_p' empty? ---*/
	if (utlIsListEmpty(*list2_p))
		return;

	list2_p->tail_p->next_p = list1_p->head_p;

	/*--- `list1_p' empty? then fix tail pointer ---*/
	if (utlIsListEmpty(*list1_p))
		list1_p->tail_p = list2_p->tail_p;

	/*--- attach `node_p' to `list_p' ---*/
	list1_p->head_p = list2_p->head_p;

	list1_p->node_count += list2_p->node_count;

	utlInitLinkedList(list2_p);
}

/*---------------------------------------------------------------------------*
* FUNCTION
*	utlPutTailList(list1_p, list2_p)
* INPUT
*	list1_p == pointer to a utlLinkedList_T
*	list2_p == pointer to a utlLinkedList_T
* OUTPUT
*	*list1_p == a full linked list header
*	*list2_p == an empy linked list header
* RETURNS
*	nothing
* DESCRIPTION
*      Merges two lists by moving all the nodes in `list2_p' onto the tail
*      of `list1_p'.
*---------------------------------------------------------------------------*/
void utlPutTailList(const utlLinkedList_P list1_p,
		    const utlLinkedList_P list2_p)
{
	utlAssert(list1_p != NULL);
	utlAssert(list2_p != NULL);

	/*--- is `list2_p' empty? ---*/
	if (utlIsListEmpty(*list2_p))
		return;

	/*--- is `list1_p' empty? then fix head pointer ---*/
	if (utlIsListEmpty(*list1_p)) list1_p->head_p         = list2_p->head_p;
	else list1_p->tail_p->next_p = list2_p->head_p;

	/*--- attach `list2_p' to tail of `list1_p' ---*/
	list1_p->tail_p = list2_p->tail_p;

	list1_p->node_count += list2_p->node_count;

	utlInitLinkedList(list2_p);
}

#ifdef utlTEST
/*---------------------------------------------------------------------------*
* FUNCTION
*	linkedListTest()
* INPUT
*	none
* OUTPUT
*	none
* RETURNS
*	"true" for pass, "false" for failure
*---------------------------------------------------------------------------*/
bool linkedListTest(void)
{
	utlLinkedList_T list;
	utlLinkedListNode_T node[10];
	utlLinkedListNode_P this_p;
	utlLinkedListNode_P prev_p;

	/*--- Check `utlGetHeadNode()' ------------------------------------------*/
	node[0].next_p = &(node[1]);
	node[1].next_p = &(node[2]);
	node[2].next_p = NULL;

	list.head_p     = &(node[0]);
	list.tail_p     = &(node[2]);
	list.node_count = 3;

	if (utlGetHeadNode(&list, utlLinkedListNode_T) != &(node[0]))
	{
		(void)fprintf(stderr, "linkedListTest: utlGetHeadNode(1) failed\n");
		return false;
	}
	if (utlGetHeadNode(&list, utlLinkedListNode_T) != &(node[1]))
	{
		(void)fprintf(stderr, "linkedListTest: utlGetHeadNode(2) failed\n");
		return false;
	}
	if (utlGetHeadNode(&list, utlLinkedListNode_T) != &(node[2]))
	{
		(void)fprintf(stderr, "linkedListTest: utlGetHeadNode(3) failed\n");
		return false;
	}
	if (utlGetHeadNode(&list, utlLinkedListNode_T) != NULL)
	{
		(void)fprintf(stderr, "linkedListTest: utlGetHeadNode(4) failed\n");
		return false;
	}
	if ((list.head_p != NULL) ||
	    (list.tail_p != NULL) ||
	    (list.node_count != 0))
	{
		(void)fprintf(stderr, "linkedListTest: utlGetHeadNode(5) failed\n");
		return false;
	}


	/*--- Check `utlGetNode()' ----------------------------------------------*/
	node[0].next_p = &(node[1]);
	node[1].next_p = &(node[2]);
	node[2].next_p = &(node[3]);
	node[3].next_p = &(node[4]);
	node[4].next_p = NULL;

	list.head_p     = &(node[0]);
	list.tail_p     = &(node[4]);
	list.node_count = 5;

	prev_p = NULL;
	this_p = &(node[0]);
	if (utlGetNode(&list, utlLinkedListNode_T, prev_p, this_p) != &(node[0]))
	{
		(void)fprintf(stderr, "linkedListTest: utlGetNode(1) failed\n");
		return false;
	}
	prev_p = &(node[1]);
	this_p = &(node[2]);
	if (utlGetNode(&list, utlLinkedListNode_T, prev_p, this_p) != &(node[2]))
	{
		(void)fprintf(stderr, "linkedListTest: utlGetNode(2) failed\n");
		return false;
	}
	prev_p = &(node[3]);
	this_p = &(node[4]);
	if (utlGetNode(&list, utlLinkedListNode_T, prev_p, this_p) != &(node[4]))
	{
		(void)fprintf(stderr, "linkedListTest: utlGetNode(3) failed\n");
		return false;
	}
	if ((list.head_p != &(node[1])) ||
	    (list.tail_p != &(node[3])) ||
	    (list.node_count != 2))
	{
		(void)fprintf(stderr, "linkedListTest: utlGetNode(4) failed\n");
		return false;
	}

	node[0].next_p = &(node[1]);
	node[1].next_p = &(node[2]);
	node[2].next_p = &(node[3]);
	node[3].next_p = &(node[4]);
	node[4].next_p = NULL;

	list.head_p     = &(node[0]);
	list.tail_p     = &(node[4]);
	list.node_count = 5;

	prev_p = NULL;
	this_p = &(node[0]);
	if (utlGetNode(&list, utlLinkedListNode_T, prev_p, this_p) != &(node[0]))
	{
		(void)fprintf(stderr, "linkedListTest: utlGetNode(5) failed\n");
		return false;
	}
	prev_p = NULL;
	this_p = &(node[2]);
	if (utlGetNode(&list, utlLinkedListNode_T, prev_p, this_p) != &(node[2]))
	{
		(void)fprintf(stderr, "linkedListTest: utlGetNode(6) failed\n");
		return false;
	}
	prev_p = NULL;
	this_p = &(node[4]);
	if (utlGetNode(&list, utlLinkedListNode_T, prev_p, this_p) != &(node[4]))
	{
		(void)fprintf(stderr, "linkedListTest: utlGetNode(7) failed\n");
		return false;
	}
	if ((list.head_p != &(node[1])) ||
	    (list.tail_p != &(node[3])) ||
	    (list.node_count != 2))
	{
		(void)fprintf(stderr, "linkedListTest: utlGetNode(8) failed\n");
		return false;
	}


	/*--- Check `utlInitLinkedList()' ---------------------------------------*/
	utlInitLinkedList(&list);
	if ((list.head_p != NULL) ||
	    (list.tail_p != NULL) ||
	    (list.node_count != 0))
	{
		(void)fprintf(stderr, "linkedListTest: utlInitLinkedList(1) failed\n");
		return false;
	}


	/*--- Check `utlGetTailNode()' ------------------------------------------*/
	node[0].next_p = &(node[1]);
	node[1].next_p = &(node[2]);
	node[2].next_p = NULL;

	list.head_p     = &(node[0]);
	list.tail_p     = &(node[2]);
	list.node_count = 3;

	if (utlGetTailNode(&list, utlLinkedListNode_T) != &(node[2]))
	{
		(void)fprintf(stderr, "linkedListTest: utlGetTailNode(1) failed\n");
		return false;
	}
	if (utlGetTailNode(&list, utlLinkedListNode_T) != &(node[1]))
	{
		(void)fprintf(stderr, "linkedListTest: utlGetTailNode(2) failed\n");
		return false;
	}
	if (utlGetTailNode(&list, utlLinkedListNode_T) != &(node[0]))
	{
		(void)fprintf(stderr, "linkedListTest: utlGetTailNode(3) failed\n");
		return false;
	}
	if (utlGetTailNode(&list, utlLinkedListNode_T) != NULL)
	{
		(void)fprintf(stderr, "linkedListTest: utlGetTailNode(4) failed\n");
		return false;
	}
	if ((list.head_p != NULL) ||
	    (list.tail_p != NULL) ||
	    (list.node_count != 0))
	{
		(void)fprintf(stderr, "linkedListTest: utlGetTailNode(5) failed\n");
		return false;
	}


	/*--- Check `utlPutHeadNode()' ------------------------------------------*/
	utlInitLinkedList(&list);

	utlPutHeadNode(&list, utlLinkedListNode_T, &(node[4]));
	utlPutHeadNode(&list, utlLinkedListNode_T, &(node[3]));
	utlPutHeadNode(&list, utlLinkedListNode_T, &(node[2]));
	utlPutHeadNode(&list, utlLinkedListNode_T, &(node[1]));
	utlPutHeadNode(&list, utlLinkedListNode_T, &(node[0]));
	if ((list.head_p != &(node[0])) ||
	    (list.tail_p != &(node[4])) ||
	    (list.node_count != 5))
	{
		(void)fprintf(stderr, "linkedListTest: utlPutHeadNode(1) failed\n");
		return false;
	}


	/*--- Check `utlPutNode()' ----------------------------------------------*/
	utlInitLinkedList(&list);

	utlPutNode(&list, utlLinkedListNode_T, NULL,       &(node[2]));
	utlPutNode(&list, utlLinkedListNode_T, NULL,       &(node[1]));
	utlPutNode(&list, utlLinkedListNode_T, &(node[2]), &(node[3]));
	utlPutNode(&list, utlLinkedListNode_T, NULL,       &(node[0]));
	utlPutNode(&list, utlLinkedListNode_T, &(node[3]), &(node[4]));
	if ((list.head_p != &(node[0])) ||
	    (list.tail_p != &(node[4])) ||
	    (list.node_count != 5))
	{
		(void)fprintf(stderr, "linkedListTest: utlPutNode(1) failed\n");
		return false;
	}


	/*--- Check `utlPutTailNode()' ------------------------------------------*/
	utlInitLinkedList(&list);

	utlPutTailNode(&list, utlLinkedListNode_T, &(node[0]));
	utlPutTailNode(&list, utlLinkedListNode_T, &(node[1]));
	utlPutTailNode(&list, utlLinkedListNode_T, &(node[2]));
	utlPutTailNode(&list, utlLinkedListNode_T, &(node[3]));
	utlPutTailNode(&list, utlLinkedListNode_T, &(node[4]));
	if ((list.head_p != &(node[0])) ||
	    (list.tail_p != &(node[4])) ||
	    (list.node_count != 5))
	{
		(void)fprintf(stderr, "linkedListTest: utlPutTailNode(1) failed\n");
		return false;
	}


	/*--- Check `utlQueryPrevNode()' ----------------------------------------*/
	node[0].next_p = &(node[1]);
	node[1].next_p = &(node[2]);
	node[2].next_p = NULL;

	list.head_p     = &(node[0]);
	list.tail_p     = &(node[2]);
	list.node_count = 3;

	if (utlQueryPrevNode(&list, utlLinkedListNode_T, &(node[0])) != NULL)
	{
		(void)fprintf(stderr, "linkedListTest: utlQueryPrevNode(1) failed\n");
		return false;
	}
	if (utlQueryPrevNode(&list, utlLinkedListNode_T, &(node[1])) != &(node[0]))
	{
		(void)fprintf(stderr, "linkedListTest: utlQueryPrevNode(2) failed\n");
		return false;
	}
	if (utlQueryPrevNode(&list, utlLinkedListNode_T, &(node[2])) != &(node[1]))
	{
		(void)fprintf(stderr, "linkedListTest: utlQueryPrevNode(3) failed\n");
		return false;
	}

	return true;
}
#endif /* utlTEST */

