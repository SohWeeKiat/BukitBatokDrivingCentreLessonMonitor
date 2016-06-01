#include <Windows.h>
#include "CMainForm.h"

class MyApp: public wxApp{

public:
    virtual bool OnInit();
};

bool MyApp::OnInit()
{
	CMainForm::GetInstance();
	return true;
}

wxIMPLEMENT_APP(MyApp);