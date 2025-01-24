/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#include "CValue.h"
#include "string.h"

eAnArgDPLYType  CValue::globalDplyType = ANARG_DPLY_NORMAL;
int CValue::globalFloatPrecision = DEFAULT_FLOAT_PRECISION;

CValue::CValue()
{
	l = 0;
	ul = 0;
    memset(c, 0, sizeof(long64));
	f = 0;
	format = ANARG_NONE;
	isSet = false;
	dataStr = "";
	labelStr = "";
	dplyType = ANARG_DPLY_NORMAL;
	m_bitSize = 0;
}



void CValue::setLong(long64 v, char* text, eAnArgTypes f)
{
	isSet = true;
	l = v;
	format = f;
	dataStr = text;
}


void CValue::setFloat(double v, char* text)
{
	isSet = true;
	f = v;
	format = ANARG_DOUBLE;
	dataStr = text;
}

char* CValue::getValuStr(char *buf, bool expandEnums)
{
	if (globalDplyType == ANARG_DPLY_NORMAL){
		if (!expandEnums && !labelStr.isEmpty()) {
			return labelStr.buf;
		}
		else if (!dataStr.isEmpty()) {
			return dataStr.buf;
		}
	}
	buf[0] = '\0';
	switch (format) {
	case ANARG_BOOL:
		if (l == 0) {
			sprintf(buf, "false");
		}
		else {
			sprintf(buf, "true");
		}
		break;
	case ANARG_INTEGER:
		if (globalDplyType == ANARG_DPLY_HEX) {
            ulong64 bitmask = 0xffffffffffffffff >> (64 - m_bitSize);
            if (m_bitSize == 0) {
                sprintf(buf, "0x%x", (int)l);
            } else if (m_bitSize <=8) {
				short x = l & 0xff;
                sprintf(buf, "0x%hx", (short)(x & bitmask));
            }
            else if (m_bitSize<=16) {
				short x = l & 0xffff;
                sprintf(buf, "0x%hx", (short)(x & bitmask));
			}
            else if (m_bitSize <= 32) {
                long x = l & 0xffffffff;
                sprintf(buf, "0x%lx", (long)(x & bitmask));
            }
			else {
                sprintf(buf, "0x%llx", (l & bitmask));
			}
		}
		else {
            sprintf(buf, "%lld", l);
		}
		break;
	case ANARG_UNSIGNED_INTEGER:
		if (globalDplyType == ANARG_DPLY_HEX) {
			sprintf(buf, "0x%llx",  ul);
		}
		else if (globalDplyType == ANARG_DPLY_INTEGER){
			sprintf(buf, "%llu", ul);
		}
		else if (dplyType == ANARG_DPLY_HEX){
			sprintf(buf, "0x%llx",  ul);
		}
		else {
			sprintf(buf, "%llu", ul);
		}
		break;
	case ANARG_DOUBLE:
		sprintf(buf, "%.*G", globalFloatPrecision, f);
		break;
    case ANARG_FLOAT32:
        sprintf(buf, "%.*G", globalFloatPrecision, f32);
        break;
	case ANARG_STRING:
	case ANARG_LABEL:
    case ANARG_ENUM_ELEMENT:
    case ANARG_ENUM_TYPE:
	case ANARG_CONSTLABEL:
		if (!expandEnums && !labelStr.isEmpty()) {
			return labelStr.buf;
		}
		else if (!dataStr.isEmpty()) {
			return dataStr.buf;
		}
		break;
	default:
		//int dbg = 0;
		break;
	}

	return buf;

}

// greater than(signed / unsigned)
bool CValue::gt(CValue* cmpValue)
{
	bool retval = false;
	switch (format){
	case ANARG_INTEGER:
		switch (cmpValue->format) {
		case ANARG_INTEGER:
			retval = l > cmpValue->l;
			break;
		case ANARG_UNSIGNED_INTEGER:
			if (l >= 0){
				retval = ul > cmpValue->ul;
			}
			else {
				retval = false;
			}
			break;
        case ANARG_DOUBLE:
            retval = double(l) > cmpValue->f;
            break;
        case ANARG_FLOAT32:
            retval = float(l) > cmpValue->f32;
            break;
		default:
			break;
		}
		break;
	case ANARG_UNSIGNED_INTEGER:
		switch (cmpValue->format) {
		case ANARG_INTEGER:
			if (cmpValue->l >= 0){
				retval = ul > cmpValue->ul;
			}
			else {
				retval = true;
			}
			break;
		case ANARG_UNSIGNED_INTEGER:
			retval = ul > cmpValue->ul;
			break;
		case ANARG_DOUBLE:
			retval = double(ul) > cmpValue->f;
			break;
        case ANARG_FLOAT32:
            retval = float(ul) > cmpValue->f32;
            break;
		default:
			break;
		}
		break;
    case ANARG_DOUBLE:
        switch (cmpValue->format) {
        case ANARG_INTEGER:
            retval = f > double(cmpValue->l);
            break;
        case ANARG_UNSIGNED_INTEGER:
            retval = f > double(cmpValue->ul);
            break;
        case ANARG_DOUBLE:
            retval = f > cmpValue->f;
            break;
        case ANARG_FLOAT32:
            retval = f > cmpValue->f32;
            break;
        default:
            break;
        }
        break;
	case ANARG_FLOAT32:
		switch (cmpValue->format) {
		case ANARG_INTEGER:
			retval = f32 > double(cmpValue->l);
			break;
		case ANARG_UNSIGNED_INTEGER:
			retval = f32 > double(cmpValue->ul);
			break;
		case ANARG_DOUBLE:
			retval = f32 > cmpValue->f;
			break;
        case ANARG_FLOAT32:
            retval = f32 > cmpValue->f32;
            break;
		default:
			break;
		}
        break;
	default:
		break;
	}

	return retval;
}

// less than (signed / unsigned)
bool CValue::lt(CValue* cmpValue)
{
	bool retval = false;
	switch (format){
	case ANARG_INTEGER:
		switch (cmpValue->format) {
		case ANARG_INTEGER:
			retval = l < cmpValue->l;
			break;
		case ANARG_UNSIGNED_INTEGER:
			if (l >= 0){
				retval = ul < cmpValue->ul;
			}
			else {
				retval = true;
			}
			break;
		case ANARG_DOUBLE:
			retval = double(l) < cmpValue->f;
			break;
        case ANARG_FLOAT32:
            retval = float(l) < cmpValue->f32;
            break;
		default:
			break;
		}
		break;
	case ANARG_UNSIGNED_INTEGER:
		switch (cmpValue->format) {
		case ANARG_INTEGER:
			if (cmpValue->l >= 0){
				retval = ul < cmpValue->ul;
			}
			else {
				retval = false;
			}
			break;
		case ANARG_UNSIGNED_INTEGER:
			retval = ul < cmpValue->ul;
			break;
		case ANARG_DOUBLE:
			retval = double(ul) < cmpValue->f;
			break;
        case ANARG_FLOAT32:
            retval = float(ul) < cmpValue->f32;
            break;
		default:
			break;
		}
		break;
	case ANARG_DOUBLE:
		switch (cmpValue->format) {
		case ANARG_INTEGER:
			retval = f < double(cmpValue->l);
			break;
		case ANARG_UNSIGNED_INTEGER:
			retval = f < double(cmpValue->ul);
			break;
		case ANARG_DOUBLE:
			retval = f < cmpValue->f;
			break;
        case ANARG_FLOAT32:
            retval = f < cmpValue->f32;
            break;
		default:
			break;
		}
        break;
    case ANARG_FLOAT32:
        switch (cmpValue->format) {
        case ANARG_INTEGER:
            retval = f32 < double(cmpValue->l);
            break;
        case ANARG_UNSIGNED_INTEGER:
            retval = f32 < double(cmpValue->ul);
            break;
        case ANARG_DOUBLE:
            retval = f32 < cmpValue->f;
            break;
        case ANARG_FLOAT32:
            retval = f32 < cmpValue->f32;
            break;
        default:
            break;
        }
        break;
	default:
		break;
	}

	return retval;
}

void CValue::signExtend(int bitsize)
{
    if (bitsize > 0) {
        m_bitSize = bitsize;
    }
	if (format == ANARG_UNSIGNED_INTEGER) {
		format = ANARG_INTEGER;
        if ((unsigned)m_bitSize < (sizeof(long64)*8)) {
            if ((l >> m_bitSize) == 0){  // only execute for valid range values
                int shift = (8 * sizeof(long64)-m_bitSize);
				l = (l << shift) >> shift;
			}
		}

	}
}

/* convert value type to identifier type, also using float32 */
void CValue::convertByteTypes(int identTypeId)
{
    switch (identTypeId) {
    case TYPE_FLOAT:
        switch (format) {
        case ANARG_INTEGER:
            f32 = (float)l;
            break;
        case ANARG_UNSIGNED_INTEGER:
            f32 = (float)ul;
            break;
        case ANARG_DOUBLE:
            f32 = (float)f;
            break;
        case ANARG_FLOAT32:
            break;
        default:
            break;
        }
        format = ANARG_FLOAT32;
        break;
    case TYPE_DOUBLE:
        switch (format) {
        case ANARG_INTEGER:
            f = (double)l;
            break;
        case ANARG_UNSIGNED_INTEGER:
            f = (double)ul;
            break;
        case ANARG_DOUBLE:
            f = (double)f;
            break;
        case ANARG_FLOAT32:
            f = (double)f32;
            break;
        default:
            break;
        }
        format = ANARG_DOUBLE;
        break;
    }
}


void CValue::copy(CValue* from)
{
    l = from->l;
    ul = from->ul;
    memmove(c, from->c, 8);
    f = from->f;
    m_bitSize = from->m_bitSize;
    dataStr = from->dataStr.buf;									
    labelStr = from->labelStr.buf;									
    format=from->format;
    dplyType=from->dplyType;
}

