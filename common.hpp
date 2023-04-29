#include <iostream>
#include <string>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <sstream> 
#include <iomanip>
#include <thread>

using namespace std;

string metaline, s1, s2, path, cmd, metaline2;
char c1;
long long int pos = -1;
long long int temp;
float f2, f1, secdur /* total length of the video in seconds */;
bool b1;
unsigned long long int i1, i2, i3;

void pcheck(FILE * &f, bool &pexit) { // check if child process exited
	if (f != NULL) { // if (FILE * ) makes the thread wait for child process to finish
		pexit = true; // pexit = true after child process has exited
	}
}

string replace(string sprstr, string substr, string repstr) {
	int pos = -1;
	pos = sprstr.find(substr);
	if (pos < 0) {
		cout << "Fatal error: Substring to replace not found. Please report, send video and command if you can. (" << substr << ")"<< endl;
		exit(2);
	}
	sprstr.erase(pos, size(substr));
	sprstr.insert(pos, repstr);
	return sprstr;
}

bool isnum(char comp) {
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

string ftog(float f) { // float to guint (or something)
	string dotstr = ".", s; // pitivi wants these in some weird format. guint or something i think. this is the only way i can think of to convert
	stringstream getsrt2;
	getsrt2 << fixed << setprecision(9) << f;
	getline(getsrt2, s);
	pos = s.find(dotstr);
	s.erase(pos, dotstr.size());
	return s;
}