
// Commnuication_ring3Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Commnuication_ring3.h"
#include "Commnuication_ring3Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCommnuication_ring3Dlg 对话框



CCommnuication_ring3Dlg::CCommnuication_ring3Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCommnuication_ring3Dlg::IDD, pParent)
	, m_StrWrite(_T(""))
	, m_strRead(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCommnuication_ring3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_StrWrite);
	DDX_Text(pDX, IDC_EDIT2, m_strRead);
}

BEGIN_MESSAGE_MAP(CCommnuication_ring3Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CCommnuication_ring3Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CCommnuication_ring3Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CCommnuication_ring3Dlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CCommnuication_ring3Dlg 消息处理程序

BOOL CCommnuication_ring3Dlg::OnInitDialog()
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

	// TODO:  在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCommnuication_ring3Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCommnuication_ring3Dlg::OnPaint()
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
HCURSOR CCommnuication_ring3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCommnuication_ring3Dlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	hFile = CreateFile(
		L"\\??\\Hello",
		FILE_ALL_ACCESS,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(_T("连接失败"));
	}
}


void CCommnuication_ring3Dlg::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	DWORD dwSize = 0;
	WriteFile(
		hFile,
		m_StrWrite.GetBuffer(),
		(m_StrWrite.GetLength() + 1) * 2,
		&dwSize,
		NULL
		);
	CString Info;
	Info.Format(L"写入了%d个字节 ", dwSize);
	MessageBox(Info);
}


void CCommnuication_ring3Dlg::OnBnClickedButton3()
{
	// TODO:  在此添加控件通知处理程序代码
	DWORD dwSize = 0;
	WCHAR buf[200] = {};
	ReadFile(
		hFile,
		buf,
		400,
		&dwSize,
		NULL
		);
	CString Info(buf);
	MessageBox(Info);
}
