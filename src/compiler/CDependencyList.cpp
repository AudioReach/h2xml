/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/



/////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief					 CDependencyList class implementation									\file
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CDependencyList.h"


/*
CDependencyList::CDependencyList()
{
memset(this,0,sizeof(CDependencyList));
}
*/

CDependencyList::CDependencyList(CStatement *_statement, CIdentifier *_identifier, CDependencyList *_prevDList)
: statement(_statement)
, prevDList(_prevDList)
, nextDList(0)
, identifier(_identifier)
, producerList(0)
, producer(0)
, consumer(0)
, left(0)
, right(0)
{
	if (identifier) {
		identifier->m_nr_total_use++;
	}
	if (prevDList)
		prevDList->nextDList = this;
}

CDependencyList* CDependencyList::makeProducerDListEntry(CStatement *statement, CIdentifier *identifier)
{
	CDependencyList *list = new CDependencyList(statement,
		identifier,
		identifier ? identifier->last_producer : 0
		);
	if (identifier)
		identifier->last_producer = list;

	return list;
}

CDependencyList* CDependencyList::makeConsumerDListEntry(CStatement *statement, CIdentifier *identifier)
{
	CDependencyList *list = new CDependencyList(statement,
		identifier,
		identifier ? identifier->last_consumer : 0
		);
	if (identifier)
		identifier->last_consumer = list;

	return list;
}

int CDependencyList::getStatementLine(int def) const
{
	return (statement != NULL) ? statement->getCodeline() : def;
};

const char *CDependencyList::getName() const {
	return identifier->getName();
};


void CDependencyList::AppendToProducerList(CProducer *_producer)
{
	if (identifier)
	{
		identifier->AppendToLastProducerList(_producer);
		producerList = identifier->last_producerList; // FIXME: Do we suffer from some double bookkeeping here
		// and producerList always shadows identifier->last_producerList?
		// If so remove the producerList member and instead refer to the entry from identifier
	}
};

void CDependencyList::AddAsLastConsumerToProducers()
{
	if (!identifier)
		return;
	if (!identifier->last_producer) {
		ycc_erpnt(114, getName());
	}
	else {
		producer = identifier->last_producer; // FIXME: does producer also just shadow identifier->last_producer (always or only under certain circumstances)
		CDependencyList *checkList = producer; // FIXME: iteration to end of list, replace by fn
		while (checkList->consumer)
			checkList = checkList->consumer;
		checkList->consumer = this;
	}
};

void CDependencyList::addRightmost(CDependencyList * atRight)
{
	CDependencyList *current = this;
	while (current->right) // FIXME: iteration to end of list, replace by fn
		current = current->right;
	atRight->left = current;
	current->right = atRight;
}

void CDependencyList::printProducerDList(FILE *dependencyOut, int codeline_cnt) const
{
	if (!producer) // Error, no producer
		fprintf(dependencyOut, "??NO PRODUCER??,");
	else {
		CProducerList &pList = *(producer->producerList);
		for (CProducerList::const_iterator it = pList.begin(); it != pList.end(); it++) {
			if (codeline_cnt >= (*it)->statement->getCodeline())
				fprintf(dependencyOut, "%05d(%01d),", (*it)->statement->getCodeline(), (*it)->branch);
		}
	}
}

void CDependencyList::printConsumerFormat(FILE *dependencyOut, int codeline_cnt) const
{
	const CDependencyList *current = this;
	while (current != NULL) // FIXME: iterator smell
	{
		const char *consumerName = current->getName();
		//int producerLine = current->getProducerStatementLine(-1);
		int prevLine = (current->getPrevDList() != NULL) ? current->getPrevDList()->getStatementLine() : 0;
		int nextLine = (current->getNextDList() != NULL) ? current->getNextDList()->getStatementLine() : 0;
		fprintf(dependencyOut, "\t\trd: %s,", consumerName);
		// write producers of this consumer
		current->printProducerDList(dependencyOut, codeline_cnt);
		// write prev and next line
		fprintf(dependencyOut, "%05d,%05d", prevLine, nextLine);

		current = current->right;
	}
}
