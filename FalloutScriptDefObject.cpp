#include "stdafx.h"
#include "FalloutScript.h"

CFalloutScript::CDefObject::CDefObject(ObjectType type, ULONG ulAttributes, ULONG ulObjectData)
{
    m_ulAttributes = ulAttributes;

    switch(m_ObjectType = type)
    {
        case OBJECT_VARIABLE:
            m_ulVarValue = ulObjectData;
            break;

        case OBJECT_PROCEDURE:
            m_ulProcIndex = ulObjectData;
    }
}
