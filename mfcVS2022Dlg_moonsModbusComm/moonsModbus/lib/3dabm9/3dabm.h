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
		 * 3D Active Button Magic OCX�� �����쿡 ��ϵǾ� �ִ��� Ȯ���Ѵ�.
		 *
		 * @retrun		BOOL			: ��ϵǾ� ������ TRUE, ������ FALSE
		 */
		BOOL CheckDllRegister() {
			HRESULT ret = CoInitialize(NULL);

			// �ý��ۿ��� 3dabm�� CLSID�� �����´�. (�������� ���� �� ��� �ȵ� ����)
			CLSID clsID;
			HRESULT hr = CLSIDFromProgID(OLESTR("BTNENH.BtnEnhCtrl.4"), &clsID);

			CoUninitialize();

			if (hr == S_OK)
				return TRUE;
			else
				return FALSE;
		}

		/**
		* 3D Active Button Magic OCX�� ������ System32 ������ ������ �� DllRegisterServer �� ȣ���Ѵ�.
		*
		* @param		strPath			: OCX�� �����ϴ� ���� ���
		* @param		strFileName		: OCX ���� �̸�
		* @retrun		E3dabm_Error	: ������ �����ڵ�
		*/
		E3dabm_Error RegisterOCX(CString strPath = _T(""), CString strFileName = _T("3dabm9u.ocx")) {
			// 64bit �ü������ ����Ǿ��� ��� Registry�� OCX�� 32bit���̸� 64bit������ �������ش�.
			if (Is64BitWindows() == TRUE
				&& strFileName == _T("3dabm9u.ocx"))
			{
				strFileName = _T("3dabm9u64.ocx");
			}

			// �ý��� ��θ� �����´�.
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

			// ������ ������ �����ϴ��� �˻��Ѵ�.
			CFileFind ffind;
			if (ffind.FindFile(strSrcPathName) == FALSE)
			{
				return E3dabm_Error::File_Not_Found;
			}

			// ��� ������ �����ϴ��� �˻��Ѵ�.
			if (ffind.FindFile(strDestPathName) == FALSE)
			{
				// ��� ������ ������ �����Ѵ�.
				if (CopyFile(strSrcPathName, strDestPathName, FALSE) == FALSE)
				{
					return E3dabm_Error::Copy_Fail;
				}
			}

			// OCX/DLL�� Load Library�Ѵ�.
			HMODULE hModule = LoadLibrary(strDestPathName);
			if (hModule == NULL)
			{
				return E3dabm_Error::LoadLibrary_Fail;
			}

			// �Լ� �����͸� GetProcAddress �Ѵ�.
			m_lpDllRegisterServer = (LP_DllRegisterServer)GetProcAddress(hModule, "DllRegisterServer");
			if (m_lpDllRegisterServer == NULL)
			{
				return E3dabm_Error::GetProcAddress_Fail;
			}

			// DllRegisterServer �� ȣ���ؼ� OCX�� ��Ͻ�Ų��.
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
		* 3D Active Button Magic OCX�� UnRegister �Ѵ�.
		*
		* @param		strPath			: OCX�� �����ϴ� ���� ���
		* @param		strFileName		: OCX ���� �̸�
		* @retrun		E3dabm_Error	: ������ �����ڵ�
		*/
		E3dabm_Error UnRegisterOCX(CString strPath = _T(""), CString strFileName = _T("3dabm9u.ocx")) {
			// �ý��� ��θ� �����´�.
			CString strSystemPath = strPath;

			if (strPath == _T(""))
			{
				strSystemPath = _T("");
				GetSystemDirectory(strSystemPath.GetBuffer(1024), 1024);
				strSystemPath.ReleaseBuffer();
			}

			CString strDestPathName = strSystemPath + _T("\\") + strFileName;

			// ��� ������ �����ϴ��� �˻��Ѵ�.
			CFileFind ffind;
			if (ffind.FindFile(strDestPathName) == FALSE)
			{
				return E3dabm_Error::File_Not_Found;
			}

			// OCX/DLL�� Load Library�Ѵ�.
			HMODULE hModule = LoadLibrary(strDestPathName);
			if (hModule == NULL)
			{
				return E3dabm_Error::LoadLibrary_Fail;
			}

			// �Լ� �����͸� GetProcAddress �Ѵ�.
			m_lpDllUnregisterServer = (LP_DllUnregisterServer)GetProcAddress(hModule, "DllUnregisterServer");
			if (m_lpDllUnregisterServer == NULL)
			{
				return E3dabm_Error::GetProcAddress_Fail;
			}

			// DllRegisterServer �� ȣ���ؼ� OCX�� ��Ͻ�Ų��.
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
		* �����ڵ忡 �´� ���ڿ��� ��ȯ�Ѵ�.
		*
		* @param		eError			: �����ڵ�
		* @retrun		CString			: ���� ���ڿ�
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