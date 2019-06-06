//-----------------------------------------------------------------------------
// File: FilterMgr.h
// Desc: game tool filter manager 1.0
// Time: 2003-1-8
// Auth: CTCRST
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine{

enum EFM_FILTER	// 过滤器种类
{
	FMF_Null,

	// 各种数据压缩算法
	FMF_Changeless,
	FMF_MiniLzo,
	FMF_ZLib,
	FMF_ZLibDeflated,

	// 各种加密解密散列算法
	FMF_Des,
	FMF_Ras,
	FMF_Md5,
	FMF_Sha1,
	FMF_Sha512,

	FMF_End
};
 
struct tagFilterParam	// 过滤器启动参数
{
	// 以下参数由调用方填写(in)
	EFM_FILTER	eFilter;			// 进行何种过滤
	BOOL		bCompress;			// 正向或者逆向
	LPVOID		lpMemIn;			// 源指针(指向输入缓冲)
	LPVOID		lpMemOut;			// 目标指针(指向输出缓冲)
	DWORD		dwInBufferSize;		// 输入缓冲大小
	DWORD		dwOutBufferSize;	// 输出缓冲大小
	DWORD		dwInDataSize;		// 输入数据大小
	DWORD		dwQueryPart;		// 仅要某一块数据

	// 以下参数由过滤器填写(out)
	DWORD		dwOutDataSize;	
	BOOL		bSuccess;
};


//-----------------------------------------------------------------------------
// 简单的过滤器管理
//-----------------------------------------------------------------------------
class FilterMgr
{
public:

	BOOL InitFilter(EFM_FILTER eFilter);	// 初始化过滤器
	
	// 得到过滤器所需要的Dest缓冲附加值
	DWORD GetFilterExtraSpace(DWORD dwSrcSize, EFM_FILTER eFilter);
	// 检查各种过滤器算法需要的额外运算空间
	BOOL CheckExtraSpace(DWORD dwInSize, DWORD dwOutSize, EFM_FILTER eFilter);

	VOID Filter(tagFilterParam* pParam);

	FilterMgr();
	~FilterMgr();

private:
	// 各种数据压缩算法
	VOID FilterMiniLzo(tagFilterParam* pParam);
	VOID FilterZLib(tagFilterParam* pParam);
	VOID FilterZLibDeflated(tagFilterParam* pParam);

};



} // namespace vEngine{
