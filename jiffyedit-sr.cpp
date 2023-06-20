#include "common.hpp" // includes inside common.hpp

using namespace std;

string bfflt, in, totlen, metaline3;
float buffx2; // minimum silence time
bool fdur = false, logging = false, cb = false; // custom buffer
long long int len;
float buff = 0.3;
float mclip = 0.5;
vector <float> silarr;
vector <float> tempsil;
int decbels = 20;

void sorter();

inline void reader() {
	char buf[1025];

	string cmd2 = "ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 \"PATH\"";
	cmd2 = replace(cmd2, "PATH", path);
	if (logging) {log("cmd2: " + cmd2);}
	const char * realcmd2 = cmd2.c_str();
	FILE * indur = NULL;
	if ((indur = popen(realcmd2, "r")) != NULL) {
		while (fgets(buf, 1024, indur) != NULL) {
			totlen = buf;
		}
		pclose(indur);
		indur = NULL;
		fdur = true;
	}

	secdur = stof(totlen);

	/// inmeta is for getting the silences

	FILE * inmeta = NULL;

	const char * realcmd = cmd.c_str();
	if (logging) {log("cmd: " + cmd);}
	inmeta = popen(realcmd, "r");
	b1 = false;
	thread pctrl(pcheck, ref(inmeta), ref(b1));
	while (not b1) {
		while (fgets(buf, 1024, inmeta) != NULL) {
			metaline2 = buf;
			c1 = metaline2.at(metaline2.size() - 1);
			if (not isnum(c1)) {
				metaline2.erase(metaline2.size() - 1, metaline2.size()); // erase newline character at the end of the string, if there is one
			}
			pos = -1;
			pos = metaline2.find("silence_start: "); // finds the start of the silence
			if (pos > 0) {
				pos = pos + 15;
				metaline2.erase(0, pos); // isolate the float value by erasing the rest of the string.
				silarr.push_back(stof(metaline2));
			}

			pos = -1;
			pos = metaline2.find("silence_end: "); // finds the ending float of the silence_detect
			if (pos > 0) {
				pos = pos + 13;
				metaline2.erase(0, pos);
				pos = 0;
				while (c1 != ' ' and pos < size(metaline2)) {
					pos++;
					c1 = metaline2.at(pos);
				}
				silarr.push_back(stof(metaline2));

				stringstream getper; // get percentage
				getper << fixed << setprecision(0) << stof(metaline2) / secdur * 100;
				getline(getper, s1);
				s1.append("%");
				cout << fixed << setprecision(3) << "reset: clipping: " << stof(metaline2) << "/" << secdur << " " << s1 << endl; // progress

				metaline3 = metaline2;
				pos = metaline3.find("silence_duration: ");
				pos = pos + 18;
				metaline3.erase(0, pos);
				silarr.push_back(stof(metaline3));
			}
		}
		pclose(inmeta);
		inmeta = NULL;
	}
	if (silarr.size() == 0) {
		cout << "Fatal error: No clips found in video." << endl;
		if (logging) {log("Fatal error: No clips found in video.");}
		exit(4);
	}

	sorter();
}

int main(int argc, char * arga[]) {
	cout << endl;
	vector <string> argv;
	path = arga[1];
	if (argc > 2) {
		for (i1 = 0; i1 < argc; i1++) {
			argv.push_back(arga[i1]);
		}
		for (i1 = 0; i1 < argv.size(); i1++) {
			if (argv.at(i1).find("bf") == 0) {i1++; buff = stof(argv.at(i1));}
			else if (argv.at(i1).find("db") == 0) {
				i1++; decbels = stof(argv.at(i1));
				if (decbels < 0.0) {decbels = decbels * -1.0;}
			} else if (argv.at(i1).find("mt") == 0) {i1++; mclip = stof(argv.at(i1));}
			else if (argv.at(i1).find("smt") == 0) {
				i1++; buffx2 = stof(argv.at(i1));
				cb = true;
			} else if (argv.at(i1).find("-log") == 0) {logging = true;}
		}

		if (cb) {
			if (buffx2 < buff * 2) {
				cout << "Fatal error: minimun silence time is less than silence buffer" << endl;
				exit(4);
			}
		}
	}

	if (logging) {log("Start of [sr]");}

	cmd = "ffmpeg -i \"PATH\" -hide_banner -af silencedetect=noise=-DECBELSdB:d=0.03 -f null - 2>&1";
	cmd = replace(cmd, "DECBELS", to_string(decbels));
	cmd = replace(cmd, "PATH", path);
	reader();
}

inline void sorter() {
	if (fdur == false) {
		cout << "Fatal error: Duration not found.";
		exit(3);
	}

	if (not cb) {
    	buffx2 = buff * 2.0;
	}

	i3 = 0;
	for (i1 = 0; i1 < silarr.size(); i1 = i1 + 3) { // filter the silences by their duration
		temp = i1 + 2; // temp is the duration of the silence
		if (silarr.at(temp) > buffx2) { // if silence duration is enough, save it
			temp = i1 + 1; // temp is now the silence end
			tempsil.push_back(silarr.at(i1)); i3++; // save the silence start to the list
			tempsil.push_back(silarr.at(temp)); i3++; // save the silence end
		}
	}

	if (tempsil.size() <= 1) {
	    cout << "Fatal error: no clips after filtering for silence duration." << endl;
	    exit(4);
	}

	i3 = 0;
	i2 = 2;

	if (tempsil.at(0) == 0.0) { // if the first item here is 0, then the next needs to be the start of the first chain because 0 cannot be the end of a chain
		silarr.at(0) = tempsil.at(1);
	}
	else {
		silarr.at(0) = 0.0; // if it isnt 0, the start of the first chain needs to be 0
	}

	for (i1 = 1; i2 < tempsil.size(); i1++) { // filter everything else
		silarr.at(i1) = tempsil.at(i2); i3++;
		i2++;
	}

	if (silarr.at(i3) != secdur) { // if the last silence end is less than the dur, make the dur the end of the final clip. if not, the silence end is junk and wont be used.
		silarr.at(i1) = secdur; i3++; b1 = true;
	}

	for (i1 = silarr.size() + 1; i1 > i3 + 2; i1--) { // loop to remove excess data from silarr
		silarr.pop_back();
	}


	i2 = 0;
	if (not(b1)) {i2++; i2++;}
	i3 = 0;

	for (i1 = 0; i1 < silarr.size() - i2; i1 = i1 + 2) { // sort the clips by the their duration
		temp = i1 + 1;

		f1 = silarr.at(temp) - silarr.at(i1); // find the clip dur
		if (f1 > mclip) {
			tempsil.at(i3) = silarr.at(i1); i3++;
			tempsil.at(i3) = silarr.at(temp); i3++;

		}
	}

	for (i1 = tempsil.size(); i1 > i3; i1--) { // loop to remove excess data from tempsil
		tempsil.pop_back();
	}

	i3 = 0;

	if (tempsil.at(0) < buff) { // beginning thing for adding the buffer
		silarr.at(0) = 0.0; i3++;
		silarr.at(1) = tempsil.at(1) + buff; i3++;
	}

	for (i1 = i3; i1 < tempsil.size(); i1 = i1 + 2) { // add the buffer to all the rest
		temp = i1 + 1;
		silarr.at(i3) = tempsil.at(i1) - buff; i3++;
		silarr.at(i3) = tempsil.at(temp) + buff; i3++;
	}

	temp = i3 - 1;

	if (secdur - silarr.at(temp) < 0.0) {
		silarr.at(temp) = secdur;
	}

	for (i1 = silarr.size(); i1 > i3; i1--) {
		silarr.pop_back();
	}

	bool isbegin = true;

	for (i1 = 0; i1 < silarr.size(); i1++) { // the master program will look for clipstart or clipend, followed by a float representing a timestamp
		if (isbegin) {
			cout << "clipstart: ";
			isbegin = false;
			if (logging) {log("clipstart: ");}
		} else {
			cout << "clipend: ";
			isbegin = true;
			if (logging) {log("clipend: ");}
		}
		cout << silarr.at(i1);
		if (logging) {log(to_string(silarr.at(i1)));}
		if (not(i1 + 1 == silarr.size())) {cout << endl;}
	}

	if (logging) {log("end of [sr]");}

	exit(0);
}
