#include "StdAfx.h"
#include ".\dx9vertdecl.h"
#include "DX9Device.h"
#include "..\..\Util\Exception.h"

namespace Cool3D
{
	namespace Detail
	{

		DX9VertDecl::DX9VertDecl(void)
		{
			m_pDecl=NULL;
		}

		DX9VertDecl::~DX9VertDecl(void)
		{
		}

		D3DDECLTYPE VertElementType2DX9(VertElementType type)
		{
			D3DDECLTYPE ret=D3DDECLTYPE_FLOAT1;
			switch(type)
			{
			case VET_FLOAT1: ret=D3DDECLTYPE_FLOAT1;
				break;
			case VET_FLOAT2: ret=D3DDECLTYPE_FLOAT2;
				break;
			case VET_FLOAT3: ret=D3DDECLTYPE_FLOAT3;
				break;
			case VET_FLOAT4: ret=D3DDECLTYPE_FLOAT4;
				break;
			case VET_COLOUR: ret=D3DDECLTYPE_D3DCOLOR;
				break;
			case VET_SHORT1: ASSERT(0);
				break;
			case VET_SHORT2: ASSERT(0);
				break;
			case VET_SHORT3: ASSERT(0);
				break;
			case VET_SHORT4: ASSERT(0);
				break;
			default:
				ASSERT(0);
			}
			return ret;
		}

		D3DDECLUSAGE VertUsage2DX9(VertElementSemantic usage)
		{
			D3DDECLUSAGE ret=D3DDECLUSAGE_POSITION;
			switch(usage)
			{
			case VES_POSITION:			ret = D3DDECLUSAGE_POSITION;
				break;
			case VES_POSITION_TRANSED:	ret = D3DDECLUSAGE_POSITIONT;
				break;
			case VES_NORMAL:			ret = D3DDECLUSAGE_NORMAL;
				break;
			case VES_PSIZE:				ret = D3DDECLUSAGE_PSIZE;
				break;
			case VES_BLEND_WEIGHT:		ret = D3DDECLUSAGE_BLENDWEIGHT;
				break;
			case VES_BLEND_INDEX:		ret = D3DDECLUSAGE_BLENDINDICES;
				break;
			case VES_DIFFUSE:			ret = D3DDECLUSAGE_COLOR;
				break;
			case VES_SPECULAR:			ret = D3DDECLUSAGE_COLOR;
				break;
			case VES_TEXCOORD:			ret = D3DDECLUSAGE_TEXCOORD;
				break;
			case VES_BINORMAL:			ret = D3DDECLUSAGE_BINORMAL;
				break;
			case VES_TANGENT:			ret = D3DDECLUSAGE_TANGENT;
				break;
			case VES_DEPTH:				ret = D3DDECLUSAGE_DEPTH;
				break;
			default:
				ASSERT(0);
			}
			return ret;
		}

		void DX9VertDecl::Begin()
		{
			m_elements.clear();
		}
		
		void DX9VertDecl::AddElement(BYTE stream,BYTE offset,VertElementType type,VertElementSemantic usage,BYTE usageIndex)
		{
			D3DVERTEXELEMENT9 tmp;
			tmp.Stream=stream;
			tmp.Offset=offset;
			tmp.Type=VertElementType2DX9(type);
			tmp.Method = D3DDECLMETHOD_DEFAULT;
			tmp.Usage=VertUsage2DX9(usage);
			tmp.UsageIndex=usageIndex;
			m_elements.push_back(tmp);
		}
		
		void DX9VertDecl::End()
		{
			D3DVERTEXELEMENT9 endElm=D3DDECL_END();
			m_elements.push_back(endElm);

			HRESULT hr=DX9Dev()->CreateVertexDeclaration(&m_elements[0],&m_pDecl);
			THROW_FAILED(hr,NULL);
		}
	}
}