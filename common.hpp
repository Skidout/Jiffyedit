#include <iostream>
#include <string>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <sstream> 

using namespace std;

string metaline, temps, path, cmd, metaline2;
char tempc;
long long int pos = -1;
long long int temp;
float tempf, countf, secdur /* total length of the video in seconds */;
bool tempb;
unsigned long long int count2, count3, count5;

string replace(string sprstr, string substr, string repstr) {
	pos = -1;
	pos = sprstr.find(substr);
	if (pos < 0) {
		cout << "Fatal error: Substring to replace not found. Please report, send video and command if you can." << endl;
		exit(2);
	}
	int end = size(substr);
	sprstr.erase(pos, size(substr));
	sprstr.insert(pos, repstr);
	return sprstr;
}

bool isnum() {
	char comp = metaline[temp]; // character to compare
	if (comp == '0') {return true;}
	else if (comp == '1') {return true;}
	else if (comp == '2') {return true;}
	else if (comp == '3') {return true;}
	else if (comp == '4') {return true;}
	else if (comp == '5') {return true;}
	else if (comp == '6') {return true;}
	else if (comp == '7') {return true;}
	else if (comp == '8') {return true;}
	else if (comp == '9') {return true;}
	else {return false;}
}