#pragma once

enum EMiniMapElement
{
	EMME_LocalPlayer = 0,		// 本地玩家
	EMME_NatuRes,				// 采集点
	EMME_Nest,					// 怪物巢穴
	EMME_NormalMonster,         // 怪物
	EMME_Boss,					// BOSS
	EMME_NormalNPC,				// NPC
	EMME_AcceptQuestNPC,         // 可接任务NPC
	EMME_CompleteQuestNPC,      // 可交任务NPC
	EMME_Teammate,				// 队友
	EMME_OtherPlayer,			// 其他玩家
	EMME_Num
};

class Role;

/** \class   MiniMapStatic
    \brief   小地图控件
    \remarks 底图中保存前景图, ColorTex为地图，AlphaTex为Alpha图
             先使用Alpha图与地图混合把圆形地图渲染到RenderTarget，
			 然后再使用RenderTarget与前景图混合得到最终小地图控件
			 图像（因为直接把前景图渲染到RenderTarget上的话会产生
			 Alpha通道覆盖的现象）。
*/

class MiniMapStatic : public GUIStatic
{
public:
	struct tagMapData
	{
		Vector2 vWorldPos;
		Vector2 vWorldSize;
		tagMapData() : vWorldPos( 0.0f, 0.0f ), vWorldSize( 0.0f, 0.0f ) {}
	};
	MiniMapStatic();
	virtual ~MiniMapStatic();
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Update();
	virtual VOID Render();
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
	virtual VOID SetInvisible(BOOL b);

	virtual tagPoint PointToMap( tagPoint ptPointView );
	virtual tagPoint UISizeToMap( tagPoint ptUI );

	BOOL   SetAlphaTex( const tstring strFilename );
	BOOL   SetColorTex( const tstring strFilename );
	VOID   SetMapRect( const tagRect& rc ); 
	BOOL   SetElementImg( const INT nIndex, tstring strFilename );
	VOID   SetElementShow( const INT nIndex, BOOL bShow );
	BOOL   GetElementShow( const INT nIndex ) const;
	VOID   SetZoom( FLOAT fZoom );
	FLOAT  GetZoom() const { return m_fZoom; }

	// 地图数据
	VOID   ResetMapData( const tagMapData& data ) { m_MapData = data; }

	// 寻路目的点标记
	BOOL	SetDestMarkImg( const tstring& strFilename );
	VOID    ShowDestMark( DWORD dwMapID, const Vector2& vDest );
	VOID    CloseDestMark();

	// 本地玩家自动移动到地图的指定点
	VOID            LPAutoMoveTo( const Vector3& vDest, float fValidDist );
	VOID			LPAutoMoveTo( const Vector2& vDest, float fValidDist );
protected:
	tagGUIImage*   m_pColorTex;
	tagGUIImage*   m_pElementImg[EMME_Num];
	BOOL           m_bElementShow[EMME_Num];
private:
	tagPoint       m_ptCenterClient;       // 地图控件圆形区域的圆心
	INT            m_nRadius;              // 地图控件圆形区域的半径
	tagPoint       m_ptMapCenter;          // 当前显示地图图片的中心点位置
	tagGUIImage*   m_pAlphaTex;
	FLOAT          m_fZoom;
protected:
	tagMapData      m_MapData;
	tagGUIImage*	m_pImgDestMark;			// 寻路目的点图标
	BOOL            m_bShowDestMark;
	DWORD           m_dwDestMarkColor;
	Vector2			m_vDestMark;
	DWORD           m_dwDestMarkMapID;
protected:
	// 世界坐标与地图坐标的转换
	inline tagPoint WorldToMap( const Vector2& vPoint );
	inline Vector2  MapToWorld( const tagPoint& ptPoint );
	EMiniMapElement GetElementType( const Role* pRole );
	// 鼠标是否指向某个显示标记
	BOOL            IsMousePoint( const INT nIndex, const Vector2& vPosWorld, tagPoint& ptMouse );
private:
	// 鼠标坐标转换为地图坐标
	VOID            DrawLocalPlayer();
	VOID            DrawElement( const INT nIndex, const tagPoint& ptPos, FLOAT fRotate = 0.0f );
	BOOL			PointInMap( tagPoint& ptView );
	VOID			DrawImg( const tagGUIImage* pImg, const tagPoint& ptPos, DWORD dwMulColor = 0xFFFFFFFF, FLOAT fRotate = 0.0f );
};
