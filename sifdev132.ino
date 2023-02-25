// by Hellfie
// Simulated Input Framework (SIF)
// Works at HID report level. Tested on Leonardo with USB host shield, mouse Ritmix RMW-555, IDE 2.0.3. Executes "EFT flee market fast buy macros (MFB)" as an example.
// This program is a violation of multiple license agreements and will likely trigger most anticheats, antiviruses, bot detectors and alike. User discretion is adviced.
// (though it's designed not to do all that bad "trigger" stuff

//~~~ known bugs:
// cursor movement gets worse when starting point is close to the destination
// added Usb.Task() in sif() so the cursor doesn't make sharp leap if mouse was moved during completion, need to forbid mouse movement without downsides somehow
// xy coordinates only work correctly if you move cursor from one corner of the screen to the opposite after boot, but that's a feature
// haltrun is commented for developing
// sometimes cursor goes through the center and keeps going, something in "if(cyb)" does that
// need to add more accuracy when close to the destination
// sometimes delay between Mouse.move() isn't accurate, it may be 5 and 3 after that when both should've been 4, which means tdf doesn't account for interrupts perfectly, i think Usb.Task() is to blame

#include <hidboot.h> // USB host mouse from USB Host Shield Library. Install using Library Manager
#include <Mouse.h> // USB device mouse library included with Arduino IDE 1.8.5
using namespace std; // easier coding

const float pi = acos(-1);
float snd = 100; // SINUSOID DIRECTION AND SIZE - 100 is default, bigger number bigger sinus
float snf = 50;  // SINUSOID SHIFT - 50 is default, changing number changes sinus peak (30-70)
float pau = 250; // PAUSE DURATION - 250 is default, accidental repeated activation countermeasure
float mic = 4;   // MOVE DELAY - 4 is RMW-555 default, used to simulate pause between moves
float xsn,ysn,stp,xh,yh,xu,yu,xexp,yexp,xm,ym,dvn,fmn;
int   r1,zx,zy,cyb,xn,yn,xo,yo,x,y,z,dela,xt,yt;
unsigned long mls = 0;
unsigned long tdf = 0;

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
  #if defined(CDC_ENABLED)
    //haltrun();
  #endif
  Usb.Init();
  HidMouse.SetReportParser(0, &Prs);
  Mouse.begin();
}

bool sif(int xa, int ya) //-- xa and ya are destination coords
{

  xsn=ysn=stp=xh=yh=xu=yu=xexp=yexp=dvn=fmn = 0;
  zx=zy=cyb=xn=yn=xo=yo = 0;
  tdf = 0;
  r1=xm=ym = 1;
  
xt=x;
yt=y;

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

  if(cyb){ //-- cyb=1 if y coord is lengthier
    fmn=(ym+4)*(ym+1)*0.1; //-- arithmetic progression formula, fmn=Sn, ym=An
    if(fmn<yh-abs(yexp) && dvn==0){ym+=8;r1+=1;} //-- need to use this paired with randomizer
    if(ym>100){ym=100;r1-=1;}
    if(fmn>yh-abs(yexp) || dvn!=0)
      {
      dvn=1;
      ym=(yh-abs(yexp))/r1+(5*r1)/2-5/2;
      r1-=1;    
      if(ym<1){ym=1;}
      }
    if(ym<1){ym=1;}
    yexp+=ym*zy;
    xexp=abs(yexp/stp)*zx;    
  }
  else{
    fmn=(xm+4)*(xm+1)*0.1;
    if(fmn<xh-abs(xexp) && dvn==0){xm+=8;r1+=1;}
    if(xm>100){xm=100;r1-=1;}
    if(fmn>xh-abs(xexp) || dvn!=0)
      {
      dvn=1;
      xm=(xh-abs(xexp))/r1+(5*r1)/2-5/2;
      r1-=1;    
      if(xm<1){xm=1;}
      }  
    if(xm<1){xm=1;}
    xexp+=xm*zx;
    yexp=abs(xexp/stp)*zy;
  }
  
  //-- getting vertical and horizontal sinusoids,
  //-- xexp/xh and yexp/yh are percents of how much distance already traveled,
  //-- 50/snf and 50/(100-snf) are needed to shift sinusoid peak,
  //-- "if" and "else" are here for the same reason, "if" is before peak and "else" is after,
  //-- zx, zy and -1 are needed to make diagonal movement work right
  if(snf>abs(xexp/xh*100)){ysn = sin(abs(xexp/xh)*(50/snf)*pi)*snd*zx;}
  else{ysn = sin(((1-abs(xexp/xh))*(50/(100-snf)))*pi)*snd*zx;}
  if(snf>abs(yexp/yh*100)){xsn = sin(abs(yexp/yh)*(50/snf)*pi)*snd*zy*(-1);}
  else{xsn = sin(((1-abs(yexp/yh))*(50/(100-snf)))*pi)*snd*zy*(-1);}

  //SetCursorPos();
  xn=static_cast<int>(xt+xexp+xsn);
  yn=static_cast<int>(yt+yexp+ysn);
  /*
  Serial.print(fmn);
  Serial.print(' ');
  Serial.print(xh-abs(xexp));
  Serial.print(' ');
  Serial.print(xm);
  Serial.print(' ');
  Serial.print(r1);
  Serial.println(); 
  */

  Serial.print(millis());
  Serial.print(' ');
  Serial.print(tdf);
  Serial.println(); 
  if(mic-(millis()-tdf)<mic+1 && mic-(millis()-tdf)>0){delay(mic-(millis()-tdf));} //-- delay is set to match mouse signature, for mine it's 4ms
  tdf=millis(); //-- tdf is to account for interrupts
  
  if(xo != xn && yo == yn){ 
    Mouse.move(xn-xo, 0, 0);
    x+=xn-xo;
    xo=xn;
    }
  if(xo == xn && yo != yn){
    Mouse.move(0, yn-yo, 0);
    y+=yn-yo;
    yo=yn;
    }
  if(xo != xn && yo != yn){
    Mouse.move(xn-xo, yn-yo, 0);
    x+=xn-xo;
    y+=yn-yo;
    xo=xn;
    yo=yn;
    }

    Usb.Task();
  }
  return 0;
}

void loop() 
{
  Usb.Task();
  if(z==1 && pau<millis()-mls){
    sif(1200, 485); 
    mls=millis();    
  }
  /*
  Serial.print(x);
  Serial.print(' ');
  Serial.print(y);
  Serial.println();  
  */
}
