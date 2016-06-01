#include "UIID.h"
#include <wx/mstream.h>
#include <iomanip>
#include <thread>
#include <sstream>

bool CWXFunction::LoadDataFromResource(char*& t_data, DWORD& t_dataSize, int ResourceID)
{
	bool r_result    = false;
	HGLOBAL a_resHandle = 0;
	HRSRC a_resource;
  
	a_resource = FindResource(0,MAKEINTRESOURCE(ResourceID), RT_RCDATA);
  
	if(a_resource != 0){
		a_resHandle = LoadResource(NULL, a_resource);
		if (0 != a_resHandle){
			t_data = (char*)LockResource(a_resHandle);
			t_dataSize = SizeofResource(NULL, a_resource);
			r_result = true;
		}
	}
	return r_result;
}

wxBitmap* CWXFunction::GetBitmapFromMemory(const char* t_data, const DWORD t_size)
{
	wxMemoryInputStream a_is(t_data, t_size);
	return new wxBitmap(wxImage(a_is, wxBITMAP_TYPE_PNG, -1), -1);
}

wxBitmap* CWXFunction::CreateBitmapFromPngResource(int ResourceID)
{
	wxBitmap*   r_bitmapPtr = 0;
  
	char*       a_data      = 0;
	DWORD       a_dataSize  = 0;
  
	if(LoadDataFromResource(a_data, a_dataSize, ResourceID))
	{
		r_bitmapPtr = GetBitmapFromMemory(a_data, a_dataSize);
	}
	return r_bitmapPtr;
}

std::string CGeneric::GetTime()
{
	std::chrono::system_clock::time_point time_point_now = std::chrono::system_clock::now();
	std::time_t time_now = std::chrono::system_clock::to_time_t(time_point_now);
	std::put_time(std::localtime(&time_now), "%F %T");
	tm TimeNow = *std::localtime(&time_now);

	char lpszTime[256];
	strftime(lpszTime, 32, "%H:%M:%S", &TimeNow);
	return std::string(lpszTime);
}

std::string CGeneric::int_to_hex(unsigned int i ,int NoOfHexDec)
{
  std::stringstream stream;
  stream << std::setfill ('0') << std::setw(NoOfHexDec) 
         << std::uppercase << std::hex << i;
  return stream.str();
}

string CGeneric::FormatString(const char* lpcszFormat, ...)
{
	char lpszBuffer[1024];

	va_list va;
	va_start(va, lpcszFormat);

	vsprintf(lpszBuffer, lpcszFormat, va);
	va_end(va);

	return lpszBuffer;
}