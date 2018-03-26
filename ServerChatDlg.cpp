
// ServerChatDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ServerChat.h"
#include "ServerChatDlg.h"
#include "afxdialogex.h"

#define WM_SOCKET WM_USER+30

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

BEGIN_MESSAGE_MAP(CServerChatDlg, CDialog)
	//{{AFX_MSG_MAP(CServerChatDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BIND_ADDR, OnBindAddr)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SENDINFO,OnSendInfo)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SOCKET,OnSocket)
	ON_BN_CLICKED(IDC_SENDINFO, &CServerChatDlg::OnSendInfo)
END_MESSAGE_MAP()

// CServerChatDlg �Ի���



CServerChatDlg::CServerChatDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVERCHAT_DIALOG, pParent)
	, m_bConnectState(FALSE)
	, m_szServerIP(_T(""))
	, m_nServerPort(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_EDIT2, m_szServerIP);
	DDX_Text(pDX, IDC_SERVER_IP, m_szServerIP);
	//  DDX_Text(pDX, IDC_EDIT3, m_nServerPort);
	//  DDX_Control(pDX, IDC_EDIT3, m_nServerPort);
	//  DDX_Text(pDX, IDC_EDIT3, m_nServerPort);
	DDX_Control(pDX, IDC_SEND_TEXT, m_SendText);
	//  DDX_Control(pDX, IDC_SERVER_PORT, m_nServerPort);
	DDX_Text(pDX, IDC_SERVER_IP, m_szServerIP);
	//  DDX_Control(pDX, IDC_BIND_ADDR, m_BindAddr);
	DDX_Control(pDX, IDC_CHAT_RECORD, m_ChatRecord);
	DDX_Control(pDX, IDC_BIND_ADDR, m_BindAddr);
	DDX_Text(pDX, IDC_SERVER_PORT, m_nServerPort);
	DDX_Control(pDX, IDC_NETSTATE, m_NetState);
}



// CServerChatDlg ��Ϣ�������

BOOL CServerChatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	WSADATA wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);
	m_LocalSocket = socket(AF_INET, SOCK_STREAM, 0);
	unsigned long nCmd;
	int nState = ioctlsocket(m_LocalSocket, FIONBIO, &nCmd);
	if (nState != 0) {
		TRACE("�����׽��ַ�����ģʽʧ��");
	}
	//��ȡIP��ַ
	char szHostName[128] = { 0 };
	gethostname(szHostName, 128);	//��ȡ������
	hostent* phostent = gethostbyname(szHostName);	//��ȡ��������
	char* szIP = inet_ntoa(*(in_addr*)&phostent->h_addr_list[2]);	//��ȡ����IP
	m_szServerIP = szIP;	//�ڱ༭������ʾIP
	UpdateData(FALSE);
	
	//�����׽���WSAASYNCSelectģ��
	int nRet = WSAAsyncSelect(m_LocalSocket, m_hWnd, WM_SOCKET, FD_ACCEPT | FD_CONNECT);
	if (nRet != 0) {
		TRACE("����WSAAsyncSelectģ��ʧ��");
	}
	SetTimer(1, 800, NULL);     
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE

}

void CServerChatDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CServerChatDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CServerChatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CServerChatDlg::OnBindAddr()
{
	UpdateData();
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(m_nServerPort);	//����IP��ַ
	sockAddr.sin_addr.S_un.S_addr = inet_addr(m_szServerIP.GetBuffer(0));
	if (bind(m_LocalSocket, (sockaddr*)&sockAddr, sizeof(sockAddr)) == 0 && listen(m_LocalSocket, 5) == 0)
	{
		m_BindAddr.EnableWindow(FALSE);
		MessageBox("��ַ�Ͷ˿ڰ󶨳ɹ���", "��ʾ");
	}
	else
	{
		MessageBox("��ַ��ʧ�ܣ�", "��ʾ");
	}
	m_szServerIP.ReleaseBuffer();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}



LRESULT CServerChatDlg::OnSocket(WPARAM wParam, LPARAM IParam)
{
	int nError = WSAGETSELECTERROR(IParam);
	int nEvent = WSAGETSELECTEVENT(IParam);
	char buffer[1024] = { 0 };
	SOCKET sock = wParam;
	int nFactLen = recv(sock, buffer, 1024, 0);
	switch (nEvent)
	{
	case FD_ACCEPT: {
		closesocket(m_AcceptSocket);
		sockaddr_in sockAddr;
		int nAddrSize = sizeof(sockaddr_in);
		m_AcceptSocket = accept(sock, (sockaddr*)&sockAddr, &nAddrSize);
		WSAAsyncSelect(m_AcceptSocket, m_hWnd, WM_SOCKET, FD_CLOSE | FD_READ);
		m_bConnectState = TRUE;
		break;
	}
	case FD_READ: {
		m_ChatRecord.SetSel(-1, -1);
		m_ChatRecord.ReplaceSel(buffer);
		m_ChatRecord.SetSel(-1, -1);
		m_ChatRecord.ReplaceSel("\n");
	}
	case FD_CLOSE: {
		closesocket(m_AcceptSocket);
		m_bConnectState = FALSE;
		break;
	}
	default:
		break;
	}
	return 1;
}


void CServerChatDlg::OnSendInfo()
{
	CString szText;
	m_SendText.GetWindowText(szText);
	szText.Insert(0, "С��:");
	m_ChatRecord.SetSel(-1, -1);
	m_ChatRecord.ReplaceSel(szText);
	m_ChatRecord.SetSel(-1, -1);
	m_ChatRecord.ReplaceSel("\n");
	send(m_AcceptSocket, szText.GetBuffer(0), szText.GetLength(), 0);
	szText.ReleaseBuffer();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CServerChatDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (!m_bConnectState)
	{
		m_NetState.SetWindowText("����δ����!");
	}
	else
	{
		m_NetState.SetWindowText("����������!");
	}
	CDialog::OnTimer(nIDEvent);
}
