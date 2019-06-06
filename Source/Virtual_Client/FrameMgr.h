#pragma once

class FrameBase;
class Player;

class FrameMgr
{
public:
	enum EFrame
	{
		EF_LOGIN,
		EF_SELECT,
		EF_WORLD,

		EF_END
	};

public:
	FrameMgr(Player* pPlayer);
	~FrameMgr(void);

public:
	VOID Init();
	VOID Update();
	VOID Destroy();

	BOOL ChangeFrame(EFrame eFrame);
	VOID ChangeLoginInfo(tagLoginInfo* pLoginInfo, BYTE srcFrame, BYTE dstFrame);

private:
	FrameBase*		m_Frame[EF_END];
	BYTE			m_byCurFrame;
	Player*			m_pPlayer;

	TList<FrameBase*>	m_listDestroy;
};
