#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <stdio.h>
#include <iomanip>
#include <cstring>
#include <vector>

using namespace std;

string bfflt, in, temps, metaline, path, cmd, totlen, metaline2, metaline3;
float tempf, countf, buffx2, secdur; // total length of the video in seconds
bool fdur = false;
char tempc;
long long int len, temp, count2, count3, count5;
long long int pos = -1;
bool hasjob = false; // stops execution in case the user has entered a command with nothing to do
float buff = 0.3;
float mclip = 0.5;
vector <float> silarr;
vector <float> tempsil;
int decbels = 20;

void primary();
void sorter();

string sprstr;
string substr;
string repstr;

string replace() {
	pos = -1;
	pos = sprstr.find(substr);
	if (pos < 0) {
		cout << "Fatal error: Substring to replace not found. Please report, send video and command if you can.";
		exit(2);
	}
	temp = size(substr);
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

void reset() {primary();} // can reset the program when needed.

void reader() {

	/// inmeta is for getting the silences
	
	bool tempb = false;
	// pipes in c++ seem to be horribly documented, so i'll do my best to explain it here. if i say i think something, it means im not entirely sure what exactly it does, so this is just my best guess from what little legible code and documentation i have been able to find
	
	FILE * inmeta = NULL; // so this here is creating a file pointer which will later become a datastream. according to cppreference.com (or somewhere similar), this type of stream is not meant to be accessed by any functions outside of the <stdio.h> library. Using functions from fstream would probably work, but i'm following their advice, just to be safe.
	
	char buf[1024]; // this is just a char array because its safer with C functions, i guess? strings would probably work, but again, better to be safe.
	
	char * getcmd = new char[cmd.length() + 1]; // the string needs to be converted to a const char * for the pipe funtion. this is done here (make sure to delete getcmd or realcmd later)
	strcpy(getcmd, cmd.c_str());
	
	const char * realcmd = getcmd;
	
	inmeta = popen(realcmd, "r"); // this tries to open a pipe (datastream) with ffmpeg for silencedetect. the first variable inside is literally a command, just like one you would run in a terminal or command line. the second variable, "r", indicates that we want to read data from this datastream.
	
	if (inmeta != NULL) {
	
	
		
		while (fgets(buf, 1024, inmeta) != NULL) { // this loop sets buf = the current line of the datastream. fgets is for plaintext data only. the 1024 specifies to read a max of 1024 bytes, so as not to overflow (at least i think so anyway). lastly, the datastream to read from is specified. the != null makes it so that when the datastream reaches its end, the loop ends, so as to prevent overflow
			metaline = buf; /// so for some reason i cant seem to add lines piped in from this specific command to an array of strings, which was originally how i was planning on doing it, so if you see any references to a string raw[], now you know why
			
			metaline2 = buf;
			tempc = metaline2[size(metaline2) - 1];
			tempb = false;
			if (tempc == '0') {tempb = true;}
			else if (tempc == '1') {tempb = true;}
			else if (tempc == '2') {tempb = true;}
			else if (tempc == '3') {tempb = true;}
			else if (tempc == '4') {tempb = true;}
			else if (tempc == '5') {tempb = true;}
			else if (tempc == '6') {tempb = true;}
			else if (tempc == '7') {tempb = true;}
			else if (tempc == '8') {tempb = true;}
			else if (tempc == '9') {tempb = true;}
			
			if (tempb == false) {
				metaline2.erase(size(metaline2) - 1, size(metaline2)); /// erase newline character at the end of the string, if there is one
			}
			
		
			pos = -1;
			pos = metaline2.find("silence_start: "); /// finds the start of the silence
		
			if (pos > 0) {
				pos = pos + 15;
				metaline2.erase(0, pos); // isolate the float value by erasing the rest of the string.
				stringstream stoof(metaline2);
			
				if (stoof >> tempf) {
					silarr.push_back(tempf);
				}
				
			}
			pos = -1;
			pos = metaline2.find("silence_end: "); // finds the ending float of the silence_detect
			
			if (pos > 0) {
				pos = pos + 13;
				metaline2.erase(0, pos); 
				pos = 0;
				while (tempc != ' ' and pos < size(metaline2)) {
					pos++;
					tempc = metaline2[pos];
				}
				stringstream stoof2(metaline2);
			
				if (stoof2 >> tempf) {
					silarr.push_back(tempf);
				}
				
				metaline3 = metaline2;
				pos = metaline3.find("silence_duration: ");
				pos = pos + 18;
				metaline3.erase(0, pos);
				stringstream stoof3(metaline3);
			
				if (stoof3 >> tempf) {
					silarr.push_back(tempf);
				}
			}
		}
		pclose(inmeta); // closes the pipe after the end of the datastream
		inmeta = NULL; // clears the datastream from memory (i think)
			
	}
	
	if (silarr.size() == 0) {
		cout << "Fatal error: No clips found in video." << endl;
		exit(4);
	}
	
	string cmd2 = "ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 \"PATH\"";
	
	sprstr = cmd2; substr = "PATH"; repstr = path; cmd2 = replace();
	
	char * getcmd2 = new char[cmd2.length() + 1]; // the string needs to be converted to a const char * for the pipe funtion. this is done here
	strcpy(getcmd2, cmd2.c_str());
	
	const char * realcmd2 = getcmd2;
	
	FILE * indur = NULL;
	
	if ((indur = popen(realcmd2, "r")) != NULL) { // this one here fetches one line
		
		while (fgets(buf, 1024, indur) != NULL) {
			totlen = buf;
		}
		pclose(indur);
		indur = NULL;
		fdur = true;
	}
	
	stringstream getlen(totlen);
	
	getlen >> secdur;
	
	delete [] realcmd2;
	delete [] realcmd;
	
	sorter();
}

void primary();

bool args = false;

int main(int argc, char * argv[]) {

	path = argv[1];
	if (argc > 1) {
	args = true;
		for (count2 = 1; count2 < argc; count2++) {
			if (argv[count2] == NULL) {
				cout << endl << "Fatal error: Invalid arguments passed." << endl;
				exit (4);
			}
			temps = argv[count2];
			in.append(temps);
			in.append(" ");
		}
	}
	primary();
}

void primary() {

	
	pos = -1;
	pos = in.find("bf");
	if (pos >= 0) {
		pos = pos + 2;
		tempc = in[pos];
		bfflt = in;
		bfflt.erase(0, pos);
		pos = 0;
		while (tempc != ' ' and pos < size(in)) {
			pos++;
			tempc = bfflt[pos];
		}
		bfflt.erase(pos, size(bfflt));
		stringstream sttof(bfflt);
		if (sttof >> buff) {
		}
	}
				
	pos = -1;
	pos = in.find("db");
	if (pos >= 0) {
		pos = pos + 2;
		tempc = in[pos];
		bfflt = in;
		bfflt.erase(0, pos);
		pos = 0;
		while (tempc != ' ' and pos < size(in)) {
			pos++;
			tempc = bfflt[pos];
		}
		bfflt.erase(pos, size(bfflt));
		stringstream sttof(bfflt);
		if (sttof >> decbels) {
		}
	}
				
	pos = -1;
	pos = in.find("mt");
	if (pos >= 0) {
		pos = pos + 2;
		tempc = in[pos];
		bfflt = in;
		bfflt.erase(0, pos);
		pos = 0;
		while (tempc != ' ' and pos < size(in)) {
			pos++;
			tempc = bfflt[pos];
		}
		bfflt.erase(pos, size(bfflt));
		stringstream sttof(bfflt);
		if (sttof >> mclip) {
		}
	}
	
	if (decbels < 0) {
		decbels = decbels * -1;
	}
	
	cmd = "ffmpeg -i \"";
	
	cmd.append(path);
	cmd.append("\" -hide_banner -af silencedetect=noise=-");
	stringstream wrtcmd;

	wrtcmd << cmd << decbels << "dB:d=0.03 -f null - 2>&1"; // not sure if the d= option actually does anything here, but it is supposed to be the precision with which it searches for silence. Since it seems to work fine like this i'll leave it for now.
	cmd = wrtcmd.str();

	// system("ffmpeg -i audiotest.mp4 -hide_banner -af silencedetect=noise=-15dB:d=0.03 -f null - > output.txt 2>&1"); // ffmpeg command must be formed with string, this is for test purposes only (change it later)
	reader();
	
}

void sorter() {
	
	
	if (fdur == false) {
		cout << "Fatal error: Duration not found.";
		exit(3);
	}
	
	count5 = 0;
	buffx2 = buff * 2.0;
	
	for (count2 = 0; count2 < silarr.size(); count2 = count2 + 3) { // filter the silences by their duration
		temp = count2 + 2; // temp is the duration of the silence
		if (silarr.at(temp) > buffx2) { // if silence duration is enough, save it
			temp = count2 + 1; // temp is now the silence end
			tempsil.push_back(silarr.at(count2)); count5++; // save the silence start to the list
			tempsil.push_back(silarr.at(temp)); count5++; // save the silence end
		}
	}
	
	
	count5 = 0;
	count3 = 2;

	if (tempsil.at(0) == 0.0) { // if the first item here is 0, then the next needs to be the start of the first chain because 0 cannot be the end of a chain
		silarr.at(0) = tempsil.at(1);
	}
	else {
		silarr.at(0) = 0.0; // if it isnt 0, the start of the first chain needs to be 0
	}
	
	for (count2 = 1; count3 < tempsil.size(); count2++) { // filter everything else
		silarr.at(count2) = tempsil.at(count3); count5++;
		count3++;
	}
	
	if (silarr.at(count5) != secdur) { // if the last silence end is less than the dur, make the dur the end of the final clip. if not, the silence end is junk and wont be used.
		silarr.at(count2) = secdur; count5++;
	}
	
	for (count2 = silarr.size() + 1; count2 > count5 + 2; count2--) { // loop to remove excess data from silarr
		silarr.pop_back();
	}
	
	
	count5 = 0;

	for (count2 = 0; count2 < silarr.size(); count2 = count2 + 2) { // sort the clips by the their duration
		temp = count2 + 1; 
		
		tempf = silarr.at(temp) - silarr.at(count2); // find the clip dur
		if (tempf > mclip) {
			tempsil.at(count5) = silarr.at(count2); count5++;
			tempsil.at(count5) = silarr.at(temp); count5++;
		}
	}
	
	for (count2 = tempsil.size(); count2 > count5; count2--) { // loop to remove excess data from tempsil
		tempsil.pop_back();
	}
	

	count5 = 0;

	if (tempsil.at(0) < buff) { // beginning thing for adding the buffer
		silarr.at(0) = 0.0; count5++;
		silarr.at(1) = tempsil.at(1) + buff; count5++;
	}

	for (count2 = count5; count2 < tempsil.size(); count2 = count2 + 2) { // add the buffer to all the rest
		temp = count2 + 1;
		silarr.at(count5) = tempsil.at(count2) - buff; count5++;
		silarr.at(count5) = tempsil.at(temp) + buff; count5++;
	}
	
	temp = count5 - 1;
	
	if (secdur - silarr.at(temp) < 0.0) {
		silarr.at(temp) = secdur;
	}
	
	for (count2 = silarr.size(); count2 > count5; count2--) {
		silarr.pop_back();
	}
	
	
	bool isbegin = true;

	for (count2 = 0; count2 < silarr.size(); count2++) { // the master program will look for clipstart or clipend, followed by a float representing a timestamp
		if (isbegin) {
			cout << "clipstart: ";
			isbegin = false;
		} else {
			cout << "clipend: ";
			isbegin = true;
		}
		cout << silarr.at(count2);
		if (not(count2 + 1 == silarr.size())) {cout << endl;}
	}
	
	exit(0);
}
// end of the writer file
