#pragma once
#include "..\util\refobj.h"
#include "..\Cool3D.h"
#pragma warning(disable:4251)

	class IFS;
namespace Cool3D
{
	/**	\class ResBase 
		\brief 所有Resource的基类
	*/
	class Cool3D_API ResBase : public RefObj
	{
	public:
		explicit ResBase(const TCHAR* szName);
		virtual ~ResBase(void);

		/**	连接到一个文件系统
		*/
		void AttachFS(IFS *pFS)
		{	m_pFS=pFS; }
		IFS	*GetSafeFS();
		/**	以阻塞方式创建实际内容,只在第一次产生对象时调用
			\return 返回资源的大小,单位由派生类决定,ResPool用这个大小来衡量是否达到设定的上限
			\remarks 默认实现为返回资源名指向的磁盘文件的大小
		*/
		virtual UINT CreateContent(DWORD param) =0;
		/**	以异步方式创建内容,内容是否创建完毕通过调用IsCreated()来判断
			\see CreateContent(),IsCreate()
		*/
		virtual void CreateContentAsync(DWORD param);

		UINT GetSize() const
		{	return m_size; }
		const TCHAR *GetName() const
		{	return m_szName.c_str(); }
		bool IsCreated() const
		{	return m_bCreated!=0; }
	protected:
		IFS		*m_pFS;
		volatile LONG	m_bCreated;//4BYTE
		UINT	m_size;
	
		tstring	m_szName;

		DECL_RTTI(ResBase);
	};
}//namespace Cool3D