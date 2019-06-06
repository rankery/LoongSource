#pragma once

//----------------------------------------------------------------
// 判断是否为百分数
//----------------------------------------------------------------
#define MIsValuePct(nNum)			((nNum) > 100000 || (nNum) < -100000)

//----------------------------------------------------------------
// 将百分数转化为数值
//----------------------------------------------------------------
#define MValuePctTrans(nNum)		((nNum) > 100000 ? (nNum) - 100000 : (nNum) + 100000)

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
	template<typename K,typename T>
	/** 在指定map中查找
	*/
	T* FindInMap(map<K,T>& dataMap,K key)
	{
		map<K,T> ::iterator pIter = dataMap.find(key);
		if(pIter != dataMap.end())
			return &pIter->second;

		// 加入提示，以防Release的数据文件出现问题无法查到
		//ERR(_T("could not find attID:%d"), attID);
		return NULL;
	}

};
