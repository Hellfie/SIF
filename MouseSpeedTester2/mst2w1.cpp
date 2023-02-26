// by Hellfie
// Mouse Speed Tester
// this program is a violation of multiple license agreements and will likely trigger most anticheats, antiviruses, bot detectors and alike. user discretion is adviced

#include <iostream>
#include <conio.h>
#include <windows.h>
#include <fstream> //writing into files
using namespace std;
// the #if is optimised milliseconds code both for linux and windows

#if !defined(_WIN32) && !defined(_WIN64) // Linux - Unix
    #  include <sys/time.h>
    typedef timeval sys_time_t;
    inline void system_time(sys_time_t* t) {
        gettimeofday(t, NULL);
    }
    inline long long time_to_msec(const sys_time_t& t) {
        return t.tv_sec * 1000LL + t.tv_usec / 1000;
    }
    #else // Windows and MinGW
    #  include <sys/timeb.h>
    typedef _timeb sys_time_t;
    inline void system_time(sys_time_t* t) { _ftime(t); }
    inline long long time_to_msec(const sys_time_t& t) {
        return t.time * 1000LL + t.millitm;
    }
#endif
int main() 
{       	
	
	ofstream out("record.txt");
	
        int r, xp, yp, z, zp, xt, yt,xd,yd;
	z = 0;
	zp = 0;
	r = 0;
	xp = 0;
	yp = 0;
	xt = 0;
	yt = 0;
	xd=yd=0;
unsigned long mls2 = 0;
long long mls3 = 0;
unsigned long mls4 = 0;
unsigned long mls5 = 0;
unsigned long mls6 = 0;
unsigned long mls7 = 0;
unsigned long mls72 = 0;
unsigned long mls8 = 0;
	POINT p;


	GetCursorPos(&p);
	int x = p.x, y = p.y;
        SetCursorPos(x, y);


	cout << "Press B to set top-left corner";
	while(r == 0){
	if(GetKeyState('B')){
		r++;
		GetCursorPos(&p);
		xt = p.x; 
		yt = p.y;
		out << xt << " " << yt << " " << "3" << "\n";
	}
}
r = 0;
	cout << "\nPress R to stop recording";


	while(r == 0){
	GetCursorPos(&p);	
	if(GetKeyState(VK_LBUTTON) & 0x8000){z = 1;}else{z = 0;}
	x = p.x; 
	y = p.y;
	    sys_time_t t;
    system_time(&t);
    long long currentTimeMs = time_to_msec(t);
	if(x != xp || y != yp || z != zp){
		xp = x;
		yp = y;
		zp = z;
		out << "X = " << abs(x-xd) << "   Y = " << abs(y-yd) << "   Ms:" << currentTimeMs-mls3 << "\n";
		mls3=currentTimeMs;
		xd=x;
		yd=y;
	}
	if(GetKeyState('R')){
		r++;
	}
}
 

	out.close();
 	cout << "\nDone\n";
        system("pause");
        return 0;
}