#pragma once

/** class	BaseData;
	brief	游戏基本数据基类
*/
class BaseData 
{
public:
	BaseData(){}
	~BaseData(){}

	virtual void LoadFromFile() = 0;

protected:
	template<typename T>
	/** 在指定map中查找
	*/
	T* FindInMap(std::map<DWORD,T>& dataMap,DWORD attID)
	{
		std::map<DWORD, T> ::iterator pIter = dataMap.find(attID);
		if(pIter != dataMap.end())
			return &pIter->second;

		return NULL;
	}

	template<typename T>
	T* FindInMap(std::map<tstring,T>& dataMap, const tstring& strName)
	{
		std::map<tstring, T> ::iterator pIter = dataMap.find(strName);
		if(pIter != dataMap.end())
			return &pIter->second;

		return NULL;
	}

};
