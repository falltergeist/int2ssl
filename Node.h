#ifndef NODE_H
#define NODE_H

#include "Namespace.h"
#include "Opcode.h"

class CNode;
typedef CArray<CNode, CNode&> CNodeArray;
typedef CArray<CNodeArray, CNodeArray&> CArrayOfNodeArray;

class CNode
{
public:
    enum Type {
        TYPE_NORMAL,
        TYPE_BEGIN_OF_BLOCK,
        TYPE_END_OF_BLOCK,
        TYPE_OMITTED_ARGUMENT,
        TYPE_BREAK,
        TYPE_CONTINUE,
        TYPE_FOR_LOOP,
        TYPE_CONDITIONAL_EXPRESSION
    };

public:
    CNode(Type type = TYPE_NORMAL);
    CNode(const CNode& node);
    virtual ~CNode();

public:
    CNode& operator = (const CNode& node);

public:
    void StoreTree(CArchive& ar, int nIndent, int nIndex);
    ULONG  GetTopOffset();
    bool IsExpression() const;
    bool IsInfix() const;

public:
    ULONG   m_ulOffset;
    COpcode m_Opcode;

    Type m_Type;
    CNodeArray m_Arguments;
};

// 'Begin' end 'End of block' 'End Else Begin' nodes
const CNode c_NodeBeginOfBlock(CNode::TYPE_BEGIN_OF_BLOCK);
const CNode c_NodeEndOfBlock(CNode::TYPE_END_OF_BLOCK);

#endif //NODE_H
