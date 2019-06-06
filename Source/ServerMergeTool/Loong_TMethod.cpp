#include "StdAfx.h"
#include <typeinfo>

#include "Loong_MergeLogicCtrl.h"
/*
/删除vec中的数据
*/
template<class T>
VOID TMethod_VecDateDel(T &vec)
{
	for(UINT i=0; i<vec.size(); i++)
	{
		SAFE_DEL(vec[i]);
	}
	vec.swap(T());
}
/*
/loong db分开合并
*/
template<class T>
BOOL TMethod_Loong_SeparateServerMerge(T ,CLoongMergeLogicCtrl &clsMergeLogicCtrl)
{
	std::vector <T> vecSrc1;
	std::vector <T> vecSrc2;
	std::vector <T> vectTar;

	BOOL bRet = sLoongBeton.Loong_Query(vecSrc1,EM_DB_SRC_ONE);
	if(bRet == FALSE)
	{
		TMethod_VecDateDel(vecSrc1);
		return FALSE;
	}	

	//合并
	bRet = clsMergeLogicCtrl.Loong_LoongMerge(vecSrc1,vecSrc2,vectTar);
	if(bRet == FALSE)
	{
		TMethod_VecDateDel(vecSrc1);
		TMethod_VecDateDel(vecSrc2);
		vectTar.swap(std::vector <T>());
		return FALSE;
	}
	bRet = sLoongBeton.Loong_Insert(vectTar,EM_DB_TAR);
	if(bRet == FALSE)
	{
		TMethod_VecDateDel(vecSrc1);
		TMethod_VecDateDel(vecSrc2);
		vectTar.swap(std::vector <T>());
		return FALSE;
	}
	//删除源1数据
	TMethod_VecDateDel(vecSrc1);
	vectTar.swap(std::vector <T>());

	//////////////////////////////////////////////////////////////////////////
	//源2
	//////////////////////////////////////////////////////////////////////////
	bRet = sLoongBeton.Loong_Query(vecSrc2,EM_DB_SRC_TWO);
	if(bRet == FALSE)
	{
		TMethod_VecDateDel(vecSrc1);
		TMethod_VecDateDel(vecSrc2);
		return FALSE;
	}
	//合并
	bRet = clsMergeLogicCtrl.Loong_LoongMerge(vecSrc1,vecSrc2,vectTar);
	if(bRet == FALSE)
	{
		TMethod_VecDateDel(vecSrc1);
		TMethod_VecDateDel(vecSrc2);
		vectTar.swap(std::vector <T>());
		return FALSE;
	}
	bRet = sLoongBeton.Loong_Insert(vectTar,EM_DB_TAR);
	if(bRet == FALSE)
	{
		TMethod_VecDateDel(vecSrc1);
		TMethod_VecDateDel(vecSrc2);
		vectTar.swap(std::vector <T>());
		return FALSE;
	}
	//删除源2数据
	TMethod_VecDateDel(vecSrc2);
	vectTar.swap(std::vector <T>());


	return TRUE;
}

/*
/ loong db统一合并
*/
template<class T>
BOOL TMethod_Loong_ServerMerge(T ,CLoongMergeLogicCtrl &clsMergeLogicCtrl)
{
	std::vector <T> vecSrc1;
	std::vector <T> vecSrc2;
	std::vector <T> vectTar;

	BOOL bRet = sLoongBeton.Loong_Query(vecSrc1,EM_DB_SRC_ONE);
	if(bRet == FALSE)
	{
		TMethod_VecDateDel(vecSrc1);
		return FALSE;
	}
	bRet = sLoongBeton.Loong_Query(vecSrc2,EM_DB_SRC_TWO);
	if(bRet == FALSE)
	{
		TMethod_VecDateDel(vecSrc1);
		TMethod_VecDateDel(vecSrc2);
		return FALSE;
	}

	//合并
	bRet = clsMergeLogicCtrl.Loong_LoongMerge(vecSrc1,vecSrc2,vectTar);
	if(bRet == FALSE)
	{
		TMethod_VecDateDel(vecSrc1);
		TMethod_VecDateDel(vecSrc2);
		vectTar.swap(std::vector <T>());
		return FALSE;
	}
	bRet = sLoongBeton.Loong_Insert(vectTar,EM_DB_TAR);
	if(bRet == FALSE)
	{
		TMethod_VecDateDel(vecSrc1);
		TMethod_VecDateDel(vecSrc2);
		vectTar.swap(std::vector <T>());
		return FALSE;
	}
	TMethod_VecDateDel(vecSrc1);
	TMethod_VecDateDel(vecSrc2);
	vectTar.swap(std::vector <T>());

	return TRUE;
}
/*
/ loong db按AccountID合并
*/
template<class T>
BOOL TMethod_Loong_AccountServerMerge(T ,CLoongMergeLogicCtrl &clsMergeLogicCtrl)
{
	std::vector <T> vecSrc1;
	std::vector <T> vecSrc2;
	std::vector <T> vectTar;
	
	BOOL bRet = FALSE;
	//得到所有源1的 旧id
	const std::vector<DWORD> &vecOldSrc1AccountID = clsMergeLogicCtrl.GetAllOldSrc1AccountID();
	std::vector<DWORD>::const_iterator itOldSrc1 = vecOldSrc1AccountID.begin();

	//源1有，源2也有的用户。旧id
	const std::map <DWORD ,DWORD> &mpOldSrc1VsSrc2 = clsMergeLogicCtrl.GetOldSrc1VsSrc2();
	std::map <DWORD ,DWORD>::const_iterator itOldSrc1VsSrc2 = mpOldSrc1VsSrc2.begin();

	//合并所有源1的用户
	for(;itOldSrc1 != vecOldSrc1AccountID.end();++itOldSrc1)
	{

		bRet = sLoongBeton.Loong_AccountQuery(vecSrc1,*itOldSrc1,EM_DB_SRC_ONE);
		if(bRet == FALSE)
		{
			TMethod_VecDateDel(vecSrc1);
			return FALSE;
		}

		//找源1有，源2也有的用户 找到的话则加载
		itOldSrc1VsSrc2 = mpOldSrc1VsSrc2.find(*itOldSrc1);
		if(itOldSrc1VsSrc2 != mpOldSrc1VsSrc2.end())
		{
			bRet = sLoongBeton.Loong_AccountQuery(vecSrc2,itOldSrc1VsSrc2->second,EM_DB_SRC_TWO);
			if(bRet == FALSE)
			{
				TMethod_VecDateDel(vecSrc1);
				TMethod_VecDateDel(vecSrc2);
				return FALSE;
			}
		}

		//合并
		bRet = clsMergeLogicCtrl.Loong_LoongMerge(vecSrc1,vecSrc2,vectTar);
		if(bRet == FALSE)
		{
			TMethod_VecDateDel(vecSrc1);
			TMethod_VecDateDel(vecSrc2);
			vectTar.swap(std::vector <T>());
			return FALSE;
		}
		//插入
		bRet = sLoongBeton.Loong_Insert(vectTar,EM_DB_TAR);
		if(bRet == FALSE)
		{
			TMethod_VecDateDel(vecSrc1);
			TMethod_VecDateDel(vecSrc2);
			vectTar.swap(std::vector <T>());
			return FALSE;
		}
		TMethod_VecDateDel(vecSrc1);
		TMethod_VecDateDel(vecSrc2);
		vectTar.swap(std::vector <T>());

	}

	//合并所有源2独有的用户
	const std::vector <DWORD> &vecOldSrc2OnlyAccountID = clsMergeLogicCtrl.GetOldSrc2OnlyAccountID();
	std::vector<DWORD>::const_iterator itOldSrc2Only = vecOldSrc2OnlyAccountID.begin();
	for(;itOldSrc2Only != vecOldSrc2OnlyAccountID.end();++itOldSrc2Only)
	{
		bRet = sLoongBeton.Loong_AccountQuery(vecSrc2,*itOldSrc2Only,EM_DB_SRC_TWO);
		if(bRet == FALSE)
		{
			TMethod_VecDateDel(vecSrc1);
			TMethod_VecDateDel(vecSrc2);
			return FALSE;
		}
		//合并
		bRet = clsMergeLogicCtrl.Loong_LoongMerge(vecSrc1,vecSrc2,vectTar);
		if(bRet == FALSE)
		{
			TMethod_VecDateDel(vecSrc1);
			TMethod_VecDateDel(vecSrc2);
			vectTar.swap(std::vector <T>());
			return FALSE;
		}
		//插入
		bRet = sLoongBeton.Loong_Insert(vectTar,EM_DB_TAR);
		if(bRet == FALSE)
		{
			TMethod_VecDateDel(vecSrc1);
			TMethod_VecDateDel(vecSrc2);
			vectTar.swap(std::vector <T>());
			return FALSE;
		}
		TMethod_VecDateDel(vecSrc1);
		TMethod_VecDateDel(vecSrc2);
		vectTar.swap(std::vector <T>());
	}

	//如果不是这3个表之中的一个
	if(typeid(T) != typeid(tagTWrap_Loong_Item*) && typeid(T) != typeid(tagTWrap_Loong_ItemDel*)\
		&& typeid(T) != typeid(tagTWrap_Loong_ItemBaiBao*))
	{
		return TRUE;
	}

	//得到所有帮派id 合并帮派仓库物品
	const std::set <DWORD> &setOldGuildID = clsMergeLogicCtrl.GetAllOldGuildID();
	std::set<DWORD>::const_iterator itOldGuildID = setOldGuildID.begin();

	for(;itOldGuildID != setOldGuildID.end();++itOldGuildID)
	{	
		bRet = sLoongBeton.Loong_AccountQuery(vecSrc1,*itOldGuildID,EM_DB_SRC_ONE);
		if(bRet == FALSE)
		{ 
			TMethod_VecDateDel(vecSrc1);
			return FALSE;
		}

		bRet = sLoongBeton.Loong_AccountQuery(vecSrc2,*itOldGuildID,EM_DB_SRC_TWO);
		if(bRet == FALSE)
		{
			TMethod_VecDateDel(vecSrc1);
			TMethod_VecDateDel(vecSrc2);
			return FALSE;
		}
		//合并
		bRet = clsMergeLogicCtrl.Loong_LoongMerge(vecSrc1,vecSrc2,vectTar);
		if(bRet == FALSE)
		{
			TMethod_VecDateDel(vecSrc1);
			TMethod_VecDateDel(vecSrc2);
			vectTar.swap(std::vector <T>());
			return FALSE;
		}
		//插入
		bRet = sLoongBeton.Loong_Insert(vectTar,EM_DB_TAR);
		if(bRet == FALSE)
		{
			TMethod_VecDateDel(vecSrc1);
			TMethod_VecDateDel(vecSrc2);
			vectTar.swap(std::vector <T>());
			return FALSE;
		}

		TMethod_VecDateDel(vecSrc1);
		TMethod_VecDateDel(vecSrc2);
		vectTar.swap(std::vector <T>());
	}

	return TRUE;
}

/*
/loong分页合并
*/
template<class T>
BOOL TMethod_Loong_PagingServerMerge(T ,CLoongMergeLogicCtrl &clsMergeLogicCtrl)
{
	std::vector <T> vecSrc1;
	std::vector <T> vecSrc2;
	std::vector <T> vectTar;

	//分页限制
	INT64 n64NumLimit = 0;
	EPagingState eRet = EPS_DB_NULL; 

	while(eRet != EPS_DB_RESULT_NULL)
	{
		eRet = sLoongBeton.Loong_PagingQuery(vecSrc1,n64NumLimit,EM_DB_SRC_ONE);
		if(eRet == EPS_DB_ERROR)
		{
			TMethod_VecDateDel(vecSrc1);
			return FALSE;
		}
		//合并
		BOOL bRet = clsMergeLogicCtrl.Loong_LoongMerge(vecSrc1,vecSrc2,vectTar);
		if(bRet == FALSE)
		{
			TMethod_VecDateDel(vecSrc1);
			TMethod_VecDateDel(vecSrc2);
			vectTar.swap(std::vector <T>());
			return FALSE;
		}
		bRet = sLoongBeton.Loong_Insert(vectTar,EM_DB_TAR);
		if(bRet == FALSE)
		{
			TMethod_VecDateDel(vecSrc1);
			TMethod_VecDateDel(vecSrc2);
			vectTar.swap(std::vector <T>());
			return FALSE;
		}
		TMethod_VecDateDel(vecSrc1);
		TMethod_VecDateDel(vecSrc2);
		vectTar.swap(std::vector <T>());

		n64NumLimit += DB_PAGING;
	}

	n64NumLimit = 0;
	eRet = EPS_DB_NULL;
	//源2
	while(eRet != EPS_DB_RESULT_NULL)
	{
		eRet = sLoongBeton.Loong_PagingQuery(vecSrc2,n64NumLimit,EM_DB_SRC_TWO);
		if(eRet == EPS_DB_ERROR)
		{
			TMethod_VecDateDel(vecSrc1);
			TMethod_VecDateDel(vecSrc2);
			return FALSE;
		}
		//合并
		BOOL bRet = clsMergeLogicCtrl.Loong_LoongMerge(vecSrc1,vecSrc2,vectTar);
		if(bRet == FALSE)
		{
			TMethod_VecDateDel(vecSrc1);
			TMethod_VecDateDel(vecSrc2);
			vectTar.swap(std::vector <T>());
			return FALSE;
		}
		bRet = sLoongBeton.Loong_Insert(vectTar,EM_DB_TAR);
		if(bRet == FALSE)
		{
			TMethod_VecDateDel(vecSrc1);
			TMethod_VecDateDel(vecSrc2);
			vectTar.swap(std::vector <T>());
			return FALSE;
		}
		TMethod_VecDateDel(vecSrc1);
		TMethod_VecDateDel(vecSrc2);
		vectTar.swap(std::vector <T>());

		n64NumLimit += DB_PAGING;
	}
	return TRUE;
}



/*
/login统一合并
*/
template<class T>
BOOL TMethod_Login_ServerMerge(T ,CLoongMergeLogicCtrl &clsMergeLogicCtrl)
{
	std::vector <T> vecSrc1;
	std::vector <T> vecSrc2;
	std::vector <T> vectTar;

	BOOL bRet = sLoginBeton.LoginDB_Accout_Query(vecSrc1,EM_DB_SRC_ONE);
	if(bRet == FALSE)
	{
		TMethod_VecDateDel(vecSrc1);
		return FALSE;
	}
	bRet = sLoginBeton.LoginDB_Accout_Query(vecSrc2,EM_DB_SRC_TWO);
	if(bRet == FALSE)
	{
		TMethod_VecDateDel(vecSrc1);
		TMethod_VecDateDel(vecSrc2);
		return FALSE;
	}

	//合并
	bRet = clsMergeLogicCtrl.Loong_LoginMerge(vecSrc1,vecSrc2,vectTar);
	if(bRet == FALSE)
	{
		TMethod_VecDateDel(vecSrc1);
		TMethod_VecDateDel(vecSrc2);
		vectTar.swap(std::vector <T>());
		return FALSE;
	}
	bRet = sLoginBeton.LoginDB_Accout_Insert(vectTar,EM_DB_TAR);
	if(bRet == FALSE)
	{
		TMethod_VecDateDel(vecSrc1);
		TMethod_VecDateDel(vecSrc2);
		vectTar.swap(std::vector <T>());
		return FALSE;
	}
	TMethod_VecDateDel(vecSrc1);
	TMethod_VecDateDel(vecSrc2);
	vectTar.swap(std::vector <T>());

	return TRUE;
}
