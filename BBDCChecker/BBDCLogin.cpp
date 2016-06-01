#include "BBDCLogin.h"
#include "CFunctions.h"
#include <htmlcxx\html\ParserDom.h>
#pragma comment(lib,"htmlcxx\\htmlcxxStatic.lib")

using namespace htmlcxx;

BBDCLogin::BBDCLogin(string Username, string Password) : Username(Username),Password(Password)
{
	curl = NULL;
}

BBDCLogin::~BBDCLogin()
{
	CleanUp();
}

void BBDCLogin::InitCURL()
{
	curl = curl_easy_init();
	curl_global_init(CURL_GLOBAL_ALL);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, true);
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ReadBuffer);
}

void BBDCLogin::CleanUp()
{
	if (curl){
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		curl = NULL;
	}
}

size_t BBDCLogin::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

BBDCErrorCodes BBDCLogin::TryLogin()
{
	curl_easy_setopt(curl, CURLOPT_URL, "https://www.bbdc.sg/bbdc/bbdc_web/header2.asp");
	curl_easy_setopt(curl, CURLOPT_REFERER, "https://www.bbdc.sg/bbdc/bbdc_web/newheader.asp");
	//curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	string POSTArg = "txtNRIC=" + Username + "&txtPassword=" + Password + "&btnLogin=+";
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, POSTArg);

	CURLcode response = curl_easy_perform(curl);

	if (response != CURLE_OK){
		return LoginError1;
	}

	char *url;
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
	if (!strstr(url,"b-mainframe.asp")){
		return LoginError2;
	}
	return NoError;
}

void BBDCLogin::Logout()
{
	curl_easy_setopt(curl, CURLOPT_URL, "https://www.bbdc.sg/bbdc/b-logoutleonnew.asp");
	curl_easy_perform(curl);

	CleanUp();
}

void BBDCLogin::TryEnumerateHumanAccess()
{
	curl_easy_setopt(curl, CURLOPT_URL, "https://www.bbdc.sg/bbdc/inc-webpage/b-topnav.asp");
	CURLcode response = curl_easy_perform(curl);
	curl_easy_setopt(curl, CURLOPT_URL, "https://www.bbdc.sg/bbdc/b-default.asp");
	response = curl_easy_perform(curl);
	curl_easy_setopt(curl, CURLOPT_URL, "https://www.bbdc.sg/bbdc/inc-webpage/b-footer.asp");
	response = curl_easy_perform(curl);
}

BBDCErrorCodes BBDCLogin::TryGetSidePanel()
{
	ReadBuffer = "";
	curl_easy_setopt(curl, CURLOPT_URL, "https://www.bbdc.sg/bbdc/inc-webpage/b-sidenav-3.asp");
	if (curl_easy_perform(curl) != CURLE_OK){
		return SidePanelError1;
	}

	HTML::ParserDom parser;
	tree<HTML::Node> dom = parser.parseTree(ReadBuffer);

	tree<HTML::Node>::iterator it = dom.begin();
	tree<HTML::Node>::iterator end = dom.end();

	bool Found = false;
	for (; it != end; ++it){
		it->parseAttributes();
		if (it->text() == "Booking without Fixed Instructor"){
			tree<HTML::Node>::iterator prev = it;
			prev--;
			Found = true;
			BookWithoutInstructorSite = CFunctions::ReplaceString(prev->attribute("href").second,"..","https://www.bbdc.sg/bbdc");
			break;
		}
	}
	return Found ? NoError : SidePanelError2;
}

BBDCErrorCodes BBDCLogin::TryAccessBookingPage1()
{
	ReadBuffer = "";
	curl_easy_setopt(curl, CURLOPT_URL, BookWithoutInstructorSite);
	if (curl_easy_perform(curl) != CURLE_OK){
		return BookingPage1Error1;
	}

	HTML::ParserDom parser;
	tree<HTML::Node> dom = parser.parseTree(ReadBuffer);

	tree<HTML::Node>::iterator it = dom.begin();
	tree<HTML::Node>::iterator end = dom.end();

	bool Found = false;
	for (; it != end; ++it){
		it->parseAttributes();
		if (it->isTag() && it->tagName() == "input" && it->attribute("onclick").second.size() > 0){
			string Attr = it->attribute("onclick").second;

			Found = true;
			AcceptSite = CFunctions::ReplaceString(Attr,"javascript:location.href='","https://www.bbdc.sg/bbdc/");
			AcceptSite = CFunctions::ReplaceString(AcceptSite,"'","");
			break;
		}
	}
	return Found ? NoError : BookingPage1Error2;
}

BBDCErrorCodes BBDCLogin::TrySendAcceptCondition()
{
	ReadBuffer = "";
	curl_easy_setopt(curl, CURLOPT_URL, AcceptSite);
	if ( curl_easy_perform(curl) != CURLE_OK){
		return SendAcceptError1;
	}
	HTML::ParserDom parser;
	tree<HTML::Node> dom = parser.parseTree(ReadBuffer);

	tree<HTML::Node>::iterator it = dom.begin();
	tree<HTML::Node>::iterator end = dom.end();

	bool Found = false;
	int Count = 0;
	for (; it != end; ++it){
		it->parseAttributes();
		if (it->isTag() && it->tagName() == "SCRIPT"){
			if (Count > 0){
				++it;
				AccID = it->text();
				AccID = AccID.substr(AccID.find('\'')+1);
				AccID = AccID.substr(0,AccID.find('\''));
				Found = true;
				break;
			}
			Count++;
		}
	}
	return Found ? NoError : SendAcceptError2;
}

BBDCErrorCodes BBDCLogin::TryGetAvailableLessons()
{
	DailyEntries.clear();

	InitCURL();
	BBDCErrorCodes Result = TryLogin();
	if (Result != NoError){
		CleanUp();
		//MessageBoxA(NULL,"Failed to login!",NULL,NULL);
		return Result;
	}
	TryEnumerateHumanAccess();
	Result = TryGetSidePanel();
	if (Result != NoError){
		Logout();
		//MessageBoxA(NULL,"Failed to get side panel!",NULL,NULL);
		return Result;
	}
	Result = TryAccessBookingPage1();
	if (Result != NoError){
		Logout();
		//MessageBoxA(NULL,"Failed to access booking page!",NULL,NULL);
		return Result;
	}
	Result = TrySendAcceptCondition();
	if (Result != NoError){
		Logout();
		MessageBoxA(NULL,"Failed to send accept condition!",NULL,NULL);
		return Result;
	}

	string POSTArg = "accId=" + AccID +
		"&Month=Jan/2016"
		"&Month=Feb/2016"
		"&Month=Mar/2016"
		"&Month=Apr/2016"
		"&allMonth="
		"&Session=1"
		"&Session=2"
		"&Session=3"
		"&Session=4"
		"&Session=5"
		"&Session=6"
		"&Session=7"
		"&Session=8"
		"&allSes=on"
		"&Day=2"
		"&Day=3"
		"&Day=4"
		"&Day=5"
		"&Day=6"
		"&Day=7"
		"&Day=1"
		"&allDay="
		"&defPLVenue=1"
		"&optVenue=1";
	ReadBuffer = "";
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, POSTArg.c_str());
	curl_easy_setopt(curl, CURLOPT_URL, "https://www.bbdc.sg/bbdc/b-3c-pLessonBooking1.asp" );
	if (curl_easy_perform(curl) != CURLE_OK){
		Logout();
		//MessageBoxA(NULL,"Failed to get booking dates!",NULL,NULL);
		return GetAvailableLessonError1;
	}
	ProcessResult();
	Logout();
	return NoError;
}

void BBDCLogin::ProcessResult()
{
	HTML::ParserDom parser;
	tree<HTML::Node> dom = parser.parseTree(ReadBuffer);
	tree<HTML::Node>::iterator it = dom.begin();
	tree<HTML::Node>::iterator end = dom.end();
	int EndOffset = 0;
	int TDCount = 0;
	bool LookForEntries = false;
	DayEntry NewDayEntry;

	for (; it != end; ++it){
		it->parseAttributes();
		if (it->isTag()){
			if (it->tagName() == "table" && it->attribute("bgcolor").second == "#666666"){
				EndOffset = it->offset() + it->length();
				LookForEntries  = true;
			}
			if (LookForEntries){
				if (it->tagName() == "tr" && it->attribute("bgcolor").second == "#FFFFFF"){
					//new day entry
					if (TDCount > 0){
						TDCount = 0;
						DailyEntries.push_back(NewDayEntry);
						NewDayEntry = DayEntry();
					}
					NewDayEntry.EndOffset = it->offset() + it->length();
				}else if (it->tagName() == "td" && NewDayEntry.EndOffset > 0 && TDCount == 0){
					//1st entry
					TDCount++;
					NewDayEntry.Date = (++it)->text();
					++it;
					NewDayEntry.DayOfWeek = (++it)->text();
				}else if (it->tagName() == "td" && TDCount > 0 && it->attribute("onmouseover").second.size() > 0){
					TDCount++;
					string Attr = it->attribute("onmouseover").second;
					vector<string> Split = CFunctions::split_string(Attr,",");
					LessonInfo NewLessonInfo;
					NewLessonInfo.StartTime = CFunctions::ReplaceString(Split[4],"\"","");
					NewLessonInfo.EndTime = CFunctions::ReplaceString(Split[5],"\"","");
					NewLessonInfo.Session = atoi(CFunctions::ReplaceString(Split[3],"\"","").c_str());
					NewDayEntry.Lessons.push_back(NewLessonInfo);
				}
			}
		}
		if (EndOffset > 0 && it->offset() == EndOffset){
			printf("Ending Found\n");
			if (TDCount > 0){
				TDCount = 0;
				DailyEntries.push_back(NewDayEntry);
				NewDayEntry = DayEntry();
			}
			LookForEntries = false;
			break;
		}
	}
}