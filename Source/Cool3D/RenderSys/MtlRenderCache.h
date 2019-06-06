#pragma once
#include "..\Device\StateBlock.h"
#include <set>
using namespace std;

namespace Cool3D
{
	enum EMtlTrait			
	{
		EMT_Unknow=0,				 //未知
		EMT_Opacity,				 //不透明
		EMT_AlphaTest,				 //镂空
		EMT_AlphaBlend,				 //透明
		EMT_AlphaBlendDisableZ,		 //透明并关闭Z缓冲
		EMT_AlphaBlendDisableZWrite, //透明并关闭Z写入
		EMT_AlphaTestAndBlend,		 //半透明
	};

	class IMaterial;
	class IMtlModifier;
	/**	\class MtlRenderCache
		\brief IMaterial的RenderSys/Device friendly 对象
	*/
	class MtlRenderCache
	{
	public:
		MtlRenderCache(void);
		virtual ~MtlRenderCache(void);

		virtual void CompileMaterial(IMaterial *pMtl) =0;
		//!	当某个MtlModifier Update时调用这个,用于那些产生动画(例如uv动画)的Mod
		virtual void UpdateMtlMod(IMtlModifier *pMod) =0;
		virtual int GetNumPass() const =0;
		virtual void Begin(IVertShader* pVS=NULL,IPixelShader* pPS=NULL);
		virtual bool BeginPass(int i) =0;
		virtual void EndPass(bool clearTex=true) =0;
		virtual void End();
		virtual void UpdateMtl(IMaterial* pMtl) =0;
		virtual EMtlTrait GetMtlTrait() =0;

	protected:
		set<ERStateType>	m_influenceState;//所有pass影响到的render state,combile时创建
		StateBlock			m_stateSave;	//begin()时将所有影响到的render state保存,end时设置回去
	};
}//namespace Cool3D