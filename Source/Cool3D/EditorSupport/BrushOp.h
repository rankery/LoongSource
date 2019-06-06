#pragma once
#include "..\Math\math.h"
#include "..\Math\Noise.h"
#include ".\TerrainEditor.h"
#include "..\Terrain\Heightmap.h"
#include "..\Terrain\BitMap.h"
#include ".\BrushOPRecorder.h"

namespace Cool3D
{
#pragma warning( disable : 4244 )
	namespace Detail
	{
		/**	如果x==0则返回a
		*/
		template<class T>
			T Lerp(T a,T b,float x,ELerpType type=ELerp_Cosine)
		{
			const float e=19.0f/7;
			switch(type)
			{
			case ELerp_None:
				return b;
			case ELerp_Cosine:
				x=(1-cos(x*3.1415927f))*0.5f;
				break;
			case ELerp_Quadratic:
				x=x*x;
				break;
			default://case ELerp_Linear:
				break;
			}

			return (T)(a*(1-x)+b*x);
		}

		template<typename BrushOp>
			void DoBrush(Heightmap *pMap,const tagTEditBrush& brush,BrushOp& op)
		{
			if(pMap==NULL)
				return;
			switch(brush.shape)
			{
			case EShape_Quad:
				{
					int strip=brush.outer-brush.inner;
					int x,z;
					for(z=brush.centerZ-brush.outer;z<brush.centerZ+brush.outer;z++)
					{
						if(z<0 || z>=(int)pMap->Height())
							continue;
						for(x=brush.centerX-brush.outer;x<brush.centerX+brush.outer;x++)
						{
							if(x<0 || x>=(int)pMap->Width())
								continue;
							if(z>=brush.centerZ-brush.inner
								&& z<brush.centerZ+brush.inner
								&& x>=brush.centerX-brush.inner
								&& x<brush.centerX+brush.inner)
								op.OnInner(pMap,x,z,brush.strength);
							else//在内圈和外圈之间
							{
								if(strip>0)
								{
									//离内圈的距离
									float zdist=abs(z-brush.centerZ)-brush.inner;
									float xdist=abs(x-brush.centerX)-brush.inner;
									float r=max((zdist/strip),(xdist/strip));

									float a=(float)brush.strength;
									float v=Lerp(a,0.0f,r,brush.lerp);
									op.OnOuter(pMap,x,z,(int)v,r,brush.lerp);
								}
							}
						}
					}//endof for(z)
				}
				break;
			case EShape_Circle:
				{
					//圆形
					POINT	tCenter={brush.centerX,brush.centerZ};
					//圆内径
					
					int x,z;
					for(z = tCenter.y-brush.outer;
						z <= tCenter.y+brush.outer;
						z++)
					{
						if(z<0 || z>=(int)pMap->Height())
							continue;
						for(x = tCenter.x-brush.outer;
							x <= tCenter.x+brush.outer;
							x++)
						{
							if(x<0 || x>=(int)pMap->Width())
								continue;

							int cx=x-tCenter.x;
							int cz=z-tCenter.y;//????cx,cz如果是float就会有问题
							float dist=sqrtf(cx*cx+cz*cz);
							if(dist > brush.outer)
								continue;
							if(dist<=brush.inner)//在内径范围内,则直接设置brush的值
								op.OnInner(pMap,x,z,brush.strength);
							else//在内径和外径之间,则对brush的值进行插值后在设置
							{
								float r=(dist-brush.inner)/(brush.outer-brush.inner);//离开内径越远r值越大,在内径上返回0
								float a=(float)brush.strength;
								float v=Lerp(a,0.0f,r,brush.lerp);
								op.OnOuter(pMap,x,z,(int)v,r,brush.lerp);
							}
						}
					}//endof for(z)
				}//endof else
				break;
			case EShape_Line:		//？？事实上线型的刷子编辑行走高度只适用于轴对齐的情况
				{	// 将笔刷的中心点作为线段的起始点
					POINT	tCenter={brush.centerX,brush.centerZ};

					int x,z;
					x = tCenter.x; z=tCenter.y;
					// 笔刷的内径作为线段的长度，笔刷的外径作为线段与X轴的角度
					for(int i=1;i<brush.inner;i++)
					{
						if(z<0 || z>=(int)pMap->Height())
							continue;
						if(x<0 || x>=(int)pMap->Width())
							continue;
						
						x = tCenter.x + cosf(_DegToRad(brush.outer))*i;
						z = tCenter.y + sinf(_DegToRad(brush.outer))*i;
						op.OnInner(pMap,x,z,brush.strength);
					}
				}
				break;
			case EShape_Plane:
				{	// 专为用于编辑类似于斜坡的行走高度设计
					int x,z;
					for(z=brush.centerZ-brush.inner+1;z<brush.centerZ+brush.inner;z++)
					{
						if(z<0 || z>=(int)pMap->Height())
							continue;
						for(x=brush.centerX-brush.inner+1;x<brush.centerX+brush.inner;x++)
						{
							if(x<0 || x>=(int)pMap->Width())
								continue;														
							op.OnInner(pMap,x,z,brush.strength);
						}
					}//end of for(z)
				}
				break;
			}// end of switch			
		}

		template<typename BrushOp>
		void DoBrush(BitMap *pMap,const tagTEditBrush& brush,BrushOp& op)
		{
			if(pMap==NULL)
				return;
			switch(brush.shape)
			{
			case EShape_Quad:
				{
					int strip=brush.outer-brush.inner;
					int x,z;
					for(z=brush.centerZ-brush.outer;z<brush.centerZ+brush.outer;z++)
					{
						if(z<0 || z>=(int)pMap->Height())
							continue;
						for(x=brush.centerX-brush.outer;x<brush.centerX+brush.outer;x++)
						{
							if(x<0 || x>=(int)pMap->Width())
								continue;
							if(z>=brush.centerZ-brush.inner
								&& z<brush.centerZ+brush.inner
								&& x>=brush.centerX-brush.inner
								&& x<brush.centerX+brush.inner)
								op.OnInner(pMap,x,z,brush.strength);
						}
					}//end of for(z)
				}
				break;
			case EShape_Circle:
				{
					//圆形
					POINT	tCenter={brush.centerX,brush.centerZ};
					//圆内径

					int x,z;
					for(z = tCenter.y-brush.outer;
						z <= tCenter.y+brush.outer;
						z++)
					{
						if(z<0 || z>=(int)pMap->Height())
							continue;
						for(x = tCenter.x-brush.outer;
							x <= tCenter.x+brush.outer;
							x++)
						{
							if(x<0 || x>=(int)pMap->Width())
								continue;

							int cx=x-tCenter.x;
							int cz=z-tCenter.y;//????cx,cz如果是float就会有问题
							float dist=sqrtf(cx*cx+cz*cz);
							if(dist > brush.outer)
								continue;
							if(dist<=brush.inner)//在内径范围内,则直接设置brush的值
								op.OnInner(pMap,x,z,brush.strength);
						}
					}//endof for(z)
				}//endof else
				break;
			case EShape_Line:		//？？事实上线型的刷子编辑行走高度只适用于轴对齐的情况
				{	// 将笔刷的中心点作为线段的起始点
					POINT	tCenter={brush.centerX,brush.centerZ};

					int x,z;
					x = tCenter.x; z=tCenter.y;
					// 笔刷的内径作为线段的长度，笔刷的外径作为线段与X轴的角度
					for(int i=1;i<brush.inner;i++)
					{
						if(z<0 || z>=(int)pMap->Height())
							continue;
						if(x<0 || x>=(int)pMap->Width())
							continue;

						x = tCenter.x + cosf(_DegToRad(brush.outer))*i;
						z = tCenter.y + sinf(_DegToRad(brush.outer))*i;
						op.OnInner(pMap,x,z,brush.strength);
					}
				}
				break;
			case EShape_Plane:
				{	// 专为用于编辑类似于斜坡的行走高度设计
					int x,z;
					for(z=brush.centerZ-brush.inner+1;z<brush.centerZ+brush.inner;z++)
					{
						if(z<0 || z>=(int)pMap->Height())
							continue;
						for(x=brush.centerX-brush.inner+1;x<brush.centerX+brush.inner;x++)
						{
							if(x<0 || x>=(int)pMap->Width())
								continue;														
							op.OnInner(pMap,x,z,brush.strength);
						}
					}//end of for(z)
				}
				break;
			}// end of switch
		}

		class BOP_Base
		{
		public:
			BOP_Base(BrushOpRecorder* pBrushRecorder=NULL)
			{
				m_brushOpRecorder=pBrushRecorder;
			}
			~BOP_Base()
			{
				//
			}
			void RecordOp(BrushOpRecorder::EBrushOpMode mode, int x, int z, int undoVal, int redoVal)
			{
				if (m_brushOpRecorder!=NULL)
					m_brushOpRecorder->AddOp(mode,x,z,undoVal,redoVal);
			}
		private:
			BrushOpRecorder	*m_brushOpRecorder;
		};

		/**	将brush的值加到height map上
		*/
		class BOP_AddBrush : public BOP_Base
		{
		public:
			BOP_AddBrush(BrushOpRecorder* pBrushRecorder=NULL):BOP_Base(pBrushRecorder)
			{
				//
			}
			void OnInner(Heightmap *pMap,int x,int z,int brushVal)
			{
				pMap->SafeAddValue(x,z,brushVal);
				RecordOp(BrushOpRecorder::EBOM_AddVal, x, z, -brushVal, brushVal);
			}
			void OnOuter(Heightmap *pMap,int x,int z,int brushVal,float ratio,ELerpType type)
			{
				pMap->SafeAddValue(x,z,brushVal);
				RecordOp(BrushOpRecorder::EBOM_AddVal, x, z, -brushVal, brushVal);
			}
		};


		/**计算Bursh内径范围内高度的平均值
		*/
		class BOP_CalAverage
		{
		public:
			BOP_CalAverage()
			{	count=0;sum=0;		}
			void OnInner(Heightmap *pMap,int x,int z,int burshVal)
			{
				count++;
				sum+=pMap->GetSafeVal(x,z);
			}
			void OnOuter(Heightmap *pMap,int x,int z,int burshVal,float ratio,ELerpType type)
			{}
			int GetResult()
			{
				return sum/count;
			}
			int GetCount()	{return count;}
		private:
			int count;
			int sum;
		};
		/**在内径范围内的height map设置为指定值,在内径和外径之间使用指定值和原来值插值
		*/
		class BOP_ApplyVal : public BOP_Base
		{
		public:
			BOP_ApplyVal(int val,BrushOpRecorder* pBrushRecorder=NULL):BOP_Base(pBrushRecorder)
			{
				m_val=val;
			}
			void OnInner(Heightmap *pMap,int x,int z,int brushVal)
			{
				int oldVal=pMap->GetSafeVal(x,z);
				pMap->SafeSetVal(x,z,m_val);
				RecordOp(BrushOpRecorder::EBOM_SetVal, x, z, oldVal, m_val);
			}
			void OnInner(BitMap *pMap,int x,int z,int brushVal)
			{
				if( brushVal>0 )
					pMap->SafeSetValue(x,z,true);
				else
					pMap->SafeSetValue(x,z,false);
			}
			void OnOuter(Heightmap *pMap,int x,int z,int burshVal,float ratio,ELerpType type)
			{
				int oldVal=pMap->GetSafeVal(x,z);
				int newVal=Lerp(m_val,oldVal,ratio,type);
				pMap->SafeSetVal(x,z,newVal);
				RecordOp(BrushOpRecorder::EBOM_SetVal, x, z, oldVal, newVal);
			}
		private:
			int m_val;
		};

		/**	产生一个边长为外径*2的噪音图像,在内径范围内使用噪音的值与原值相加,在外径范围内使用噪音值与0插值,再与原值相加
		*/
		class BOP_ApplyNoise : public BOP_Base
		{
		public:
			BOP_ApplyNoise(UINT noiseSize,float freq,int startX,int startZ,BrushOpRecorder* pBrushRecorder=NULL)
				:BOP_Base(pBrushRecorder)
			{
				if(fabsf(m_noise.GetFreq()-freq) > 0.01f)//优化:只有Freq变化时才重新生成,对于鼠标拖动会比较快(拖动brush时freq不会改变)
				{
					m_noise.SetFreq(freq);
					m_noise.Generate(noiseSize,noiseSize);
				}
				m_startX=startX;
				m_startZ=startZ;
			}
			void OnInner(Heightmap *pMap,int x,int z,int burshVal)
			{
				int nx=x-m_startX;
				int ny=z-m_startZ;
				if(nx<0
					|| nx>=(int)m_noise.Width()
					|| ny<0
					|| ny>=(int)m_noise.Height())
					return;
				float ns=m_noise.GetData(nx,ny)*255;
				ns-=128;
				ns=ns/128*burshVal;
				int i=ftol_ambient(ns);
				pMap->SafeAddValue(x,z,i);
				RecordOp(BrushOpRecorder::EBOM_AddVal, x, z, -i, i);
			}
			void OnOuter(Heightmap *pMap,int x,int z,int burshVal,float ratio,ELerpType type)
			{
				int nx=x-m_startX;
				int ny=z-m_startZ;
				if(nx<0
					|| nx>=(int)m_noise.Width()
					|| ny<0
					|| ny>=(int)m_noise.Height())
					return;
				float ns=m_noise.GetData(nx,ny)*255;
				ns-=128;
				ns=ns/128*burshVal;
				int i=ftol_ambient(ns);
				pMap->SafeAddValue(x,z,i);
				RecordOp(BrushOpRecorder::EBOM_AddVal, x, z, -i, i);
			}
		private:
			int		m_startX,
					m_startZ;//噪音图像的(0,0)对应height map上的哪个坐标
		public:
			static Noise	m_noise;
		};

		/** 对内径范围内的height map使用虑镜进行平滑,对于内径外径之间的heightmap使用平滑值与原值进行插值
			\todo 改进
		*/
		class BOP_SmoothFilter : public BOP_Base
		{
		public:
			BOP_SmoothFilter(int filterSize,BrushOpRecorder* pBrushRecorder=NULL):BOP_Base(pBrushRecorder)
			{
				ASSERT(filterSize > 0);
				size=filterSize;
				pFilter=(float*)malloc(sizeof(float)*filterSize*filterSize);

				float hSize=filterSize/2.0f;
				float r=sqrtf(hSize*hSize*2);
				filterSum=0;
				for(int y=0;y<filterSize;y++)
				{
					int cy=y-filterSize/2;
					for(int x=0;x<filterSize;x++)
					{
						int cx=x-filterSize/2;
						float myR=sqrtf(cx*cx+cy*cy);//当前点离开中心的距离
						float val=Lerp(1.0f,0.0f,myR/r,ELerp_Cosine);
						pFilter[y*filterSize+x]=val;
						filterSum+=val;
					}
				}
			}
			~BOP_SmoothFilter()
			{
				SAFE_FREE(pFilter);
			}
			void OnInner(Heightmap *pMap,int x,int z,int burshVal)
			{
				tagVal tv;
				tv.x=x;
				tv.z=z;
				tv.val=CalSmoothVal(pMap,x,z);
				m_valArray.push_back(tv);
			}

			void OnOuter(Heightmap *pMap,int x,int z,int burshVal,float ratio,ELerpType type)
			{
				float sv=CalSmoothVal(pMap,x,z);
				float ov=pMap->GetSafeVal(x,z);
				tagVal tv;
				tv.x=x;
				tv.z=z;
				tv.val=Lerp(sv,ov,ratio,type);
				m_valArray.push_back(tv);
			}
			void Apply(Heightmap *pMap)
			{
				int valArraySize=(int)m_valArray.size();
				for(int i=0;i<valArraySize;i++)
				{
					tagVal& tv=m_valArray[i];
					int oldVal=pMap->GetSafeVal(tv.x,tv.z);
					pMap->SafeSetVal(tv.x,tv.z,tv.val);
					RecordOp(BrushOpRecorder::EBOM_SetVal, tv.x, tv.z, oldVal, tv.val);
				}
			}
		private:
			struct tagVal
			{
				int x,z;
				int val;
			};
			vector<tagVal>	m_valArray;
			float CalSmoothVal(Heightmap *pMap,int x,int z)
			{
				float sum=0;
				int hSize=size/2;
				for(int fy=0;fy<size;fy++)
				{
					for(int fx=0;fx<size;fx++)
					{
						float f=pFilter[fy*size+fx];
						sum+=pMap->GetSafeVal(x+fx-hSize,z+fy-hSize)*f;
					}
				}
				return sum/filterSum+0.5f;//四舍五入
			}
			int	size;
			float *pFilter;
			float filterSum;
		};

		/**只是将BOP_SmoothFilter类型的ELerp_Cosine改成了ELerp_Linear
		*/
		class BOP_LineFilter : public BOP_Base
		{
		public:
			BOP_LineFilter(int filterSize,BrushOpRecorder* pBrushRecorder=NULL):BOP_Base(pBrushRecorder)
			{
				ASSERT(filterSize > 0);
				size=filterSize;
				pFilter=(float*)malloc(sizeof(float)*filterSize*filterSize);

				float hSize=filterSize/2.0f;
				float r=sqrtf(hSize*hSize*2);
				filterSum=0;
				for(int y=0;y<filterSize;y++)
				{
					int cy=y-filterSize/2;
					for(int x=0;x<filterSize;x++)
					{
						int cx=x-filterSize/2;
						float myR=sqrtf(cx*cx+cy*cy);//当前点离开中心的距离
						float val=Lerp(1.0f,0.0f,myR/r,ELerp_Linear);
						pFilter[y*filterSize+x]=val;
						filterSum+=val;
					}
				}
			}
			~BOP_LineFilter()
			{
				SAFE_FREE(pFilter);
			}
			void OnInner(Heightmap *pMap,int x,int z,int burshVal)
			{
				tagVal tv;
				tv.x=x;
				tv.z=z;
				tv.val=CalSmoothVal(pMap,x,z);
				m_valArray.push_back(tv);
			}

			void OnOuter(Heightmap *pMap,int x,int z,int burshVal,float ratio,ELerpType type)
			{
				float sv=CalSmoothVal(pMap,x,z);
				float ov=pMap->GetSafeVal(x,z);
				tagVal tv;
				tv.x=x;
				tv.z=z;
				tv.val=Lerp(sv,ov,ratio,type);
				m_valArray.push_back(tv);
			}
			void Apply(Heightmap *pMap)
			{
				int valArraySize=(int)m_valArray.size();
				for(int i=0;i<valArraySize;i++)
				{
					tagVal& tv=m_valArray[i];
					int oldVal=pMap->GetSafeVal(tv.x, tv.z);
					pMap->SafeSetVal(tv.x,tv.z,tv.val);
					RecordOp(BrushOpRecorder::EBOM_SetVal, tv.x, tv.z, oldVal, tv.val);
				}
			}
		private:
			struct tagVal
			{
				int x,z;
				int val;
			};
			vector<tagVal>	m_valArray;
			float CalSmoothVal(Heightmap *pMap,int x,int z)
			{
				float sum=0;
				int hSize=size/2;
				for(int fy=0;fy<size;fy++)
				{
					for(int fx=0;fx<size;fx++)
					{
						float f=pFilter[fy*size+fx];
						sum+=pMap->GetSafeVal(x+fx-hSize,z+fy-hSize)*f;
					}
				}
				return sum/filterSum+0.5f;//四舍五入
			}
			int	size;
			float *pFilter;
			float filterSum;
		};
#pragma warning( default : 4244 )
	}//namespace Detail
}//namespace Cool3D