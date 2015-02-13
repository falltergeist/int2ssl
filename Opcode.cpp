// Opcode.cpp : implementation file
//

#include "stdafx.h"
#include "Opcode.h"
#include "Utility.h"


#include <iostream>

// Globals
extern int g_nFalloutVersion;


// COpcode

COpcode::COpcode() :
	m_wOperator(O_NOOP),
	m_ulArgument(0)
{
}

COpcode::COpcode(const COpcode& opcode) :
	m_wOperator(opcode.m_wOperator),
	m_ulArgument(opcode.m_ulArgument)
{
}


COpcode::~COpcode()
{
}

COpcode& COpcode::operator = (const COpcode& opcode)
{
	m_wOperator  = opcode.m_wOperator;
	m_ulArgument = opcode.m_ulArgument;

	return (*this);
}

void COpcode::Serialize(CArchive& ar)
{
		if (ReadMSBWord(ar, m_wOperator) != OPERATOR_SIZE) {

			printf("Error: Unable read opcode\n");
			AfxThrowUserException();
		}

		if ((m_wOperator < O_OPERATOR) ||
			((m_wOperator >= O_END_OP) && 
			 (m_wOperator != O_STRINGOP) && 
			 (m_wOperator != O_FLOATOP)&& 
			 (m_wOperator != O_INTOP))) {
			ar.Flush();
			printf("Error: Invalid opcode at 0x%08x\n", ar.GetFile()->GetPosition() - 2);
			AfxThrowUserException();
		}

		if ((m_wOperator == O_STRINGOP) || (m_wOperator == O_FLOATOP) || (m_wOperator == O_INTOP)) {
			if (ReadMSBULong(ar, m_ulArgument) != ARGUMENT_SIZE) {
				printf("Error: Unable read opcode argument\n");
				AfxThrowUserException();
			}
		}
}

void COpcode::Expect(CArchive& ar, WORD wOperator, BOOL bArgumentFound, ULONG ulArgument)
{
	Serialize(ar);

	if (m_wOperator != wOperator) {
		printf("Error: Unexpected opcode (0x%04X expected, but 0x%04X found)\n", wOperator, m_wOperator);
		AfxThrowUserException();
	}

	if (bArgumentFound && ((m_wOperator == O_STRINGOP) || (m_wOperator == O_FLOATOP) || (m_wOperator == O_INTOP))) {
		if (m_ulArgument != ulArgument) {
			printf("Error: Unexpected argument of opcode. (0x%08X expected, but 0x%08X found)\n", ulArgument, m_ulArgument);
			AfxThrowUserException();
		}
	}
}

void COpcode::Expect(CArchive& ar, int nCount, WORD pwOperators[])
{
	Serialize(ar);
	BOOL bFound = FALSE;

	for(int i = 0; i < nCount; i++) {
		if (m_wOperator == pwOperators[i]) {
			bFound = TRUE;
			break;
		}
	}

	if (!bFound) {
		printf("Error: Unexpected opcode (");

		for(int i = 0; i < nCount; i++) {
			if (i != 0) {
				printf(" or ");
			}

			printf("0x%04X", pwOperators[i]);
		}

		printf(" expected, but 0x%04X found)\n", m_wOperator);
		AfxThrowUserException();
	}
}

BOOL COpcode::HasArgument() const
{
	return ((m_wOperator == O_STRINGOP) || (m_wOperator == O_FLOATOP) || (m_wOperator == O_INTOP));
}

int COpcode::GetSize() const
{
	if ((m_wOperator == O_STRINGOP) || (m_wOperator == O_FLOATOP) || (m_wOperator == O_INTOP)) {
		return OPERATOR_SIZE + ARGUMENT_SIZE;
	}
	else {
		return OPERATOR_SIZE;
	}
}

WORD COpcode::GetOperator() const
{
	return m_wOperator;
}

void COpcode::SetOperator(WORD op)
{
	m_wOperator = op;
}

ULONG COpcode::GetArgument() const
{
	return m_ulArgument;
}

const COpcode::COpcodeAttributes COpcode::GetAttributes() const
{
	static CF1OpcodeAttributesMap f1OpcodeAttributes;
	static CF2OpcodeAttributesMap f2OpcodeAttributes;
	
	COpcodeAttributes result;

	if (g_nFalloutVersion == 1) {
		if (f1OpcodeAttributes.Lookup(m_wOperator, result)) {
			return result;
		}
	}

	if (!f2OpcodeAttributes.Lookup(m_wOperator, result)) {
		printf("Error: Attempt to obtain attributes of unknown opcode\n");
		AfxThrowUserException();
	}

	return result;
}
