#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cctype>
#include <algorithm>
#include <fstream>

using namespace std;

namespace Menu {
	constexpr int EXIT = 0;
	constexpr int ADD_SCHEDULE = 1;
	constexpr int ADD_REPEAT_SCHEDULE = 2;
	constexpr int SHOW_TODAY = 3;
	constexpr int SHOW_CALENDAR = 4;
	constexpr int SHOW_BY_DATE = 5;
	constexpr int SHOW_ALL = 6;
	constexpr int EDIT_SCHEDULE = 7;
	constexpr int DELETE_SCHEDULE = 8;
	constexpr int MANAGE_REPEAT_SCHEDULE = 9;
}

namespace Config {
	constexpr int TITLE_PREVIEW_LENGTH = 4;
	constexpr int MIN_YEAR = 2026;
	constexpr int MAX_YEAR = 2099;
	constexpr int CALENDAR_CELL_WIDTH = 12;

	constexpr int MIN_MONTH = 1;
	constexpr int MAX_MONTH = 12;
	constexpr int MIN_DAY = 1;
	constexpr int MIN_HOUR = 0;
	constexpr int MAX_HOUR = 23;
	constexpr int MIN_MINUTE = 0;
	constexpr int MAX_MINUTE = 59;
	constexpr int LEAP_YEAR_REFERENCE = 2000;
	constexpr const char* SAVE_FILE_NAME = "schedule.txt";
	constexpr int MAX_REPEAT_SEARCH_DAYS = 3650;
}

enum class RepeatType
{
	DAILY,
	WEEKLY,
	MONTHLY
};

struct RepeatSchedule
{
	int id;

	RepeatType repeatType;

	string repeatRule;

	int hour;
	int minute;

	string title;

	int startYear;
	int startMonth;
	int startDay;

	int endYear;
	int endMonth;
	int endDay;
};

struct Schedule {
	int id;

	int year;
	int month;
	int day;

	int hour;
	int minute;

	string title;
};

vector<Schedule> g_scheduleList;
vector<RepeatSchedule> g_repeatScheduleList;
int g_nextId = 1;

// ===== 메뉴 =====
void showMenu();						// 메인 메뉴 출력
void manageRepeatSchedule();			// 반복 일정 관리 메뉴
// ===== 일반 일정 =====
void addSchedule();						// 일반 일정 추가
void editSchedule();					// 일반 일정 수정
void deleteSchedule();					// 일반 일정 삭제
// ===== 반복 일정 =====
void addRepeatSchedule();				// 반복 일정 추가
void editRepeatSchedule();				// 반복 일정 수정
void deleteRepeatSchedule();			// 반복 일정 삭제
// ===== 조회 =====
void showTodaySchedules();				// 오늘 일정 조회
void showScheduleByDate();				// 날짜별 일정 조회
void showAllSchedules();				// 전체 일정 조회
void showRepeatSchedules();				// 반복 일정 목록 조회
void showMonthlyCalendar();				// 월간 달력 출력
// ===== 저장 / 불러오기 =====
void saveSchedules();					// 일정 파일 저장
void loadSchedules();					// 일정 파일 불러오기
// ===== 입력 처리 =====
bool parseScheduleInput(const string& input,Schedule& schedule);						// 일반 일정 입력 파싱
bool parseRepeatInput(const string& input,RepeatSchedule& schedule,int repeatMenu);		// 반복 일정 입력 파싱
bool parseDate(int& year,int& month,int& day);											// 날짜 입력 파싱
bool getEditDateInput(int& year,int& month,int& day,const string& message);				// 수정용 날짜 입력
bool getRepeatEditInput(RepeatSchedule& schedule,int repeatMenu);						// 반복 일정 수정 입력
// ===== 날짜 / 시간 검증 =====
bool isLeapYear(int year);																// 윤년 판별
int getDaysInMonth(int year,int month);													// 월별 최대 일수 반환
bool isValidDate(int year,int month,int day);											// 유효한 날짜 검사
bool isEarlierDate(int year1,int month1,int day1,int year2,int month2,int day2);		// 날짜 선후 비교
// ===== 반복 일정 처리 =====
bool isDateInRange(int year,int month,int day,const RepeatSchedule& schedule);			// 반복 일정 기간 내 여부
bool isRepeatMatched(const RepeatSchedule& schedule,int year,int month,int day);		// 반복 조건 일치 여부
bool getNextOccurrence(const RepeatSchedule& schedule,int& year,int& month,int& day);	// 다음 발생 날짜 조회
string getRepeatInfo(const RepeatSchedule& schedule);									// 반복 정보 문자열 생성
void addRepeatSchedulesForDate(vector<Schedule>& schedules,int year,int month,int day);	// 반복 일정을 일반 일정 형태로 추가
// ===== 정렬 =====
bool compareSchedule(const Schedule& a,const Schedule& b);		// 일정 정렬 기준
void sortSchedules();											// 일정 정렬
// ===== 달력 출력 =====
int getDayOfWeek(int year,int month,int day);					// 요일 계산
void printHorizontalLine();										// 달력 구분선 출력
vector<string> getScheduleLines(int year,int month,int day);	// 달력 셀 일정 문자열 생성
// ===== 출력 =====
void printDateTime(const Schedule& schedule);					// 일정 날짜/시간 출력
string getSchedulePreview(const string& title);					// 달력용 제목 축약

int main(void)
{
	loadSchedules();

	int menu;
	while (true) {
		showMenu();
		cout << "\n메뉴 선택 : ";

		if (!(cin >> menu)) {
			cout << "\n[오류] 메뉴는 숫자로 입력해주세요.\n\n";
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			continue;
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		switch (menu) {
		case Menu::ADD_SCHEDULE:
			addSchedule();
			break;
		case Menu::ADD_REPEAT_SCHEDULE:
			addRepeatSchedule();
			break;
		case Menu::SHOW_TODAY:
			showTodaySchedules();
			break;
		case Menu::SHOW_CALENDAR:
			showMonthlyCalendar();
			break;
		case Menu::SHOW_BY_DATE:
			showScheduleByDate();
			break;
		case Menu::SHOW_ALL:
			showAllSchedules();
			break;
		case Menu::EDIT_SCHEDULE:
			editSchedule();
			break;
		case Menu::DELETE_SCHEDULE:
			deleteSchedule();
			break;
		case Menu::MANAGE_REPEAT_SCHEDULE:
			manageRepeatSchedule();
			break;
		case Menu::EXIT:
			saveSchedules();
			return EXIT_SUCCESS;
		default:
			cout << "\n[오류] 잘못된 메뉴입니다.\n";
			cout << "다시 입력해주세요.\n\n";
		}
	}
	return EXIT_SUCCESS;
}

void showMenu() {
	cout << "==== TimeCollieMini ====\n\n";
	cout << "    1. 일정 추가" << endl;
	cout << "    2. 반복 일정 추가" << endl;
	cout << "    3. 오늘 일정 보기" << endl;
	cout << "    4. 월간 달력 보기" << endl;
	cout << "    5. 날짜별 조회" << endl;
	cout << "    6. 전체 일정 조회" << endl;
	cout << "    7. 일정 수정" << endl;
	cout << "    8. 일정 삭제" << endl;
	cout << "    9. 반복 일정 수정/삭제" << endl;
	cout << "    0. 종료" << endl;
}

void manageRepeatSchedule() {

	while (true) {

		cout << "\n==== 반복 일정 관리 ====\n\n";

		cout << "1. 반복 일정 수정\n";
		cout << "2. 반복 일정 삭제\n";
		cout << "0. 뒤로가기\n";

		cout << "\n선택 : ";

		int menu;

		if (!(cin >> menu)) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			cout << "\n[오류] 숫자를 입력해주세요.\n";
			continue;
		}

		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		switch (menu) {

		case 1:
			editRepeatSchedule();
			return;

		case 2:
			deleteRepeatSchedule();
			return;

		case 0:
			return;

		default:
			cout << "\n[오류] 잘못된 메뉴입니다.\n";
		}
	}
}

void addSchedule()
{
	cout << "\n입력 형식 : YYYY-MM-DD HH:MM 제목\n";
	cout << "입력 : ";

	Schedule schedule;

	string input;
	getline(cin, input);

	if (!parseScheduleInput(input, schedule))
	{
		return;
	}

	schedule.id = g_nextId++;

	g_scheduleList.push_back(schedule);

	sortSchedules();
	saveSchedules();

	cout << "\n일정이 등록되었습니다.\n\n";
}

void editSchedule()
{
	if (g_scheduleList.empty())
	{
		cout << "\n등록된 일정이 없습니다.\n";
		return;
	}

	int id;

	showAllSchedules();

	cout << "\n==== 일정 수정 ====\n";
	cout << "수정할 ID : ";

	if (!(cin >> id))
	{
		cout << "\n[오류] 숫자를 입력해주세요.\n";
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return;
	}

	if (id <= 0)
	{
		cout << "\n[오류] 올바른 ID를 입력해주세요.\n";
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return;
	}

	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	for (Schedule& schedule : g_scheduleList)
	{
		if (schedule.id == id)
		{
			cout << "\n==== 현재 일정 ====\n";

			cout << "ID : " << schedule.id << '\n';

			cout << "날짜 : "
				<< schedule.year << '-'
				<< setw(2) << setfill('0') << schedule.month << '-'
				<< setw(2) << setfill('0') << schedule.day
				<< setfill(' ') << '\n';

			cout << "시간 : "
				<< setw(2) << setfill('0') << schedule.hour << ':'
				<< setw(2) << setfill('0') << schedule.minute
				<< setfill(' ') << '\n';

			cout << "제목 : ";

			if (schedule.title.empty())
			{
				cout << "(제목 없음)\n";
			}
			else
			{
				cout << schedule.title << '\n';
			}

			cout << "\n새 일정 입력\n";
			cout << "입력 형식 : YYYY-MM-DD HH:MM 제목\n";
			cout << "입력 : ";

			string input;
			getline(cin, input);

			Schedule newSchedule = schedule;

			if (!parseScheduleInput(input, newSchedule))
			{
				return;
			}

			// 기존 ID 유지
			newSchedule.id = schedule.id;

			schedule = newSchedule;

			sortSchedules();
			saveSchedules();

			cout << "\n일정이 수정되었습니다.\n\n";
			return;
		}
	}

	cout << "\n[오류] 해당 ID를 찾을 수 없습니다.\n\n";
}

void deleteSchedule()
{
	if (g_scheduleList.empty())
	{
		cout << "\n등록된 일정이 없습니다.\n";
		return;
	}

	int id;

	showAllSchedules();

	cout << "\n==== 일정 삭제 ====\n";
	cout << "삭제할 ID : ";



	if (!(cin >> id))
	{
		cout << "\n[오류] 숫자를 입력해주세요.\n";
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return;
	}

	if (id <= 0)
	{
		cout << "\n[오류] 올바른 ID를 입력해주세요.\n\n";
		return;
	}

	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	for (size_t i = 0; i < g_scheduleList.size(); i++)
	{
		if (g_scheduleList[i].id == id)
		{
			char confirm;

			cout << "\n==== 삭제 대상 일정 ====\n\n";

			cout << "ID : "
				<< g_scheduleList[i].id << '\n';

			cout << "날짜 : "
				<< g_scheduleList[i].year << '-'
				<< setw(2) << setfill('0') << g_scheduleList[i].month << '-'
				<< setw(2) << setfill('0') << g_scheduleList[i].day
				<< setfill(' ') << '\n';

			cout << "시간 : "
				<< setw(2) << setfill('0') << g_scheduleList[i].hour << ':'
				<< setw(2) << setfill('0') << g_scheduleList[i].minute
				<< setfill(' ') << '\n';

			cout << "제목 : ";

			if (g_scheduleList[i].title.empty())
			{
				cout << "(제목 없음)\n";
			}
			else
			{
				cout << g_scheduleList[i].title << '\n';
			}

			cout << "\n정말 삭제하시겠습니까? (Y/N) : ";

			cin >> confirm;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			confirm = toupper(confirm);

			if (confirm != 'Y')
			{
				cout << "\n삭제가 취소되었습니다.\n\n";
				return;
			}

			g_scheduleList.erase(g_scheduleList.begin() + i);
			saveSchedules();

			cout << "\n일정이 삭제되었습니다.\n\n";
			return;
		}
	}

	cout << "\n[오류] 해당 ID를 찾을 수 없습니다.\n\n";
}

void addRepeatSchedule()
{
	cout << "\n==== 반복 일정 추가 ====\n\n";

	cout << "1. 매일\n";
	cout << "2. 매주\n";
	cout << "3. 매월\n";

	cout << "\n선택 : ";

	int repeatMenu;

	if (!(cin >> repeatMenu))
	{
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		cout << "\n[오류] 숫자를 입력해주세요.\n";
		return;
	}

	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	if (repeatMenu < 1 || repeatMenu > 3)
	{
		cout << "\n[오류] 잘못된 메뉴입니다.\n";
		return;
	}

	RepeatSchedule schedule;

	schedule.id = g_nextId++;

	switch (repeatMenu)
	{
	case 1:
		schedule.repeatType = RepeatType::DAILY;
		break;

	case 2:
		schedule.repeatType = RepeatType::WEEKLY;
		break;

	case 3:
		schedule.repeatType = RepeatType::MONTHLY;
		break;
	}

	cout << "\n입력 형식 : ";

	if (repeatMenu == 1)
	{
		cout << "HH:MM 제목\n";
	}
	else if (repeatMenu == 2)
	{
		cout << "월,수 13:00 운동\n";
	}
	else
	{
		cout << "1,15,20 09:00 카드값\n";
	}
	cout << "\n입력 : ";

	string input;
	getline(cin, input);

	if (!parseRepeatInput(input, schedule, repeatMenu)) {
		return;
	}

	cout << "\n시작일 (YYYY-MM-DD) : ";

	if (!parseDate(schedule.startYear, schedule.startMonth, schedule.startDay)) {
		cout << "\n[오류] 시작일 입력 오류\n";
		return;
	}

	cout << "종료일 (YYYY-MM-DD) : ";

	if (!parseDate(schedule.endYear, schedule.endMonth, schedule.endDay)) {
		cout << "\n[오류] 종료일 입력 오류\n";
		return;
	}

	if (isEarlierDate(schedule.endYear, schedule.endMonth, schedule.endDay,
		schedule.startYear, schedule.startMonth, schedule.startDay)) {
		cout << "\n[오류] 종료일은 시작일보다 빠를 수 없습니다.\n";
		return;
	}

	g_repeatScheduleList.push_back(schedule);

	saveSchedules();

	cout << "\n반복 일정이 등록되었습니다.\n\n";
}

void editRepeatSchedule() {

	if (g_repeatScheduleList.empty()) {

		cout << "\n등록된 반복 일정이 없습니다.\n\n";
		return;
	}

	int id;

	showRepeatSchedules();

	cout << "\n==== 반복 일정 수정 ====\n";
	cout << "수정할 ID : ";

	if (!(cin >> id)) {

		cout << "\n[오류] 숫자를 입력해주세요.\n";

		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		return;
	}

	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	for (size_t i = 0; i < g_repeatScheduleList.size(); i++) {

		if (g_repeatScheduleList[i].id != id) {
			continue;
		}

		const RepeatSchedule& current =
			g_repeatScheduleList[i];

		cout << "\n==== 현재 정보 ====\n\n";

		cout << "ID : "
			<< current.id
			<< '\n';

		cout << "제목 : ";

		if (current.title.empty()) {
			cout << "(제목 없음)\n";
		}
		else {
			cout << current.title << '\n';
		}

		cout << "시간 : "
			<< setw(2) << setfill('0')
			<< current.hour << ':'
			<< setw(2)
			<< current.minute
			<< setfill(' ')
			<< '\n';

		cout << "반복 정보 : "
			<< getRepeatInfo(current)
			<< '\n';

		cout << "기간 : "
			<< current.startYear << '-'
			<< setw(2) << setfill('0')
			<< current.startMonth << '-'
			<< setw(2)
			<< current.startDay
			<< " ~ "
			<< current.endYear << '-'
			<< setw(2)
			<< current.endMonth << '-'
			<< setw(2)
			<< current.endDay
			<< setfill(' ')
			<< "\n\n";

		int repeatMenu;

		cout << "1. 매일\n";
		cout << "2. 매주\n";
		cout << "3. 매월\n";
		cout << "0. 취소\n";

		cout << "\n선택 : ";

		if (!(cin >> repeatMenu)) {

			cin.clear();
			cin.ignore(
				numeric_limits<streamsize>::max(),
				'\n');

			cout << "\n[오류] 숫자를 입력해주세요.\n\n";
			return;
		}

		cin.ignore(
			numeric_limits<streamsize>::max(),
			'\n');

		if (repeatMenu == 0) {

			cout << "\n수정이 취소되었습니다.\n\n";
			return;
		}

		if (repeatMenu < 1 ||
			repeatMenu > 3) {

			cout << "\n[오류] 잘못된 메뉴입니다.\n\n";
			return;
		}

		RepeatSchedule newSchedule;

		if (!getRepeatEditInput(
			newSchedule,
			repeatMenu)) {
			return;
		}

		if (!getEditDateInput(
			newSchedule.startYear,
			newSchedule.startMonth,
			newSchedule.startDay,
			"\n시작일 (YYYY-MM-DD)")) {
			return;
		}

		if (!getEditDateInput(
			newSchedule.endYear,
			newSchedule.endMonth,
			newSchedule.endDay,
			"\n종료일 (YYYY-MM-DD)")) {
			return;
		}

		if (isEarlierDate(
			newSchedule.endYear,
			newSchedule.endMonth,
			newSchedule.endDay,
			newSchedule.startYear,
			newSchedule.startMonth,
			newSchedule.startDay)) {

			cout << "\n[오류] 종료일은 시작일보다 빠를 수 없습니다.\n\n";
			return;
		}

		newSchedule.id = current.id;

		switch (repeatMenu) {

		case 1:
			newSchedule.repeatType =
				RepeatType::DAILY;
			break;

		case 2:
			newSchedule.repeatType =
				RepeatType::WEEKLY;
			break;

		case 3:
			newSchedule.repeatType =
				RepeatType::MONTHLY;
			break;
		}

		g_repeatScheduleList[i] =
			newSchedule;

		saveSchedules();

		cout << "\n반복 일정이 수정되었습니다.\n\n";

		return;
	}

	cout << "\n[오류] 해당 ID를 찾을 수 없습니다.\n\n";
}

void deleteRepeatSchedule() {

	if (g_repeatScheduleList.empty()) {

		cout << "\n등록된 반복 일정이 없습니다.\n\n";
		return;
	}

	int id;

	showRepeatSchedules();

	cout << "\n==== 반복 일정 삭제 ====\n";
	cout << "삭제할 ID : ";

	if (!(cin >> id)) {

		cout << "\n[오류] 숫자를 입력해주세요.\n";

		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		return;
	}

	if (id <= 0) {

		cout << "\n[오류] 올바른 ID를 입력해주세요.\n\n";

		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return;
	}

	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	for (size_t i = 0; i < g_repeatScheduleList.size(); i++) {

		if (g_repeatScheduleList[i].id == id) {

			char confirm;

			cout << "\n==== 삭제 대상 반복 일정 ====\n\n";

			cout << "ID : "
				<< g_repeatScheduleList[i].id
				<< '\n';

			cout << "제목 : ";

			if (g_repeatScheduleList[i].title.empty()) {
				cout << "(제목 없음)\n";
			}
			else {
				cout << g_repeatScheduleList[i].title << '\n';
			}
			cout << "시간 : "
				<< setw(2) << setfill('0')
				<< g_repeatScheduleList[i].hour
				<< ':'
				<< setw(2)
				<< g_repeatScheduleList[i].minute
				<< setfill(' ')
				<< '\n';

			cout << "반복 정보 : "
				<< getRepeatInfo(
					g_repeatScheduleList[i])
				<< '\n';

			cout << "기간 : "
				<< g_repeatScheduleList[i].startYear << '-'
				<< setw(2) << setfill('0')
				<< g_repeatScheduleList[i].startMonth << '-'
				<< setw(2)
				<< g_repeatScheduleList[i].startDay
				<< " ~ "
				<< g_repeatScheduleList[i].endYear << '-'
				<< setw(2)
				<< g_repeatScheduleList[i].endMonth << '-'
				<< setw(2)
				<< g_repeatScheduleList[i].endDay
				<< setfill(' ')
				<< '\n';

			cout << "\n정말 삭제하시겠습니까? (Y/N) : ";

			cin >> confirm;
			cin.ignore(
				numeric_limits<streamsize>::max(),
				'\n');

			confirm = toupper(confirm);

			if (confirm != 'Y') {

				cout << "\n삭제가 취소되었습니다.\n\n";
				return;
			}

			g_repeatScheduleList.erase(
				g_repeatScheduleList.begin() + i);

			saveSchedules();

			cout << "\n반복 일정이 삭제되었습니다.\n\n";

			return;
		}
	}

	cout << "\n[오류] 해당 ID를 찾을 수 없습니다.\n\n";
}

void showTodaySchedules() {

	time_t now = time(nullptr);
	tm localTime;

	localtime_s(&localTime, &now);

	int currentYear = localTime.tm_year + 1900;
	int currentMonth = localTime.tm_mon + 1;
	int currentDay = localTime.tm_mday;

	vector<Schedule> todaySchedules;

	for (const Schedule& schedule : g_scheduleList) {

		if (schedule.year == currentYear &&
			schedule.month == currentMonth &&
			schedule.day == currentDay) {

			todaySchedules.push_back(schedule);
		}
	}

	addRepeatSchedulesForDate(
		todaySchedules,
		currentYear,
		currentMonth,
		currentDay);

	sort(
		todaySchedules.begin(),
		todaySchedules.end(),
		compareSchedule);

	cout << "\n==== 오늘 일정 ====\n\n";

	cout << "================================================\n";
	cout << "ID    날짜         시간    제목\n";
	cout << "================================================\n";

	if (todaySchedules.empty()) {
		cout << "오늘 등록된 일정이 없습니다.\n";
		cout << "================================================\n\n";
		return;
	}

	for (const Schedule& schedule : todaySchedules) {

		cout << left << setw(6) << schedule.id;

		printDateTime(schedule);

		if (schedule.title.empty()) {
			cout << "(제목 없음)\n";
		}
		else {
			cout << schedule.title << '\n';
		}
	}

	cout << "================================================\n";
	cout << "총 " << todaySchedules.size() << "건\n\n";
}

void showScheduleByDate() {

	string input;

	cout << "\n==== 날짜별 조회 ====\n";
	cout << "입력 (MM-DD 또는 YYYY-MM-DD) : ";

	getline(cin, input);

	int searchYear = 0;
	int searchMonth = 0;
	int searchDay = 0;

	int dashCount = 0;

	for (char ch : input) {
		if (ch == '-') {
			dashCount++;
		}
	}

	try {
		string token;

		if (dashCount == 1) {

			stringstream parser(input);

			getline(parser, token, '-');
			searchMonth = stoi(token);

			getline(parser, token);
			searchDay = stoi(token);

			if (searchMonth < Config::MIN_MONTH ||
				searchMonth > Config::MAX_MONTH) {

				cout << "\n[오류] 월 입력 오류 (01~12)\n";
				return;
			}

			if (searchDay < 1 ||
				searchDay > getDaysInMonth(
					Config::LEAP_YEAR_REFERENCE,
					searchMonth)) {

				cout << "\n[오류] 유효하지 않은 날짜입니다.\n";
				return;
			}
		}
		else if (dashCount == 2) {

			stringstream parser(input);

			getline(parser, token, '-');
			searchYear = stoi(token);

			getline(parser, token, '-');
			searchMonth = stoi(token);

			getline(parser, token);
			searchDay = stoi(token);

			if (searchYear < Config::MIN_YEAR ||
				searchYear > Config::MAX_YEAR) {

				cout << "\n[오류] 년도 입력 오류 (2026~2099)\n";
				return;
			}

			if (searchMonth < Config::MIN_MONTH ||
				searchMonth > Config::MAX_MONTH) {

				cout << "\n[오류] 월 입력 오류 (01~12)\n";
				return;
			}

			if (!isValidDate(
				searchYear,
				searchMonth,
				searchDay)) {

				cout << "\n[오류] 유효하지 않은 날짜입니다.\n";
				return;
			}
		}
		else {
			cout << "\n[오류] 입력 형식 오류\n";
			cout << "예시 : 06-10 또는 2026-06-10\n";
			return;
		}
	}
	catch (const exception&) {
		cout << "\n[오류] 입력 형식 오류\n";
		cout << "예시 : 06-10 또는 2026-06-10\n";
		return;
	}

	vector<Schedule> resultSchedules;

	if (dashCount == 2) {

		for (const Schedule& schedule : g_scheduleList) {

			if (schedule.year == searchYear &&
				schedule.month == searchMonth &&
				schedule.day == searchDay) {

				resultSchedules.push_back(schedule);
			}
		}

		addRepeatSchedulesForDate(
			resultSchedules,
			searchYear,
			searchMonth,
			searchDay);
	}
	else {

		for (int year = Config::MIN_YEAR;
			year <= Config::MAX_YEAR;
			year++) {

			for (const Schedule& schedule : g_scheduleList) {

				if (schedule.year == year &&
					schedule.month == searchMonth &&
					schedule.day == searchDay) {

					resultSchedules.push_back(schedule);
				}
			}

			addRepeatSchedulesForDate(
				resultSchedules,
				year,
				searchMonth,
				searchDay);
		}
	}

	sort(
		resultSchedules.begin(),
		resultSchedules.end(),
		compareSchedule);

	cout << "\n================================================\n";
	cout << "ID    날짜         시간    제목\n";
	cout << "================================================\n";

	if (resultSchedules.empty()) {
		cout << "해당 날짜의 일정이 없습니다.\n";
		cout << "================================================\n\n";
		return;
	}

	for (const Schedule& schedule : resultSchedules) {

		cout << left << setw(6) << schedule.id;

		printDateTime(schedule);

		if (schedule.title.empty()) {
			cout << "(제목 없음)\n";
		}
		else {
			cout << schedule.title << '\n';
		}
	}

	cout << "================================================\n";
	cout << "총 " << resultSchedules.size() << "건\n\n";
}

void showAllSchedules() {
	cout << "\n==== 전체 일정 조회 ====\n\n";

	if (g_scheduleList.empty() &&
		g_repeatScheduleList.empty()) {
		cout << "등록된 일정이 없습니다." << endl;
		return;
	}
	cout << "================================================================================================\n";
	cout << "ID    날짜         시간    제목                반복정보            기간\n";
	cout << "================================================================================================\n";

	for (const Schedule& schedule : g_scheduleList)
	{
		cout << left << setw(6) << schedule.id;
		printDateTime(schedule);

		if (schedule.title.empty())
		{
			cout << "(제목 없음)\n";
		}
		else
		{
			cout << schedule.title << "\n";
		}
	}
	for (const RepeatSchedule& schedule : g_repeatScheduleList) {

		int nextYear;
		int nextMonth;
		int nextDay;

		if (!getNextOccurrence(
			schedule,
			nextYear,
			nextMonth,
			nextDay)) {
			continue;
		}

		cout << left
			<< setw(6)
			<< schedule.id;

		cout << right
			<< nextYear << '-'
			<< setw(2) << setfill('0') << nextMonth << '-'
			<< setw(2) << nextDay
			<< setfill(' ')
			<< "   ";

		cout << setw(2)
			<< setfill('0')
			<< schedule.hour
			<< ':'
			<< setw(2)
			<< schedule.minute
			<< setfill(' ')
			<< "   ";

		if (schedule.title.empty()) {
			cout << setw(20)
				<< "(제목 없음)";
		}
		else {
			cout << left
				<< setw(20)
				<< schedule.title;
		}

		cout << setw(20)
			<< getRepeatInfo(schedule);

		cout
			<< right
			<< schedule.startYear << '-'
			<< setw(2) << setfill('0') << schedule.startMonth << '-'
			<< setw(2) << schedule.startDay
			<< " ~ "
			<< schedule.endYear << '-'
			<< setw(2) << schedule.endMonth << '-'
			<< setw(2) << schedule.endDay
			<< setfill(' ');

		cout << '\n';
	}

	cout << "================================================================================================\n";
	cout << "총 "
		<< g_scheduleList.size()
		+ g_repeatScheduleList.size()
		<< "건\n\n";
	return;
}

void showRepeatSchedules()
{
	if (g_repeatScheduleList.empty())
	{
		cout << "\n등록된 반복 일정이 없습니다.\n\n";
		return;
	}

	cout << "\n==== 반복 일정 목록 ====\n\n";

	cout << "===============================================================================\n";
	cout << "ID    시간    제목                반복정보            기간\n";
	cout << "===============================================================================\n";

	for (const RepeatSchedule& schedule : g_repeatScheduleList)
	{
		cout << left
			<< setw(6)
			<< schedule.id;

		cout << right
			<< setw(2) << setfill('0')
			<< schedule.hour
			<< ':'
			<< setw(2)
			<< schedule.minute
			<< setfill(' ')
			<< "   ";

		if (schedule.title.empty())
		{
			cout << left
				<< setw(20)
				<< "(제목 없음)";
		}
		else
		{
			cout << left
				<< setw(20)
				<< schedule.title;
		}

		cout << setw(20)
			<< getRepeatInfo(schedule);

		cout
			<< right
			<< schedule.startYear << '-'
			<< setw(2) << setfill('0')
			<< schedule.startMonth << '-'
			<< setw(2)
			<< schedule.startDay
			<< " ~ "
			<< schedule.endYear << '-'
			<< setw(2)
			<< schedule.endMonth << '-'
			<< setw(2)
			<< schedule.endDay
			<< setfill(' ');

		cout << '\n';
	}

	cout << "===============================================================================\n";
	cout << "총 "
		<< g_repeatScheduleList.size()
		<< "건\n\n";
}

void showMonthlyCalendar()
{
	time_t now = time(nullptr);
	tm localTime;

	localtime_s(&localTime, &now);

	int year = localTime.tm_year + 1900;
	int month = localTime.tm_mon + 1;

	while (true)
	{
		int firstDay = getDayOfWeek(year, month, 1);
		int lastDay = getDaysInMonth(year, month);

		cout << "\n";
		cout << year << "년 " << month << "월\n\n";

		cout << setw(Config::CALENDAR_CELL_WIDTH - 3) << "일"
			<< setw(Config::CALENDAR_CELL_WIDTH + 1) << "월"
			<< setw(Config::CALENDAR_CELL_WIDTH + 1) << "화"
			<< setw(Config::CALENDAR_CELL_WIDTH + 1) << "수"
			<< setw(Config::CALENDAR_CELL_WIDTH + 1) << "목"
			<< setw(Config::CALENDAR_CELL_WIDTH + 1) << "금"
			<< setw(Config::CALENDAR_CELL_WIDTH + 1) << "토"
			<< '\n';

		printHorizontalLine();

		int currentDay = 1;

		for (int week = 0; week < 6; week++)
		{
			int weekDays[7] = { 0 };

			for (int dow = 0; dow < 7; dow++)
			{
				if (week == 0 && dow < firstDay)
				{
					weekDays[dow] = 0;
				}
				else if (currentDay <= lastDay)
				{
					weekDays[dow] = currentDay++;
				}
			}

			// 날짜 줄
			for (int dow = 0; dow < 7; dow++)
			{
				cout << "|";

				if (weekDays[dow] == 0)
				{
					cout << setw(Config::CALENDAR_CELL_WIDTH) << "";
				}
				else
				{
					cout << right
						<< setw(Config::CALENDAR_CELL_WIDTH)
						<< weekDays[dow];
				}
			}
			cout << "|\n";

			// 일정 첫 줄
			for (int dow = 0; dow < 7; dow++)
			{
				cout << "|";

				if (weekDays[dow] == 0)
				{
					cout << setw(Config::CALENDAR_CELL_WIDTH) << "";
				}
				else
				{
					vector<string> lines =
						getScheduleLines(
							year,
							month,
							weekDays[dow]);

					cout << right
						<< setw(Config::CALENDAR_CELL_WIDTH)
						<< lines[0];
				}
			}
			cout << "|\n";

			// 일정 두 번째 줄
			for (int dow = 0; dow < 7; dow++)
			{
				cout << "|";

				if (weekDays[dow] == 0)
				{
					cout << setw(Config::CALENDAR_CELL_WIDTH) << "";
				}
				else
				{
					vector<string> lines =
						getScheduleLines(
							year,
							month,
							weekDays[dow]);

					cout << right
						<< setw(Config::CALENDAR_CELL_WIDTH)
						<< lines[1];
				}
			}
			cout << "|\n";

			printHorizontalLine();

			if (currentDay > lastDay)
			{
				break;
			}
		}

		cout << "\n";
		cout << "[입력]\n";
		cout << "< : 이전 달\n";
		cout << "> : 다음 달\n";
		cout << "YYYY-MM : 해당 월 이동\n";
		cout << "0 : 메뉴로\n";
		cout << "입력 : ";

		string input;
		getline(cin, input);

		if (input == "0")
		{
			return;
		}

		if (input == "<")
		{
			month--;

			if (month < 1)
			{
				month = 12;
				year--;
			}

			continue;
		}

		if (input == ">")
		{
			month++;

			if (month > 12)
			{
				month = 1;
				year++;
			}

			continue;
		}

		try
		{
			string token;
			stringstream parser(input);

			getline(parser, token, '-');
			year = stoi(token);

			getline(parser, token);
			month = stoi(token);

			if (year < Config::MIN_YEAR ||
				year > Config::MAX_YEAR)
			{
				cout << "\n[오류] 년도 입력 오류 (2026~2099)\n";
				continue;
			}

			if (month < Config::MIN_MONTH ||
				month > Config::MAX_MONTH)
			{
				cout << "\n[오류] 월 입력 오류 (01~12)\n";
				continue;
			}
		}
		catch (const exception&)
		{
			cout << "\n[오류] 입력 형식 오류\n";
			cout << "예시 : 2026-07\n";
		}
	}
}

void saveSchedules()
{
	ofstream file(Config::SAVE_FILE_NAME);

	if (!file)
	{
		cout << "[오류] 저장 파일을 열 수 없습니다.\n";
		return;
	}

	for (const Schedule& schedule : g_scheduleList)
	{
		file
			<< "S|"
			<< schedule.id << '|'
			<< schedule.year << '|'
			<< schedule.month << '|'
			<< schedule.day << '|'
			<< schedule.hour << '|'
			<< schedule.minute << '|'
			<< schedule.title
			<< '\n';
	}
	for (const RepeatSchedule& schedule : g_repeatScheduleList)
	{
		file
			<< "R|"
			<< schedule.id << '|'
			<< static_cast<int>(schedule.repeatType) << '|'
			<< schedule.repeatRule << '|'
			<< schedule.hour << '|'
			<< schedule.minute << '|'
			<< schedule.title << '|'
			<< schedule.startYear << '|'
			<< schedule.startMonth << '|'
			<< schedule.startDay << '|'
			<< schedule.endYear << '|'
			<< schedule.endMonth << '|'
			<< schedule.endDay
			<< '\n';
	}
}

void loadSchedules()
{
	ifstream file(Config::SAVE_FILE_NAME);

	if (!file)
	{
		return;
	}

	g_scheduleList.clear();
	g_repeatScheduleList.clear();

	string line;
	int maxId = 0;

	while (getline(file, line))
	{
		try
		{
			string token;
			stringstream parser(line);

			string type;
			getline(parser, type, '|');

			// 일반 일정
			if (type == "S")
			{
				Schedule schedule;

				getline(parser, token, '|');
				schedule.id = stoi(token);

				getline(parser, token, '|');
				schedule.year = stoi(token);

				getline(parser, token, '|');
				schedule.month = stoi(token);

				getline(parser, token, '|');
				schedule.day = stoi(token);

				getline(parser, token, '|');
				schedule.hour = stoi(token);

				getline(parser, token, '|');
				schedule.minute = stoi(token);

				getline(parser, schedule.title);

				if (!isValidDate(
					schedule.year,
					schedule.month,
					schedule.day))
				{
					throw runtime_error("Invalid date");
				}

				if (schedule.hour < Config::MIN_HOUR ||
					schedule.hour > Config::MAX_HOUR)
				{
					throw runtime_error("Invalid hour");
				}

				if (schedule.minute < Config::MIN_MINUTE ||
					schedule.minute > Config::MAX_MINUTE)
				{
					throw runtime_error("Invalid minute");
				}

				if (schedule.id <= 0)
				{
					throw runtime_error("Invalid id");
				}

				g_scheduleList.push_back(schedule);

				if (schedule.id > maxId)
				{
					maxId = schedule.id;
				}
			}

			// 반복 일정
			else if (type == "R")
			{
				RepeatSchedule schedule;

				getline(parser, token, '|');
				schedule.id = stoi(token);

				getline(parser, token, '|');
				schedule.repeatType =
					static_cast<RepeatType>(stoi(token));

				getline(parser,
					schedule.repeatRule,
					'|');

				getline(parser, token, '|');
				schedule.hour = stoi(token);

				getline(parser, token, '|');
				schedule.minute = stoi(token);

				getline(parser,
					schedule.title,
					'|');

				getline(parser, token, '|');
				schedule.startYear = stoi(token);

				getline(parser, token, '|');
				schedule.startMonth = stoi(token);

				getline(parser, token, '|');
				schedule.startDay = stoi(token);

				getline(parser, token, '|');
				schedule.endYear = stoi(token);

				getline(parser, token, '|');
				schedule.endMonth = stoi(token);

				getline(parser, token);
				schedule.endDay = stoi(token);

				if (schedule.id <= 0)
				{
					throw runtime_error("Invalid repeat id");
				}

				if (schedule.hour < Config::MIN_HOUR ||
					schedule.hour > Config::MAX_HOUR)
				{
					throw runtime_error("Invalid repeat hour");
				}

				if (schedule.minute < Config::MIN_MINUTE ||
					schedule.minute > Config::MAX_MINUTE)
				{
					throw runtime_error("Invalid repeat minute");
				}

				if (!isValidDate(
					schedule.startYear,
					schedule.startMonth,
					schedule.startDay))
				{
					throw runtime_error("Invalid repeat start date");
				}

				if (!isValidDate(
					schedule.endYear,
					schedule.endMonth,
					schedule.endDay))
				{
					throw runtime_error("Invalid repeat end date");
				}

				if (isEarlierDate(
					schedule.endYear,
					schedule.endMonth,
					schedule.endDay,
					schedule.startYear,
					schedule.startMonth,
					schedule.startDay))
				{
					throw runtime_error("Invalid repeat period");
				}

				int repeatType =
					static_cast<int>(schedule.repeatType);

				if (repeatType < 0 ||
					repeatType > 2)
				{
					throw runtime_error("Invalid repeat type");
				}

				g_repeatScheduleList.push_back(schedule);

				if (schedule.id > maxId)
				{
					maxId = schedule.id;
				}
			}
		}
		catch (const exception&)
		{
			cout << "[경고] 손상된 일정 데이터 발견 - 건너뜀\n";
			continue;
		}
	}

	g_nextId = maxId + 1;

	sortSchedules();
}

bool parseScheduleInput(const string& input, Schedule& schedule) {
	try
	{
		string token;
		string datePart;
		string timePart;

		stringstream inputParser(input);

		if (!(inputParser >> datePart >> timePart))
		{
			cout << "\n[오류] 입력 형식 오류\n";
			cout << "예시 : YYYY-MM-DD HH:MM 제목\n";
			return false;
		}

		stringstream dateParser(datePart);
		stringstream timeParser(timePart);

		getline(dateParser, token, '-');
		schedule.year = stoi(token);

		if (schedule.year < Config::MIN_YEAR ||
			schedule.year > Config::MAX_YEAR)
		{
			cout << "\n[오류] 년도 입력 오류 (2026~2099)\n";
			return false;
		}

		getline(dateParser, token, '-');
		schedule.month = stoi(token);

		if (schedule.month < Config::MIN_MONTH ||
			schedule.month > Config::MAX_MONTH)
		{
			cout << "\n[오류] 월 입력 오류 (01~12)\n";
			return false;
		}

		getline(dateParser, token, '-');
		schedule.day = stoi(token);

		if (!isValidDate(
			schedule.year,
			schedule.month,
			schedule.day))
		{
			cout << "\n[오류] 유효하지 않은 날짜입니다.\n";
			return false;
		}

		getline(timeParser, token, ':');
		schedule.hour = stoi(token);

		if (schedule.hour < Config::MIN_HOUR ||
			schedule.hour > Config::MAX_HOUR)
		{
			cout << "\n[오류] 시간 입력 오류 (00~23)\n";
			return false;
		}

		getline(timeParser, token, ':');
		schedule.minute = stoi(token);

		if (schedule.minute < Config::MIN_MINUTE ||
			schedule.minute > Config::MAX_MINUTE)
		{
			cout << "\n[오류] 분 입력 오류 (00~59)\n";
			return false;
		}

		inputParser >> ws;
		getline(inputParser, schedule.title);

		return true;
	}
	catch (const exception&)
	{
		cout << "\n[오류] 입력 값이 올바르지 않습니다.\n";
		cout << "예시 : 2026-06-01 14:30 회의\n";
		return false;
	}
}

bool parseRepeatInput(const string& input, RepeatSchedule& schedule, int repeatMenu) {
	try
	{
		stringstream parser(input);

		string token;
		string timePart;

		if (repeatMenu == 1)
		{
			// 매일
			if (!(parser >> timePart))
			{
				cout << "\n[오류] 입력 형식 오류\n";
				cout << "예시 : 08:00 약 먹기\n\n";
				return false;
			}

			getline(parser >> ws, schedule.title);

			stringstream timeParser(timePart);

			getline(timeParser, token, ':');
			schedule.hour = stoi(token);

			getline(timeParser, token);
			schedule.minute = stoi(token);

			if (schedule.hour < Config::MIN_HOUR ||
				schedule.hour > Config::MAX_HOUR)
			{
				cout << "\n[오류] 시간 입력 오류 (00~23)\n\n";
				return false;
			}

			if (schedule.minute < Config::MIN_MINUTE ||
				schedule.minute > Config::MAX_MINUTE)
			{
				cout << "\n[오류] 분 입력 오류 (00~59)\n\n";
				return false;
			}

			schedule.repeatRule = "";
		}
		else
		{
			if (!(parser >> schedule.repeatRule >> timePart))
			{
				cout << "\n[오류] 입력 형식 오류\n";

				if (repeatMenu == 2)
				{
					cout << "예시 : 월,수 13:00 운동\n\n";
				}
				else
				{
					cout << "예시 : 1,15,20 09:00 카드값\n\n";
				}

				return false;
			}

			getline(parser >> ws, schedule.title);

			stringstream timeParser(timePart);

			getline(timeParser, token, ':');
			schedule.hour = stoi(token);

			getline(timeParser, token);
			schedule.minute = stoi(token);

			if (schedule.hour < Config::MIN_HOUR ||
				schedule.hour > Config::MAX_HOUR)
			{
				cout << "\n[오류] 시간 입력 오류 (00~23)\n\n";
				return false;
			}

			if (schedule.minute < Config::MIN_MINUTE ||
				schedule.minute > Config::MAX_MINUTE)
			{
				cout << "\n[오류] 분 입력 오류 (00~59)\n\n";
				return false;
			}

			// 매주 검증
			if (repeatMenu == 2)
			{
				vector<string> usedDays;

				string dayToken;
				stringstream dayParser(schedule.repeatRule);

				while (getline(dayParser, dayToken, ','))
				{
					if (dayToken != "월" &&
						dayToken != "화" &&
						dayToken != "수" &&
						dayToken != "목" &&
						dayToken != "금" &&
						dayToken != "토" &&
						dayToken != "일")
					{
						cout << "\n[오류] 반복 요일 입력 오류\n";
						cout << "예시 : 월,수 13:00 운동\n\n";
						return false;
					}

					for (const string& usedDay : usedDays)
					{
						if (usedDay == dayToken)
						{
							cout << "\n[오류] 중복된 요일이 있습니다.\n\n";
							return false;
						}
					}

					usedDays.push_back(dayToken);
				}
			}

			// 매월 검증
			if (repeatMenu == 3)
			{
				vector<int> usedDays;

				string dayToken;
				stringstream dayParser(schedule.repeatRule);

				while (getline(dayParser, dayToken, ','))
				{
					int day = stoi(dayToken);

					if (day < 1 || day > 31)
					{
						cout << "\n[오류] 반복 날짜 입력 오류 (1~31)\n\n";
						return false;
					}

					for (int usedDay : usedDays)
					{
						if (usedDay == day)
						{
							cout << "\n[오류] 중복된 날짜가 있습니다.\n\n";
							return false;
						}
					}

					usedDays.push_back(day);
				}
			}
		}

		return true;
	}
	catch (const exception&)
	{
		cout << "\n[오류] 입력 형식 오류\n";

		if (repeatMenu == 1)
		{
			cout << "예시 : 08:00 약 먹기\n\n";
		}
		else if (repeatMenu == 2)
		{
			cout << "예시 : 월,수 13:00 운동\n\n";
		}
		else
		{
			cout << "예시 : 1,15,20 09:00 카드값\n\n";
		}

		return false;
	}
}

bool parseDate(int& year, int& month, int& day) {
	string input;

	getline(cin, input);

	try
	{
		string token;
		stringstream parser(input);

		getline(parser, token, '-');
		year = stoi(token);

		getline(parser, token, '-');
		month = stoi(token);

		getline(parser, token);
		day = stoi(token);

		if (year < Config::MIN_YEAR ||
			year > Config::MAX_YEAR)
		{
			return false;
		}

		if (month < Config::MIN_MONTH ||
			month > Config::MAX_MONTH) {
			return false;
		}

		if (!isValidDate(year, month, day))
		{
			return false;
		}

		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool getEditDateInput(int& year, int& month, int& day, const string& message) {

	string input;

	cout << message;
	cout << " (취소 : 0)\n";
	cout << "입력 : ";

	getline(cin, input);

	if (input == "0") {
		cout << "\n수정이 취소되었습니다.\n\n";
		return false;
	}

	try {

		string token;

		stringstream parser(input);

		getline(parser, token, '-');
		year = stoi(token);

		getline(parser, token, '-');
		month = stoi(token);

		getline(parser, token);
		day = stoi(token);

		if (year < Config::MIN_YEAR ||
			year > Config::MAX_YEAR)
		{
			cout << "\n[오류] 년도 입력 오류 (2026~2099)\n\n";
			return false;
		}

		if (month < Config::MIN_MONTH ||
			month > Config::MAX_MONTH)
		{
			cout << "\n[오류] 월 입력 오류 (01~12)\n\n";
			return false;
		}

		if (!isValidDate(year, month, day))
		{
			cout << "\n[오류] 유효하지 않은 날짜입니다.\n\n";
			return false;
		}

		return true;
	}
	catch (...)
	{
		cout << "\n[오류] 날짜 입력 형식 오류\n";
		cout << "예시 : 2026-06-01\n\n";
		return false;
	}
}

bool getRepeatEditInput(RepeatSchedule& schedule, int repeatMenu) {

	string input;

	if (repeatMenu == 1) {
		cout << "\n입력 (예 : 08:00 약 먹기)\n";
	}
	else if (repeatMenu == 2) {
		cout << "\n입력 (예 : 월,수 13:00 운동)\n";
	}
	else {
		cout << "\n입력 (예 : 1,15,20 09:00 카드값)\n";
	}

	cout << "(취소 : 0)\n";
	cout << "입력 : ";

	getline(cin, input);

	if (input == "0") {
		cout << "\n수정이 취소되었습니다.\n\n";
		return false;
	}

	return parseRepeatInput(
		input,
		schedule,
		repeatMenu);
}

bool isLeapYear(int year) {
	if (year % 400 == 0)
		return true;
	if (year % 100 == 0)
		return false;
	return year % 4 == 0;
}

int getDaysInMonth(int year, int month) {

	switch (month) {
	case 2: return isLeapYear(year) ? 29 : 28;

	case 4:
	case 6:
	case 9:
	case 11: return 30;

	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12: return 31;

	default:
		return 0;
	}
}

bool isValidDate(int year, int month, int day) {
	return day > 0 && day <= getDaysInMonth(year, month);
}

bool isEarlierDate(int year1, int month1, int day1, int year2, int month2, int day2) {
	if (year1 != year2)
	{
		return year1 < year2;
	}

	if (month1 != month2)
	{
		return month1 < month2;
	}

	return day1 < day2;
}

bool isDateInRange(int year, int month, int day, const RepeatSchedule& schedule) {
	if (isEarlierDate(
		year,
		month,
		day,
		schedule.startYear,
		schedule.startMonth,
		schedule.startDay))
	{
		return false;
	}

	if (isEarlierDate(
		schedule.endYear,
		schedule.endMonth,
		schedule.endDay,
		year,
		month,
		day))
	{
		return false;
	}

	return true;
}

bool isRepeatMatched(const RepeatSchedule& schedule, int year, int month, int day) {

	if (!isDateInRange(
		year,
		month,
		day,
		schedule)) {
		return false;
	}

	switch (schedule.repeatType) {

	case RepeatType::DAILY:
		return true;

	case RepeatType::WEEKLY:
	{
		int dayOfWeek =
			getDayOfWeek(year, month, day);

		string currentDay;

		switch (dayOfWeek) {
		case 0: currentDay = "일"; break;
		case 1: currentDay = "월"; break;
		case 2: currentDay = "화"; break;
		case 3: currentDay = "수"; break;
		case 4: currentDay = "목"; break;
		case 5: currentDay = "금"; break;
		case 6: currentDay = "토"; break;
		default:
			return false;
		}

		string token;
		stringstream parser(schedule.repeatRule);

		while (getline(parser, token, ',')) {
			if (token == currentDay) {
				return true;
			}
		}

		return false;
	}

	case RepeatType::MONTHLY:
	{
		string token;
		stringstream parser(schedule.repeatRule);

		while (getline(parser, token, ',')) {
			if (stoi(token) == day) {
				return true;
			}
		}

		return false;
	}

	default:
		return false;
	}
}

bool getNextOccurrence(const RepeatSchedule& schedule, int& year, int& month, int& day) {
	time_t now = time(nullptr);

	tm localTime;

	localtime_s(
		&localTime,
		&now);

	for (int i = 0; i < Config::MAX_REPEAT_SEARCH_DAYS; i++)
	{
		int currentYear =
			localTime.tm_year + 1900;

		int currentMonth =
			localTime.tm_mon + 1;

		int currentDay =
			localTime.tm_mday;

		if (isRepeatMatched(
			schedule,
			currentYear,
			currentMonth,
			currentDay))
		{
			year = currentYear;
			month = currentMonth;
			day = currentDay;

			return true;
		}

		localTime.tm_mday++;

		mktime(&localTime);
	}

	return false;
}

string getRepeatInfo(const RepeatSchedule& schedule) {

	switch (schedule.repeatType) {

	case RepeatType::DAILY:
		return "[매일 반복]";

	case RepeatType::WEEKLY:
		return "[" +
			schedule.repeatRule +
			" 반복]";

	case RepeatType::MONTHLY:
		return "[" +
			schedule.repeatRule +
			" 반복]";

	default:
		return "[반복]";
	}
}

void addRepeatSchedulesForDate(vector<Schedule>& schedules, int year, int month, int day) {

	for (const RepeatSchedule& repeatSchedule : g_repeatScheduleList) {

		if (!isRepeatMatched(
			repeatSchedule,
			year,
			month,
			day)) {
			continue;
		}

		Schedule schedule;

		schedule.id = repeatSchedule.id;

		schedule.year = year;
		schedule.month = month;
		schedule.day = day;

		schedule.hour = repeatSchedule.hour;
		schedule.minute = repeatSchedule.minute;

		schedule.title = repeatSchedule.title + " [반복]";

		schedules.push_back(schedule);
	}
}

bool compareSchedule(const Schedule& a, const Schedule& b)
{
	if (a.year != b.year)
		return a.year < b.year;

	if (a.month != b.month)
		return a.month < b.month;

	if (a.day != b.day)
		return a.day < b.day;

	if (a.hour != b.hour)
		return a.hour < b.hour;

	if (a.minute != b.minute)
		return a.minute < b.minute;

	return a.id < b.id;
}

void sortSchedules()
{
	sort(
		g_scheduleList.begin(),
		g_scheduleList.end(),
		compareSchedule
	);
}

int getDayOfWeek(int year, int month, int day)
{
	if (month < 3)
	{
		month += 12;
		year--;
	}

	int k = year % 100;
	int j = year / 100;

	int h =
		(day +
			(13 * (month + 1)) / 5 +
			k +
			(k / 4) +
			(j / 4) +
			(5 * j)) % 7;

	return (h + 6) % 7;
}

void printHorizontalLine()
{
	for (int i = 0; i < 7; i++)
	{
		cout << '+'
			<< string(Config::CALENDAR_CELL_WIDTH, '-');
	}
	cout << "+\n";
}

string getSchedulePreview(const string& title)
{
	if (title.empty())
	{
		return "(제목없음)";
	}

	if (title.length() <= Config::TITLE_PREVIEW_LENGTH)
	{
		return title;
	}

	return title.substr(0, Config::TITLE_PREVIEW_LENGTH) + "...";
}

void printDateTime(const Schedule& schedule)
{
	cout << right;
	cout << schedule.year << '-'
		<< setw(2) << setfill('0') << schedule.month << '-'
		<< setw(2) << setfill('0') << schedule.day << "   "
		<< setw(2) << setfill('0') << schedule.hour << ':'
		<< setw(2) << setfill('0') << schedule.minute << "   "
		<< setfill(' ');
}

vector<string> getScheduleLines(int year, int month, int day) {

	vector<string> result;
	vector<string> schedules;

	for (const Schedule& schedule : g_scheduleList) {

		if (schedule.year == year &&
			schedule.month == month &&
			schedule.day == day) {

			schedules.push_back(
				getSchedulePreview(schedule.title));
		}
	}

	for (const RepeatSchedule& repeatSchedule : g_repeatScheduleList) {

		if (isRepeatMatched(
			repeatSchedule,
			year,
			month,
			day)) {

			schedules.push_back(
				getSchedulePreview(
					repeatSchedule.title));
		}
	}

	if (schedules.empty()) {

		result.push_back("");
		result.push_back("");
	}
	else if (schedules.size() == 1) {

		result.push_back(schedules[0]);
		result.push_back("");
	}
	else if (schedules.size() == 2) {

		result.push_back(schedules[0]);
		result.push_back(schedules[1]);
	}
	else {

		result.push_back(schedules[0]);

		result.push_back(
			"외 " +
			to_string(schedules.size() - 1) +
			"건");
	}

	return result;
}

