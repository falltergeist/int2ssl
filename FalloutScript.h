#ifndef FALLOUT_SCRIPT_H
#define FALLOUT_SCRIPT_H

// C++ standard includes
#include <vector>

// int2ssl includes
#include "StartupCode.h"
#include "ProcTable.h"
#include "Namespace.h"
#include "Node.h"

// Third party includes

class CFalloutScript
{
public:
    CFalloutScript();
    virtual ~CFalloutScript();

public:
    virtual void Serialize(CArchive& ar);

    void Dump(CArchive& ar);

    void InitDefinitions();
    void ProcessCode();

    void StoreSource(CArchive& ar);
    void StoreTree(CArchive& ar);

private:
    enum Assoc {
        NON_ASSOC,
        LEFT_ASSOC,
        RIGHT_ASSOC,
    };

private:
    void ExtractCodeElements(COpcodeArray& Source, COpcodeArray& Destination, uint16_t wDelimeter, int nSizeOfCodeItem, const char* lpszErrorMessage, bool (CFalloutScript::*pCheckFunc)(uint16_t, INT_PTR));
    bool CheckExportVarCode(uint16_t wOperator, INT_PTR nIndex);
    bool CheckSetExportedVarValueCode(uint16_t wOperator, INT_PTR nIndex);
    bool CheckExportProcCode(uint16_t wOperator, INT_PTR nIndex);

    INT_PTR GetIndexOfProc(const char* lpszName);
    INT_PTR GetIndexOfProc(uint32_t ulNameOffset);
    INT_PTR GetIndexOfExportedVariable(uint32_t ulNameOffset);

    void SetExternalVariable(uint32_t ulNameOffset);
    void TryRenameGlobalVariables();
    void TryRenameImportedVariables();

    INT_PTR NextNodeIndex( CNodeArray& NodeArray, INT_PTR nCurrentIndex, INT_PTR nSteep);
    bool CheckSequenceOfNodes(CNodeArray& NodeArray, INT_PTR nStartIndex, const uint16_t wSequence[], INT_PTR nSequenceLen);
    bool RemoveSequenceOfNodes(CNodeArray& NodeArray,INT_PTR nStartIndex, INT_PTR nCount, const uint16_t wSequence[], INT_PTR nSequenceLen);

    void InitialReduce();
    void BuildTree(CNodeArray& NodeArray);
    void ExtractAndReduceCondition(CNodeArray& Source, CNodeArray& Destination, INT_PTR nStartIndex);
    void SetBordersOfBlocks(CNodeArray& NodeArray);
    uint32_t BuildTreeBranch(CNodeArray& NodeArray, uint32_t nStartIndex, uint32_t ulEndOffset);
    void ReduceConditionalExpressions(CNodeArray& NodeArray);
    bool IsOmittetArgsAllowed(uint16_t wOpcode);

    void StoreDefinitions(CArchive& ar);
    void StoreDeclarations(CArchive& ar);

    std::string GetSource( CNode& node, bool bLabel, uint32_t ulNumArgs);
    std::string GetSource( CNode& node, bool bLabel, uint32_t ulNumArgs, uint32_t aulProcArg[], uint32_t ulProcArgCount);
    bool ArgNeedParens(const CNode& node, const CNode& argument, CFalloutScript::Assoc assoc = CFalloutScript::NON_ASSOC);
    std::string GetIndentString(INT_PTR nLevel);

    int GetPriority(uint16_t wOperator);
    Assoc GetAssociation(uint16_t wOperator);

private:
    
    class CDefObject
    {
    public:
        enum ObjectType {
            OBJECT_VARIABLE,
            OBJECT_PROCEDURE
        };

    public:
        CDefObject(ObjectType type = OBJECT_VARIABLE, uint32_t ulAttributes = 0, uint32_t ulObjectData = 0);

    public:
        ObjectType m_ObjectType;
        uint32_t m_ulAttributes;

        union {
            uint32_t m_ulVarValue;
            uint32_t m_ulProcIndex;
        };
    };

private:
    // CMapuint32_tToDefObject
    typedef CMap<uint32_t, uint32_t, CDefObject, CDefObject&> CMapuint32_tToDefObject;

    CStartupCode m_StartupCode;
    CProcTable   m_ProcTable;
    CNamespace   m_Namespace;
    CNamespace   m_Stringspace;

    COpcodeArray m_GlobalVar;
    COpcodeArray m_ExportedVar;
    COpcodeArray m_ExportedVarValue;
    COpcodeArray m_ExportedProc;

    CArrayOfNodeArray m_ProcBodies;
    CArrayOfNodeArray m_Conditions;

    CMapuint32_tToDefObject m_Definitions;
    std::vector<std::string> m_GlobalVarsNames;
};

#endif //FALLOUT_SCRIPT_H
