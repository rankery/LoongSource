
#ifdef Cool3D_EXPORTS
#define Cool3D_API __declspec(dllexport)
#else
#define Cool3D_API __declspec(dllimport)
#endif

#pragma once
#include <D3D9.h>
#include <d3dx9math.h>
#pragma comment(lib,"d3dx9.lib")

#include ".\Util\UnicodeUtil.h"

/** \mainpage Cool3D Engine
	\section Design
	\par
	设计目标: 
	<ol>
	<li>场景核心数据管理+碰撞检测作为核心算法, 与外围算法--渲染分开, 渲染算法可根据显卡级别
	以及API而设计出不同的strategy; 
		</li>
	</ol>

	\section Features
	\par

	\todo 是否支持序列化??
	
	\version 1.0.0
	\author	文斌
	\date	创建于2008年4月

	\par
	注意事项:
	<ol>
		<li>外部项目必须使用多线程运行库</li>
		<li>外部项目需要在Pch中加入常用的std模板,否则可能会出现link error</li>
	</ol>
*/
