#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <stdio.h>
#include <iomanip>
#include <cstring>
#include <filesystem>

using namespace std;

string metaline;
int temp;
int aspwidi; // aspect ratio width int
int aspheii;
string audchan; // audio channels
string path;
string outpath;
string ismeta;
string cmd;
int boool; // int used to check whether string contains substring
string totlen;
string metaline2;
string metaline3;

float fpsnum, fpsden;
string width;
string height;
string dur; // full duration of clip including last frame
string durm1; // the second to last frame (needed for some reason)
string clspc;
string aspwid; // aspect ratio width string
string asphei;
string var1;

string temps, temps2;
int len;
float secdur; // total length of the video in seconds
bool hasres = false;
bool hasfps = false;
bool hasdur = false;
bool fdur = false;
bool fheight = false; // found height indicator
bool fwidth = false;
bool fclsp = false;
bool fchan = false;
char tempc;
char tempc2;
int count2;
int count3;
int count5;
int pos = -1;
bool hasjob = false; // stops execution in case the user has entered a command with nothing to do
float buff = 0.3;
float buffx2;
float mclip = 0.5;
string mflt, bfflt;
bool ac = false;
int playlines = 0;
int lines = 0;
float tempf, tempf2, tempf3;
float onefrm; // how often a new frame occurs in seconds.
float silarr[1000000];
int silcnt = 0;
int decbels = 20;
float countf;

void primary();
void shotcut();

string sprstr;
string substr;
string repstr;
string in;

string replace() {
	pos = -1;
	pos = sprstr.find(substr);
	if (pos < 0) {
		cout << "Fatal error: Substring to replace not found. Please report, send video and command.";
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

float tofrmvar;

float tonrfram() { // to nearest valid frame. shotcut needs timestamps to be frames but in a decimal format. weird
	tempf = 0.0;
	for (countf = 2.0; tempf < tofrmvar; countf = countf + 1.0) {
		tempf = onefrm * countf;
	}
	
	tempf2 = tempf - onefrm;
	if (tempf2 < 0.0) {
		tempf2 = tempf2 * -1.0;
	}
	
	if (tempf < tempf2) {
		return tempf;
	} else {
		return tempf2;
	}
}

void reset() {primary();} // can reset the program when needed.

float totimvar;

string toanatim() { // to analogue time

	string result;
	int hrs = 0;
	int mins = 0;
	float secs = 0.0;
	countf = 0.0;
	tempf = totimvar;
	while (tempf >= 1.0) { // find the stuff after the .
		tempf = tempf - 1.0;
	}
	totimvar = totimvar - tempf; // totimvar is the stuff before the .
	if (totimvar >= 3600.0) { // if totimvar is longer than 1 hour
		tempf2 = totimvar / 3600.0; // convert to hours
		while (tempf2 >= 1.0) { // find how many hours there are
			tempf2 = tempf2 - 1.0;
			hrs++;
			countf = countf + 1.0;
		}
	}
	totimvar = totimvar - 3600.0 * countf; // subtract the hours, if any
	
	if (totimvar >= 60.0) { // if totimvar is longer than 1 minute
	 	tempf2 = totimvar / 60.0; // convert to minutes
	 	countf = 0.0;
	 	while (tempf2 >= 1.0) { // find how many minutes there are
			tempf2 = tempf2 - 1.0;
			mins++;
			countf = countf + 1.0;
			
		}
		
	}
	totimvar = totimvar - 60.0 * countf; // subtract the minutes, if any

	secs = totimvar + tempf;
	
	if (hrs < 10) { 
		result = "0";
	}
	stringstream wrttim; // write time
	wrttim << result;
	if (wrttim << hrs) {}
	else {
		cout << "hours writing to string failed. fatal" << endl;
		exit(2);
	}
	wrttim << ":";
	
	if (mins < 10) {
		wrttim << "0";
	}
	if (wrttim << mins) {}
	else {
		cout << "minutes writing to string failed. fatal" << endl;
		exit(2);
	}
	wrttim << ":";
	
	if (secs < 10.0) {
		wrttim << "0";
		
	}
	if (wrttim << fixed << setprecision(3) << secs) {}
	else {
		cout << "seconds writing to string failed. fatal" << endl;
		exit(2);
	}
	
	getline(wrttim, result);
	
	return result;
}

void reader() {
	
	/// inmeta is for getting the silences
	bool tempb = false;
	// pipes in c++ seem to be horribly documented, so i'll do my best to explain it here. if i say i think something, it means im not entirely sure what exactly it does, so this is just my best guess from what little legible code and documentation i have been able to find
	
	FILE * inmeta = NULL; // so this here is creating a file pointer which will later become a datastream. according to cppreference.com, this type of stream is not meant to be accessed by any functions outside of the <stdio.h> library. Using functions from fstream would probably work, but i'm following their advice, just to be safe.
	char buf[1024]; // this is just a char array because its safer with C functions, i guess? strings would probably work, but again, better to be safe.
	
	char * getcmd = new char[cmd.length() + 1]; // the string needs to be converted to a const char * for the pipe funtion. this is done here
	strcpy(getcmd, cmd.c_str());
	
	const char * realcmd = getcmd;
	
	inmeta = popen(realcmd, "r"); // this tries to open a pipe (datastream) with ffmpeg for silencedetect. the first variable inside is literally a command, just like one you would run in a terminal or command line. the second variable, "r", indicates that we want to read data from this datastream.
	if (inmeta != NULL) {
		
		while (fgets(buf, 1024, inmeta) != NULL) { // this loop sets buf = the current line of the datastream. fgets is for plaintext data only. the 1024 specifies to read a max of 1024 bytes, so as not to overflow (at least i think so anyway). lastly, the datastream to read from is specified. the != null makes it so that when the datastream reaches its end, the loop ends, so as to prevent overflow
			metaline = buf; /// so for some reason i cant seem to add lines piped in from this specific command to an array of strings, which was originally how i was planning on doing it, so if you see any references to a string raw[], now you know why
			
			if (silcnt > 999000) {
				cout << endl << "FATAL ERROR: Video file too large." << endl;
				exit(1);
			}
			/// find lines with "Stream"
			ismeta = metaline.find("Stream"); /// finding the resolution and other metadata will be much easier with ffprobe, so that will happen in a later version
			boool = ismeta.compare("npos");
		
			if (boool < 0) {
				len = size(metaline);
			
				for (count3 = 0; count3 < len; count3++) { /// find resolution by scanning for three numbers in a row, then an 'x', then another three numbers
					if (hasres) {break;}
					temp = count3;
					if (isnum()) {
						temp = count3 + 1;
						if (isnum()) {
							temp = count3 + 2;
							if (isnum()) {
								temp = count3 + 3;
								tempc = metaline[temp];
								if (tempc == 'x') {
									temp = count3 + 4;
									if (isnum()) {
										temp = count3 + 5;
										if (isnum()) {
											temp = count3 + 6;
											if (isnum()) {
										
												hasres = true; /// resolution found
												temp = count3 + 4; /// should be the first number of the height
												while (isnum() and temp < len and fheight == false) { // save every number after the 'x' until it finds a character that is not a number (should be ' ')
													temps = metaline[temp];
													height.append(temps);
													temp++;
												}
												fheight = true;
											
												temp = count3 + 2; /// should be the last number of width
												while (isnum() and temp > 0 and fwidth == false) { /// save every before after the 'x' until it finds a character that is not a number (should be ' ')
													temps = metaline[temp];
													width.append(temps);
													temp--;
												}
												fwidth = true;
												reverse(width.begin(), width.end());
											}
										}
									}
								}
							}			
						}
					}
					if (fwidth and fheight) {break;}
				}
			}
			
			ismeta = metaline.find("Stream"); /// finding the resolution and other metadata will be much easier with ffprobe, so that will happen in a later version
			boool = ismeta.compare("npos");
		
			if (boool < 0) {
				len = size(metaline);
				temps = metaline.find("709");
				boool = temps.compare("npos");
				if (boool < 0 and fclsp == false) {clspc = "709"; fclsp = true;}
					temps = metaline.find("601");
					boool = temps.compare("npos");
				if (boool < 0 and fclsp == false) {clspc = "601"; fclsp = true;}
					temps = metaline.find("stereo");
					boool = temps.compare("npos");
				if (boool < 0 and fchan == false) {audchan = "stereo"; fchan = true;}
			
			}
		
			ismeta = metaline.find("Duration");
			boool = ismeta.compare("npos");
		
			if (boool < 0) {
				len = size(metaline);
				hasdur = true;
			
				for (count3 = 0; count3 < len; count3++) {
					tempc = metaline[count3];
					if (tempc == '.') {
						temp = count3 + 2; /// move forward 2 to start logging backward
						while (temp > 0 and fdur == false) {
							temps = metaline [temp];
							dur.append(temps);
							temp--;
							tempc = metaline[temp];
							if (tempc == ' ') {break;}
						}
						fdur = true;
						reverse(dur.begin(), dur.end());
					}
					if (fdur) {break;}
				}
			}
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
			
		
			pos = -1;
			pos = metaline2.find("silence_start: "); /// finds the start of the silence
		
			if (pos > 0) {
				pos = pos + 15;
				metaline2.erase(0, pos); // isolate the float value by erasing the rest of the string.
				stringstream stoof(metaline2);
			
				if (stoof >> silarr[silcnt]) {
					silcnt++;
				}
				else {
					cout << "Silence start float conversion failed. Fatal, probably ffmpeg error: " << endl;
					exit(2);
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
			
				if (stoof2 >> silarr[silcnt]) {
					silcnt++;
				}
				else {
					cout << "Silence end float conversion failed. Fatal." << endl << metaline2 << endl;
					exit(2);
				}
				metaline3 = metaline2;
				pos = metaline3.find("silence_duration: ");
				pos = pos + 18;
				metaline3.erase(0, pos);
				stringstream stoof3(metaline3);
			
				if (stoof3 >> silarr[silcnt]) {
					silcnt++;
				}
				else {
					cout << "Silence duration float conversion failed. Fatal." << endl;
					exit(2);
				}
			}
	
		
	//	if (ffps and fdur and fheight and fwidth and fclsp and fchan) {break;}
			}
		}
		pclose(inmeta); // closes the pipe after the end of the datastream
		inmeta = NULL; // clears the datastream from memory (i think)
			
	} else {
		cout << endl << "Pipe opening failed. Try installing FFmpeg. Fatal." << endl;
		exit(1);
	}
	
	inmeta = NULL;
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
	}
	
	
	
	stringstream getlen(totlen);
	
	getlen >> secdur;
	
	string cmd3 = "ffprobe -v error -select_streams v -of default=noprint_wrappers=1:nokey=1 -show_entries stream=r_frame_rate \"PATH\""; // fetch fps
	
	sprstr = cmd3; substr = "PATH"; repstr = path; cmd3 = replace();
	
	char * getcmd3 = new char[cmd3.length() + 1]; // the string needs to be converted to a const char * for the pipe funtion. this is done here
	strcpy(getcmd3, cmd3.c_str());
	
	const char * realcmd3 = getcmd3;
	
	FILE * infps = NULL;
	
	if ((infps = popen(realcmd3, "r")) != NULL) { 
		
		while (fgets(buf, 1024, infps) != NULL) {
			temps = buf;
		}
		pclose(infps);
		infps = NULL;
	}
	
	sprstr = temps; substr = "/"; repstr = " "; temps = replace();
	
	stringstream getfps(temps);
	
	getfps >> fpsnum >> fpsden;
	
	int wid = stoi(width);
	int hei = stoi(height);
	temp = __gcd(wid, hei);
	aspwidi = wid / temp;
	aspheii = hei / temp;
	
	delete [] realcmd3;
	delete [] realcmd2;
	delete [] realcmd;
	
	stringstream getwid;
	
	getwid << aspwidi;
	
	getline(getwid, aspwid);
	
	stringstream gethei;
	
	gethei << aspheii;
	
	getline(gethei, asphei);
	
	shotcut();
}

void primary();


// Starting the program in the primary function intead of the main allows for returning to the beginning of the program without restarting.
int main() {primary();}

void primary() {
	
	cout << "Welcome to JiffyEdit V1.0.1-1. Enter [?] for help." << endl;
	cout << ">>: ";
	
	getline(cin, in); // need to use getline for parsing input. cin >> doesnt work well enough for this
	
	tempc = in[0];
	
	if (tempc == '?') {
		
		cout << endl << "First, enter the file path. ex. \"/home/user/Videos/myvid.mp4\" Then, enter at least one of the options listed below;" << endl;
		cout << endl << "		\"sr\" --> Silence Remover. Removes silent portions from a video. Suboptions:" << endl;
		cout << endl << "			\"sr-bf#\" --> Buffer. The amount of time to extend the clips by. Helps prevent audio clipping. Clips that are closer to each other than the set buffer will be joined. Default 0.3 seconds." << endl;
		cout << endl << "			\"sr-mt#\" --> The minimum amount of time for a non-silent clip to be included in the final resut. Default 0.5 seconds." << endl;
		cout << endl << "			\"sr-db#\" -->  The maximum volume for where silence occors, in dB. Default -20 dB." << endl;
		
		cout << "Example command: \"/home/user/Videos/myvid.mp4 -inpf" << " sr" << " sr-bf0.2" << " sr-mt1.0" << " sr-db15\"" << endl;
		reset();
	} else {
	
		tempc = in[0];
		if (tempc == '\'') {
			in.erase(0, 1);
		}
		else if (tempc == '\"') {
			in.erase(0, 1);
		}
		path = in;
		reverse(path.begin(), path.end());
		pos = -1;
		pos = path.find("fpni-");
		if (pos < 0) {
			cout << endl << "Fatal error. User failed to specify input." << endl;
			exit(4);
		}
		pos = pos + 5;
		
		while (tempc == ' ' and pos < size(path)) {
			pos++;
			tempc = path[pos];
		}
		
		pos++;
		path.erase(0, pos); // isolate the path of the file to the path object
		tempc = path[0];
		if (tempc == '\'') {
			path.erase(0, 1);
		}
		else if (tempc == '\"') {
			path.erase(0, 1);
		}
		reverse(path.begin(), path.end());
		
		// so this here is also terribly documented online. i will explain this in detail as well.
		namespace fs = std::filesystem; // this just sets it so that you can use fs:: instead filesystem::
		fs::path vidpath(path); // this is declaring what path you want to check
		bool fexists = true;
		fexists = fs::exists(vidpath); // fs::exists will return a bool, true if it exists, false if not.
		if (fexists == false) {
			cout << endl << "FATAL ERROR: Video file does not exist!" << endl;
			exit(4);
		}
		
		outpath = path; // get the path of the resulting mlt file
		reverse(outpath.begin(), outpath.end());
		pos = -1;
		pos = outpath.find(".");
		outpath.erase(0, pos);
		temps = "tlm";
		temps.append(outpath);
		outpath = temps;
		reverse(outpath.begin(), outpath.end());
		
		fs::path xmlpath(outpath);
		fexists = fs::exists(xmlpath);
		if (fexists) {
			cout << endl << outpath << " already exists. Continuing will overwrite it. Continue? (Y/n): ";
			getline(cin, temps);
			tempc = temps[0];
			if (tempc == 'y' or tempc == 'Y') {
				cout << endl << "Continuing..." << endl;
			} else {
				cout << endl << "Program aborted by user." << endl;
				exit(0);
			}
		}
		
		in.erase(0, size(path)); // remove unecessary and potentially harmful info from in
		pos = -1;
		in.append("  "); // this is to ensure there is no out of range errors
		pos = in.find("sr");
		if (pos >= 0) {
			hasjob = true;
			
			pos = -1;
			pos = in.find("sr-bf");
			if (pos >= 0) {
				pos = pos + 5;
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
				} else {
					cout << endl << "Error: Failed to convert buffer string to buffer float. Using default buffer value." << endl;
				}
			}
			
			pos = -1;
			pos = in.find("sr-db");
			if (pos >= 0) {
				pos = pos + 5;
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
				} else {
					cout << endl << "Error: Failed to convert buffer string to buffer float. Using default buffer value." << endl;
				}
			}
			
			pos = -1;
			pos = in.find("sr-mt");
			if (pos >= 0) {
				pos = pos + 5;
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
				} else {
					cout << endl << "Error: Failed to convert minimum clip string to float. Using default value." << endl;
				}
			}
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
    cout << "Detecting silence..." << endl;
	// system("ffmpeg -i audiotest.mp4 -hide_banner -af silencedetect=noise=-15dB:d=0.03 -f null - > output.txt 2>&1"); // ffmpeg command must be formed with string, this is for test purposes only (change it later)
	if (hasjob) {reader();}
}
// start of the shotcut xml writer
void shotcut() {
	// Shotcut MLT XML format
	
	if (fdur == false) {
		cout << "Duration not found. Fatal.";
		exit(3);
	}
	
	tempf = fpsnum / fpsden; // find how often a new frame occurs. this more convoluted method is necessary for decimal framerates.
	onefrm = 1.0 / tempf;
	tofrmvar = 3.25786;
	
	tofrmvar = buff;
	buff = tonrfram();
	tofrmvar = mclip;
	mclip = tonrfram();
	buffx2 = buff * 2;
	tofrmvar = secdur;
	secdur = tonrfram();
	
	cout << "Filtering clips..." << endl;
	
	float tempsil[1000000];
	for (count3 = 0; count3 < silcnt; count3++) { // round all the silence values to the nearest frame
		tofrmvar = silarr[count3];
		silarr[count3] = tonrfram();
		//cout << endl << "rounding timestamp: " << silarr[count3] << endl;
	}
	
	count5 = 0;
	
	for (count2 = 0; count2 < silcnt; count2 = count2 + 3) { // filter the silences by their duration
		temp = count2 + 2; // temp is the duration of the silence
		if (silarr[temp] > buffx2) { // if silence duration is enough, save it
			temp = count2 + 1; // temp is now the silence end
			tempsil[count5] = silarr[count2]; count5++; // save the silence start to the list
			tempsil[count5] = silarr[temp]; count5++; // save the silence end
		}
	}
	
	
	silcnt = count5;
	count5 = 0;
	count3 = 2;
	/*
	for (count2 = 0; count2 < silcnt; count2++) {
		cout << endl << "tempsil: " << tempsil[count2] << endl;
	}
	*/
	if (tempsil[0] == 0.0) { // if the first item here is 0, then the next needs to be the start of the first chain because 0 cannot be the end of a chain
		silarr[0] = tempsil[1];
	}
	else {
		silarr[0] = 0.0; // if it isnt 0, the start of the first chain needs to be 0
	}
	
	for (count2 = 1; count3 < silcnt; count2++) { // filter everything else
		silarr[count2] = tempsil[count3]; count5++;
		count3++;
	}
	
	if (silarr[count5] != secdur) { // if the last silence end is less than the dur, make the dur the end of the final clip. if not, the silence end is junk and wont be used.
		
		silarr[count2] = secdur; count5++;
		cout << endl << silarr[count2] << endl;
	}
	
	silcnt = count5;
	count5 = 0;
	
	for (count2 = 0; count2 < silcnt; count2++) {
		cout << endl << "silarr: " << silarr[count2] << endl;
	}
	
	for (count2 = 0; count2 < silcnt; count2 = count2 + 2) { // sort the clips by the their duration
		temp = count2 + 1; // tempf2 is the clip ends
		tempf = silarr[temp] - silarr[count2]; // find the clip dur
		if (tempf > mclip) {
			tempsil[count5] = silarr[count2]; count5++;
			tempsil[count5] = silarr[temp]; count5++;
		}
	}
	
	silcnt = count5;
	count5 = 0;
	
	for (count2 = 0; count2 < silcnt; count2++) {
		cout << endl << "tempsil: " << tempsil[count2] << endl;
		}
	
	if (tempsil[0] < buff) { // beginning endian for adding the buffer
		silarr[0] = 0.0; count5++;
		silarr[1] = tempsil[1] + buff; count5++;
	}
	
	for (count2 = count5; count2 < silcnt; count2 = count2 + 2) { // add the buffer to all the rest
		temp = count2 + 1;
		silarr[count5] = tempsil[count2] - buff; count5++;
		silarr[count5] = tempsil[temp] + buff; count5++;
	}
	
	temp = count5 - 1;
	
	if (secdur - silarr[temp] < 0.0) {
		silarr[temp] = secdur;
	}

	for (count2 = 0; count2 < silcnt; count2++) {
		cout << endl << "silarr: " << silarr[count2] << endl;
	}

	
	cout << "Generating MLT XML file..." << endl;
	
	int finlines = 0;
	
	string fin[30000]; // final list to write because shotcut wants total time before it can be calculated
	string out[30000]; // holds the chains
	string playlist[10000]; // the playlist section goes directly after the chains and lists where each of the chains begins and ends
	
	
	fin[finlines] = "<?xml version=\"1.0\" standalone=\"no\"?>"; finlines++; // All strings put directly into the output do not seem to change regardless of the video in question.
	fin[finlines] = "<mlt LC_NUMERIC=\"C\" version=\"7.8.0\" title=\"Shotcut version 22.09.23\" producer=\"main_bin\">"; finlines++;
	
	var1 = "  <profile description=\"automatic\" width=\"WIDTH\" height=\"HEIGHT\" progressive=\"1\" sample_aspect_num=\"1\" sample_aspect_den=\"1\" display_aspect_num=\"ASPWID\" display_aspect_den=\"ASPHEI\" frame_rate_num=\"FPSNUM\" frame_rate_den=\"FPSDEN\" colorspace=\"CLSPC\"/>";
	
	stringstream wrtnum;
	
	wrtnum << fpsnum;
	
	getline(wrtnum, temps);
	
	stringstream wrtden;
	
	wrtden << fpsden;
	
	getline(wrtden, temps2);
	
	sprstr = var1; substr = "WIDTH"; repstr = width; var1 = replace();
	sprstr = var1; substr = "HEIGHT"; repstr = height; var1 = replace();
	sprstr = var1; substr = "ASPWID"; repstr = aspwid; var1 = replace();
	sprstr = var1; substr = "ASPHEI"; repstr = asphei; var1 = replace();
	sprstr = var1; substr = "FPSNUM"; repstr = temps; var1 = replace();
	sprstr = var1; substr = "FPSDEN"; repstr = temps2; var1 = replace();
	sprstr = var1; substr = "CLSPC"; repstr = clspc; var1 = replace();
	
	fin[finlines] = var1; finlines++;
	
	fin[finlines] = "  <playlist id=\"main_bin\">"; finlines++;
	fin[finlines] = "    <property name=\"xml_retain\">1</property>"; finlines++;
	fin[finlines] = "  </playlist>"; finlines++;
	
	
	
	out[lines] = "  </playlist>"; lines++;
	
	playlist[playlines] = "  <playlist id=\"playlist0\">"; playlines++;
	playlist[playlines] = "    <property name=\"shotcut:video\">1</property>"; playlines++;
	playlist[playlines] = "    <property name=\"shotcut:name\">V1</property>"; playlines++;
	
	
	totimvar = secdur; dur = toanatim();
	totimvar = secdur - onefrm; durm1 = toanatim();
	
	float prolenm1 = 0.0; // project length minus 1 frame
	float prolen = 0.0; // total project length
	
	
	
	count2 = 0;
	
	for (int count = 0; count < silcnt and lines < 29000; count = count + 2) {
		repstr = "chain";
		repstr.append(to_string(count2));
		
		string cnum = repstr; // chain num
		
		string var4 = "  <chain id=\"CHAIN\" out=\"DURM1\">";  // here the chain id gets replaced and then the durations after it on both strings
		sprstr = var4; substr = "CHAIN"; repstr = cnum; var4 = replace();
		
			temps = "    <entry producer=\"CHAIN\" in=\"IN\" out=\"OUT\"/>";  // everything that is indented here is to do with the playlist, just to make things clearer
			sprstr = temps; temps = replace();
			
		sprstr = var4; substr = "DURM1"; repstr = durm1; out[lines] = replace(); lines++;
		
			temp = count + 1; // need this line a few times because this variable is getting reassigned in some other function
			sprstr = temps; substr = "IN"; totimvar = silarr[count]; repstr = toanatim(); temps = replace(); // replace "IN" with the beginning of the clip
			temp = count + 1;
			sprstr = temps; substr = "OUT"; totimvar = silarr[temp]; repstr = toanatim(); playlist[playlines] = replace(); playlines++; // replace "OUT" witht the end of the clip
			
			temp = count + 1;
			
			tempf = silarr[temp] - silarr[count]; // get the length of the clip by subtracting the beginning from the end
			
			prolen = prolen + tempf; // add the duration of the clip to the aggregate project length
		
		temps = "    <property name=\"length\">DUR</property>";
		sprstr = temps; substr = "DUR"; repstr = dur; out[lines] = replace(); lines++;
		
		out[lines] = "    <property name=\"eof\">pause</property>"; lines++;
		 
		temps = "    <property name=\"resource\">PATH</property>";
		sprstr = temps; substr = "PATH"; repstr = path; temps = replace();
		 
		out[lines] = temps; lines++;
		 
		out[lines] = "    <property name=\"mlt_service\">avformat-novalidate</property>"; lines++;
		out[lines] = "    <property name=\"seekable\">1</property>"; lines++;
		out[lines] = "    <property name=\"audio_index\">1</property>"; lines++;
		out[lines] = "    <property name=\"video_index\">0</property>"; lines++;
		out[lines] = "    <property name=\"mute_on_pause\">0</property>"; lines++;
		out[lines] = "    <property name=\"ignore_points\">0</property>"; lines++;
		
		temps = "    <property name=\"shotcut:caption\">PATH</property>";
		
		sprstr = temps; substr = "PATH"; repstr = path; temps = replace();
		
		out[lines] = temps; lines++;
		
		out[lines] = "    <property name=\"xml\">was here</property>"; lines++;
		out[lines] = "  </chain>"; lines++;
		
		count2++;
	}
	
	for (int count2 = 0; count2 < playlines; count2++) {
		out[lines] = playlist[count2]; lines++; // add the whole playlist array to the end of the out array
	}
	
	prolenm1 = prolen - onefrm;
	
	out[lines] = "  </playlist>"; lines++;
	
	temps = "  <tractor id=\"tractor0\" title=\"Shotcut version 22.09.23\" in=\"00:00:00.000\" out=\"TOTLENM1\">";
	sprstr = temps; substr = "TOTLENM1"; totimvar = prolenm1; repstr = toanatim(); out[lines] = replace(); lines++;
	
	out[lines] = "    <property name=\"shotcut\">1</property>"; lines++;
	out[lines] = "    <property name=\"shotcut:projectAudioChannels\">2</property>"; lines++;
	out[lines] = "    <property name=\"shotcut:projectFolder\">0</property>"; lines++;
	out[lines] = "    <track producer=\"background\"/>"; lines++;
	out[lines] = "    <track producer=\"playlist0\"/>"; lines++;
	out[lines] = "    <transition id=\"transition0\">"; lines++;
	out[lines] = "      <property name=\"a_track\">0</property>"; lines++;
	out[lines] = "      <property name=\"b_track\">1</property>"; lines++;
	out[lines] = "      <property name=\"mlt_service\">mix</property>"; lines++;
	out[lines] = "      <property name=\"always_active\">1</property>"; lines++;
	out[lines] = "      <property name=\"sum\">1</property>"; lines++;
	out[lines] = "    </transition>"; lines++;
	out[lines] = "    <transition id=\"transition1\">"; lines++;
	out[lines] = "      <property name=\"a_track\">0</property>"; lines++;
	out[lines] = "      <property name=\"b_track\">1</property>"; lines++;
	out[lines] = "      <property name=\"version\">0.1</property>"; lines++;
	out[lines] = "      <property name=\"mlt_service\">frei0r.cairoblend</property>"; lines++;
	out[lines] = "      <property name=\"threads\">0</property>"; lines++;
	out[lines] = "      <property name=\"disable\">1</property>"; lines++;
	out[lines] = "    </transition>"; lines++;
	out[lines] = "  </tractor>"; lines++;
	out[lines] = "</mlt>"; lines++;
	
	
	
	string var2 = "  <producer id=\"black\" in=\"00:00:00.000\" out=\"TOTLENM1\">"; // TOTLENM1 has to be the total length of project
	sprstr = var2; substr = "TOTLENM1"; totimvar = prolenm1; repstr = toanatim(); var2 = replace();
	
	fin[finlines] = var2; finlines++;
	
	string var3 = "    <property name=\"length\">TOTLEN</property>"; //  This appears to be the last frame of the project approximated to the thousandth.
	sprstr = var3; substr = "TOTLEN"; totimvar = prolen; repstr = toanatim(); var3 = replace();
	
	fin[finlines] = var3; finlines++;
	
	fin[finlines] = "    <property name=\"eof\">pause</property>"; finlines++;
	fin[finlines] = "    <property name=\"resource\">0</property>"; finlines++;
	fin[finlines] = "    <property name=\"aspect_ratio\">1</property>"; finlines++;
	fin[finlines] = "    <property name=\"mlt_service\">color</property>"; finlines++;
	fin[finlines] = "    <property name=\"mlt_image_format\">rgba</property>"; finlines++;
	fin[finlines] = "    <property name=\"set.test_audio\">0</property>"; finlines++;
	fin[finlines] = "  </producer>"; finlines++;
	fin[finlines] = "  <playlist id=\"background\">"; finlines++;
	
	temps = "    <entry producer=\"black\" in=\"00:00:00.000\" out=\"TOTLENM1\"/>"; 
	sprstr = temps; substr = "TOTLENM1"; totimvar = prolenm1; repstr = toanatim(); temps = replace();
	fin[finlines] = temps; finlines++;
	
	for (count2 = 0; count2 < lines; count2++) {
		fin[finlines] = out[count2]; finlines++; // add the whole out to the end of the fin array
	}
	
	if (lines >= 28998) {
		cout << endl << "Warning: Too much silence in video. The resulting file may not be complete." << endl;
	}
	
	ofstream wrtmlt(outpath);
	
	for (count2 = 0; count2 < finlines; count2++) {
		wrtmlt << fin[count2] << endl;
	}
	
	wrtmlt.close();
	
	cout << "Done!" << endl;
	
	
	
	exit(0);
}
// end of the writer file
