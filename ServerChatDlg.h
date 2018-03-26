
// ServerChatDlg.h : ͷ�ļ�
//

#pragma once
#include <winsock2.h>
#include "afxwin.h"
#include "afxcmn.h"
#pragma comment(lib,"ws2_32.lib")


// CServerChatDlg �Ի���
class CServerChatDlg : public CDialogEx
{
// ����
public:
	CServerChatDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVERCHAT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// �����׽���
	SOCKET m_LocalSocket;
	// �������ӵ��׽���
	SOCKET m_AcceptSocket;
	// ��������״̬
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
