#pragma once

//! \class	HttpPicEx
//! \brief	能够实时更改控件图片URL
//! \author	hyu
//! \date	2009-5-25
//! \base	GUIHttpPic
//! \sa		GUIHttpPic

class HttpPicEx :
    public GUIHttpPic
{
public:
    virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
    virtual VOID Destroy();
    virtual VOID Render();
    virtual VOID Update();

    // 设置url
    VOID SetPicURL(const tstring& strFileURL);

    // 设置当url无法解析时默认的图片
    VOID SetDefaultPic(const tstring& strFilePath);

    HttpPicEx(void);
    virtual ~HttpPicEx(void);

protected:
    tagGUIImage*        m_pImageDefault;        //< 默认图片
    tagGUIImage*        m_pUrlPic;              //< URL图片
};
