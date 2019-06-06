#pragma once

class SaveAccount 
{
public:
	SaveAccount(void);
	~SaveAccount(void);

	static SaveAccount* Inst();
	void SetAccount(const TCHAR* szAccount);
	tstring& GetAccount() { return m_account; }
	void SetWorldID(const TCHAR* world_id){ m_world_id = world_id; }
	tstring& GetWorldID() { return m_world_id; }

	INT	GetCurMusic() { return m_nMusic; }
	void SetCurMusic(INT nMusic) { m_nMusic = nMusic; }

private:
	tstring				m_account;
	tstring				m_world_id;

	INT					m_nMusic;
};

