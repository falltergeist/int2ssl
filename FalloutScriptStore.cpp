#include "stdafx.h"
#include "FalloutScript.h"
#include "ObjectAttributes.h"

// Globals
extern CString g_strIndentFill;

void CFalloutScript::StoreTree(CArchive& ar)
{
    CString strOutLine;

    ar.WriteString("============== Procedures ==================\n");
    ar.WriteString("\n");

    for(INT_PTR nIndexOfProc = 0; nIndexOfProc < m_ProcTable.GetSize(); nIndexOfProc++)
    {
        strOutLine.Format("%d: %s (0x%08x)\n", nIndexOfProc, m_Namespace[m_ProcTable[nIndexOfProc].m_ulNameOffset].c_str(), m_ProcTable[nIndexOfProc].m_ulBodyOffset);
        ar.WriteString(strOutLine);
        ar.WriteString("===============================\n");

        if (m_ProcTable[nIndexOfProc].m_ulType & P_CONDITIONAL)
        {
            ar.WriteString("Condition\n");
            ar.WriteString("===============================\n");

            for(INT_PTR i = 0; i < m_Conditions[nIndexOfProc].GetSize(); i++)
            {
                strOutLine.Format("0x%08X: ", m_Conditions[nIndexOfProc][i].m_ulOffset);
                ar.WriteString(strOutLine);
                m_Conditions[nIndexOfProc][i].StoreTree(ar, 0, 0);
            }

            ar.WriteString("\n");
            ar.WriteString("Body\n");
            ar.WriteString("===============================\n");
        }

        for(INT_PTR i = 0; i < m_ProcBodies[nIndexOfProc].GetSize(); i++)
        {
            strOutLine.Format("0x%08X: ", m_ProcBodies[nIndexOfProc][i].m_ulOffset);
            ar.WriteString(strOutLine);
            m_ProcBodies[nIndexOfProc][i].StoreTree(ar, 0, 0);
        }

        ar.WriteString("\n");
    }

}

void CFalloutScript::StoreSource(CArchive& ar)
{
    StoreDefinitions(ar);
    ar.WriteString("\n");
    ar.WriteString("\n");
    StoreDeclarations(ar);
}

void CFalloutScript::StoreDefinitions(CArchive& ar)
{
    CString c_strBogusProcedureName("..............");
    CString c_strArgumentTemplate("arg%u");

    printf("    Storing definitions\n");

    // Unnamed global variables
    INT_PTR nNamesCount = m_Namespace.GetSize();
    INT_PTR nDefinitionsCount = m_Definitions.GetSize();
    CString strDefinition;
    ULONG ulVarValue;

    if (nNamesCount != nDefinitionsCount)
    {
        ar.WriteString("/*******************************************************\n");
        ar.WriteString("*      Some unreferenced imported varables found.      *\n");
        ar.WriteString("*      Because of it it is impossible to specify       *\n");
        ar.WriteString("*      the real names of global variables.             *\n");
        ar.WriteString("*******************************************************/\n");
        ar.WriteString("\n");

        for(INT_PTR i = 0; i < m_GlobalVar.GetSize(); i++)
        {
            ulVarValue = m_GlobalVar[i].GetArgument();
            strDefinition += "variable ";
            strDefinition += m_GlobalVarsNames[i];

            switch(m_GlobalVar[i].GetOperator())
            {
                case COpcode::O_STRINGOP:
                    strDefinition.Format(strDefinition + " := \"%s\"", m_Stringspace[ulVarValue].c_str());
                    break;

                case COpcode::O_FLOATOP:
                    strDefinition.Format(strDefinition + " := %.5f", *((float*)(&ulVarValue)));
                    break;

                case COpcode::O_INTOP:
                    if (ulVarValue != 0)
                    {
                        strDefinition.Format(strDefinition + " := %d", (long)ulVarValue);
                    }
                    break;
            }

            strDefinition += ";";

            if ((m_GlobalVar[i].GetOperator() == COpcode::O_INTOP) && (ulVarValue & 0x80000000))
            {
                strDefinition.Format(strDefinition + " /* (%d) */", ulVarValue);
            }

            ar.WriteString(strDefinition + "\n");
        }

        ar.WriteString("\n");
    }


    // Named objects
    ULONG ulNameOffset;
    CDefObject defObject;

    enum OutDefObject {
        OUT_NOTHING,
        OUT_UNKNOWN_VARIABLE,
        OUT_GLOBAL_VARIABLE,
        OUT_EXPORTED_VARIABLE,
        OUT_IMPORTED_VARIABLE,
        OUT_PROCEDURE,
        OUT_EXPORTED_PROCEDURE,
        OUT_IMPORTED_PROCEDURE
    };

    OutDefObject lastOut = OUT_NOTHING;
    OutDefObject currentOut = OUT_NOTHING;
    CProcDescriptor procDescriptor;

    for(INT_PTR i = 0; i < m_Namespace.GetSize(); i++)
    {
        ulNameOffset = m_Namespace.GetOffsetByIndex(i);

        if (i == 0)
        {
            if (m_Namespace[ulNameOffset] == c_strBogusProcedureName.c_str())
            {
                continue;
            }
            else
            {
                std::string msg = "Warning: Bogus procedure with name ";
                            msg += c_strBogusProcedureName;
                            msg += " not found at expected location\n";
                printf(msg.c_str());
            }
        }

        if (m_Definitions.Lookup(ulNameOffset, defObject))
        {
            strDefinition = "";

            switch(defObject.m_ObjectType)
            {
                case CDefObject::OBJECT_PROCEDURE:
                    procDescriptor = m_ProcTable[defObject.m_ulProcIndex];

                    if (procDescriptor.m_ulType & P_IMPORT)
                    {
                        strDefinition = "import ";
                        currentOut = OUT_IMPORTED_PROCEDURE;
                    }
                    else if (procDescriptor.m_ulType & P_EXPORT)
                    {
                        strDefinition = "export ";
                        currentOut = OUT_EXPORTED_PROCEDURE;
                    }
                    else
                    {
                        currentOut = OUT_PROCEDURE;
                    }

                    strDefinition += "procedure ";
                    strDefinition += m_Namespace[ulNameOffset];

                    if (procDescriptor.m_ulNumArgs != 0)
                    {
                        strDefinition += "(";

                        for(ULONG i = 0; i < procDescriptor.m_ulNumArgs; i++)
                        {
                            if (i == 0)
                            {
                                strDefinition.Format(strDefinition + "variable " + c_strArgumentTemplate.c_str(), i);
                            }
                            else
                            {
                                strDefinition.Format(strDefinition + ", variable " + c_strArgumentTemplate.c_str(), i);
                            }
                        }

                        strDefinition += ")";
                    }

                    strDefinition += ";";

                    if (procDescriptor.m_ulType & P_NOTIMPLEMENTED)
                    {
                        strDefinition += " /* Prodedure defined, but not implemented */";
                    }

                    break;

                case CDefObject::OBJECT_VARIABLE:
                    if (defObject.m_ulAttributes & V_IMPORT)
                    {
                        strDefinition = "import ";
                        currentOut = OUT_IMPORTED_VARIABLE;
                    }
                    else if (defObject.m_ulAttributes & V_EXPORT)
                    {
                        strDefinition = "export ";
                        currentOut = OUT_EXPORTED_VARIABLE;
                    }
                    else 
                    {
                        currentOut = OUT_GLOBAL_VARIABLE;
                    }

                    strDefinition += "variable ";
                    strDefinition += m_Namespace[ulNameOffset];

                    if (!(defObject.m_ulAttributes & V_IMPORT))
                    {
                        switch(defObject.m_ulAttributes & 0xFFFF)
                        {
                            case COpcode::O_STRINGOP:
                                strDefinition.Format(strDefinition + " := \"%s\"", m_Stringspace[defObject.m_ulVarValue].c_str());
                                break;

                            case COpcode::O_FLOATOP:
                                strDefinition.Format(strDefinition + " := %.5f", *((float*)(&defObject.m_ulVarValue)));
                                break;

                            case COpcode::O_INTOP:
                                if (defObject.m_ulVarValue != 0)
                                {
                                    strDefinition.Format(strDefinition + " := %d", (long)defObject.m_ulVarValue);
                                }
                                break;
                        }
                    }

                    strDefinition += ";";

            }
        }
        else 
        {
            currentOut = OUT_UNKNOWN_VARIABLE;
            strDefinition = "/* ?import? variable ";
            strDefinition+= m_Namespace.GetStringByIndex(i) + "; */";
        }

        if ((currentOut != lastOut) && (lastOut != OUT_NOTHING))
        {
            ar.WriteString("\n");
        }

        lastOut = currentOut;
        ar.WriteString(strDefinition + "\n");
    }
}

void CFalloutScript::StoreDeclarations(CArchive& ar)
{
    CString c_strBogusProcedureName("..............");
    CString c_strArgumentTemplate("arg%u");
    CString c_strLocalVarTemplate("LVar%u");

    printf("    Storing declarations\n");

    CString strOutLine;

    for(INT_PTR i = 0; i < m_ProcTable.GetSize(); i++)
    {
        printf("        Procedure: %d\r", i);

        // Bogus procedure
        if ((i == 0) && (m_Namespace[m_ProcTable[i].m_ulNameOffset] == c_strBogusProcedureName.c_str()))
        {
            continue;
        }

        // Empty procedure
        if (m_ProcTable.GetSizeOfProc(i) == 0)
        {
            ar.WriteString("/*******************************************************\n");
            ar.WriteString("*    Found Procedure without body.                     *\n");
            ar.WriteString("*                                                      *\n");
            strOutLine = "*    Name: ";
            strOutLine+= m_Namespace[m_ProcTable[i].m_ulNameOffset];

            while(strOutLine.GetLength() <  55)
            {
                strOutLine += " ";
            }

            strOutLine += "*\n";
            ar.WriteString(strOutLine);

            ar.WriteString("*                                                      *\n");

            if (!(m_ProcTable[i].m_ulType & P_NOTIMPLEMENTED))
            {
                ar.WriteString("*    Other possible name(s):                           *\n");

                for(INT_PTR j = i + 1; j < m_ProcTable.GetSize(); j++)
                {
                    if (m_ProcTable[j].m_ulBodyOffset == m_ProcTable[i].m_ulBodyOffset)
                    {
                        strOutLine = "*       ";
                        strOutLine+= m_Namespace[m_ProcTable[j].m_ulNameOffset];

                        while(strOutLine.GetLength() <  55)
                        {
                            strOutLine += " ";
                        }

                        strOutLine += "*\n";
                        ar.WriteString(strOutLine);
                        strOutLine = "*       ";
                    }
                }
            }
            else
            {
                ar.WriteString("*           Not implemented                            *\n");
            }

            ar.WriteString("*                                                      *\n");
            ar.WriteString("*******************************************************/\n");
            ar.WriteString("\n");
            continue;
        }

        // 'Normal' procedure
        CProcDescriptor procDescriptor = m_ProcTable[i];

        if (procDescriptor.m_ulType & P_CRITICAL)
        {
            strOutLine = "critical ";
        }
        else
        {
            strOutLine = "";
        }

        strOutLine += "procedure ";
        strOutLine+= m_Namespace[m_ProcTable[i].m_ulNameOffset];

        if (procDescriptor.m_ulNumArgs != 0)
        {
            strOutLine += "(";

            for(ULONG i = 0; i < procDescriptor.m_ulNumArgs; i++)
            {
                if (i == 0)
                {
                    strOutLine.Format(strOutLine + "variable " + c_strArgumentTemplate.c_str(), i);
                }
                else
                {
                    strOutLine.Format(strOutLine + ", variable " + c_strArgumentTemplate.c_str(), i);
                }
            }

            strOutLine += ")";
        }

        if (procDescriptor.m_ulType & P_TIMED)
        {
            strOutLine.Format(strOutLine + " in %d", procDescriptor.m_ulTime);
        }
        else if (procDescriptor.m_ulType & P_CONDITIONAL)
        {
            strOutLine.Format(strOutLine + " when (%s)", GetSource(m_Conditions[i][0], FALSE, procDescriptor.m_ulNumArgs).c_str());
        }

        ar.WriteString(strOutLine + "\n");

        BOOL bLocalVar = TRUE;
        ULONG ulLocalVarIndex = procDescriptor.m_ulNumArgs;
        INT_PTR nIndentLevel = 0;
        CNode::Type prevNodeType = CNode::TYPE_NORMAL;

        for(INT_PTR nNodeIndex = 0; nNodeIndex < m_ProcBodies[i].GetSize(); nNodeIndex++)
        {
            if (m_ProcBodies[i][nNodeIndex].m_Type == CNode::TYPE_BEGIN_OF_BLOCK)
            {
                if ((nNodeIndex - 1 > 0) && (m_ProcBodies[i][nNodeIndex - 1].m_Type == CNode::TYPE_END_OF_BLOCK))
                {
                    ar.WriteString(GetIndentString(nIndentLevel) + "else begin\n");
                    nIndentLevel++;
                }              
                else
                {
                    ar.WriteString("begin\n");
                    nIndentLevel++;
                }
                prevNodeType = CNode::TYPE_BEGIN_OF_BLOCK;
            }
            else if (m_ProcBodies[i][nNodeIndex].m_Type == CNode::TYPE_END_OF_BLOCK)
            {
                nIndentLevel--;
                ar.WriteString(GetIndentString(nIndentLevel) + "end\n");
                prevNodeType = CNode::TYPE_END_OF_BLOCK;
            }
            else
            {
                prevNodeType = CNode::TYPE_NORMAL;
                WORD wOperator = m_ProcBodies[i][nNodeIndex].m_Opcode.GetOperator();

                if ((m_ProcBodies[i][nNodeIndex].m_Opcode.GetAttributes().m_Type == COpcode::COpcodeAttributes::TYPE_EXPRESSION) &&
                    (wOperator != COpcode::O_STRINGOP) && (wOperator != COpcode::O_FLOATOP) && (wOperator != COpcode::O_INTOP))
                {
                    printf("Warning: Result of expression is left in stack. (Opcode 0x%04X at 0x%08X)\n", wOperator, m_ProcBodies[i][nNodeIndex].m_ulOffset);
                }

                switch(wOperator)
                {
                    case COpcode::O_STRINGOP:
                    case COpcode::O_FLOATOP:
                    case COpcode::O_INTOP:
                    {
                        if (!bLocalVar)
                        {
                            printf("Warning: Free value found. Converted to definition of local variable\n");
                        }

                        std::string str = "variable ";
                        str += c_strLocalVarTemplate + " := ";

                        strOutLine.Format(str.c_str(), ulLocalVarIndex);
                        ar.WriteString(GetIndentString(nIndentLevel) + strOutLine + GetSource(m_ProcBodies[i][nNodeIndex], FALSE, procDescriptor.m_ulNumArgs) + ";\n");
                        ulLocalVarIndex++;
                        break;
                    }
                    case COpcode::O_IF:
                        ar.WriteString(GetIndentString(nIndentLevel) + GetSource(m_ProcBodies[i][nNodeIndex], FALSE, procDescriptor.m_ulNumArgs) + " then ");
                        break;

                    case COpcode::O_WHILE:
                        if (m_ProcBodies[i][nNodeIndex].m_Type == CNode::TYPE_FOR_LOOP)
                        {
                            CString str = GetIndentString(nIndentLevel) + "for (";
                            for (INT_PTR j = 0; j < m_ProcBodies[i][nNodeIndex].m_Arguments.GetSize(); j++)
                            {
                                if (j > 0)
                                {
                                    str += "; ";
                                }
                                str += GetSource(m_ProcBodies[i][nNodeIndex].m_Arguments[j], FALSE, procDescriptor.m_ulNumArgs);
                            }
                            str += ") ";
                            ar.WriteString(str);
                        }
                        else
                        {
                            ar.WriteString(GetIndentString(nIndentLevel) + GetSource(m_ProcBodies[i][nNodeIndex], FALSE, procDescriptor.m_ulNumArgs) + " do ");
                        }
                        break;

                    default:
                        ar.WriteString(GetIndentString(nIndentLevel) + GetSource(m_ProcBodies[i][nNodeIndex], FALSE, procDescriptor.m_ulNumArgs) + ";\n");

                        if ((m_ProcBodies[i][nNodeIndex].m_Type != CNode::TYPE_BEGIN_OF_BLOCK) &&
                            (m_ProcBodies[i][nNodeIndex].m_Type != CNode::TYPE_END_OF_BLOCK))
                        {
                            bLocalVar = FALSE;
                        }
                }
            }
        }

        ar.WriteString("\n");
    }
}

CString CFalloutScript::GetSource(CNode& node, BOOL bLabel, ULONG ulNumArgs)
{
    CString c_strArgumentTemplate("arg%u");
    CString c_strLocalVarTemplate("LVar%u");

    CString strResult("Damn!!!");

    if (node.m_Type == CNode::TYPE_OMITTED_ARGUMENT)
    {
        return CString("/* Omitted argument */");
    }

    WORD wOperator = node.m_Opcode.GetOperator();
    ULONG ulArgument = node.m_Opcode.GetArgument();

    // sslc additions:
    if (wOperator == COpcode::O_JMP)
    {
        if (node.m_Type == CNode::TYPE_BREAK)
        {
            return CString("break");
        }
        else if (node.m_Type == CNode::TYPE_CONTINUE)
        {
            return CString("continue");
        }
    }
    switch(wOperator)
    {
        case COpcode::O_STRINGOP:
            try
            {
                if (bLabel)
                {
                    strResult = m_Namespace[ulArgument];
                }
                else
                {
                    strResult = "\"";
                    strResult += m_Stringspace[ulArgument] + "\"";
                }
            }

            catch(UserException& e)
            {
                printf("Warning: Restoration after exception. Object name replaced with '/* Fake object name */'\n");
                strResult = "/* Fake object name */";
            }

            break;

        case COpcode::O_FLOATOP:
            strResult.Format("%.5f", *((float*)(&ulArgument)));
            break;

        case COpcode::O_INTOP:
            try
            {
                if (bLabel)
                {
                    strResult = m_Namespace[ulArgument];
                }
                else
                {
                    strResult.Format("%d", (long)ulArgument);
                }
            }

            catch(UserException& e)
            {
                printf("Warning: Restoration after exception. Object name replaced with '/* Fake object name */'\n");
                strResult = "/* Fake object name */";
            }
            break;

        case COpcode::O_POP_RETURN:
            strResult = "return ";
            strResult += GetSource(node.m_Arguments[0], FALSE, ulNumArgs);
            break;

        case COpcode::O_LOOKUP_STRING_PROC:
            strResult = GetSource(node.m_Arguments[0], FALSE, ulNumArgs);
            break;

        case COpcode::O_FETCH_GLOBAL:
            if (node.m_Arguments[0].m_Opcode.GetOperator() != COpcode::O_INTOP)
            {
                printf("Error: Invalid argument to O_FETCH_GLOBAL opcode\n");
                AfxThrowUserException();
            }

            if (INT_PTR(node.m_Arguments[0].m_Opcode.GetArgument()) > m_GlobalVarsNames.GetUpperBound())
            {
                printf("Error: Invalid index of global variable\n");
                AfxThrowUserException();
            }

            strResult = m_GlobalVarsNames[node.m_Arguments[0].m_Opcode.GetArgument()];
            break;

        case COpcode::O_STORE_GLOBAL:
            if (node.m_Arguments[1].m_Opcode.GetOperator() != COpcode::O_INTOP)
            {
                printf("Error: Invalid argument to O_STORE_GLOBAL opcode\n");
                AfxThrowUserException();
            }

            if (INT_PTR(node.m_Arguments[1].m_Opcode.GetArgument()) > m_GlobalVarsNames.GetUpperBound())
            {
                printf("Error: Invalid index of global variable\n");
                AfxThrowUserException();
            }

            strResult = m_GlobalVarsNames[node.m_Arguments[1].m_Opcode.GetArgument()];
            strResult += " := ";
            strResult += GetSource(node.m_Arguments[0], FALSE, ulNumArgs);
            break;

        case COpcode::O_FETCH_EXTERNAL:
            {
                WORD wOperator = node.m_Arguments[0].m_Opcode.GetOperator();

                if ((wOperator != COpcode::O_STRINGOP) && (wOperator != COpcode::O_INTOP))
                {
                    printf("Error: Invalid argument to O_FETCH_EXTERNAL opcode\n");
                    AfxThrowUserException();
                }
            }

            strResult = GetSource(node.m_Arguments[0], TRUE, ulNumArgs);
            break;

        case COpcode::O_STORE_EXTERNAL:
            {
                WORD wOperator = node.m_Arguments[1].m_Opcode.GetOperator();

                if ((wOperator != COpcode::O_STRINGOP) && (wOperator != COpcode::O_INTOP))
                {
                    printf("Error: Invalid argument to O_STORE_EXTERNAL opcode\n");
                    AfxThrowUserException();
                }
            }

            strResult = GetSource(node.m_Arguments[1], TRUE, ulNumArgs);
            strResult += " := ";
            strResult += GetSource(node.m_Arguments[0], FALSE, ulNumArgs);
            break;

        case COpcode::O_FETCH:
            if (node.m_Arguments[0].m_Opcode.GetOperator() != COpcode::O_INTOP)
            {
                printf("Error: Invalid argument to O_FETCH opcode\n");
                AfxThrowUserException();
            }

            {
                ULONG ulVarNum = node.m_Arguments[0].m_Opcode.GetArgument();

                if (ulVarNum < ulNumArgs)
                {
                    strResult.Format(c_strArgumentTemplate.c_str(), ulVarNum);
                }
                else
                {
                    strResult.Format(c_strLocalVarTemplate.c_str(), ulVarNum);
                }
            }

            break;

        case COpcode::O_STORE:
            if (node.m_Arguments[1].m_Opcode.GetOperator() != COpcode::O_INTOP)
            {
                printf("Error: Invalid argument to O_STORE opcode\n");
                AfxThrowUserException();
            }

            {
                ULONG ulVarNum = node.m_Arguments[1].m_Opcode.GetArgument();
                if (ulVarNum < ulNumArgs)
                {
                    strResult.Format(c_strArgumentTemplate.c_str(), ulVarNum);
                }
                else
                {
                    strResult.Format(c_strLocalVarTemplate.c_str(), ulVarNum);
                }
                strResult += " := ";
                strResult += GetSource(node.m_Arguments[0], FALSE, ulNumArgs);
            }

            break;

        case COpcode::O_POP:
            strResult = GetSource(node.m_Arguments[0], FALSE, ulNumArgs);

            if (node.m_Arguments[0].m_Opcode.GetOperator() == COpcode::O_CALL)
            {
                strResult = "call " + strResult.str();
            }

            break;

        case COpcode::O_CALL:
            if (node.m_Arguments[node.m_Arguments.GetUpperBound()].m_Opcode.GetOperator() == COpcode::O_INTOP)
            {
                strResult = m_Namespace[m_ProcTable[node.m_Arguments[node.m_Arguments.GetUpperBound()].m_Opcode.GetArgument()].m_ulNameOffset];
            }
            else
            {
                strResult = GetSource(node.m_Arguments[node.m_Arguments.GetUpperBound()], FALSE, ulNumArgs);
            }

            strResult += "(";

            for(INT_PTR nArgIndex = 0; nArgIndex < node.m_Arguments.GetUpperBound() - 1; nArgIndex++)
            {
                if (nArgIndex == 0)
                {
                    strResult += GetSource(node.m_Arguments[nArgIndex], FALSE, ulNumArgs);
                }
                else 
                {
                    strResult += ", ";
                    strResult +=  GetSource(node.m_Arguments[nArgIndex], FALSE, ulNumArgs);
                }
            }

            strResult += ")";

            break;

        case COpcode::O_CALL_AT:
            strResult = "call ";

            if (node.m_Arguments[1].m_Opcode.GetOperator() == COpcode::O_INTOP)
            {
                strResult += m_Namespace[m_ProcTable[node.m_Arguments[1].m_Opcode.GetArgument()].m_ulNameOffset];
            }
            else
            {
                strResult += GetSource(node.m_Arguments[1], FALSE, ulNumArgs);
            }

            strResult += " in (";
            strResult +=  GetSource(node.m_Arguments[0], FALSE, ulNumArgs) + ")";
            break;

        case COpcode::O_CALL_CONDITION:
            strResult = "call ";

            if (node.m_Arguments[1].m_Opcode.GetOperator() == COpcode::O_INTOP)
            {
                strResult += m_Namespace[m_ProcTable[node.m_Arguments[1].m_Opcode.GetArgument()].m_ulNameOffset];
            }
            else
            {
                strResult += GetSource(node.m_Arguments[1], FALSE, ulNumArgs);
            }

            strResult += " when (";
            strResult +=  GetSource(node.m_Arguments[0], FALSE, ulNumArgs) + ")";
            break;

        case COpcode::O_ADDREGION:
            strResult = "addRegion ";
            strResult += GetSource(node.m_Arguments[0], FALSE, ulNumArgs) + " { ";

            for(INT_PTR nArgIndex = 1; nArgIndex < node.m_Arguments.GetUpperBound(); nArgIndex++)
            {
                if (nArgIndex == 1)
                {
                    strResult += GetSource(node.m_Arguments[nArgIndex], FALSE, ulNumArgs);
                }
                else
                {
                    strResult += ", ";
                    strResult += GetSource(node.m_Arguments[nArgIndex], FALSE, ulNumArgs);
                }
            }

            strResult += " }";
            break;

        case COpcode::O_REFRESHMOUSE:
            {
                ULONG aulProcArg[1] = { 1 };
                strResult = GetSource(node, bLabel, ulNumArgs, aulProcArg, 1);
            }

            break;

        case COpcode::O_ADDBUTTONPROC:
            {
                ULONG aulProcArg[4] = { 2, 3, 4, 5 };
                strResult = GetSource(node, bLabel, ulNumArgs, aulProcArg, 4);
            }

            break;

        case COpcode::O_ADDBUTTONRIGHTPROC:
            {
                ULONG aulProcArg[2] = { 2, 3 };
                strResult = GetSource(node, bLabel, ulNumArgs, aulProcArg, 2);
            }

            break;

        case COpcode::O_SAYOPTION:
            {
                if (node.m_Arguments[1].m_Opcode.GetOperator() == COpcode::O_INTOP)
                {
                    ULONG aulProcArg[2] = { 2 };
                    strResult = GetSource(node, bLabel, ulNumArgs, aulProcArg, 1);
                }
                else
                {
                    strResult = "sayOption(";
                    strResult += GetSource(node.m_Arguments[0], FALSE, ulNumArgs) + ", " + GetSource(node.m_Arguments[1], FALSE, ulNumArgs) + ")";
                }
            }

            break;

        case COpcode::O_ADDREGIONPROC:
            {
                ULONG aulProcArg[4] = { 2, 3, 4, 5 };
                strResult = GetSource(node, bLabel, ulNumArgs, aulProcArg, 4);
            }

            break;

        case COpcode::O_ADDREGIONRIGHTPROC:
            {
                ULONG aulProcArg[2] = { 2, 3 };
                strResult = GetSource(node, bLabel, ulNumArgs, aulProcArg, 2);
            }

            break;

        case COpcode::O_ADDNAMEDEVENT:
            {
                ULONG aulProcArg[1] = { 2 };
                strResult = GetSource(node, bLabel, ulNumArgs, aulProcArg, 1);
            }

            break;

        case COpcode::O_ADDNAMEDHANDLER:
            {
                ULONG aulProcArg[1] = { 2 };
                strResult = GetSource(node, bLabel, ulNumArgs, aulProcArg, 1);
            }

            break;

        case COpcode::O_ADDKEY:
            {
                ULONG aulProcArg[1] = { 2 };
                strResult = GetSource(node, bLabel, ulNumArgs, aulProcArg, 1);
            }

            break;

        case COpcode::O_GSAY_OPTION:
            {
                ULONG aulProcArg[1] = { 3 };
                strResult = GetSource(node, bLabel, ulNumArgs, aulProcArg, 1);
            }

            break;

        case COpcode::O_GIQ_OPTION:
            {
                ULONG aulProcArg[1] = { 4 };
                strResult = GetSource(node, bLabel, ulNumArgs, aulProcArg, 1);
            }

            break;

        case COpcode::O_CANCEL:
            if (node.m_Arguments[0].m_Opcode.GetOperator() != COpcode::O_INTOP)
            {
                printf("Error: Invalid argument to O_CANCEL opcode\n");
                AfxThrowUserException();
            }

            strResult = "cancel(";
            strResult += m_Namespace[m_ProcTable[node.m_Arguments[0].m_Opcode.GetArgument()].m_ulNameOffset] + ")";
            break;

        case COpcode::O_NEGATE:
            if (node.m_Arguments[0].m_Opcode.GetAttributes().m_Category == COpcode::COpcodeAttributes::CATEGORY_INFIX)
            {
                strResult = "-(";
                strResult += GetSource(node.m_Arguments[0], bLabel, ulNumArgs) + ")";
            }
            else
            {
                strResult = "-";
                strResult += GetSource(node.m_Arguments[0], bLabel, ulNumArgs);
            }

            break;
            
        default:
            COpcode::COpcodeAttributes attributes = node.m_Opcode.GetAttributes();
            ULONG *procArgs = attributes.m_procArgs;
            ULONG numProcArgs = attributes.m_numProcArgs;
            if (numProcArgs > 0)
            {
                strResult = GetSource(node, bLabel, ulNumArgs, procArgs, numProcArgs);
                break;
            }
            if (node.m_Type == CNode::TYPE_CONDITIONAL_EXPRESSION)
            {
                if (node.m_Arguments.GetSize() != 3)
                {
                    printf("Error: Invalid number of arguments in conditional expression\n");
                    AfxThrowUserException();
                }
                CString sPostfix[] = { CString(" if "), CString(" else "), CString("")};
                strResult = "";
                for(INT_PTR i = 0; i < node.m_Arguments.GetSize(); i++)
                {
                    bool bParens = ArgNeedParens(node, node.m_Arguments[i], CFalloutScript::RIGHT_ASSOC);
                    strResult += (bParens ? CString("(") : CString("")) + GetSource(node.m_Arguments[i], bLabel, ulNumArgs) + (bParens ? ")" : "") + sPostfix[i];
                }
                break;
            }
            switch(attributes.m_Category)
            {
                case COpcode::COpcodeAttributes::CATEGORY_INFIX:
                    if (ArgNeedParens(node, node.m_Arguments[0], CFalloutScript::LEFT_ASSOC))
                    {
                        strResult = "(";
                        strResult += GetSource(node.m_Arguments[0], FALSE, ulNumArgs) + ")";
                    }
                    else
                    {
                        strResult = GetSource(node.m_Arguments[0], FALSE, ulNumArgs);
                    }

                    strResult += " ";
                    strResult += attributes.m_strName + " ";

                    
                    if (ArgNeedParens(node, node.m_Arguments[1], CFalloutScript::RIGHT_ASSOC))
                    {
                        strResult += "(";
                        strResult += GetSource(node.m_Arguments[1], FALSE, ulNumArgs) + ")";
                    }
                    else
                    {
                        strResult += GetSource(node.m_Arguments[1], FALSE, ulNumArgs);
                    }

                    break;

                case COpcode::COpcodeAttributes::CATEGORY_PREFIX:
                    strResult = attributes.m_strName;

                    if (node.m_Arguments.GetSize() != 0)
                    {
                        if (wOperator == COpcode::O_IF || wOperator == COpcode::O_WHILE)
                        {
                            strResult += " ";
                        }

                        strResult += "(";

                        for(INT_PTR i = 0; i < node.m_Arguments.GetSize(); i++)
                        {
                            if (i == 0)
                            {
                                strResult += GetSource(node.m_Arguments[i], FALSE, ulNumArgs);
                            }
                            else
                            {
                                strResult += ", ";
                                strResult += GetSource(node.m_Arguments[i], FALSE, ulNumArgs);
                            }
                        }

                        strResult += ")";
                    }

                    break;
            }
    }

    return strResult;
}

CString CFalloutScript::GetSource( CNode& node, BOOL bLabel, ULONG ulNumArgs, ULONG aulProcArg[], ULONG ulProcArgCount)
{
    COpcode::COpcodeAttributes attributes = node.m_Opcode.GetAttributes();
    CString strResult = attributes.m_strName + "(";
    CString strArgument;
    BOOL bIsProcArg;

    for(INT_PTR nArgIndex = 0; nArgIndex < node.m_Arguments.GetSize(); nArgIndex++)
    {
        bIsProcArg = FALSE;

        for(ULONG ulProcArgIndex = 0; ulProcArgIndex < ulProcArgCount; ulProcArgIndex++)
        {
            if (nArgIndex == aulProcArg[ulProcArgIndex] - 1)
            {
                bIsProcArg = TRUE;
                break;
            }
        }

        if (bIsProcArg)
        {
            if (node.m_Arguments[nArgIndex].m_Opcode.GetOperator() == COpcode::O_INTOP)
            {
                try
                {
                    strArgument = m_Namespace[m_ProcTable[node.m_Arguments[nArgIndex].m_Opcode.GetArgument()].m_ulNameOffset];
                }

                catch(UserException& e)
                {
                    printf("Warning: Restoration after exception. Object name replaced with '/* Fake object name */'\n");
                    strArgument.Format("/* Fake object name: %u (%d)*/", node.m_Arguments[nArgIndex].m_Opcode.GetArgument(), node.m_Arguments[nArgIndex].m_Opcode.GetArgument());
                }

            }
            else
            {
                strArgument = GetSource(node.m_Arguments[nArgIndex], FALSE, ulNumArgs);
            }
        }
        else
        {
            strArgument = GetSource(node.m_Arguments[nArgIndex], FALSE, ulNumArgs);
        }

        if (nArgIndex == 0)
        {
            strResult += strArgument;
        }
        else
        {
            strResult += ", ";
            strResult += strArgument;
        }
    }

    strResult += ")";

    return strResult;
}

CString CFalloutScript::GetIndentString(INT_PTR nLevel)
{
    CString strResult;
    
    for(; nLevel > 0; nLevel--)
    {
        strResult += g_strIndentFill;
    }

    return strResult;
}

int CFalloutScript::GetPriority(WORD wOperator)
{
    switch(wOperator)
    {
        case COpcode::O_IF:
            return 0;
        case COpcode::O_OR:
        case COpcode::O_AND:
            return 2;

        case COpcode::O_GREATER:
        case COpcode::O_LESS:
        case COpcode::O_GREATER_EQUAL:
        case COpcode::O_LESS_EQUAL:
        case COpcode::O_EQUAL:
        case COpcode::O_NOT_EQUAL:
            return 3;

        case COpcode::O_ADD:
        case COpcode::O_SUB:
        case COpcode::O_BWXOR:
        case COpcode::O_BWOR:
        case COpcode::O_BWAND:
            return 4;

        case COpcode::O_MUL:
        case COpcode::O_DIV:
        case COpcode::O_MOD:
        case COpcode::O_TS_POW:
            return 5;
        
        default:
            return 6;
    }
}

CFalloutScript::Assoc CFalloutScript::GetAssociation(WORD wOperator)
{
    switch(wOperator)
    {
        case COpcode::O_GREATER:
        case COpcode::O_LESS:
        case COpcode::O_GREATER_EQUAL:
        case COpcode::O_LESS_EQUAL:
        case COpcode::O_EQUAL:
        case COpcode::O_NOT_EQUAL:
            return CFalloutScript::NON_ASSOC;

        case COpcode::O_OR:
        case COpcode::O_AND:
        case COpcode::O_ADD:
        case COpcode::O_SUB:
        case COpcode::O_BWXOR:
        case COpcode::O_BWOR:
        case COpcode::O_BWAND:
        case COpcode::O_MUL:
        case COpcode::O_DIV:
        case COpcode::O_MOD:
            return CFalloutScript::LEFT_ASSOC;
        
        default:
            return CFalloutScript::NON_ASSOC;
    }
}

