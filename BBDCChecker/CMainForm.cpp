#include "CMainForm.h"

CMainForm* CMainForm::GetInstance()
{
	static CMainForm* Instance = NULL;
	if (!Instance){
		Instance = new CMainForm("BBDC driving lesson monitor",wxDefaultPosition,wxSize(900,500));
		Instance->Center();
		Instance->SetMinClientSize(wxSize(900,500));
		//Instance->SetWindowStyle(wxMINIMIZE_BOX|wxSYSTEM_MENU|wxCAPTION);
		Instance->Show();
	}
	return Instance;
}

CMainForm::CMainForm(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, wxID_ANY, title, pos, size)
{
	wxLocale* Locale = new wxLocale();
	Locale->Init(wxLANGUAGE_ENGLISH);

	char Path[255];
	GetModuleFileNameA(NULL,Path,255);
	*strrchr(Path, '\\') = 0;
	ExePath = Path;

	panelMain = new wxPanel(this, -1, wxDefaultPosition, wxSize(0,0), wxTAB_TRAVERSAL);
	this->SetBackgroundColour(panelMain->GetBackgroundColour());

	wxBoxSizer* MainSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* RowSizer = new wxBoxSizer(wxHORIZONTAL);

	RowSizer->Add(new wxStaticText(panelMain,wxID_ANY,"Username:"),0,wxRIGHT|wxALIGN_CENTER_VERTICAL,5);
	tBUsername = new wxTextCtrl(panelMain,wxID_ANY);
	RowSizer->Add(tBUsername);
	RowSizer->Add(new wxStaticText(panelMain,wxID_ANY,"Password:"),0,wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL,5);
	tBPassword = new wxTextCtrl(panelMain,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_PASSWORD);
	RowSizer->Add(tBPassword);
	RowSizer->Add(new wxStaticText(panelMain,wxID_ANY,"Interval:"),0,wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL,5);
	NUDInternval = new wxSpinCtrl(panelMain,wxID_ANY,wxEmptyString,wxDefaultPosition,wxSize(70,22),wxSP_ARROW_KEYS,5,60,30);
	RowSizer->Add(NUDInternval);
	RowSizer->Add(new wxStaticText(panelMain,wxID_ANY,"minutes"),0,wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL,5);
	cBMonitor = new wxCheckBox(panelMain,wxID_ANY,"Start Monitoring");
	cBMonitor->Bind(wxEVT_CHECKBOX,&CMainForm::OncBMonitorTick,this);

	RowSizer->Add(cBMonitor,0,wxALIGN_CENTER_VERTICAL);
	MainSizer->Add(RowSizer,0,wxEXPAND|wxLEFT|wxTOP|wxRIGHT,5);

	RowSizer = new wxBoxSizer(wxHORIZONTAL);
	lVChecks = new wxListView(panelMain,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxLC_REPORT|wxLC_SINGLE_SEL);
	ListView_SetExtendedListViewStyle(lVChecks->GetHWND(), LVS_EX_GRIDLINES | ListView_GetExtendedListViewStyle(lVChecks->GetHWND()));
	lVChecks->Bind(wxEVT_LIST_ITEM_SELECTED,&CMainForm::OnlVChecksClick,this);

	lVChecks->AppendColumn("No.",wxLIST_FORMAT_LEFT,32);
	lVChecks->AppendColumn("Date Time",wxLIST_FORMAT_LEFT,150);
	lVChecks->AppendColumn("Lessons",wxLIST_FORMAT_LEFT,60);

	lVRecords = new wxListView(panelMain,wxID_ANY);
	ListView_SetExtendedListViewStyle(lVRecords->GetHWND(), LVS_EX_GRIDLINES | ListView_GetExtendedListViewStyle(lVRecords->GetHWND()));
	lVRecords->AppendColumn("Date",wxLIST_FORMAT_LEFT,120);
	lVRecords->AppendColumn("Session",wxLIST_FORMAT_LEFT,60);
	lVRecords->AppendColumn("Time",wxLIST_FORMAT_LEFT,120);

	RowSizer->Add(lVChecks,1,wxEXPAND|wxRIGHT,5);
	RowSizer->Add(lVRecords,1,wxEXPAND);
	MainSizer->Add(RowSizer,2,wxEXPAND|wxALL,5);

	MainSizer->Add(new wxStaticText(panelMain,wxID_ANY,"Log:"),0,wxLEFT,5);
	lBLogs = new wxListBox(panelMain,wxID_ANY);
	MainSizer->Add(lBLogs,1,wxEXPAND|wxALL,5);

	panelMain->SetSizer(MainSizer);
	panelMain->Layout();

	Monitor = new CMonitor(string(ExePath) + "\\Records.txt");
	for(int i = 0;i < Monitor->AllEntries.size();i++){
		long Index = lVChecks->InsertItem(lVChecks->GetItemCount(),wxString::Format("%d",i+1));
		lVChecks->SetItem(Index,1,Monitor->AllEntries[i].DateTime);
		lVChecks->SetItem(Index,2,wxString::Format("%d",Monitor->AllEntries[i].GetLessonsCount()));
	}
	Login = NULL;
	TimerLoop = new wxTimer(this);
	Bind(wxEVT_TIMER,&CMainForm::OnTimer,this);
	
	TrayIcon = new wxTaskBarIcon();
	TrayIcon->Bind(wxEVT_TASKBAR_LEFT_DCLICK,[&](wxTaskBarIconEvent&){
		this->Show();
	});
	TrayIcon->Bind(wxEVT_TASKBAR_CLICK,[&](wxTaskBarIconEvent&){
		wxMenu TrayMenu;
		TrayMenu.Append(0,"Quit");
		TrayMenu.Connect(wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CMainForm::OnTrayRightClickMenu,NULL,this);
		TrayIcon->PopupMenu(&TrayMenu);
	});
	
	TrayIcon->SetIcon(wxIcon("#101"),"BBDC driving lesson monitor");
	Bind(wxEVT_CLOSE_WINDOW,[&](wxCloseEvent&){
		this->Hide();
	});
}

void CMainForm::Log(char *fmt, ...)
{
	va_list list;
	char buffer[1024];
	static DWORD w;

	va_start(list, fmt);

	DWORD len = wvsprintf(buffer, fmt, list);
	//lBLogs->AppendString(
	lBLogs->AppendAndEnsureVisible("[" + wxString(CGeneric::GetTime().c_str()) + "]" + buffer);
	//lBLogs->EnsureVisible(lBLogs->GetCount()-1);
	lBLogs->SetSelection(lBLogs->GetCount()-1);
	lBLogs->SetSelection(-1);
	va_end(list);
}

void CMainForm::OnlVChecksClick(wxCommandEvent& event)
{
	int Index = -1;
	for(int i = 0;i < lVChecks->GetItemCount();i++){
		if (lVChecks->IsSelected(i)){
			Index = i;
			break;
		}
	}
	if (Index < 0){
		lVRecords->DeleteAllItems();
		return;
	}
	//wxMessageBox(wxString::Format("%d",Monitor->AllEntries[Index].DailyEntries.size()));
	lVRecords->DeleteAllItems();
	vector<DayEntry>& Entries = Monitor->AllEntries[Index].DailyEntries;
	for(int i = 0;i < Entries.size();i++){
		for(int i2 = 0;i2 < Entries[i].Lessons.size();i2++){
			long Index = lVRecords->InsertItem(lVRecords->GetItemCount(),Entries[i].Date + ", " + Entries[i].DayOfWeek);
			lVRecords->SetItem(Index,1,wxString::Format("%d",Entries[i].Lessons[i2].Session));
			lVRecords->SetItem(Index,2,Entries[i].Lessons[i2].StartTime + " - " + Entries[i].Lessons[i2].EndTime);
		}
	}
}

void CMainForm::OncBMonitorTick(wxCommandEvent& event)
{
	if (Login){
		delete Login;
	}
	Login = new BBDCLogin(tBUsername->GetValue().ToStdString(),tBPassword->GetValue().ToStdString());
	if (cBMonitor->GetValue()){
		tBUsername->Enable(false);
		tBPassword->Enable(false);
		NUDInternval->Enable(false);
		TimerLoop->Start(NUDInternval->GetValue() * 1000 * 60);
		Log("Monitoring started");
	}else{
		TimerLoop->Stop();
		tBUsername->Enable(true);
		tBPassword->Enable(true);
		NUDInternval->Enable(true);
		Log("Monitoring stopped");
	}
	/*AllocConsole();
	AttachConsole(GetCurrentProcessId());
	
	FILE* pFile = nullptr;
	freopen_s(&pFile, "CON", "r", stdin);
	freopen_s(&pFile, "CON", "w", stdout);
	freopen_s(&pFile, "CON", "w", stderr);*/
	//CMonitor Monitor(string(ExePath) + "\\Records.txt");
	/*BBDCLogin NewLogin("S9706326E","095528");
	if (!NewLogin.TryGetAvailableLessons()){
		wxMessageBox("Failed!");
		return;
	}
	Monitor.InputData(NewLogin.DailyEntries);*/
}

void CMainForm::OnTimer(wxTimerEvent& event)
{
	if (!Login){
		Log("Login Pointer is empty...");
		return;
	}
	switch (Login->TryGetAvailableLessons())
	{
	case NoError:
		{
			Log("Successfully grabbed lesson data");
			wxDateTime Date;
			Date.SetToCurrent();
			string DateTime = Date.FormatDate().ToStdString() + " " + Date.Format("%I:%M %p").ToStdString();
			Monitor->InputData(Login->DailyEntries,DateTime);
			long Index = lVChecks->InsertItem(lVChecks->GetItemCount(),wxString::Format("%d",lVChecks->GetItemCount()+1));
			lVChecks->SetItem(Index,1,DateTime);

			int Count = 0;
			for(int i = 0;i < Login->DailyEntries.size();i++){
				Count += Login->DailyEntries[i].Lessons.size();
			}
			lVChecks->SetItem(Index,2,wxString::Format("%d",Count));

			lVChecks->EnsureVisible(Index);
			break;
		}
	case LoginError1: Log("Failed to login, Error Code 1"); break;
	case LoginError2: Log("Failed to login, Error Code 2"); break;
	case SidePanelError1: Log("Failed to get side panel, Error Code 1"); break;
	case SidePanelError2: Log("Failed to get side panel, Error Code 2"); break;
	case BookingPage1Error1: Log("Failed to access booking page, Error Code 1"); break;
	case BookingPage1Error2: Log("Failed to access booking page, Error Code 2"); break;
	case SendAcceptError1: Log("Failed to send accept, Error Code 1"); break;
	case SendAcceptError2: Log("Failed to send accept, Error Code 2"); break;
	case GetAvailableLessonError1: Log("Failed to get available lessons, Error Code 1"); break;
	default:
		break;
	}
}

void CMainForm::OnTrayRightClickMenu(wxCommandEvent& event)
{
	ExitProcess(0);
}