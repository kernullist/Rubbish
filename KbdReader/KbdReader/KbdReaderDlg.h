#pragma once

#include "..\KnCommLib\KnCommLibUser.h"
#include "afxcmn.h"


class CKbdReaderDlg : public CDialogEx
{
public:
	CKbdReaderDlg(CWnd* pParent = NULL);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KBDREADER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnAddLog(WPARAM wp, LPARAM lp);

private:
	static void CALLBACK OnKeyboardDataRecv(
		IN	ULONG			dataId,
		IN	PVOID			pData,
		IN	SIZE_T			dataSize,
		IN	BOOL			isReplyRequired,
		IN	PVOID			pContext
		);

private:
	HMODULE					m_knCommUserLibDll;
	KNCOMM					m_knCommHandle;
	FN_ConnectToKnComm		m_pfnConnectToKnComm;
	FN_DisconnectFromKnComm	m_pfnDisconnectFromKnComm;

private:
	CListCtrl				m_logListCtrl;
};

#define UM_ADD_LOG		(WM_USER + 1)