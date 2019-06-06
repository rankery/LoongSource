#pragma once

//----------------------------------------------------------------
// 为了内存池
//----------------------------------------------------------------
class ServerMemPool
{
public:
	LPVOID	Alloc(DWORD dwSize);
	VOID	Free(LPVOID pMem);
};

extern ServerMemPool g_MemPoolSafe;

#pragma pack(push, 1)

//----------------------------------------------------------------
// 常量
//----------------------------------------------------------------
const INT X_ROLE_BUFF_SIZE			= (1024 * 1024);		// 1M.读取保存角色数据时，消息buff大小

const INT64 MIN_ITEM_SERIAL_INTERNAL	= 500000000000;
const INT64 MAX_ITEM_SERIAL_OUTTER		= 400000000000;
const INT64 MIN_ITEM_SERIAL_OUTTER		= 300000000001;
const INT64	MAX_ITEM_SERIAL_BILL		= 300000000000;

//----------------------------------------------------------------
// 字符集无关的计算字符数组字符个数宏
//----------------------------------------------------------------
#define MCalTCharArrayNum(szArray)		(sizeof(TCHAR) == 1 ? sizeof(szArray) : (sizeof(szArray) >> 1))

//----------------------------------------------------------------
// 判断装备是否已鉴定
//----------------------------------------------------------------
#define MIsIdentified(pEquip)			(pEquip->equipSpec.byQuality != (BYTE)EIQ_Null)

//----------------------------------------------------------------
// 判断装备是否为ring
//----------------------------------------------------------------
#define MIsRing(pEquip)				(EEP_Finger1 == pEquip->pEquipProto->eEquipPos)
#define MIsWeapon(pEquip)			(EIT_Weapon == pEquip->pProtoType->eType)
#define MIsFashion(pEquip)			(EIT_Fashion == pEquip->pProtoType->eType)

#define MIsWeaponByDisplayPos(nPos)	(EAE_RWeapon == nPos || EAE_LWeapon == nPos)
#define MIsWeaponByEquipPos(nPos)	(EEP_RightHand == nPos || EEP_LeftHand == nPos)

//----------------------------------------------------------------
// 判断装备是否有效果
//----------------------------------------------------------------
//#define MIsEquipIneffective(pEquip)	\
//		(((tagEquipProto *)pEquip->pProtoType)->n16Newness > 0 \
//		&& pEquip->nUseTimes / 100 > ((tagEquipProto *)pEquip->pProtoType)->n16Newness)
//
//#define MIsEquipMainWeapon(pEquip)	\
//		(((tagEquipProto*)pEquip->pProtoType)->eEquipPos == EEP_RightHand)
//
//#define MIsEquipAsstWeapon(pEquip)	\
//		(((tagEquipProto*)pEquip->pProtoType)->eEquipPos == EEP_LeftHand)
//
//#define MIsEquipNeedCalEffect(pEquip)	\
//		(!MIsEquipAsstWeapon(pEquip) && !MIsEquipIneffective(pEquip))


//----------------------------------------------------------------
// 判断是否为百分数
//----------------------------------------------------------------
#define MIsValuePct(nNum)			((nNum) > 100000 || (nNum) < -100000)

//----------------------------------------------------------------
// 将百分数转化为数值
//----------------------------------------------------------------
#define MValuePctTrans(nNum)		((nNum) > 100000 ? (nNum) - 100000 : (nNum) + 100000)

#define MMSGINIT(ptr, TYPE)				\
	{											\
	TYPE tmp##TYPE;								\
	memcpy(ptr, &tmp##TYPE, sizeof(tagNetCmd));	\
	}

//----------------------------------------------------------------
// 获取消息
//----------------------------------------------------------------
#define MGET_MSG(PRECV, PMSG, MSG_KIND)	\
	tag##MSG_KIND* PRECV = (tag##MSG_KIND*)PMSG;

//----------------------------------------------------------------
// 指针类型转换
//----------------------------------------------------------------
#define MTRANS_POINTER(PNEWPOINTER, PDATA, DST_POINTER_TYPE)	\
	DST_POINTER_TYPE* PNEWPOINTER = (DST_POINTER_TYPE*)PDATA;

//----------------------------------------------------------------
// 若无效则返回
//----------------------------------------------------------------
#define MTRANS_ELSE_RET(PNEWPOINTER, PDATA, DST_POINTER_TYPE, RETVALUE)		\
	MTRANS_POINTER(PNEWPOINTER, PDATA, DST_POINTER_TYPE);		\
	if(!P_VALID(PNEWPOINTER))	return RETVALUE;

//----------------------------------------------------------------
// 不定长消息内存申请处理 -- 需外部重新设置消息大小
//----------------------------------------------------------------
//#define MCREATE_MSG(PSEND, SIZE, MSG_KIND)	\
//	BYTE* temp_p_##MSG_KIND = new BYTE[SIZE];					\
//	ZeroMemory(temp_p_##MSG_KIND, SIZE);						\
//	tag##MSG_KIND* PSEND = new(temp_p_##MSG_KIND) tag##MSG_KIND;\
//	PSEND->dwSize = SIZE;
//
//
//#define MDEL_MSG(PSEND)		delete [](BYTE*)PSEND;

//----------------------------------------------------------------
// 不定长消息内存申请处理 -- 需外部重新设置消息大小
// 优点：不用管理内存；缺点：编译时会生成相应消息的模板函数
//----------------------------------------------------------------
class MsgSend
{
public:
	MsgSend(INT nSize)
	{
		m_nSize = nSize;

		// 调整大小为4的整数倍
		m_nSize = (((m_nSize - 1) >> 2) + 1) << 2;

		m_pBuff = (BYTE*)g_MemPoolSafe.Alloc(m_nSize);
	}

	~MsgSend()
	{
		if(m_pBuff != NULL)
		{
			g_MemPoolSafe.Free(m_pBuff);
		}
	}

	template<typename T>
	VOID GetData(T* &pT)
	{
		ASSERT(m_nSize >= sizeof(T));
		ZeroMemory(m_pBuff, m_nSize);
		MMSGINIT(m_pBuff, T);
		pT = (T*)m_pBuff;
	}

private:
	BYTE	*m_pBuff;
	INT		m_nSize;
};

// 把现在用的宏重新定义下
#define MCREATE_MSG(PSEND, SIZE, MSG_KIND)		\
	MsgSend tmp##PSEND(SIZE);	\
	tag##MSG_KIND* PSEND;		\
	tmp##PSEND.GetData(PSEND);	\
	PSEND->dwSize = SIZE

#define MDEL_MSG(PSEND)		(0)

//----------------------------------------------------------------
// 服务器活动客户端字符串后处理 -- 确保有字符串结束符
//----------------------------------------------------------------
#define MGET_MSG_STRING(TSTR, PMSGSTR, MSGSZ, MSG_KIND, STRFIELD)						\
	INT32 tmpStrSz = MSGSZ - FIELD_OFFSET(tag##MSG_KIND, STRFIELD) / sizeof(TCHAR);		\
	if(tmpStrSz < 0)	tmpStrSz = 0;													\
	tstring TSTR(PMSGSTR, tmpStrSz)

//----------------------------------------------------------------
// 向服务器控制台发送警告信息
//----------------------------------------------------------------
#define MAttResCaution(FileName, KeyName, KeyVal)	\
	IMSG(_T("\n\nCaution:\n"));						\
	IMSG(_T("\tThere is a critical error in proto <%s,%s:%u>!!!!!\n\n"), (FileName), (KeyName), (KeyVal));

#define ASSERT_P_VALID( ptr ) ASSERT(P_VALID(ptr))

#pragma pack(pop)

