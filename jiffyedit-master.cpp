#include <fstream> // other includes inside common.hpp
#include <algorithm>
#include <filesystem>

#include "common.hpp"

using namespace std;
namespace fs = std::filesystem; // this sets it so that you can use fs:: instead of filesystem::

long long int boool /* int used to check whether string contains substring */;
string outpath, ismeta, totlen, width, height, dur, /* full duration of clip including last frame */ durm1, /* the second to last frame (needed for some reason) */ clspc, /* colorspace */ aspwid, /* aspect width string */ asphei, temps2;
bool hasres = false;
bool fheight = false; // found height indicator
bool fwidth = false;
bool fclsp = false;
bool shotcutb = false;
bool pitivib = false;
bool overwrite = false;
unsigned long long int aspwidi, /* aspect ratio width int */ aspheii, count6, len, clpint; // clipper int
float tempf2, fpsnum, fpsden, onefrm; // how often a new frame occurs in seconds
vector <float> clparr; // timestamps where 0 is begin, 1 is end, 2 is begin and so on

class clipper {
	public:
		string name; // front-facing name of the clipper
		string exe; // full path to executable
		string call; // front-facing call
		string args; // user specified arguments
		vector <string> help; // front-facing help section, defined by the clipper
};

vector <clipper> clipls;

void primary();
void shotcut();


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

string toanatim(float totimvar, int precision) { // to analogue time

	string result;
	int hrs = 0;
	int mins = 0;
	float secs = 0.0;
	float timecount = 0.0;
	float postdot = totimvar;
	while (postdot >= 1.0) { // find the stuff after the .
		postdot = postdot - 1.0;
	}
	totimvar = totimvar - postdot; // totimvar is the stuff before the .
	if (totimvar >= 3600.0) { // if totimvar is longer than 1 hour
		float hours = totimvar / 3600.0; // convert to hours
		while (hours >= 1.0) { // find how many hours there are
			hours = hours - 1.0;
			hrs++;
			timecount = timecount + 1.0;
		}
	}
	totimvar = totimvar - 3600.0 * timecount; // subtract the hours, if any

	if (totimvar >= 60.0) { // if totimvar is longer than 1 minute
	 	float minutes = totimvar / 60.0; // convert to minutes
	 	timecount = 0.0;
	 	while (minutes >= 1.0) { // find how many minutes there are
			minutes = minutes - 1.0;
			mins++;
			timecount = timecount + 1.0;

		}

	}
	totimvar = totimvar - 60.0 * timecount; // subtract the minutes, if any

	secs = totimvar + postdot;

	if (hrs < 10) {
		result = "0";
	}
	stringstream wrttim; // write time
	wrttim << result;
	wrttim << hrs;
	wrttim << ":";

	if (mins < 10) {
		wrttim << "0";
	}
	
	wrttim << mins;
	wrttim << ":";

	if (secs < 10.0) {
		wrttim << "0";

	}
	
	wrttim << fixed << setprecision(precision) << secs;

	getline(wrttim, result);

	return result;
}

void pitivi() {
	vector <string> out;
	
	cout << "Generating XGES file..." << endl;
	
	tempb = true;
	while (tempb) { // replace all spaces with %20 code
		pos = -1;
		pos = path.find(" ");
		if (pos == -1) {
			tempb = false;
			break;
		} else {
			path = replace(path, " ", "%20");
		}
	}
	
	out.push_back("<ges version=\'0.7\'>");
	out.push_back("  <project properties=\'properties;\' metadatas=\'metadatas, scaled_proxy_width=(int)0, scaled_proxy_height=(int)0, pitivi::title_safe_area_vertical=(double)0.80000000000000004, pitivi::title_safe_area_horizontal=(double)0.80000000000000004, pitivi::action_safe_area_vertical=(double)0.90000000000000002, pitivi::action_safe_area_horizontal=(double)0.90000000000000002, render-scale=(double)100;\'>");
	out.push_back("    <encoding-profiles>");
	out.push_back("      <encoding-profile name=\'pitivi-profile\' description=\'Pitivi encoding profile\' type=\'container\' preset-name=\'webmmux\' format=\'video/webm\' >");
	out.push_back("        <stream-profile parent=\'pitivi-profile\' id=\'0\' type=\'video\' presence=\'0\' format='video/x-vp8, profile=(string){ 0, 1, 2, 3 }' />");
	out.push_back("        <stream-profile parent=\'pitivi-profile\' id=\'1\' type=\'audio\' presence=\'0\' format='audio/x-vorbis, rate=(int)[ 1, 200000 ], channels=(int)[ 1, 255 ]' />");
	out.push_back("      </encoding-profile>");
	out.push_back("    </encoding-profiles>");
	out.push_back("    <ressources>");
	out.push_back(replace("      <asset id=\'file://PATH\' extractable-type-name=\'GESUriClip\' >", "PATH", path));
	out.push_back("        <stream-info />");
	out.push_back("        <stream-info />");
	out.push_back("      </asset>");
	out.push_back("      <asset id=\'crossfade\' extractable-type-name=\'GESTransitionClip\' >");
	out.push_back("      </asset>");
	out.push_back("    </ressources>");
	out.push_back("    <timeline >");
	out.push_back("      <track caps=\'video/x-raw(ANY)\' track-type=\'4\' track-id=\'0\' />");
	out.push_back("      <track caps=\'audio/x-raw(ANY)\' track-type=\'2\' track-id=\'1\' />");
	out.push_back("      <layer priority=\'0\' >");
	
	float prolen = 0.0;
	string dotstr = ".";
	count3 = 0;
	for (count2 = 0; count2 < clparr.size(); count2 = count2 + 2) {
		temps = "        <clip id=\'CLPNUM\' asset-id=\'file://PATH\' type-name='GESUriClip\' layer-priority=\'0\' track-types=\'6\' start=\'START\' duration=\'DUR\' inpoint=\'INPOINT\' rate=\'0\' properties=\'properties, name=(string)uriclipCOUNT3;\' >";
		temps = replace(temps, "CLPNUM", to_string(count3));
		temps = replace(temps, "PATH", path);
		
		stringstream getsrt; 
		//precision = 9.0; // fixed doesn't seem to be working in this part of the file so the precision has to be determined manually.
	//	if (clparr.at(count2) > 10.0 and clparr.at(count2) < 100.0 { // and i really don't understand this one. fixed is having completely different behavior in different parts of the same BINARY
//			precision = 10.0;
//		} else if (clparr.at(count2) > 100.0 and <clparr.at(count2 < ))
		getsrt << fixed << setprecision(9) << prolen;
		getline(getsrt, temps2);
		pos = temps2.find(dotstr);
		temps2.erase(pos, dotstr.size());  // pitivi wants these in some weird format. guint or something i think. this is the only way i can think of to convert
		temps = replace(temps, "START", temps2);
		
		stringstream getend;
		getend << fixed << setprecision(9) << clparr.at(count2 + 1) - clparr.at(count2);
		getline(getend, temps2);
		pos = temps2.find(dotstr);
		temps2.erase(pos,  dotstr.size());
		temps = replace(temps, "DUR", temps2);
		
		tempf = clparr.at(count2 + 1) - clparr.at(count2);
		prolen = prolen + tempf;
		
		stringstream getinp;
		getinp << fixed << setprecision(9) << clparr.at(count2);
		getline(getinp, temps2);
		pos = temps2.find(dotstr); 
		temps2.erase(pos, dotstr.size());
		temps = replace(temps, "INPOINT", temps2);
		
		temps = replace(temps, "COUNT3", to_string(count3));
		out.push_back(temps);
		
		out.push_back("          <source track-id=\'0\' >"); // video track
		
		temps = "            <binding type='direct' source_type='interpolation' property='alpha' mode='1' track_id='0' values =' START:1  END:1 '/>";
		
		stringstream getsrt2;
		getsrt2 << fixed << setprecision(9) << clparr.at(count2);
		getline(getsrt2, temps2);
		pos = temps2.find(dotstr);
		temps2.erase(pos, dotstr.size());
		temps = replace(temps, "START", temps2);
		
		stringstream getend2;
		getend2 << fixed << setprecision(9) << clparr.at(count2 + 1);
		getline(getend2, temps2);
		pos = temps2.find(dotstr);
		temps2.erase(pos, dotstr.size());
		temps = replace(temps, "END", temps2);
		
		out.push_back(temps);
		
		out.push_back("          </source>");
		out.push_back("          <source track-id='1' >"); // audio track
		
		temps = "            <binding type='direct' source_type='interpolation' property='volume' mode='1' track_id='1' values =' START:0.10000000149011612  END:0.10000000149011612 '/>";
		
		stringstream getsrt3; 
		getsrt3 << fixed << setprecision(9) << clparr.at(count2);
		getline(getsrt3, temps2);
		pos = temps2.find(dotstr);
		temps2.erase(pos, dotstr.size());
		temps = replace(temps, "START", temps2);
		
		stringstream getend3;
		getend3 << fixed << setprecision(9) << clparr.at(count2 + 1);
		getline(getend3, temps2);
		pos = temps2.find(dotstr);
		temps2.erase(pos, dotstr.size());
		temps = replace(temps, "END", temps2);
		
		out.push_back(temps);
		
		out.push_back("          </source>");
		out.push_back("        </clip>");
		
		count3++;
	}
	
	out.push_back("      </layer>");
	out.push_back("      <groups>");
	out.push_back("      </groups>");
	out.push_back("    </timeline>");
	out.push_back("  </project>");
	out.push_back("</ges>");
	
	ofstream wrtxges(outpath);
	for (count2 = 0; count2 < out.size(); count2++) {
		wrtxges << out.at(count2) << endl;
	}
	wrtxges.close();
	
	cout << "Done!" << endl;
	exit(0);
}

void reader() {

	/// inmeta is for getting info about the video
	tempb = false;
	// pipes in c++ seem to be horribly documented, so i'll do my best to explain it here. if i say i think something, it means im not entirely sure what exactly it does, so this is just my best guess from what little legible code and documentation i have been able to find

	FILE * inmeta = NULL; // so this here is creating a file pointer which will later become a datastream. according to cppreference.com (or somewhere similar), this type of stream is not meant to be accessed by any functions outside of the <stdio.h> library. Using functions from fstream would probably work, but i'm following their advice, just to be safe.
	char buf[1024]; // this is just a char array because its safer with C functions, i guess? strings would probably work, but again, better to be safe.

	const char * realcmd = cmd.c_str();
	cout << "Gathering metadata..." << endl;
	inmeta = popen(realcmd, "r"); // this tries to open a pipe (datastream) with ffmpeg for silencedetect. the first variable inside is literally a command, just like one you would run in a terminal or command line. the second variable, "r", indicates that we want to read data from this datastream.
	if (inmeta != NULL) {

		while (fgets(buf, 1024, inmeta) != NULL) { // this loop sets buf = the current line of the datastream. fgets is for plaintext data only. the 1024 specifies to read a max of 1024 bytes, so as not to overflow (at least i think so anyway). lastly, the datastream to read from is specified. the != null makes it so that when the datastream reaches its end, the loop ends, so as to prevent overflow
			metaline = buf; /// so for some reason i cant seem to add lines piped in from this specific command to an array of strings, which was originally how i was planning on doing it, so if you see any references to a string raw[], now you know why
			/// find lines with "Stream"
			ismeta = metaline.find("Stream"); /// finding the resolution and other metadata will be much easier with ffprobe, so that will happen in a later version
			boool = ismeta.compare("npos");

			if (boool < 0) {
				len = size(metaline);

				for (count3 = 0; count3 < len; count3++) { /// find resolution by scanning for three numbers in a row, then an 'x', then another three numbers. needs to stay because ffprobe is being weird
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
			}
			if (fheight and fwidth and fclsp) {break;}
		}
			pclose(inmeta); // closes the pipe after the end of the datastream
			inmeta = NULL; // clears the datastream from memory (i think)

	}

	string cmd2 = "ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 \"PATH\"";

	cmd2 = replace(cmd2, "PATH", path);

	const char * realcmd2 = cmd2.c_str();

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

	cmd3 = replace(cmd3, "PATH", path);

	const char * realcmd3 = cmd3.c_str();

	FILE * infps = NULL;

	if ((infps = popen(realcmd3, "r")) != NULL) {

		while (fgets(buf, 1024, infps) != NULL) {
			temps = buf;
		}
		pclose(infps);
		infps = NULL;
	}

	temps = replace(temps, "/", " ");

	stringstream getfps(temps);

	getfps >> fpsnum >> fpsden;
	
	string cmd4 = "\""; // take clips in
	clipls.at(clpint).exe.insert(0, "/");
	clipls.at(clpint).exe.insert(0, fs::current_path().generic_string());
	cmd4.append(clipls.at(clpint).exe);
	cmd4.append("\" \"PATH\"");
	cmd4 = replace(cmd4, "PATH", path);
	cmd4.append(" ");
	cmd4.append(clipls.at(clpint).args);

	const char * realcmd4 = cmd4.c_str();

	FILE * inclps = NULL;
	
	tempb = false; // tempb here is used to store whether the last timestamp was a clipstart or clipend. true = clipstart, false = clipend
	cout << "Clipping..." << endl;
	if ((inclps = popen(realcmd4, "r")) != NULL) {

		while (fgets(buf, 1024, inclps) != NULL) {
			temps = buf;
			pos = -1;
			pos = temps.find("clipstart: ");
			temp = temps.find("clipend: ");
			count6 = temps.find("Fatal error:");
			if (pos == 0) {
				if (tempb == true) {
					cout << endl << "Fatal error: Two clipstarts before clipend from clipper: " << clipls.at(clpint).name << endl;
					exit(6);
				}
				temps.erase(0, 10);
				stringstream getstart(temps);
				
				getstart >> tempf;
				
				clparr.push_back(tempf);
				tempb = true;
			} else if (temp == 0) {
				if (tempb == false) {
					cout << endl << "Fatal error: Two clipends before clipstart from clipper: " << clipls.at(clpint).name << endl;
					exit(6);
				}
				temps.erase(0, 8);
				stringstream getstart(temps);
				
				getstart >> tempf;
				
				clparr.push_back(tempf);
				tempb = false;
			} else if (count6 == 0) {
				cout << endl << temps << " (from clipper: " << clipls.at(clpint).name << " with call: " << clipls.at(clpint).call << ")" << endl;
				exit(6);
			} else {
				cout << temps << endl;
			}
		}
		if (tempb) {
			cout << endl << "Fatal error: Final clipstart with no matching clipend from clipper: " << clipls.at(clpint).name << endl;
			exit(6);
		}
		pclose(inclps);
		inclps = NULL;
	}

	if (clparr.size() == 0) {
		cout << "Fatal error: No clips from clipper: " << clipls.at(clpint).name << " with call: " << clipls.at(clpint).call << endl;
		exit(6);
	}

	int wid = stoi(width);
	int hei = stoi(height);
	temp = __gcd(wid, hei);
	aspwidi = wid / temp;
	aspheii = hei / temp;

	stringstream getwid;

	getwid << aspwidi;

	getline(getwid, aspwid);

	stringstream gethei;

	gethei << aspheii;

	getline(gethei, asphei);

	if (shotcutb) {shotcut();}
	if (pitivib) {pitivi();}
}

void primary();

int main(int argc, char * arga[]) {

	vector <string> ls; 
	stringstream getls; 
	string temp;
	unsigned long long int datcnt = 0;

	for (auto& p: fs::directory_iterator(fs::current_path())) { // get a list of every file in the current directory, then sort them to .dat files only
		getls << p << endl;
		getline(getls, temp);
		ls.push_back(temp); 
	}

	for (count6 = 0; count6 < ls.size(); count6++) {
		char tempc = ls.at(count6)[0]; 

		if (tempc == '\'') { // remove quotation marks, if any
			ls.at(count6).erase(0, 1);
		} else if (tempc == '\"') {
			ls.at(count6).erase(0, 1);
		}
		reverse(ls.at(count6).begin(), ls.at(count6).end()); 

        if (tempc == '\'') {
			ls.at(count6).erase(0, 1);
		} else if (tempc == '\"') {
			ls.at(count6).erase(0, 1);
		} // path will be reversed again later, just need to check for .dat files first

        pos = -1;
        pos = ls.at(count6).find("tad.");
        if (pos == 0) {
            ls.at(datcnt) = ls.at(count6);
            reverse(ls.at(datcnt).begin(), ls.at(datcnt).end()); datcnt++;
        }
    }

	for (count6 = ls.size(); count6 > datcnt; count6--) {
		ls.pop_back();
	}

	for (count6 = 0; count6 < ls.size(); count6++) {
		ifstream getplgs(ls.at(count6));
		string temptype;
		getline(getplgs, temptype);
		pos = -1;
		pos = temptype.find("ignore");
		if (pos < 0) {
			pos = -1;
    			pos = temptype.find("clipper");
    			if (pos == 0) {
      				clipper tmpclp;
      				getline(getplgs, tmpclp.name);
        			getline(getplgs, tmpclp.exe);
        			getline(getplgs, tmpclp.call);
        			pos = -1;
				while (getplgs and pos < 0) {
					getline(getplgs, temps);
					tmpclp.help.push_back(temps);
					pos = -1;
					pos = temps.find(tmpclp.call);
				}
 				clipls.push_back(tmpclp);
			} else {cout << endl << "Warning: Potentially invalid .dat file at " << ls.at(count6) << endl;}
		}
	}

	for (count6 = 0; count6 < clipls.size(); count6++) { // make sure that executable for clipper exists
		temps = fs::current_path().generic_string();

		reverse(temps.begin(), temps.end());

		temps2 = clipls.at(count6).exe;

		reverse(temps2.begin(), temps2.end());

		temps.insert(0, "/");

		temps.insert(0, temps2);
		
		reverse(temps.begin(), temps.end());

		fs::path tempath(temps);

		if (not(fs::exists(tempath))) {
			cout << "Error: Executable does not exist for " << clipls.at(count6).name << " with call: " << clipls.at(count6).call << endl << "You cannot use " << clipls.at(count6).name << endl;
			auto iter = clipls.begin();
			for (count3 = 0; count3 < count6; count3++) { // convert int to iterator
				iter++;
			}
			clipls.erase(iter);
		} 
	}
	vector <string> argv;
	
	bool fclipr = false; // found clipper
	string tempargs;
	if (argc == 1) {
		cout << "Fatal error: no path or instruction given" << endl;
		exit(4);
	} else if (arga[1][0] == '?') {
		cout << endl << "First, enter the file path. ex. \"/home/user/Videos/myvid.mp4\" Then, enter one of the options listed below;" << endl;
		for (count6 = 0; count6 < size(clipls); count6++) {
			cout << '	' << clipls.at(count6).name << ' ' << clipls.at(count6).call << endl;
			for (count2 = 0; count2 < size(clipls.at(count6).help); count2++) {
				cout << "		";
				if (count2 == size(clipls.at(count6).help) - 1) {
					cout << "Example command: ";
				}
				cout << clipls.at(count6).help.at(count2) << endl;
			}
		}
		exit(0);
	}
	
	for (count2 = 2; count2 < argc; count2++) { // convert arguments to safe format
		temps = arga[count2];
		argv.push_back(temps);
	}
	
	path = arga[1];
	for (count2 = 0; count2 < argv.size(); count2++) {
		tempc = argv.at(count2).at(0);
		if (tempc == '[') {
			tempc = argv.at(count2).at(argv.at(count2).size() - 1);
			if (tempc == ']') {
				for (count3 = 0; count3 < clipls.size(); count3++) {
					pos = -1;
					pos = clipls.at(count3).call.find(argv.at(count2));
					if (pos == 0) {
						clpint = count3;
						fclipr = true;
						break;
					}
				}
			} else {
				temps = argv.at(count2);
				temps.append("]");
				for (count3 = 0; count3 < clipls.size(); count3++) {
					pos = -1;
					pos = clipls.at(count3).call.find(temps);
					if (pos == 0) {
						clpint = count3;
						fclipr = true;
						break;
					}
				}
				
				for (count2 = count2; count2 < argv.size(); count2++) { // count2 has to be used here to increment the arguments
					tempc = argv.at(count2).at(0);
					if (not(tempc == ']')) {
						tempargs.append(argv.at(count2));
					} else {
						break;
					}
				}
			}
			if (not(fclipr)) {
				cout << "Fatal error: clipper not found" << endl;
				exit(4);
			}
		} else if (tempc == '-') {
			pos = -1;
			pos = argv.at(count2).find("-shotcut");
			if (pos == 0) {
				shotcutb = true;
			}
			pos = -1;
			pos = argv.at(count2).find("-pitivi");
			if (pos == 0) {
				pitivib = true;
			}
			pos = -1;
			pos = argv.at(count2).find("-overwrite");
			if (pos == 0) {
				overwrite = true;
			}
		}
	}
	if (not(shotcutb) and not(pitivib)) {
				cout << "Fatal error: no editor selected" << endl;
				exit(4);
	}
	
	primary();
}

void primary() {

	tempc = path[0];
	if (tempc == '\'') { // remove quotation marks sorrounding path, if any
		path.erase(0, 1);
	}
	else if (tempc == '\"') {
		path.erase(0, 1);
	}
	reverse(path.begin(), path.end());

	tempc = path[0];
	if (tempc == '\'') {
		path.erase(0, 1);
	}
	else if (tempc == '\"') {
		path.erase(0, 1);
	}
	reverse(path.begin(), path.end());

	// so this here is also terribly documented online. i will explain this in detail as well.
	// look up top for namespace stuff
	fs::path vidpath(path); // this is declaring what path you want to check
	bool fexists = true;
	fexists = fs::exists(vidpath); // fs::exists will return a bool, true if it exists, false if not.
	if (not(fexists)) {
		cout << endl << "Fatal error: Video file does not exist!" << endl;
		exit(4);
	}

	outpath = path; // get the path of the resulting file
	reverse(outpath.begin(), outpath.end());
	pos = -1;
	pos = outpath.find(".");
	outpath.erase(0, pos);
	if (shotcutb) {
		temps = "tlm";
	}
	if (pitivib) {
		temps = "segx";
	}
	temps.append(outpath);
	outpath = temps;
	reverse(outpath.begin(), outpath.end());

	fs::path xmlpath(outpath);
	fexists = fs::exists(xmlpath);
	if (fexists and not(overwrite)) {
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
	

	cmd = "ffmpeg -i \"";

	cmd.append(path);
	cmd.append("\" -hide_banner");
	cmd.append(" 2>&1");
	reader();
}

void shotcut() {
	// Shotcut MLT XML format

	tempf = fpsnum / fpsden; // find how often a new frame occurs. this more convoluted method is necessary for decimal framerates.
	onefrm = 1.0 / tempf;

	tofrmvar = secdur;
	secdur = tonrfram();

	cout << "Filtering clips..." << endl;

	for (count3 = 0; count3 < clparr.size(); count3++) { // round all the silence values to the nearest frame
		tofrmvar = clparr.at(count3); clparr.at(count3) = tonrfram();
	}


	cout << "Generating MLT XML file..." << endl;

	vector <string> fin; // final list to write because shotcut wants total time before it can be calculated
	vector <string> out; // holds the chains
	vector <string> playlist; // the playlist section goes directly after the chains and lists where each of the chains begins and ends


	fin.push_back("<?xml version=\"1.0\" standalone=\"no\"?>"); // All strings put directly into the output do not seem to change regardless of the video in question.
	fin.push_back("<mlt LC_NUMERIC=\"C\" version=\"7.8.0\" title=\"Shotcut version 22.09.23\" producer=\"main_bin\">");

	temps = "  <profile description=\"automatic\" width=\"WIDTH\" height=\"HEIGHT\" progressive=\"1\" sample_aspect_num=\"1\" sample_aspect_den=\"1\" display_aspect_num=\"ASPWID\" display_aspect_den=\"ASPHEI\" frame_rate_num=\"FPSNUM\" frame_rate_den=\"FPSDEN\" colorspace=\"CLSPC\"/>";

	temps = replace(temps, "WIDTH", width);
	temps = replace(temps, "HEIGHT", height);
	temps = replace(temps, "ASPWID", aspwid);
	temps = replace(temps, "ASPHEI", asphei);
	temps = replace(temps, "FPSNUM", to_string(fpsnum));
	temps = replace(temps, "FPSDEN", to_string(fpsden));
	temps = replace(temps, "CLSPC", clspc);

	fin.push_back(temps);

	fin.push_back("  <playlist id=\"main_bin\">");
	fin.push_back("    <property name=\"xml_retain\">1</property>");
	fin.push_back("  </playlist>");

	out.push_back("  </playlist>");

	playlist.push_back("  <playlist id=\"playlist0\">");
	playlist.push_back("    <property name=\"shotcut:video\">1</property>");
	playlist.push_back("    <property name=\"shotcut:name\">V1</property>");


	dur = toanatim(secdur, 3);
	durm1 = toanatim(secdur - onefrm, 3);

	float prolenm1 = 0.0; // project length minus 1 frame
	float prolen = 0.0; // total project length

	count2 = 0;

	for (count6 = 0; count6 < clparr.size(); count6 = count6 + 2) {

		string cnum = "chain"; // chain num
		cnum.append(to_string(count2));

		temps2 = "  <chain id=\"CHAIN\" out=\"DURM1\">";  // here the chain id gets replaced and then the durations after it on both strings
		temps2 = replace(temps2, "CHAIN", cnum);
		out.push_back(replace(temps2, "DURM1", durm1));

			temps = "    <entry producer=\"CHAIN\" in=\"IN\" out=\"OUT\"/>";  // everything that is indented here is to do with the playlist, just to make things clearer
			temps = replace(temps, "CHAIN", cnum);

			temp = count6 + 1; // need this line a few times because this variable is getting reassigned in some other function
			temps = replace(temps, "IN", toanatim(clparr.at(count6), 3)); // replace "IN" with the beginning of the clip
			temp = count6 + 1;
			playlist.push_back(replace(temps, "OUT", toanatim(clparr.at(temp), 3))); // replace "OUT" with the end of the clip

			temp = count6 + 1;

			tempf = clparr.at(temp) - clparr.at(count6); // get the length of the clip by subtracting the beginning from the end

			prolen = prolen + tempf; // add the duration of the clip to the aggregate project length

		temps = "    <property name=\"length\">DUR</property>";
		out.push_back(replace(temps, "DUR", dur));

		out.push_back("    <property name=\"eof\">pause</property>");

		temps = "    <property name=\"resource\">PATH</property>";
		out.push_back(replace(temps, "PATH", path));

		out.push_back("    <property name=\"mlt_service\">avformat-novalidate</property>");
		out.push_back("    <property name=\"seekable\">1</property>");
		out.push_back("    <property name=\"audio_index\">1</property>");
		out.push_back("    <property name=\"video_index\">0</property>");
		out.push_back("    <property name=\"mute_on_pause\">0</property>");
		out.push_back("    <property name=\"ignore_points\">0</property>");

		temps = "    <property name=\"shotcut:caption\">PATH</property>";
		out.push_back(replace(temps, "PATH", path));

		out.push_back("    <property name=\"xml\">was here</property>");
		out.push_back("  </chain>");

		count2++;
	}

	for (count2 = 0; count2 < playlist.size(); count2++) {
		out.push_back(playlist.at(count2)); // add the whole playlist array to the end of the out array
	}

	prolenm1 = prolen - onefrm;

	out.push_back("  </playlist>");

	temps = "  <tractor id=\"tractor0\" title=\"Shotcut version 22.09.23\" in=\"00:00:00.000\" out=\"TOTLENM1\">";
	out.push_back(replace(temps, "TOTLENM1", toanatim(prolenm1, 3)));

	out.push_back("    <property name=\"shotcut\">1</property>");
	out.push_back("    <property name=\"shotcut:projectAudioChannels\">2</property>");
	out.push_back("    <property name=\"shotcut:projectFolder\">0</property>");
	out.push_back("    <track producer=\"background\"/>");
	out.push_back("    <track producer=\"playlist0\"/>");
	out.push_back("    <transition id=\"transition0\">");
	out.push_back("      <property name=\"a_track\">0</property>");
	out.push_back("      <property name=\"b_track\">1</property>");
	out.push_back("      <property name=\"mlt_service\">mix</property>");
	out.push_back("      <property name=\"always_active\">1</property>");
	out.push_back("      <property name=\"sum\">1</property>");
	out.push_back("    </transition>");
	out.push_back("    <transition id=\"transition1\">");
	out.push_back("      <property name=\"a_track\">0</property>");
	out.push_back("      <property name=\"b_track\">1</property>");
	out.push_back("      <property name=\"version\">0.1</property>");
	out.push_back("      <property name=\"mlt_service\">frei0r.cairoblend</property>");
	out.push_back("      <property name=\"threads\">0</property>");
	out.push_back("      <property name=\"disable\">1</property>");
	out.push_back("    </transition>");
	out.push_back("  </tractor>");
	out.push_back("</mlt>");

	temps = "  <producer id=\"black\" in=\"00:00:00.000\" out=\"TOTLENM1\">"; // TOTLENM1 has to be the total length of project
	fin.push_back(replace(temps, "TOTLENM1", toanatim(prolenm1, 3)));

	temps = "    <property name=\"length\">TOTLEN</property>"; //  This appears to be the last frame of the project approximated to the thousandth.
	fin.push_back(replace(temps, "TOTLEN", toanatim(prolen, 3)));

	fin.push_back("    <property name=\"eof\">pause</property>");
	fin.push_back("    <property name=\"resource\">0</property>");
	fin.push_back("    <property name=\"aspect_ratio\">1</property>");
	fin.push_back("    <property name=\"mlt_service\">color</property>");
	fin.push_back("    <property name=\"mlt_image_format\">rgba</property>");
	fin.push_back("    <property name=\"set.test_audio\">0</property>");
	fin.push_back("  </producer>");
	fin.push_back("  <playlist id=\"background\">");

	temps = "    <entry producer=\"black\" in=\"00:00:00.000\" out=\"TOTLENM1\"/>";
	fin.push_back(replace(temps, "TOTLENM1", toanatim(prolenm1, 3)));

	for (count2 = 0; count2 < out.size(); count2++) {
		fin.push_back(out.at(count2));  // add the whole out to the end of the fin array
	}

	ofstream wrtmlt(outpath);

	for (count2 = 0; count2 < fin.size(); count2++) {
		wrtmlt << fin.at(count2) << endl;
	}
	wrtmlt.close();

	cout << "Done!" << endl;

	exit(0);
}
