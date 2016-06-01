#include "CMonitor.h"
#include <sstream>
#include <fstream>
#include <Poco/Util/XMLConfiguration.h>
#include <Poco/Util/AbstractConfiguration.h>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

using Poco::AutoPtr;
using Poco::Util::XMLConfiguration;
using Poco::Util::AbstractConfiguration;

bool operator==(const LessonInfo& lhs, const LessonInfo& rhs)
{
    return lhs.StartTime==rhs.StartTime && lhs.EndTime==rhs.EndTime && lhs.Session==rhs.Session;
}

bool operator==(const DayEntry& lhs, const DayEntry& rhs)
{
    return lhs.Date==rhs.Date && lhs.DayOfWeek==rhs.DayOfWeek && lhs.Lessons==rhs.Lessons;
}

bool operator==(const CheckEntry& lhs, const CheckEntry& rhs)
{
    return lhs.DateTime==rhs.DateTime && lhs.DailyEntries==rhs.DailyEntries;
}

namespace boost { namespace serialization {
    template<class Archive>
    void serialize(Archive & ar, CheckEntry& v, const unsigned int version)
    {
        ar & v.DateTime; 
		ar & v.DailyEntries; 
    }
	template<class Archive>
	void serialize(Archive & ar, DayEntry& v, const unsigned int version)
    {
        ar & v.Date; 
		ar & v.DayOfWeek; 
		ar & v.Lessons;
    }
	template<class Archive>
	void serialize(Archive & ar, LessonInfo& v, const unsigned int version)
    {
        ar & v.StartTime; 
		ar & v.EndTime; 
		ar & v.Session;
    }
} }

CMonitor::CMonitor(string FilePath) : FilePath(FilePath)
{
	LoadPrevData();

	/*ofstream os("C:\\Users\\Wee Kiat\\Documents\\Visual Studio 2012\\Projects\\BBDCChecker\\Release\\RecordsB.txt", ios::binary);
	boost::archive::binary_oarchive oar(os);
    oar << AllEntries;
    os.close();*/

	/*ifstream is("C:\\Users\\Wee Kiat\\Documents\\Visual Studio 2012\\Projects\\BBDCChecker\\Release\\RecordsB.txt", ios::binary);
    boost::archive::binary_iarchive iar(is);
	vector<CheckEntry> list2;
    iar >> list2;
	AllEntries = list2;
	if (AllEntries == list2){
		MessageBoxA(NULL,"Passed",NULL,NULL);
	}else{
		MessageBoxA(NULL,"Failed",NULL,NULL);
	}*/
}

void CMonitor::LoadPrevData()
{
	AutoPtr<XMLConfiguration> SavedData(new XMLConfiguration(FilePath));

	AbstractConfiguration::Keys key1;
	SavedData->keys("Logs", key1);
	for(int i = 0;i < key1.size();i++){
		AbstractConfiguration::Keys key2;
		SavedData->keys("Logs.Log[" + std::to_string(i) + "].Entrys", key2);
		CheckEntry NewCheckEntry;
		NewCheckEntry.DateTime = SavedData->getString("Logs.Log[" + std::to_string(i) + "].DateTime");
		for(int i2 = 0;i2 < key2.size();i2++){
			DayEntry NewDayEntry;
			NewDayEntry.Date = SavedData->getString("Logs.Log[" + std::to_string(i) + "].Entrys.Entry[" + std::to_string(i2) + "].Date");
			NewDayEntry.DayOfWeek = SavedData->getString("Logs.Log[" + std::to_string(i) + "].Entrys.Entry[" + std::to_string(i2) + "].DayOfWeek");
		
			AbstractConfiguration::Keys key3;
			SavedData->keys("Logs.Log[" + std::to_string(i) + "].Entrys.Entry[" + std::to_string(i2) + "].Lessons", key3);
			//char hehe[25];
			//sprintf(hehe,"%d",key3.size());
			//MessageBoxA(NULL,hehe,NULL,NULL);
			for(int i3 = 0;i3 < key3.size();i3++){
				LessonInfo NewLessonInfo;
				NewLessonInfo.StartTime = SavedData->getString("Logs.Log[" + std::to_string(i) + "].Entrys.Entry[" + std::to_string(i2) + "].Lessons.Lesson[" + std::to_string(i3) + "].StartTime");
				NewLessonInfo.EndTime = SavedData->getString("Logs.Log[" + std::to_string(i) + "].Entrys.Entry[" + std::to_string(i2) + "].Lessons.Lesson[" + std::to_string(i3) + "].EndTime");
				NewLessonInfo.Session = SavedData->getInt("Logs.Log[" + std::to_string(i) + "].Entrys.Entry[" + std::to_string(i2) + "].Lessons.Lesson[" + std::to_string(i3) + "].Session");
				NewDayEntry.Lessons.push_back(NewLessonInfo);
			}
			NewCheckEntry.DailyEntries.push_back(NewDayEntry);
		}
		AllEntries.push_back(NewCheckEntry);
	}
}

void CMonitor::SaveData()
{
	AutoPtr<XMLConfiguration> NewLog = new XMLConfiguration;

	NewLog->loadEmpty("BBDC");
	for(int i = 0;i < AllEntries.size();i++){
		NewLog->setString("Logs.Log[" + std::to_string(i) + "].DateTime",AllEntries[i].DateTime);
		for(int i2 = 0;i2 < AllEntries[i].DailyEntries.size();i2++){
			NewLog->setString("Logs.Log[" + std::to_string(i) + "].Entrys.Entry[" + std::to_string(i2) + "].Date",AllEntries[i].DailyEntries[i2].Date);
			NewLog->setString("Logs.Log[" + std::to_string(i) + "].Entrys.Entry[" + std::to_string(i2) + "].DayOfWeek",AllEntries[i].DailyEntries[i2].DayOfWeek);
			for(int i3 = 0;i3 < AllEntries[i].DailyEntries[i2].Lessons.size();i3++){
				NewLog->setString("Logs.Log[" + std::to_string(i) + "].Entrys.Entry[" + std::to_string(i2) + "].Lessons.Lesson[" + std::to_string(i3) + "].StartTime",AllEntries[i].DailyEntries[i2].Lessons[i3].StartTime);
				NewLog->setString("Logs.Log[" + std::to_string(i) + "].Entrys.Entry[" + std::to_string(i2) + "].Lessons.Lesson[" + std::to_string(i3) + "].EndTime",AllEntries[i].DailyEntries[i2].Lessons[i3].EndTime);
				NewLog->setInt("Logs.Log[" + std::to_string(i) + "].Entrys.Entry[" + std::to_string(i2) + "].Lessons.Lesson[" + std::to_string(i3) + "].Session",AllEntries[i].DailyEntries[i2].Lessons[i3].Session);
			}
		}
	}
	std::ostringstream StringStream;
	NewLog->save(StringStream);
	FILE* F = fopen(FilePath.c_str(),"w+");
	if (F){
		string Temp = StringStream.str();
		fputs(Temp.c_str(),F);
		fclose(F);
	}
}

void CMonitor::InputData(vector<DayEntry>& Entries,string DateTime)
{
	CheckEntry NewCheckEntry;
	NewCheckEntry.DateTime = DateTime;
	NewCheckEntry.DailyEntries = Entries;
	AllEntries.push_back(NewCheckEntry);
	SaveData();
}