#pragma once

const float g_collideStep		= 50.0f;		//碰撞步长
const float g_moveFall			= 100.0f;		//移动时最大上下落差
const float g_floatMax			= 1000000.0f;	//最大浮点数
const float g_tileSize			= 50.0f;		//格子大小
const float g_slideFall			= 50.0f;		//地形高度图两个点滑落落差
const float g_gravitation		= 1200.0f;		//重力加速度		
const float g_halfGravitation	= 600.0f;		//重力加速度*0.5f			


inline POINT _World2Tile(Vector3& pos)
{
	POINT tile;
	tile.x=long(pos.x/g_tileSize);
	tile.y=long(pos.z/g_tileSize);
	return tile;
}

inline POINT _World2Tile(float x,float z)
{
	POINT tile;
	tile.x=long(x/g_tileSize);
	tile.y=long(z/g_tileSize);
	return tile;
}