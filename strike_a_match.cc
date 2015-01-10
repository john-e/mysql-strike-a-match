#include<iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mutex>
#include <vector>

#include <mysql/mysql.h>
#include <ctype.h>

using namespace std;

typedef unsigned long long ulonglong;
typedef long long longlong;

double strike_a_match_check(std::wstring s1, std::wstring s2);

extern "C" {
	double strike_a_match(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
	my_bool strike_a_match_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
	void strike_a_match_deinit(UDF_INIT *initid);
}

mutex locale_mx;

my_bool init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
	lock_guard<mutex> guard(locale_mx);
	if (!strcmp("C", setlocale(LC_ALL, 0)) && !setlocale(LC_ALL, "en_US.UTF-8")) {
		strcpy(message, "Can't change default locale to UTF-8");
		return 1;
	}
	initid->maybe_null = 0;
	if (args->arg_count != 2 || args->arg_type[0] != STRING_RESULT || args->arg_type[1] != STRING_RESULT) {
		strcpy(message, "This function requires two string arguments");
		return 1;
	}
	if(!args->args[0] || !args->args[1]) {
		strcpy(message, "This function requires non-NULL arguments");
		return 1;
	}
	return 0;
}

wstring from_cstr(const char* s, size_t l) {
	wstring ws(l, L' ');
	mbstowcs(&ws[0], s, l);
	return ws;
}

double strike_a_match(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
	wstring s1 = from_cstr(args->args[0], args->lengths[0]);
	wstring s2 = from_cstr(args->args[1], args->lengths[1]);
	return strike_a_match_check(s1.c_str(), s2.c_str());
}

my_bool strike_a_match_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
	return init(initid, args, message);
}

void strike_a_match_deinit(UDF_INIT *initid) {}

std::vector<std::wstring> explode(const std::wstring& str, const std::wstring& delim) {
	std::vector<std::wstring> ret;
	int delimSize = delim.size();
	if(str.size()>0) {
		size_t pos = 0;
		size_t found;
		while ((found = str.find(delim, pos)) != std::wstring::npos) {
			ret.push_back(str.substr(pos, found - pos));
			pos = found + delimSize;
		}
		ret.push_back(str.substr(pos));
	}
	return ret;
}

int main(int argc, const char* argv[]) {
	cout << "test" << strike_a_match_check(L"Healed",L"Sealed") << endl;
	return 0;
}

std::vector<std::wstring> letterPairs(const std::wstring& str) {
	std::vector<std::wstring> ret;
	if(str.size()>0) {
		int numPairs = str.length()-1;
		for (int i=0;i<numPairs;i++){
			ret.push_back(str.substr(i,2));
		}
	}
	return ret;
}

std::vector<std::wstring> wordLetterPairs(const std::wstring& str) {
	std::vector<std::wstring> ret;
	if(str.size()>0) {
		std::vector<std::wstring> words;
		words = explode(str,L" ");
		for (const std::wstring& w : words){
			std::vector<std::wstring> pairs = letterPairs(w);
			for (const std::wstring& p : pairs){
				ret.push_back(p);
			}
		}
	}
	return ret;
}

double strike_a_match_check(std::wstring s1, std::wstring s2){
	std::vector<std::wstring> pairs1,pairs2;

	std::wcout.imbue(std::locale());
	const std::ctype<wchar_t>& f = std::use_facet< std::ctype<wchar_t> >(std::locale());

	f.toupper(&s1[0], &s1[0] + s1.size());
	f.toupper(&s2[0], &s2[0] + s2.size());

	pairs1 = wordLetterPairs(s1);
	pairs2 = wordLetterPairs(s2);

	int intersection = 0;
	int total = pairs1.size() + pairs2.size();

	for (const std::wstring& w1 : pairs1){
		for (const std::wstring& w2 : pairs2){
			if(w1.compare(w2) == 0){
				intersection++;
				break;
			}
		}
	}
	return (2.0*intersection)/total;
}
