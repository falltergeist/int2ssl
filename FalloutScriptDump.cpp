/**
 *
 * Copyright (c) 2005-2009 Anchorite (TeamX), <anchorite2001@yandex.ru>
 * Copyright (c) 2014-2015 Nirran, phobos2077
 * Copyright (c) 2015 alexeevdv <mail@alexeevdv.ru>
 * Distributed under the GNU GPL v3. For full terms see the file license.txt
 *
 */

// C++ standard includes

// int2ssl includes
#include "FalloutScript.h"
#include "Utility.h"

// Third party includes

void CFalloutScript::Dump(CArchive& ar)
{
    ar.WriteString("============== Procedures table ==================\n");
    ar.WriteString("\n");
    m_ProcTable.Dump(ar);
    ar.WriteString("\n");
    ar.WriteString("\n");

    ar.WriteString("============== Namespace ==================\n");
    m_Namespace.Dump(ar);
    ar.WriteString("\n");
    ar.WriteString("\n");

    ar.WriteString("============== Stringspace ==================\n");
    m_Stringspace.Dump(ar);
    ar.WriteString("\n");
    ar.WriteString("\n");


    std::string strOutLine;
    uint16_t wOperator;
    uint32_t ulArgument = 0;

    ar.WriteString("============== Global variables values ==================\n");

    if (m_GlobalVar.empty())
    {
        ar.WriteString("Not found\n");
    }
    else
    {
        for(unsigned int i = 0; i < m_GlobalVar.size(); i++)
        {
            wOperator = m_GlobalVar[i].GetOperator();
            ulArgument = m_GlobalVar[i].GetArgument();

            switch(wOperator)
            {
                case COpcode::O_STRINGOP:
                case COpcode::O_INTOP:
                    strOutLine = format("%d: %s(0x%08x)   // %u (%d)\n",
                                    i,
                                    m_GlobalVar[i].GetAttributes().m_strMnemonic.c_str(),
                                    ulArgument,
                                    ulArgument,
                                    ulArgument);
                    ar.WriteString(strOutLine);
                    break;

                case COpcode::O_FLOATOP:
                    strOutLine = format("%d: %s(0x%08x)   // %05f\n",
                                    i,
                                    m_GlobalVar[i].GetAttributes().m_strMnemonic.c_str(),
                                    ulArgument,
                                    *((float*)(&ulArgument)));
                    ar.WriteString(strOutLine);
            }
        }
    }

    ar.WriteString("\n");
    ar.WriteString("\n");

    ar.WriteString("============== Exported variables ==================\n");

    if (m_ExportedVarValue.empty())
    {
        ar.WriteString("Not found\n");
    }
    else
    {
        for(uint32_t i = 0; i < m_ExportedVarValue.size(); i += 2)
        {
            wOperator = m_ExportedVarValue[i].GetOperator();
            ulArgument = m_ExportedVarValue[i].GetArgument();
            uint32_t ulNameArgument = m_ExportedVarValue[i + 1].GetArgument();

            switch(wOperator)
            {
                case COpcode::O_STRINGOP:
                    strOutLine = format("%s := \"%s\"\n",
                                    m_Namespace[ulNameArgument].c_str(),
                                    m_Stringspace[ulArgument].c_str());
                    ar.WriteString(strOutLine);
                    break;

                case COpcode::O_INTOP:
                    strOutLine = format("%s := %u (%d)\n",
                                    m_Namespace[ulNameArgument].c_str(),
                                    ulArgument,
                                    ulArgument); 
                    ar.WriteString(strOutLine);
                    break;

                case COpcode::O_FLOATOP:
                    strOutLine = format("%s := %05f\n",
                                    m_Namespace[ulNameArgument].c_str(),
                                    *((float*)(&ulArgument)));
                    ar.WriteString(strOutLine);
            }
        }
    }

    ar.WriteString("\n");
    ar.WriteString("\n");

    ar.WriteString("============== Procedures ==================\n");
    ar.WriteString("\n");

    for(uint32_t nIndexOfProc = 0; nIndexOfProc < m_ProcTable.GetSize(); nIndexOfProc++)
    {
        strOutLine = format("%d: %s (0x%08x)\n", nIndexOfProc,
                                               m_Namespace[m_ProcTable[nIndexOfProc].m_ulNameOffset].c_str(),
                                               m_ProcTable[nIndexOfProc].m_ulBodyOffset);
        ar.WriteString(strOutLine);
        ar.WriteString("===============================\n");

        for(uint32_t i = 0; i < m_ProcBodies[nIndexOfProc].size(); i++)
        {
            wOperator = m_ProcBodies[nIndexOfProc][i].m_Opcode.GetOperator();
            ulArgument = m_ProcBodies[nIndexOfProc][i].m_Opcode.GetArgument();

            switch(wOperator)
            {
                case COpcode::O_STRINGOP:
                case COpcode::O_INTOP:
                    strOutLine = format("0x%08X: 0x%04X 0x%08x - %s(0x%08x)   // %u (%d)\n",
                                      m_ProcBodies[nIndexOfProc][i].m_ulOffset,
                                      wOperator,
                                      ulArgument,
                                      m_ProcBodies[nIndexOfProc][i].m_Opcode.GetAttributes().m_strMnemonic.c_str(),
                                      ulArgument,
                                      ulArgument,
                                      ulArgument);
                    ar.WriteString(strOutLine);
                    break;

                case COpcode::O_FLOATOP:
                    strOutLine = format("0x%08X: 0x%04X 0x%08X - %s // %05f\n",
                                      m_ProcBodies[nIndexOfProc][i].m_ulOffset,
                                      wOperator,
                                      ulArgument,
                                      m_ProcBodies[nIndexOfProc][i].m_Opcode.GetAttributes().m_strMnemonic.c_str(),
                                      *((float*)(&ulArgument)));
                    ar.WriteString(strOutLine);
                    break;

                default:
                    strOutLine = format("0x%08X: 0x%04X            - %s\n",
                                      m_ProcBodies[nIndexOfProc][i].m_ulOffset,
                                      wOperator, 
                                      m_ProcBodies[nIndexOfProc][i].m_Opcode.GetAttributes().m_strMnemonic.c_str());
                    ar.WriteString(strOutLine);
            }
        }

        ar.WriteString("\n");
    }
}
