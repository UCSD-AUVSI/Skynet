#pragma once

#define VIDEO_FRAMERATE 20

#define AVERAGE_VIDEO_LATENCY 150 // (ms) +- 50ms
#define RABBIT_DELAY	125 // (ms)


#define MAX_ROLL	4400
#define MIN_ROLL	1200
#define MAX_PITCH	4400
#define MIN_PITCH	1200
#define MAX_ZOOM_LEVEL	10
#define MIN_ZOOM_LEVEL	0


#define PRINT System::Diagnostics::Trace::WriteLine
#define BADSTRING String::IsNullOrWhiteSpace


#define PI 3.14159265358979323846
#define RAD_TO_DEG 180.0 / PI
#define PI_TO_RAD PI / 180.0



#define FIELD_ALTITUDE 156.5

#define HTTP_HOME_FOLDER

#define HTTP_SERVER_ROOT "C:\\xampp\\htdocs\\"
#define HTTP_SERVER_TARGET_PATH "C:\\xampp\\htdocs\\targets\\"
#define HTTP_SERVER_IMAGERY_PATH "C:\\xampp\\htdocs\\imagery\\"
#define HTTP_SERVER_EXPORT_PATH "C:\\xampp\\htdocs\\turnin\\"
#define TAB_DELIMITED_TURNIN_FILE_PATH (HTTP_SERVER_ROOT + "turnin\\UCSD.txt")