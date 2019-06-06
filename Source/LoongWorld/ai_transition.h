//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: ai_transition.h
// author: wjiang
// actor:
// data: 2009-5-31
// last:
// brief: AI×´Ì¬×ª»»Æ÷
//-------------------------------------------------------------------------------------------------------
#pragma once
enum AIStateType;
class Creature;
class AIController;

//-----------------------------------------------------------------------------
// ×´Ì¬×ª»»Æ÷½Ó¿Ú
//-----------------------------------------------------------------------------
class AITransition
{
public:
	AITransition() {}
	virtual ~AITransition() {}

	virtual AIStateType Transition(AIController* pAI, AIStateType eCurState) = 0;
};

//-----------------------------------------------------------------------------
// ¹¥»÷ÐÍ×´Ì¬×ª»»Æ÷
//-----------------------------------------------------------------------------
class AggressiveTransition : public AITransition
{
public:
	AggressiveTransition() {}
	~AggressiveTransition() {}

public:
	AIStateType	Transition(AIController* pAI, AIStateType eCurState);
	static AITransition* Instance();

private:
	AIStateType IdleTransition(AIController* pAI);
	AIStateType PursueTransition(AIController* pAI);
	AIStateType AttackTransition(AIController* pAI);
	AIStateType ReturnTransition(AIController* pAI);
};

//-----------------------------------------------------------------------------
// ·ÀÓùÐÍ×´Ì¬×ª»»Æ÷
//-----------------------------------------------------------------------------
class GuardTransition : public AITransition
{
public:
	GuardTransition() {}
	~GuardTransition() {}

public:
	AIStateType Transition(AIController* pAI, AIStateType eCurState);
	static AITransition* Instance();

private:
	AIStateType IdleTransition(AIController* pAI);
	AIStateType AttackTransition(AIController* pAI);
	AIStateType ReturnTransition(AIController* pAI);
};

//-----------------------------------------------------------------------------
// ÅÚËþÐÍ×´Ì¬×ª»»Æ÷
//-----------------------------------------------------------------------------
class BarbetteTransition : public AITransition
{
public:
	BarbetteTransition() {}
	~BarbetteTransition() {}

public:
	AIStateType Transition(AIController* pAI, AIStateType eCurState);
	static AITransition* Instance();

private:
	AIStateType IdleTransition(AIController* pAI);
	AIStateType AttackTransition(AIController* pAI);
};

//-----------------------------------------------------------------------------
// ÌÓÅÜÐÍ×´Ì¬×ª»»Æ÷
//-----------------------------------------------------------------------------
class LamsterTransition : public AITransition
{
public:
	LamsterTransition() {}
	~LamsterTransition() {}

public:
	AIStateType Transition(AIController* pAI, AIStateType eCurState);
	static AITransition* Instance();

private:
	AIStateType IdleTransition(AIController* pAI);
	AIStateType PursueTransition(AIController* pAI);
	AIStateType AttackTransition(AIController* pAI);
	AIStateType ReturnTransition(AIController* pAI);
	AIStateType	FleeTransition(AIController* pAI);
};

//-----------------------------------------------------------------------------
// ºô¾ÈÐÍ×´Ì¬×ª»»Æ÷
//-----------------------------------------------------------------------------
class CallHelpTransition : public AITransition
{
public:
	CallHelpTransition() {}
	~CallHelpTransition() {}

public:
	AIStateType Transition(AIController* pAI, AIStateType eCurState);
	static AITransition* Instance();

private:
	AIStateType IdleTransition(AIController* pAI);
	AIStateType PursueTransition(AIController* pAI);
	AIStateType AttackTransition(AIController* pAI);
	AIStateType ReturnTransition(AIController* pAI);
	AIStateType CallForHelpTransition(AIController* pAI);
};

//-----------------------------------------------------------------------------
// ·çóÝÐÍ×´Ì¬×ª»»Æ÷
//-----------------------------------------------------------------------------
class SpaceOutTransition : public AITransition
{
public:
	SpaceOutTransition() {}
	~SpaceOutTransition() {}

public:
	AIStateType Transition(AIController* pAI, AIStateType eCurState);
	static AITransition* Instance();

private:
	AIStateType IdleTransition(AIController* pAI);
	AIStateType PursueTransition(AIController* pAI);
	AIStateType AttackTransition(AIController* pAI);
	AIStateType ReturnTransition(AIController* pAI);
	AIStateType SpaceTransition(AIController* pAI);
};