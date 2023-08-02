#include <U8g2lib.h>
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include<EEPROM.h>
#define STDYEAR 2017
#define STDMON 1
#define STDDAY 1
#define WEEK 7 
#define FLSKEY 0
//bug:读写EEPROM，编辑，断电恢复
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
int frtdys;
typedef struct _lnyar{
    int year;
    int month;
    int day;
    int reserved;
} lnyar;
unsigned int lunar200y[199] = {
    0x04AE53,0x0A5748,0x5526BD,0x0D2650,0x0D9544,0x46AAB9,0x056A4D,0x09AD42,0x24AEB6,0x04AE4A,/*1901-1910*/
    0x6A4DBE,0x0A4D52,0x0D2546,0x5D52BA,0x0B544E,0x0D6A43,0x296D37,0x095B4B,0x749BC1,0x049754,/*1911-1920*/
    0x0A4B48,0x5B25BC,0x06A550,0x06D445,0x4ADAB8,0x02B64D,0x095742,0x2497B7,0x04974A,0x664B3E,/*1921-1930*/
    0x0D4A51,0x0EA546,0x56D4BA,0x05AD4E,0x02B644,0x393738,0x092E4B,0x7C96BF,0x0C9553,0x0D4A48,/*1931-1940*/
    0x6DA53B,0x0B554F,0x056A45,0x4AADB9,0x025D4D,0x092D42,0x2C95B6,0x0A954A,0x7B4ABD,0x06CA51,/*1941-1950*/
    0x0B5546,0x555ABB,0x04DA4E,0x0A5B43,0x352BB8,0x052B4C,0x8A953F,0x0E9552,0x06AA48,0x6AD53C,/*1951-1960*/
    0x0AB54F,0x04B645,0x4A5739,0x0A574D,0x052642,0x3E9335,0x0D9549,0x75AABE,0x056A51,0x096D46,/*1961-1970*/
    0x54AEBB,0x04AD4F,0x0A4D43,0x4D26B7,0x0D254B,0x8D52BF,0x0B5452,0x0B6A47,0x696D3C,0x095B50,/*1971-1980*/
    0x049B45,0x4A4BB9,0x0A4B4D,0xAB25C2,0x06A554,0x06D449,0x6ADA3D,0x0AB651,0x093746,0x5497BB,/*1981-1990*/
    0x04974F,0x064B44,0x36A537,0x0EA54A,0x86B2BF,0x05AC53,0x0AB647,0x5936BC,0x092E50,0x0C9645,/*1991-2000*/
    0x4D4AB8,0x0D4A4C,0x0DA541,0x25AAB6,0x056A49,0x7AADBD,0x025D52,0x092D47,0x5C95BA,0x0A954E,/*2001-2010*/
    0x0B4A43,0x4B5537,0x0AD54A,0x955ABF,0x04BA53,0x0A5B48,0x652BBC,0x052B50,0x0A9345,0x474AB9,/*2011-2020*/
    0x06AA4C,0x0AD541,0x24DAB6,0x04B64A,0x69573D,0x0A4E51,0x0D2646,0x5E933A,0x0D534D,0x05AA43,/*2021-2030*/
    0x36B537,0x096D4B,0xB4AEBF,0x04AD53,0x0A4D48,0x6D25BC,0x0D254F,0x0D5244,0x5DAA38,0x0B5A4C,/*2031-2040*/
    0x056D41,0x24ADB6,0x049B4A,0x7A4BBE,0x0A4B51,0x0AA546,0x5B52BA,0x06D24E,0x0ADA42,0x355B37,/*2041-2050*/
    0x09374B,0x8497C1,0x049753,0x064B48,0x66A53C,0x0EA54F,0x06B244,0x4AB638,0x0AAE4C,0x092E42,/*2051-2060*/
    0x3C9735,0x0C9649,0x7D4ABD,0x0D4A51,0x0DA545,0x55AABA,0x056A4E,0x0A6D43,0x452EB7,0x052D4B,/*2061-2070*/
    0x8A95BF,0x0A9553,0x0B4A47,0x6B553B,0x0AD54F,0x055A45,0x4A5D38,0x0A5B4C,0x052B42,0x3A93B6,/*2071-2080*/
    0x069349,0x7729BD,0x06AA51,0x0AD546,0x54DABA,0x04B64E,0x0A5743,0x452738,0x0D264A,0x8E933E,/*2081-2090*/
    0x0D5252,0x0DAA47,0x66B53B,0x056D4F,0x04AE45,0x4A4EB9,0x0A4D4C,0x0D1541,0x2D92B5          /*2091-2099*/
};
int monthTotal[13] = {0,31,59,90,120,151,181,212,243,273,304,334,365};
lnyar toSolar(lnyar lunar){
    int year = lunar.year,
    month = lunar.month,
    day = lunar.day;
    int byNow, xMonth, i;
    lnyar solar;
    byNow = (lunar200y[year-1901] & 0x001F) - 1;
    if( ((lunar200y[year-1901]&0x0060)>>5) == 2)
        byNow += 31;
    for(i = 1; i < month; i ++){
        if( ( lunar200y[year - 1901] & (0x80000 >> (i-1)) ) ==0){
            byNow += 29;
        }
        else
            byNow += 30;
    }
    byNow += day;
    xMonth = (lunar200y[year - 1901] & 0xf00000)>>20;
    if(xMonth != 0){
        if(month > xMonth
           ||(month==xMonth && lunar.reserved == 1)){
            if((lunar200y[year-1901] & (0x80000>>(month-1)))==0)
                byNow += 29;
            else
                byNow += 30;
        }
    }
    if(byNow > 366
       ||(year%4!=0 && byNow == 365)){
        year += 1;
        if(year%4==0)
            byNow -= 366;
        else
            byNow -= 365;
    }
    for(i=1; i <= 13; i ++){
        if(monthTotal[i] >= byNow){
            month = i;
            break;
        }
    }
    solar.day = byNow - monthTotal[month-1];
    solar.month = month;
    solar.year = year;
    
    return solar;
}
lnyar toLunar(lnyar solar){
    int year = solar.year,
    month = solar.month,
    day = solar.day;
    int bySpring,bySolar,daysPerMonth;
    int index,flag;
    lnyar lunar;
    if( ((lunar200y[year-1901] & 0x0060) >> 5) == 1)
        bySpring = (lunar200y[year-1901] & 0x001F) - 1;
    else
        bySpring = (lunar200y[year-1901] & 0x001F) - 1 + 31;
    bySolar = monthTotal[month-1] + day - 1;
    if( (!(year % 4)) && (month > 2))
        bySolar++;
    if (bySolar >= bySpring) {
        bySolar -= bySpring;
        month = 1;
        index = 1;
        flag = 0;
        if( ( lunar200y[year - 1901] & (0x80000 >> (index-1)) ) ==0)
            daysPerMonth = 29;
        else
            daysPerMonth = 30;
        while(bySolar >= daysPerMonth) {
            bySolar -= daysPerMonth;
            index++;
            if(month == ((lunar200y[year - 1901] & 0xF00000) >> 20) ) {
                flag = ~flag;
                if(flag == 0)
                    month++;
            }
            else
                month++;
            if( ( lunar200y[year - 1901] & (0x80000 >> (index-1)) ) ==0)
                daysPerMonth=29;
            else
                daysPerMonth=30;
        }
        day = bySolar + 1;
    }
    else {
        bySpring -= bySolar;
        year--;
        month = 12;
        if ( ((lunar200y[year - 1901] & 0xF00000) >> 20) == 0)
            index = 12;
        else
            index = 13;
        flag = 0;
        if( ( lunar200y[year - 1901] & (0x80000 >> (index-1)) ) ==0)
            daysPerMonth = 29;
        else
            daysPerMonth = 30;
        while(bySpring > daysPerMonth) {
            bySpring -= daysPerMonth;
            index--;
            if(flag == 0)
                month--;
            if(month == ((lunar200y[year - 1901] & 0xF00000) >> 20))
                flag = ~flag;
            if( ( lunar200y[year - 1901] & (0x80000 >> (index-1)) ) ==0)
                daysPerMonth = 29;
            else
                daysPerMonth = 30;
        }
        
        day = daysPerMonth - bySpring + 1;
    }
    lunar.day = day;
    lunar.month = month;
    lunar.year = year;
    if(month == ((lunar200y[year - 1901] & 0xF00000) >> 20))
        lunar.reserved = 8;
    else
        lunar.reserved = 9;     //8R9P
    return lunar;
}
int wherun(int year)
{
    if(((year%4==0)&&(year%100!=0))||(year%400==0))
        return 1;
    return 0;
}
int monshu(int year,int mon)
{
    if((mon==1)||(mon==3)||(mon==5)||(mon==7)||(mon==8)||(mon==10)||(mon==12))
        return 31;
    else if(mon==2)
    {
        if(wherun(year)==1)
            return 29;
        return 28;
    }
    else
        return 30;
}
long long int daydistance(int year,int mon,int day)
{
    int yearbk=year,dis=0;
    if((year>STDYEAR)||( year==STDYEAR)&&( mon>STDMON)||( year==STDYEAR)&&( mon==STDMON)&&( day>=STDDAY))
    {
        while(year>STDYEAR)
        {
            if(wherun(year-1)==1)
                dis=dis+366;
            else
                dis=dis+365;
            year--;
        }
        while(mon>STDMON)
        {
            dis=dis+monshu(yearbk,mon-1);
            mon--;
        }
        while(day>STDDAY)
        {
            dis++;
            day--;
        }
    }
    else
    {
        while(year<STDYEAR-1)
        {
            if(wherun(year+1)==1)
                dis=dis-366;
            else
                dis=dis-365;
            year++;
        }
        while( mon<12)
        {
            dis= dis- monshu(yearbk,mon);
            mon++;
        }
        while(day<=31)
        {
            dis--;
            day++;
        }
    }
    return dis;
}
int dayweeker(int year,int mon,int day)
{
    if (day==1)
    {
        int dwk,fu=0;
        dwk=daydistance(year,mon,day);
        if(dwk<0)
        {
            fu=1;
        }
        dwk=abs(dwk);
        while (dwk>=WEEK)
            dwk=dwk-WEEK;
        if(fu==1)
        {
            if(dwk==0)
            {
                frtdys=0;
                return 0;
            }
            else
            {
                frtdys=WEEK-dwk;
                return frtdys;
            }

        }
        else
        {
            frtdys=dwk;
            return dwk;
        }
    }
    else
    {
        int mdd=day-1,fixout=frtdys+mdd;
        while(fixout>=WEEK)
            fixout=fixout-WEEK;
        return fixout;
    }
}
bool iszhi(int data)
{
  for(int i=2;i<data;i++)
  if(data%i==0)
  return false;
  return true;
}
void expandstr(char *s)
{
  char temp[50];
  if(strlen(s)==1)
  {
    for(int i=0;i<15;i++)
  temp[i]=s[0];
  temp[14]=0;
  strcpy(s,temp);
  }
  else
  {
    for(int i=0;i<15;i++)
  temp[i]=s[i%2];
  temp[14]=0;
  strcpy(s,temp);
  }
}
char* callstr(int data,int type,char *s)
{
    if(type==1)
    strcpy(s,"2-64 SU DEMO...");
    else if(type==2)
    strcpy(s,"POWERED BY  ...");
    else if(type==3)
    strcpy(s,"HARMONYHI   ...");
    else
    strcpy(s,"STARTUPING  ...");
    return s;
}
bool plus(int *y,int *m,int *d)
{
    *d=*d+1;
    if(*d>monshu(*y,*m))
    {
      *d=*d%monshu(*y,*m);
      *m=*m+1;
    }
    if(*m>12)
    {
      *m=*m%12;
      *y=*y+1;
    }
    if(*y>2099)
    return false;
    return true;
}



void demo()
{
  char temp1[50],temp2[50],tempc[50];
    int totali=0;
      for(int i=2;i<64;i++)
    {
      if(!iszhi(i))
      continue;
      int bkp=i;
      int ctr=0;
      while(bkp!=0)
      {
        temp1[ctr++]=bkp%10+'0';
        bkp=bkp/10;
      }
      temp1[ctr]=0;
      int ctr2=0;
      for(int ii=ctr-1;ii>=0;ii--)
      temp2[ctr2++]=temp1[ii];
      temp2[ctr2]=0;
      expandstr(temp2);
      u8g2.clearBuffer();
      u8g2.setCursor(0, 14);
      u8g2.print(temp2);
      u8g2.setCursor(0, 28);
      u8g2.print(temp2);
      u8g2.setCursor(0, 42);
      u8g2.print(temp2);
      u8g2.setCursor(0, 56);
      u8g2.print(temp2);
      u8g2.sendBuffer();
      delay(100);
    }
    totali++;
    u8g2.clearBuffer();
      u8g2.setCursor(0, 14);
      u8g2.print(callstr(totali,1,tempc));
      u8g2.setCursor(0, 28);
      u8g2.print(callstr(totali,2,tempc));
      u8g2.setCursor(0, 42);
      u8g2.print(callstr(totali,3,tempc));
      u8g2.setCursor(0, 56);
      u8g2.print(callstr(totali,4,tempc));
      u8g2.sendBuffer();
      delay(7000);
}
char* message(int y,int m,int d,int w,char *s,bool lunar)
{
    if(lunar)
    {
     s[0]=y/1000+'0';
    s[1]=(y/100)%10+'0';
    s[2]=(y/10)%10+'0';
    s[3]=y%10+'0';
    s[4]='/';
    }
    else
    {
      if(y%12==0)
      strcpy(s,"Mnky");
      else if(y%12==1)
      strcpy(s,"Henn");
      else if(y%12==2)
      strcpy(s,"Doge");
      else if(y%12==3)
      strcpy(s,"Pigi");
      else if(y%12==4)
      strcpy(s,"Mice");
      else if(y%12==5)
      strcpy(s,"Bull");
      else if(y%12==6)
      strcpy(s,"Tigr");
      else if(y%12==7)
      strcpy(s,"Rabt");
      else if(y%12==8)
      strcpy(s,"Dagn");
      else if(y%12==9)
      strcpy(s,"Snak");
      else if(y%12==10)
      strcpy(s,"Huse");
      else
      strcpy(s,"Shep");
      s[4]='~';
    }
    s[5]=m/10+'0';
    s[6]=m%10+'0';
    if(lunar)
    s[7]='/';
    else
    s[7]='~';
    s[8]=d/10+'0';
    s[9]=d%10+'0';
    s[10]=' ';
    if(w==1)
    strcpy(s+11,"Mon");
    else if(w==2)
    strcpy(s+11,"Tue");
    else if(w==3)
    strcpy(s+11,"Wed");
    else if(w==4)
    strcpy(s+11,"Thr");
    else if(w==5)
    strcpy(s+11,"Fri");
    else if(w==6)
    strcpy(s+11,"Sat*");
    else if(w==8)
    strcpy(s+11,"N-Rn");
    else if(w==9)
    strcpy(s+11,"");
    else
    strcpy(s+11,"SUN*");
    return s;
}
void set_string(int a,int b,char *str)
{
  EEPROM.write(a,strlen(str));
  for(int i=0;i<strlen(str);i++)
    EEPROM.write(b+i,str[i]);
   EEPROM.commit();
}
char* get_string(int a,int b,char *str)
{
  int ctr=0;
  for(int i=0;i<a;i++)
  str[ctr++]=EEPROM.read(b+i);
  str[ctr]=0;
  return str;
}


bool calender(int *sto1,int *sto2,int *sto3,int *dly)
{
    int yar=*sto1,mon=*sto2,dy=*sto3;
    int loadweek=dayweeker(yar,mon,dy);
    char mes[50];
    bool fastmode=false;
    while(1)
    {
      for(int iflash=0;iflash<=28;iflash++)
        {
      int bkpy=yar,bkpm=mon,bkpd=dy;
      u8g2.clearBuffer();
      for(int i=1;i<5;i++)
        {
      u8g2.setCursor(0, 14*(2*i-1)-iflash);
      u8g2.print(message(yar,mon,dy,(loadweek+i-1)%7,mes,true));
      lnyar temp={.year=yar,.month=mon,.day=dy};
      lnyar tmp2=toLunar(temp);
      u8g2.setCursor(0, 14*2*i-iflash);
      u8g2.print(message(tmp2.year,tmp2.month,tmp2.day,tmp2.reserved,mes,false));
      plus(&yar,&mon,&dy);
         }
      u8g2.sendBuffer();
      yar=bkpy;
      mon=bkpm;
      dy=bkpd;
      delay(*dly);
      if(digitalRead(FLSKEY)==0)
      {
        *sto1=yar;
        *sto2=mon;
        *sto3=dy;
        return true;
      }
        }
      loadweek++;
      loadweek=loadweek%7;
      if(!plus(&yar,&mon,&dy))
      return false;
    }
}
void deal_str_to_date(int *y,int *m,int *d,char *s)
{
    *y=(s[0]-'0')/1000+(s[1]-'0')/100%10+(s[2]-'0')*10%10+(s[3]-'0')%10;
    *m=(s[5]-'0')/10+(s[6]-'0')%10;
    *d=(s[8]-'0')/10+(s[9]-'0')%10;
}
void setup_read(int *y,int *m,int *d,char *str)
{
    EEPROM.begin(1024);
    get_string(10,15,str);
    EEPROM.end();
    if(str[0]=='2')
    deal_str_to_date(y,m,d,str);
    else
    {
    *y=2022;
    *m=1;
    *d=1;
    }
}
void short_time_fun(bool *always_on,int *y,int *m,int *d,int *dly)
{
      u8g2.clearBuffer();
      char datastr[100];
      bool firstin=true;
      while(*always_on)
      {
          u8g2.clearBuffer();
          u8g2.setCursor(0, 14);
          u8g2.print("    USB Mode   ");
          u8g2.setCursor(0, 28);
          u8g2.print("Long fls to exit");
          u8g2.sendBuffer();
          if(firstin)
          {
            delay(300);
            firstin=false;
          }
          int ctr=0;
          for(int i=0;i<32;i++)
          {
            datastr[ctr++]=Serial.read();
            if(datastr[ctr-1]==-1)
              break;
          }
          datastr[--ctr]=0;
          if(datastr[0]=='e'&&datastr[1]=='x'&&datastr[2]=='i'&&datastr[3]=='t'||digitalRead(FLSKEY)==0)
          {
          *always_on=false;
          u8g2.setCursor(0, 42);
          u8g2.print("Exiting...");
          u8g2.sendBuffer();
          delay(2000);
          }
          else if(datastr[0]=='d'&&datastr[1]=='a'&&datastr[2]=='t'&&datastr[3]=='e')
          {
          *always_on=false;
          u8g2.setCursor(0, 42);
          u8g2.print("Edit date:x-x-x");
          u8g2.sendBuffer();
          ctr=0;
          for(int i=0;i<32;i++)
          {
            datastr[ctr++]=Serial.read();
            if(datastr[ctr-1]==-1)
              break;
          }
          datastr[--ctr]=0;
          u8g2.setCursor(0, 42);
          u8g2.print(datastr);
          u8g2.sendBuffer();
          deal_str_to_date(y,m,d,datastr);
          delay(2000);
          }
          else if(datastr[0]=='e'&&datastr[1]=='r'&&datastr[2]=='a'&&datastr[3]=='s')
          {
          u8g2.setCursor(0, 42);
          u8g2.print("Erasing...");
          u8g2.sendBuffer();
          *y=2022;
          *m=1;
          *d=1;
          delay(2000);
          u8g2.setCursor(0, 56);
          u8g2.print("Okay...");
          u8g2.sendBuffer();
          delay(1000);
          *always_on=false;
          }
          else if(datastr[0]=='s'&&datastr[1]=='a'&&datastr[2]=='v'&&datastr[3]=='e')
          {
          EEPROM.begin(1024);
          *always_on=false;
          u8g2.setCursor(0, 42);
          u8g2.print("Saving...");
          u8g2.sendBuffer();
          set_string(10,15,datastr);
          EEPROM.end();
          delay(2000);
          }
          else if(datastr[0]=='r'&&datastr[1]=='e'&&datastr[2]=='a'&&datastr[3]=='d')
          {
          EEPROM.begin(1024);
          *always_on=false;
          u8g2.setCursor(0, 42);
          u8g2.print("Reading...");
          u8g2.sendBuffer();
          get_string(10,15,datastr);
          EEPROM.end();
          deal_str_to_date(y,m,d,datastr);
          delay(2000);
          }
          else
          {
          u8g2.setCursor(0, 42);
          u8g2.print(datastr);
          u8g2.sendBuffer();
          delay(2000);
          }
      }
}




void usbing(int *y,int *m,int *d,int *dly)
{
  bool always_on=true;
  if(always_on)
          short_time_fun(&always_on,y,m,d,dly);
}



void setup()
{
    Serial.begin(115200);
    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_unifont_t_chinese2);
    pinMode(FLSKEY,INPUT);
}
void loop()
{
      
      static bool isusb=false;
      static bool is_startup=true;
      static int y=2022,m=1,d=1;
      int dly=25;
      if(isusb)
      {
      usbing(&y,&m,&d,&dly);
      isusb=false;
      }
      else
      {
        if(is_startup)
        {
          demo();
          is_startup=false;
        }
        
      isusb=calender(&y,&m,&d,&dly);
      }
      
}
