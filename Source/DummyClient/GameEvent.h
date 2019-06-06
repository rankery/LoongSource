#pragma once

enum EGameEvent
{
	EGE_Move,
	EGE_Drop,
	EGE_VDrop,
	EGE_Jump,
	EGE_Slide,
	EGE_Swim,
	EGE_StandOnCarrier,
	EGE_MoveOnCarrier
};

struct tagGameEvent
{
	EGameEvent event;
};

struct tagMoveEvent : public tagGameEvent
{
	tagMoveEvent()
	{
		event=EGE_Move;
	}
	Vector3 start;
	Vector3 end;
	DWORD dwCarrierObjID;
};

struct tagDropEvent : public tagGameEvent
{
	tagDropEvent()
	{
		event=EGE_Drop;
	}
	Vector3 start;
	Vector3 xzDir;
	float xzSpeed;
	float ySpeed;
	float startTime;
};

struct tagVDropEvent : public tagGameEvent
{
	tagVDropEvent()
	{
		event=EGE_VDrop;
	}
	Vector3 start;
	Vector3 xzDir;
	float xzSpeed;
	float ySpeed;
	float startTime;
};

struct tagJumpEvent : public tagGameEvent
{
	tagJumpEvent()
	{
		event=EGE_Jump;
	}
	Vector3 start;
	Vector3 xzDir;
	float xzSpeed;
	float ySpeed;
};

struct tagSlideEvent : public tagGameEvent
{
	tagSlideEvent()
	{
		event=EGE_Slide;
	}
	Vector3 start;
};

struct tagSwimEvent : public tagGameEvent
{
	tagSwimEvent()
	{
		event=EGE_Swim;
	}
	Vector3 start;
	Vector3 end;
};

struct tagStandOnCarrierEvent : public tagGameEvent
{
	tagStandOnCarrierEvent()
	{
		event=EGE_StandOnCarrier;
	}
	DWORD dwCarrierObjID;
	float startTime;
};
