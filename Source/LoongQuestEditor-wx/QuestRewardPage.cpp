/////////////////////////////////////////////////////////////////////////////
// Program:     loong任务编辑器
// Name:        QuestRewardPage.cpp
// Created:     2008-11-03
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "wx/wx.h"
#include "wx/panel.h"
#include "wx/treectrl.h"
#include "wx/app.h"
#include "wx/log.h"
#include "wx/frame.h"
#include "wx/menu.h"

#include "wx/button.h"
#include "wx/checkbox.h"
#include "wx/listbox.h"
#include "wx/statbox.h"
#include "wx/stattext.h"
#include "wx/textctrl.h"
#include "wx/msgdlg.h"

#include "wx/sysopt.h"
#include "wx/bookctrl.h"
#include "wx/sizer.h"
#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/textdlg.h"

#include "LoongQuestEditor.h"
#include "..\WorldDefine\Questdef.h"
#include "QuestMgr.h"
// ----------------------------------------------------------------------------
// 任务奖励
// ----------------------------------------------------------------------------
class QuestRewardPage: public MyPage
{
public:
	QuestRewardPage(wxBookCtrlBase *book);
	//更新数据
	void Update(tagQuestProto *pQuestProto);
	//保存数据
	void Save(tagQuestProto *pQuestProto);

	static MyPageInfo ms_infoQuestRewardPage;
protected:
	//TextCtrl
	wxTextCtrl *m_textItem1;
	wxTextCtrl *m_textItem1_Num;
	wxTextCtrl *m_textItem1_Quality;
	wxTextCtrl *m_textItem2;
	wxTextCtrl *m_textItem2_Num;
	wxTextCtrl *m_textItem2_Quality;
	wxTextCtrl *m_textItem3;
	wxTextCtrl *m_textItem3_Num;
	wxTextCtrl *m_textItem3_Quality;
	wxTextCtrl *m_textItem4;
	wxTextCtrl *m_textItem4_Num;
	wxTextCtrl *m_textItem4_Quality;
	wxTextCtrl *m_textItem5;
	wxTextCtrl *m_textItem5_Num;
	wxTextCtrl *m_textItem5_Quality;

	wxTextCtrl *m_textChoiceItem1;
	wxTextCtrl *m_textChoiceItem1_Num;
	wxTextCtrl *m_textChoiceItem1_Quality;
	wxTextCtrl *m_textChoiceItem2;
	wxTextCtrl *m_textChoiceItem2_Num;
	wxTextCtrl *m_textChoiceItem2_Quality;
	wxTextCtrl *m_textChoiceItem3;
	wxTextCtrl *m_textChoiceItem3_Num;
	wxTextCtrl *m_textChoiceItem3_Quality;
	wxTextCtrl *m_textChoiceItem4;
	wxTextCtrl *m_textChoiceItem4_Num;
	wxTextCtrl *m_textChoiceItem4_Quality;
	wxTextCtrl *m_textChoiceItem5;
	wxTextCtrl *m_textChoiceItem5_Num;
	wxTextCtrl *m_textChoiceItem5_Quality;

	wxTextCtrl *m_textRep1;
	wxTextCtrl *m_textRep1_Val;
	wxTextCtrl *m_textRep2;
	wxTextCtrl *m_textRep2_Val;
	wxTextCtrl *m_textRep3;
	wxTextCtrl *m_textRep3_Val;
	wxTextCtrl *m_textRep4;
	wxTextCtrl *m_textRep4_Val;

	wxTextCtrl *m_textContribution1;
	wxTextCtrl *m_textContribution1_Val;
	wxTextCtrl *m_textContribution2;
	wxTextCtrl *m_textContribution2_Val;
	wxTextCtrl *m_textContribution3;
	wxTextCtrl *m_textContribution3_Val;
	wxTextCtrl *m_textContribution4;
	wxTextCtrl *m_textContribution4_Val;

	wxTextCtrl *m_textAtt1;
	wxTextCtrl *m_textAtt1_Val;
	wxTextCtrl *m_textAtt2;
	wxTextCtrl *m_textAtt2_Val;
	wxTextCtrl *m_textAtt3;
	wxTextCtrl *m_textAtt3_Val;
	wxTextCtrl *m_textAtt4;
	wxTextCtrl *m_textAtt4_Val;

	wxTextCtrl *m_textSkill1;
	wxTextCtrl *m_textSkill1_Val;
	wxTextCtrl *m_textSkill2;
	wxTextCtrl *m_textSkill2_Val;
	wxTextCtrl *m_textSkill3;
	wxTextCtrl *m_textSkill3_Val;
	wxTextCtrl *m_textSkill4;
	wxTextCtrl *m_textSkill4_Val;

	wxTextCtrl *m_textBuff1;
	wxTextCtrl *m_textBuff2;
	wxTextCtrl *m_textBuff3;
	wxTextCtrl *m_textBuff4;
	
	wxTextCtrl *m_textMoney;
	wxTextCtrl *m_textXP;
};

// ----------------------------------------------------------------------------
// 巨大的构造函数
// ---------------------------------------------------------------------------
QuestRewardPage::QuestRewardPage(wxBookCtrlBase *book) : MyPage(book)
{
	wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);
	wxSizer *sizerLeft = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizerMiddle = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);

	//布局-左
	wxStaticBox *att_box = new wxStaticBox(this, wxID_ANY, _T("属性"));
	wxSizer *sizer_att = new wxStaticBoxSizer(att_box, wxVERTICAL);
	m_textAtt1 = CreateInfoText();
	m_textAtt1_Val = CreateInfoText();
	m_textAtt2 = CreateInfoText();
	m_textAtt2_Val = CreateInfoText();
	m_textAtt3 = CreateInfoText();
	m_textAtt3_Val = CreateInfoText();
	m_textAtt4 = CreateInfoText();
	m_textAtt4_Val = CreateInfoText();

	sizer_att->Add(CreateTextWithLabelSizer(_T("属性1"), m_textAtt1, _T("值"), m_textAtt1_Val), 1, wxGROW|wxALL, 5);
	sizer_att->Add(CreateTextWithLabelSizer(_T("属性2"), m_textAtt2, _T("值"), m_textAtt2_Val), 1, wxGROW|wxALL, 5);
	sizer_att->Add(CreateTextWithLabelSizer(_T("属性3"), m_textAtt3, _T("值"), m_textAtt3_Val), 1, wxGROW|wxALL, 5);
	sizer_att->Add(CreateTextWithLabelSizer(_T("属性4"), m_textAtt4, _T("值"), m_textAtt4_Val), 1, wxGROW|wxALL, 5);

	//贡献
	wxStaticBox *contribution_box = new wxStaticBox(this, wxID_ANY, _T("贡献"));
	wxSizer *sizer_contribution = new wxStaticBoxSizer(contribution_box, wxVERTICAL);
	m_textContribution1 = CreateInfoText();
	m_textContribution1_Val = CreateInfoText();
	m_textContribution2 = CreateInfoText();
	m_textContribution2_Val = CreateInfoText();
	m_textContribution3 = CreateInfoText();
	m_textContribution3_Val = CreateInfoText();
	m_textContribution4 = CreateInfoText();
	m_textContribution4_Val = CreateInfoText();

	sizer_contribution->Add(CreateTextWithLabelSizer(_T("贡献1"), m_textContribution1, _T("值"), m_textContribution1_Val), 1, wxGROW|wxALL, 5);
	sizer_contribution->Add(CreateTextWithLabelSizer(_T("贡献2"), m_textContribution2, _T("值"), m_textContribution2_Val), 1, wxGROW|wxALL, 5);
	sizer_contribution->Add(CreateTextWithLabelSizer(_T("贡献3"), m_textContribution3, _T("值"), m_textContribution3_Val), 1, wxGROW|wxALL, 5);
	sizer_contribution->Add(CreateTextWithLabelSizer(_T("贡献4"), m_textContribution4, _T("值"), m_textContribution4_Val), 1, wxGROW|wxALL, 5);

	wxStaticBox *money_box = new wxStaticBox(this, wxID_ANY, _T("金钱"));
	wxSizer *sizer_money = new wxStaticBoxSizer(money_box, wxVERTICAL);
	m_textMoney = CreateInfoText();
	sizer_money->Add(CreateTextWithLabelSizer(_T("金钱"), m_textMoney, wxEmptyString, NULL), 1, wxGROW|wxALL, 5);

	//布局-中
	wxStaticBox *choice_box = new wxStaticBox(this, wxID_ANY, _T("可选物品"));
	wxSizer *sizer_choice = new wxStaticBoxSizer(choice_box, wxVERTICAL);
	m_textChoiceItem1 = CreateInfoText();
	m_textChoiceItem1_Num = CreateInfoText();
	m_textChoiceItem1_Quality = CreateInfoText();
	m_textChoiceItem2 = CreateInfoText();
	m_textChoiceItem2_Num = CreateInfoText();
	m_textChoiceItem2_Quality = CreateInfoText();
	m_textChoiceItem3 = CreateInfoText();
	m_textChoiceItem3_Num = CreateInfoText();
	m_textChoiceItem3_Quality = CreateInfoText();
	m_textChoiceItem4 = CreateInfoText();
	m_textChoiceItem4_Num = CreateInfoText();
	m_textChoiceItem4_Quality = CreateInfoText();
	m_textChoiceItem5 = CreateInfoText();
	m_textChoiceItem5_Num = CreateInfoText();
	m_textChoiceItem5_Quality = CreateInfoText();
	sizer_choice->Add(CreateTextWithLabelSizer(_T("ID1"), m_textChoiceItem1, _T("数量"), m_textChoiceItem1_Num, _T("品级"), m_textChoiceItem1_Quality), 1, wxGROW|wxALL, 5);
	sizer_choice->Add(CreateTextWithLabelSizer(_T("ID2"), m_textChoiceItem2, _T("数量"), m_textChoiceItem2_Num, _T("品级"), m_textChoiceItem2_Quality), 1, wxGROW|wxALL, 5);
	sizer_choice->Add(CreateTextWithLabelSizer(_T("ID3"), m_textChoiceItem3, _T("数量"), m_textChoiceItem3_Num, _T("品级"), m_textChoiceItem3_Quality), 1, wxGROW|wxALL, 5);
	sizer_choice->Add(CreateTextWithLabelSizer(_T("ID4"), m_textChoiceItem4, _T("数量"), m_textChoiceItem4_Num, _T("品级"), m_textChoiceItem4_Quality), 1, wxGROW|wxALL, 5);
	sizer_choice->Add(CreateTextWithLabelSizer(_T("ID5"), m_textChoiceItem5, _T("数量"), m_textChoiceItem5_Num, _T("品级"), m_textChoiceItem5_Quality), 1, wxGROW|wxALL, 5);

	wxStaticBox *skill_box = new wxStaticBox(this, wxID_ANY, _T("技能"));
	wxSizer *sizer_skill = new wxStaticBoxSizer(skill_box, wxVERTICAL);
	m_textSkill1 = CreateInfoText();
	m_textSkill1_Val = CreateInfoText();
	m_textSkill2 = CreateInfoText();
	m_textSkill2_Val = CreateInfoText();
	m_textSkill3 = CreateInfoText();
	m_textSkill3_Val = CreateInfoText();
	m_textSkill4 = CreateInfoText();
	m_textSkill4_Val = CreateInfoText();
	sizer_skill->Add(CreateTextWithLabelSizer(_T("技能1"), m_textSkill1, _T("值"), m_textSkill1_Val), 1, wxGROW|wxALL, 5);
	sizer_skill->Add(CreateTextWithLabelSizer(_T("技能2"), m_textSkill2, _T("值"), m_textSkill2_Val), 1, wxGROW|wxALL, 5);
	sizer_skill->Add(CreateTextWithLabelSizer(_T("技能3"), m_textSkill3, _T("值"), m_textSkill3_Val), 1, wxGROW|wxALL, 5);
	sizer_skill->Add(CreateTextWithLabelSizer(_T("技能4"), m_textSkill4, _T("值"), m_textSkill4_Val), 1, wxGROW|wxALL, 5);

	wxStaticBox *item_box = new wxStaticBox(this, wxID_ANY, _T("物品"));
	wxSizer *sizer_item = new wxStaticBoxSizer(item_box, wxVERTICAL);
	m_textItem1 = CreateInfoText();
	m_textItem1_Num = CreateInfoText();
	m_textItem1_Quality = CreateInfoText();
	m_textItem2 = CreateInfoText();
	m_textItem2_Num = CreateInfoText();
	m_textItem2_Quality = CreateInfoText();
	m_textItem3 = CreateInfoText();
	m_textItem3_Num = CreateInfoText();
	m_textItem3_Quality = CreateInfoText();
	m_textItem4 = CreateInfoText();
	m_textItem4_Num = CreateInfoText();
	m_textItem4_Quality = CreateInfoText();
	m_textItem5 = CreateInfoText();
	m_textItem5_Num = CreateInfoText();
	m_textItem5_Quality = CreateInfoText();
	sizer_item->Add(CreateTextWithLabelSizer(_T("ID1"), m_textItem1, _T("数量"), m_textItem1_Num, _T("品级"), m_textItem1_Quality), 1, wxGROW|wxALL, 5);
	sizer_item->Add(CreateTextWithLabelSizer(_T("ID2"), m_textItem2, _T("数量"), m_textItem2_Num, _T("品级"), m_textItem2_Quality), 1, wxGROW|wxALL, 5);
	sizer_item->Add(CreateTextWithLabelSizer(_T("ID3"), m_textItem3, _T("数量"), m_textItem3_Num, _T("品级"), m_textItem3_Quality), 1, wxGROW|wxALL, 5);
	sizer_item->Add(CreateTextWithLabelSizer(_T("ID4"), m_textItem4, _T("数量"), m_textItem4_Num, _T("品级"), m_textItem4_Quality), 1, wxGROW|wxALL, 5);
	sizer_item->Add(CreateTextWithLabelSizer(_T("ID5"), m_textItem5, _T("数量"), m_textItem5_Num, _T("品级"), m_textItem5_Quality), 1, wxGROW|wxALL, 5);

	wxStaticBox *xp_box = new wxStaticBox(this, wxID_ANY, _T("经验值"));
	wxSizer *sizer_xp = new wxStaticBoxSizer(xp_box, wxVERTICAL);
	m_textXP = CreateInfoText();
	sizer_xp->Add(CreateTextWithLabelSizer(_T("数量"), m_textXP, wxEmptyString, NULL), 1, wxGROW|wxALL, 5);

	//布局-右
	wxStaticBox *rep_box = new wxStaticBox(this, wxID_ANY, _T("声望"));
	wxSizer *sizer_rep = new wxStaticBoxSizer(rep_box, wxVERTICAL);
	m_textRep1 = CreateInfoText();
	m_textRep1_Val = CreateInfoText();
	m_textRep2 = CreateInfoText();
	m_textRep2_Val = CreateInfoText();
	m_textRep3 = CreateInfoText();
	m_textRep3_Val = CreateInfoText();
	m_textRep4 = CreateInfoText();
	m_textRep4_Val = CreateInfoText();
	sizer_rep->Add(CreateTextWithLabelSizer(_T("声望1"), m_textRep1, _T("值"), m_textRep1_Val), 1, wxGROW|wxALL, 5);
	sizer_rep->Add(CreateTextWithLabelSizer(_T("声望2"), m_textRep2, _T("值"), m_textRep2_Val), 1, wxGROW|wxALL, 5);
	sizer_rep->Add(CreateTextWithLabelSizer(_T("声望3"), m_textRep3, _T("值"), m_textRep3_Val), 1, wxGROW|wxALL, 5);
	sizer_rep->Add(CreateTextWithLabelSizer(_T("声望4"), m_textRep4, _T("值"), m_textRep4_Val), 1, wxGROW|wxALL, 5);

	wxStaticBox *buff_box = new wxStaticBox(this, wxID_ANY, _T("Buff"));
	wxSizer *sizer_buff = new wxStaticBoxSizer(buff_box, wxVERTICAL);

	m_textBuff1 = CreateInfoText();
	m_textBuff2 = CreateInfoText();
	m_textBuff3 = CreateInfoText();
	m_textBuff4 = CreateInfoText();
	
	sizer_buff->Add(CreateTextWithLabelSizer(_T("Buff1"), m_textBuff1, wxEmptyString, NULL), 1, wxGROW|wxALL, 5);
	sizer_buff->Add(CreateTextWithLabelSizer(_T("Buff2"), m_textBuff2, wxEmptyString, NULL), 1, wxGROW|wxALL, 5);
	sizer_buff->Add(CreateTextWithLabelSizer(_T("Buff3"), m_textBuff3, wxEmptyString, NULL), 1, wxGROW|wxALL, 5);
	sizer_buff->Add(CreateTextWithLabelSizer(_T("Buff4"), m_textBuff4, wxEmptyString, NULL), 1, wxGROW|wxALL, 5);

	//布局-总
	sizerLeft->Add(sizer_att,0,wxGROW | wxALL, 5);
	sizerLeft->Add(sizer_skill,0,wxGROW | wxALL, 5);
	sizerLeft->Add(sizer_contribution,0,wxGROW | wxALL, 5);

	sizerMiddle->Add(sizer_choice,0,wxGROW | wxALL, 5);
	sizerMiddle->Add(sizer_item,0,wxGROW | wxALL, 5);

	sizerRight->Add(sizer_rep,0,wxGROW | wxALL, 5);
	sizerRight->Add(sizer_buff,0,wxGROW | wxALL, 5);
	sizerRight->Add(sizer_xp,0,wxGROW | wxALL, 5);
	sizerRight->Add(sizer_money,0,wxGROW | wxALL, 5);

	sizerTop->Add(sizerLeft,0,wxGROW | wxALL, 5);
	sizerTop->Add(sizerMiddle,1,wxGROW | wxALL, 5);
	sizerTop->Add(sizerRight,0,wxGROW | wxALL, 5);

	SetSizer(sizerTop);
	sizerTop->Fit(this);
}

MyPage *wxCtorForQuestRewardPage(wxBookCtrlBase *book)
{
	return new QuestRewardPage(book);
}

MyPageInfo QuestRewardPage::ms_infoQuestRewardPage(wxCtorForQuestRewardPage, _T("任务奖励"));

void QuestRewardPage::Update(tagQuestProto *pQuestProto)
{
	//金钱 经验值
	SET_TEXT_CTRL_VAL(m_textMoney, rew_money);
	SET_TEXT_CTRL_VAL(m_textXP, rew_xp);
	
	//声望
	SET_TEXT_CTRL_VAL_DWORD(m_textRep1, rew_rep[0]);
	SET_TEXT_CTRL_VAL(m_textRep1_Val, rew_rep_val[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textRep2, rew_rep[1]);
	SET_TEXT_CTRL_VAL(m_textRep2_Val, rew_rep_val[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textRep3, rew_rep[2]);
	SET_TEXT_CTRL_VAL(m_textRep3_Val, rew_rep_val[2]);
	SET_TEXT_CTRL_VAL_DWORD(m_textRep4, rew_rep[3]);
	SET_TEXT_CTRL_VAL(m_textRep4_Val, rew_rep_val[3]);

	//物品
	SET_TEXT_CTRL_VAL_DWORD(m_textItem1, rew_item[0]);
	SET_TEXT_CTRL_VAL(m_textItem1_Num, rew_item_num[0]);
	SET_TEXT_CTRL_VAL(m_textItem1_Quality, rew_item_quality[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem2, rew_item[1]);
	SET_TEXT_CTRL_VAL(m_textItem2_Num, rew_item_num[1]);
	SET_TEXT_CTRL_VAL(m_textItem2_Quality, rew_item_quality[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem3, rew_item[2]);
	SET_TEXT_CTRL_VAL(m_textItem3_Num, rew_item_num[2]);
	SET_TEXT_CTRL_VAL(m_textItem3_Quality, rew_item_quality[2]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem4, rew_item[3]);
	SET_TEXT_CTRL_VAL(m_textItem4_Num, rew_item_num[3]);
	SET_TEXT_CTRL_VAL(m_textItem4_Quality, rew_item_quality[3]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem5, rew_item[4]);
	SET_TEXT_CTRL_VAL(m_textItem5_Num, rew_item_num[4]);
	SET_TEXT_CTRL_VAL(m_textItem5_Quality, rew_item_quality[4]);
	
	//可选物品
	SET_TEXT_CTRL_VAL_DWORD(m_textChoiceItem1, rew_choice_item[0]);
	SET_TEXT_CTRL_VAL(m_textChoiceItem1_Num, rew_choice_item_num[0]);
	SET_TEXT_CTRL_VAL(m_textChoiceItem1_Quality, rew_choice_quality[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textChoiceItem2, rew_choice_item[1]);
	SET_TEXT_CTRL_VAL(m_textChoiceItem2_Num, rew_choice_item_num[1]);
	SET_TEXT_CTRL_VAL(m_textChoiceItem2_Quality, rew_choice_quality[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textChoiceItem3, rew_choice_item[2]);
	SET_TEXT_CTRL_VAL(m_textChoiceItem3_Num, rew_choice_item_num[2]);
	SET_TEXT_CTRL_VAL(m_textChoiceItem3_Quality, rew_choice_quality[2]);
	SET_TEXT_CTRL_VAL_DWORD(m_textChoiceItem4, rew_choice_item[3]);
	SET_TEXT_CTRL_VAL(m_textChoiceItem4_Num, rew_choice_item_num[3]);
	SET_TEXT_CTRL_VAL(m_textChoiceItem4_Quality, rew_choice_quality[3]);
	SET_TEXT_CTRL_VAL_DWORD(m_textChoiceItem5, rew_choice_item[4]);
	SET_TEXT_CTRL_VAL(m_textChoiceItem5_Num, rew_choice_item_num[4]);
	SET_TEXT_CTRL_VAL(m_textChoiceItem5_Quality, rew_choice_quality[4]);

	//人物属性
	SET_TEXT_CTRL_VAL(m_textAtt1, rew_att[0]);
	SET_TEXT_CTRL_VAL(m_textAtt1_Val, rew_att_val[0]);
	SET_TEXT_CTRL_VAL(m_textAtt2, rew_att[1]);
	SET_TEXT_CTRL_VAL(m_textAtt2_Val, rew_att_val[1]);
	SET_TEXT_CTRL_VAL(m_textAtt3, rew_att[2]);
	SET_TEXT_CTRL_VAL(m_textAtt3_Val, rew_att_val[2]);
	SET_TEXT_CTRL_VAL(m_textAtt4, rew_att[3]);
	SET_TEXT_CTRL_VAL(m_textAtt4_Val, rew_att_val[3]);

	//技能
	SET_TEXT_CTRL_VAL_DWORD(m_textSkill1, rew_skill[0]);
	SET_TEXT_CTRL_VAL(m_textSkill1_Val, rew_skill_val[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textSkill2, rew_skill[1]);
	SET_TEXT_CTRL_VAL(m_textSkill2_Val, rew_skill_val[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textSkill3, rew_skill[2]);
	SET_TEXT_CTRL_VAL(m_textSkill3_Val, rew_skill_val[2]);
	SET_TEXT_CTRL_VAL_DWORD(m_textSkill4, rew_skill[3]);
	SET_TEXT_CTRL_VAL(m_textSkill4_Val, rew_skill_val[3]);
	
	//贡献
	SET_TEXT_CTRL_VAL_DWORD(m_textContribution1, rew_contribution[0]);
	SET_TEXT_CTRL_VAL(m_textContribution1_Val, rew_contribution_val[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textContribution2, rew_contribution[1]);
	SET_TEXT_CTRL_VAL(m_textContribution2_Val, rew_contribution_val[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textContribution3, rew_contribution[2]);
	SET_TEXT_CTRL_VAL(m_textContribution3_Val, rew_contribution_val[2]);
	SET_TEXT_CTRL_VAL_DWORD(m_textContribution4, rew_contribution[3]);
	SET_TEXT_CTRL_VAL(m_textContribution4_Val, rew_contribution_val[3]);
	//buff
	SET_TEXT_CTRL_VAL_DWORD(m_textBuff1, rew_buff[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textBuff2, rew_buff[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textBuff3, rew_buff[2]);
	SET_TEXT_CTRL_VAL_DWORD(m_textBuff4, rew_buff[3]);
}

void QuestRewardPage::Save(tagQuestProto *pQuestProto)
{
	//金钱 经验值
	GET_TEXT_CTRL_VAL_INT(m_textMoney, rew_money);
	GET_TEXT_CTRL_VAL_INT(m_textXP, rew_xp);

	//声望
	GET_TEXT_CTRL_VAL_INT(m_textRep1, rew_rep[0]);
	GET_TEXT_CTRL_VAL_INT(m_textRep1_Val, rew_rep_val[0]);
	GET_TEXT_CTRL_VAL_INT(m_textRep2, rew_rep[1]);
	GET_TEXT_CTRL_VAL_INT(m_textRep2_Val, rew_rep_val[1]);
	GET_TEXT_CTRL_VAL_INT(m_textRep3, rew_rep[2]);
	GET_TEXT_CTRL_VAL_INT(m_textRep3_Val, rew_rep_val[2]);
	GET_TEXT_CTRL_VAL_INT(m_textRep4, rew_rep[3]);
	GET_TEXT_CTRL_VAL_INT(m_textRep4_Val, rew_rep_val[3]);

	//贡献
	GET_TEXT_CTRL_VAL_INT(m_textContribution1, rew_contribution[0]);
	GET_TEXT_CTRL_VAL_INT(m_textContribution1_Val, rew_contribution_val[0]);
	GET_TEXT_CTRL_VAL_INT(m_textContribution2, rew_contribution[1]);
	GET_TEXT_CTRL_VAL_INT(m_textContribution2_Val, rew_contribution_val[1]);
	GET_TEXT_CTRL_VAL_INT(m_textContribution3, rew_contribution[2]);
	GET_TEXT_CTRL_VAL_INT(m_textContribution3_Val, rew_contribution_val[2]);
	GET_TEXT_CTRL_VAL_INT(m_textContribution4, rew_contribution[3]);
	GET_TEXT_CTRL_VAL_INT(m_textContribution4_Val, rew_contribution_val[3]);


	//物品
	GET_TEXT_CTRL_VAL_DWORD(m_textItem1, rew_item[0]);
	GET_TEXT_CTRL_VAL_INT(m_textItem1_Num, rew_item_num[0]);
	
	//不填就是未鉴定, 值为-1
	if (m_textItem1_Quality->IsEmpty())
	{
		pQuestProto->rew_item_quality[0] = -1;
	}
	else
	{
		GET_TEXT_CTRL_VAL_INT(m_textItem1_Quality, rew_item_quality[0]);
	}
	
	GET_TEXT_CTRL_VAL_DWORD(m_textItem2, rew_item[1]);
	GET_TEXT_CTRL_VAL_INT(m_textItem2_Num, rew_item_num[1]);
	
	//不填就是未鉴定, 值为-1
	if (m_textItem2_Quality->IsEmpty())
	{
		pQuestProto->rew_item_quality[1] = -1;
	}
	else
	{
		GET_TEXT_CTRL_VAL_INT(m_textItem2_Quality, rew_item_quality[1]);
	}

	GET_TEXT_CTRL_VAL_DWORD(m_textItem3, rew_item[2]);
	GET_TEXT_CTRL_VAL_INT(m_textItem3_Num, rew_item_num[2]);

	//不填就是未鉴定, 值为-1
	if (m_textItem3_Quality->IsEmpty())
	{
		pQuestProto->rew_item_quality[2] = -1;
	}
	else
	{
		GET_TEXT_CTRL_VAL_INT(m_textItem3_Quality, rew_item_quality[2]);
	}

	GET_TEXT_CTRL_VAL_DWORD(m_textItem4, rew_item[3]);
	GET_TEXT_CTRL_VAL_INT(m_textItem4_Num, rew_item_num[3]);

	//不填就是未鉴定, 值为-1
	if (m_textItem4_Quality->IsEmpty())
	{
		pQuestProto->rew_item_quality[3] = -1;
	}
	else
	{
		GET_TEXT_CTRL_VAL_INT(m_textItem4_Quality, rew_item_quality[3]);
	}

	GET_TEXT_CTRL_VAL_DWORD(m_textItem5, rew_item[4]);
	GET_TEXT_CTRL_VAL_INT(m_textItem5_Num, rew_item_num[4]);
	//不填就是未鉴定, 值为-1
	if (m_textItem5_Quality->IsEmpty())
	{
		pQuestProto->rew_item_quality[4] = -1;
	}
	else
	{
		GET_TEXT_CTRL_VAL_INT(m_textItem5_Quality, rew_item_quality[4]);
	}

	//可选物品
	GET_TEXT_CTRL_VAL_DWORD(m_textChoiceItem1, rew_choice_item[0]);
	GET_TEXT_CTRL_VAL_INT(m_textChoiceItem1_Num, rew_choice_item_num[0]);

	//不填就是未鉴定, 值为-1
	if (m_textChoiceItem1_Quality->IsEmpty())
	{
		pQuestProto->rew_choice_quality[0] = -1;
	}
	else
	{
		GET_TEXT_CTRL_VAL_INT(m_textChoiceItem1_Quality, rew_choice_quality[0]);
	}

	GET_TEXT_CTRL_VAL_DWORD(m_textChoiceItem2, rew_choice_item[1]);
	GET_TEXT_CTRL_VAL_INT(m_textChoiceItem2_Num, rew_choice_item_num[1]);

	if (m_textChoiceItem2_Quality->IsEmpty())
	{
		pQuestProto->rew_choice_quality[1] = -1;
	}
	else
	{
		GET_TEXT_CTRL_VAL_INT(m_textChoiceItem2_Quality, rew_choice_quality[1]);
	}

	GET_TEXT_CTRL_VAL_DWORD(m_textChoiceItem3, rew_choice_item[2]);
	GET_TEXT_CTRL_VAL_INT(m_textChoiceItem3_Num, rew_choice_item_num[2]);

	if (m_textChoiceItem3_Quality->IsEmpty())
	{
		pQuestProto->rew_choice_quality[2] = -1;
	}
	else
	{
		GET_TEXT_CTRL_VAL_INT(m_textChoiceItem3_Quality, rew_choice_quality[2]);
	}

	GET_TEXT_CTRL_VAL_DWORD(m_textChoiceItem4, rew_choice_item[3]);
	GET_TEXT_CTRL_VAL_INT(m_textChoiceItem4_Num, rew_choice_item_num[3]);

	if (m_textChoiceItem4_Quality->IsEmpty())
	{
		pQuestProto->rew_choice_quality[3] = -1;
	}
	else
	{
		GET_TEXT_CTRL_VAL_INT(m_textChoiceItem4_Quality, rew_choice_quality[3]);
	}

	GET_TEXT_CTRL_VAL_DWORD(m_textChoiceItem5, rew_choice_item[4]);
	GET_TEXT_CTRL_VAL_INT(m_textChoiceItem5_Num, rew_choice_item_num[4]);
	if (m_textChoiceItem5_Quality->IsEmpty())
	{
		pQuestProto->rew_choice_quality[4] = -1;
	}
	else
	{
		GET_TEXT_CTRL_VAL_INT(m_textChoiceItem5_Quality, rew_choice_quality[4]);
	}

	//人物属性
	GET_TEXT_CTRL_VAL_INT(m_textAtt1, rew_att[0]);
	GET_TEXT_CTRL_VAL_INT(m_textAtt1_Val, rew_att_val[0]);
	GET_TEXT_CTRL_VAL_INT(m_textAtt2, rew_att[1]);
	GET_TEXT_CTRL_VAL_INT(m_textAtt2_Val, rew_att_val[1]);
	GET_TEXT_CTRL_VAL_INT(m_textAtt3, rew_att[2]);
	GET_TEXT_CTRL_VAL_INT(m_textAtt3_Val, rew_att_val[2]);
	GET_TEXT_CTRL_VAL_INT(m_textAtt4, rew_att[3]);
	GET_TEXT_CTRL_VAL_INT(m_textAtt4_Val, rew_att_val[3]);

	//技能
	GET_TEXT_CTRL_VAL_DWORD(m_textSkill1, rew_skill[0]);
	GET_TEXT_CTRL_VAL_INT(m_textSkill1_Val, rew_skill_val[0]);
	GET_TEXT_CTRL_VAL_DWORD(m_textSkill2, rew_skill[1]);
	GET_TEXT_CTRL_VAL_INT(m_textSkill2_Val, rew_skill_val[1]);
	GET_TEXT_CTRL_VAL_DWORD(m_textSkill3, rew_skill[2]);
	GET_TEXT_CTRL_VAL_INT(m_textSkill3_Val, rew_skill_val[2]);
	GET_TEXT_CTRL_VAL_DWORD(m_textSkill4, rew_skill[3]);
	GET_TEXT_CTRL_VAL_INT(m_textSkill4_Val, rew_skill_val[3]);

	//buff
	GET_TEXT_CTRL_VAL_DWORD(m_textBuff1, rew_buff[0]);
	GET_TEXT_CTRL_VAL_DWORD(m_textBuff2, rew_buff[1]);
	GET_TEXT_CTRL_VAL_DWORD(m_textBuff3, rew_buff[2]);
	GET_TEXT_CTRL_VAL_DWORD(m_textBuff4, rew_buff[3]);
}