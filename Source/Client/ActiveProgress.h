#pragma once

/** \class   ActiveProgress
    \brief   ActiveProgress，继承GUIProgress
    \remarks 提供更多功能接口
*/
class ActiveProgress : public GUIProgress
{
public:
	ActiveProgress();
	virtual ~ActiveProgress();
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Update();

	DWORD GetForePicColor() const { return m_dwForePicColor; }
	VOID  SetForePicColor( DWORD dwColor ) { m_dwForePicColor = dwColor; }

	virtual VOID Glint( const BOOL bGlint, const BYTE byGlintEndAlpha = 0 );
	virtual VOID SetGlintTime( DWORD dwCycleTime ) { m_dwGlintCycleTime = dwCycleTime; }
private:
	DWORD      m_dwInitForePicColor;
	DWORD      m_dwGlintStartTime;
	DWORD      m_dwGlintCycleTime;
	DWORD      m_dwLastUpdataTime;
	BOOL       m_bGlint;
	BYTE       m_byGlintEndAlpha;
	BYTE       GetAlphaARGB( DWORD dwColor ) { return BYTE( dwColor>>24 ); }
	VOID       SetAlphaARGB( DWORD& dwColor, BYTE nAlpha ) { dwColor = ( nAlpha << 24 ) | ( dwColor & 0x00ffffff ); }
};
