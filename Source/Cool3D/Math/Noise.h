#pragma once
#include "..\Cool3D.h"

namespace Cool3D
{
	/**	\class Noise
		\brief дывє
	*/
	class Cool3D_API Noise
	{
		typedef float DataType;
	public:
		Noise(void);
		virtual ~Noise(void);

		void SetFreq(float freq)	{	m_frequency=freq; }
		void SetParam(float amplitude,UINT numOfOctaves);
		void Generate(UINT width,UINT height);
		void ReGenerate();
		void SaveToRAW(const TCHAR* szFileName);
		float GetData(UINT x,UINT y);
		UINT Width()	{	return m_width;}
		UINT Height()	{	return m_height;}
		float GetFreq()	{	return m_frequency;}

	private:
		void Destroy();
	private:
		float		m_frequency,
					m_amplitude;
		UINT		m_numOfOctaves;
		UINT		m_width,
					m_height;
		DataType	*m_pData;
	};
}//namespace Cool3D