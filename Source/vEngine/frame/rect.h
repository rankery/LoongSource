//-----------------------------------------------------------------------------
//!\file rect.h
//!\author Lyp
//!
//!\date 2004-08-16
//! last 2004-08-16
//!
//!\brief 比windows提供的RECT更规范的XRect
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {
//-----------------------------------------------------------------------------
//!\brief 比windows提供的POINT更灵活的tagPoint
//-----------------------------------------------------------------------------
struct tagPoint
{
    LONG x, y;

	tagPoint():x(0),y(0){}
	tagPoint(INT n):x(n),y(n){}
	tagPoint(INT x, INT y):x(x),y(y){}
	
	bool operator==(const tagPoint& pt) { return (x==pt.x && y==pt.y); }

	tagPoint& Set(LONG x2, LONG y2) { x=x2; y=y2; return *this; }
	tagPoint& operator=(const tagPoint& pt) { x=pt.x;y=pt.y; return *this; }
	tagPoint& operator=(const POINT& pt) { x=pt.x;y=pt.y; return *this; }

	tagPoint operator+(const tagPoint& pt) {tagPoint ptNew; ptNew.x = x + pt.x; ptNew.y = y + pt.y; return ptNew;}
	tagPoint operator-(const tagPoint& pt) {tagPoint ptNew; ptNew.x = x - pt.x; ptNew.y = y - pt.y; return ptNew;}
	tagPoint operator*(const tagPoint& pt) {tagPoint ptNew; ptNew.x = x * pt.x; ptNew.y = y * pt.y; return ptNew;}
	tagPoint operator/(const tagPoint& pt) {tagPoint ptNew; ptNew.x = x / pt.x; ptNew.y = y / pt.y; return ptNew;}
	tagPoint operator+(INT t) {tagPoint ptNew; ptNew.x = x + t; ptNew.y = y + t; return ptNew;}
	tagPoint operator-(INT t) {tagPoint ptNew; ptNew.x = x - t; ptNew.y = y - t; return ptNew;}
	tagPoint operator*(INT t) {tagPoint ptNew; ptNew.x = x * t; ptNew.y = y * t; return ptNew;}
	tagPoint operator/(INT t) {tagPoint ptNew; ptNew.x = x / t; ptNew.y = y / t; return ptNew;}

	bool IsEmpty() const { return ( 0==x && 0==y ); }
	void Clear() { x=y=0; }
};


//-----------------------------------------------------------------------------
//!\brief 比windows提供的RECT更灵活的tagRect
//-----------------------------------------------------------------------------
struct tagRect
{
    LONG left, top, right, bottom;

	tagRect():left(0),top(0),right(0),bottom(0){}
	tagRect(INT n):left(n),top(n),right(n),bottom(n){}
	tagRect(INT n1, INT n2, INT n3, INT n4):left(n1),top(n2),right(n3),bottom(n4){}
	tagRect(tagPoint& ptStart)	// 设置left,top
	{ left=ptStart.x;top=ptStart.y;right=left;bottom=top; }
	tagRect(tagPoint& ptStart, tagPoint& ptSize)	// 设置left,top,宽,高
	{ left=ptStart.x;top=ptStart.y;right=left+ptSize.x;bottom=top+ptSize.y; }

	tagRect& Set(LONG x1, LONG y1, LONG x2, LONG y2)	// 直接设置left/top/right/bottom
	{ left=x1;top=y1;right=x2;bottom=y2; return *this; }
	tagRect& SetLTWH(LONG x, LONG y, LONG w, LONG h)		// 设置left,top,宽,高
	{ left=x;top=y;right=x+w;bottom=y+h; return *this; }
	tagRect& SetLTWH(const tagPoint& ptStart, const tagPoint& ptSize)		// 设置left,top,宽,高
	{ left=ptStart.x;top=ptStart.y;right=left+ptSize.x;bottom=top+ptSize.y; return *this; }

	tagRect& operator=(const tagPoint& pt) {left=0;top=0;right=pt.x;bottom=pt.y; return *this;}
	tagRect& operator=(const tagRect& rc)
	{	
		left=rc.left;top=rc.top;right=rc.right;bottom=rc.bottom;
		return *this;
	}
	tagRect& operator=(const RECT& rc)
	{	
		left=rc.left;top=rc.top;right=rc.right;bottom=rc.bottom;
		return *this;
	}
	tagRect operator+(const tagPoint& pt)	// 加上偏移
	{	
		tagRect rcNew; 
		rcNew.left=left+pt.x; rcNew.top=top+pt.y; rcNew.right=right+pt.x; rcNew.bottom=bottom+pt.y; 
		return rcNew;
	}
	tagRect operator-(const tagPoint& pt)	// 加上偏移
	{	
		tagRect rcNew; 
		rcNew.left=left-pt.x; rcNew.top=top-pt.y; rcNew.right=right-pt.x; rcNew.bottom=bottom-pt.y; 
		return rcNew;
	}
	tagRect operator+(INT n)	// 加上偏移
	{	
		tagRect rcNew; 
		rcNew.left=left+n; rcNew.top=top+n; rcNew.right=right+n; rcNew.bottom=bottom+n; 
		return rcNew;
	}

	bool IsContain(INT nX, INT nY) const 	// 指定点是否在矩形内
	{ return (nX>=left && nX < right && nY>=top && nY<bottom); }
	bool IsContain(const tagPoint& pt) const	// 指定点是否在矩形内
	{ return (pt.x>=left && pt.x<right && pt.y>=top && pt.y<bottom); }
	
	bool IsEmpty() const { return ( 0==left && 0==right && 0==top && 0==bottom); }
	void Clear() { left=top=right=bottom=0; }

};



}	// namespace vEngine{
