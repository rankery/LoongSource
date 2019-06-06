#include "stdafx.h"
#include "StaticNoInput.h"


BOOL StaticNoInput::Init( GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement )
{
	GUIStatic::Init(pSystem, pFather, pXmlElement);
	m_bUnClickable = TRUE;
	return TRUE;
}

