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
#include "Node.h"
#include "Utility.h"

// Third party includes

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
    m_Arguments = node.m_Arguments;
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
        m_Arguments = node.m_Arguments;
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

        default:
            break;
    }

    // Node
    std::string strOutLine;
    uint16_t wOperator = m_Opcode.GetOperator();
    uint32_t ulArgument = m_Opcode.GetArgument();

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
    if (!m_Arguments.empty())
    {
        for(uint32_t i = 0; i < m_Arguments.size(); i++)
        {
            m_Arguments[i].StoreTree(ar, nIndent + 1, i);
        }
    }
    else
    {
        ar.WriteString("\n");
    }

}

uint32_t CNode::GetTopOffset()
{
    if (m_Arguments.size() > 0)
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
