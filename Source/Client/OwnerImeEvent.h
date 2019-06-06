#pragma once

//ime显示
struct tagOwnerImeShow : public tagGameEvent
{
	BOOL bShow;
	tagOwnerImeShow():tagGameEvent(_T("tagOwnerImeShow"),NULL)
	{}
};

//ime输入数据
struct tagOwnerInputWordEvent : public tagGameEvent
{
	tstring strText;
	tagOwnerInputWordEvent():tagGameEvent(_T("tagOwnerInputWordEvent"),NULL)
	{}
};

//ime清空
struct tagOwnerInputWordClear : public tagGameEvent
{
	tagOwnerInputWordClear():tagGameEvent(_T("tagOwnerInputWordClear"),NULL)
	{}
};

//ime备选字
struct tagOwnerImeWordEvent : public tagGameEvent
{
	tstring		strText;
	tagOwnerImeWordEvent():tagGameEvent(_T("tagOwnerImeWordEvent"),NULL)
	{}
};