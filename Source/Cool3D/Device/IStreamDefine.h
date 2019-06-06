#pragma once

namespace Cool3D
{
	/// Vertex element semantics, used to identify the meaning of vertex buffer contents
	enum VertElementSemantic 
	{
		VES_POSITION,			// Position
		VES_POSITION_TRANSED,	// transformed position data ranging from (0,0) to (viewport width, viewport height). 
		VES_NORMAL,				// Normal
		VES_PSIZE,				// Size of point sprites
		VES_BLEND_WEIGHT,		// Blending weights
		VES_BLEND_INDEX,		// Blending indices
		VES_DIFFUSE,			// Diffuse colours
		VES_SPECULAR,			// Specular colours
		VES_TEXCOORD,			// Texture coordinates
		VES_BINORMAL,			// Binormal (Y axis if normal is Z)
		VES_TANGENT,			// Tangent (X axis if normal is Z)
		VES_DEPTH,				// Depth
	};

	/// Vertex element type, used to identify the base types of the vertex contents
	enum VertElementType
	{
		VET_FLOAT1,
		VET_FLOAT2,
		VET_FLOAT3,
		VET_FLOAT4,
		VET_COLOUR,
		VET_SHORT1,
		VET_SHORT2,
		VET_SHORT3,
		VET_SHORT4
	};

	/**	\class IStreamDefine
		\brief 定义当前参与渲染的所有VertBuffer Stream的格式
	*/
	class IStreamDefine
	{
	public:
		IStreamDefine(void);
		virtual ~IStreamDefine(void);

		virtual void Begin() =0;
		virtual void AddElement(BYTE stream,BYTE offset,VertElementType type,VertElementSemantic usage,BYTE	usageIndex) =0;
		virtual void End() =0;
	};
}//namespace Cool3D