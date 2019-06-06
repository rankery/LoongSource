
#pragma once
#include "..\Math\GeomDefine.h"
#include "..\Device\IVertBuffer.h"
#include "..\Util\SharedPtr.h"
#include "..\Util\RTTIObj.h"
#include "..\Public\VertexDefine.h"
#include "..\ResSys\VertStream.h"
#include "..\RenderMtl\IMaterial.h"
#include ".\RenderSys.h"
#include ".\MtlRenderCache.h"

namespace Cool3D
{
	class SceneNode;
	class RenderSys;
	class IVertBuffer;
	class IStreamDefine;
	class IIndexBuffer;
	class IMaterial;
	
	typedef SharedPtr<IVertBuffer,SharePtrDelete<IVertBuffer> > SharedVB;
	typedef SharedPtr<IMaterial,SharePtrDelete<IMaterial> >	SharedMtl;

	/**	\class RenderNode
		\brief 提交给RenderSys的一个渲染单位，是SceneGraph与RenderSys的接口
		\remarks 具体的RenderNode可以在创建Render Device Resource时检测RenderSys的类别,从而采取不同的策略
		\see RenderSys, SceneNode
		\todo 所有的GetMtlTrait()都需要改动, 因为Mtl对象改由scene node保存, 所以mtl对象指针不再具有特征意义
	*/
	class Cool3D_API RenderNode : public RTTIObj
	{
	public:
		enum ERenderPri
		{
			EPri_Hightest=100,
			EPri_Normal=50,//default
			EPri_Low=25,
			EPri_Lowest=0
		};
	public:
		RenderNode(void);
		virtual ~RenderNode(void);

		virtual bool SupportLod();	//是否支持LOD,默认为不支持
		virtual void Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod=0,RenderItem* pItem=NULL,SceneGraph* pSG=NULL);	//支持Lod的
		virtual EMtlTrait GetMtlTrait(SceneNode *pSGNode) const =0;
		virtual DWORD GetMtlTrait1(SceneNode *pSGNode) const =0;
		virtual DWORD GetMtlTrait2(SceneNode *pSGNode) const =0;
		virtual const AABBox& GetAABBox() const =0;
		virtual int GetMtlID() const{ return 0;}

	public:
		static IMaterial	*m_pSpecialMtl;//如果此Mtl不为NULL,则不使用BeginMtl()参数指定的mtl
		static IVertShader	*m_pSpecialVS;
		static IPixelShader	*m_pSpecialPS;
		/**	\return number of passes	*/
		int BeginMtl(RenderSys *pSys,IMaterial *pMtl);
		void BeginMtlPass(int passIndex);
		void EndMtlPass(bool clearTex=false);
		void EndMtl();
		EMtlTrait GetMtlTrait(int mtlID,SceneNode* pSGNode) const;
		DWORD GetMtlTrait1(int mtlID,SceneNode *pSGNode) const;
		DWORD GetMtlTrait2(int mtlID,SceneNode *pSGNode) const;

		int GetPriority() const		{	return m_renderPri;}
		void SetPriority(int pri)	{	m_renderPri=pri;}
	private:
		IMaterial	*m_pCurMtl;//begin mtl所启动的mtl
		int			m_renderPri;//渲染优先级别,priority,数字越大越优先;目前只影响不透明物体的渲染顺序
        
		DECL_RTTI(RenderNode);
	};
}//namespace Cool3D