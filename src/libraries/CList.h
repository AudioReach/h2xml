/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#ifndef _YDFPASM_CLIST_H
#define _YDFPASM_CLIST_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _DEBUG
#define LIST_DEBUG
#endif

#define DEFAULT_LIST_SIZE	3

	enum dir {
		lPREVIOUS,
		lNEXT,
		lLEFT,
		lRIGHT,
		lUP,
		lDOWN
	};

//!	Class for CList
/*!
*
*	*/
template<class T> class CList {

	public:
		CList() ;
		CList(long n);
		~CList() {};
	T** lpt;		// list pointer
	T*** lptpt;		// pointer to Adress of previous pointer
	
	// functions:
	T* get(int connection);

	// for debugging, these pointers will point to elements of list pointer
	#ifdef LIST_DEBUG
	T* d_previous;
	T* d_next;
	T* d_left;
	T* d_right;
	T* d_up;
	T* d_down;
	#endif
};

template<class T1> class CListElement:public CList<CListElement<T1> > {
	public:
	CListElement();
	~CListElement();
	T1 m_data;
};

template<class T2> class CListItem {
	public:
	CListItem();
	~CListItem();
	CListElement<T2> *m_firstElement;
	CListElement<T2> *m_lastElement;
	CListElement<T2> *m_actualElement;
	long m_knElements;
	CListElement<T2> *push(T2 data);			// add to end
	T2 pop();								// get from end and erase
	void clearList();
};

///////////////////////////////////////////////////////////
// CListelements
///////////////////////////////////////////////////////////

/*!
*	Standard Constructor
*/
template<class T> CList<T>::CList()
{
	// debug
	memset(this,0,sizeof(CList));
	lpt=new(T*[2*DEFAULT_LIST_SIZE]);
	memset(lpt,0,sizeof(T*)*2*DEFAULT_LIST_SIZE);
	lptpt=new(T**[2*DEFAULT_LIST_SIZE]);
	memset(lptpt,0,sizeof(T*)*2*DEFAULT_LIST_SIZE);

	// for debugging
	//previous=lpt[lPREVIOUS];
	/*next=lpt[lNEXT];
	left=lpt[lLEFT];
	right=lpt[lRIGHT];*/

}



/*!
*	Constructor for variable number of connection points
*/
template<class T> CList<T>::CList(long n)
{
	memset(this,0,sizeof(CList));
	lpt=new(T*[2*n]);
	memset(lpt,0,sizeof(T*)*2*n);
	lptpt=new(T**[2*n]);
	memset(lptpt,0,sizeof(T*)*2*n);
}

/*!
*	Generate new List element
*/
template<class T> T* newList()
{
	T* tmp=new(T);
	return tmp;
}


/*!
*	Insert existing element in List
*/
template<class T> T* insertInList(int connection, T* &addto, T* new_element)
{
	if (addto!=NULL) {														// not first element in List
		new_element->lpt[connection]=addto->lpt[connection];				// new element points to element that was previously connnected to addto
		if (addto->lpt[connection]!=NULL) {									// next element points back to new element
			addto->lpt[connection]->lpt[connection^1]=new_element;
		} else {															// if pointer to start pointer
			if (addto->lptpt[connection]!=NULL) {
				*(addto->lptpt[connection])=new_element;
			}
		}
		addto->lpt[connection]=new_element;									// addto pointer to new element
		new_element->lpt[connection ^1]=addto;								// new element pointer backk to addto
	} else {																// first element in list if addto is NULL: 
		new_element->lpt[connection]=NULL;									// new element points to element that was previously connnected to addto
		new_element->lptpt[connection ^1]=&addto;							// new element pointer backk to addto
		addto=new_element;													// addto pointer to new element
	} 
#ifdef LIST_DEBUG
	CopyListPointers(new_element);
	CopyListPointers(new_element->lpt[connection]);
	CopyListPointers(new_element->lpt[connection^1]);
#endif

	return new_element;
}

/*!
*	generate new element and insert into list
*/
template<class T> T* addToList(int connection, T* &addto)
{
	
	//int size=sizeof(T);	// for debug
	T* new_element=newList<T>();
	return insertInList(connection, addto, new_element);
}

/*
*	Remove element from List
*/
template<class T> T* removeFromList(int connection, T* &addto)
{
	if (addto->lptpt[connection]!=NULL) {
		T* tmp=*(addto->lptpt[connection]);
		T* tmp_next=*(tmp->lptpt[connection]);
		*(addto->lptpt[connection])=tmp_next;
		delete(tmp_next);
	}
	return addto;
}

/*
*	Remove element from List
*/
template<class T> T* deleteFromList(int connection, T* &remove)
{
	T* tmp=remove;
	if (remove!=NULL) {
		if (remove->lpt[connection]) {
			remove->lpt[connection]->lpt[connection^1]=remove->lpt[connection^1];
			remove->lpt[connection]->lptpt[connection^1]=remove->lptpt[connection^1];
		}
		T* previous=remove->lpt[connection^1];
		if (previous) {
			previous->lpt[connection]=remove->lpt[connection]; // next pointer
		} else {
            if (remove->lptpt[connection ^ 1]) {
                *(remove->lptpt[connection ^ 1]) = remove->lpt[connection];
            }
		}
		delete tmp;
	}
	return tmp;
}

template<class T> void deleteList(int connection, T* &start)
{
    if (start) {
        if (start->lpt[connection]) {
            deleteList(connection, start->lpt[connection]);
        }
        start = NULL;
    }
}

/*!
*	insert exisitng element to end of list
*/
template<class T> T* insertToEnd(int connection, T* &start, T* new_element)
{
	if (start!=NULL) {
		T* list=findLast(connection,start);
		return insertInList(connection, list, new_element);		// don't change start pointer
	} else {
		return insertInList(connection, start, new_element);	// also update start pointer
	}
}

/*!
*	generate new element and insert to end of list
*/
template<class T> T* addToEnd(int connection, T &start)
{
	T* res = addToEnd(connection, start.lpt[connection]);
#ifdef LIST_DEBUG
	CopyListPointers(&start);
#endif
	return res;
}

/*!
*	generate new element and insert to end of list
*/
template<class T> T* addToEnd(int connection, T* &start)
{
	if (start!=NULL) {
		T* list=findLast(connection,start);
		return addToList(connection, list);			// don't change start pointer
	} else {
		return addToList(connection, start);		// also update start pointer
	}
}

/*!
*	generate new element and insert to end of list
*/
template<class T> T* addToEnd(int connection, T* start, int connectionElement)
{
	T* res= addToEnd(connectionElement, start->lpt[connection]);
#ifdef LIST_DEBUG
	CopyListPointers(start);
#endif
	return res;
}


/*!
*	remove from End of list
*/
template<class T> T* deleteFromEnd(int connection, T* &start)
{
	T* retval=NULL;
	if (start!=NULL) {
		T* list=findLast(connection,start);
		if (list) {
			retval=list->lpt[connection^1];
			if (retval) {
				retval->lpt[connection]=NULL;
			} else if (list->lptpt[connection^1]){
				*(list->lptpt[connection^1])=NULL;
			}
			delete list;
            if (list == start) {
                start = NULL;
            }
		}
	} 
	#ifdef LIST_DEBUG
	CopyListPointers(retval);
	#endif
	return retval;
}


/*!
*	Find last Element in List, in direction according to connection
*/
template<class T> T* findLast(int connection, T* list)
{
	if (list!=NULL) {
		while(list->lpt[connection]!=NULL) {
			list=list->lpt[connection];
		}
	}
	return list;
}

/*!
*	get connecting element
*/
template<class T> inline T* CList<T>::get(int connection)
{
	return lpt[connection];
}


/*!
*	Constructor for variable number of connection points
*/
template<class T> void CopyListPointers(T* list)
{
	if (list!=NULL) {
		list->d_previous=list->lpt[lPREVIOUS];
		list->d_next=list->lpt[lNEXT];
		list->d_left=list->lpt[lLEFT];
		list->d_right=list->lpt[lRIGHT];
		list->d_up=list->lpt[lUP];
		list->d_down=list->lpt[lDOWN];
	}
}

// CListElement Methods
template<class T1> CListElement<T1>::CListElement()
{
};

template<class T1> CListElement<T1>::~CListElement()
{
};

// CListItem Methods
template<class T2> CListItem<T2>::CListItem()
{
	m_firstElement=NULL;
	m_lastElement=NULL;
	m_actualElement=NULL;
	m_knElements=0;
}

template<class T2> CListItem<T2>::~CListItem()
{
	
}

template<class T2> CListElement<T2>* CListItem<T2>::push(T2 data)
{
	CListElement<T2>* element=addToEnd(lNEXT, m_firstElement);
	element->m_data=data;
	m_lastElement=element;
	m_knElements++;
	return element;
}

template<class T2> T2 CListItem<T2>::pop()
{
	if (m_lastElement) {
		T2 lastElementData=m_lastElement->m_data;
		CListElement<T2>* previousElement=m_lastElement->get(lPREVIOUS);
		m_lastElement=deleteFromList(lNEXT,(CListElement<T2>*) m_lastElement);
		m_lastElement=previousElement;
		m_knElements--;
		return lastElementData;
	}
	else {
#ifdef _DEBUG
		printf("Internal Error: POP stack empty\n");
#endif
		return 0;
	}
}


/*
template<class T> class CListItem {
	public:
	CListItem();
	~CListItem();
	CList<T> *firstElement;
	CList<T> *lastElement;
	CList<T> *actualElement;
	CList<T> *push(T);			// add to end
	T pop();					// get from end and erase
	void clearList();
};*/
#endif

