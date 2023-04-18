#include <fstream> // other includes inside common.hpp
#include <algorithm>
#include <filesystem>

#include "common.hpp"

using namespace std;
namespace fs = std::filesystem; // this sets it so that you can use fs:: instead of filesystem::

string outpath, ismeta, totlen, width, progress = "\r", /* so that the progress is not reset to the beginning of the line if specified with -no-reset */ height, dur, /* full duration of clip including last frame */ durm1, /* the second to last frame (needed for some reason) */ clspc, /* colorspace */ aspwid, /* aspect width string */ asphei;
bool hasres = false, fheight = false /* ound height indicator */, fwidth = false, fclsp = false, shotcutb = false, pitivib = false, overwrite = false, losslessb = false, vidcutb = false, openshotb = false;
unsigned long long int aspwidi, /* aspect ratio width int */ aspheii, i4, vbsden, /* video base denominator */ vbsnum, absden, absnum, abr, /* audio bitrate */ vbr, chanls /* audio channel count */, clpint; // clipper int
float f3, fpsnum, fpsden, onefrm; // how often a new frame occurs in seconds
vector <float> clparr; // timestamps where 0 is begin, 1 is end, 2 is begin and so on

class plugin {
	public:
		bool filter; // false for clipper, true for filter
		string name /* front-facing name of the plugin */, exe /* full path to executable */, call /* front-facing call */, args = " "; // user specified arguments
		vector <string> help; // front-facing help section, defined by the plugin
};

vector <plugin> fltarr;
vector <plugin> plugls;
string aggcll; // aggregate calls, used to detect conflicting calls.

void shotcut();

float tonrfram(float tofrmvar) { // to nearest valid frame. shotcut needs timestamps to be frames but in a decimal format. weird
	f2 = 0.0;
	for (f1 = 2.0; f2 < tofrmvar; f1 = f1 + 1.0) {
		f2 = onefrm * f1;
	}

	f3 = f2 - onefrm;
	if (f3 < 0.0) {f3 = f3 * -1.0;}

	if (f2 < f3) {return f2;} 
	else {return f3;}
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

void openshot() {
	vector <string> out;
	
	cout << "Generating OSP file..." << endl;
	
	out.push_back("{");
	out.push_back(replace(" \"id\": \"PATH\",", "PATH", path));
	out.push_back(" \"fps\": {");
	out.push_back(replace("  \"num\": FPSNUM,", "FPSNUM", to_string(fpsnum)));
	out.push_back(replace("  \"den\": FPSDEN", "FPSDEN", to_string(fpsden)));
	out.push_back(" },\n \"display_ratio\": {");
	out.push_back(replace("  \"num\": ASPWID,", "ASPWID", aspwid));
	out.push_back(replace("  \"den\": ASPHEI", "ASPHEI", asphei));
	out.push_back(" },\n \"pixel_ratio\": {\n  \"num\": 1,\n  \"den\": 1\n },");
	out.push_back(replace(" \"width\": WIDTH,", "WIDTH", width));
	out.push_back(replace(" \"height\": HEIGHT,", "HEIGHT", height));
	out.push_back("\n \"settings\": {},\n \"clips\": [");
	
	float prolen = 0.0;
	int clpnum = 0;
	for (i1 = 0; i1 < clparr.size(); i1 = i1 + 2) {
		out.push_back("  {\n   \"alpha\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": 1.0\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },\n      \"handle_type\": 0,\n      \"interpolation\": 0\n     }\n    ]\n   },\n   \"anchor\": 0,\n   \"channel_filter\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": -1.0\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },\n      \"handle_type\": 0,\n      \"interpolation\": 0\n     }\n    ]\n   },\n   \"channel_mapping\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": -1.0\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },\n      \"handle_type\": 0,\n      \"interpolation\": 0\n     }\n    ]\n   },\n   \"display\": 0,");
		stringstream wrtdur;
		wrtdur << fixed << setprecision(14) << secdur;
		getline(wrtdur, s1);
		out.push_back(replace("   \"duration\": DUR,", "DUR", dur));
		
		out.push_back("   \"effects\": [],");
		
		stringstream wrtend;
		wrtend << fixed << setprecision(14) << clparr.at(i1 + 1);
		getline(wrtend, s1);
		out.push_back(replace("   \"end\": END,", "END", s1));
		
		out.push_back("   \"gravity\": 4,\n   \"has_audio\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": -1.0\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },      \"handle_type\": 0,\n      \"interpolation\": 0\n     }\n    ]\n   },\n   \"has_video\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": -1.0\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },\n      \"handle_type\": 0,\n      \"interpolation\": 0\n     }\n    ]\n   },");
		
		out.push_back(replace("   \"id\": \"ID\",", "ID", to_string(clpnum)));
		
		out.push_back("   \"layer\": 5000000,\n   \"location_x\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": 0.0\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },\n      \"handle_type\": 0,\n      \"interpolation\": 0\n     }\n    ]\n   },\n   \"location_y\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": 0.0\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },\n      \"handle_type\": 0,\n      \"interpolation\": 0\n     }\n    ]\n   },\n   \"mixing\": 0,\n   \"origin_x\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": 0.5\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },\n      \"handle_type\": 0,\n      \"interpolation\": 0\n     }\n    ]\n   },\n   \"origin_y\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": 0.5\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },\n      \"handle_type\": 0,\n      \"interpolation\": 0\n     }\n    ]\n   },\n   \"parentObjectId\": \"\",\n   \"perspective_c1_x\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": -1.0\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },\n      \"handle_type\": 0,\n      \"interpolation\": 0\n     }\n    ]\n   },\n   \"perspective_c1_y\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": -1.0\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },\n      \"handle_type\": 0,\n      \"interpolation\": 0\n     }\n    ]\n   },\n   \"perspective_c1_y\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": -1.0\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },\n      \"handle_type\": 0,"); // starting a new one here because IDE is acting up
		out.push_back("      \"interpolation\": 0\n     }\n    ]\n   },\n   \"perspective_c2_x\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": -1.0\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },\n      \"handle_type\": 0,\n      \"interpolation\": 0\n     }\n    ]\n   },\n   \"perspective_c2_y\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": -1.0\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },\n      \"handle_type\": 0,\n      \"interpolation\": 0\n     }\n    ]\n   },\n   \"perspective_c3_x\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": -1.0\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },\n      \"handle_type\": 0,\n      \"interpolation\": 0\n     }\n    ]\n   },\n   \"perspective_c3_y\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": -1.0\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },\n      \"handle_type\": 0,\n      \"interpolation\": 0\n     }\n    ]\n   },\n   \"perspective_c4_x\": {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": -1.0\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },\n      \"handle_type\": 0,\n      \"interpolation\": 0\n     }\n    ]\n   },\n   \"perspective_c4_y\":\n   {\n    \"Points\": [\n     {\n      \"co\": {\n       \"X\": 1.0,\n       \"Y\": -1.0\n      },\n      \"handle_left\": {\n       \"X\": 0.5,\n       \"Y\": 1.0\n      },\n      \"handle_right\": {\n       \"X\": 0.5,\n       \"Y\": 0.0\n      },");
		out.push_back("      \"interpolation\": 0\n     }\n    ]\n   },\n      \"handle_type\": 0,");
		
		out.push_back(replace("   \"position\": PROLEN,", "PROLEN", to_string(prolen)));
		
		out.push_back("   \"reader\": {");
		
		out.push_back(replace("    \"audio_bit_rate\": ABR,", "ABR", to_string(abr)));
		
		out.push_back("    \"audio_stream_index\": 1,\n    \"audio_timebase\": {");
		
		out.push_back(replace("     \"den\": ABSDEN,", "ABSDEN", to_string(absden)));
		out.push_back(replace("     \"num\": ABSNUM", "ABSNUM", to_string(absnum)));
		out.push_back("    },");
		if (chanls == 2) {
			out.push_back("    \"channel_layout\": 3,");
		} else if (chanls == 1) {
			out.push_back("    \"channel_layout\": 4,");
		} else {
			out.push_back("    \"channel_layout\": 63,");
		}
		out.push_back(replace("    \"channels\": CHANLS,", "CHANLS", to_string(chanls)));
		
		out.push_back("    \"display_ratio\": {");
    
		out.push_back(replace("     \"den\": ASPHEI,", "ASPHEI", asphei));
		out.push_back(replace("     \"num\": 16", "ASPWID", aspwid));
		
		out.push_back("    },");
		
		stringstream getdur;
		getdur << fixed << setprecision(14) << secdur;
		getline(getdur, s1);
		out.push_back(replace("    \"duration\": DUR,", "DUR", s1));
		out.push_back(replace("    \"file_size\": \"FSIZE\",", "FSIZE", to_string(fs::file_size(path))));

		out.push_back("    \"fps\": {");
		
		temp = fpsden;
		out.push_back(replace("     \"den\": FPSDEN,", "FPSDEN", to_string(temp)));
		temp = fpsnum;
		out.push_back(replace("     \"num\": FPSNUM", "FPSNUM", to_string(temp)));
		
		out.push_back("    },\n    \"has_audio\": true,\n    \"has_single_image\": false,\n    \"has_video\": true,");
		
		out.push_back(replace("    \"height\": HEIGHT,", "HEIGHT", height));
		/* openshot seems to have broken so ill leave this here just in case it getsd fixed in the future
        out.push_back("    "interlaced_frame": false,
    "metadata": {
     "compatible_brands": "isomiso2avc1mp41",
     "encoder": "Lavf59.27.100",
     "handler_name": "SoundHandler",
     "language": "und",
     "major_brand": "isom",
     "minor_version": "512"
    },
    "path": "./audiotest.mp4",
    "pixel_format": 0,
    "pixel_ratio": {
     "den": 1,
     "num": 1
    },
    "sample_rate": 48000,
    "top_field_first": true,
    "type": "FFmpegReader",
    "vcodec": "h264",
    "video_bit_rate": 1088632,
    "video_length": "1271",
    "video_stream_index": 0,
    "video_timebase": { ffprobe -v 0 -of compact=p=0:nk=1 -show_entries stream=time_base -select_streams v:0 INPUT
     "den": 30000000,
     "num": 1
    },
    "width": 1920
   },
   "rotation": {
    "Points": [
     {
      "co": {
       "X": 1.0,
       "Y": 0.0
      },
      "handle_left": {
       "X": 0.5,
       "Y": 1.0
      },
      "handle_right": {
       "X": 0.5,
       "Y": 0.0
      },
      "handle_type": 0,
      "interpolation": 0
     }
    ]
   },
   "scale": 1,
   "scale_x": {
    "Points": [
     {
      "co": {
       "X": 1.0,
       "Y": 1.0
      },
      "handle_left": {
       "X": 0.5,
       "Y": 1.0
      },
      "handle_right": {
       "X": 0.5,
       "Y": 0.0
      },
      "handle_type": 0,
      "interpolation": 0
     }
    ]
   },
   "scale_y": {
    "Points": [
     {
      "co": {
       "X": 1.0,
       "Y": 1.0
      },
      "handle_left": {
       "X": 0.5,
       "Y": 1.0
      },
      "handle_right": {
       "X": 0.5,
       "Y": 0.0
      },
      "handle_type": 0,
      "interpolation": 0
     }
    ]
   },
   "shear_x": {
    "Points": [
     {
      "co": {
       "X": 1.0,
       "Y": 0.0
      },
      "handle_left": {
       "X": 0.5,
       "Y": 1.0
      },
      "handle_right": {
       "X": 0.5,
       "Y": 0.0
      },
      "handle_type": 0,
      "interpolation": 0
     }
    ]
   },
   "shear_y": {
    "Points": [
     {
      "co": {
       "X": 1.0,
       "Y": 0.0
      },
      "handle_left": {
       "X": 0.5,
       "Y": 1.0
      },
      "handle_right": {
       "X": 0.5,
       "Y": 0.0
      },
      "handle_type": 0,
      "interpolation": 0
     }
    ]
   },
   "start": 0,
   "time": {
    "Points": [
     {
      "co": {
       "X": 1.0,
       "Y": 1.0
      },
      "handle_left": {
       "X": 0.5,
       "Y": 1.0
      },
      "handle_right": {
       "X": 0.5,
       "Y": 0.0
      },
      "handle_type": 0,
      "interpolation": 0
     }
    ]
   },
   "volume": {
    "Points": [
     {
      "co": {
       "X": 1.0,
       "Y": 1.0
      },
      "handle_left": {
       "X": 0.5,
       "Y": 1.0
      },
      "handle_right": {
       "X": 0.5,
       "Y": 0.0
      },
      "handle_type": 0,
      "interpolation": 0
     }
    ]
   },
   "wave_color": {
    "alpha": {
     "Points": [
      {
       "co": {
        "X": 1.0,
        "Y": 255.0
       },
       "handle_left": {
        "X": 0.5,
        "Y": 1.0
       },
       "handle_right": {
        "X": 0.5,
        "Y": 0.0
       },
       "handle_type": 0,
       "interpolation": 0
      }
     ]
    },
    "blue": {
     "Points": [
      {
       "co": {
        "X": 1.0,
        "Y": 255.0
       },
       "handle_left": {
        "X": 0.5,
        "Y": 1.0
       },
       "handle_right": {
        "X": 0.5,
        "Y": 0.0
       },
       "handle_type": 0,
       "interpolation": 0
      }
     ]
    },
    "green": {
     "Points": [
      {
       "co": {
        "X": 1.0,
        "Y": 123.0
       },
       "handle_left": {
        "X": 0.5,
        "Y": 1.0
       },
       "handle_right": {
        "X": 0.5,
        "Y": 0.0
       },
       "handle_type": 0,
       "interpolation": 0
      }
     ]
    },
    "red": {
     "Points": [
      {
       "co": {
        "X": 1.0,
        "Y": 0.0
       },
       "handle_left": {
        "X": 0.5,
        "Y": 1.0
       },
       "handle_right": {
        "X": 0.5,
        "Y": 0.0
       },
       "handle_type": 0,
       "interpolation": 0
      }
     ]
    }
   },
   "waveform": false,
   "file_id": "W9N9H4K71B",
   "title": "audiotest.mp4",
   "image": "@assets/thumbnail/W9N9H4K71B.png"
  },*/
		clpnum++;
	}
}

void vidcut() {
	vector <string> out;
	
	cout << "Generating VCP file..." << endl;
	
	if (fltarr.size() > 0) {
		cout << endl << "Warning: Vidcutter does not support filters." << endl;
	}
	
	out.push_back(path);
	for (i1 = 0; i1 < clparr.size(); i1 = i1 + 2) {
		s1 = "START	END	0	\"\"";
		s1 = replace(s1, "START", to_string(clparr.at(i1)));
		out.push_back(replace(s1, "END", to_string(clparr.at(i1 + 1))));
	}
	
	ofstream wrtvcp(outpath);
	for (i1 = 0; i1 < out.size(); i1++) {
		wrtvcp << out.at(i1) << endl;
	}
	wrtvcp.close();
	
	cout << "Done!" << endl;
	exit(0);
}

void lossless() {
	vector <string> out;
	
	if (fltarr.size() > 0) {
		cout << endl << "Warning: filters are not supported by losslesscut." << endl;
	}
	
	cout << "Writing LLC file..." << endl;
	
	out.push_back("{");
	out.push_back("  version: 1,");
	out.push_back(replace("  mediaFileName: 'PATH',", "PATH", path));
	out.push_back("  cutSegments: [");
	
	for (i1 = 0; i1 < clparr.size(); i1 = i1 + 2) {
		out.push_back("    {");
		out.push_back(replace("      start: START,", "START", to_string(clparr.at(i1))));
		out.push_back(replace("      end: END,", "END", to_string(clparr.at(i1 + 1))));
		out.push_back("      name: '',");
		out.push_back("    },");
	}

	out.push_back("  ],");
	out.push_back("}");

	ofstream wrtllc(outpath);
	for (i1 = 0; i1 < out.size(); i1++) {
		wrtllc << out.at(i1) << endl;
	}
	wrtllc.close();
	
	cout << "Done!" << endl;
	exit(0);
}

void pitivi() {
	vector <string> out;
	string path2 = path; // for pitivi's format requirment
	
	cout << "Generating XGES file..." << endl;
	
	b1 = true;
	while (b1) { // replace all spaces with %20 code
		pos = -1;
		pos = path2.find(" ");
		if (pos == -1) {
			b1 = false;
			break;
		} else {
			path2 = replace(path2, " ", "%20");
		}
	}
	
	out.push_back("<ges version=\'0.7\'>");
	out.push_back("  <project properties=\'properties;\' metadatas=\'metadatas, scaled_proxy_width=(int)0, scaled_proxy_height=(int)0, pitivi::title_safe_area_vertical=(double)0.80000000000000004, pitivi::title_safe_area_horizontal=(double)0.80000000000000004, pitivi::action_safe_area_vertical=(double)0.90000000000000002, pitivi::action_safe_area_horizontal=(double)0.90000000000000002, render-scale=(double)100;\'>");
	out.push_back("    <encoding-profiles>");
	out.push_back("      <encoding-profile name=\'pitivi-profile\' description=\'Pitivi encoding profile\' type=\'container\' preset-name=\'webmmux\' format=\'video/webm\' >");
	
	s1 = "        <stream-profile parent=\'pitivi-profile\' id=\'0\' type=\'video\' presence=\'0\' format='video/x-vp8, profile=(string){ 0, 1, 2, 3 }' restriction='video/x-raw, width=(int)WIDTH, height=(int)HEIGHT, framerate=(fraction)FPSNUM/FPSDEN, pixel-aspect-ratio=(fraction)1/1' />";
	s1 = replace(s1, "WIDTH", width);
	s1 = replace(s1, "HEIGHT", height);
	temp = fpsnum;
	s1 = replace(s1, "FPSNUM", to_string(temp));
	temp = fpsden;
	out.push_back(replace(s1, "FPSDEN", to_string(temp)));
	
	out.push_back("        <stream-profile parent=\'pitivi-profile\' id=\'1\' type=\'audio\' presence=\'0\' format='audio/x-vorbis, rate=(int)[ 1, 200000 ], channels=(int)[ 1, 255 ]' />");
	out.push_back("      </encoding-profile>");
	out.push_back("    </encoding-profiles>");
	out.push_back("    <ressources>");
	out.push_back(replace("      <asset id=\'file://PATH\' extractable-type-name=\'GESUriClip\' >", "PATH", path2));
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
	i2 = 0;
	cout << "Filtering: 0/" << clparr.size() / 2;
	for (i1 = 0; i1 < clparr.size(); i1 = i1 + 2) {
		s1 = "        <clip id=\'CLPNUM\' asset-id=\'file://PATH\' type-name='GESUriClip\' layer-priority=\'0\' track-types=\'6\' start=\'START\' duration=\'DUR\' inpoint=\'INPOINT\' rate=\'0\' properties=\'properties, name=(string)uriclipCLPNUM2;\' >";
		s1 = replace(s1, "CLPNUM", to_string(i2));
		s1 = replace(s1, "START", ftog(prolen));
		s1 = replace(s1, "DUR", ftog(clparr.at(i1 + 1) - clparr.at(i1)));
		s1 = replace(s1, "INPOINT", ftog(clparr.at(i1)));
		s1 = replace(s1, "CLPNUM2", to_string(i2));
		s1 = replace(s1, "PATH", path2);
		out.push_back(s1);
		
		f2 = clparr.at(i1 + 1) - clparr.at(i1);
		prolen = prolen + f2;
		
		bool audfltb;
		bool vidfltb;
		string audflt; // audio and video filters for the special ones that need it
		string vidflt;

		for (i2 = 0; i2 < fltarr.size(); i2++) {
			string fltcmd = "\""; 
			fltcmd.append(fltarr.at(i2).exe);
			fltcmd.append("\" \"PATH\" ");
			fltcmd = replace(fltcmd, "PATH", path);
			fltcmd.append("pitivi ");
			fltcmd.append(to_string(clparr.at(i1)));
			fltcmd.append(" ");
			fltcmd.append(to_string(clparr.at(i1 + 1)));
			fltcmd.append(" ");
			fltcmd.append(to_string(i1));
			fltcmd.append("/");
			fltcmd.append(to_string(clparr.size() - 1));
			fltcmd.append(" ");
			fltcmd.append(fltarr.at(i2).args);

			const char * realfltcmd = fltcmd.c_str();

			FILE * inflts = NULL;
			char buf[1025];
			
			audfltb = false;
			vidfltb = false;

			if ((inflts = popen(realfltcmd, "r")) != NULL) {

				while (fgets(buf, 1024, inflts) != NULL) {
					s1 = buf;
					if (s1.find("-volume-control: ") == 0) {
						s1.erase(0, 17);			
						audflt = s1;
						c1 = audflt.at(audflt.size() - 1);
						if (c1 == '\n') {audflt.pop_back();} // remove newline if present
						audfltb = true;
					} else if  (s1.find("-alpha-control: ") == 0) {
						s1.erase(0, 16);
						vidflt = s1;
						c1 = audflt.at(audflt.size() - 1);
						if (c1 == '\n') {audflt.pop_back();} // remove newline if present
						vidfltb = true;
					} else if (s1.find("Fatal error: ") == 0) {
						cout << endl << s1 << " (from filterer: " << fltarr.at(i2).name << " with call: " << fltarr.at(i2).call << ")" << endl;
						exit(6);
					} else {
						c1 = s1.at(s1.size() - 1);
						if (c1 == '\n') {s1.pop_back();} // remove newline if present
						out.push_back(s1);
					}
				}
				
				pclose(inflts);
				inflts = NULL;
			}
		}
		
		out.push_back("          <source track-id=\'0\' >"); // video track
		if (not vidfltb) {
			s1 = "            <binding type='direct' source_type='interpolation' property='alpha' mode='1' track_id='0' values =' START:1  END:1 '/>";
			s1 = replace(s1, "START", ftog(clparr.at(i1)));
			s1 = replace(s1, "END", ftog(clparr.at(i1 + 1)));
		} else {
			s1 = "            <binding type='direct' source_type='interpolation' />";
			s1.insert(s1.size() - 3, vidflt);
		}
		out.push_back(s1);
		
		out.push_back("          </source>");
		out.push_back("          <source track-id='1' >"); // audio track
		
		if (not audfltb) {
			s1 = "            <binding type='direct' source_type='interpolation' property='volume' mode='1' track_id='1' values =' START:0.10000000149011612  END:0.10000000149011612 '/>";
			s1 = replace(s1, "START", ftog(clparr.at(i1)));
			s1 = replace(s1, "END", ftog(clparr.at(i1 + 1)));
		} else {
			s1 = "            <binding type='direct' source_type='interpolation' />";
			s1.insert(s1.size() - 3, audflt);
		}
		out.push_back(s1);
		
		out.push_back("          </source>");
		out.push_back("        </clip>");
		
		i2++;
		cout << progress << "Filtering: " << i1 / 2 + 1 << "/" << clparr.size() / 2;
	}
	
	out.push_back("      </layer>");
	out.push_back("      <groups>");
	out.push_back("      </groups>");
	out.push_back("    </timeline>");
	out.push_back("  </project>");
	out.push_back("</ges>");
	
	ofstream wrtxges(outpath);
	for (i1 = 0; i1 < out.size(); i1++) {
		wrtxges << out.at(i1) << endl;
	}
	wrtxges.close();
	
	cout << endl << "Done!" << endl;
	exit(0);
}

void reader() {

	/// inmeta is for getting info about the video
	// pipes in c++ seem to be horribly documented, so i'll do my best to explain it here. if i say i think something, it means im not entirely sure what exactly it does, so this is just my best guess from what little legible code and documentation i have been able to find
	FILE * inmeta = NULL; // so this here is creating a file pointer which will later become a datastream. according to cppreference.com (or somewhere similar), this type of stream is not meant to be accessed by any functions outside of the <stdio.h> library. Using functions from fstream would probably work, but i'm following their advice, just to be safe.
	char buf[1025]; // this is just a char array because its safer with C functions, i guess? strings would probably work, but again, better to be safe.

	const char * realcmd = cmd.c_str();
	cout << "Gathering metadata..." << endl;
	inmeta = popen(realcmd, "r"); // this tries to open a pipe (datastream) with ffmpeg for silencedetect. the first variable inside is literally a command, just like one you would run in a terminal or command line. the second variable, "r", indicates that we want to read data from this datastream.
	if (inmeta != NULL) {
		while (fgets(buf, 1024, inmeta) != NULL) { // this loop sets buf = the current line of the datastream. fgets is for plaintext data only. the 1024 specifies to read a max of 1024 characters, so as not to overflow (at least i think so anyway). lastly, the datastream to read from is specified. the != null makes it so that when the datastream reaches its end, the loop ends, so as to prevent overflow
			metaline = buf; /// so for some reason i cant seem to add lines piped in from this specific command to an array of strings, which was originally how i was planning on doing it, so if you see any references to a string raw[], now you know why
			
			if (metaline.find("Stream") >= 0) {
				for (i2 = 0; i2 < metaline.size(); i2++) { /// find resolution by scanning for three numbers in a row, then an 'x', then another three numbers. needs to stay because ffprobe is being weird
					if (hasres) {break;}
					if (isnum(metaline.at(i2)) and isnum(metaline.at(i2 + 1)) and isnum(metaline.at(i2 + 2)) and metaline.at(i2 + 3) == 'x' and isnum(metaline.at(i2 + 4)) and isnum(metaline.at(i2 + 5)) and isnum(metaline.at(i2 + 6))) {

						hasres = true; /// resolution found
						i3 = i2 + 4; /// should be the first number of the height
						while (isnum(metaline.at(i3)) and i3 < metaline.size() and fheight == false) { // save every number after the 'x' until it finds a character that is not a number (should be ' ')
							height.push_back(metaline.at(i3)); i3++;
						}
						fheight = true;

						i3 = i2 + 2; /// should be the last number of width
						while (isnum(metaline.at(i3)) and i3 > 0 and fwidth == false) { /// save everything before after the 'x' until it finds a character that is not a number (should be ' ')
							width.push_back(metaline.at(i3)); i3--;
						}
						fwidth = true;
						reverse(width.begin(), width.end());
					}
				}
			}
			if (metaline.find("Stream") >= 0) {
				if (metaline.find("709") >= 0 and fclsp == false) {clspc = "709"; fclsp = true;}
				if (metaline.find("601") >= 0 and fclsp == false) {clspc = "601"; fclsp = true;}
			}

			metaline2 = buf;
			if (not isnum(metaline2.at(metaline2.size() - 1))) {
				metaline2.erase(metaline2.size() - 1, metaline2.size()); /// erase newline character at the end of the string, if there is one
			}
			if (fheight and fwidth and fclsp) {break;}
		}
	}

	pclose(inmeta); // closes the pipe after the end of the datastream
	inmeta = NULL; // clears the datastream from memory (i think)

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

	secdur = stof(totlen);
	
	string cmd3 = "ffprobe -v error -select_streams v -of default=noprint_wrappers=1:nokey=1 -show_entries stream=r_frame_rate \"PATH\""; // fetch fps
	cmd3 = replace(cmd3, "PATH", path);

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
	string cmd4 = "\""; // take clips in
	plugls.at(clpint).exe.insert(0, "/");
	plugls.at(clpint).exe.insert(0, fs::current_path().generic_string());
	cmd4.append(plugls.at(clpint).exe);
	cmd4.append("\" \"PATH\"");
	cmd4 = replace(cmd4, "PATH", path);
	cmd4.append(" ");
	cmd4.append(plugls.at(clpint).args);

	const char * realcmd4 = cmd4.c_str();

	FILE * inclps = NULL;
	
	b1 = false; // b1 here is used to store whether the last timestamp was a clipstart or clipend. true = clipstart, false = clipend
	cout << "Clipping..." << endl;
	if ((inclps = popen(realcmd4, "r")) != NULL) {
		while (fgets(buf, 1024, inclps) != NULL) {
			s1 = buf;
			if (s1.find("clipstart: ") == 0) { 
				if (b1 == true) {
					cout << endl << "Fatal error: Two clipstarts before clipend from clipper: " << plugls.at(clpint).name << endl;
					exit(6);
				}
				s1.erase(0, 10);
				
				f2 = stof(s1);
				
				clparr.push_back(f2);
				b1 = true;
			} else if (s1.find("clipend: ") == 0) {
				if (b1 == false) {
					cout << endl << "Fatal error: Two clipends before clipstart from clipper: " << plugls.at(clpint).name << endl;
					exit(6);
				}
				s1.erase(0, 8);
				
				f2 = stof(s1);
				
				clparr.push_back(f2);
				b1 = false;
			} else if (s1.find("Fatal error: ") == 0) {
				cout << endl << s1 << " (from clipper: " << plugls.at(clpint).name << " with call: " << plugls.at(clpint).call << ")" << endl;
				exit(6);
			} else {
				cout << s1 << endl;
			}
		}
		if (b1) {
			cout << endl << "Fatal error: Final clipstart with no matching clipend from clipper: " << plugls.at(clpint).name << endl;
			exit(6);
		}
		pclose(inclps);
		inclps = NULL;
	}

	if (clparr.size() == 0) {
		cout << "Fatal error: No clips from clipper: " << plugls.at(clpint).name << " with call: " << plugls.at(clpint).call << endl;
		exit(6);
	}
	
	FILE * inchan;
	
	string cmd5 = replace("ffprobe -v error -show_entries stream=channel_layout -of csv=p=0 PATH", "PATH", path);
	
	s1 = "";
	if ((inchan = popen(cmd5.c_str(), "r"))) {
		while (fgets(buf, 1024, inchan)) {
			s1.append(buf);
			s1.pop_back();
		}
	}
	pclose(inchan);
	if (s1.find("5.1") == 0) {chanls = 6;}
	else if (s1.find("stereo") == 0) {chanls = 2;} 
	else if (s1.find("mono") == 0) {chanls = 1;} 
	else {chanls = 6;}
	
	FILE * invbse; // get video timebase
	
	s1 = replace("ffprobe -v 0 -of compact=p=0:nk=1 -show_entries stream=time_base -select_streams v:0 PATH", "PATH", path);
	
	if ((invbse = popen(s1.c_str(), "r"))) {
		while (fgets(buf, 1024, invbse)) {
			s1 = buf;
		}
	}
	pclose(invbse);
	
	stringstream getvbs;
	getvbs << replace(s1, "/", " ");
	getvbs >> vbsnum >> vbsden;
	
	FILE * inabse; // get audio timebase
	
	s1 = replace("ffprobe -v 0 -of compact=p=0:nk=1 -show_entries stream=time_base -select_streams a:0 PATH", "PATH", path);
	
	if ((inabse = popen(s1.c_str(), "r"))) {
		while (fgets(buf, 1024, inabse)) {
			s1 = buf;
		}
	}
	pclose(inabse);
	
	FILE * invbr; // get video bitrate
	
	s1 = replace("ffprobe -v error -select_streams v:0 -show_entries stream=bit_rate -of default=noprint_wrappers=1:nokey=1 PATH", "PATH", path);
	
	if ((invbr = popen(s1.c_str(), "r"))) {
		while (fgets(buf, 1024, invbr)) {
			vbr = stoi(buf);
		}
	}
	pclose(invbr);
	
	FILE * inabr; // get audio bitrate
	
	s1 = replace("ffprobe -v error -select_streams a:0 -show_entries stream=bit_rate -of default=noprint_wrappers=1:nokey=1 PATH", "PATH", path);
	
	if ((inabr = popen(s1.c_str(), "r"))) {
		while (fgets(buf, 1024, inabr)) {
			abr = stoi(buf);
		}
	}
	pclose(inabr);

	stringstream getabs;
	getabs << replace(s1, "/", " ");
	getabs >> absnum >> absden;
	
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
	else if (pitivib) {pitivi();}
	else if (losslessb) {lossless();}
	else if (vidcutb) {vidcut();}
	else if (openshotb) {openshot();}
}

int main(int argc, const char * arga[]) {

	vector <string> argv;
	
	for (i1 = 0; i1 < argc; i1++) { // convert arguments to safe format
		argv.push_back(arga[i1]);
	}
	
	for (i1 = 0; i1 < argv.size(); i1++) { // get the full path of the input
		if (argv.at(i1).find("-i") == 0) {
			i1++;
			if (argv.at(i1).find("/") == 0) {break;}
			else if (argv.at(i1).find(":/") == 1) {break;} // windows full paths
			else if (argv.at(i1).find("./") == 0) {
				argv.at(i1).erase(0, 1);
				
				s1 = fs::current_path().generic_string();
	            
				c1 = s1.at(0);
				if (c1 == '\'') {path.erase(0, 1);} // remove quotation marks sorrounding path, if any
				else if (c1 == '\"') {path.erase(0, 1);}
				reverse(path.begin(), path.end());
	
				c1 = s1.at(0);
				if (c1 == '\'') {path.erase(0, 1);}
				else if (c1 == '\"') {path.erase(0, 1);}
				reverse(path.begin(), path.end());
				argv.at(i1).insert(0, s1);
				break;
			} else if (argv.at(i1).find("~/") == 0) {
				argv.at(i1).erase(0, 1);
				
				FILE * inuser; // get user directory
				
				char buf[1025];
				
				if ((inuser = popen("echo ~", "r"))) {
					while (fgets(buf, 1024, inuser)) {
						argv.at(i1).insert(0, buf);
					}
				}
				break;
			} else {
				s1 = fs::current_path().generic_string();
	            
				c1 = s1.at(0);
				if (c1 == '\'') {path.erase(0, 1);} // remove quotation marks sorrounding path, if any
				else if (c1 == '\"') {path.erase(0, 1);}
				reverse(path.begin(), path.end());
	
				c1 = s1.at(0);
				if (c1 == '\'') {path.erase(0, 1);}
				else if (c1 == '\"') {path.erase(0, 1);}
				reverse(path.begin(), path.end());
				argv.at(i1).insert(0, "/");
				argv.at(i1).insert(0, s1);
				break;
			}
		}
	}
	
	cout << argv.at(i1) << endl;
	
	#ifdef __linux__
        fs::path wrkdir("/usr/lib/jiffyedit"); // working directory for linux
	#endif // working directories for other OS' can be added later
	
	fs::current_path(wrkdir); // set working directory
	
	cout << fs::current_path() << endl;

	vector <string> ls; 
	stringstream getls; 
	unsigned long long int datcnt = 0;

	for (auto& p: fs::directory_iterator(fs::current_path())) { // get a list of every file in the current directory, then sort them to .dat files only
		getls << p << endl;
		getline(getls, s1);
		ls.push_back(s1); 
	}

	for (i4 = 0; i4 < ls.size(); i4++) {
		char c1 = ls.at(i4).at(0); 

		if (c1 == '\'') { // remove quotation marks, if any
			ls.at(i4).erase(0, 1);
		} else if (c1 == '\"') {
			ls.at(i4).erase(0, 1);
		}
		reverse(ls.at(i4).begin(), ls.at(i4).end()); 

        if (c1 == '\'') {
			ls.at(i4).erase(0, 1);
		} else if (c1 == '\"') {
			ls.at(i4).erase(0, 1);
		} // path will be reversed again later, just need to check for .dat files first

        if (ls.at(i4).find("tad.") == 0) { // make list of only .dat files
            ls.at(datcnt) = ls.at(i4);
            reverse(ls.at(datcnt).begin(), ls.at(datcnt).end()); datcnt++;
        }
    }

	for (i4 = ls.size(); i4 > datcnt; i4--) {
		ls.pop_back();
	}

	for (i4 = 0; i4 < ls.size(); i4++) {
		ifstream getplgs(ls.at(i4));
		string temptype;
		getline(getplgs, temptype);
		if (temptype.find("ignore") >= 0) {
			if (temptype.find("clipper") == 0) { // im using string.find() as my string ==
				plugin tmpclp;
				tmpclp.filter = false;
				getline(getplgs, tmpclp.name);
        		getline(getplgs, tmpclp.exe);
        		getline(getplgs, tmpclp.call);
        		pos = -1;
        		pos = aggcll.find(tmpclp.call);
        		if (pos >= 0) {
        			cout << "Warning: conflicting call for \"" << tmpclp.name << "(" << tmpclp.call << ")" << endl;
        		} else {
        			aggcll.append(tmpclp.call);
        			while (getplgs and not (s1.find(tmpclp.call) == 0)) {
						getline(getplgs, s1);
						tmpclp.help.push_back(s1);
					}
					plugls.push_back(tmpclp);
        		}
    		} else if (temptype.find("filterer") == 0) {
				plugin tmpflt;
				tmpflt.filter = true;
				getline(getplgs, tmpflt.name);
        		getline(getplgs, tmpflt.exe);    
        		getline(getplgs, tmpflt.call);
        		
        		pos = -1;
        		pos = aggcll.find(tmpflt.call);
        		if (pos >= 0) {
					cout << "Warning: conflicting call for \"" << tmpflt.name << "(" << tmpflt.call << ")" << endl;
				} else {
					aggcll.append(tmpflt.call);
					while (getplgs and not (s1.find(tmpflt.call) == 0)) {
						getline(getplgs, s1);
						tmpflt.help.push_back(s1);
					}
					plugls.push_back(tmpflt);
				}	
			} else {cout << endl << "Warning: Potentially invalid .dat file at " << ls.at(i4) << endl;}
		}
	}

	for (i4 = 0; i4 < plugls.size(); i4++) { // make sure that executable for clipper exists
		s1 = fs::current_path().generic_string();
		reverse(s1.begin(), s1.end());

		s2 = plugls.at(i4).exe;
		reverse(s2.begin(), s2.end());

		s1.insert(0, "/");
		s1.insert(0, s2);
		reverse(s1.begin(), s1.end());

		fs::path tempath(s1);

		if (not fs::exists(tempath)) {
			cout << "Warning: Executable does not exist for " << plugls.at(i4).name << " with call: " << plugls.at(i4).call << endl << "You cannot use " << plugls.at(i4).name << endl;
			auto iter = plugls.begin();
			for (i2 = 0; i2 < i4; i2++) { // convert int to iterator
				iter++;
			}
			plugls.erase(iter);
		} 
	}
	
	if (argv.size() < 2) {
		cout << "Fatal error: invalid input. Usage:" << endl;
		cout << endl << "First, enter the file path. ex. -i \"/home/user/Videos/myvid.mp4\" Then, enter one of the options listed below and select an editor;" << endl;
		for (i4 = 0; i4 < plugls.size(); i4++) {
			cout << '	' << plugls.at(i4).name << ' ' << plugls.at(i4).call << endl;
			for (i1 = 0; i1 < size(plugls.at(i4).help); i1++) {
				cout << "		";
				if (i1 == size(plugls.at(i4).help) - 1) {
					cout << "Example command: ";
				}
				cout << plugls.at(i4).help.at(i1) << endl;
			}
		}
		cout << "Currently the editors available are;" << endl;
		cout << "	shotcut" << endl << "	pitivi" << endl << "	losslesscut" << endl << "	vidcutter" << endl;
		exit(4);
	}
	
	bool fclipr = false, finput = false; // found clipper, found input
	string tempargs;
	for (i1 = 1; i1 < argv.size(); i1++) {
		c1 = argv.at(i1).at(0); // for checking for [ and - later
		if (argv.at(i1).find("--help") == 0 or argv.at(i1).at(0) == '?' or argv.at(i1).find("-help") == 0 or (argv.at(i1).find("-h") == 0 and argv.at(i1).size() < 3)) {
			cout << endl << "First, enter the file path. ex. -i \"/home/user/Videos/myvid.mp4\" Then, enter one of the options listed below and select an editor;" << endl;
			for (i4 = 0; i4 < plugls.size(); i4++) {
				cout << '	' << plugls.at(i4).name << ' ' << plugls.at(i4).call << endl;
				for (i1 = 0; i1 < size(plugls.at(i4).help); i1++) {
					cout << "		";
					if (i1 == size(plugls.at(i4).help) - 1) {
						cout << "Example command: ";
					}
				cout << plugls.at(i4).help.at(i1) << endl;
			}
		}
		cout << "Currently the editors available are;" << endl;
		cout << "	shotcut" << endl << "	pitivi" << endl << "	losslesscut" << endl << "	vidcutter" << endl;
		exit(0);
		} else if (argv.at(i1).find("-i") == 0) {
			i1++; path = argv.at(i1); // get the file path after "-i"
			finput = true;
		} else if (c1 == '[') {
			c1 = argv.at(i1).at(argv.at(i1).size() - 1);
			if (c1 == ']') {
				s1 = argv.at(i1);
				if (not (aggcll.find(s1) >= 0)) {
					cout << "Fatal error: unrecognized call: " << s1 << endl;
					exit(4);
				}
				for (i2 = 0; i2 < plugls.size(); i2++) {
					if (plugls.at(i2).call.find(argv.at(i1)) == 0 and not plugls.at(i2).filter) {
						if (fclipr) {
							cout << "Fatal error: two clippers selected: " << argv.at(i1) << " & " << plugls.at(clpint).call << endl;
							exit(4);
						}
						clpint = i2;
						fclipr = true;
						break;
					} else if (plugls.at(i2).call.find(argv.at(i1)) == 0 and plugls.at(i2).filter) {
						fltarr.push_back(plugls.at(i2));
					}
				}
			} else {
				s1 = argv.at(i1);
				s1.append("]");
				if (not (aggcll.find(s1) >= 0)) {
					cout << "Fatal error: unrecognized call: " << s1 << endl;
					exit(4);
				}
				
				for (i2 = 0; i2 < plugls.size(); i2++) {
					if (plugls.at(i2).call.find(s1) == 0 and not plugls.at(i2).filter) {
						if (fclipr) {
							cout << "Fatal error: two clippers selected: " << argv.at(i1) << " & " << plugls.at(clpint).call << endl;
							exit(4);
						}
						clpint = i2;
						fclipr = true;
						for (i1 = i1 + 1; i1 < argv.size(); i1++) { // i1 has to be used again here to increment the arguments
							c1 = argv.at(i1).at(0);
							if (not(c1 == ']')) {
								plugls.at(clpint).args.append(argv.at(i1));
								plugls.at(clpint).args.append(" ");
							} else {break;}
						}
						break;
					} else if (plugls.at(i2).call.find(s1) == 0 and plugls.at(i2).filter) {
						for (i1 = i1 + 1; i1 < argv.size(); i1++) { // i1 has to be used again here to increment the arguments
							c1 = argv.at(i1).at(0);
							if (not(c1 == ']')) {
								plugls.at(i2).args.append(argv.at(i1));
								plugls.at(i2).args.append(" ");
							} else {break;}
						}
						fltarr.push_back(plugls.at(i2));
						break;
					}
				}
			}
		} else if (c1 == '-') {
			if (argv.at(i1).find("-overwrite") == 0) {overwrite = true;}
			else if (argv.at(i1).find("-no-reset") == 0) {progress = "\n";}
			else {
				cout << "Fatal error: unrecognized option: " << argv.at(i1) << endl;
				exit(4);
			}
		} else if (argv.at(i1).find("shotcut") == 0) {shotcutb = true;}
		else if (argv.at(i1).find("pitivi") == 0) {pitivib = true;}
		else if (argv.at(i1).find("losslesscut") == 0) {losslessb = true;}
		else if (argv.at(i1).find("vidcutter") == 0) {vidcutb = true;}
	}
	if (not fclipr) {
		cout << "Fatal error: clipper not found." << endl;
		exit(4);
	}
	if (not shotcutb and not pitivib and not losslessb and not vidcutb) {
		cout << "Fatal error: no editor selected." << endl;
		exit(4);
	}
	if (not finput) {
		cout << "Fatal error: no input found." << endl;
	}
	
	c1 = path.at(0);
	if (c1 == '\'') {path.erase(0, 1);} // remove quotation marks sorrounding path, if any
	else if (c1 == '\"') {path.erase(0, 1);}
	reverse(path.begin(), path.end());
	
	c1 = path.at(0);
	if (c1 == '\'') {path.erase(0, 1);}
	else if (c1 == '\"') {path.erase(0, 1);}
	reverse(path.begin(), path.end());

	// so this here is also terribly documented online. i will explain this in detail as well.
	// look up top for namespace stuff
	fs::path vidpath(path); // this is declaring what path you want to check
	if (not fs::exists(vidpath)) { // fs::exists will return a bool, true if it exists, false if not.
		cout << endl << "Fatal error: Video file does not exist!" << endl;
		exit(4);
	}

	outpath = path; // get the path of the resulting file
	reverse(outpath.begin(), outpath.end());
	pos = -1;
	pos = outpath.find(".");
	outpath.erase(0, pos);
	if (shotcutb) {
		s1 = "tlm";
	}
	if (pitivib) {
		s1 = "segx";
	}
	if (losslessb) {
		outpath.erase(0, 1);
		s1 = "cll.jorp-";
	}
	if (vidcutb) {
		s1 = "pcv";
	}
	s1.append(outpath);
	outpath = s1;
	reverse(outpath.begin(), outpath.end());

	fs::path xmlpath(outpath);
	if (fs::exists(xmlpath) and not overwrite) {
		cout << endl << outpath << " already exists. Continuing will overwrite it. Continue? (Y/n): ";
		getline(cin, s1);
		c1 = s1[0];
		if (c1 == 'y' or c1 == 'Y') {
			cout << endl << "Continuing..." << endl;
		} else {
			cout << endl << "Program aborted by user." << endl;
			exit(0);
		}
	}

	for (i2 = 0; i2 < fltarr.size(); i2++) {
		fltarr.at(i2).exe.insert(0, "/");
		fltarr.at(i2).exe.insert(0, fs::current_path().generic_string());
	}

	cmd = "ffmpeg -i \"PATH\" -hide_banner 2>&1";
	cmd = replace(cmd, "PATH", path);
	reader();
}

void shotcut() { // Shotcut/MLT XML format
	f2 = fpsnum / fpsden; // find how often a new frame occurs. this more convoluted method is necessary for decimal framerates.
	onefrm = 1.0 / f2;

	secdur = tonrfram(secdur);

	cout << "Rounding timestamps..." << endl;

	for (i2 = 0; i2 < clparr.size(); i2++) { // round all the silence values to the nearest frame
		clparr.at(i2) = tonrfram(clparr.at(i2));
	}


	cout << "Generating MLT XML file..." << endl;

	vector <string> fin; // final list to write because shotcut wants total time before it can be calculated
	vector <string> out; // holds the chains
	vector <string> playlist; // the playlist section goes directly after the chains and lists where each of the chains begins and ends


	fin.push_back("<?xml version=\"1.0\" standalone=\"no\"?>"); // All strings put directly into the output do not seem to change regardless of the video in question.
	fin.push_back("<mlt LC_NUMERIC=\"C\" version=\"7.8.0\" title=\"Shotcut version 22.09.23\" producer=\"main_bin\">");

	s1 = "  <profile description=\"automatic\" width=\"WIDTH\" height=\"HEIGHT\" progressive=\"1\" sample_aspect_num=\"1\" sample_aspect_den=\"1\" display_aspect_num=\"ASPWID\" display_aspect_den=\"ASPHEI\" frame_rate_num=\"FPSNUM\" frame_rate_den=\"FPSDEN\" colorspace=\"CLSPC\"/>";

	s1 = replace(s1, "WIDTH", width);
	s1 = replace(s1, "HEIGHT", height);
	s1 = replace(s1, "ASPWID", aspwid);
	s1 = replace(s1, "ASPHEI", asphei);
	s1 = replace(s1, "FPSNUM", to_string(fpsnum));
	s1 = replace(s1, "FPSDEN", to_string(fpsden));
	s1 = replace(s1, "CLSPC", clspc);

	fin.push_back(s1);

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

	cout << "Filtering: 0/" << clparr.size() / 2;
	i1 = 0;
	long long unsigned int fltnum = 0;
	for (i4 = 0; i4 < clparr.size(); i4 = i4 + 2) {
		string cnum = "chain"; // chain num
		cnum.append(to_string(i1));

		s2 = "  <chain id=\"CHAIN\" out=\"DURM1\">";  // here the chain id gets replaced and then the durations after it on both strings
		s2 = replace(s2, "CHAIN", cnum);
		out.push_back(replace(s2, "DURM1", durm1));

			s1 = "    <entry producer=\"CHAIN\" in=\"IN\" out=\"OUT\"/>";  // everything that is indented here is to do with the playlist, just to make things clearer
			s1 = replace(s1, "CHAIN", cnum);

			temp = i4 + 1;
			s1 = replace(s1, "IN", toanatim(clparr.at(i4), 3)); // replace "IN" with the beginning of the clip
			playlist.push_back(replace(s1, "OUT", toanatim(clparr.at(temp), 3))); // replace "OUT" with the end of the clip

			f2 = clparr.at(temp) - clparr.at(i4); // get the length of the clip by subtracting the beginning from the end

			prolen = prolen + f2; // add the duration of the clip to the aggregate project length

		s1 = "    <property name=\"length\">DUR</property>";
		out.push_back(replace(s1, "DUR", dur));

		out.push_back("    <property name=\"eof\">pause</property>");

		s1 = "    <property name=\"resource\">PATH</property>";
		out.push_back(replace(s1, "PATH", path));

		out.push_back("    <property name=\"mlt_service\">avformat-novalidate</property>");
		out.push_back("    <property name=\"seekable\">1</property>");
		out.push_back("    <property name=\"audio_index\">1</property>");
		out.push_back("    <property name=\"video_index\">0</property>");
		out.push_back("    <property name=\"mute_on_pause\">0</property>");
		out.push_back("    <property name=\"ignore_points\">0</property>");

		s1 = "    <property name=\"shotcut:caption\">PATH</property>";
		out.push_back(replace(s1, "PATH", path));

		out.push_back("    <property name=\"xml\">was here</property>");
		
		for (i2 = 0; i2 < fltarr.size(); i2++) {
			b1 = false; // the first line has to be added to fltnum, the rest will be pushed into out
		    string fltcmd = "\""; 
			fltcmd.append(fltarr.at(i2).exe);
			fltcmd.append("\" \"PATH\"");
			fltcmd = replace(fltcmd, "PATH", path);
			fltcmd.append(" ");
			fltcmd.append("shotcut ");
			fltcmd.append(to_string(clparr.at(i4) + onefrm));
			fltcmd.append(" ");
			fltcmd.append(to_string(clparr.at(i4 + 1) + onefrm));
			fltcmd.append(" ");
			fltcmd.append(to_string(i1));
			fltcmd.append("/");
			fltcmd.append(to_string(clparr.size() - 1));
			fltcmd.append(" ");
			fltcmd.append(to_string(fltnum));
			fltcmd.append(" ");
			fltcmd.append(fltarr.at(i2).args);

			const char * realfltcmd = fltcmd.c_str();

			FILE * inflts = NULL;
			char buf[1025];
			
			if ((inflts = popen(realfltcmd, "r")) != NULL) {
				if (inflts) {
					fgets(buf, 1024, inflts);
					fltnum = fltnum + stoi(s1 = buf);
				}
				while (fgets(buf, 1024, inflts) != NULL) {
					if (s1.find("Fatal error: ") == 0) {
						cout << endl << s1 << " (from filterer: " << fltarr.at(i2).name << " with call: " << fltarr.at(i2).call << ")" << endl;
						exit(6);
					} else {
						s1 = buf;
						c1 = s1.at(s1.size() - 1);
						if (c1 == '\n') {s1.pop_back();} // remove newline if present
						out.push_back(s1);
					}
				}
				pclose(inflts);
				inflts = NULL;
			}
		}
		
		out.push_back("  </chain>");

		i1++;
		cout << progress << "Filtering: " << i1 << "/" << clparr.size() / 2;
	}

	for (i1 = 0; i1 < playlist.size(); i1++) {
		out.push_back(playlist.at(i1)); // add the whole playlist array to the end of the out array
	}

	prolenm1 = prolen - onefrm;

	out.push_back("  </playlist>");

	s1 = "  <tractor id=\"tractor0\" title=\"Shotcut version 22.09.23\" in=\"00:00:00.000\" out=\"TOTLENM1\">";
	out.push_back(replace(s1, "TOTLENM1", toanatim(prolenm1, 3)));

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

	s1 = "  <producer id=\"black\" in=\"00:00:00.000\" out=\"TOTLENM1\">"; // TOTLENM1 has to be the total length of project
	fin.push_back(replace(s1, "TOTLENM1", toanatim(prolenm1, 3)));

	s1 = "    <property name=\"length\">TOTLEN</property>"; //  This appears to be the last frame of the project approximated to the thousandth.
	fin.push_back(replace(s1, "TOTLEN", toanatim(prolen, 3)));

	fin.push_back("    <property name=\"eof\">pause</property>");
	fin.push_back("    <property name=\"resource\">0</property>");
	fin.push_back("    <property name=\"aspect_ratio\">1</property>");
	fin.push_back("    <property name=\"mlt_service\">color</property>");
	fin.push_back("    <property name=\"mlt_image_format\">rgba</property>");
	fin.push_back("    <property name=\"set.test_audio\">0</property>");
	fin.push_back("  </producer>");
	fin.push_back("  <playlist id=\"background\">");

	s1 = "    <entry producer=\"black\" in=\"00:00:00.000\" out=\"TOTLENM1\"/>";
	fin.push_back(replace(s1, "TOTLENM1", toanatim(prolenm1, 3)));

	for (i1 = 0; i1 < out.size(); i1++) {
		fin.push_back(out.at(i1));  // add the whole out to the end of the fin array
	}

	ofstream wrtmlt(outpath);
	for (i1 = 0; i1 < fin.size(); i1++) {
		wrtmlt << fin.at(i1) << endl;
	}
	wrtmlt.close();

	cout << endl << "Done!" << endl;

	exit(0);
}
