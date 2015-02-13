#pragma once

#ifndef STARTUP_CODE_H
#define STARTUP_CODE_H


#include "Opcode.h"

// CStartupCode
class CStartupCode : public CObject {
public:
	enum Defaults {
		c_nDefaultSize = 42
	};

public:
	CStartupCode();
	virtual ~CStartupCode();

public:
	virtual void Serialize(CArchive& ar);

private:
	COpcode m_Code[17];
};

#endif
