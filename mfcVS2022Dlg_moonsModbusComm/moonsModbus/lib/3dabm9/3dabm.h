#pragma once

#include "3dabm9.h"


namespace LIB_BOTTON_3D
{


	enum class E3dabm_Error
	{
		Success = 0,

		File_Not_Found,
		Copy_Fail,
		LoadLibrary_Fail,
		GetProcAddress_Fail,
		DllRegisterServer_Fail,

		Max,
	};

	typedef HRESULT(*LP_DllRegisterServer)(void);
	typedef HRESULT(*LP_DllUnregisterServer)(void);

	class G3dabm
	{
	public:
		G3dabm() {
			HRESULT ret = CoInitialize(NULL);

			m_lpDllRegisterServer = NULL;
			m_lpDllUnregisterServer = NULL;
		}
		~G3dabm() {
			CoUninitialize();
		}

	public:
		/**
		 * 3D Active Button Magic OCX가 윈도우에 등록되어 있는지 확인한다.
		 *
		 * @retrun		BOOL			: 등록되어 있으면 TRUE, 없으면 FALSE
		 */
		BOOL CheckDllRegister() {
			HRESULT ret = CoInitialize(NULL);

			// 시스템에서 3dabm의 CLSID를 가져온다. (가져오기 실패 시 등록 안된 상태)
			CLSID clsID;
			HRESULT hr = CLSIDFromProgID(OLESTR("BTNENH.BtnEnhCtrl.4"), &clsID);

			CoUninitialize();

			if (hr == S_OK)
				return TRUE;
			else
				return FALSE;
		}

		/**
		* 3D Active Button Magic OCX를 윈도우 System32 폴더에 복사한 후 DllRegisterServer 를 호출한다.
		*
		* @param		strPath			: OCX가 존재하는 파일 경로
		* @param		strFileName		: OCX 파일 이름
		* @retrun		E3dabm_Error	: 실패한 에러코드
		*/
		E3dabm_Error RegisterOCX(CString strPath = _T(""), CString strFileName = _T("3dabm9u.ocx")) {
			// 64bit 운영체제에서 실행되었을 경우 Registry할 OCX가 32bit용이면 64bit용으로 변경해준다.
			if (Is64BitWindows() == TRUE
				&& strFileName == _T("3dabm9u.ocx"))
			{
				strFileName = _T("3dabm9u64.ocx");
			}

			// 시스템 경로를 가져온다.
			CString strSystemPath;
			GetSystemDirectory(strSystemPath.GetBuffer(1024), 1024);
			strSystemPath.ReleaseBuffer();

			if (strPath == _T(""))
			{
				CString strCurrPath;
				GetCurrentDirectory(1024, strCurrPath.GetBuffer(1024));
				strCurrPath.ReleaseBuffer();

				strPath = strCurrPath;
			}

			CString strSrcPathName = strPath + _T("\\") + strFileName;
			CString strDestPathName = strSystemPath + _T("\\") + strFileName;

			// 복사할 파일이 존재하는지 검사한다.
			CFileFind ffind;
			if (ffind.FindFile(strSrcPathName) == FALSE)
			{
				return E3dabm_Error::File_Not_Found;
			}

			// 대상 파일이 존재하는지 검사한다.
			if (ffind.FindFile(strDestPathName) == FALSE)
			{
				// 대상 파일이 없으면 복사한다.
				if (CopyFile(strSrcPathName, strDestPathName, FALSE) == FALSE)
				{
					return E3dabm_Error::Copy_Fail;
				}
			}

			// OCX/DLL을 Load Library한다.
			HMODULE hModule = LoadLibrary(strDestPathName);
			if (hModule == NULL)
			{
				return E3dabm_Error::LoadLibrary_Fail;
			}

			// 함수 포인터를 GetProcAddress 한다.
			m_lpDllRegisterServer = (LP_DllRegisterServer)GetProcAddress(hModule, "DllRegisterServer");
			if (m_lpDllRegisterServer == NULL)
			{
				return E3dabm_Error::GetProcAddress_Fail;
			}

			// DllRegisterServer 를 호출해서 OCX를 등록시킨다.
			HRESULT hr = m_lpDllRegisterServer();
			if (hr != S_OK)
			{
				FreeLibrary(hModule);
				return E3dabm_Error::DllRegisterServer_Fail;
			}

			FreeLibrary(hModule);
			return E3dabm_Error::Success;
		}

		/**
		* 3D Active Button Magic OCX를 UnRegister 한다.
		*
		* @param		strPath			: OCX가 존재하는 파일 경로
		* @param		strFileName		: OCX 파일 이름
		* @retrun		E3dabm_Error	: 실패한 에러코드
		*/
		E3dabm_Error UnRegisterOCX(CString strPath = _T(""), CString strFileName = _T("3dabm9u.ocx")) {
			// 시스템 경로를 가져온다.
			CString strSystemPath = strPath;

			if (strPath == _T(""))
			{
				strSystemPath = _T("");
				GetSystemDirectory(strSystemPath.GetBuffer(1024), 1024);
				strSystemPath.ReleaseBuffer();
			}

			CString strDestPathName = strSystemPath + _T("\\") + strFileName;

			// 대상 파일이 존재하는지 검사한다.
			CFileFind ffind;
			if (ffind.FindFile(strDestPathName) == FALSE)
			{
				return E3dabm_Error::File_Not_Found;
			}

			// OCX/DLL을 Load Library한다.
			HMODULE hModule = LoadLibrary(strDestPathName);
			if (hModule == NULL)
			{
				return E3dabm_Error::LoadLibrary_Fail;
			}

			// 함수 포인터를 GetProcAddress 한다.
			m_lpDllUnregisterServer = (LP_DllUnregisterServer)GetProcAddress(hModule, "DllUnregisterServer");
			if (m_lpDllUnregisterServer == NULL)
			{
				return E3dabm_Error::GetProcAddress_Fail;
			}

			// DllRegisterServer 를 호출해서 OCX를 등록시킨다.
			HRESULT hr = m_lpDllUnregisterServer();
			if (hr != S_OK)
			{
				FreeLibrary(hModule);
				return E3dabm_Error::DllRegisterServer_Fail;
			}

			FreeLibrary(hModule);
			return E3dabm_Error::Success;
		}

		/**
		* 에러코드에 맞는 문자열을 반환한다.
		*
		* @param		eError			: 에러코드
		* @retrun		CString			: 에러 문자열
		*/
		CString GetErrorString(E3dabm_Error eError) {
			CString strMsg;
			switch (eError)
			{
			case E3dabm_Error::File_Not_Found:			strMsg = _T("File_Not_Found"); 			break;
			case E3dabm_Error::Copy_Fail:				strMsg = _T("Copy_Fail");			 	break;
			case E3dabm_Error::LoadLibrary_Fail:		strMsg = _T("LoadLibrary_Fail"); 		break;
			case E3dabm_Error::GetProcAddress_Fail:		strMsg = _T("GetProcAddress_Fail"); 	break;
			case E3dabm_Error::DllRegisterServer_Fail:	strMsg = _T("DllRegisterServer_Fail"); 	break;
			default:									strMsg = _T("Unknown Error"); 			break;
			}

			return strMsg;
		}

		BOOL IsCurrentProcess64bit() {
#if defined(_WIN64)
			return TRUE;
#else
			return FALSE;
#endif
		}

		BOOL IsCurrentProcessWow64() {
			BOOL bIsWow64 = FALSE;
			typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
			LPFN_ISWOW64PROCESS fnIsWow64Process;

			fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32")), "IsWow64Process");
			if (!fnIsWow64Process)
				return FALSE;

			return fnIsWow64Process(GetCurrentProcess(), &bIsWow64) && bIsWow64;
		}

		BOOL Is64BitWindows() {
			if (IsCurrentProcess64bit())
				return TRUE;

			return IsCurrentProcessWow64();
		}


	private:
		LP_DllRegisterServer		m_lpDllRegisterServer;
		LP_DllUnregisterServer		m_lpDllUnregisterServer;
	};

}