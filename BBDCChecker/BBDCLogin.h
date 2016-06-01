#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include "CMonitor.h"
#include <curl\curl.h>

#pragma comment(lib,"libcurl.lib")

using namespace std;

enum BBDCErrorCodes{
	NoError,
	LoginError1,
	LoginError2,
	SidePanelError1,
	SidePanelError2,
	BookingPage1Error1,
	BookingPage1Error2,
	SendAcceptError1,
	SendAcceptError2,
	GetAvailableLessonError1
};

class BBDCLogin{

public:
	BBDCLogin(string Username, string Password);
	~BBDCLogin();

	void InitCURL();
	void CleanUp();
	BBDCErrorCodes TryGetAvailableLessons();
	vector<DayEntry> DailyEntries;
private:
	BBDCErrorCodes TryLogin();
	void Logout();
	void TryEnumerateHumanAccess();
	BBDCErrorCodes TryGetSidePanel();
	BBDCErrorCodes TryAccessBookingPage1();
	BBDCErrorCodes TrySendAcceptCondition();
	void ProcessResult();

	CURL* curl;
	string Username;
	string Password;
	string AccID;
	string BookWithoutInstructorSite;
	string AcceptSite;
	string ReadBuffer;

	
	static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
};