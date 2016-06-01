#pragma once
#define wxDEBUG_LEVEL 0
#define NDEBUG

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    //#include <wx/wx.h>
	#include <wx/frame.h>
	#include <wx/bitmap.h>
	#include <wx/button.h>
	#include <wx/panel.h>
	#include <wx/sizer.h>
	#include <wx/textctrl.h>
	#include <wx/checkbox.h>
	#include <wx/listbox.h>
	#include <wx/app.h>
	#include <wx/msgdlg.h>
	#include <wx/dcclient.h>
	#include <wx/stattext.h>
	#include <wx/statbox.h>
#endif
#include <wx/mediactrl.h>
#include <string>

using namespace std;

#define OurDataPath "Data\\Data.dat"

class CWXFunction{

public:
	static wxBitmap* CreateBitmapFromPngResource(int ResourceID);

private:
	static bool LoadDataFromResource(char*& t_data, DWORD& t_dataSize, int ResourceID);
	static wxBitmap* GetBitmapFromMemory(const char* t_data, const DWORD t_size);
};

class CGeneric{

public:
	static std::string FormatString(const char* lpcszFormat, ...);
	static std::string GetTime();

	static std::string int_to_hex(unsigned int i ,int NoOfHexDec);
};