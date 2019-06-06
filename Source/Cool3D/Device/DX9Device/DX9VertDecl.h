#pragma once
#include "..\istreamdefine.h"
#include <d3d9.h>

namespace Cool3D
{
	namespace Detail
	{
		class DX9VertDecl : public IStreamDefine
		{
		public:
			DX9VertDecl(void);
			virtual ~DX9VertDecl(void);

			IDirect3DVertexDeclaration9* GetD3D()
			{
				return m_pDecl;
			}

			virtual void Begin();
			virtual void AddElement(BYTE stream,BYTE offset,VertElementType type,VertElementSemantic usage,BYTE	usageIndex);
			virtual void End();

		private:
			IDirect3DVertexDeclaration9 *m_pDecl;
			vector<D3DVERTEXELEMENT9>	m_elements;
		};

	}
}