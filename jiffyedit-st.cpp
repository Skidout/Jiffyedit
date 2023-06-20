#include "common.hpp" // includes inside common.hpp
#include <iomanip>

using namespace std;

float f3, fpsnum, fpsden, onefrm, dur = 2.0 / 30.0; // how often a new frame occurs in seconds
vector <string> argv;
bool logging = false;

inline float tonrfram(float tofrmvar) { // to nearest valid frame. shotcut needs timestamps to be frames but in a decimal format. weird
	f2 = 0.0;
	for (f1 = 2.0; f2 < tofrmvar; f1 = f1 + 1.0) {
		f2 = onefrm * f1;
	}

	f3 = f2 - onefrm;
	if (f3 < 0.0) {f3 = f3 * -1.0;}

	if (f2 < f3) {return f2;}
	else {return f3;}
}

inline string toanatim(float totimvar, int precision) { // to analogue time
	string result;
	int hrs = 0, mins = 0;
	float secs = 0.0, timecount = 0.0, postdot = totimvar;
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

	if (hrs < 10) {result = "0";}
	stringstream wrttim; // write time
	wrttim << result << hrs << ":";

	if (mins < 10) {wrttim << "0";}

	wrttim << mins << ":";

	if (secs < 10.0) {wrttim << "0";}

	wrttim << fixed << setprecision(precision) << secs;
	getline(wrttim, result);

	return result;
}

inline void pitivi() {
	if (argv.size() > 6) {
		s1 = argv.at(7);
		if (not (dur = stof(s1))) {
			cout << "Fatal error: incorrect usage of options." << endl;
			if (logging) {log("Fatal error: incorrect usage of options.");}
			exit(4);
		}
	}
	s1 = " property='alpha' mode='1' track_id='0' values =' START:0 STRTP#:1 ENM#:1 END:0 '/>";
	s1 = replace(s1, "START", ftog(stof(argv.at(3))));
	s1 = replace(s1, "STRTP#", ftog(stof(argv.at(3)) + dur));
	s1 = replace(s1, "ENM#", ftog(stof(argv.at(4)) - dur));
	cout << "-volume-control: " << replace(s1, "END", ftog(stof(argv.at(4)))) << endl;
	exit(0);
}

inline void shotcut() {
	cout << 2 << endl;
	if (argv.size() > 7) {
		s1 = argv.at(8);
		if (not (dur = stof(s1))) {
			cout << "Fatal error: incorrect usage of options." << endl;
			if (logging) {log("Fatal error: incorrect usage of options.");}
			exit(4);
		} else if (dur < onefrm * 2.0) {
			cout << "Fatal error: Shotcut fades cannot be less than two frames." << endl;
			if (logging) {log("Fatal error: Shotcut fades cannot be less than two frames.");}
			exit(4);
		}
	}
	s1 = "    <filter id=\"FLTNUM\" in=\"IN\" out=\"OUT\">";
	s1 = replace(s1, "FLTNUM", argv.at(6));
	s1 = replace(s1, "IN", toanatim(tonrfram(stof(argv.at(3))), 3)); // to nearest valid frame timestamp of the beginning of the clip with a precision of three
	cout << replace(s1, "OUT", toanatim(tonrfram(stof(argv.at(4))), 3)) << endl;
	cout << "      <property name=\"window\">75</property>" << endl;
	cout << "      <property name=\"max_gain\">20dB</property>" << endl;
	cout << replace("      <property name=\"level\">00:00:00.000=-60;OUT=0</property>", "OUT", toanatim(tonrfram(dur), 3)) << endl;
	cout << "      <property name=\"mlt_service\">volume</property>" << endl;
	cout << "      <property name=\"shotcut:filter\">fadeInVolume</property>" << endl;
	cout << replace("      <property name=\"shotcut:animIn\">TIME</property>", "TIME", toanatim(dur, 3)) << endl;
	cout << "    </filter>" << endl;
	argv.at(6) = to_string(stoi(argv.at(6)) + 1); // increment filter number
	s1 = "    <filter id=\"FLTNUM\" in=\"IN\" out=\"OUT\">";
	s1 = replace(s1, "FLTNUM", argv.at(6));
	s1 = replace(s1, "IN", toanatim(tonrfram(stof(argv.at(3))), 3)); // to nearest valid frame timestamp of the beginning of the clip with a precision of three
	cout << replace(s1, "OUT", toanatim(tonrfram(stof(argv.at(4))), 3)) << endl;
	cout << "      <property name=\"window\">75</property>" << endl;
	cout << "      <property name=\"max_gain\">20dB</property>" << endl;
	s1 = "      <property name=\"level\">IN=0;OUT=-60</property>";
	s1 = replace(s1, "IN", toanatim(tonrfram(stof(argv.at(4)) - stof(argv.at(3)) - dur + onefrm), 3));
	cout << replace(s1, "OUT", toanatim(tonrfram(stof(argv.at(4)) - stof(argv.at(3))), 3)) << endl;
	cout << "      <property name=\"mlt_service\">volume</property>" << endl;
	cout << "      <property name=\"shotcut:filter\">fadeOutVolume</property>" << endl;
	cout << replace("      <property name=\"shotcut:animOut\">TIME</property>", "TIME", toanatim(dur, 3)) << endl;
	cout << "    </filter>" << endl;
}

int main(int argc, const char * arga[]) {

	for (i1 = 0; i1 < argc; i1++) {
		argv.push_back(arga[i1]);
	}

	for (i1 = 0; i1 < argv.size(); i1++) {
		if (argv.at(i1).find("-log") == 0) {logging = true;}
	}

	path = argv.at(1);
	char buf[1025];

	string cmd3 = "ffprobe -v error -select_streams v -of default=noprint_wrappers=1:nokey=1 -show_entries stream=r_frame_rate \"PATH\""; // fetch fps
	cmd3 = replace(cmd3, "PATH", path);
	if (logging) {log("cmd3: " + cmd3);}
	const char * realcmd3 = cmd3.c_str();
	FILE * infps = NULL;
	if ((infps = popen(realcmd3, "r")) != NULL) {
		while (fgets(buf, 1024, infps) != NULL) {
			s1 = buf;
		}
		pclose(infps);
		infps = NULL;
	}

	s1 = replace(s1, "/", " ");
	stringstream getfps(s1);
	getfps >> fpsnum >> fpsden;

	f2 = fpsnum / fpsden; // find how often a new frame occurs. this more convoluted method is necessary for decimal framerates.
	onefrm = 1.0 / f2;
	dur = onefrm * 2.0;

	if (logging) {
		log("args: ");
		for (i1 = 0; i1 < argv.size(); i1++) {log(argv.at(i1));}
	}
	if (argv.at(2).find("shotcut") == 0) {shotcut();}
	else if (argv.at(2).find("pitivi") == 0) {pitivi();}
}
