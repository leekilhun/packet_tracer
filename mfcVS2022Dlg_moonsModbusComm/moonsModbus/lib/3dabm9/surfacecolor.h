#if !defined(AFX_SURFACECOLOR_H__C6425A9A_E45A_4DD6_BB82_0603E9663A09__INCLUDED_)
#define AFX_SURFACECOLOR_H__C6425A9A_E45A_4DD6_BB82_0603E9663A09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.

/////////////////////////////////////////////////////////////////////////////
// CSurfaceColor wrapper class

namespace LIB_BOTTON_3D
{
	class CSurfaceColor : public COleDispatchDriver
	{
	public:
		CSurfaceColor() {}		// Calls COleDispatchDriver default constructor
		CSurfaceColor(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
		CSurfaceColor(const CSurfaceColor& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

		// Attributes
	public:
		unsigned long GetColor();
		void SetColor(unsigned long);
		long GetGradientType();
		void SetGradientType(long);
		short GetGradientFactor();
		void SetGradientFactor(short);
		long GetRender3DType();
		void SetRender3DType(long);
		unsigned long GetColor2();
		void SetColor2(unsigned long);

		// Operations
	public:
	};
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SURFACECOLOR_H__C6425A9A_E45A_4DD6_BB82_0603E9663A09__INCLUDED_)
