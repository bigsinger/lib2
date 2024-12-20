// FileVersionInfo.cpp: implementation of the CFileVersionInfo class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "FileVersionInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileVersionInfo::CFileVersionInfo()
{
   m_lpVerInfoBlock = NULL; 
   ::ZeroMemory( m_szTransBlock, sizeof(m_szTransBlock)/sizeof(TCHAR) );
}

CFileVersionInfo::~CFileVersionInfo()
{
   if( m_lpVerInfoBlock!=NULL ) delete [] m_lpVerInfoBlock;
   m_lpVerInfoBlock = NULL;
}

#ifdef _DEBUG
void CFileVersionInfo::AssertValid() const
{
   // call inherited AssertValid first
    CObject::AssertValid(); 
    // check CFileVersionInfo members...
   ASSERT( m_lpVerInfoBlock );
   ASSERT( !m_strFilename.IsEmpty() );
} 

void CFileVersionInfo::Dump( CDumpContext &dc ) const
{
   CObject::Dump( dc );
   dc << "Filename = " << m_strFilename;
   dc << "Ver File Description = " << GetFileDescription();
   dc << "Ver File Version = " << GetFileVersion();
   dc << "Ver Product Name = " << GetProductName();
   dc << "Ver Product Version = " << GetProductVersion();
}
#endif


//////////////////////////////////////////////////////////////////////
// Implementation

BOOL CFileVersionInfo::Open( LPCTSTR pszFilename )
// Initialize version information from file. 
// Returns TRUE on success, FALSE on failure
{
   ASSERT( AfxIsValidString(pszFilename) );
   m_strFilename.Empty();
   if( !AfxIsValidString(pszFilename)) return FALSE;

   // Get the size of the version information
   DWORD dwHandle = 0;
   DWORD dwSize = ::GetFileVersionInfoSize( (LPTSTR)pszFilename, &dwHandle );

   // Does the file have VERSION information
   if ( dwSize ) {
      // If previous m_lpVerInfoBlock was allocated, delete it now
      if( m_lpVerInfoBlock!=NULL ) delete [] m_lpVerInfoBlock;
      m_lpVerInfoBlock = NULL;

      // Allocate memory for the version information block
      m_lpVerInfoBlock = new char[dwSize];

      if ( m_lpVerInfoBlock && ::GetFileVersionInfo((LPTSTR)pszFilename, dwHandle, dwSize, m_lpVerInfoBlock) )
      {
         // Get the language ID
         TCHAR  szLang[30];
         TCHAR *pszLang = szLang;
         UINT   nLen = 0;

         if( ::VerQueryValue( m_lpVerInfoBlock, _T("\\VarFileInfo\\Translation"), (LPVOID *)&pszLang, &nLen) )
         {
            /* The retrieved 'hex' value looks a little confusing, but
               essentially what it is, is the hexidecimal representation
               of a couple different values that represent the language
               and character set that we are wanting string values for.
               040904E4 is a very common one, because it means:
                US English, Windows MultiLingual characterset
               Or to pull it all apart:
               04------        = SUBLANG_ENGLISH_USA
               --09----        = LANG_ENGLISH
               ----04E4 = 1252 = Codepage for Windows:Multilingual
               */
            // Swap the words so wsprintf() will print the lang-charset in the correct format
            *(LPDWORD)pszLang = MAKELONG(HIWORD(*(LPDWORD)pszLang), LOWORD(*(LPDWORD)pszLang));

            // Now save the whole string info including the language ID
            // and final backslash as m_szTransBlock
            ::wsprintf( m_szTransBlock, _T("\\StringFileInfo\\%08lx\\"), *(LPDWORD)(pszLang) );
            m_strFilename = pszFilename;
            return TRUE;
         };
      };
      // Cleanup on failure
      if( m_lpVerInfoBlock!=NULL ) delete [] m_lpVerInfoBlock;
      m_lpVerInfoBlock = NULL;
   };
   // Failure
   return FALSE;
};


BOOL CFileVersionInfo::Open( HINSTANCE hInstance /*=NULL*/ )
// Initialize from already loaded module
// Supply NULL for current process
{
   TCHAR sz[MAX_PATH];
   m_strFilename.Empty();
   if ( ::GetModuleFileName(hInstance, sz, sizeof(sz)/sizeof(TCHAR)) )
      return Open(sz);
   return FALSE;
}


CString CFileVersionInfo::GetItem( LPCTSTR pszItem ) const
// Get a specific verinfo item (CompanyName, etc)
{
   ASSERT( AfxIsValidString(pszItem) );
   ASSERT_VALID( this );
   // Validate
   if( !AfxIsValidString(pszItem) ) return CString();
   // Get item from version block
   CString s;
   UINT nLen = 0;
   // Format sub-block
   TCHAR  szSubBlock[1024];
   ::wsprintf( szSubBlock, _T("%s%s"), m_szTransBlock, pszItem );
   // Get result
   TCHAR *pszOut = NULL;
   if ( ::VerQueryValue(m_lpVerInfoBlock, szSubBlock, (LPVOID*)&pszOut, &nLen) ) {
      s = pszOut;
   };
   return s;
};
