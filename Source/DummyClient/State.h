#pragma once

class FSM;
struct tagGameEvent;
/** ×´Ì¬»ú×´Ì¬»ùÀà
*/
class State
{
public:
	State(void);
	virtual ~State(void);

	void SetFSM(FSM* pFSM){m_pFSM=pFSM;}
	virtual void Active(tagGameEvent* pEvent)=0;
	virtual void Update()=0;
	virtual void OnGameEvent(tagGameEvent* pEvent) =0;

protected:
	FSM* m_pFSM;
};
