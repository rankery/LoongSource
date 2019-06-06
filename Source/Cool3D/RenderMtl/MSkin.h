#pragma once
#include "MGeneral.h"

namespace Cool3D
{
	class ResVertShader;
	class ResPixelShader;
	/**	\class MSkin
		\brief 蒙皮模型的材质
	*/
	class Cool3D_API MSkin : public MGeneral
	{
	public:
		enum EShader
		{
			ES_None=-1,
			ES_Bump_Direct_Skin=0,
			ES_Bump_Direct_Point1_Skin,
			ES_Direct_Skin,
			ES_Direct_Point1_Skin,
			ES_Num
		};
	public:
		MSkin(void);
		virtual ~MSkin(void);

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

		MBitmap*		m_pBlendLayer;   // 主要用于面部细节

		DECL_RTTI_DYNC(MSkin);
	};
}//namespace Cool3D