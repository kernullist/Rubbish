#include "stdafx.h"
#include "KbdReader.h"
#include "KbdReaderDlg.h"
#include "afxdialogex.h"
#include "..\Common\KernelUserCommon.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};


CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CKbdReaderDlg::CKbdReaderDlg(CWnd* pParent) : CDialogEx(IDD_KBDREADER_DIALOG, pParent)
	, m_knCommUserLibDll(nullptr)
	, m_knCommHandle(nullptr)
	, m_pfnConnectToKnComm(nullptr)
	, m_pfnDisconnectFromKnComm(nullptr)	
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CKbdReaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_logListCtrl);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CKbdReaderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CKbdReaderDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CKbdReaderDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CKbdReaderDlg::OnBnClickedButtonClear)
	ON_WM_DESTROY()
	ON_MESSAGE(UM_ADD_LOG, &CKbdReaderDlg::OnAddLog)
END_MESSAGE_MAP()



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CKbdReaderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	
	m_logListCtrl.InsertColumn(0, L"Time", LVCFMT_LEFT, 80);
	m_logListCtrl.InsertColumn(1, L"Make Code", LVCFMT_LEFT, 80);
	m_logListCtrl.InsertColumn(2, L"Satus", LVCFMT_LEFT, 80);
	m_logListCtrl.InsertColumn(3, L"Flags", LVCFMT_LEFT, 80);
	m_logListCtrl.SetExtendedStyle(m_logListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);


	//
	// 드라이버 설치 및 KnCommLibUser.dll 관련 초기화
	//

	do
	{
		m_knCommUserLibDll = LoadLibrary(L"KnCommLibUser.dll");
		if (m_knCommUserLibDll == nullptr)
		{
			break;
		}

		m_pfnConnectToKnComm = reinterpret_cast<FN_ConnectToKnComm>(GetProcAddress(m_knCommUserLibDll, "ConnectToKnComm"));
		m_pfnDisconnectFromKnComm = reinterpret_cast<FN_DisconnectFromKnComm>(GetProcAddress(m_knCommUserLibDll, "DisconnectFromKnComm"));
		if (m_pfnConnectToKnComm == nullptr || m_pfnDisconnectFromKnComm == nullptr)
		{
			break;
		}

		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);

	} while (false);
	
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CKbdReaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CKbdReaderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HCURSOR CKbdReaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CKbdReaderDlg::OnBnClickedButtonStart()
{
	do
	{
		if (m_pfnConnectToKnComm == nullptr)
		{
			break;
		}

		KNCOMM_CB_INFO info;
		info.pfnOnDataRecv = OnKeyboardDataRecv;
		info.pCallbackContext = this;
		if (m_pfnConnectToKnComm(KBDRDR_DRIVER_NAME, &info, &m_knCommHandle) == FALSE)
		{
			break;
		}

		GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);

	} while (false);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CKbdReaderDlg::OnBnClickedButtonStop()
{
	do
	{
		if (m_pfnDisconnectFromKnComm == nullptr)
		{
			break;
		}

		if (m_knCommHandle == nullptr)
		{
			break;
		}

		if (m_pfnDisconnectFromKnComm(m_knCommHandle) == FALSE)
		{
			break;
		}

		m_knCommHandle = nullptr;

		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);

	} while (false);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CKbdReaderDlg::OnBnClickedButtonClear()
{
	m_logListCtrl.DeleteAllItems();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CKbdReaderDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	OnBnClickedButtonStop();

	m_pfnConnectToKnComm = nullptr;
	m_pfnDisconnectFromKnComm = nullptr;

	if (m_knCommUserLibDll != nullptr)
	{
		FreeLibrary(m_knCommUserLibDll);
		m_knCommUserLibDll = nullptr;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CKbdReaderDlg::OnAddLog(WPARAM wp, LPARAM lp)
{
	PKEYEVENT_DATA pKeyEventData = reinterpret_cast<PKEYEVENT_DATA>(wp);
	if (pKeyEventData != nullptr)
	{
		CString msg;

		int logItemCount = m_logListCtrl.GetItemCount();

		CTime currentTime(CTime::GetTickCount());

		msg.Format(L"%02d:%02d:%02d", currentTime.GetHour(), currentTime.GetMinute(), currentTime.GetSecond());
		m_logListCtrl.InsertItem(logItemCount, msg);

		msg.Format(L"0x%02X", pKeyEventData->makeCode);
		m_logListCtrl.SetItemText(logItemCount, 1, msg);

		static const USHORT	KEY_BREAK = 1;
		static const USHORT KEY_E0 = 2;
		static const USHORT KEY_E1 = 4;

		if (pKeyEventData->flags & KEY_BREAK)
		{
			msg = L"Up";
		}
		else
		{
			msg = L"Down";
		}

		m_logListCtrl.SetItemText(logItemCount, 2, msg);

		msg.Format(L"0x%02X", pKeyEventData->flags);
		m_logListCtrl.SetItemText(logItemCount, 3, msg);

		m_logListCtrl.EnsureVisible(logItemCount, FALSE);

		delete pKeyEventData;
		pKeyEventData = nullptr;
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CALLBACK CKbdReaderDlg::OnKeyboardDataRecv(
	IN	ULONG			dataId,
	IN	PVOID			pData,
	IN	SIZE_T			dataSize,
	IN	BOOL			isReplyRequired,
	IN	PVOID			pContext
	)
{
	if (pData == nullptr || dataSize != sizeof(KEYEVENT_DATA) || pContext == nullptr)
	{
		return;
	}

	CKbdReaderDlg* pThis = static_cast<CKbdReaderDlg*>(pContext);

	PKEYEVENT_DATA pKeyEventData = new (std::nothrow) KEYEVENT_DATA;
	if (pKeyEventData != nullptr)
	{
		RtlCopyMemory(pKeyEventData, pData, sizeof(KEYEVENT_DATA));
		::PostMessage(pThis->GetSafeHwnd(), UM_ADD_LOG, reinterpret_cast<WPARAM>(pKeyEventData), NULL);
	}
}

