#include "stdafx.h"
#include "FalloutScript.h"

#include <iostream>

CFalloutScript::CFalloutScript()
{
}

CFalloutScript::~CFalloutScript()
{
}

void CFalloutScript::Serialize(CArchive& ar)
{

    printf("  Read strtup code\n");
    m_StartupCode.Serialize(ar);

    printf("  Read procedures table\n");
    m_ProcTable.Serialize(ar);

    printf("  Read namespace\n");
    m_Namespace.Serialize(ar);

    printf("  Read stringspace\n");
    m_Stringspace.Serialize(ar);

    COpcode opcode;

    printf("  Read tail of startup code\n");
    opcode.Expect(ar, COpcode::O_SET_GLOBAL);

    // Sections with variable sizes
    ar.Flush();
    ULONG ullCurrentOffset = ar.GetFile()->GetPosition();

    // Load globals
    m_GlobalVar.RemoveAll();
    m_ExportedVar.RemoveAll();
    m_ExportedVarValue.RemoveAll();
    m_ExportedProc.RemoveAll();

    COpcodeArray HeaderTail;

    while(ullCurrentOffset < m_ProcTable.GetOffsetOfProcSection())
    {
        opcode.Serialize(ar);
        ullCurrentOffset += opcode.GetSize();
        HeaderTail.Add(opcode);
    }

    // Jump to 'start' procedure
    printf("    Check \"Jump to \'start\' procedure\" / \"Jump to end of statup code\"\n");

    if (!HeaderTail.IsEmpty())
    {
        INT_PTR nIndexOfStart = GetIndexOfProc("start");
        ULONG ulStartProcAddress = (nIndexOfStart != -1) ? m_ProcTable[nIndexOfStart].m_ulBodyOffset : 18;

        opcode = HeaderTail.at(HeaderTail.GetUpperBound());

        if (opcode.GetOperator() == COpcode::O_JMP)
        {
            HeaderTail.RemoveAt(HeaderTail.GetUpperBound());

            if (HeaderTail.IsEmpty())
            {
                printf("\n");
                printf("Warning: Omitted address of jump\n");
                printf("\n");
            }
            else
            {
                opcode = HeaderTail.at(HeaderTail.GetUpperBound());

                if (opcode.GetOperator() == COpcode::O_INTOP)
                {
                    HeaderTail.RemoveAt(HeaderTail.GetUpperBound());

                    if (opcode.GetArgument() != ulStartProcAddress)
                    {
                        printf("\n");
                        printf("Warning: Invalid jump address\n");
                        printf("\n");
                    }
                }
                else
                {
                    printf("\n");
                    printf("Warning: Invalid opcode for jump addres\n");
                    printf("\n");
                }
            }
        }
        else
        {
            printf("\n");
            printf("Warning: Omitted  \"Jump to \'start\' procedure\" / \"Jump to end of statup code\"\n");
            printf("\n");
        }
    }

    // # of argument to 'start' procedure
    printf("    Check \"# of argument to \'start\' procedure\"\n");

    CString strNumOfArgsWarning("Warning: Omitted  \"# of argument to \'start\' procedure\"\n");

    if (!HeaderTail.IsEmpty())
    {
        opcode = HeaderTail.at(HeaderTail.GetUpperBound());

        if (opcode.GetOperator() == COpcode::O_CRITICAL_DONE)
        {
            HeaderTail.RemoveAt(HeaderTail.GetUpperBound());

            if (!HeaderTail.IsEmpty())
            {
                opcode = HeaderTail.at(HeaderTail.GetUpperBound());

                if (opcode.HasArgument())
                {
                    HeaderTail.RemoveAt(HeaderTail.GetUpperBound());
                }
                else
                {
                    printf("\n");
                    printf("%s", strNumOfArgsWarning.c_str());
                    printf("\n");
                }
            }
            else
            {
                printf("\n");
                printf("%s", strNumOfArgsWarning.c_str());
                printf("\n");
            }
        }
        else
        {
            printf("\n");
            printf("Warning: Omitted expected opcode 0x8003. Opcode with # of arguments will be not analysed\n");
            printf("\n");
        }
    }
    else
    {
        printf("\n");
        printf("%s", strNumOfArgsWarning.c_str());
        printf("\n");
    }

    // Extract 'Export var' section
    printf("    Extract \"Export var\" section\n");
    ExtractCodeElements(HeaderTail, m_ExportedVar, COpcode::O_EXPORT_VAR, 2, "Error: Malformed \"Export var\" section\n", &CFalloutScript::CheckExportVarCode);

    // Extract 'Set exported var values' section
    printf("    Extract \"Set exported var values\" section\n");
    ExtractCodeElements(HeaderTail, m_ExportedVarValue, COpcode::O_STORE_EXTERNAL, 3, "Error: Malformed \"Set exported var values\" section\n", &CFalloutScript::CheckSetExportedVarValueCode);

    // Extract 'Export procedures' section
    printf("    Extract \"Export procedures\" section\n");
    ExtractCodeElements(HeaderTail, m_ExportedProc, COpcode::O_EXPORT_PROC, 3, "Error: Malformed \"Export procedures\" section\n", &CFalloutScript::CheckExportProcCode);

    // Global variables
    printf("    Extract \"Global variables\" section\n");

    for(INT_PTR i = 0; i < HeaderTail.GetSize(); i++)
    {
        WORD wGlobalVarOperator = HeaderTail.at(i).GetOperator();

        if ((wGlobalVarOperator != COpcode::O_STRINGOP) &&
            (wGlobalVarOperator != COpcode::O_FLOATOP) &&
            (wGlobalVarOperator != COpcode::O_INTOP)) 
        {
            printf("Error: Malformed \"Global variables\" section\n");
            AfxThrowUserException();
        }

        m_GlobalVar.Add(HeaderTail.at(i));
    }

    // Procedures bodyes
    printf("  Read procedure\'s bodies\n");
    m_ProcBodies.RemoveAll();   // Destroy old data
    m_Conditions.RemoveAll();
    m_ProcBodies.SetSize(m_ProcTable.GetSize());
    m_Conditions.SetSize(m_ProcTable.GetSize());

    CNode node;

    for(INT_PTR i = 0; i < m_ProcTable.GetSize(); i++)
    {
        printf("    Procedure: %d\r", i);
        ULONG ulOffset = m_ProcTable[i].m_ulBodyOffset;
        ULONG ulSize = m_ProcTable.GetSizeOfProc(i);
        ar.Flush();
        ar.GetFile()->Seek(ulOffset, CFile::begin);

        while(ulOffset < m_ProcTable[i].m_ulBodyOffset + ulSize)
        {
            node.m_Opcode.Serialize(ar);
            node.m_ulOffset = ulOffset;
            m_ProcBodies.at(i).Add(node);
            ulOffset += node.m_Opcode.GetSize();
        }
    }
}

void CFalloutScript::ExtractCodeElements(COpcodeArray& Source, COpcodeArray& Destination, WORD wDelimeter, int nSizeOfCodeItem, LPCTSTR lpszErrorMessage, BOOL (CFalloutScript::*pCheckFunc)(WORD, INT_PTR))
{
    INT_PTR i = 0;

    for(; i < Source.GetSize(); i++)
    {
        if (Source.at(i).GetOperator() == wDelimeter)
        {
            break;
        }
    }

    if (i < Source.GetSize())
    {
        if (i < nSizeOfCodeItem - 1)
        {
            printf(lpszErrorMessage);
            AfxThrowUserException();
        }

        while(Source.at(i).GetOperator() == wDelimeter)
        {
            for(INT_PTR j = 0; j < nSizeOfCodeItem - 1; j++)
            {
                if (!((this->*pCheckFunc)(Source.at(i - nSizeOfCodeItem + 1 + j).GetOperator(), j)))
                {
                    printf(lpszErrorMessage);
                    AfxThrowUserException();
                }
            }

            for(INT_PTR j = 0; j < nSizeOfCodeItem - 1; j++)
            {
                Destination.Add(Source.at(i - nSizeOfCodeItem + 1));
                Source.RemoveAt(i - nSizeOfCodeItem + 1);
            }

            Source.RemoveAt(i - nSizeOfCodeItem + 1);   // Delimeter


            if (i > Source.GetUpperBound())
            {
                break;
            }
        }
    }
}

BOOL CFalloutScript::CheckExportVarCode(WORD wOperator, INT_PTR nIndex)
{
    return (nIndex == 0) && (wOperator == COpcode::O_STRINGOP);
}

BOOL CFalloutScript::CheckSetExportedVarValueCode(WORD wOperator, INT_PTR nIndex)
{
    switch(nIndex)
    {
        case 0:
            return (wOperator == COpcode::O_STRINGOP) || 
                   (wOperator == COpcode::O_FLOATOP)  || 
                   (wOperator == COpcode::O_INTOP);
        case 1:
            return (wOperator == COpcode::O_STRINGOP);

        default:
            return FALSE;
    }
}

BOOL CFalloutScript::CheckExportProcCode(WORD wOperator, INT_PTR nIndex)
{
    return ((nIndex == 0) || (nIndex == 1)) && (wOperator == COpcode::O_INTOP);
}

bool CFalloutScript::ArgNeedParens( const CNode& node, const CNode& argument, CFalloutScript::Assoc assoc)
{
    return (argument.IsInfix()
        && ((GetPriority(argument.m_Opcode.GetOperator()) != GetPriority(node.m_Opcode.GetOperator()))
            || (GetAssociation(node.m_Opcode.GetOperator()) != assoc)));
}

