
// Commnuication_ring3Dlg.h : ͷ�ļ�
//

#pragma once


// CCommnuication_ring3Dlg �Ի���
class CCommnuication_ring3Dlg : public CDialogEx
{
// ����
public:
	CCommnuication_ring3Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_COMMNUICATION_RING3_DIALOG };

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
	afx_msg void OnBnClickedButton1();
private:
	HANDLE hFile;
public:
	CString m_StrWrite;
	afx_msg void OnBnClickedButton2();
	CString m_strRead;
	afx_msg void OnBnClickedButton3();
};
