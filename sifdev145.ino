// by Hellfie
// Simulated Input Framework (SIF)
// Works at HID report level. Tested on Leonardo with USB host shield, mouse Ritmix RMW-555, IDE 2.0.3. Executes "EFT flee market fast buy macros (MFB)" as an example.
// This program is a violation of multiple license agreements and will likely trigger most anticheats, antiviruses, bot detectors and alike. User discretion is adviced.
// (though it's designed not to do all that bad "trigger" stuff)

//~~~ known bugs:
// added Usb.Task() in sif() so the cursor doesn't make sharp leap if mouse was moved during completion, need to forbid mouse movement without downsides somehow
// xy coordinates only work correctly if you move cursor from one corner of the screen to the opposite after boot, but that's a feature
// movement aborts when starting point is too close to the destination, but that's a feature
// haltrun is commented for developing
// sometimes cursor goes through the center and keeps going, something in "if(cyb)" does that (this seems to be fixed after adding tpm and opm)
// sometimes delay between Mouse.move() isn't accurate, it may be 5 and 3 after that when both should've been 4, which means tdf doesn't account for interrupts perfectly, i think Usb.Task() is to blame

#include <hidboot.h> // USB host mouse from USB Host Shield Library. Install using Library Manager
#include <Mouse.h> // USB device mouse library included with Arduino IDE 1.8.5
using namespace std; // easier coding
const float pi = acos(-1);

float pau = 250;  // PAUSE DURATION - 250 is default, accidental repeated activation countermeasure
float mic = 4;    // MOVE DELAY - 4 is RMW-555 default, used to simulate pause between moves
int   wdt = 1920; // SCREEN WIDTH - 1920 is default, width part of your screen's resolution
int   hgt = 1080; // SCREEN HEIGHT - 1080 is default, height part of your screen's resolution

// randomized stuff:
float snd = 10;   // SINUSOID DIRECTION AND SIZE - 10 is default, bigger number bigger sinus, negative numbers reverse direction (-10 - 10)
float snf = 50;   // SINUSOID SHIFT - 50 is default, changing number changes sinus peak (30 - 70)
float lmd = 0.65; // LAST MOUSE DASH - 0.65 is default, controls last 2 mouse moves (0.60-0.75)
float opm = 10;   // HIGH PRECISION MOVE - 10 is default, ends cursor movement in one dash (9-11)
float tpm = 30;   // LOW PRECISION MOVE - 30 is default, ends cursor movement in two dashes (28-32)
float slw = 10;   // SLOW SPEED - 10 is default, how fast cursor starts slowing down (9-11)
float fst = 8;    // FASTEN SPEED - 8 is default, how fast cursor gains speed (6-10)
float sst = 0.5;  // SLOW STEP - 0.5 is default, how fast cursor slows down (0.5-0.7)

float arn[9]{0}; // array of random numbers
float xsn,ysn,stp,xh,yh,xu,yu,xexp,yexp,xm,ym,dvn,fmn,dcn;
int   r1,zx,zy,cyb,xn,yn,xo,yo,x,y,z,dela,xt,yt,ct2;
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
  if (x < wdt-1 && buf[1] > 0 && buf[1] < 128) {
    x+=buf[1];
    if (x>wdt-1) {x=wdt-1;}
  }
  if (x > 0 && buf[1] > 128) {
    x+=buf[1]-256;
    if (x<0) {x=0;}
  }

  if (y < hgt-1 && buf[2] > 0 && buf[2] < 128) {
    y+=buf[2];
    if (y>hgt-1) {y=hgt-1;}
  }
  if (y > 0 && buf[2] > 128) {
    y+=buf[2]-256;
    if (y<0) {y=0;}
  }
  //-- getting pressed button
  z=buf[0];
}



USB Usb;
HIDBoot<USB_HID_PROTOCOL_MOUSE> HidMouse(&Usb);
MouseRptParser Prs;

void setup() {
  #if defined(CDC_ENABLED)
    //while(1){delay(100000);}
  #endif
  Usb.Init();
  HidMouse.SetReportParser(0, &Prs);
  Mouse.begin();
  randomizer();
}

void activeDelay(int acd){ //-- pauses code execution for acd miliseconds while still allowing mouse to move
  unsigned long dct = millis();
  while(millis()-dct < acd){
    Usb.Task();
}
}

void randomizer() { //-- this resets entropy
  int s1,s4,s16,s64,s256,ct1;
  s1=s4=s16=s64=s256 = 0;
  ct1 = 1;

    while(ct1<9){
    activeDelay(5);
    s1=(analogRead(A5) & 3);
    activeDelay(5);
    s4=(analogRead(A5) & 3);
    activeDelay(5);
    s16=(analogRead(A5) & 3);
    activeDelay(5);
    s64=(analogRead(A5) & 3);
    activeDelay(5);
    s256=(analogRead(A5) & 3);
    arn[ct1]=(s1+1+(s4*4)+(s16*16)+(s64*64)+(s256*256));
    ct1++;
    }
}

bool sif(int xa, int ya) //-- xa and ya are destination coords
{

  xsn=ysn=stp=xh=yh=xu=yu=xexp=yexp=dvn=fmn=dcn = 0;
  zx=zy=cyb=xn=yn=xo=yo=ct2 = 0;
  tdf = 0;
  r1=xm=ym = 1;

xt=x;
yt=y;

//-- randomizing stuff
if(arn[2]!=0 && arn[2]!=arn[3]){ //-- checking if entropy is alright
snd = -10+(20*(arn[1]/1024)); 
snf = 30+(40*(arn[2]/1024));
lmd = 0.60+(0.15*(arn[3]/1024));
opm = 9+(2*(arn[4]/1024));
tpm = 28+(4*(arn[5]/1024));
slw = 9+(2*(arn[6]/1024));
fst = 6+(4*(arn[7]/1024));
sst = 0.5+(0.2*(arn[8]/1024));
}
else{while(1){delay(100000);}} //-- if entropy is not right cease all actions, this shouldn't happen ever though
while(ct2<9){arn[ct2]=0;ct2++;} //-- unchanged entropy failsafe

if(xt<xa){zx=1;}else{zx=-1;} //-- zx and zy are moving directions
if(yt<ya){zy=1;}else{zy=-1;}

xh=abs(xt-xa); //-- xh and yh are x and y length
yh=abs(yt-ya);
if(xh==0){xh+=0.01;} //-- later we'll use them for division so it mustn't be zero
if(yh==0){yh+=0.01;}
if(xh>yh){stp=xh/yh;}else{stp=yh/xh; cyb=1;} //-- getting which coord is lengthier

  xo = xt;
  yo = yt;
  //if((yh<10 && cyb==1) || (xh<10 && cyb!=1)){xh=1;yh=1;xexp=2;yexp=2;}//-- if cursor is too close to destination then abort
  if(xt==xa && yt==ya){xh=1;yh=1;xexp=2;yexp=2;}//-- if cursor is already at destination then don't move
  
  //~~~~~ main function
  while ((xh > abs(xexp) && cyb!=1) || (yh > abs(yexp) && cyb==1)) {

  if(cyb){ //-- cyb=1 if y coord is lengthier
    fmn=((ym-1+fst)*(ym+1))/(2*fst); //-- arithmetic progression formula, fmn=Sn ym=An fst=d
    if(fmn<yh-abs(yexp) && dvn==0 && yh-abs(yexp)>tpm){ym+=fst;r1++;} //-- need to use this paired with randomizer
    if(ym>126){ym=126;r1-=1;}
    if((fmn>yh-abs(yexp) || dvn!=0) && yh-abs(yexp)>tpm)
      {
      dvn=1;
      ym=(yh-abs(yexp))/r1+(slw*r1)/2-slw/2; //-- arithmetic progression formula, (yh-abs(yexp))=Sn ym=An slw=d r1=n
      slw-=sst; //-- slw added so cursor starts slowing down in big leaps and ends slowing down in short leaps
      if(slw<5){slw=5;}
      r1-=1;    
      if(ym<1){ym=1;}
      }
    if(yh-abs(yexp)<=opm && dvn!=2){dcn=ym-(yh-abs(yexp))/1;dvn=2;} //-- this is for precision
    if(yh-abs(yexp)<=tpm && dvn!=2){dcn=(yh-abs(yexp))*(lmd-(1-lmd)); ym=(yh-abs(yexp))*lmd+dcn; dvn=2;}
    if(yh-abs(yexp)<=tpm && dvn==2){ym-=dcn;} 
    if(ym<1){ym=1;}
    yexp+=ym*zy;
    xexp=abs(yexp/stp)*zx;    
  }
  else{ //-- all the same but in case if x coord is lengthier
    fmn=((xm-1+fst)*(xm+1))/(2*fst);
    if(fmn<xh-abs(xexp) && dvn==0 && xh-abs(xexp)>tpm){xm+=fst;r1++;}
    if(xm>126){xm=126;r1-=1;}
    if((fmn>xh-abs(xexp) || dvn!=0) && xh-abs(xexp)>tpm)
      {
      dvn=1;
      xm=(xh-abs(xexp))/r1+(slw*r1)/2-slw/2;
      slw-=sst;
      if(slw<5){slw=5;}
      r1-=1;    
      if(xm<1){xm=1;}
      }  
    if(xh-abs(xexp)<=opm && dvn!=2){dcn=xm-(xh-abs(xexp))/1;dvn=2;}
    if(xh-abs(xexp)<=tpm && dvn!=2){dcn=(xh-abs(xexp))*(lmd-(1-lmd)); xm=(xh-abs(xexp))*lmd+dcn; dvn=2;}
    if(xh-abs(xexp)<=tpm && dvn==2){xm-=dcn;}
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
    activeDelay(15); 
    sif(1200, 485); 
    mls=millis();    
    randomizer();
  }
}