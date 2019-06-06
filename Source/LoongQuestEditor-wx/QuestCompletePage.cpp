/////////////////////////////////////////////////////////////////////////////
// Program:     loong任务编辑器
// Name:        QuestCompletePage.cpp
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
	QuestCompletePage_Trigger1 = 100,
	QuestCompletePage_Trigger2,
	QuestCompletePage_Trigger3,
	QuestCompletePage_Trigger4,
	QuestCompletePage_Trigger5,
	QuestCompletePage_Trigger6,
	QuestCompletePage_Trigger7,
	QuestCompletePage_Trigger8
};
// ----------------------------------------------------------------------------
// 任务完成条件
// ----------------------------------------------------------------------------
class QuestCompletePage: public MyPage
{
public:
	QuestCompletePage(wxBookCtrlBase *book);
	//更新数据
	void Update(tagQuestProto *pQuestProto);
	//保存数据
	void Save(tagQuestProto *pQuestProto);

	static MyPageInfo ms_infoQuestCompletePage;
protected:
	//TextCtrl
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

	wxTextCtrl *m_textNPC1;
	wxTextCtrl *m_textNPC2;
	wxTextCtrl *m_textNPC3;
	wxTextCtrl *m_textNPC4;
	wxTextCtrl *m_textNPC5;
	wxTextCtrl *m_textNPC6;
	wxTextCtrl *m_textNPC7;
	wxTextCtrl *m_textNPC8;

	wxTextCtrl *m_textTrigger1;
	wxTextCtrl *m_textTrigger2;
	wxTextCtrl *m_textTrigger3;
	wxTextCtrl *m_textTrigger4;
	wxTextCtrl *m_textTrigger5;
	wxTextCtrl *m_textTrigger6;
	wxTextCtrl *m_textTrigger7;
	wxTextCtrl *m_textTrigger8;
	
	wxTextCtrl *m_textCreature1;
	wxTextCtrl *m_textCreature2;
	wxTextCtrl *m_textCreature3;
	wxTextCtrl *m_textCreature1_Num;
	wxTextCtrl *m_textCreature1_Des; //任务追踪描述
	wxTextCtrl *m_textCreature4;
	wxTextCtrl *m_textCreature5;
	wxTextCtrl *m_textCreature6;
	wxTextCtrl *m_textCreature2_Num;
	wxTextCtrl *m_textCreature2_Des; //任务追踪描述
	wxTextCtrl *m_textCreature7;
	wxTextCtrl *m_textCreature8;
	wxTextCtrl *m_textCreature9;
	wxTextCtrl *m_textCreature3_Num;
	wxTextCtrl *m_textCreature3_Des; //任务追踪描述
	wxTextCtrl *m_textCreature10;
	wxTextCtrl *m_textCreature11;
	wxTextCtrl *m_textCreature12;
	wxTextCtrl *m_textCreature4_Num;
	wxTextCtrl *m_textCreature4_Des; //任务追踪描述
	wxTextCtrl *m_textCreature13;
	wxTextCtrl *m_textCreature14;
	wxTextCtrl *m_textCreature15;
	wxTextCtrl *m_textCreature5_Num;
	wxTextCtrl *m_textCreature5_Des; //任务追踪描述
	wxTextCtrl *m_textCreature16;
	wxTextCtrl *m_textCreature17;
	wxTextCtrl *m_textCreature18;
	wxTextCtrl *m_textCreature6_Num;
	wxTextCtrl *m_textCreature6_Des; //任务追踪描述

	wxTextCtrl *m_textMoney_Num;
	wxTextCtrl *m_textLevel;
	wxTextCtrl *m_textLover;
	
	wxTextCtrl *m_textMap;
	wxTextCtrl *m_textX;
	wxTextCtrl *m_textY;
	wxTextCtrl *m_textZ;
	wxTextCtrl *m_textRadius;
	wxTextCtrl *m_textEvent_Item;
	wxTextCtrl *m_textEvent_Skill;
	wxTextCtrl *m_textEvent_Other;

	//CheckBox
	wxCheckBox *m_chkOnlyOneItem;
	wxCheckBox *m_chkDeleteItem;
	wxCheckBox *m_chkOnlyInOrder;
	wxCheckBox *m_chkOnlyOneNPC;
	wxCheckBox *m_chkOnlyOneTrigger;
	wxCheckBox *m_chkCreatureLevel;
	wxCheckBox *m_chkOnlyOneCreature;
	wxCheckBox *m_chkOnlyOneCondition;
	wxCheckBox *m_chkRandom;
	wxCheckBox *m_chkReqMarried;
	wxCheckBox *m_chkDelMoney;

	//RadioButton
	wxRadioButton  *m_rabItem;
	wxRadioButton  *m_rabSkill;
	wxRadioButton  *m_rabOther;
};

// ----------------------------------------------------------------------------
// 巨大的构造函数
// ---------------------------------------------------------------------------
QuestCompletePage::QuestCompletePage(wxBookCtrlBase *book) : MyPage(book)
{
	wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);
	wxSizer *sizerLeft = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizerMiddle = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizerRow;

	//布局-左
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

	m_chkOnlyOneItem = CreateCheckBoxAndAddToSizer(sizer_item,_T("搜集完其中一种就可完成"));
	m_chkDeleteItem = CreateCheckBoxAndAddToSizer(sizer_item,_T("完成任务后删除物品"));

	wxStaticBox *trigger_box = new wxStaticBox(this, wxID_ANY, _T("触发器"));
	wxSizer *sizer_trigger = new wxStaticBoxSizer(trigger_box, wxVERTICAL);

	m_textTrigger1 = CreateInfoText();
	m_textTrigger2 = CreateInfoText();
	m_textTrigger3 = CreateInfoText();
	m_textTrigger4 = CreateInfoText();
	m_textTrigger5 = CreateInfoText();
	m_textTrigger6 = CreateInfoText();
	m_textTrigger7 = CreateInfoText();
	m_textTrigger8 = CreateInfoText();
	sizer_trigger->Add(CreateTextWithLabelSizer(_T("ID1"), m_textTrigger1, _T("ID2"), m_textTrigger2), 1, wxGROW|wxALL, 5);
	sizer_trigger->Add(CreateTextWithLabelSizer(_T("ID3"), m_textTrigger3, _T("ID4"), m_textTrigger4), 1, wxGROW|wxALL, 5);
	sizer_trigger->Add(CreateTextWithLabelSizer(_T("ID5"), m_textTrigger5, _T("ID6"), m_textTrigger6), 1, wxGROW|wxALL, 5);
	sizer_trigger->Add(CreateTextWithLabelSizer(_T("ID7"), m_textTrigger7, _T("ID8"), m_textTrigger8), 1, wxGROW|wxALL, 5);
	m_chkOnlyOneTrigger = CreateCheckBoxAndAddToSizer(sizer_trigger,_T("只触发一个就可完成"));

	wxStaticBox *map_box = new wxStaticBox(this, wxID_ANY, _T("地图"));
	wxSizer *sizer_map = new wxStaticBoxSizer(map_box, wxVERTICAL);

	m_textMap = CreateInfoText();
	m_textX = CreateInfoText();
	m_textY = CreateInfoText();
	m_textZ = CreateInfoText();
	m_textRadius = CreateInfoText();
	sizer_map->Add(CreateTextWithLabelSizer(_T("ID"), m_textMap, _T("X"), m_textX), 1, wxGROW|wxALL, 5);
	sizer_map->Add(CreateTextWithLabelSizer(_T("Y"), m_textY,  _T("Z"), m_textZ), 1, wxGROW|wxALL, 5);
	sizer_map->Add(CreateTextWithLabelSizer(_T("半径"), m_textRadius, wxEmptyString, NULL), 1, wxGROW|wxALL, 5);
	m_chkRandom = CreateCheckBoxAndAddToSizer(sizer_map,_T("这个坐标是否随机生成"));

	wxStaticBox *money_box = new wxStaticBox(this, wxID_ANY, _T("金钱&等级"));
	wxSizer *sizer_money = new wxStaticBoxSizer(money_box, wxVERTICAL);
	m_textMoney_Num = CreateInfoText();
	m_textLevel = CreateInfoText();
	sizer_money->Add(CreateTextWithLabelSizer(_T("金钱"), m_textMoney_Num, _T("等级"), m_textLevel), 1, wxGROW|wxALL, 5);
	m_chkDelMoney = CreateCheckBoxAndAddToSizer(sizer_money,_T("是否删除金钱"));

	//布局-中
	wxStaticBox *creature_box = new wxStaticBox(this, wxID_ANY, _T("怪物数量"));
	wxSizer *sizer_creature = new wxStaticBoxSizer(creature_box, wxVERTICAL);
	
	m_textCreature1 = CreateInfoText();
	m_textCreature2 = CreateInfoText();
	m_textCreature3 = CreateInfoText();
	m_textCreature1_Num = CreateInfoText();
	//m_textCreature1_Des = CreateInfoText();
	sizer_creature->Add(CreateTextWithLabelSizer(_T("ID"), m_textCreature1, m_textCreature2, m_textCreature3,_T("数量"),m_textCreature1_Num), 1, wxGROW|wxALL, 5);
	//sizer_creature->Add(CreateTextWithLabelSizer(_T("任务追踪描述:"), m_textCreature1_Des, NULL, NULL,NULL,wxEmptyString))
	m_textCreature4 = CreateInfoText();
	m_textCreature5 = CreateInfoText();
	m_textCreature6 = CreateInfoText();
	m_textCreature2_Num = CreateInfoText();
	sizer_creature->Add(CreateTextWithLabelSizer(_T("ID"), m_textCreature4, m_textCreature5, m_textCreature6,_T("数量"),m_textCreature2_Num), 1, wxGROW|wxALL, 5);
	m_textCreature7 = CreateInfoText();
	m_textCreature8 = CreateInfoText();
	m_textCreature9 = CreateInfoText();
	m_textCreature3_Num = CreateInfoText();
	sizer_creature->Add(CreateTextWithLabelSizer(_T("ID"), m_textCreature7, m_textCreature8, m_textCreature9,_T("数量"),m_textCreature3_Num), 1, wxGROW|wxALL, 5);
	m_textCreature10 = CreateInfoText();
	m_textCreature11 = CreateInfoText();
	m_textCreature12 = CreateInfoText();
	m_textCreature4_Num = CreateInfoText();
	sizer_creature->Add(CreateTextWithLabelSizer(_T("ID"), m_textCreature10, m_textCreature11, m_textCreature12,_T("数量"),m_textCreature4_Num), 1, wxGROW|wxALL, 5);
	m_textCreature13 = CreateInfoText();
	m_textCreature14 = CreateInfoText();
	m_textCreature15 = CreateInfoText();
	m_textCreature5_Num = CreateInfoText();
	sizer_creature->Add(CreateTextWithLabelSizer(_T("ID"), m_textCreature13, m_textCreature14, m_textCreature15,_T("数量"),m_textCreature5_Num), 1, wxGROW|wxALL, 5);
	m_textCreature16 = CreateInfoText();
	m_textCreature17 = CreateInfoText();
	m_textCreature18 = CreateInfoText();
	m_textCreature6_Num = CreateInfoText();
	sizer_creature->Add(CreateTextWithLabelSizer(_T("ID"), m_textCreature16, m_textCreature17, m_textCreature18,_T("数量"),m_textCreature6_Num), 1, wxGROW|wxALL, 5);
	
	m_chkCreatureLevel = CreateCheckBoxAndAddToSizer(sizer_creature,_T("指定怪物等级"));
	m_chkOnlyOneCreature = CreateCheckBoxAndAddToSizer(sizer_creature,_T("杀完指定数量的一种怪就可完成"));

	wxStaticBox *love_box = new wxStaticBox(this, wxID_ANY, _T("感情"));
	wxSizer *sizer_love = new wxStaticBoxSizer(love_box, wxVERTICAL);
	m_textLover = CreateInfoText();
	sizer_love->Add(CreateTextWithLabelSizer(_T("情侣数量"), m_textLover, wxEmptyString, NULL), 1, wxGROW|wxALL, 5);
	m_chkReqMarried = CreateCheckBoxAndAddToSizer(sizer_love,_T("是否结婚"));

	//布局-右
	wxStaticBox *npc_box = new wxStaticBox(this, wxID_ANY, _T("NPC"));
	wxSizer *sizer_npc = new wxStaticBoxSizer(npc_box, wxVERTICAL);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID1"), QuestCompletePage_Trigger1,&m_textNPC1);
	sizer_npc->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID2"), QuestCompletePage_Trigger2,&m_textNPC2);
	sizer_npc->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID3"), QuestCompletePage_Trigger3,&m_textNPC3);
	sizer_npc->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID4"), QuestCompletePage_Trigger4,&m_textNPC4);
	sizer_npc->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID5"), QuestCompletePage_Trigger5,&m_textNPC5);
	sizer_npc->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID6"), QuestCompletePage_Trigger6,&m_textNPC6);
	sizer_npc->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID7"), QuestCompletePage_Trigger7,&m_textNPC7);
	sizer_npc->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("ID8"), QuestCompletePage_Trigger8,&m_textNPC8);
	sizer_npc->Add(sizerRow, 1, wxALL | wxGROW, 5);

	m_chkOnlyInOrder = CreateCheckBoxAndAddToSizer(sizer_npc,_T("必须按顺序对话"));
	m_chkOnlyOneNPC = CreateCheckBoxAndAddToSizer(sizer_npc,_T("只和一个NPC对话后就可完成"));

	wxStaticBox *event_box = new wxStaticBox(this, wxID_ANY, _T("事件"));
	wxSizer *sizer_event = new wxStaticBoxSizer(event_box, wxVERTICAL);
	m_rabItem = CreateInfoRadio(_T("使用物品"), wxRB_GROUP);
	m_rabSkill = CreateInfoRadio(_T("使用技能"));
	m_rabOther = CreateInfoRadio(_T("待定"));
	m_rabItem->SetValue(false);
	m_rabSkill->SetValue(false);
	m_rabOther->SetValue(false);
	m_textEvent_Item = CreateInfoText();
	m_textEvent_Skill = CreateInfoText();
	m_textEvent_Other = CreateInfoText();
	sizerRow = CreateRadioSizer(m_rabItem, _T("物品ID"), m_textEvent_Item);
	sizer_event->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateRadioSizer(m_rabSkill, _T("技能ID"), m_textEvent_Skill);
	sizer_event->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateRadioSizer(m_rabOther, _T("待定"), m_textEvent_Other);
	sizer_event->Add(sizerRow, 1, wxALL | wxGROW, 5);

	//布局-总
	//sizerLeft->Add(sizer_item,0,wxGROW|wxALL, 5);
	sizerLeft->Add(sizer_item,0,wxGROW|wxALL, 5);
	sizerLeft->Add(sizer_trigger,0,wxGROW|wxALL, 5);
	sizerLeft->Add(sizer_money,0,wxGROW|wxALL, 5);

	//sizerMiddle->Add(sizer_creature,0,wxGROW|wxALL, 5);
	sizerMiddle->Add(sizer_creature,0,wxGROW|wxALL, 5);
	sizerMiddle->Add(sizer_map,0,wxGROW|wxALL, 5);
	sizerMiddle->Add(sizer_love,0,wxGROW|wxALL, 5);
	m_chkOnlyOneCondition = CreateCheckBoxAndAddToSizer(sizerMiddle,_T("以上所有条件完成一项即可完成整个任务"));

	sizerRight->Add(sizer_npc,0,wxGROW|wxALL, 5);
	sizerRight->Add(sizer_event,0,wxGROW|wxALL, 5);

	sizerTop->Add(sizerLeft,0,wxGROW | wxALL, 5);
	sizerTop->Add(sizerMiddle,1,wxGROW | wxALL, 5);
	sizerTop->Add(sizerRight,0,wxGROW | wxALL, 5);

	SetSizer(sizerTop);
	sizerTop->Fit(this);
}


MyPage *wxCtorForQuestCompletePage(wxBookCtrlBase *book)
{
	return new QuestCompletePage(book);
}

MyPageInfo QuestCompletePage::ms_infoQuestCompletePage(wxCtorForQuestCompletePage, _T("任务完成条件"));

void QuestCompletePage::Update(tagQuestProto *pQuestProto)
{
	//物品
	SET_TEXT_CTRL_VAL_DWORD(m_textItem1, complete_req_item[0]);
	SET_TEXT_CTRL_VAL(m_textItem1_Num, complete_req_item_num[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem2, complete_req_item[1]);
	SET_TEXT_CTRL_VAL(m_textItem2_Num, complete_req_item_num[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem3, complete_req_item[2]);
	SET_TEXT_CTRL_VAL(m_textItem3_Num, complete_req_item_num[2]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem4, complete_req_item[3]);
	SET_TEXT_CTRL_VAL(m_textItem4_Num, complete_req_item_num[3]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem5, complete_req_item[4]);
	SET_TEXT_CTRL_VAL(m_textItem5_Num, complete_req_item_num[4]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem6, complete_req_item[5]);
	SET_TEXT_CTRL_VAL(m_textItem6_Num, complete_req_item_num[5]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem7, complete_req_item[6]);
	SET_TEXT_CTRL_VAL(m_textItem7_Num, complete_req_item_num[6]);
	SET_TEXT_CTRL_VAL_DWORD(m_textItem8, complete_req_item[7]);
	SET_TEXT_CTRL_VAL(m_textItem8_Num, complete_req_item_num[7]);
	m_chkOnlyOneItem->SetValue(pQuestProto->only_one_item);
	m_chkDeleteItem->SetValue(pQuestProto->delete_item);

	//怪
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature1, complete_req_creature[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature2, complete_req_creature[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature3, complete_req_creature[2]);
	SET_TEXT_CTRL_VAL(m_textCreature1_Num, complete_req_creature_num[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature4, complete_req_creature[3]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature5, complete_req_creature[4]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature6, complete_req_creature[5]);
	SET_TEXT_CTRL_VAL(m_textCreature2_Num, complete_req_creature_num[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature7, complete_req_creature[6]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature8, complete_req_creature[7]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature9, complete_req_creature[8]);
	SET_TEXT_CTRL_VAL(m_textCreature3_Num, complete_req_creature_num[2]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature10, complete_req_creature[9]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature11, complete_req_creature[10]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature12, complete_req_creature[11]);
	SET_TEXT_CTRL_VAL(m_textCreature4_Num, complete_req_creature_num[3]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature13, complete_req_creature[12]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature14, complete_req_creature[13]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature15, complete_req_creature[14]);
	SET_TEXT_CTRL_VAL(m_textCreature5_Num, complete_req_creature_num[4]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature16, complete_req_creature[15]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature17, complete_req_creature[16]);
	SET_TEXT_CTRL_VAL_DWORD(m_textCreature18, complete_req_creature[17]);
	SET_TEXT_CTRL_VAL(m_textCreature6_Num, complete_req_creature_num[5]);
	m_chkCreatureLevel->SetValue((bool)pQuestProto->creature_level);
	m_chkOnlyOneCreature->SetValue((bool)pQuestProto->only_one_creature);

	//NPC
	SET_TEXT_CTRL_VAL_DWORD(m_textNPC1, complete_req_npc[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textNPC2, complete_req_npc[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textNPC3, complete_req_npc[2]);
	SET_TEXT_CTRL_VAL_DWORD(m_textNPC4, complete_req_npc[3]);
	SET_TEXT_CTRL_VAL_DWORD(m_textNPC5, complete_req_npc[4]);
	SET_TEXT_CTRL_VAL_DWORD(m_textNPC6, complete_req_npc[5]);
	SET_TEXT_CTRL_VAL_DWORD(m_textNPC7, complete_req_npc[6]);
	SET_TEXT_CTRL_VAL_DWORD(m_textNPC8, complete_req_npc[7]);
	m_chkOnlyInOrder->SetValue((bool)pQuestProto->only_in_order);
	m_chkOnlyOneNPC->SetValue((bool)pQuestProto->only_one_npc);

	//触发器
	SET_TEXT_CTRL_VAL_DWORD(m_textTrigger1, complete_req_trigger[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textTrigger2, complete_req_trigger[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textTrigger3, complete_req_trigger[2]);
	SET_TEXT_CTRL_VAL_DWORD(m_textTrigger4, complete_req_trigger[3]);
	SET_TEXT_CTRL_VAL_DWORD(m_textTrigger5, complete_req_trigger[4]);
	SET_TEXT_CTRL_VAL_DWORD(m_textTrigger6, complete_req_trigger[5]);
	SET_TEXT_CTRL_VAL_DWORD(m_textTrigger7, complete_req_trigger[6]);
	SET_TEXT_CTRL_VAL_DWORD(m_textTrigger8, complete_req_trigger[7]);
	m_chkOnlyOneTrigger->SetValue(pQuestProto->only_one_trigger);

	//钱 等级
	SET_TEXT_CTRL_VAL(m_textMoney_Num, complete_req_money);
	m_chkDelMoney->SetValue((bool)pQuestProto->complete_del_money);
	SET_TEXT_CTRL_VAL(m_textLevel, complete_req_level);

	//地图
	SET_TEXT_CTRL_VAL_DWORD(m_textMap, complete_req_map);
	SET_TEXT_CTRL_VAL_FLOAT(m_textX, complete_req_map_x);
	SET_TEXT_CTRL_VAL_FLOAT(m_textY, complete_req_map_y);
	SET_TEXT_CTRL_VAL_FLOAT(m_textZ, complete_req_map_z);
	SET_TEXT_CTRL_VAL_FLOAT(m_textRadius, complete_req_map_radius);
	m_chkRandom->SetValue(pQuestProto->coordinate_random);

	//任务完成事件
	switch (pQuestProto->event_type)
	{
	case EQCET_UseItem:
		m_rabItem->SetValue(true);
		m_rabSkill->SetValue(false);
		SET_TEXT_CTRL_VAL_DWORD(m_textEvent_Item, use_item);
		SET_TEXT_CTRL_VAL_DWORD(m_textEvent_Skill, use_skill);
		break;
	case EQCET_UseSkill:
		m_rabItem->SetValue(false);
		m_rabSkill->SetValue(true);
		SET_TEXT_CTRL_VAL_DWORD(m_textEvent_Item, use_item);
		SET_TEXT_CTRL_VAL_DWORD(m_textEvent_Skill, use_skill);
		break;
	default:
		m_rabItem->SetValue(false);
		m_rabSkill->SetValue(false);
		SET_TEXT_CTRL_VAL_DWORD(m_textEvent_Item, use_item);
		SET_TEXT_CTRL_VAL_DWORD(m_textEvent_Skill, use_skill);
		break;
	}

	//任意一个条件
	m_chkOnlyOneCondition->SetValue(pQuestProto->only_one_condition);

	//结婚 情侣数量
	m_chkReqMarried->SetValue(pQuestProto->complete_req_married);
	SET_TEXT_CTRL_VAL(m_textLover, complete_req_lover_num);

}

void QuestCompletePage::Save(tagQuestProto *pQuestProto)
{
	//物品
	GET_TEXT_CTRL_VAL_DWORD(m_textItem1, complete_req_item[0]);
	GET_TEXT_CTRL_VAL_INT(m_textItem1_Num, complete_req_item_num[0]);
	GET_TEXT_CTRL_VAL_DWORD(m_textItem2, complete_req_item[1]);
	GET_TEXT_CTRL_VAL_INT(m_textItem2_Num, complete_req_item_num[1]);
	GET_TEXT_CTRL_VAL_DWORD(m_textItem3, complete_req_item[2]);
	GET_TEXT_CTRL_VAL_INT(m_textItem3_Num, complete_req_item_num[2]);
	GET_TEXT_CTRL_VAL_DWORD(m_textItem4, complete_req_item[3]);
	GET_TEXT_CTRL_VAL_INT(m_textItem4_Num, complete_req_item_num[3]);
	GET_TEXT_CTRL_VAL_DWORD(m_textItem5, complete_req_item[4]);
	GET_TEXT_CTRL_VAL_INT(m_textItem5_Num, complete_req_item_num[4]);
	GET_TEXT_CTRL_VAL_DWORD(m_textItem6, complete_req_item[5]);
	GET_TEXT_CTRL_VAL_INT(m_textItem6_Num, complete_req_item_num[5]);
	GET_TEXT_CTRL_VAL_DWORD(m_textItem7, complete_req_item[6]);
	GET_TEXT_CTRL_VAL_INT(m_textItem7_Num, complete_req_item_num[6]);
	GET_TEXT_CTRL_VAL_DWORD(m_textItem8, complete_req_item[7]);
	GET_TEXT_CTRL_VAL_INT(m_textItem8_Num, complete_req_item_num[7]);
	GET_TEXT_CTRL_VAL_BOOL(m_chkOnlyOneItem,only_one_item)
	GET_TEXT_CTRL_VAL_BOOL(m_chkDeleteItem,delete_item)
	
	//怪
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature1, complete_req_creature[0]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature2, complete_req_creature[1]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature3, complete_req_creature[2]);
	GET_TEXT_CTRL_VAL_INT(m_textCreature1_Num, complete_req_creature_num[0]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature4, complete_req_creature[3]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature5, complete_req_creature[4]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature6, complete_req_creature[5]);
	GET_TEXT_CTRL_VAL_INT(m_textCreature2_Num, complete_req_creature_num[1]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature7, complete_req_creature[6]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature8, complete_req_creature[7]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature9, complete_req_creature[8]);
	GET_TEXT_CTRL_VAL_INT(m_textCreature3_Num, complete_req_creature_num[2]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature10, complete_req_creature[9]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature11, complete_req_creature[10]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature12, complete_req_creature[11]);
	GET_TEXT_CTRL_VAL_INT(m_textCreature4_Num, complete_req_creature_num[3]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature13, complete_req_creature[12]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature14, complete_req_creature[13]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature15, complete_req_creature[14]);
	GET_TEXT_CTRL_VAL_INT(m_textCreature5_Num, complete_req_creature_num[4]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature16, complete_req_creature[15]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature17, complete_req_creature[16]);
	GET_TEXT_CTRL_VAL_DWORD(m_textCreature18, complete_req_creature[17]);
	GET_TEXT_CTRL_VAL_INT(m_textCreature6_Num, complete_req_creature_num[5]);
	GET_TEXT_CTRL_VAL_BOOL(m_chkCreatureLevel,creature_level)
	GET_TEXT_CTRL_VAL_BOOL(m_chkOnlyOneCreature,only_one_creature)

	//NPC
	GET_TEXT_CTRL_VAL_DWORD(m_textNPC1, complete_req_npc[0]);
	GET_TEXT_CTRL_VAL_DWORD(m_textNPC2, complete_req_npc[1]);
	GET_TEXT_CTRL_VAL_DWORD(m_textNPC3, complete_req_npc[2]);
	GET_TEXT_CTRL_VAL_DWORD(m_textNPC4, complete_req_npc[3]);
	GET_TEXT_CTRL_VAL_DWORD(m_textNPC5, complete_req_npc[4]);
	GET_TEXT_CTRL_VAL_DWORD(m_textNPC6, complete_req_npc[5]);
	GET_TEXT_CTRL_VAL_DWORD(m_textNPC7, complete_req_npc[6]);
	GET_TEXT_CTRL_VAL_DWORD(m_textNPC8, complete_req_npc[7]);
	GET_TEXT_CTRL_VAL_BOOL(m_chkOnlyInOrder,only_in_order)
	GET_TEXT_CTRL_VAL_BOOL(m_chkOnlyOneNPC,only_one_npc)

	//触发器
	GET_TEXT_CTRL_VAL_DWORD(m_textTrigger1, complete_req_trigger[0]);
	GET_TEXT_CTRL_VAL_DWORD(m_textTrigger2, complete_req_trigger[1]);
	GET_TEXT_CTRL_VAL_DWORD(m_textTrigger3, complete_req_trigger[2]);
	GET_TEXT_CTRL_VAL_DWORD(m_textTrigger4, complete_req_trigger[3]);
	GET_TEXT_CTRL_VAL_DWORD(m_textTrigger5, complete_req_trigger[4]);
	GET_TEXT_CTRL_VAL_DWORD(m_textTrigger6, complete_req_trigger[5]);
	GET_TEXT_CTRL_VAL_DWORD(m_textTrigger7, complete_req_trigger[6]);
	GET_TEXT_CTRL_VAL_DWORD(m_textTrigger8, complete_req_trigger[7]);
	GET_TEXT_CTRL_VAL_BOOL(m_chkOnlyOneTrigger,only_one_trigger)
	
	//钱 等级
	GET_TEXT_CTRL_VAL_INT(m_textMoney_Num, complete_req_money);
	GET_TEXT_CTRL_VAL_BOOL(m_chkDelMoney,complete_del_money);
	GET_TEXT_CTRL_VAL_INT(m_textLevel, complete_req_level);

	//地图
	GET_TEXT_CTRL_VAL_DWORD(m_textMap, complete_req_map);
	GET_TEXT_CTRL_VAL_FLOAT(m_textX, complete_req_map_x, numx);
	GET_TEXT_CTRL_VAL_FLOAT(m_textY, complete_req_map_y, numy);
	GET_TEXT_CTRL_VAL_FLOAT(m_textZ, complete_req_map_z, numz);
	GET_TEXT_CTRL_VAL_FLOAT(m_textRadius, complete_req_map_radius, numrad);
	GET_TEXT_CTRL_VAL_BOOL(m_chkRandom,coordinate_random)

	//任务完成事件
	if (m_rabItem->GetValue() == true)
	{
		pQuestProto->event_type = EQCET_UseItem;
		GET_TEXT_CTRL_VAL_INT(m_textEvent_Item, use_item);
	}
	
	if (m_rabSkill->GetValue() == true)
	{
		pQuestProto->event_type = EQCET_UseSkill;
		GET_TEXT_CTRL_VAL_INT(m_textEvent_Skill, use_skill);
	}

	//任意一个条件
	GET_TEXT_CTRL_VAL_BOOL(m_chkOnlyOneCondition,only_one_condition)

	//结婚 情侣数量
	GET_TEXT_CTRL_VAL_BOOL(m_chkReqMarried,complete_req_married)
	GET_TEXT_CTRL_VAL_INT(m_textLover, complete_req_lover_num);
}