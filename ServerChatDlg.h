
// ServerChatDlg.h : 头文件
//

#pragma once
#include <winsock2.h>
#include "afxwin.h"
#include "afxcmn.h"
#pragma comment(lib,"ws2_32.lib")


// CServerChatDlg 对话框
class CServerChatDlg : public CDialogEx
{
// 构造
public:
	CServerChatDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVERCHAT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 本地套接字
	SOCKET m_LocalSocket;
	// 接收连接的套接字
	SOCKET m_AcceptSocket;
	// 网络连接状态
	BOOL m_bConnectState;
//	CString m_szServerIP;
//	CString m_szServerIP;
	afx_msg void OnBindAddr();
//	CString m_nServerPort;
//	CEdit m_nServerPort;
//	CString m_nServerPort;
	CEdit m_SendText;
//	UNIT m_nServerPort;
	CString m_szServerIP;
//	CButton m_BindAddr;
	CRichEditCtrl m_ChatRecord;
	CButton m_BindAddr;
	UINT m_nServerPort;
	CStatic m_NetState;
	LRESULT OnSocket(WPARAM wParam, LPARAM IParam);
	afx_msg void OnSendInfo();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
