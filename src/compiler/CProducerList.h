/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

//!	Defines CProducerList class
/*! \file 
*/
#include "CIdentifier.h"
#include "CStatement.h"
#include <list>

#ifndef CPRODUCERLIST__H__INCLUDED_
#define CPRODUCERLIST__H__INCLUDED_

class CProducer {
private:
	CProducer(); // prevent no-args constructor from being called
public:
	CProducer(CIdentifier *_identifier, CStatement *_statement, int _branch);

	CIdentifier* identifier;
	CStatement  *statement;
	int branch;
};

typedef std::list<CProducer *> CProducerList;


#endif	// CPRODUCERLIST__H__INCLUDED_

