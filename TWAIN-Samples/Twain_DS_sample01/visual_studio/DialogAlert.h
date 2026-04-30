#pragma once
#include "afxdialogex.h"


// caixa de diálogo de DialogAlert

class DialogAlert : public CDialog
{
	DECLARE_DYNAMIC(DialogAlert)

public:
	DialogAlert(CWnd* pParent = nullptr);   // construtor padrão
	virtual ~DialogAlert();

// Janela de Dados
#ifdef AFX_DESIGN_TIME
	enum { IDD = alert };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Suporte DDX/DDV

	DECLARE_MESSAGE_MAP()
};
