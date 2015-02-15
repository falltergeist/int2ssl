#include "stdafx.h"
#include "FalloutScript.h"
#include "ObjectAttributes.h"
#include "Utility.h"

#include <iostream>
#include <algorithm>

// Globals
extern BOOL g_bIgnoreWrongNumOfArgs;
extern BOOL g_bInsOmittedArgsBackward;

void CFalloutScript::InitDefinitions()
{
    ULONG ulNameOffset;
    INT_PTR nObjectIndex;
    std::string c_strGlobalVarTemplate("GVar%u");


    m_Definitions.RemoveAll();

    for(INT_PTR i = 0; i < m_Namespace.GetSize(); i++)
    {
        ulNameOffset = m_Namespace.GetOffsetByIndex(i);
        
        if ((nObjectIndex = GetIndexOfProc(ulNameOffset)) != -1)
        {
            m_Definitions.SetAt(ulNameOffset, CDefObject(CDefObject::OBJECT_PROCEDURE, 0, ULONG(nObjectIndex)));
        }
        else if ((nObjectIndex = GetIndexOfExportedVariable(ulNameOffset)) != -1)
        {
            WORD wOpcode = m_ExportedVarValue[nObjectIndex].GetOperator();
            ULONG ulValue = m_ExportedVarValue[nObjectIndex].GetArgument();

            m_Definitions.SetAt(ulNameOffset, CDefObject(CDefObject::OBJECT_VARIABLE, V_EXPORT | wOpcode, ulValue));
        }
    }

    m_GlobalVarsNames.resize(m_GlobalVar.GetSize());

    for(INT_PTR i = 0; i < m_GlobalVarsNames.size(); i++)
    {
        m_GlobalVarsNames[i] = format(c_strGlobalVarTemplate, i);
    }
}

void CFalloutScript::ProcessCode()
{
    printf("    Intial reducing\n");
    InitialReduce();

    printf("    Building execution tree\n");
    
    for(INT_PTR i = 0; i < m_ProcTable.GetSize(); i++)
    {
        printf("        Procedure: %d\n", i);
        BuildTree(m_ProcBodies[i]);
    }

    printf("    Extracting and reducing conditions\n");
    
    for(INT_PTR i = 0; i < m_ProcTable.GetSize(); i++)
    {
        if (m_ProcTable[i].m_ulType & P_CONDITIONAL)
        {
            ExtractAndReduceCondition(m_ProcBodies[i], m_Conditions[i], 0);
        }

        for(INT_PTR j = 0; j < m_ProcBodies[i].GetSize(); j++)
        {
            if (m_ProcBodies[i][j].m_Opcode.GetOperator() == COpcode::O_CALL_CONDITION)
            {
                CNodeArray Condition;
                CNode node = m_ProcBodies[i][j].m_Arguments[0];

                if (node.m_Opcode.GetOperator() != COpcode::O_INTOP)
                {
                    printf("Error: Invalid opcode for start address of condition\n");
                    AfxThrowUserException();
                }

                ULONG ulCondStartAddress = node.m_Opcode.GetArgument();

                do
                {
                    node = m_ProcBodies[i][j = NextNodeIndex(m_ProcBodies[i], j, -1)];
                } 
                while(node.m_ulOffset != ulCondStartAddress);

                j = NextNodeIndex(m_ProcBodies[i], j, -1);   // For O_JMP opcode

                ExtractAndReduceCondition(m_ProcBodies[i], Condition, j);
                m_ProcBodies[i][j].m_Arguments[0] = Condition[0];
            }
        }
    }


    printf("    Setting borders of blocks\n");

    for(INT_PTR i = 0; i < m_ProcTable.GetSize(); i++)
    {
        printf("        Procedure: %d\r", i);
        SetBordersOfBlocks(m_ProcBodies[i]);
    }

    printf("    Renaming global variables\n");
    TryRenameGlobalVariables();

    printf("    Renaming imported variables\n");
    TryRenameImportedVariables();
}

INT_PTR CFalloutScript::GetIndexOfProc(const char* lpszName)
{
    INT_PTR nResult = -1;
    std::string strName(lpszName);
    std::string strTestName;

    std::transform(strName.begin(), strName.end(), strName.begin(), ::tolower);

    for(INT_PTR i = 0; i < m_ProcTable.GetSize(); i++)
    {
        strTestName = m_Namespace[m_ProcTable[i].m_ulNameOffset];
        std::transform(strTestName.begin(), strTestName.end(), strTestName.begin(), ::tolower);

        if (strTestName == strName)
        {
            nResult = i;
            break;
        }
    }

    return nResult;
}

INT_PTR CFalloutScript::GetIndexOfProc(ULONG ulNameOffset)
{
    INT_PTR nResult = -1;

    for(INT_PTR i = 0; i < m_ProcTable.GetSize(); i++)
    {
        if (m_ProcTable[i].m_ulNameOffset == ulNameOffset)
        {
            nResult = i;
            break;
        }
    }

    return nResult;
}

INT_PTR CFalloutScript::GetIndexOfExportedVariable(ULONG ulNameOffset)
{
    INT_PTR nResult = -1;

    for(INT_PTR i = 0; i < m_ExportedVarValue.GetSize(); i += 2)
    {
        if (m_ExportedVarValue[i + 1].GetArgument() == ulNameOffset)
        {
            nResult = i;
            break;
        }
    }

    return nResult;
}

void CFalloutScript::SetExternalVariable(ULONG ulNameOffset)
{
    CDefObject defObject;

    if (!m_Definitions.Lookup(ulNameOffset, defObject))
    {
        m_Definitions.SetAt(ulNameOffset, CDefObject(CDefObject::OBJECT_VARIABLE, V_IMPORT));
    }
}

void CFalloutScript::TryRenameGlobalVariables()
{
    INT_PTR nNamesCount = m_Namespace.GetSize();
    INT_PTR nDefinitionsCount = m_Definitions.GetSize();
    INT_PTR nGlobalVarCount = m_GlobalVar.GetSize();

    CDefObject defObject;
    INT_PTR nGlobalVarIndex = 0;

    if (nNamesCount - nDefinitionsCount == nGlobalVarCount)
    {
        for(INT_PTR i = 0; i < m_Namespace.GetSize(); i++)
        {
            if (!m_Definitions.Lookup(m_Namespace.GetOffsetByIndex(i), defObject))
            {
                defObject.m_ObjectType = CDefObject::OBJECT_VARIABLE;
                defObject.m_ulAttributes = V_GLOBAL | m_GlobalVar[nGlobalVarIndex].GetOperator();
                defObject.m_ulVarValue = m_GlobalVar[nGlobalVarIndex].GetArgument();

                m_Definitions.SetAt(m_Namespace.GetOffsetByIndex(i), defObject);
                m_GlobalVarsNames[nGlobalVarIndex] = m_Namespace.GetStringByIndex(i);
                nGlobalVarIndex++;
            }
        }
    }
}

void CFalloutScript::TryRenameImportedVariables()
{
    CDefObject defObject;
    ULONG ulNameOffset;

    if (m_GlobalVar.GetSize() == 0)
    {
        for(INT_PTR i = 0; i < m_Namespace.GetSize(); i++)
        {
            ulNameOffset = m_Namespace.GetOffsetByIndex(i);

            if (!m_Definitions.Lookup(ulNameOffset, defObject))
            {
                m_Definitions.SetAt(ulNameOffset, CDefObject(CDefObject::OBJECT_VARIABLE, V_IMPORT));
            }
        }
    }
}

INT_PTR CFalloutScript::NextNodeIndex( CNodeArray& NodeArray, INT_PTR nCurrentIndex, INT_PTR nStep)
{
    INT_PTR nResult = nCurrentIndex + nStep;

    if ((nResult < 0) || (nResult > NodeArray.GetUpperBound()))
    {
        printf("Error: Index of node out of range\n");
        AfxThrowUserException();
    }

    return nResult;
}

BOOL CFalloutScript::CheckSequenceOfNodes(CNodeArray& NodeArray, INT_PTR nStartIndex, const WORD wSequence[], INT_PTR nSequenceLen)
{
    return RemoveSequenceOfNodes(NodeArray, nStartIndex, 0, wSequence, nSequenceLen);
}

BOOL CFalloutScript::RemoveSequenceOfNodes(CNodeArray& NodeArray, INT_PTR nStartIndex, INT_PTR nCount, const WORD wSequence[], INT_PTR nSequenceLen)
{
    INT_PTR nCurrentNodeIndex = nStartIndex - 1;

    for(INT_PTR i = 0; i < nSequenceLen; i++)
    {
        nCurrentNodeIndex = NextNodeIndex(NodeArray, nCurrentNodeIndex, 1);

        if (NodeArray[nCurrentNodeIndex].m_Opcode.GetOperator() != wSequence[nCurrentNodeIndex - nStartIndex])
        {
            return FALSE;
        }
    }

    NodeArray.RemoveAt(nStartIndex, nCount);

    return TRUE;
}

void CFalloutScript::InitialReduce()
{
    static WORD awTailOfProc[3] = {
        COpcode::O_POP_TO_BASE,
        COpcode::O_POP_BASE,
        COpcode::O_POP_RETURN
    };

    static WORD awTailOfCriticalProc[4] = {
        COpcode::O_POP_TO_BASE,
        COpcode::O_POP_BASE,
        COpcode::O_CRITICAL_DONE,
        COpcode::O_POP_RETURN
    };

    static WORD awCheckArgCount[3] = {
        COpcode::O_DUP,
        COpcode::O_INTOP,
        COpcode::O_CHECK_ARG_COUNT
    };

    static WORD awShortCircuitAnd[5] = {
        COpcode::O_DUP,
        COpcode::O_INTOP,
        COpcode::O_SWAP,
        COpcode::O_IF,
        COpcode::O_POP
    };

    static WORD awShortCircuitOr[6] = {
        COpcode::O_DUP,
        COpcode::O_INTOP,
        COpcode::O_SWAP,
        COpcode::O_NOT,
        COpcode::O_IF,
        COpcode::O_POP
    };

    static WORD awStoreReturnAdress[2] = {
        COpcode::O_INTOP,
        COpcode::O_D_TO_A
    };

    static WORD awReturn[6] = {
        COpcode::O_D_TO_A,
        COpcode::O_SWAPA,
        COpcode::O_POP_TO_BASE,
        COpcode::O_POP_BASE,
        COpcode::O_A_TO_D,
        COpcode::O_POP_RETURN,
    };

    static WORD awCriticalReturn[7] = {
        COpcode::O_D_TO_A,
        COpcode::O_SWAPA,
        COpcode::O_POP_TO_BASE,
        COpcode::O_POP_BASE,
        COpcode::O_A_TO_D,
        COpcode::O_CRITICAL_DONE,
        COpcode::O_POP_RETURN,
    };

    WORD* pwCode;
    INT_PTR nCount;

    for(INT_PTR i = 0 ; i < m_ProcBodies.GetSize(); i++)
    {
        // Tail
        if (!m_ProcBodies[i].IsEmpty())
        {
            pwCode = (m_ProcTable[i].m_ulType & P_CRITICAL) ? awTailOfCriticalProc : awTailOfProc;
            nCount = (m_ProcTable[i].m_ulType & P_CRITICAL) ? 4 : 3;

            if (!RemoveSequenceOfNodes(m_ProcBodies[i], m_ProcBodies[i].GetSize() - nCount, nCount, pwCode, nCount))
            {
                printf("Error: Invalid tail of procedure\'s body\n");
                AfxThrowUserException();
            }
        }

        // Body
        for(INT_PTR j = 0; j < m_ProcBodies[i].GetSize(); j++)
        {
            switch(m_ProcBodies[i][j].m_Opcode.GetOperator())
            {
                case COpcode::O_DUP:
                    // 'Check procedure's arguments count' statement
                    if (!RemoveSequenceOfNodes(m_ProcBodies[i], j, 3, awCheckArgCount, 3))
                    {
                        // short circuit AND
                        UINT actualOperator = CheckSequenceOfNodes(m_ProcBodies[i], j, awShortCircuitAnd, 5)
                                        ? COpcode::O_AND 
                                        : (CheckSequenceOfNodes(m_ProcBodies[i], j, awShortCircuitOr, 6)
                                            ? COpcode::O_OR
                                            : 0);
                        if (actualOperator)
                        {
                            UINT k, skipOffset = m_ProcBodies[i][j+1].m_Opcode.GetArgument();
                            CNode node;
                            k = j - 1;
                            do
                            {
                                k = NextNodeIndex(m_ProcBodies[i], k, 1);
                            }
                            while (skipOffset > m_ProcBodies[i][k].m_ulOffset);

                            m_ProcBodies[i].InsertAt(k, m_ProcBodies[i][j]);
                            m_ProcBodies[i][k].m_Opcode.SetOperator(actualOperator); // place AND/OR here, so BuildTree() will treat it as a regular binary operator
                            m_ProcBodies[i][k].m_ulOffset = m_ProcBodies[i][k-1].m_ulOffset + COpcode::OPERATOR_SIZE; // adjust offset
                            m_ProcBodies[i].RemoveAt(j, (actualOperator == COpcode::O_AND) ? 5 : 6); // reduce
                        }
                        else
                        {
                            printf("Error: Unknown sequence of opcodes\n");
                            AfxThrowUserException();
                        }
                    }

                    j--;
                    break;

                case COpcode::O_D_TO_A:
                    // 'return' and 'store return address' statements
                    pwCode = (m_ProcTable[i].m_ulType & P_CRITICAL) ? awCriticalReturn : awReturn;
                    nCount = (m_ProcTable[i].m_ulType & P_CRITICAL) ? 7 : 6;

                    if (!RemoveSequenceOfNodes(m_ProcBodies[i], j, nCount - 1, pwCode, nCount))
                    {
                        if (!RemoveSequenceOfNodes(m_ProcBodies[i], j - 1, 2, awStoreReturnAdress, 2))
                        {
                            printf("Error: Unknown sequence of opcodes\n");
                            AfxThrowUserException();
                        }
                    }

                    j--;
                    break;
            }
        }
    }
}

// build tree for all nodes from nStartIndex to file offset ulEndOffset (not including)
ULONG CFalloutScript::BuildTreeBranch(CNodeArray& NodeArray, ULONG nStartIndex, ULONG ulEndOffset)
{
    WORD wOperator;
    ULONG ulArgument;
    INT_PTR nNumOfArgs;

    COpcode::COpcodeAttributes opcodeAttributes;
    INT_PTR j;
    for (j = nStartIndex; (j < NodeArray.GetSize() && NodeArray[j].m_ulOffset < ulEndOffset); j++)
    {
        wOperator = NodeArray[j].m_Opcode.GetOperator();
        ulArgument = NodeArray[j].m_Opcode.GetArgument();

        opcodeAttributes = NodeArray[j].m_Opcode.GetAttributes();
        nNumOfArgs = INT_PTR(opcodeAttributes.m_ulNumArgs);

        switch(wOperator)
        {
            case COpcode::O_FETCH_EXTERNAL:
            case COpcode::O_STORE_EXTERNAL:
            {
                INT_PTR nExtVarNameNodeIndex = NextNodeIndex(NodeArray, j, -1);
                WORD wOpeartor = NodeArray[nExtVarNameNodeIndex].m_Opcode.GetOperator();
                ULONG ulArgument = NodeArray[nExtVarNameNodeIndex].m_Opcode.GetArgument();

                if ((wOpeartor != COpcode::O_STRINGOP) && (wOpeartor != COpcode::O_INTOP))
                {
                    printf("Error: Invalid reference to external variable\n");
                    AfxThrowUserException();
                }

                SetExternalVariable(ulArgument);
                break;
            }

            case COpcode::O_CALL:
            {
                INT_PTR nProcNumOfArgsNodeIndex = NextNodeIndex(NodeArray, j, -2);
                WORD wProcNumOfArgsOperator = NodeArray[nProcNumOfArgsNodeIndex].m_Opcode.GetOperator();
                ULONG ulProcNumOfArgs = NodeArray[nProcNumOfArgsNodeIndex].m_Opcode.GetArgument();

                if (wProcNumOfArgsOperator != COpcode::O_INTOP)
                {
                    printf("Error: Invalid opcode for procedure\'s number of arguments\n");
                    AfxThrowUserException();
                }

                nNumOfArgs = INT_PTR(ulProcNumOfArgs) + 2;
                break;
            }

            case COpcode::O_ADDREGION:
            {
                INT_PTR nAddRegionNumOfArgsNodeIndex = NextNodeIndex(NodeArray, j, -1);
                WORD wAddRegionNumOfArgsOperator = NodeArray[nAddRegionNumOfArgsNodeIndex].m_Opcode.GetOperator();
                ULONG ulAddRegionNumOfArgs = NodeArray[nAddRegionNumOfArgsNodeIndex].m_Opcode.GetArgument();

                if (wAddRegionNumOfArgsOperator != COpcode::O_INTOP)
                {
                    printf("Error: Invalid opcode for addRegion number of arguments\n");
                    AfxThrowUserException();
                }

                nNumOfArgs = INT_PTR(ulAddRegionNumOfArgs) + 1;
                break;
            }
        }

        // Check nodes
        INT_PTR nOmittedArgStartIndex = nNumOfArgs;
        COpcode::COpcodeAttributes checkOpcodeAttributes;
        INT_PTR nNodeIndex = j;

        for(INT_PTR k = 0; k < nNumOfArgs; k++)
        {
            nNodeIndex = NextNodeIndex(NodeArray, nNodeIndex, -1);
            if (!NodeArray[nNodeIndex].IsExpression())
            {
                if (g_bIgnoreWrongNumOfArgs)
                {
                    if (IsOmittetArgsAllowed(wOperator))
                    {
                        printf("Warning: Omitted expression found\n");
                        nOmittedArgStartIndex = k;
                        break;
                    }
                    else
                    {
                        printf("Error: Not enough arguments for %X\n", NodeArray[j].m_ulOffset);
                        AfxThrowUserException();
                    }
                }
                else
                {
                    printf("Error: Expression required for %X\n", NodeArray[j].m_ulOffset);
                    AfxThrowUserException();
                }
            }
        }

        // Move arguments
        for(INT_PTR k = 0; k < nNumOfArgs; k++)
        {
            if (k < nOmittedArgStartIndex)
            {
                NodeArray[j].m_Arguments.InsertAt(0, NodeArray[j-1]);
                NodeArray.RemoveAt(j - 1);
                j--;
            }
            else
            {
                if (g_bInsOmittedArgsBackward)
                {
                    NodeArray[j].m_Arguments.Add(CNode(CNode::TYPE_OMITTED_ARGUMENT));
                }
                else
                {
                    NodeArray[j].m_Arguments.InsertAt(0, CNode(CNode::TYPE_OMITTED_ARGUMENT));
                }
            }
        }

        if (wOperator == COpcode::O_IF)
        {
            // process possible conditional expression - this may be either normal IF statement or (x IF y ELSE z) expression
            ULONG ulElseOffset = NodeArray[j].m_Arguments[0].m_Opcode.GetArgument();
            ULONG ulElseIndex, ulSkipIndex = -1;
            ulElseIndex = BuildTreeBranch(NodeArray, j + 1, ulElseOffset); // true branch
            if (NodeArray[ulElseIndex - 1].m_Opcode.GetOperator() == COpcode::O_JMP)
            {
                ULONG ulSkipOffset = NodeArray[ulElseIndex - 1].m_Opcode.GetArgument();
                if (ulSkipOffset > NodeArray[j].m_ulOffset)
                {
                    ulSkipIndex = BuildTreeBranch(NodeArray, ulElseIndex, ulSkipOffset); // false branch
                    if (NodeArray[ulElseIndex - 2].IsExpression() && NodeArray[ulSkipIndex - 1].IsExpression())
                    { // conditional expression
                        NodeArray[j].m_Type = CNode::TYPE_CONDITIONAL_EXPRESSION;
                        NodeArray[j].m_Arguments.RemoveAt(0); // address not needed anymore
                        NodeArray[j].m_Arguments.InsertAt(0, NodeArray[ulElseIndex - 2]); // true expression
                        NodeArray[j].m_Arguments.InsertAt(2, NodeArray[ulSkipIndex - 1]); // false expression
                        NodeArray.RemoveAt(j + 1, ulSkipIndex - j - 1);
                        continue;
                    }
                }
            }
            j = ((ulSkipIndex != -1) ? ulSkipIndex : ulElseIndex) - 1; // skip already built
        }
    }

    return j;
}

void CFalloutScript::BuildTree(CNodeArray& NodeArray)
{
    if (NodeArray.GetSize() > 0)
    {
        BuildTreeBranch(NodeArray, 0, NodeArray[NodeArray.GetSize() - 1].m_ulOffset + COpcode::OPERATOR_SIZE);
    }
}

void CFalloutScript::ExtractAndReduceCondition(CNodeArray& Source, CNodeArray& Destination, INT_PTR nStartIndex)
{
    // Extract
    CNode node;
    INT_PTR nNodeIndex;;

    node = Source[nNodeIndex = NextNodeIndex(Source, nStartIndex - 1, 1)];

    if (node.m_Opcode.GetOperator() != COpcode::O_JMP)
    {
        printf("Error: Invalid startup of condition\n");
        AfxThrowUserException();
    }

    CNode nodeJumpAddress = node.m_Arguments[0];

    if (nodeJumpAddress.m_Opcode.GetOperator() != COpcode::O_INTOP)
    {
        printf("Error: Invalid startup of condition\n");
        AfxThrowUserException();
    }

    ULONG ulJumpOffset = node.m_Arguments[0].m_Opcode.GetArgument();

    do
    {
        node = Source[nNodeIndex = NextNodeIndex(Source, nNodeIndex, 1)];
    }
    while(node.m_ulOffset < ulJumpOffset);

    Destination.SetSize(nNodeIndex - nStartIndex);

    for(INT_PTR j = 0; j < nNodeIndex - nStartIndex; j++)
    {
        Destination[j] = Source[nStartIndex + j];
    }

    // Reduce
    static WORD awStartupOfCondition[2] = {
        COpcode::O_JMP,
        COpcode::O_CRITICAL_START
    };

    static WORD awCleanupOfCondition[2] = {
        COpcode::O_CRITICAL_DONE,
        COpcode::O_STOP_PROG
    };

    // Startup
    if (!RemoveSequenceOfNodes(Destination, 0, 2, awStartupOfCondition, 2))
    {
        printf("Error: Invalid startup of condition\n");
        AfxThrowUserException();
    }

    // Cleanup
    if (!RemoveSequenceOfNodes(Destination, Destination.GetSize() - 2, 2, awCleanupOfCondition, 2))
    {
        printf("Error: Invalid cleanup of condition\n");
        AfxThrowUserException();
    }

    // Check condition
    if (Destination.GetSize() != 1)
    {
        printf("Error: Invalid condition. Only one expression allowed\n");
        AfxThrowUserException();
    }
    else
    {
        if (Destination[0].m_Opcode.GetAttributes().m_Type != COpcode::COpcodeAttributes::TYPE_EXPRESSION)
        {
            printf("Error: Invalid condition. Expression required\n");
            AfxThrowUserException();
        }
    }

    // Remove from source
    Source.RemoveAt(nStartIndex, nNodeIndex - nStartIndex);
}

void CFalloutScript::SetBordersOfBlocks(CNodeArray& NodeArray)
{
    if (NodeArray.IsEmpty())
    {
        return;
    }

    ULONG ulOffset;

    // Start of procedure
    if (NodeArray[0].m_Opcode.GetOperator() == COpcode::O_PUSH_BASE)
    {
        ulOffset = NodeArray[0].m_ulOffset;
        NodeArray[0] = c_NodeBeginOfBlock;
        NodeArray[0].m_ulOffset = ulOffset;
    }

    // End of procedure
    INT_PTR nLastNodeIndex = NodeArray.GetUpperBound();

    if ((NodeArray[nLastNodeIndex].m_Opcode.GetOperator() == COpcode::O_POP_RETURN) &&
        (NodeArray[nLastNodeIndex].m_Opcode.GetArgument() == 0) &&
        (NodeArray[nLastNodeIndex].m_Arguments[0].m_Opcode.GetOperator() == COpcode::O_INTOP))
    {
        ulOffset = NodeArray[nLastNodeIndex].m_ulOffset;
        NodeArray[nLastNodeIndex] = c_NodeEndOfBlock;
        NodeArray[nLastNodeIndex].m_ulOffset = ulOffset;
    }
    else
    {
        ulOffset = NodeArray[nLastNodeIndex].m_ulOffset;
        NodeArray.InsertAt(nLastNodeIndex + 1, CNode(c_NodeEndOfBlock));
        NodeArray[nLastNodeIndex].m_ulOffset = ulOffset;
    }
    
    // Body
    CNode node;

    for(INT_PTR i = 0; i < NodeArray.GetSize(); i++)
    {
        switch(NodeArray[i].m_Opcode.GetOperator())
        {
            case COpcode::O_WHILE:
            {
                CNode node = NodeArray[i].m_Arguments[0];
                ULONG loopOffset = NodeArray[i].m_Arguments[1].GetTopOffset();

                if (node.m_Opcode.GetOperator() != COpcode::O_INTOP)
                {
                    printf("Error: Invalid opcode for jump-address\n");
                    AfxThrowUserException();
                }

                NodeArray.InsertAt(i + 1, CNode(c_NodeBeginOfBlock));
                NodeArray[i + 1].m_ulOffset = NodeArray[i].m_ulOffset;
                ulOffset = node.m_Opcode.GetArgument();

                INT_PTR nNodeIndex = i + 1;
                CArray <INT_PTR, INT_PTR&> jumps;
                    
                do
                {
                    node = NodeArray[nNodeIndex = NextNodeIndex(NodeArray, nNodeIndex, 1)];
                    if (node.m_Opcode.GetOperator() == COpcode::O_JMP && node.m_Type == CNode::TYPE_NORMAL && node.m_Arguments.GetSize() > 0)
                    {
                        ULONG ofs = node.m_Arguments[0].m_Opcode.GetArgument();
                        if (ofs == ulOffset)
                        {
                            NodeArray[nNodeIndex].m_Type = CNode::TYPE_BREAK;
                        }
                        else if (ofs == loopOffset)
                        {
                            NodeArray[nNodeIndex].m_Type = CNode::TYPE_CONTINUE; // continue in "while" loop
                        }
                        else
                        {
                            jumps.Add(nNodeIndex);
                        }
                    }
                }
                while(node.m_ulOffset < ulOffset);

                bool isForLoop = false;
                if (NodeArray[nNodeIndex - 2].m_Arguments.GetSize() > 0 && i > 0)
                { // *might* be a "for" loop
                    loopOffset = NodeArray[nNodeIndex - 2].GetTopOffset();
                    for (INT_PTR j=0; j<jumps.GetSize(); j++)
                    {
                        if (NodeArray[jumps[j]].m_Arguments[0].m_Opcode.GetArgument() == loopOffset)
                        { // jump points to the last statement in loop
                            NodeArray[jumps[j]].m_Type = CNode::TYPE_CONTINUE; // it's a continue
                            isForLoop = true; // in a "for" loop
                        }
                    }
                }

                NodeArray.InsertAt(nNodeIndex, CNode(c_NodeEndOfBlock));
                NodeArray[nNodeIndex].m_ulOffset = NodeArray[nNodeIndex + 1].m_ulOffset;

                node = NodeArray[nNodeIndex - 1];

                if (node.m_Opcode.GetOperator() != COpcode::O_JMP)
                {
                    printf("Error: Invalid tail of \'while\' statement\n");
                    AfxThrowUserException();
                }

                if (node.m_Arguments[0].m_Opcode.GetOperator() != COpcode::O_INTOP)
                {
                    printf("Error: Invalid opcode for jump-address\n");
                    AfxThrowUserException();
                }

                if (isForLoop)
                {
                    NodeArray[i].m_Type = CNode::TYPE_FOR_LOOP;
                    NodeArray[i].m_Arguments[0] = NodeArray[i - 1]; // "for" initializer
                    NodeArray[i].m_Arguments.Add(NodeArray[nNodeIndex - 2]); // "for" increment
                    NodeArray.RemoveAt(i - 1); // eat statement before "while"
                    NodeArray.RemoveAt(nNodeIndex - 3, 2); // eat last statement in loop along with jmp
                }
                else
                {
                    NodeArray.RemoveAt(nNodeIndex - 1);
                    NodeArray[i].m_Arguments.RemoveAt(0);
                }
            }
            break;

            case COpcode::O_IF:
            {
                if (NodeArray[i].m_Type == CNode::TYPE_CONDITIONAL_EXPRESSION)
                {
                    printf("Error: Conditional expression left in stack\n");
                    AfxThrowUserException();
                }
                CNode node = NodeArray[i].m_Arguments[0];

                if (node.m_Opcode.GetOperator() != COpcode::O_INTOP)
                {
                    printf("Error: Invalid opcode for jump-address\n");
                    AfxThrowUserException();
                }

                NodeArray.InsertAt(i + 1, CNode(c_NodeBeginOfBlock));
                NodeArray[i + 1].m_ulOffset = NodeArray[i + 2].m_ulOffset;
                ulOffset = node.m_Opcode.GetArgument(); // offset for jump
                    
                INT_PTR nNodeIndex = i + 1;

                do
                {
                    node = NodeArray[nNodeIndex = NextNodeIndex(NodeArray, nNodeIndex, 1)];
                }
                while(node.m_ulOffset < ulOffset);

                NodeArray.InsertAt(nNodeIndex, CNode(c_NodeEndOfBlock));
                NodeArray[nNodeIndex].m_ulOffset = NodeArray[nNodeIndex + 1].m_ulOffset;

                node = NodeArray[nNodeIndex - 1];

                if (node.m_Opcode.GetOperator() == COpcode::O_JMP && node.m_Type != CNode::TYPE_BREAK && node.m_Type != CNode::TYPE_CONTINUE)
                { // else block
                    if (node.m_Arguments[0].m_Opcode.GetOperator() != COpcode::O_INTOP)
                    {
                        printf("Error: Invalid opcode for jump-address\n");
                        AfxThrowUserException();
                    }
                    //ULONG jumpPastElseOffset = 
                    ulOffset = node.m_Arguments[0].m_Opcode.GetArgument();
                    //printf("(else) goto %x > %x", ulOffset, node.m_ulOffset);
                    if (ulOffset > node.m_ulOffset)
                    {
                        NodeArray.RemoveAt(nNodeIndex - 1); // remove jump
                        NodeArray.InsertAt(nNodeIndex, CNode(c_NodeBeginOfBlock));
                        NodeArray[nNodeIndex].m_ulOffset = NodeArray[nNodeIndex + 1].m_ulOffset;

                        CNode Bnode = NodeArray[nNodeIndex - 1];
                        CNode Cnode = NodeArray[nNodeIndex + 1];
                        if ((Bnode.m_Type == CNode::TYPE_END_OF_BLOCK)/* &&
                                (Cnode.m_Opcode.GetOperator() != COpcode::O_IF)*/)
                        {
                            do
                            {
                                node = NodeArray[nNodeIndex = NextNodeIndex(NodeArray, nNodeIndex, 1)];
                            }
                            while(node.m_ulOffset < ulOffset);
                            NodeArray.InsertAt(nNodeIndex, CNode(c_NodeEndOfBlock));
                            NodeArray[nNodeIndex].m_ulOffset = NodeArray[nNodeIndex + 1].m_ulOffset;
                        }
                    }
                }
            }
            NodeArray[i].m_Arguments.RemoveAt(0);
            i++;

            break;
        }
    }
}

void CFalloutScript::ReduceConditionalExpressions(CNodeArray& NodeArray)
{
    /*
    if (NodeArray.IsEmpty())
    {
        return;
    }
    CNode node;

    for (INT_PTR i = 0; i < NodeArray.GetSize(); i++)
    {
        if (NodeArray[i].m_Opcode.GetOperator() == COpcode::O_IF)
        {
            if (ReduceExpressionBlock(NodeArray, i+1))
            {

            }
        }
    }
    */
}

BOOL CFalloutScript::IsOmittetArgsAllowed(WORD wOpcode)
{
    if (((wOpcode >= COpcode::O_END_CORE) && (wOpcode < COpcode::O_END_OP)) ||
        (wOpcode == COpcode::O_POP_RETURN))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
