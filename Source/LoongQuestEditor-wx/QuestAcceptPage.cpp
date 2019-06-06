/////////////////////////////////////////////////////////////////////////////
// Program:     loong任务编辑器
// Name:        QuestAcceptPage.cpp
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

// control ids
enum
{
	QuestAcceptPage_MaxLevel = 100,
	QuestAcceptPage_MinLevel,
	QuestAcceptPage_Rep1,
	QuestAcceptPage_Rep1Min,
	QuestAcceptPage_Rep1Max,
	QuestAcceptPage_Rep2,
	QuestAcceptPage_Rep2Min,
	QuestAcceptPage_Rep2Max,
	QuestAcceptPage_Rep3,
	QuestAcceptPage_Rep3Min,
	QuestAcceptPage_Rep3Max,
	QuestAcceptPage_Rep4,
	QuestAcceptPage_Rep4Min,
	QuestAcceptPage_Rep4Max,
	QuestAcceptPage_Skill1,
	QuestAcceptPage_Skill1Val,
	QuestAcceptPage_Skill2,
	QuestAcceptPage_Skill2Val,
	QuestAcceptPage_Skill3,
	QuestAcceptPage_Skill3Val,
	QuestAcceptPage_Skill4,
	QuestAcceptPage_Skill4Val,
	QuestAcceptPage_Item1,
	QuestAcceptPage_Item1Num,
	QuestAcceptPage_Item2,
	QuestAcceptPage_Item2Num,
	QuestAcceptPage_Item3,
	QuestAcceptPage_Item3Num,
	QuestAcceptPage_Item4,
	QuestAcceptPage_Item4Num,
	QuestAcceptPage_Item5,
	QuestAcceptPage_Item5Num,
	QuestAcceptPage_Item6,
	QuestAcceptPage_Item6Num,
	QuestAcceptPage_Item7,
	QuestAcceptPage_Item7Num,
	QuestAcceptPage_Item8,
	QuestAcceptPage_Item8Num,
	QuestAcceptPage_Trigger1,
	QuestAcceptPage_Trigger2,
	QuestAcceptPage_Trigger3,
	QuestAcceptPage_Trigger4,
	QuestAcceptPage_Trigger5,
	QuestAcceptPage_Trigger6,
	QuestAcceptPage_Trigger7,
	QuestAcceptPage_Trigger8,
	QuestAcceptPage_Att1,
	QuestAcceptPage_Att1Val,
	QuestAcceptPage_Att2,
	QuestAcceptPage_Att2Val,
	QuestAcceptPage_Att3,
	QuestAcceptPage_Att3Val,
	QuestAcceptPage_Att4,
	QuestAcceptPage_Att4Val,
	QuestAcceptPage_Class,
	QuestAcceptPage_Map,
	QuestAcceptPage_Lover,
	QuestAcceptPage_DelRestrictedItem,
	QuestAcceptPage_ReqMarried,
	QuestAcceptPage_Sex,
	QuestAcceptPage_Money,
	QuestAcceptPage_Del_Money
};

enum
{
	MAN,
	WOMAN,
	NONE
};

// ----------------------------------------------------------------------------
// 任务接取条件
// ----------------------------------------------------------------------------
class QuestAcceptPage: public MyPage
{
public:
	QuestAcceptPage(wxBookCtrlBase *book);
	//更新数据
	void Update(tagQuestProto *pQuestProto);
	//保存数据
	void Save(tagQuestProto *pQuestProto);

	static MyPageInfo ms_infoQuestAcceptPage;
protected:
	//TextCtrl
	wxTextCtrl *m_textMinLevel;
	wxTextCtrl *m_textMaxLevel;
	
	wxTextCtrl *m_textRep1;
	wxTextCtrl *m_textRep1_Max;
	wxTextCtrl *m_textRep1_Min;
	wxTextCtrl *m_textRep2;
	wxTextCtrl *m_textRep2_Max;
	wxTextCtrl *m_textRep2_Min;
	wxTextCtrl *m_textRep3;
	wxTextCtrl *m_textRep3_Max;
	wxTextCtrl *m_textRep3_Min;
	wxTextCtrl *m_textRep4;
	wxTextCtrl *m_textRep4_Max;
	wxTextCtrl *m_textRep4_Min;
	
	wxTextCtrl *m_textSkill1;
	wxTextCtrl *m_textSkill1_Val;
	wxTextCtrl *m_textSkill2;
	wxTextCtrl *m_textSkill2_Val;
	wxTextCtrl *m_textSkill3;
	wxTextCtrl *m_textSkill3_Val;
	wxTextCtrl *m_textSkill4;
	wxTextCtrl *m_textSkill4_Val;

	wxTextCtrl *m_textItem1;
	wxTextCtrl *m_textItem1_Num;
	wxTextCtrl *m_textItem2;
	wxTextCtrl *m_textItem2_Num;
	wxTextCtrl *m_textItem3;
	wxTextCtrl *m_textItem3_Num;
	wxTextCtrl *m_textItem4;
	wxTextCtrl *m_textItem4_Num;
	wxTextCtrl *m_textItem5;
	wxTextCtrl *m_textItem5_Num;
	wxTextCtrl *m_textItem6;
	wxTextCtrl *m_textItem6_Num;
	wxTextCtrl *m_textItem7;
	wxTextCtrl *m_textItem7_Num;
	wxTextCtrl *m_textItem8;
	wxTextCtrl *m_textItem8_Num;

	wxTextCtrl *m_textTrigger1;
	wxTextCtrl *m_textTrigger2;
	wxTextCtrl *m_textTrigger3;
	wxTextCtrl *m_textTrigger4;
	wxTextCtrl *m_textTrigger5;
	wxTextCtrl *m_textTrigger6;
	wxTextCtrl *m_textTrigger7;
	wxTextCtrl *m_textTrigger8;

	wxTextCtrl *m_textAtt1;
	wxTextCtrl *m_textAtt1_Val;
	wxTextCtrl *m_textAtt2;
	wxTextCtrl *m_textAtt2_Val;
	wxTextCtrl *m_textAtt3;
	wxTextCtrl *m_textAtt3_Val;
	wxTextCtrl *m_textAtt4;
	wxTextCtrl *m_textAtt4_Val;
	
	wxTextCtrl *m_textClass;
	wxTextCtrl *m_textMap;
	wxTextCtrl *m_textLover;

	wxTextCtrl *m_textMoney;
	//CheckBox
	wxCheckBox *m_chkDelRestrictedItem;
	wxCheckBox *m_chkReqMarried;
	wxCheckBox *m_chkDelMoney;

	//wxRadioBox
	wxRadioBox *m_radioSex;
};

// ----------------------------------------------------------------------------
// 巨大的构造函数
// ----------------------------------------------------------------------------
QuestAcceptPage::QuestAcceptPage(wxBookCtrlBase *book) : MyPage(book)
{
	wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);
	wxSizer *sizerLeft = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizerMiddle = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizerRow;

	//布局-左
	//等级
	wxStaticBox *level_box = new wxStaticBox(this, wxID_ANY, _T("等级"));
	wxSizer *sizer_level = new wxStaticBoxSizer(level_box, wxVERTICAL);
	m_textMinLevel = CreateInfoText();
	m_textMaxLevel = CreateInfoText();
	sizer_level->Add(CreateTextWithLabelSizer(_T("Max"), m_textMaxLevel, _T("Min"), m_textMinLevel), 1, wxGROW|wxALL, 5);

	wxStaticBox *rep_box = new wxStaticBox(this, wxID_ANY, _T("声望"));
	wxSizer *sizer_rep = new wxStaticBoxSizer(rep_box, wxVERTICAL);
	m_textRep1 = CreateInfoText();
	m_textRep1_Max = CreateInfoText();
	m_textRep1_Min = CreateInfoText();
	m_textRep2 = CreateInfoText();
	m_textRep2_Max = CreateInfoText();
	m_textRep2_Min = CreateInfoText();
	m_textRep3 = CreateInfoText();
	m_textRep3_Max = CreateInfoText();
	m_textRep3_Min = CreateInfoText();
	m_textRep4 = CreateInfoText();
	m_textRep4_Max = CreateInfoText();
	m_textRep4_Min = CreateInfoText();
	sizer_rep->Add(CreateTextWithLabelSizer(_T("声望1"), m_textRep1, _T("Max"), m_textRep1_Max, _T("Min"),m_textRep1_Min), 1, wxGROW|wxALL, 5);
	sizer_rep->Add(CreateTextWithLabelSizer(_T("声望2"), m_textRep2, _T("Max"), m_textRep2_Max, _T("Min"),m_textRep2_Min), 1, wxGROW|wxALL, 5);
	sizer_rep->Add(CreateTextWithLabelSizer(_T("声望3"), m_textRep3, _T("Max"), m_textRep3_Max, _T("Min"),m_textRep3_Min), 1, wxGROW|wxALL, 5);
	sizer_rep->Add(CreateTextWithLabelSizer(_T("声望4"), m_textRep4, _T("Max"), m_textRep4_Max, _T("Min"),m_textRep4_Min), 1, wxGROW|wxALL, 5);

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

	static const wxString kinds[] =
	{
		_T("无限制"),
		_T("男"),
		_T("女"),
	};
	m_radioSex = new wxRadioBox(this, wxID_ANY, _T("性别"), wxDefaultPosition, wxDefaultSize,WXSIZEOF(kinds), kinds,3, wxRA_SPECIFY_COLS);
	m_radioSex->Enable(false);

	wxStaticBox *money_box = new wxStaticBox(this, wxID_ANY, _T("金钱"));
	wxSizer *sizer_money_box = new wxStaticBoxSizer(money_box, wxVERTICAL);
	int s_maxWidth;
	GetTextExtent(_T("999"), &s_maxWidth, NULL);
	sizerRow = CreateSizerWithTextAndLabel(_T("金钱"), QuestAcceptPage_Money,&m_textMoney,wxHORIZONTAL,wxSize(s_maxWidth, wxDefaultCoord));
	sizer_money_box->Add(sizerRow, 1, wxALL | wxGROW, 5);
	m_chkDelMoney = CreateCheckBoxAndAddToSizer(sizer_money_box,_T("是否删除金钱"),QuestAcceptPage_Del_Money);

	sizerLeft->Add(sizer_level,0,wxGROW | wxALL, 5);
	sizerLeft->Add(sizer_rep,0,wxGROW | wxALL, 5);
	sizerLeft->Add(sizer_skill,0,wxGROW | wxALL, 5);
	sizerLeft->Add(m_radioSex,0,wxGROW | wxALL, 5);
	sizerLeft->Add(sizer_money_box,0,wxGROW | wxALL, 5);

	//布局-中
	wxStaticBox *item_box = new wxStaticBox(this, wxID_ANY, _T("物品"));
	wxSizer *sizer_item = new wxStaticBoxSizer(item_box, wxVERTICAL);
	m_textItem1 = CreateInfoText();
	m_textItem1_Num = CreateInfoText();
	m_textItem2 = CreateInfoText();
	m_textItem2_Num = CreateInfoText();
	m_textItem3 = CreateInfoText();
	m_textItem3_Num = CreateInfoText();
	m_textItem4 = CreateInfoText();
	m_textItem4_Num = CreateInfoText();
	m_textItem5 = CreateInfoText();
	m_textItem5_Num = CreateInfoText();
	m_textItem6 = CreateInfoText();
	m_textItem6_Num = CreateInfoText();
	m_textItem7 = CreateInfoText();
	m_textItem7_Num = CreateInfoText();
	m_textItem8 = CreateInfoText();
	m_textItem8_Num = CreateInfoText();
	sizer_item->Add(CreateTextWithLabelSizer(_T("ID1"), m_textItem1, _T("数量"), m_textItem1_Num), 1, wxGROW|wxALL, 5);
	sizer_item->Add(CreateTextWithLabelSizer(_T("ID2"), m_textItem2, _T("数量"), m_textItem2_Num), 1, wxGROW|wxALL, 5);
	sizer_item->Add(CreateTextWithLabelSizer(_T("ID3"), m_textItem3, _T("数量"), m_textItem3_Num), 1, wxGROW|wxALL, 5);
	sizer_item->Add(CreateTextWithLabelSizer(_T("ID4"), m_textItem4, _T("数量"), m_textItem4_Num), 1, wxGROW|wxALL, 5);
	sizer_item->Add(CreateTextWithLabelSizer(_T("ID5"), m_textItem5, _T("数量"), m_textItem5_Num), 1, wxGROW|wxALL, 5);
	sizer_item->Add(CreateTextWithLabelSizer(_T("ID6"), m_textItem6, _T("数量"), m_textItem6_Num), 1, wxGROW|wxALL, 5);
	sizer_item->Add(CreateTextWithLabelSizer(_T("ID7"), m_textItem7, _T("数量"), m_textItem7_Num), 1, wxGROW|wxALL, 5);
	sizer_item->Add(CreateTextWithLabelSizer(_T("ID8"), m_textItem8, _T("数量"), m_textItem8_Num), 1, wxGROW|wxALL, 5);

	m_chkDelRestrictedItem = CreateCheckBoxAndAddToSizer(sizer_item,_T("删除限制物品"),QuestAcceptPage_DelRestrictedItem);

	wxStaticBox *att_box = new wxStaticBox(this, wxID_ANY, _T("人物属性"));
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

	sizerMiddle->Add(sizer_item,0,wxGROW|wxALL, 5);
	sizerMiddle->Add(sizer_att,0,wxGROW|wxALL, 5);

	//布局-右
	wxStaticBox *trigger_box = new wxStaticBox(this, wxID_ANY, _T("触发器"));
	wxSizer *sizer_trigger = new wxStaticBoxSizer(trigger_box, wxVERTICAL);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID1"), QuestAcceptPage_Trigger1,&m_textTrigger1);
	sizer_trigger->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID2"), QuestAcceptPage_Trigger2,&m_textTrigger2);
	sizer_trigger->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID3"), QuestAcceptPage_Trigger3,&m_textTrigger3);
	sizer_trigger->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID4"), QuestAcceptPage_Trigger4,&m_textTrigger4);
	sizer_trigger->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID5"), QuestAcceptPage_Trigger5,&m_textTrigger5);
	sizer_trigger->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID6"), QuestAcceptPage_Trigger6,&m_textTrigger6);
	sizer_trigger->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID7"), QuestAcceptPage_Trigger7,&m_textTrigger7);
	sizer_trigger->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID8"), QuestAcceptPage_Trigger8,&m_textTrigger8);
	sizer_trigger->Add(sizerRow, 1, wxALL | wxGROW, 5);

	wxStaticBox *class_box = new wxStaticBox(this, wxID_ANY, _T("职业"));
	wxSizer *sizer_class = new wxStaticBoxSizer(class_box, wxVERTICAL);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID"), QuestAcceptPage_Class,&m_textClass);
	sizer_class->Add(sizerRow, 1, wxALL | wxGROW, 5);

	wxStaticBox *map_box = new wxStaticBox(this, wxID_ANY, _T("地图"));
	wxSizer *sizer_map = new wxStaticBoxSizer(map_box, wxVERTICAL);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID"), QuestAcceptPage_Map,&m_textMap);
	sizer_map->Add(sizerRow, 1, wxALL | wxGROW, 5);

	wxStaticBox *lover_box = new wxStaticBox(this, wxID_ANY, _T("情侣"));
	wxSizer *sizer_lover = new wxStaticBoxSizer(lover_box, wxVERTICAL);
	sizerRow = CreateSizerWithTextAndLabel(_T("个数"), QuestAcceptPage_Lover, &m_textLover);
	sizer_lover->Add(sizerRow, 0, wxALL | wxGROW, 5);

	wxStaticBox *spouse_box = new wxStaticBox(this, wxID_ANY, _T("夫妻"));
	wxSizer *sizer_spouse = new wxStaticBoxSizer(spouse_box, wxVERTICAL);
	m_chkReqMarried = CreateCheckBoxAndAddToSizer(sizer_spouse,_T("是否结婚"),QuestAcceptPage_ReqMarried);

	sizerRight->Add(sizer_trigger,0,wxGROW|wxALL, 5);
	sizerRight->Add(sizer_class,0,wxGROW|wxALL, 5);
	sizerRight->Add(sizer_map,0,wxGROW|wxALL, 5);
	sizerRight->Add(sizer_lover,0,wxGROW|wxALL, 5);
	sizerRight->Add(sizer_spouse,0,wxGROW|wxALL, 5);

	//布局-总
	sizerTop->Add(sizerLeft,0,wxGROW | wxALL, 5);
	sizerTop->Add(sizerMiddle,1,wxGROW | wxALL, 5);
	sizerTop->Add(sizerRight,0,wxGROW | wxALL, 5);

	SetSizer(sizerTop);
	sizerTop->Fit(this);
}

MyPage *wxCtorForQuestAcceptPage(wxBookCtrlBase *book)
{
	return new QuestAcceptPage(book);
}

MyPageInfo QuestAcceptPage::ms_infoQuestAcceptPage(wxCtorForQuestAcceptPage, _T("任务接取条件"));

void QuestAcceptPage::Update(tagQuestProto *pQuestProto)
{
	//等级
	SET_TEXT_CTRL_VAL(m_textMinLevel, accept_req_min_level);
	SET_TEXT_CTRL_VAL(m_textMaxLevel, accept_req_max_level);

	//声望
	SET_TEXT_CTRL_VAL(m_textRep1, accept_req_rep[0]);
	SET_TEXT_CTRL_VAL(m_textRep1_Max, accept_req_max_rep[0]);
	SET_TEXT_CTRL_VAL(m_textRep1_Min, accept_req_min_rep[0]);
	SET_TEXT_CTRL_VAL(m_textRep2, accept_req_rep[1]);
	SET_TEXT_CTRL_VAL(m_textRep2_Max, accept_req_max_rep[1]);
	SET_TEXT_CTRL_VAL(m_textRep2_Min, accept_req_min_rep[1]);
	SET_TEXT_CTRL_VAL(m_textRep3, accept_req_rep[2]);
	SET_TEXT_CTRL_VAL(m_textRep3_Max, accept_req_max_rep[2]);
	SET_TEXT_CTRL_VAL(m_textRep3_Min, accept_req_min_rep[2]);
	SET_TEXT_CTRL_VAL(m_textRep4, accept_req_rep[3]);
	SET_TEXT_CTRL_VAL(m_textRep4_Max, accept_req_max_rep[3]);
	SET_TEXT_CTRL_VAL(m_textRep4_Min, accept_req_min_rep[3]);

	//技能
	SET_TEXT_CTRL_VAL_DWORD(m_textSkill1, accept_req_skill[0]);
	SET_TEXT_CTRL_VAL(m_textSkill1_Val, accept_req_skill_val[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textSkill2, accept_req_skill[1]);
	SET_TEXT_CTRL_VAL(m_textSkill2_Val, accept_req_skill_val[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textSkill3, accept_req_skill[2]);
	SET_TEXT_CTRL_VAL(m_textSkill3_Val, accept_req_skill_val[2]);
	SET_TEXT_CTRL_VAL_DWORD(m_textSkill4, accept_req_skill[3]);
	SET_TEXT_CTRL_VAL(m_textSkill4_Val, accept_req_skill_val[3]);

	//物品
	SET_TEXT_CTRL_VAL_DWORD(m_textItem1, accept_req_item[0]);
	SET_TEXT_CTRL_VAL(m_textItem1_Num, accept_req_item_num[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem2, accept_req_item[1]);
	SET_TEXT_CTRL_VAL(m_textItem2_Num, accept_req_item_num[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem3, accept_req_item[2]);
	SET_TEXT_CTRL_VAL(m_textItem3_Num, accept_req_item_num[2]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem4, accept_req_item[3]);
	SET_TEXT_CTRL_VAL(m_textItem4_Num, accept_req_item_num[3]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem5, accept_req_item[4]);
	SET_TEXT_CTRL_VAL(m_textItem5_Num, accept_req_item_num[4]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem6, accept_req_item[5]);
	SET_TEXT_CTRL_VAL(m_textItem6_Num, accept_req_item_num[5]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem7, accept_req_item[6]);
	SET_TEXT_CTRL_VAL(m_textItem7_Num, accept_req_item_num[6]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem8, accept_req_item[7]);
	SET_TEXT_CTRL_VAL(m_textItem8_Num, accept_req_item_num[7]);
	m_chkDelRestrictedItem->SetValue((bool)pQuestProto->del_req_item);

	//人物属性
	SET_TEXT_CTRL_VAL(m_textAtt1, accept_req_att[0]);
	SET_TEXT_CTRL_VAL(m_textAtt1_Val, accept_req_att_val[0]);
	SET_TEXT_CTRL_VAL(m_textAtt2, accept_req_att[1]);
	SET_TEXT_CTRL_VAL(m_textAtt2_Val, accept_req_att_val[1]);
	SET_TEXT_CTRL_VAL(m_textAtt3, accept_req_att[2]);
	SET_TEXT_CTRL_VAL(m_textAtt3_Val, accept_req_att_val[2]);
	SET_TEXT_CTRL_VAL(m_textAtt4, accept_req_att[3]);
	SET_TEXT_CTRL_VAL(m_textAtt4_Val, accept_req_att_val[3]);

	//触发器
	SET_TEXT_CTRL_VAL_DWORD(m_textTrigger1, accept_req_trriger[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textTrigger2, accept_req_trriger[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textTrigger3, accept_req_trriger[2]);
	SET_TEXT_CTRL_VAL_DWORD(m_textTrigger4, accept_req_trriger[3]);
	SET_TEXT_CTRL_VAL_DWORD(m_textTrigger5, accept_req_trriger[4]);
	SET_TEXT_CTRL_VAL_DWORD(m_textTrigger6, accept_req_trriger[5]);
	SET_TEXT_CTRL_VAL_DWORD(m_textTrigger7, accept_req_trriger[6]);
	SET_TEXT_CTRL_VAL_DWORD(m_textTrigger8, accept_req_trriger[7]);

	//性别
	if (pQuestProto->sex>=0 && pQuestProto->sex < 3)
	{
		m_radioSex->Enable(true);
		m_radioSex->SetSelection(pQuestProto->sex);
	}

	//职业
	SET_TEXT_CTRL_VAL(m_textClass, class_type);

	//地图
	SET_TEXT_CTRL_VAL_DWORD(m_textMap, accept_req_map);

	//情侣
	SET_TEXT_CTRL_VAL(m_textLover, lover_num);

	//是否结婚
	m_chkReqMarried->SetValue((bool)pQuestProto->married);

	//金钱
	SET_TEXT_CTRL_VAL(m_textMoney, accept_req_money);
	m_chkDelMoney->SetValue((bool)pQuestProto->accept_del_money);
}

void QuestAcceptPage::Save(tagQuestProto *pQuestProto)
{
	//等级
	GET_TEXT_CTRL_VAL_INT(m_textMinLevel, accept_req_min_level)
	GET_TEXT_CTRL_VAL_INT(m_textMaxLevel, accept_req_max_level)
	
	//声望
	GET_TEXT_CTRL_VAL_INT(m_textRep1, accept_req_rep[0]);
	GET_TEXT_CTRL_VAL_INT(m_textRep1_Max, accept_req_max_rep[0]);
	GET_TEXT_CTRL_VAL_INT(m_textRep1_Min, accept_req_min_rep[0]);
	GET_TEXT_CTRL_VAL_INT(m_textRep2, accept_req_rep[1]);
	GET_TEXT_CTRL_VAL_INT(m_textRep2_Max, accept_req_max_rep[1]);
	GET_TEXT_CTRL_VAL_INT(m_textRep2_Min, accept_req_min_rep[1]);
	GET_TEXT_CTRL_VAL_INT(m_textRep3, accept_req_rep[2]);
	GET_TEXT_CTRL_VAL_INT(m_textRep3_Max, accept_req_max_rep[2]);
	GET_TEXT_CTRL_VAL_INT(m_textRep3_Min, accept_req_min_rep[2]);
	GET_TEXT_CTRL_VAL_INT(m_textRep4, accept_req_rep[3]);
	GET_TEXT_CTRL_VAL_INT(m_textRep4_Max, accept_req_max_rep[3]);
	GET_TEXT_CTRL_VAL_INT(m_textRep4_Min, accept_req_min_rep[3]);

	//技能
	GET_TEXT_CTRL_VAL_DWORD(m_textSkill1, accept_req_skill[0]);
	GET_TEXT_CTRL_VAL_INT(m_textSkill1_Val, accept_req_skill_val[0]);
	GET_TEXT_CTRL_VAL_DWORD(m_textSkill2, accept_req_skill[1]);
	GET_TEXT_CTRL_VAL_INT(m_textSkill2_Val, accept_req_skill_val[1]);
	GET_TEXT_CTRL_VAL_DWORD(m_textSkill3, accept_req_skill[2]);
	GET_TEXT_CTRL_VAL_INT(m_textSkill3_Val, accept_req_skill_val[2]);
	GET_TEXT_CTRL_VAL_DWORD(m_textSkill4, accept_req_skill[3]);
	GET_TEXT_CTRL_VAL_INT(m_textSkill4_Val, accept_req_skill_val[3]);

	//物品
	GET_TEXT_CTRL_VAL_DWORD(m_textItem1, accept_req_item[0]);
	GET_TEXT_CTRL_VAL_INT(m_textItem1_Num, accept_req_item_num[0]);
	GET_TEXT_CTRL_VAL_DWORD(m_textItem2, accept_req_item[1]);
	GET_TEXT_CTRL_VAL_INT(m_textItem2_Num, accept_req_item_num[1]);
	GET_TEXT_CTRL_VAL_DWORD(m_textItem3, accept_req_item[2]);
	GET_TEXT_CTRL_VAL_INT(m_textItem3_Num, accept_req_item_num[2]);
	GET_TEXT_CTRL_VAL_DWORD(m_textItem4, accept_req_item[3]);
	GET_TEXT_CTRL_VAL_INT(m_textItem4_Num, accept_req_item_num[3]);
	GET_TEXT_CTRL_VAL_DWORD(m_textItem5, accept_req_item[4]);
	GET_TEXT_CTRL_VAL_INT(m_textItem5_Num, accept_req_item_num[4]);
	GET_TEXT_CTRL_VAL_DWORD(m_textItem6, accept_req_item[5]);
	GET_TEXT_CTRL_VAL_INT(m_textItem6_Num, accept_req_item_num[5]);
	GET_TEXT_CTRL_VAL_DWORD(m_textItem7, accept_req_item[6]);
	GET_TEXT_CTRL_VAL_INT(m_textItem7_Num, accept_req_item_num[6]);
	GET_TEXT_CTRL_VAL_DWORD(m_textItem8, accept_req_item[7]);
	GET_TEXT_CTRL_VAL_INT(m_textItem8_Num, accept_req_item_num[7]);
	GET_TEXT_CTRL_VAL_BOOL(m_chkDelRestrictedItem,del_req_item)
	
	//人物属性
	GET_TEXT_CTRL_VAL_INT(m_textAtt1, accept_req_att[0]);
	GET_TEXT_CTRL_VAL_INT(m_textAtt1_Val, accept_req_att_val[0]);
	GET_TEXT_CTRL_VAL_INT(m_textAtt2, accept_req_att[1]);
	GET_TEXT_CTRL_VAL_INT(m_textAtt2_Val, accept_req_att_val[1]);
	GET_TEXT_CTRL_VAL_INT(m_textAtt3, accept_req_att[2]);
	GET_TEXT_CTRL_VAL_INT(m_textAtt3_Val, accept_req_att_val[2]);
	GET_TEXT_CTRL_VAL_INT(m_textAtt4, accept_req_att[3]);
	GET_TEXT_CTRL_VAL_INT(m_textAtt4_Val, accept_req_att_val[3]);

	//触发器
	GET_TEXT_CTRL_VAL_DWORD(m_textTrigger1, accept_req_trriger[0]);
	GET_TEXT_CTRL_VAL_DWORD(m_textTrigger2, accept_req_trriger[1]);
	GET_TEXT_CTRL_VAL_DWORD(m_textTrigger3, accept_req_trriger[2]);
	GET_TEXT_CTRL_VAL_DWORD(m_textTrigger4, accept_req_trriger[3]);
	GET_TEXT_CTRL_VAL_DWORD(m_textTrigger5, accept_req_trriger[4]);
	GET_TEXT_CTRL_VAL_DWORD(m_textTrigger6, accept_req_trriger[5]);
	GET_TEXT_CTRL_VAL_DWORD(m_textTrigger7, accept_req_trriger[6]);
	GET_TEXT_CTRL_VAL_DWORD(m_textTrigger8, accept_req_trriger[7]);

	//性别
	GET_RADIO_CTRL_VAL(m_radioSex, sex)

	//职业
	GET_TEXT_CTRL_VAL_ENUM(m_textClass, class_type,EClassType)

	//地图
	GET_TEXT_CTRL_VAL_DWORD(m_textMap, accept_req_map);

	//情侣
	GET_TEXT_CTRL_VAL_INT(m_textLover, lover_num);

	//是否结婚
	GET_TEXT_CTRL_VAL_BOOL(m_chkReqMarried,married);

	//金钱
	GET_TEXT_CTRL_VAL_INT(m_textMoney, accept_req_money);
	GET_TEXT_CTRL_VAL_BOOL(m_chkDelMoney,accept_del_money);
}