#include "common.hpp" // includes inside common.hpp

/*
this modification to [sr] was supposed to improve stability in unusual situations, aka one video that just wouldnt.
however, this is not working. i have no clue why, my best guess is that its to do with my compiler or library implementation.
itll be seen whether or not switching to a more stable distro will make a difference or not. also would like to try clang with
a different library implementation.
*/

using namespace std;

string bfflt, in, totlen, metaline3;
double buffx2; // minimum silence time
bool fdur = false, logging = false, cb = false; // custom buffer
long long int len;
double buff = 0.3;
double mclip = 0.5;
vector <clip> clparr;
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

	secdur = stod(totlen);

	// inmeta is for getting the silences

	FILE * inmeta = NULL;

	const char * realcmd = cmd.c_str();
	if (logging) {log("cmd: " + cmd);}
	inmeta = popen(realcmd, "r");
	b1 = false;
	thread pctrl(pcheck, ref(inmeta), ref(b1));
	while (not b1) {
		while (fgets(buf, 1024, inmeta) != NULL) {
			metaline2 = buf;
			clip tmpclp;
			c1 = metaline2.at(metaline2.size() - 1);
			if (not isnum(c1)) {
				metaline2.erase(metaline2.size() - 1, metaline2.size()); // erase newline character at the end of the string, if there is one
			}
			pos = -1;
			pos = metaline2.find("silence_start: "); // finds the start of the silence
			if (pos > 0) {
				pos = pos + 15;
				metaline2.erase(0, pos); // isolate the float value by erasing the rest of the string.
				tmpclp.start = stod(metaline2);
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
				tmpclp.end = stod(metaline2);
				tmpclp.sd();
				clparr.push_back(tmpclp);

				stringstream getper; // get percentage
				getper << fixed << setprecision(0) << stod(metaline2) / secdur * 100;
				getline(getper, s1);
				s1.append("%");
				cout << fixed << setprecision(3) << "reset: clipping: " << stod(metaline2) << "/" << secdur << " " << s1 << endl; // progress
			}
		}
		pclose(inmeta);
		inmeta = NULL;
	}
	if (clparr.size() == 0) {
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
			if (argv.at(i1).find("bf") == 0) {i1++; buff = stod(argv.at(i1));}
			else if (argv.at(i1).find("db") == 0) {
				i1++; decbels = stod(argv.at(i1));
				if (decbels < 0.0) {decbels = decbels * -1.0;}
			} else if (argv.at(i1).find("mt") == 0) {i1++; mclip = stod(argv.at(i1));}
			else if (argv.at(i1).find("smt") == 0) {
				i1++; buffx2 = stod(argv.at(i1));
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

	cmd = "ffmpeg -i \"PATH\" -hide_banner -nostats -af silencedetect=noise=-DECBELSdB:d=0.03 -f null - 2>&1";
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

	if (logging) {
		log("Silence array:");
		for (i1 = 0; i1 < clparr.size(); i1++) {
			log(to_string(clparr.at(i1).start) + " : " + to_string(clparr.at(i1).end));
		}
	}

	if (logging) {log("converting silences to clips");}
	if (clparr.at(0).start != 0.0) { // if the start of the first silence is not 0, the start of the first chain must be 0
		clip tmpclp;
		tmpclp.start = 0.0;
		tmpclp.end = clparr.at(0).start;
		clparr.insert(clparr.begin(), tmpclp);
		i1 = 1;
	} else {
		i1 = 0;
	}
	for (i1; i1 < clparr.size() - 2; i1++) {
		clparr.at(i1).start = clparr.at(i1).end;
		clparr.at(i1).end = clparr.at(i1 + 1).start;
	}
	if (clparr.at(++i1).end != secdur) { // if the end of the last silence is not the end of the video, the last chain must be the end of the video. if it is, it is discarded.
		clparr.at(i1).start = clparr.at(i1).end;
		clparr.at(i1).end = secdur;
	} else {
		clparr.pop_back();
	}
	if (clparr.size() <= 1) {
	    cout << "Fatal error: no clips after filtering for chains." << endl;
	    exit(4);
	}
	if (logging) {
		log("Converted to clip array:");
		for (i1 = 0; i1 < clparr.size(); i1++) {
			log(to_string(clparr.at(i1).start) + " : " + to_string(clparr.at(i1).end));
		}
	}

	if (logging) {
		log ("Merging clips");
		if (logging) {log(to_string(clparr.size()));}
	}
	for (i1 = 0; i1 < clparr.size() - 1; i1++) {
		while (i1 < clparr.size() - 1 and clparr.at(i1).end + buff > clparr.at(i1 + 1).start - buff) {
			clparr.at(i1 + 1).start = clparr.at(i1).start;
			auto it1 = clparr.begin();
			for (i3 = 0; i3 < i1; i3++) {it1++;}
			clparr.erase(it1);
		}
	}
	if (logging) {
		log("Merged clips array:");
		for (i1 = 0; i1 < clparr.size(); i1++) {
			log(to_string(clparr.at(i1).start) + " : " + to_string(clparr.at(i1).end));
		}
	}

	if (logging) {log("sorting clip durations");}
	for (i1 = 0; i1 < clparr.size(); i1++) { // if chains are too short, remove them
		clparr.at(i1).sd();
		if (clparr.at(i1).end - clparr.at(i1).start < mclip) {
			auto it1 = clparr.begin();
			for (i3 = 0; i3 < i1; i3++) {it1++;}
			clparr.erase(it1);
		}
	}
	if (clparr.size() <= 1) {
	    cout << "Fatal error: no clips after filtering for duration." << endl;
	    exit(4);
	}

	if (logging) {
		log("Sorted clip dur array:");
		for (i1 = 0; i1 < clparr.size(); i1++) {
			log(to_string(clparr.at(i1).start) + " : " + to_string(clparr.at(i1).end));
		}
	}

	if (logging) {log("adding buffer to clips");}
	if (clparr.at(0).start < buff) {
		clparr.at(0).start = 0.0;
		clparr.at(0).end += buff;
	} else {
		clparr.at(0).start -= buff;
		clparr.at(0).end += buff;
	}
	for (i1 = 1; i1 < clparr.size() - 2; i1++) {
		clparr.at(i1).start -= buff;
		clparr.at(i1).end += buff;
	}
	if (clparr.at(i1).end > secdur - buff) {
		clparr.at(i1).start -= buff;
		clparr.at(i1).end = secdur;
	} else {
		clparr.at(i1).start -= buff;
		clparr.at(i1).end += buff;
	}

	if (logging) {
		log("Added buf array (final):");
	}

	for (i1 = 0; i1 < clparr.size(); i1++) { // the master program will look for clipstart or clipend, followed by a float representing a timestamp
		clparr.at(i1).sd();
		if (clparr.at(i1).dur >= mclip and clparr.at(i1).end - clparr.at(i1).start >= mclip) {
			cout << "clipstart: ";
			cout << clparr.at(i1).start << endl;
			if (logging) {
				log("clipstart: ");
				log(to_string(clparr.at(i1).start));
			}
			cout << "clipend: ";
			cout << clparr.at(i1).end;
			if (logging) {
				log("clipend: ");
				log(to_string(clparr.at(i1).end));
			}
			if (not(i1 + 1 == clparr.size())) {cout << endl;}
		}
	}

	if (logging) {log("end of [sr]");}

	exit(0);
}
