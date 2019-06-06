#pragma once

#include <list>
using namespace std;

/** 在矩形中的旋转器
*/
class RotaterInRect 
{
	struct PNT{
		short x;
		short y;		
	};
public:
	RotaterInRect();
	~RotaterInRect();

	int GetPointList( float fRatio, list<POINT>& list, int nLength );	// 根据弧度，取多边形列表， 通过nLength/m_nLength计算比率
	void InitSquare();													// 此方法目前仅适用于正方形
private:
	int	GetPoint( float fRadian );										// 根据弧度，取相应的坐标点。
	void SetRect( short nWidth, short nHeight );						// 设置矩形，计算相应坐标点。
																		// 可以通过小正方形，按比率计算大正方形的角度值

	POINT CalcPOINT( const PNT& pnt, int nLength ){
		POINT p;
		p.x = pnt.x * nLength / m_nLength;
		p.y = pnt.y * nLength / m_nLength;
		return p;
	}
	
private:
	int			m_nCount;		// 数组大小
	PNT			m_pPt[360];		// 冷却状态时，
	int			m_nLength;		// 正方形边长
};
