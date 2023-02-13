// by Hellfie
// Simulated Input Framework (SIF)
// Works at HID report level. HID report is not decoded. Tested on Leonardo with Arduino USB host shield, IDE 2.0.3. Executes "EFT flee market fast buy macros (MFB)" as an example.
// This program is a violation of multiple license agreements and will likely trigger most anticheats, antiviruses, bot detectors and alike. User discretion is adviced.
// (though it's designed not to do all that bad "trigger" stuff)

//~~~ known bugs:
// cursor movement gets worse when starting point is close to destination, see "dela"
// cursor makes sharp leap if mouse was moved during sif() completion
// xy coordinates only work correctly if you move cursor from one corner of the screen to the opposite after boot, but that's a feature
// nops don't work, currently using delay instead
// haltrun is commented for developing


#include <hidboot.h> // USB host mouse from USB Host Shield Library. Install using Library Manager
#include <Mouse.h> // USB device mouse library included with Arduino IDE 1.8.5
#ifdef dobogusinclude // Satisfy the IDE, which needs to see the include statment in the ino too.
#include <spi4teensy3.h>
#endif
#include <SPI.h>
using namespace std; // easier coding

const float pi = acos(-1);
float ms = 500.0 * 1000.0;  // MOUSE SPEED - 500k is default, lesser number - faster moving
float snd = 100;            // SINUSOID DIRECTION AND SIZE - 100 is default, bigger number bigger sinus
int   r,zx,zy,cyb,xn,yn,xo,yo,x,y,z,dela;
float xsn,ysn,stp,xh,yh,xu,yu,xexp,yexp;


class MouseRptParser : public MouseReportParser 
{
protected:
  void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};

void MouseRptParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf)
{
  if (buf[2] == 1)
    return;

  if (len > 2) {
    uint8_t mouseRpt[4];
    mouseRpt[0] = buf[0];
    mouseRpt[1] = buf[1];
    mouseRpt[2] = buf[2];
    mouseRpt[3] = 0;
    HID().SendReport(1,mouseRpt,sizeof(mouseRpt));
  }

  //-- tracking the position of mouse cursor
  if (x < 1919 && buf[1] > 0 && buf[1] < 128) {
    x+=buf[1];
    if (x>1919) {x=1919;}
  }
  if (x > 0 && buf[1] > 128) {
    x+=buf[1]-256;
    if (x<0) {x=0;}
  }

  if (y < 1079 && buf[2] > 0 && buf[2] < 128) {
    y+=buf[2];
    if (y>1079) {y=1079;}
  }
  if (y > 0 && buf[2] > 128) {
    y+=buf[2]-256;
    if (y<0) {y=0;}
  }
  //-- getting pressed button
  z=buf[0];
}

void haltrun(){
  while(1){delay(1000);
  Serial.print("Error: compiled without disabling serial ports");
  Serial.println(); 
}
}

USB Usb;
HIDBoot<USB_HID_PROTOCOL_MOUSE> HidMouse(&Usb);
MouseRptParser Prs;

void setup() {
  Usb.Init();
  HidMouse.SetReportParser(0, &Prs);
  Mouse.begin();
  #if defined(CDC_ENABLED)
    //haltrun();
  #endif
}

bool sif(int xt, int yt, int xa, int ya) //-- xt and yt are starting point coords, xa and ya are destination coords
{
  r=zx=zy=cyb=xn=yn=xo=yo=x=y = 0;
  xsn=ysn=stp=xh=yh=xu=yu=xexp=yexp = 0;

if(xt<xa){zx=1;}else{zx=-1;} //-- zx and zy are moving directions
if(yt<ya){zy=1;}else{zy=-1;}

xh=abs(xt-xa); //-- xh and yh are x and y length
yh=abs(yt-ya);
if(xh==0){xh+=0.01;} //-- later we'll use them for division so it mustn't be zero
if(yh==0){yh+=0.01;}
if(xh>yh){stp=xh/yh;}else{stp=yh/xh; cyb=1;} //-- getting which coord is lengthier

  xo = xt;
  yo = yt;
  if(xt==xa && yt==ya){xh=1;yh=1;xexp=2;yexp=2;}//-- if cursor is already at destination then don't move
  
  //~~~~~ main function
  while (xh > abs(xexp) || yh > abs(yexp)) {
  if(cyb){yexp+=zy;}
  else{xexp+=zx;}
  if(cyb){xexp=abs(yexp/stp)*zx;}
  else{yexp=abs(xexp/stp)*zy;}
  //-- getting vertical and horizontal sinusoids,
  //-- -1 is needed to make sure diagonal movement works right,
  //-- snd is the height and direction of sinusoid

  ysn = sin(xexp/xh*pi)*snd;
  xsn = sin(yexp/yh*pi)*snd*(-1);

  //SetCursorPos();
  xn=static_cast<int>(xt+xexp+xsn);
  yn=static_cast<int>(yt+yexp+ysn);

  if(xo != xn && yo == yn){
    Mouse.move(xn-xo, 0, 0);
    //x+=xn-xo;
    xo=xn;
    }
  if(xo == xn && yo != yn){
    Mouse.move(0, yn-yo, 0);
    //y+=yn-yo;
    yo=yn;
    }
  if(xo != xn && yo != yn){
    Mouse.move(xn-xo, yn-yo, 0);
    //x+=xn-xo;
    //y+=yn-yo;
    xo=xn;
    yo=yn;
    }

    //-- pause between movement
    while (r < ms) {
      void nop();
      r++;
    }
    r=0;
    //-- nops don't seem to work on arduino (or syntax is different) so here's an actual pause
    //-- dela needed so cursor doesn't go up when starting point is too close to ending point
    //-- set delay to 1 to see dela bug
    dela = static_cast<int>(100/(xh+yh));
    if(dela==0){dela=1;}
    if(dela>100){dela=100;}
    delay(dela);
  }
  x = xa;
  y = ya;
  return 0;
}

void loop() 
{
  Usb.Task();
  if(z==1){
    sif(x, y, 1200, 485); 
  }
  /*
  Serial.print(x);
  Serial.print(' ');
  Serial.print(y);
  Serial.println();  
  */
}
