#include "stdafx.h"
#include "Node.h"
#include "Utility.h"

CNode::CNode(Type type) :
    m_ulOffset(0),
    m_Type(type)
{
}


CNode::CNode(const CNode& node) :
    m_ulOffset(node.m_ulOffset),
    m_Opcode(node.m_Opcode),
    m_Type(node.m_Type)
{
    m_Arguments.Copy(node.m_Arguments);
}

CNode::~CNode()
{
}

CNode& CNode::operator = (const CNode& node)
{
    if (&node != this)
    {
        m_ulOffset = node.m_ulOffset;
        m_Opcode = node.m_Opcode;
        m_Type = node.m_Type;
        m_Arguments.Copy(node.m_Arguments);
    }
    return (*this);
}

void CNode::StoreTree(CArchive& ar, int nIndent, int nIndex)
{
    // Indent
    if (nIndex != 0)
    {
        ar.WriteString("            ");

        for(int i = 0; i < nIndent; i++)
        {
            ar.WriteString("                  ");
        }
    }

    switch(m_Type)
    {
        case TYPE_BEGIN_OF_BLOCK:
            ar.WriteString("========= begin of block =========\n");
            return;

        case TYPE_END_OF_BLOCK:
            ar.WriteString("========= end of block =========\n");
            return;
    }

    // Node
    std::string strOutLine;
    uint16_t wOperator = m_Opcode.GetOperator();
    ULONG ulArgument = m_Opcode.GetArgument();

    switch(wOperator)
    {
        case COpcode::O_STRINGOP:
        case COpcode::O_INTOP:
            strOutLine = format("0x%04X 0x%08x ",
                              wOperator,
                              ulArgument);
            ar.WriteString(strOutLine);
            break;

        case COpcode::O_FLOATOP:
            strOutLine = format("0x%04X 0x%08X ",
                                wOperator,
                                ulArgument);
            ar.WriteString(strOutLine);
            break;

        default:
            strOutLine = format("0x%04X .......... ",
                                wOperator);
            ar.WriteString(strOutLine);
    }
    if (!m_Arguments.IsEmpty())
    {
        for(int i = 0; i < m_Arguments.GetSize(); i++)
        {
            m_Arguments[i].StoreTree(ar, nIndent + 1, i);
        }
    }
    else
    {
        ar.WriteString("\n");
    }

}

ULONG CNode::GetTopOffset()
{
    if (m_Arguments.GetSize() > 0)
    {
        return m_Arguments[0].GetTopOffset();
    }
    else
    {
        return m_ulOffset;
    }
}

bool CNode::IsExpression() const
{
    return (m_Opcode.GetAttributes().m_Type == COpcode::COpcodeAttributes::TYPE_EXPRESSION 
        || m_Type == TYPE_CONDITIONAL_EXPRESSION);
}

bool CNode::IsInfix() const
{
    return (m_Opcode.GetAttributes().m_Category == COpcode::COpcodeAttributes::CATEGORY_INFIX
        || m_Type == TYPE_CONDITIONAL_EXPRESSION);
}
