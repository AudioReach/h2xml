/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/


#include "CIdentifier.h"
#include "CStatement.h"
#include "CProducerList.h"

#ifndef CDEPENDENCYLIST__H__INCLUDED_
#define CDEPENDENCYLIST__H__INCLUDED_

//!	Class for generating the dependency list

class CDependencyList {
private:
	CStatement      *statement;
	CDependencyList *prevDList;
	CDependencyList *nextDList;
	CIdentifier     *identifier;
	CProducerList   *producerList;
	CDependencyList *producer;
	CDependencyList *consumer;
	CDependencyList *left;
	CDependencyList *right;

	CDependencyList(); // prevent no-arg constructor from being called
	CDependencyList(CStatement *_statement, CIdentifier *_identifier, CDependencyList *_prevDList);
	
	void                   printProducerDList(FILE *dependencyOut, int codeline_cnt) const;
	
	int                    getProducerStatementLine(int def = 0) const { return producer ? producer->getStatementLine(def) : def; };
public:
	static CDependencyList* makeProducerDListEntry(CStatement *_statement, CIdentifier *_identifier);
	static CDependencyList* makeConsumerDListEntry(CStatement *_statement, CIdentifier *_identifier);
	const CStatement*      getStatement() const { return statement; };
	const CDependencyList *getPrevDList() const { return prevDList; };
	const CDependencyList *getNextDList() const { return nextDList; };
	const CProducerList   *getProducerList() const { return producerList; };
	int                    getStatementLine(int def = 0) const; // return line number of contained statement or 0 if there is no embedded statement
	int                    getConsumerStatementLine(int def = 0) const { return consumer ? consumer->getStatementLine(def) : def; };
	const char            *getName() const;
	/* Append a CProducerList entry to the end of producerList (which is also identifier->lastProducerList) */
	void                   AppendToProducerList(CProducer *_producer);
	void                   AddAsLastConsumerToProducers();
	/* add element atRight as the rightmost element to the current dependency list */
	void                   addRightmost(CDependencyList * atRight);
	void                   printConsumerFormat(FILE *dependencyOut, int codeline_cnt) const; // print DList in Consumer Format
};

#endif //CDEPENDENCYLIST__H__INCLUDED_

