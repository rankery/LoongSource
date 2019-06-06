#pragma once
#include "MGeneral.h"

namespace Cool3D
{
	class ResVertShader;
	class ResPixelShader;
	/**	\class MStatic
		\brief 静态模型的材质
	*/
	class Cool3D_API MStatic : public MGeneral
	{
	public:
		enum EShader
		{
			ES_None=-1,
			ES_Direct=0,
			ES_DirectPoint1,
			ES_Num
		};
	public:
		MStatic(void);
		virtual ~MStatic(void);

		//--MGeneral
		virtual bool IsResourceReady() const;
		virtual void Destroy();
		virtual DWORD GetRenderTrait1() const;

		//--ISerializable
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);
		
		//--
		void LoadShaders();

	public:
		ResVertShader* m_pVS[ES_Num];
		ResPixelShader* m_pPS[ES_Num];
		EShader	m_shader;

		DECL_RTTI_DYNC(MStatic);
	};
}//namespace Cool3D