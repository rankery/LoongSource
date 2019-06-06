#pragma once

/**
	class: AvatarCfg
	brief: 采用单键模式，由程序来指定模型、纹理帖图的路径
*/
#define AVATAR_HAIR_TYPE 5
#define AVATAR_HAIR_COLOR 6
#define AVATAR_FACE 5
#define AVATAR_FACE_DETAIL 7
#define AVATAR_DRESS 3           // 可选新手时装样式的数目

class AvatarCfg
{
public:
	AvatarCfg(void);
	~AvatarCfg(void);
	static AvatarCfg* Inst();

	int GetNumHairType()const	{ return AVATAR_HAIR_TYPE; }
	int GetNumHairColor()const	{ return AVATAR_HAIR_COLOR; }
	int GetNumFace()const		{ return AVATAR_FACE; }
	int GetNumFaceDetail()const	{ return AVATAR_FACE_DETAIL; }
	int GetNumDress()const		{ return AVATAR_DRESS; }

	const TCHAR* GetHairTypeMdlPath(BYTE bySex, WORD id);						//头发模型路径
	const TCHAR* GetHairColorTexPath(BYTE bySex, WORD hairID, WORD colorID);	//头发纹理贴图路径
	const TCHAR* GetFaceMdlPath(BYTE bySex, WORD id);							//面部模型路径
	const TCHAR* GetFaceDtlTexPath(BYTE bySex, WORD id);						//面部纹理贴图路径
	/** \获取内衣模型的路径
	*/
	const TCHAR* GetUpperUWMdlPath(BYTE bySex,BOOL bFashion);					//上衣模型路径
	const TCHAR* GetLowerUWMdlPath(BYTE bySex,BOOL bFashion);					//裤子模型路径
	const TCHAR* GetBootUWMdlPath(BYTE bySex,BOOL bFashion);					//鞋模型路径	
	const TCHAR* GetGloveUWMdlPath(BYTE bySex,BOOL bRight,BOOL bFashion);		//手套型路路径(分左右)

	/** \根据性别及序号生成一套新手时装结构
	*/
	void MakeNewPlayerFahion( const BYTE bySex, const int n, tagAvatarEquip& out ) const;
private:
	tstring							m_strPath;
};
