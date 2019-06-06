#pragma once

/** \class TalentButton
	\brief 天资按钮
*/
class TalentButton :
	public GUIStatic
{
public:
	TalentButton();
	~TalentButton();

	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
	virtual VOID Render();
	virtual VOID Destroy();

	// 设值天资图标
	VOID SetIcon(const tstring str);
	// 设置天资图标的Alpha通道
	VOID SetAlphaTex(const tstring str);

private:
	tagGUIImage*		m_pIcon;				// 天资的图标
	tagGUIImage*		m_pAlphaTex;			// 天资的图标的Alpha通道
};