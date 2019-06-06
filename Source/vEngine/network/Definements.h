#pragma once
namespace vEngine {



namespace nsHelper
{
   /// Class with static functions to get information about an error.
   class CError
   {
   public:
      static DWORD GetLastError() { return ::GetLastError(); }
   
      static tstring GetErrorDescription(int iErrorCode = GetLastError())
      {
         LPVOID lpMsgBuf=NULL;
         FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
                     NULL, iErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                     reinterpret_cast<LPTSTR>(&lpMsgBuf), 0, NULL);
         tstring strErrorDescription;
         if( lpMsgBuf )
         {
            strErrorDescription = reinterpret_cast<LPTSTR>(lpMsgBuf);
            LocalFree(lpMsgBuf);
         }
         return strErrorDescription;
      }
   };

/// Class with static functions to do string conversions.
class CCnv
{
public:
   static long TStringToLong(const tstring& strIn)
   {
      TCHAR* pStopString = 0;
   #ifdef _UNICODE
      return wcstol(&*strIn.begin(), &pStopString, 10);
   #else
      return strtol(&*strIn.begin(), &pStopString, 10);
   #endif
   }

   static std::string ConvertToString(const tstring& strIn)
   {
      std::string strOut;
      return ConvertToString(strIn, strOut);
   }

   static std::string& ConvertToString(const tstring& strIn, std::string& strOut)
   {
   #ifdef _UNICODE
      if( strIn.size() == 0 )
      {
         strOut.clear();
      }
      else
      {
         strOut.resize(strIn.size());
         wcstombs(&*strOut.begin(), strIn.c_str(), strOut.size());
      }
   #else
      strOut = strIn;
   #endif
      return strOut;
   }

   static tstring ConvertToTString(const char* szIn)
   {
      tstring strOut;
      return ConvertToTString(szIn, strOut);
   }

   static tstring& ConvertToTString(const char* szIn, tstring& strOut)
   {
   #ifdef _UNICODE
      if( strlen(szIn) == 0 )
      {
         strOut.clear();
      }
      else
      {
         strOut.resize(strlen(szIn));
         mbstowcs(&*strOut.begin(), szIn, strOut.size());
      }
   #else
      strOut = szIn;
   #endif
      return strOut;
   }
};

/// Base class for implementing the notification stuff.
/// @remarks Inherit public (instead of private) because it wouldn't compile under Dev-C++
template <typename T, typename T2> class CObserverPatternBase : public std::set<T>
{
public:
   typedef typename std::set<T> base_type;
   typedef typename std::set<T>::iterator iterator;

   ~CObserverPatternBase()
   {
      for( iterator it=base_type::begin(); it!=base_type::end(); it=base_type::begin() )
         (*it)->Detach(static_cast<T2>(this));
   }

   bool Attach(T p)
   {
      if (find(p)!=base_type::end())
         return false;
      insert(p);
      p->Attach(static_cast<T2>(this));
      return true;
   }
   bool Detach(T p)
   {
      if (find(p)==base_type::end())
         return false;
      erase(p);
      p->Detach(static_cast<T2>(this));
      return true;
   }
};

/// @brief Calculates elapsed CPU time.
///
/// Is useful for calculating transferrates.
class CTimer
{
public:
   CTimer() : m_dfStart(clock()) {}

   /// Restarts the timer.
   void Restart() { m_dfStart = clock(); }

   /// Get the elapsed time in seconds.
   double GetElapsedTime() const
   {
      return (static_cast<double>(clock()) - m_dfStart)/CLOCKS_PER_SEC;
   }

private:
   double m_dfStart; ///< elapsed CPU time for process (start of measurement)
};

}
}

