#pragma once
#include "..\util\FArchive.h"
#include "..\util\rttiobj.h"
#include "..\Cool3D.h"

namespace Cool3D
{
	class MtlRenderCache;
	class IMtlModifier;
	/**	\class IMaterial 
		\brief 基本材质的接口类
		\remarks Material包括Texture和pixel stage的计算方式(各个Texture的混合以及VertexColor的处理等),
		可以使用FixedFunc pipeline的texture stage参数实现,也可以使用pixel shader实现
		目前只支持静态的Material,即CubeMap,Sphere等贴图都是事先生成好的
	*/
	class Cool3D_API IMaterial : public RTTIObj,public ISerializable
	{
	public:
		IMaterial(void);
		virtual ~IMaterial(void);

		/**	附加RenderCache,由this管理pCache的生存期		*/
		void AttachRenderCache(MtlRenderCache *pCache);
		MtlRenderCache* GetRenderCache()	{	return m_pRenderCache;}
		
		/**	附加MtlModifier,由this管理pMod的生存期		*/
		void AttachModifier(IMtlModifier *pMod);
		bool RemoveModifier(const TCHAR* szName);
		UINT GetNumMods() const;
		const IMtlModifier *GetModifier(UINT i) const;
		IMtlModifier *GetModifier(const TCHAR* szClassName);
		virtual void UpdateModifiers(float deltaTime);
		void ResetModAni();

		/** 是否所使用的resource(主要指texture)都已经load ok
			\remark 主要是为了处理resource 异步IO,如果全部res load ok,则可以创建mtl render cache
			\see class ResMgr, class ResBase, class ResTexture
		*/
		virtual bool IsResourceReady() const;
		/**返回值被用于渲染的排序,目的是减少shader切换次数*/
		virtual DWORD GetRenderTrait1() const;
		/**返回值被用于渲染的排序,目的是减少贴图切换次数*/
		virtual DWORD GetRenderTrait2() const;

		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		static bool SaveToFile(IMaterial *pMtl,const TCHAR* szFileName);
		static IMaterial* LoadFromFile(const TCHAR* szFileName);

		void InvalidateRenderCache();
		void UpdateRenderCache();
	protected:
		IMaterial				*m_pFallbackMtl;//如果当前系统不支持this Mtl,则使用Fallback Mtl代替
		MtlRenderCache			*m_pRenderCache;//渲染系统的Cache数据,由this管理生存期

	private:
		class Member;
		Member	*m_p;

		DECL_RTTI(IMaterial);
	};
}//namespace Cool3D