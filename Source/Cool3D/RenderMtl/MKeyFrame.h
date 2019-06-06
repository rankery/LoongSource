#pragma once
#include "MGeneral.h"

namespace Cool3D
{
	class ResVertShader;
	class ResPixelShader;
	/**	\class MKeyFrame
		\brief 关键帧模型的材质
	*/
	class Cool3D_API MKeyFrame : public MGeneral
	{
	public:
		enum EShader
		{
			ES_None=-1,
			ES_Direct=0,
			ES_DirectPoint1,
			ES_DirectPoint2,
			ES_Num
		};
	public:
		MKeyFrame(void);
		virtual ~MKeyFrame(void);

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

		DECL_RTTI_DYNC(MKeyFrame);
	};
}//namespace Cool3D