#if !defined(AFX_TEXTURE_H__AF25E66F_E258_4096_BA2D_3E2E9DB88B3D__INCLUDED_)
#define AFX_TEXTURE_H__AF25E66F_E258_4096_BA2D_3E2E9DB88B3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.

/////////////////////////////////////////////////////////////////////////////
// CTexture wrapper class

namespace LIB_BOTTON_3D
{
	class CTexture : public COleDispatchDriver
	{
	public:
		CTexture() {}		// Calls COleDispatchDriver default constructor
		CTexture(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
		CTexture(const CTexture& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

		// Attributes
	public:
		CString GetFilepath();
		void SetFilepath(LPCTSTR);
		long GetMode();
		void SetMode(long);
		short GetLightIntensity();
		void SetLightIntensity(short);
		unsigned long GetColorToMix();
		void SetColorToMix(unsigned long);
		short GetPercentageColorMix();
		void SetPercentageColorMix(short);

		// Operations
	public:
	};
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTURE_H__AF25E66F_E258_4096_BA2D_3E2E9DB88B3D__INCLUDED_)