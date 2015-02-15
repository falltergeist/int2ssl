#ifndef STARTUP_CODE_H
#define STARTUP_CODE_H

// C++ standard includes

// int2ssl includes
#include "Opcode.h"

// Third party includes

class CStartupCode
{
public:

    enum Defaults {
        c_nDefaultSize = 42
    };

    CStartupCode();
    virtual ~CStartupCode();
    virtual void Serialize(CArchive& ar);

private:
    COpcode m_Code[17];
};

#endif // STARTUP_CODE_H
