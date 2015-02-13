#include "stdafx.h"
#include "StartupCode.h"

CStartupCode::CStartupCode()
{
}

CStartupCode::~CStartupCode()
{
}

void CStartupCode::Serialize(CArchive& ar)
{
    WORD wExpectOpcodes[17] = {
        COpcode::O_CRITICAL_START,
        COpcode::O_INTOP,
        COpcode::O_D_TO_A,
        COpcode::O_INTOP,
        COpcode::O_JMP,
        COpcode::O_EXIT_PROG,
        COpcode::O_POP,
        COpcode::O_POP_FLAGS_RETURN,
        COpcode::O_POP,
        COpcode::O_POP_FLAGS_EXIT,
        COpcode::O_POP,
        COpcode::O_POP_FLAGS_RETURN_EXTERN,
        COpcode::O_POP,
        COpcode::O_POP_FLAGS_EXIT_EXTERN,
        COpcode::O_POP_FLAGS_RETURN_VAL_EXTERN,
        COpcode::O_POP_FLAGS_RETURN_VAL_EXIT,
        COpcode::O_POP_FLAGS_RETURN_VAL_EXIT_EXTERN
    };

    for(INT_PTR i = 0; i < 17; i++)
    {
        if (i == 1)
        {
            m_Code[i].Expect(ar, wExpectOpcodes[i], TRUE, 18);
        }
        else
        {
            m_Code[i].Expect(ar, wExpectOpcodes[i]);
        }
    }

}
