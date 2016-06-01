#pragma once
#include <Windows.h>
#include <string>
#include <vector>

using namespace std;

struct LessonInfo{
	string StartTime;
	string EndTime;
	int Session;
};

struct DayEntry{
	string Date;
	string DayOfWeek;
	vector<LessonInfo> Lessons;
	unsigned int EndOffset;

	DayEntry(){
		EndOffset = 0;
	}
};

struct CheckEntry{
	string DateTime;
	vector<DayEntry> DailyEntries;

	int GetLessonsCount(){
		int Count = 0;
		for(int i = 0;i < DailyEntries.size();i++){
			Count += DailyEntries[i].Lessons.size();
		}
		return Count;
	}
};

class CMonitor{

public:
	CMonitor(string FilePath);

	void LoadPrevData();
	void SaveData();
	
	void InputData(vector<DayEntry>& Entries,string DateTime);
	vector<CheckEntry> AllEntries;
private:
	string FilePath;
};