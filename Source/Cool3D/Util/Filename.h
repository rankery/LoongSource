#pragma once
#include ".\UnicodeUtil.h"

namespace Cool3D
{
	class Filename : public tstring
	{
	public:
		Filename( )                                            { }
		Filename(const TCHAR* pFilename) : tstring(pFilename)    { }
		Filename(tstring strFilename) :tstring(strFilename) { }

		Filename GetExtension(void);      // if filename is "/idv/code/file.cpp", it returns "cpp"
		Filename GetPath(void);           // if filename is "/idv/code/file.cpp", it returns "/idv/code"
		Filename NoExtension(void);       // if filename is "/idv/code/file.cpp", it returns "/idv/code/file"
		Filename NoPath(void);            // if filename is "/idv/code/file.cpp", it returns "file.cpp"
	};


	///////////////////////////////////////////////////////////////////////  
	//  Filename::GetExtension

	inline Filename Filename::GetExtension(void)
	{
		tstring strExtension;

		int nLength = (int)length( );
		for (int i = nLength - 1; i > 0 && (*this)[i] != _T('/'); i--)
			if ((*this)[i] == _T('.'))
			{
				strExtension = tstring(c_str( ) + i + 1);
				break;
			}

			return strExtension;
	}


	///////////////////////////////////////////////////////////////////////  
	//  Filename::GetPath

	inline Filename Filename::GetPath(void)
	{
		TCHAR szPath[1024];

		szPath[0] = _T('\0');
		int nLength = (int)length( );
		for (int i = nLength - 1; i >= 0; i--)
			if ((*this)[i] == '/' ||
				(*this)[i] == '\\')
			{
				int j = 0;
				for (j = 0; j < i + 1; j++)
					szPath[j] = (*this)[j];
				szPath[j] = '\0';
				break;
			}

			return Filename(szPath);
	}


	///////////////////////////////////////////////////////////////////////  
	//  Filename::NoExtension

	inline Filename Filename::NoExtension(void)
	{
		TCHAR szExtension[1024];

		_tcscpy_s(szExtension, sizeof(szExtension)/sizeof(TCHAR), c_str( ));
		int nLength = (int)length( );
		for (int i = nLength - 1; i >= 0 && (*this)[i] != '/'; i--)
			if ((*this)[i] == '.')
			{
				int j;
				for (j = 0; j < i; j++)
					szExtension[j] = (*this)[j];
				szExtension[j] = '\0';
				break;
			}

			return szExtension;
	}


	///////////////////////////////////////////////////////////////////////  
	//  Filename::NoPath

	inline Filename Filename::NoPath(void)
	{
		TCHAR szPath[1024];

		_tcscpy_s(szPath, sizeof(szPath)/sizeof(TCHAR), c_str( ));
		int nLength = (int)length( );
		for (int i = nLength - 1; i >= 0; i--)
			if ((*this)[i] == '/' || (*this)[i] == '\\')
			{
				int j, k;
				for (j = i + 1, k = 0; j < nLength; j++, k++)
					szPath[k] = (*this)[j];
				szPath[k] = '\0';
				break;
			}

			return tstring(szPath);
	}
}//endof namespace Cool3D