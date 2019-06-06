#include "StdAfx.h"
#include "ProgressEx.h"

ProgressEx::ProgressEx(void)
{
}

ProgressEx::~ProgressEx(void)
{
}

VOID ProgressEx::SetForePic( const tstring strFileName )
{
	m_pRender->DestroyImage(m_pForePic);
	m_strForePicFileName = strFileName;
	m_pForePic = m_pRender->CreateImage(m_strForePicFileName, m_rcForePic);
}