/////////////////////////////////////////////////////////////////////////////
// Program:     loong任务编辑器
// Name:        QuestAttPage.cpp
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
	QuestAttPage_QuestID = 100,
	QuestAttPage_QuestName,
	QuestAttPage_PrevQuest1ID,
	QuestAttPage_PrevQuest2ID,
	QuestAttPage_NextQuestID,
	QuestAttPage_UnCompleteQuestID,
	QuestAttPage_AutoAdd,
	QuestAttPage_ShowQuestDialog,
	QuestAttPage_NoDelete,
	QuestAttPage_PlayerAccept,
	QuestAttPage_SystemRelease,
	QuestAttPage_ItemAccept,
	QuestAttPage_Conditions,
	QuestAttPage_Overview,
	QuestAttPage_QuestType,
	QuestAttPage_SuggestedPlayers,
	QuestAttPage_DoQuestTimes,
	QuestAttPage_Repeatable,
	QuestAttPage_LimitTime_Hour,
	QuestAttPage_LimitTime_Min,
	QuestAttPage_LimitTime_Sec,
	QuestAttPage_Periodic,
	QuestAttPage_SrcItemID,
	QuestAttPage_SrcItemCount,
	QuestAttPage_Periodic_RadioBox,
	QuestAttPage_Destination_x,
	QuestAttPage_Destination_y,
	QuestAttPage_Destination_z,
};

enum QuestType
{
	QT_Beginer,	        // 新手任务
	QT_Gut,			    // 剧情任务
	QT_Skill,			// 技能任务
	QT_System,			// 系统任务(活动任务)
	QT_Commission,		// 玩家委托任务
	QT_Arrest,			// 通辑任务
	QT_Star,			// 星级任务(主线任务)
	QT_Guild,			// 门派任务
	QT_Profession,		// 职业任务
	QT_Study,			// 修炼任务
	QT_Lovers,			// 情侣任务
	QT_Legend,    		// 历险任务 
	QT_Clan,			// 氏族任务
	QT_Exchange,		// 兑换任务
};

enum Week
{
	Week_MON,
	Week_TUES,
	Week_WEDNES,
	Week_THURS,
	Week_FRI,
	Week_SAT,
	Week_SUN
};

enum QuestPeriodic
{
	QuestPeriodic_DAY,
	QuestPeriodic_WEEK
};

// ----------------------------------------------------------------------------
// 任务属性
// ----------------------------------------------------------------------------
class QuestAttPage: public MyPage
{
public:
	QuestAttPage(wxBookCtrlBase *book);
	//更新数据
	void Update(tagQuestProto *pQuestProto);
	//保存数据
	void Save(tagQuestProto *pQuestProto);

	INT16 GetQuestID() { return wxAtoi(m_textID->GetValue()); }

	static MyPageInfo ms_infoQuestAttPage;

	void OnPeriodicCheckBox(wxCommandEvent& event);

	void OnPeriodicRadioBox(wxCommandEvent& event);

protected:
	//TextCtrl
	wxTextCtrl *m_textID;
	wxTextCtrl *m_textName;
	wxTextCtrl *m_textPrev1ID;
	wxTextCtrl *m_textPrev2ID;
	wxTextCtrl *m_textNextID;
	wxTextCtrl *m_textUnCompleteID;
	wxTextCtrl *m_textHour;
	wxTextCtrl *m_textMin;
	wxTextCtrl *m_textSec;
	wxTextCtrl *m_textConditions;
	wxTextCtrl *m_textOverview;
	wxTextCtrl *m_textAccept_ID1;
	wxTextCtrl *m_textAccept_ID2;
	wxTextCtrl *m_textAccept_ID3;
	wxTextCtrl *m_textAccept_ID4;
	wxTextCtrl *m_textAccept_ID5;
	wxTextCtrl *m_textAccept_ID6;
	wxTextCtrl *m_textComplete_ID1;
	wxTextCtrl *m_textComplete_ID2;
	wxTextCtrl *m_textComplete_ID3;
	wxTextCtrl *m_textComplete_ID4;
	wxTextCtrl *m_textComplete_ID5;
	wxTextCtrl *m_textComplete_ID6;
	wxTextCtrl *m_textSrcItemID;
	wxTextCtrl *m_textSrcItemCount;
	wxTextCtrl *m_textSuggestedPlayers;
	wxTextCtrl *m_textDoQuestTimes;
	wxTextCtrl *m_textDestination_x;
	wxTextCtrl *m_textDestination_y;
	wxTextCtrl *m_textDestination_z;
	wxTextCtrl *m_textInvestigationalObject_ID1;
	wxTextCtrl *m_textInvestigationalObject_ID2;
	wxTextCtrl *m_textInvestigationalObject_ID3;
	wxTextCtrl *m_textInvestigationalObject_ID4;
	wxTextCtrl *m_textInvestigationalObject_ID5;
	wxTextCtrl *m_textInvestigationalObject_ID6;
	wxTextCtrl *m_textInvestigationalObject_ID7;
	wxTextCtrl *m_textInvestigationalObject_ID8;

	//CheckBox
	wxCheckBox *m_chkAutoAdd;
	wxCheckBox *m_chkShowQuestDialog;
	wxCheckBox *m_chkNoDelete;
	wxCheckBox *m_chkPlayerAccept;
	wxCheckBox *m_chkSystemRelease;
	wxCheckBox *m_chkItemAccept;
	wxCheckBox *m_chkPeriodic;
	wxCheckBox *m_chkRepeatable;

	//wxRadioBox
	 wxRadioBox *m_radioPeriodic;
	 wxRadioBox *m_radioWeek;
	 wxListBox  *m_listType;
	 wxRadioBox *m_radioPrevQuestRelation;
	 wxRadioBox *m_radioTargetMode;
private:
	 DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// 巨大的构造函数
// ----------------------------------------------------------------------------
QuestAttPage::QuestAttPage(wxBookCtrlBase *book) : MyPage(book)
{
	wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);
	wxSizer *sizerLeft = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizerMiddle = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);

	//布局-左

	//任务基本参数
	wxStaticBox *box = new wxStaticBox(this, wxID_ANY, _T("任务基本参数"));
	wxSizer *sizer_arg = new wxStaticBoxSizer(box, wxVERTICAL);

	wxSizer *sizerRow;
	
	sizerRow = CreateSizerWithTextAndLabel(_T("ID"), QuestAttPage_QuestID,&m_textID);
	sizer_arg->Add(sizerRow, 0, wxALL , 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("名称"), QuestAttPage_QuestName,&m_textName);
	sizer_arg->Add(sizerRow, 0, wxALL , 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("前序任务1ID"), QuestAttPage_PrevQuest1ID,&m_textPrev1ID);
	sizer_arg->Add(sizerRow, 0, wxALL , 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("前序任务2ID"), QuestAttPage_PrevQuest2ID,&m_textPrev2ID);
	sizer_arg->Add(sizerRow, 0, wxALL, 5);
	static const wxString relation[] =
	{
		_T("必须全部完成"),
		_T("完成其中一个就可以"),
	};
	m_radioPrevQuestRelation = new wxRadioBox(this, wxID_ANY, _T("前序任务之间关系"), wxDefaultPosition, wxDefaultSize,WXSIZEOF(relation), relation,
		2, wxRA_SPECIFY_COLS);
	sizer_arg->Add(m_radioPrevQuestRelation,0,wxALL, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("后续任务ID"), QuestAttPage_NextQuestID,&m_textNextID);
	sizer_arg->Add(sizerRow, 0, wxALL, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("未完成任务ID"),QuestAttPage_UnCompleteQuestID, &m_textUnCompleteID);
	sizer_arg->Add(sizerRow, 0, wxALL , 5);
	m_chkAutoAdd = CreateCheckBoxAndAddToSizer(sizer_arg,_T("后续任务是否自动加载到玩家身上"), QuestAttPage_AutoAdd);
	m_chkShowQuestDialog = CreateCheckBoxAndAddToSizer(sizer_arg,_T("接取任务后弹出任务界面"), QuestAttPage_ShowQuestDialog);
	m_chkNoDelete = CreateCheckBoxAndAddToSizer(sizer_arg,_T("任务不能被删除"), QuestAttPage_NoDelete);

	//ID不能更改
	//m_textID->Enable(false);

	//周期性任务
	wxStaticBox *periodic_box = new wxStaticBox(this, wxID_ANY, _T("周期性任务"));
	wxSizer *sizer_periodic_box = new wxStaticBoxSizer(periodic_box, wxVERTICAL);
	m_chkPeriodic = CreateCheckBoxAndAddToSizer(sizer_periodic_box,_T("周期性任务"),QuestAttPage_Periodic);
	static const wxString kind1s[] =
	{
		_T("每日"),
		_T("每周"),
	};
	m_radioPeriodic = new wxRadioBox(this, QuestAttPage_Periodic_RadioBox, _T("周期"), wxDefaultPosition, wxDefaultSize,WXSIZEOF(kind1s), kind1s,
		2, wxRA_SPECIFY_COLS);
	sizer_periodic_box->Add(m_radioPeriodic,0,wxGROW | wxALL, 5);
	m_radioPeriodic->Enable(false);

	static const wxString kind2s[] =
	{
		_T("日"),
		_T("一"),
		_T("二"),
		_T("三"),
		_T("四"),
		_T("五"),
		_T("六"),
	};
	m_radioWeek = new wxRadioBox(this, wxID_ANY, _T("星期"), wxDefaultPosition, wxDefaultSize,WXSIZEOF(kind2s), kind2s,
		2, wxRA_SPECIFY_ROWS);
	sizer_periodic_box->Add(m_radioWeek,0,wxGROW | wxALL, 5);
	m_radioWeek->Enable(false);

	wxSizer *sizer_tmp = new wxBoxSizer(wxHORIZONTAL);
	//任务接取类型
	wxStaticBox *type_box = new wxStaticBox(this, wxID_ANY, _T("任务接取类型"));
	wxSizer *sizer_type_box = new wxStaticBoxSizer(type_box, wxVERTICAL);
	m_chkPlayerAccept = CreateCheckBoxAndAddToSizer(sizer_type_box,_T("玩家主动接取"), QuestAttPage_PlayerAccept);
	m_chkItemAccept = CreateCheckBoxAndAddToSizer(sizer_type_box,_T("物品接取"),QuestAttPage_ItemAccept);
	m_chkSystemRelease = CreateCheckBoxAndAddToSizer(sizer_type_box,_T("系统发布"), QuestAttPage_SystemRelease);
	sizer_tmp->Add(sizer_type_box,0,wxALL | wxGROW, 5);

	//任务目标类型
	static const wxString mode[] =
	{
		_T("静态目标"),
		_T("动态目标"),
	};
	m_radioTargetMode = new wxRadioBox(this, wxID_ANY, _T("任务目标类型"), wxDefaultPosition, wxDefaultSize,WXSIZEOF(mode), mode,
		2, wxRA_SPECIFY_COLS);
	sizer_tmp->Add(m_radioTargetMode,1,wxALL, 5);

	sizerLeft->Add(sizer_arg,0,wxALL | wxGROW, 5);
	sizerLeft->Add(sizer_periodic_box,0,wxALL | wxGROW, 5);
	sizerLeft->Add(sizer_tmp,0,wxALL | wxGROW, 5);

	
	
	//布局-中
	//任务信息
	wxStaticBox *info_box = new wxStaticBox(this, wxID_ANY, _T("任务信息"));
	wxSizer *sizer_info_box = new wxStaticBoxSizer(info_box, wxVERTICAL);
	wxSizer *sizer_Col = new wxBoxSizer(wxVERTICAL);
	m_textConditions = new wxTextCtrl(this, QuestAttPage_Conditions, wxEmptyString, wxDefaultPosition, wxSize(190,40), wxTE_PROCESS_ENTER|wxTE_MULTILINE);
	sizer_Col->Add(new wxStaticText(this,wxID_ANY, _T("任务条件")),0,wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	sizer_Col->Add(m_textConditions,0,wxLEFT|wxGROW|wxALIGN_CENTER_VERTICAL, 5);
	
	m_textOverview = new wxTextCtrl(this, QuestAttPage_Overview, wxEmptyString, wxDefaultPosition, wxSize(190,80), wxTE_PROCESS_ENTER|wxTE_MULTILINE);
	sizer_Col->Add(new wxStaticText(this,wxID_ANY, _T("任务概述")),0,wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	sizer_Col->Add(m_textOverview,1,wxLEFT|wxGROW|wxALIGN_CENTER_VERTICAL, 5);

	sizer_info_box->Add(sizer_Col, 1, wxALL | wxGROW, 5);
	//接取任务NPC
	wxStaticBox *accept_box = new wxStaticBox(this, wxID_ANY, _T("接取任务NPC"));
	wxSizer *sizer_accept_box = new wxStaticBoxSizer(accept_box, wxVERTICAL);
	
	m_textAccept_ID1 = CreateInfoText();
	m_textAccept_ID2 = CreateInfoText();
	m_textAccept_ID3 = CreateInfoText();
	m_textAccept_ID4 = CreateInfoText();
	m_textAccept_ID5 = CreateInfoText();
	m_textAccept_ID6 = CreateInfoText();
	sizer_accept_box->Add(CreateTextWithLabelSizer(_T("ID1"), m_textAccept_ID1, _T("ID2"), m_textAccept_ID2), 1, wxGROW|wxALL, 5);
	sizer_accept_box->Add(CreateTextWithLabelSizer(_T("ID3"), m_textAccept_ID3, _T("ID4"), m_textAccept_ID4), 1, wxGROW|wxALL, 5);
	sizer_accept_box->Add(CreateTextWithLabelSizer(_T("ID5"), m_textAccept_ID5, _T("ID6"), m_textAccept_ID6), 1, wxGROW|wxALL, 5);

	//完成任务NPC
	wxStaticBox *complete_box = new wxStaticBox(this, wxID_ANY, _T("完成任务NPC"));
	wxSizer *sizer_complete_box = new wxStaticBoxSizer(complete_box, wxVERTICAL);
	m_textComplete_ID1 = CreateInfoText();
	m_textComplete_ID2 = CreateInfoText();
	m_textComplete_ID3 = CreateInfoText();
	m_textComplete_ID4 = CreateInfoText();
	m_textComplete_ID5 = CreateInfoText();
	m_textComplete_ID6 = CreateInfoText();
	sizer_complete_box->Add(CreateTextWithLabelSizer(_T("ID1"), m_textComplete_ID1, _T("ID2"), m_textComplete_ID2), 1, wxGROW|wxALL, 5);
	sizer_complete_box->Add(CreateTextWithLabelSizer(_T("ID3"), m_textComplete_ID3, _T("ID4"), m_textComplete_ID4), 1, wxGROW|wxALL, 5);
	sizer_complete_box->Add(CreateTextWithLabelSizer(_T("ID5"), m_textComplete_ID5, _T("ID6"), m_textComplete_ID6), 1, wxGROW|wxALL, 5);

	//任务限制时间
	wxStaticBox *time_box = new wxStaticBox(this, wxID_ANY, _T("任务限制时间"));
	wxSizer *sizer_time_box = new wxStaticBoxSizer(time_box, wxVERTICAL);
	m_textHour = CreateInfoText(QuestAttPage_LimitTime_Hour);
	m_textMin = CreateInfoText(QuestAttPage_LimitTime_Min);
	m_textSec = CreateInfoText(QuestAttPage_LimitTime_Sec);
	sizer_time_box->Add(CreateTextWithLabelSizer(_T("小时"), m_textHour, _T("分"), m_textMin, _T("秒"),m_textSec), 1, wxGROW|wxALL, 5);

	sizerMiddle->Add(sizer_info_box,1,wxGROW|wxALL, 5);
	sizerMiddle->Add(sizer_accept_box,0,wxGROW | wxALL, 5);
	sizerMiddle->Add(sizer_complete_box,0,wxGROW | wxALL, 5);
	sizerMiddle->Add(sizer_time_box,0,wxALL | wxGROW, 5);

	//布局-右
	//任务类型
	static const wxString kinds[] =
	{
		_T("无"),
		_T("新手任务"),
		_T("生产任务"),
		_T("活动任务"),
		_T("系统任务"),
		_T("剧情任务"),
		_T("职业任务"),
		_T("情侣任务"),
		_T("帮派任务"),
		_T("奇遇任务"),
		_T("挖宝任务"),
		_T("夫妻任务"),
		_T("挑战任务"),
		_T("氏族任务"),
		_T("兑换任务")
	};//WXSIZEOF(kinds), kinds,wxDefaultSize
	m_listType = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(120,140),0,NULL,
		wxLB_SINGLE | wxLB_ALWAYS_SB | wxHSCROLL );
	QuestTypeArray& questType = QuestMgr::getSingleton().GetQuestType();
	for( UINT i=0; i<questType.size(); i++ )
	{
		m_listType->Append(questType[i].second);
	}
	//wxSizer *sizer_quest_type_box = new wxStaticBoxSizer(quest_type_box, wxVERTICAL);

	//任务参数
	wxStaticBox *arg_box = new wxStaticBox(this, wxID_ANY, _T("任务参数"));
	wxSizer *sizer_arg_box = new wxStaticBoxSizer(arg_box, wxVERTICAL);
	int s_maxWidth;
	GetTextExtent(_T("999"), &s_maxWidth, NULL);
	sizerRow = CreateSizerWithTextAndLabel(_T("建议玩家个数"), QuestAttPage_SuggestedPlayers,&m_textSuggestedPlayers,wxHORIZONTAL,wxSize(s_maxWidth, wxDefaultCoord));
	sizer_arg_box->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("每个玩家接取任务次数"), QuestAttPage_DoQuestTimes,&m_textDoQuestTimes,wxHORIZONTAL,wxSize(s_maxWidth, wxDefaultCoord));
	sizer_arg_box->Add(sizerRow, 1, wxALL | wxGROW, 5);
	m_chkRepeatable = CreateCheckBoxAndAddToSizer(sizer_arg_box,_T("任务可以重复接取"),QuestAttPage_Repeatable);

	//任务目的地坐标
	wxStaticBox *des_box = new wxStaticBox(this, wxID_ANY, _T("任务适合等级"));
	wxSizer *sizer_des_box = new wxStaticBoxSizer(des_box, wxVERTICAL);
	sizerRow = CreateSizerWithTextAndLabel(_T("等级"), QuestAttPage_Destination_x,&m_textDestination_x);
	sizer_des_box->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("Y"), QuestAttPage_Destination_y,&m_textDestination_y);
	sizer_des_box->Add(sizerRow, 1, wxALL | wxGROW, 5);
	sizerRow = CreateSizerWithTextAndLabel(_T("Z"), QuestAttPage_Destination_z,&m_textDestination_z);
	sizer_des_box->Add(sizerRow, 1, wxALL | wxGROW, 5);

	//可调查地物ID
	wxStaticBox *investigational_object_box = new wxStaticBox(this, wxID_ANY, _T("可调查地物"));
	wxSizer *sizer_investigational_object_box = new wxStaticBoxSizer(investigational_object_box, wxVERTICAL);

	m_textInvestigationalObject_ID1 = CreateInfoText();
	m_textInvestigationalObject_ID2 = CreateInfoText();
	m_textInvestigationalObject_ID3 = CreateInfoText();
	m_textInvestigationalObject_ID4 = CreateInfoText();
	m_textInvestigationalObject_ID5 = CreateInfoText();
	m_textInvestigationalObject_ID6 = CreateInfoText();
	m_textInvestigationalObject_ID7 = CreateInfoText();
	m_textInvestigationalObject_ID8 = CreateInfoText();

	sizer_investigational_object_box->Add(CreateTextWithLabelSizer(_T("ID1"), m_textInvestigationalObject_ID1, 
										_T("ID2"), m_textInvestigationalObject_ID2), 1, wxGROW|wxALL, 5);
	sizer_investigational_object_box->Add(CreateTextWithLabelSizer(_T("ID3"), m_textInvestigationalObject_ID3, 
										_T("ID4"), m_textInvestigationalObject_ID4), 1, wxGROW|wxALL, 5);
	sizer_investigational_object_box->Add(CreateTextWithLabelSizer(_T("ID5"), m_textInvestigationalObject_ID5,
										_T("ID6"), m_textInvestigationalObject_ID6), 1, wxGROW|wxALL, 5);
	sizer_investigational_object_box->Add(CreateTextWithLabelSizer(_T("ID7"), m_textInvestigationalObject_ID7,
										_T("ID8"), m_textInvestigationalObject_ID8), 1, wxGROW|wxALL, 5);
	
	//任务物品
	wxStaticBox *item_box = new wxStaticBox(this, wxID_ANY, _T("任务物品"));
	wxSizer *sizer_item_box = new wxStaticBoxSizer(item_box, wxVERTICAL);
	m_textSrcItemID = CreateInfoText();
	m_textSrcItemCount = CreateInfoText();
	sizer_item_box->Add(CreateTextWithLabelSizer(_T("ID"), m_textSrcItemID, _T("数量"), m_textSrcItemCount), 1, wxGROW|wxALL, 5);
	
	sizerRight->Add(m_listType,0,wxGROW | wxALL, 5);
	sizerRight->Add(sizer_arg_box,0,wxGROW | wxALL, 5);
	sizerRight->Add(sizer_des_box,0,wxGROW | wxALL, 5);
	sizerRight->Add(sizer_investigational_object_box,0,wxGROW | wxALL, 5);
	sizerRight->Add(sizer_item_box,0,wxGROW | wxALL, 5);

	//布局-总
	sizerTop->Add(sizerLeft,1,wxGROW | wxALL, 5);
	sizerTop->Add(sizerMiddle,1,wxGROW | wxALL, 5);
	sizerTop->Add(sizerRight,1,wxGROW | wxALL, 5);
	
	SetSizer(sizerTop);
	sizerTop->Fit(this);
}

MyPage *wxCtorForQuestAttPage(wxBookCtrlBase *book)
{
	return new QuestAttPage(book);
}

MyPageInfo QuestAttPage::ms_infoQuestAttPage(wxCtorForQuestAttPage, _T("任务属性"));

void QuestAttPage::Update(tagQuestProto *pQuestProto)
{
	//id
	SET_TEXT_CTRL_VAL(m_textID, id);
	//前序1ID
	SET_TEXT_CTRL_VAL(m_textPrev1ID, prev_quest_id);
	//前序2ID
	SET_TEXT_CTRL_VAL(m_textPrev2ID, prev_quest2_id);
	//前序任务之间的关系
	m_radioPrevQuestRelation->SetSelection(pQuestProto->prev_quest_relation);

	//后续ID
	SET_TEXT_CTRL_VAL(m_textNextID, next_quest_id);
	//未完成任务ID
	SET_TEXT_CTRL_VAL(m_textUnCompleteID, uncomplete_quest_id);

	//自动加载
	m_chkAutoAdd->SetValue((bool)pQuestProto->auto_add_quest);
	//接取任务后是否弹出任务界面
	m_chkShowQuestDialog->SetValue((bool)pQuestProto->show_quest_dialog);
	//任务不能被删除
	m_chkNoDelete->SetValue((bool)pQuestProto->no_delete);

	//任务接取标志
	m_chkPlayerAccept->SetValue(pQuestProto->accept_flags & Quest_Accept_Flag_Player);
	m_chkSystemRelease->SetValue(pQuestProto->accept_flags & Quest_Accept_Flag_System);
	m_chkItemAccept->SetValue(pQuestProto->accept_flags & Quest_Accept_Flag_Item);

	//任务目标类型
	m_radioTargetMode->SetSelection(pQuestProto->target_mode);

	//任务类型	
	QuestTypeArray& questType = QuestMgr::getSingleton().GetQuestType();
	m_listType->DeselectAll();
	for( UINT i=0; i<questType.size(); i++ )
	{
		if( questType[i].first == pQuestProto->type && m_listType->GetCount() > i )
		{
			m_listType->SetSelection(i);
			break;
		}
	}

	//建议玩家个数
	SET_TEXT_CTRL_VAL(m_textSuggestedPlayers, suggested_players);

	//任务完成的限制时间
	//小时
	INT32 time = pQuestProto->limit_time / 3600;
	SET_TEXT_CTRL_TIME(m_textHour, time);
	time = (pQuestProto->limit_time % 3600) / 60;
	SET_TEXT_CTRL_TIME(m_textMin, time);
	time = (pQuestProto->limit_time % 3600) % 60;
	SET_TEXT_CTRL_TIME(m_textSec, time);
	//每个玩家接取任务次数
	SET_TEXT_CTRL_VAL(m_textDoQuestTimes, accept_times);

	//是否是周期性任务
	m_chkPeriodic->SetValue((bool)pQuestProto->period);
	if (pQuestProto->period == TRUE)
	{
		//任务周期类别
		m_radioPeriodic->Enable(true);
		m_radioPeriodic->SetSelection(pQuestProto->period_type);
		if(pQuestProto->period_type == EQuestPeriodic_WEEK)
		{
			//星期
			m_radioWeek->Enable(true);
			m_radioWeek->SetSelection(pQuestProto->week);
		}
		else
		{
			m_radioWeek->SetSelection(0);
			m_radioWeek->Enable(false);
		}
	}
	else
	{
		m_radioPeriodic->SetSelection(0);
		m_radioWeek->SetSelection(0);
		m_radioPeriodic->Enable(false);
		m_radioWeek->Enable(false);
	}
	//任务是否可以重复
	m_chkRepeatable->SetValue((bool)pQuestProto->repeatable);

	//接取任务NPC
	SET_TEXT_CTRL_VAL_DWORD(m_textAccept_ID1, accept_quest_npc[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textAccept_ID2, accept_quest_npc[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textAccept_ID3, accept_quest_npc[2]);
	SET_TEXT_CTRL_VAL_DWORD(m_textAccept_ID4, accept_quest_npc[3]);
	SET_TEXT_CTRL_VAL_DWORD(m_textAccept_ID5, accept_quest_npc[4]);
	SET_TEXT_CTRL_VAL_DWORD(m_textAccept_ID6, accept_quest_npc[5]);

	//完成任务NPC
	SET_TEXT_CTRL_VAL_DWORD(m_textComplete_ID1, complete_quest_npc[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textComplete_ID2, complete_quest_npc[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textComplete_ID3, complete_quest_npc[2]);
	SET_TEXT_CTRL_VAL_DWORD(m_textComplete_ID4, complete_quest_npc[3]);
	SET_TEXT_CTRL_VAL_DWORD(m_textComplete_ID5, complete_quest_npc[4]);
	SET_TEXT_CTRL_VAL_DWORD(m_textComplete_ID6, complete_quest_npc[5]);

	//可调查地物
	SET_TEXT_CTRL_VAL_DWORD(m_textInvestigationalObject_ID1, investigate_objects[0]);
	SET_TEXT_CTRL_VAL_DWORD(m_textInvestigationalObject_ID2, investigate_objects[1]);
	SET_TEXT_CTRL_VAL_DWORD(m_textInvestigationalObject_ID3, investigate_objects[2]);
	SET_TEXT_CTRL_VAL_DWORD(m_textInvestigationalObject_ID4, investigate_objects[3]);
	SET_TEXT_CTRL_VAL_DWORD(m_textInvestigationalObject_ID5, investigate_objects[4]);
	SET_TEXT_CTRL_VAL_DWORD(m_textInvestigationalObject_ID6, investigate_objects[5]);
	SET_TEXT_CTRL_VAL_DWORD(m_textInvestigationalObject_ID7, investigate_objects[6]);
	SET_TEXT_CTRL_VAL_DWORD(m_textInvestigationalObject_ID8, investigate_objects[7]);

	//任务发布者给的任务物品
	SET_TEXT_CTRL_VAL_DWORD(m_textSrcItemID, src_item);
	//任务发布者给的任务物品数量
	SET_TEXT_CTRL_VAL(m_textSrcItemCount, src_item_num);

	//任务名称
	SET_TEXT_CTRL_VAL(m_textName, name);
	//任务条件
	SET_TEXT_CTRL_VAL(m_textConditions, conditions);
	//任务概述
	SET_TEXT_CTRL_VAL(m_textOverview, over_view);

	//任务目的地坐标
	SET_TEXT_CTRL_VAL_FLOAT(m_textDestination_x, destination_x);
	SET_TEXT_CTRL_VAL_FLOAT(m_textDestination_y, destination_y);
	SET_TEXT_CTRL_VAL_FLOAT(m_textDestination_z, destination_z);
}

void QuestAttPage::Save(tagQuestProto *pQuestProto)
{
	//id
	GET_TEXT_CTRL_VAL_INT(m_textID, id);
	//前序1ID
	GET_TEXT_CTRL_VAL_INT(m_textPrev1ID, prev_quest_id);
	//前序2ID
	GET_TEXT_CTRL_VAL_INT(m_textPrev2ID, prev_quest2_id);
	//前序任务之间的关系
	pQuestProto->prev_quest_relation = m_radioPrevQuestRelation->GetSelection();
	//后续ID
	GET_TEXT_CTRL_VAL_INT(m_textNextID, next_quest_id);
	//未完成任务ID
	GET_TEXT_CTRL_VAL_INT(m_textUnCompleteID, uncomplete_quest_id);
	//自动加载
	GET_TEXT_CTRL_VAL_BOOL(m_chkAutoAdd,auto_add_quest);
	//接取任务后是否弹出任务界面
	GET_TEXT_CTRL_VAL_BOOL(m_chkShowQuestDialog,show_quest_dialog);
	//任务不能被删除
	GET_TEXT_CTRL_VAL_BOOL(m_chkNoDelete,no_delete);

	//任务接取标志
	if (m_chkPlayerAccept->GetValue() == true)
		pQuestProto->accept_flags = pQuestProto->accept_flags | Quest_Accept_Flag_Player;
	else
		pQuestProto->accept_flags = pQuestProto->accept_flags & ~Quest_Accept_Flag_Player;

	if (m_chkItemAccept->GetValue() == true)
		pQuestProto->accept_flags = pQuestProto->accept_flags | Quest_Accept_Flag_Item;
	else
		pQuestProto->accept_flags = pQuestProto->accept_flags & ~Quest_Accept_Flag_Item;
											
	if (m_chkSystemRelease->GetValue() == true)
		pQuestProto->accept_flags = pQuestProto->accept_flags | Quest_Accept_Flag_System;
	else
		pQuestProto->accept_flags = pQuestProto->accept_flags & ~Quest_Accept_Flag_System;

	//任务目标类型
	pQuestProto->target_mode = (EQuestTargetMode)m_radioTargetMode->GetSelection();

	//任务类型
	QuestTypeArray& questType = QuestMgr::getSingleton().GetQuestType();
	int index = m_listType->GetSelection();
	if(index >= 0 && index < (int)questType.size())
		pQuestProto->type = questType[index].first;

	//建议玩家个数
	GET_TEXT_CTRL_VAL_INT(m_textSuggestedPlayers, suggested_players);

	//任务完成的限制时间
	pQuestProto->limit_time = wxAtoi(m_textHour->GetValue()) * 3600 +  wxAtoi(m_textMin->GetValue()) * 60 + wxAtoi(m_textSec->GetValue());
	
	//每个玩家接取任务次数
	GET_TEXT_CTRL_VAL_INT(m_textDoQuestTimes, accept_times)

	//是否是周期性任务
	GET_TEXT_CTRL_VAL_BOOL(m_chkPeriodic,period)

	//任务周期类别
	pQuestProto->period_type = (EQuestPeriodic)m_radioPeriodic->GetSelection();
	//星期
	pQuestProto->week = (EWeek)m_radioWeek->GetSelection();

	//任务是否可以重复
	GET_TEXT_CTRL_VAL_BOOL(m_chkRepeatable,repeatable)

	//接取任务NPC
	GET_TEXT_CTRL_VAL_DWORD(m_textAccept_ID1, accept_quest_npc[0]);
	GET_TEXT_CTRL_VAL_DWORD(m_textAccept_ID2, accept_quest_npc[1]);
	GET_TEXT_CTRL_VAL_DWORD(m_textAccept_ID3, accept_quest_npc[2]);
	GET_TEXT_CTRL_VAL_DWORD(m_textAccept_ID4, accept_quest_npc[3]);
	GET_TEXT_CTRL_VAL_DWORD(m_textAccept_ID5, accept_quest_npc[4]);
	GET_TEXT_CTRL_VAL_DWORD(m_textAccept_ID6, accept_quest_npc[5]);

	//完成任务NPC
	GET_TEXT_CTRL_VAL_DWORD(m_textComplete_ID1, complete_quest_npc[0]);
	GET_TEXT_CTRL_VAL_DWORD(m_textComplete_ID2, complete_quest_npc[1]);
	GET_TEXT_CTRL_VAL_DWORD(m_textComplete_ID3, complete_quest_npc[2]);
	GET_TEXT_CTRL_VAL_DWORD(m_textComplete_ID4, complete_quest_npc[3]);
	GET_TEXT_CTRL_VAL_DWORD(m_textComplete_ID5, complete_quest_npc[4]);
	GET_TEXT_CTRL_VAL_DWORD(m_textComplete_ID6, complete_quest_npc[5]);

	//可调查地物
	GET_TEXT_CTRL_VAL_DWORD(m_textInvestigationalObject_ID1, investigate_objects[0]);
	GET_TEXT_CTRL_VAL_DWORD(m_textInvestigationalObject_ID2, investigate_objects[1]);
	GET_TEXT_CTRL_VAL_DWORD(m_textInvestigationalObject_ID3, investigate_objects[2]);
	GET_TEXT_CTRL_VAL_DWORD(m_textInvestigationalObject_ID4, investigate_objects[3]);
	GET_TEXT_CTRL_VAL_DWORD(m_textInvestigationalObject_ID5, investigate_objects[4]);
	GET_TEXT_CTRL_VAL_DWORD(m_textInvestigationalObject_ID6, investigate_objects[5]);
	GET_TEXT_CTRL_VAL_DWORD(m_textInvestigationalObject_ID7, investigate_objects[6]);
	GET_TEXT_CTRL_VAL_DWORD(m_textInvestigationalObject_ID8, investigate_objects[7]);

	//任务发布者给的任务物品
	GET_TEXT_CTRL_VAL_DWORD(m_textSrcItemID, src_item);
	//任务发布者给的任务物品数量
	GET_TEXT_CTRL_VAL_INT(m_textSrcItemCount, src_item_num);

	//任务名称
	{
		wxString str = m_textName->GetValue();
		size_t len = str.Len();
		if (len >= X_SHORT_NAME)
			len = X_SHORT_NAME - 1;
		const wxChar* chars = str.c_str();
		for (size_t i = 0; i < len; i++)
			pQuestProto->name[i] = chars[i];
		pQuestProto->name[len] = _T('\0');
	}

	//任务条件
	{
		wxString str = m_textConditions->GetValue();
		size_t len = str.Len();
		if (len >= X_SHORT_NAME)
			len = X_SHORT_NAME - 1;
		const wxChar* chars = str.c_str();
		for (size_t i = 0; i < len; i++)
			pQuestProto->conditions[i] = chars[i];
		pQuestProto->conditions[len] = _T('\0');
	}
	//任务概述
	{
		wxString str = m_textOverview->GetValue();
		size_t len = str.Len();
		if (len >= QUEST_OVERVIEW)
			len = QUEST_OVERVIEW - 1;
		const wxChar* chars = str.c_str();
		for (size_t i = 0; i < len; i++)
			pQuestProto->over_view[i] = chars[i];
		pQuestProto->over_view[len] = _T('\0');
	}

	//任务目的地坐标
	GET_TEXT_CTRL_VAL_FLOAT(m_textDestination_x, destination_x, numx);
	GET_TEXT_CTRL_VAL_FLOAT(m_textDestination_y, destination_y, numy);
	GET_TEXT_CTRL_VAL_FLOAT(m_textDestination_z, destination_z, numz);
}

void QuestAttPage::OnPeriodicCheckBox(wxCommandEvent &event)
{
	if(m_chkPeriodic->GetValue() == true)
	{
		m_radioPeriodic->Enable(true);
	}
	else
	{
		m_radioPeriodic->Enable(false);
	}
}

void QuestAttPage::OnPeriodicRadioBox(wxCommandEvent &event)
{
	if (m_radioPeriodic->GetSelection() == 1)
	{
		m_radioWeek->Enable(true);
	}
	else
	{
		m_radioWeek->Enable(false);
	}
}

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(QuestAttPage, MyPage)
	EVT_CHECKBOX(QuestAttPage_Periodic, QuestAttPage::OnPeriodicCheckBox)
	EVT_RADIOBOX(QuestAttPage_Periodic_RadioBox, QuestAttPage::OnPeriodicRadioBox)
END_EVENT_TABLE()