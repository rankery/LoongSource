#pragma once

struct tagNS_Fishing;
class ItemFrame;
class LocalPlayer;
class Item;

class FishMgr
{
public:
	FishMgr(void);
	~FishMgr(void);
	
	static FishMgr* Inst();

	VOID SetFishFoodType( DWORD dwFishFoodType );

	DWORD GetFishFoodType();

	VOID StartFish( Item* pItem );

private:
	ItemFrame*					m_pItemFrame;
	DWORD						m_dwFishFoodType;
};
