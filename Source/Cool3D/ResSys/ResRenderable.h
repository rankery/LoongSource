#pragma once
#include "resbase.h"
#include "..\Math\GeomDefine.h"
#include "..\Util\FArchive.h"
#include "..\Public\MtlDesc.h"

namespace Cool3D
{
	class RenderNode;
	class IMaterial;
	/**	\class ResRenderable 
		\brief 所有可渲染的资源的基类
		\todo 派生类的序列化支持问题
	*/
	class Cool3D_API ResRenderable : public ResBase
	{
	public:
		enum ETexPathMode
		{
			ETexPath_Lib,	//texture放在指定的目录中
			ETexPath_Mdl	//texture放在模型所在目录中
		};
	public:
		ResRenderable(const TCHAR* szName);
		virtual ~ResRenderable(void);

		virtual UINT GetNumRenderNode() const =0;
		virtual RenderNode *GetRenderNode(UINT i) const =0;
		virtual const AABBox	*GetAABBox() const =0;
		virtual int GetNumMtl() const =0;
		/**	将内部所有需要的材质创建到一个数组中,由调用者管理其生存期
			\remarks 注意,当使用resource 异步IO时,只有resource创建成功后,此调用才有效
		*/
		virtual void CreateMtlArray(vector<IMaterial*>&	mtls,const TCHAR* szLightMap,float mtlDiffuseFactor=1.0f) const =0;
	protected:
		void CreatemtlArrayFromDesc(vector<IMaterial*>&	mtls,const vector<tagMtlDesc>& descs,UINT mipLevels,const TCHAR* szLightMap) const;
		DECL_RTTI(ResRenderable);
	};
}//namespace Cool3D