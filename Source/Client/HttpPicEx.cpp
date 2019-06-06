#include "StdAfx.h"
#include "HttpPicEx.h"

#include "..\vEngine\network\HttpDownload.h"

HttpPicEx::HttpPicEx(void) :
    m_pImageDefault(NULL),
    m_pUrlPic(NULL)
{
}

HttpPicEx::~HttpPicEx(void)
{
    SAFE_DEL(m_pHttp);
}

BOOL HttpPicEx::Init( GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement )
{
    return GUIHttpPic::Init(pSystem, pFather, pXmlElement);
    if (P_VALID(m_pPic))
    {
        m_pRender->DestroyImage(m_pPic);
        m_pPic = NULL;
    }
}

VOID HttpPicEx::Destroy()
{
    if (P_VALID(m_pImageDefault))
    {
        m_pRender->DestroyImage(m_pImageDefault);
        m_pImageDefault = NULL;
    }

    if (P_VALID(m_pUrlPic))
    {
        m_pRender->DestroyImage(m_pUrlPic);
        m_pUrlPic = NULL;
    }

    m_pPic = NULL;

    GUIHttpPic::Destroy();
}

VOID HttpPicEx::Render()
{
    GUIHttpPic::Render();
}

VOID HttpPicEx::Update()
{
    GUIStatic::Update();

    if( !m_pUrlPic )
    {
        if( P_VALID(m_pHttp) && TRUE == m_pHttp->GetResult() )
        {
            tstring strDestFile = m_pHttp->GetDestFileName();
            if (P_VALID(m_pUrlPic))
            {
                m_pRender->DestroyImage(m_pUrlPic);
                m_pUrlPic = NULL;
                m_pPic = NULL;
            }
            m_pUrlPic = m_pRender->CreateImage(strDestFile, m_rcPic);
        }
    }

    if (m_pUrlPic)
    {
        m_pPic = m_pUrlPic;
        this->SetRefresh(TRUE);
    }
    else if (m_pImageDefault)
    {
        m_pPic = m_pImageDefault;
        this->SetRefresh(TRUE);
    }
    else
    {
        m_pPic = NULL;
    }
}

VOID HttpPicEx::SetPicURL( const tstring& strFileURL )
{	
    // 释放图片
    if (P_VALID(m_pUrlPic))
    {
        m_pRender->DestroyImage(m_pUrlPic);
        m_pUrlPic = NULL;
        m_pPic = NULL;
    }

    // 设置新的url
    if (strFileURL.empty())
        return;

    m_strPic = strFileURL;

    // 新建下载
    SAFE_DEL(m_pHttp);
    m_pHttp = new HttpDownload();
    m_pHttp->Download(m_strPic.c_str());
}

VOID HttpPicEx::SetDefaultPic( const tstring& strFilePath )
{
    // 释放图片
    if (P_VALID(m_pImageDefault))
    {
        m_pRender->DestroyImage(m_pImageDefault);
        m_pImageDefault = NULL;
        m_pPic = NULL;
    }

    // 建新图片
    m_pImageDefault = m_pRender->CreateImage(strFilePath, m_rcPic);
}