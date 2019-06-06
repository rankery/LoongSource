
#include "RTTIObj.h"
#include "RTTInfo.h"
#pragma once
#include "..\Cool3D.h"
#include "Exception.h"
#include "..\VFS\IFS.h"
#include <string>
#include "..\Math\Math.h"
#include "..\Math\Color.h"

namespace Cool3D
{
	/**	\class FArchive
		\brief 序列化所使用的目标文档
		\remarks 序列化适合用于处理复杂的对象,例如树形接口存储的对象
	*/
	class Cool3D_API FArchive
	{
	public:
		FArchive(void);
		virtual ~FArchive(void);

		void OpenForWrite(const TCHAR* szFileName);
		void OpenForRead(IFS* pFS,const TCHAR* szFileName);
		void Close();


		//--color
		friend FArchive& operator<<(FArchive& ar,const Color4f& color)
		{
			ar.ValueWrite(color.R);
			ar.ValueWrite(color.G);
			ar.ValueWrite(color.B);
			ar.ValueWrite(color.A);
			return ar;
		}
		friend FArchive& operator>>(FArchive& ar,Color4f& color)
		{
			ar.ValueRead(color.R);
			ar.ValueRead(color.G);
			ar.ValueRead(color.B);
			ar.ValueRead(color.A);
			return ar;
		}
		friend FArchive& operator<<(FArchive& ar,const Color4ub& color)
		{
			ar.ValueWrite(color.R);
			ar.ValueWrite(color.G);
			ar.ValueWrite(color.B);
			ar.ValueWrite(color.A);
			return ar;
		}
		friend FArchive& operator>>(FArchive& ar,Color4ub& color)
		{
			ar.ValueRead(color.R);
			ar.ValueRead(color.G);
			ar.ValueRead(color.B);
			ar.ValueRead(color.A);
			return ar;
		}
		//--math
		friend FArchive& operator<<(FArchive& ar,const Vector2& vec)
		{
			ar.ValueWrite(vec.x);
			ar.ValueWrite(vec.y);
			return ar;
		}
		friend FArchive& operator>>(FArchive& ar,Vector2& vec)
		{
			ar.ValueRead(vec.x);
			ar.ValueRead(vec.y);
			return ar;
		}
		friend FArchive& operator<<(FArchive& ar,const Vector3& vec)
		{
			ar.ValueWrite(vec.x);
			ar.ValueWrite(vec.y);
			ar.ValueWrite(vec.z);
			return ar;
		}
		friend FArchive& operator>>(FArchive& ar,Vector3& vec)
		{
			ar.ValueRead(vec.x);
			ar.ValueRead(vec.y);
			ar.ValueRead(vec.z);
			return ar;
		}
		friend FArchive& operator<<(FArchive& ar,const Vector4& vec)
		{
			ar.ValueWrite(vec.x);
			ar.ValueWrite(vec.y);
			ar.ValueWrite(vec.z);
			ar.ValueWrite(vec.w);
			return ar;
		}
		friend FArchive& operator>>(FArchive& ar,Vector4& vec)
		{
			ar.ValueRead(vec.x);
			ar.ValueRead(vec.y);
			ar.ValueRead(vec.z);
			ar.ValueRead(vec.w);
			return ar;
		}
		friend FArchive& operator<<(FArchive& ar,const Quaternion& quat)
		{
			ar.ValueWrite(quat.x);
			ar.ValueWrite(quat.y);
			ar.ValueWrite(quat.z);
			ar.ValueWrite(quat.w);
			return ar;
		}
		friend FArchive& operator>>(FArchive& ar,Quaternion& quat)
		{
			ar.ValueRead(quat.x);
			ar.ValueRead(quat.y);
			ar.ValueRead(quat.z);
			ar.ValueRead(quat.w);
			return ar;
		}
		friend FArchive& operator<<(FArchive& ar,const Matrix4& mat)
		{
			ar.BufferWrite(mat.m,sizeof(float)*16);
			return ar;
		}
		friend FArchive& operator>>(FArchive& ar,Matrix4& mat)
		{
			ar.BufferRead(mat.m,sizeof(float)*16);
			return ar;
		}
		//--string
		friend FArchive& operator<<(FArchive& ar,const TCHAR* szStr)
		{
			ASSERT(szStr!=NULL);
			size_t len=_tcslen(szStr)+1;
			ar.ValueWrite(len);
			ar.BufferWrite(szStr,len*sizeof(TCHAR));//写入字符串和结尾的0
			return ar;
		}
		friend FArchive& operator<<(FArchive& ar,tstring& szStr)
		{
			return ar<<szStr.c_str();
		}
		friend FArchive& operator>>(FArchive& ar,tstring& szStr)
		{
			unsigned int len=0;
			ar.ValueRead(len);
			if(len==0)
			{
				szStr=_T("");
				return ar;
			}
			TCHAR* szTmp=new TCHAR[len];
			ar.BufferRead(szTmp,len*sizeof(TCHAR));
			szStr=szTmp;
			delete[] szTmp;
			return ar;
		}

		//--int
		friend FArchive& operator<<(FArchive& ar,int n)
		{
			ar.ValueWrite(n);
			return ar;
		}
		friend FArchive& operator>>(FArchive& ar,int& n)
		{
			ar.ValueRead(n);
			return ar;
		}
		friend FArchive& operator<<(FArchive& ar,unsigned int n)
		{
			ar.ValueWrite(n);
			return ar;
		}
		friend FArchive& operator>>(FArchive& ar,unsigned int& n)
		{
			ar.ValueRead(n);
			return ar;
		}
		//short
		friend FArchive& operator<<(FArchive& ar,short n)
		{
			ar.ValueWrite(n);
			return ar;
		}
		friend FArchive& operator>>(FArchive& ar,short& n)
		{
			ar.ValueRead(n);
			return ar;
		}
		friend FArchive& operator<<(FArchive& ar,unsigned short n)
		{
			ar.ValueWrite(n);
			return ar;
		}
		friend FArchive& operator>>(FArchive& ar,unsigned short& n)
		{
			ar.ValueRead(n);
			return ar;
		}
		//char
		friend FArchive& operator<<(FArchive& ar,char n)
		{
			ar.ValueWrite(n);
			return ar;
		}
		friend FArchive& operator>>(FArchive& ar,char& n)
		{
			ar.ValueRead(n);
			return ar;
		}
		friend FArchive& operator<<(FArchive& ar,unsigned char n)
		{
			ar.ValueWrite(n);
			return ar;
		}
		friend FArchive& operator>>(FArchive& ar,unsigned char& n)
		{
			ar.ValueRead(n);
			return ar;
		}
		//float
		friend FArchive& operator<<(FArchive& ar,float n)
		{
			ar.ValueWrite(n);
			return ar;
		}
		friend FArchive& operator>>(FArchive& ar,float& n)
		{
			ar.ValueRead(n);
			return ar;
		}
		//double
		friend FArchive& operator<<(FArchive& ar,double n)
		{
			ar.ValueWrite(n);
			return ar;
		}
		friend FArchive& operator>>(FArchive& ar,double& n)
		{
			ar.ValueRead(n);
			return ar;
		}
		//bool
		friend FArchive& operator<<(FArchive& ar,bool n)
		{
			ar.ValueWrite(n);
			return ar;
		}
		friend FArchive& operator>>(FArchive& ar,bool& n)
		{
			ar.ValueRead(n);
			return ar;
		}
	public:
		//--对于简单的int等值类型的读写
		template<typename ValType>
			void ValueWrite(ValType& val)
		{
			ASSERT(m_pWriteFile!=NULL);
			if(m_pWriteFile)
				fwrite(&val,sizeof(ValType),1,m_pWriteFile);
		}
		template<typename ValType>
			void ValueRead(ValType& val)
		{
			ASSERT(m_pFS!=NULL && m_hReadFile!=NULL);
			if(m_pFS)
				m_pFS->Read(&val,sizeof(ValType),m_hReadFile);
		}
		//--写入buffer
		void BufferWrite(const void *pBuf,DWORD size)
		{
			ASSERT(m_pWriteFile!=NULL);
			if(m_pWriteFile)
				fwrite(pBuf,size,1,m_pWriteFile);
		}
		void BufferRead(void *pBuf,DWORD size)
		{
			ASSERT(m_pFS!=NULL && m_hReadFile!=NULL);
			if(m_pFS)
				m_pFS->Read(pBuf,size,m_hReadFile);
		}
	protected:
		int		m_mode;	//-1无效,0为OpenFroRead(),1为OpenForWrite
		FILE*	m_pWriteFile;	//写入时使用stdio file system
		IFS*	m_pFS;
		DWORD	m_hReadFile;	//读入时使用虚拟文件系统
	};

	/**	\class ISerializable
		\brief 序列化接口
		\remarks ISerializable是一个纯接口class,外部class可以多继承
	*/
	class Cool3D_API ISerializable
	{
	public:
		ISerializable()	{}
		virtual ~ISerializable()	{}

		virtual void Serialize(FArchive& ar) =0;
		virtual void Deserialize(FArchive& ar) =0;
	};

	/**	将一个保存有ISerializable派生类对象"指针"的vector序列化
		\remarks 为了支持逆序列化,object必须是可动态创建的RTTIObj
	*/
	template<typename SerType>
		void SerialObjPtrVector(FArchive& ar,vector<SerType*>& objPtrVector)
	{
		unsigned int size=objPtrVector.size();
		ar<<size;
		if(size==0)//没有任何元素
			return;
		//--
		for(vector<SerType*>::iterator iter=objPtrVector.begin();
			iter!=objPtrVector.end();iter++)
		{
			SerType* obj=(*iter);
			//--保存class name
			const RTTInfo *pClass=obj->GetRTTInfo();
			ar<<pClass->m_szClassName;
			ASSERT(pClass->m_pfnCreateObj!=NULL);

			obj->Serialize(ar);
		}
	}
	template<typename SerType>
		void DeserialObjPtrVector(FArchive& ar,vector<SerType*>& objPtrVector)
	{
		unsigned int size=0;
		ar>>size;
		if(size==0)
			return;
		//--
		ASSERT(objPtrVector.size()==0);//里面不能有任何元素
		objPtrVector.resize(size);
		for(vector<SerType*>::iterator iter=objPtrVector.begin();
			iter!=objPtrVector.end();iter++)
		{
			tstring szClassName;
			ar>>szClassName;
			SerType* obj=(SerType*)RTTFactory::Inst()->CreateObj(szClassName.c_str());
			obj->Deserialize(ar);
			(*iter)=obj;
		}
	}
	/**	将一个存有ISerializable派生类对象的vector序列化
	*/
	template<typename SerType>
		void SerialObjVector(FArchive& ar,vector<SerType>& objVector)
	{
		unsigned int size=objVector.size();
		ar<<size;
		if(size==0)
			return;
		for(vector<SerType>::iterator iter=objVector.begin();
			iter!=objVector.end();iter++)
		{
			SerType& obj=(*iter);
			obj.Serialize(ar);
		}
	}
	template<typename SerType>
		void DeserialObjVector(FArchive& ar,vector<SerType>& objVector)
	{
		objVector.clear();
		//--
		unsigned int size=0;
		ar>>size;
		if(size==0)
			return;
		objVector.resize(size);
		for(vector<SerType>::iterator iter=objVector.begin();
			iter!=objVector.end();iter++)
		{
			SerType& obj=(*iter);
			obj.Deserialize(ar);
		}
	}
	//--对一个简单类型(int,float等)的vector进行序列化
	template<typename ValType>
		void SerialValVector(FArchive& ar,vector<ValType>& valVector)
	{
		unsigned int size=valVector.size();
		ar<<size;
		if(size==0)
			return;
		//for(vector<ValType>::iterator iter=valVector.begin();
		//	iter!=valVector.end();iter++)
		//{
		//	ar.ValueWrite(*iter);
		//}
		ar.BufferWrite(&valVector[0],size*sizeof(ValType));
	}
	template<typename ValType>
		void DeserialValVector(FArchive& ar,vector<ValType>& valVector)
	{
		valVector.clear();
		//--
		unsigned int size=0;
		ar>>size;
		if(size==0)
			return;
		valVector.resize(size);
		//for(vector<ValType>::iterator iter=valVector.begin();
		//	iter!=valVector.end();iter++)
		//{
		//	ar.ValueRead(*iter);
		//}
		ar.BufferRead(&valVector[0],size*sizeof(ValType));
	}
}//namespace Cool3D