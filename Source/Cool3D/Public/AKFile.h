#pragma once
#include ".\MathBaseDefine.h"
#include ".\TagNodeDefine.h"
#include ".\FileUtil.h"

namespace Cool3D
{
	static inline UINT16 floatToHalfI(UINT32 i);
	static inline UINT32 halfToFloatI(UINT16 y);
	/** Convert a float32 to a float16 (NV_half_float)
	Courtesy of OpenEXR
	*/
	static inline UINT16 floatToHalf(float i)
	{
		union { float f; UINT32 i; } v;
		v.f = i;
		return floatToHalfI(v.i);
	}
	/**  
	*/
	static inline UINT16 floatToHalfI(UINT32 i)
	{
		register int s =  (i >> 16) & 0x00008000;
		register int e = ((i >> 23) & 0x000000ff) - (127 - 15);
		register int m =   i        & 0x007fffff;

		if (e <= 0)
		{
			if (e < -10)
			{
				return 0;
			}
			m = (m | 0x00800000) >> (1 - e);

			return s | (m >> 13);
		}
		else if (e == 0xff - (127 - 15))
		{
			if (m == 0) // Inf
			{
				return s | 0x7c00;
			} 
			else    // NAN
			{
				m >>= 13;
				return s | 0x7c00 | m | (m == 0);
			}
		}
		else
		{
			if (e > 30) // Overflow
			{
				return s | 0x7c00;
			}

			return s | (e << 10) | (m >> 13);
		}
	}

	/**
	* Convert a float16 (NV_half_float) to a float32
	* Courtesy of OpenEXR
	*/
	static inline float halfToFloat(UINT16 y)
	{
		union { float f; UINT32 i; } v;
		v.i = halfToFloatI(y);
		return v.f;
	}
	/** Converts a half in uint16 format to a float
	in uint32 format
	*/
	static inline UINT32 halfToFloatI(UINT16 y)
	{
		register int s = (y >> 15) & 0x00000001;
		register int e = (y >> 10) & 0x0000001f;
		register int m =  y        & 0x000003ff;

		if (e == 0)
		{
			if (m == 0) // Plus or minus zero
			{
				return s << 31;
			}
			else // Denormalized number -- renormalize it
			{
				while (!(m & 0x00000400))
				{
					m <<= 1;
					e -=  1;
				}

				e += 1;
				m &= ~0x00000400;
			}
		}
		else if (e == 31)
		{
			if (m == 0) // Inf
			{
				return (s << 31) | 0x7f800000;
			}
			else // NaN
			{
				return (s << 31) | 0x7f800000 | (m << 13);
			}
		}

		e = e + (127 - 15);
		m = m << 13;

		return (s << 31) | (e << 23) | m;
	}

	//--关键帧动画中的一帧
	struct VertFrame
	{
		DWORD					name;
		float					time;//秒
		std::vector<BYTE>	vertices;
		TagNodeFrame			tagNodes;
		std::vector<UINT16>		optimizeVerts;

		void WriteFile(FILE *fp, const VertFrame *preFrame, EVertType vertType)
		{
			FWriteValue(fp,name);
			FWriteValue(fp,time);
			//将vertices中的float32类型转换为uint16类型
			UINT vertSize=GetVertSize(vertType);
			UINT vertCount=vertices.size()/vertSize;
			optimizeVerts.resize(vertCount*vertSize/4);
			float *prop=(float*)&(*vertices.begin());
			UINT16 *prop16=&(*optimizeVerts.begin());
			for (UINT i=0;i<vertices.size();)
			{
				*prop16++=floatToHalf(*prop++);
				i+=4;
			}

			//压缩相邻帧的顶点数据
			std::vector<short> arrayDiff;
			std::vector<BYTE> arrayFlags;
			arrayFlags.resize(vertCount);
			//额外的顶点信息G T
			std::vector<short> arrayDiffEx;
			std::vector<BYTE> arrayFlagsEx;
			if (vertSize>GetVertSize(EVType_PNT))
				arrayFlagsEx.resize(vertCount);

			BYTE *pFlag=&(*arrayFlags.begin());
			BYTE *pFlagEx=&(*arrayFlagsEx.begin());

			for (UINT i=0;i<optimizeVerts.size();pFlag++)
			{
				prop16=&optimizeVerts[i];
				if (!preFrame)
				{
					*pFlag=0xFF;
					UINT oldIndex=arrayDiff.size();
					arrayDiff.resize(oldIndex+vertSize/4);
					for (UINT propIndex=0;propIndex<vertSize/4;propIndex++)
						arrayDiff[oldIndex+propIndex]=*prop16++;

					switch (vertType)
					{
					case EVType_PNT2:
						{
							*pFlagEx=0x18;
							arrayDiffEx.push_back(*prop16++);
							arrayDiffEx.push_back(*prop16++);
						}
						break;
					case EVType_PNGT:
						{
							*pFlagEx=0xE0;
							for (int diffIndex=0;diffIndex<3;diffIndex++)
								arrayDiffEx.push_back(*prop16++);
						}
						break;
					}
				}
				else
				{
					const UINT16 *preProp=&preFrame->optimizeVerts[i];

					//pos.x
					if (*prop16!=*preProp)
					{
						*pFlag=0x80;
						arrayDiff.push_back(*prop16-*preProp);
					}
					prop16++;preProp++;

					//pos.y
					if (*prop16!=*preProp)
					{
						*pFlag|=0x40;
						arrayDiff.push_back(*prop16-*preProp);
					}
					prop16++;preProp++;

					//pos.z
					if (*prop16!=*preProp)
					{
						*pFlag|=0x20;
						arrayDiff.push_back(*prop16-*preProp);
					}
					prop16++;preProp++;

					//normal.x
					if (*prop16!=*preProp)
					{
						*pFlag|=0x10;
						arrayDiff.push_back(*prop16-*preProp);
					}
					prop16++;preProp++;

					//normal.y
					if (*prop16!=*preProp)
					{
						*pFlag|=0x08;
						arrayDiff.push_back(*prop16-*preProp);
					}
					prop16++;preProp++;

					//normal.z
					if (*prop16!=*preProp)
					{
						*pFlag|=0x04;
						arrayDiff.push_back(*prop16-*preProp);
					}
					prop16++;preProp++;

					if (vertType==EVType_PNGT)
					{
						//tangent.x
						if (*prop16!=*preProp)
						{
							*pFlagEx|=0x80;
							arrayDiffEx.push_back(*prop16-*preProp);
						}
						prop16++;preProp++;
						//tangent.y
						if (*prop16!=*preProp)
						{
							*pFlagEx|=0x40;
							arrayDiffEx.push_back(*prop16-*preProp);
						}
						prop16++;preProp++;
						//tangent.z
						if (*prop16!=*preProp)
						{
							*pFlagEx|=0x20;
							arrayDiffEx.push_back(*prop16-*preProp);
						}
						prop16++;preProp++;
					}

					//uv.x
					if (*prop16!=*preProp)
					{
						*pFlag|=0x02;
						arrayDiff.push_back(*prop16-*preProp);
					}
					prop16++;preProp++;

					//uv.y
					if (*prop16!=*preProp)
					{
						*pFlag|=0x01;
						arrayDiff.push_back(*prop16-*preProp);
					}
					prop16++;preProp++;

					if (vertType==EVType_PNT2)
					{
						//uv2.x
						if (*prop16!=*preProp)
						{
							*pFlagEx|=0x10;
							arrayDiffEx.push_back(*prop16-*preProp);
						}
						prop16++;preProp++;
						//uv2.y
						if (*prop16!=*preProp)
						{
							*pFlagEx|=0x08;
							arrayDiffEx.push_back(*prop16-*preProp);
						}
					}
				}//end if else

				i+=vertSize/4;
			}//end for
			FWriteValVector(fp, arrayFlags);
			FWriteValVector(fp, arrayDiff);
			if (vertSize>GetVertSize(EVType_PNT))
			{
				FWriteValVector(fp, arrayFlagsEx);
				FWriteValVector(fp, arrayDiffEx);
			}
			tagNodes.WriteFile(fp);
		}
#ifdef VFS_READ
		void ReadFile(IFS *pFS,DWORD hFile,const VertFrame* preFrame, EVertType vertType)
		{
			FReadValue(pFS,hFile,name);
			FReadValue(pFS,hFile,time);

			std::vector<BYTE> arrayFlags;
			std::vector<short> arrayDiff;
			FReadValVector(pFS,hFile,arrayFlags);
			FReadValVector(pFS,hFile,arrayDiff);
			std::vector<BYTE> arrayFlagsEx;
			std::vector<short> arrayDiffEx;
			if (GetVertSize(vertType)>GetVertSize(EVType_PNT))
			{
				FReadValVector(pFS,hFile,arrayFlagsEx);
				FReadValVector(pFS,hFile,arrayDiffEx);
			}
			ConvertCompressToHalf(arrayFlags, arrayDiff, arrayFlagsEx, arrayDiffEx, 
				optimizeVerts, preFrame, vertType);

			//将uint16类型转换成float32类型
			vertices.clear();
			vertices.resize(optimizeVerts.size()*4);
			float *prop=(float *)&(*vertices.begin());
			const UINT16 *prop16=&(*optimizeVerts.begin());
			for (UINT i=0;i<optimizeVerts.size();i++)
				*prop++=halfToFloat(*prop16++);

			tagNodes.ReadFile(pFS,hFile);
		}
#else
		void ReadFile(FILE *fp, const VertFrame* preFrame, EVertType vertType)
		{
			FReadValue(fp,name);
			FReadValue(fp,time);

			std::vector<BYTE> arrayFlags;
			std::vector<short> arrayDiff;
			FReadValVector(fp,arrayFlags);
			FReadValVector(fp,arrayDiff);
			std::vector<BYTE> arrayFlagsEx;
			std::vector<short> arrayDiffEx;
			if (GetVertSize(vertType)>GetVertSize(EVType_PNT))
			{
				FReadValVector(fp,arrayFlagsEx);
				FReadValVector(fp,arrayDiffEx);
			}
			ConvertCompressToHalf(arrayFlags, arrayDiff, arrayFlagsEx, arrayDiffEx, 
				optimizeVerts, preFrame, vertType);

			//将uint16类型转换成float32类型
			vertices.clear();
			vertices.resize(optimizeVerts.size()*4);
			float *prop=(float *)&(*vertices.begin());
			const UINT16 *prop16=&(*optimizeVerts.begin());
			for (int i=0;i<optimizeVerts.size();i++)
				*prop++=halfToFloat(*prop16++);

			tagNodes.ReadFile(fp);
		}
#endif

		VertFrame()
		{}
		VertFrame(const VertFrame& other)
		{
			name		=other.name;
			vertices	=other.vertices;
			tagNodes	=other.tagNodes;
		}
		const VertFrame& operator = (const VertFrame& other)
		{
			if(this == &other)
				return *this;
			name=other.name;
			vertices=other.vertices;
			tagNodes	=other.tagNodes;
			return *this;
		}

	private:
		void ConvertCompressToHalf(const std::vector<BYTE>& arrayFlags, 
			const std::vector<short>& arrayDiff,
			const std::vector<BYTE>& arrayFlagsEx,
			const std::vector<short>& arrayDiffEx,
			std::vector<UINT16>& halfVerts,
			const VertFrame* preFrame, EVertType vertType)
		{
			UINT vertSize=GetVertSize(vertType)/2;
			halfVerts.resize(arrayFlags.size()*vertSize/2);
			if (!preFrame)
			{
				memcpy(&(halfVerts[0]), &(arrayDiff[0]), arrayDiff.size()*2);
			}
			else
			{
				UINT diffIndex=0;
				UINT diffIndexEx=0;
				for (UINT i=0;i<arrayFlags.size();i++)
				{
					UINT16 *prop16=&halfVerts[i*vertSize/2];
					const UINT16 *preprop16=&preFrame->optimizeVerts[i*vertSize/2];
					const BYTE& flag=arrayFlags[i];
					memcpy(prop16, preprop16, vertSize);

					int dataIndex=0;
					//pos.x
					if (flag&0x80)
						*(prop16+dataIndex)+=arrayDiff[diffIndex++];
					dataIndex++;

					//pos.y
					if (flag&0x40)
						*(prop16+dataIndex)+=arrayDiff[diffIndex++];
					dataIndex++;

					//pos.z
					if (flag&0x20)
						*(prop16+dataIndex)+=arrayDiff[diffIndex++];
					dataIndex++;

					//normal.x
					if (flag&0x10)
						*(prop16+dataIndex)+=arrayDiff[diffIndex++];
					dataIndex++;

					//normal.y
					if (flag&0x08)
						*(prop16+dataIndex)+=arrayDiff[diffIndex++];
					dataIndex++;

					//normal.z
					if (flag&0x04)
						*(prop16+dataIndex)+=arrayDiff[diffIndex++];
					dataIndex++;

					if (vertType==EVType_PNGT)
					{
						const BYTE& flagEx=arrayFlagsEx[i];
						//tangent.x
						if (flagEx&0x80)
							*(prop16+dataIndex)+=arrayDiffEx[diffIndexEx++];
						dataIndex++;
						//tangent.y
						if (flagEx&0x40)
							*(prop16+dataIndex)+=arrayDiffEx[diffIndexEx++];
						dataIndex++;
						//tangent.z
						if (flagEx&0x20)
							*(prop16+dataIndex)+=arrayDiffEx[diffIndexEx++];
						dataIndex++;
					}

					//uv.x
					if (flag&0x02)
						*(prop16+dataIndex)+=arrayDiff[diffIndex++];
					dataIndex++;

					//uv.y
					if (flag&0x01)
						*(prop16+dataIndex)+=arrayDiff[diffIndex++];
					dataIndex++;

					if (vertType==EVType_PNT2)
					{
						const BYTE& flagEx=arrayFlagsEx[i];
						//uv2.x
						if (flagEx&0x10)
							*(prop16+dataIndex)+=arrayDiffEx[diffIndexEx++];
						dataIndex++;
						//uv2.y
						if (flagEx&0x08)
							*(prop16+dataIndex)+=arrayDiffEx[diffIndexEx++];
					}
				}//end for
			}
		}//end func
	};
	//---------------------
	const TCHAR* const AK_MAGIC=_T("AKK");

	struct AKHeader
	{
		TCHAR	magic[4];
		int		ver;
		EVertType	vertType;
		
		DWORD	numVert;
		
		DWORD	numMesh;
		DWORD	offsetMesh;
		DWORD	numMtl;
		DWORD	offsetMtl;
		DWORD	numFrame;
		DWORD	offsetFrame;

		Vector3 max,min;

		int		lightmapSize;

		DWORD	reserve[63];
	};
};