#pragma once
#include <Windows.h>
#include "UIID.h"
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include <wx/taskbar.h>
#include "BBDCLogin.h"
#include "CMonitor.h"

using namespace std;

class CMainForm : public wxFrame{

public:
	static CMainForm* GetInstance();
	CMainForm(const wxString& title, const wxPoint& pos, const wxSize& size);

	void Log(char *fmt, ...);
private:
	wxPanel* panelMain;
	wxTextCtrl* tBUsername;
	wxTextCtrl* tBPassword;
	wxSpinCtrl* NUDInternval;
	wxCheckBox* cBMonitor;
	wxListView* lVChecks;
	wxListView* lVRecords;
	wxListBox* lBLogs;
	wxTimer* TimerLoop;
	wxTaskBarIcon* TrayIcon;

	void OncBMonitorTick(wxCommandEvent& event);
	void OnlVChecksClick(wxCommandEvent& event);
	void OnTimer(wxTimerEvent& event);
	void OnTrayRightClickMenu(wxCommandEvent& event);

	string ExePath;

	CMonitor* Monitor;
	BBDCLogin* Login;
};