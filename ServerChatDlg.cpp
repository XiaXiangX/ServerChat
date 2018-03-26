
// ServerChatDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServerChat.h"
#include "ServerChatDlg.h"
#include "afxdialogex.h"

#define WM_SOCKET WM_USER+30

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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

// CServerChatDlg 对话框



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



// CServerChatDlg 消息处理程序

BOOL CServerChatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	WSADATA wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);
	m_LocalSocket = socket(AF_INET, SOCK_STREAM, 0);
	unsigned long nCmd;
	int nState = ioctlsocket(m_LocalSocket, FIONBIO, &nCmd);
	if (nState != 0) {
		TRACE("设置套接字非阻塞模式失败");
	}
	//获取IP地址
	char szHostName[128] = { 0 };
	gethostname(szHostName, 128);	//获取主机名
	hostent* phostent = gethostbyname(szHostName);	//获取主机内容
	char* szIP = inet_ntoa(*(in_addr*)&phostent->h_addr_list[2]);	//获取主机IP
	m_szServerIP = szIP;	//在编辑框中显示IP
	UpdateData(FALSE);
	
	//设置套接字WSAASYNCSelect模型
	int nRet = WSAAsyncSelect(m_LocalSocket, m_hWnd, WM_SOCKET, FD_ACCEPT | FD_CONNECT);
	if (nRet != 0) {
		TRACE("设置WSAAsyncSelect模型失败");
	}
	SetTimer(1, 800, NULL);     
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE

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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CServerChatDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CServerChatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CServerChatDlg::OnBindAddr()
{
	UpdateData();
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(m_nServerPort);	//设置IP地址
	sockAddr.sin_addr.S_un.S_addr = inet_addr(m_szServerIP.GetBuffer(0));
	if (bind(m_LocalSocket, (sockaddr*)&sockAddr, sizeof(sockAddr)) == 0 && listen(m_LocalSocket, 5) == 0)
	{
		m_BindAddr.EnableWindow(FALSE);
		MessageBox("地址和端口绑定成功！", "提示");
	}
	else
	{
		MessageBox("地址绑定失败！", "提示");
	}
	m_szServerIP.ReleaseBuffer();
	// TODO: 在此添加控件通知处理程序代码
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
	szText.Insert(0, "小雨:");
	m_ChatRecord.SetSel(-1, -1);
	m_ChatRecord.ReplaceSel(szText);
	m_ChatRecord.SetSel(-1, -1);
	m_ChatRecord.ReplaceSel("\n");
	send(m_AcceptSocket, szText.GetBuffer(0), szText.GetLength(), 0);
	szText.ReleaseBuffer();
	// TODO: 在此添加控件通知处理程序代码
}


void CServerChatDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (!m_bConnectState)
	{
		m_NetState.SetWindowText("网络未连接!");
	}
	else
	{
		m_NetState.SetWindowText("网络已连接!");
	}
	CDialog::OnTimer(nIDEvent);
}
