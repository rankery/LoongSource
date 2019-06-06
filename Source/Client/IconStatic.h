#pragma once

class IconStatic
	: public GUIStatic
{
public:
	IconStatic(void);
	~IconStatic(void);

	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);


	//ÉèÖÃ¿Ø¼þÍ¼Æ¬
	void SetPic(const tstring& strPicName)
	{
		if( P_VALID(m_pPic) )
			m_pRender->DestroyImage(m_pPic);

		m_strPic = strPicName;
		m_pPic = m_pRender->CreateImage(m_strPic, m_rcPic);
		if( !P_VALID(m_pPic) )
			IMSG(_T("ComposeStatic pic cann't createimage!\n"));

		SetRefresh( TRUE );
	}

private:
};