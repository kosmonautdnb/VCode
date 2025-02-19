#include "util.hpp"
#include "types.hpp"
#include "dos.hpp"
#include <conio.h>
#include "vga.hpp"
#include "valert.hpp"

String dosFileName(const String &fileName);
bool fileWasWritten = false;

const char *weekdays[8] = {"non","sun","mon","tue","wed","thu","fri","sat"};

String hex(int v) {
  char *h="0123456789ABCDEF";
  String r;
  r.resize(2);
  r[0] = h[v / 16];
  r[1] = h[v & 15];
  return r;
}

String currentToolString="";
String currentTimeString() {
  int rtc_seconds = rtcRead(0);
  int rtc_minutes = rtcRead(2);
  int rtc_hours   = rtcRead(4);
  int rtc_weekday = rtcRead(6);
  int rtc_day     = rtcRead(7);
  int rtc_month   = rtcRead(8);
  int rtc_year    = rtcRead(9);
  return hex(rtc_day)+"."+hex(rtc_month)+"."+hex(rtc_year)+" "+hex(rtc_hours)+":"+hex(rtc_minutes)+"."+hex(rtc_seconds);
}

void make0d0ato0a(String &r) {
  for (int i = 0; i < r.length()-1; i++) {
    char c = r[i];
    if (c==0x0d&&r[i+1]==0x0a) {
      r.erase(i,1);
      i--;
    }
  }
}

void make0ato0d0a(String &r) {
  for (int i = 0; i < r.length(); i++) {
    if (r[i]==0x0a) {
      r.insert(i,String::chr(0x0d));
      i++;
    }
  }
}

String stringFromFile(const String &fileName, bool textFile) {
  FILE *in = fopen(dosFileName(fileName).c_str(),"rb");
  if (in != NULL) {
    setPalette(0,0xffffffff);
    fseek(in,0,SEEK_END);
    long fileSize = ftell(in);
    fseek(in,0,SEEK_SET);
    String r;
    r.resize(fileSize);
    fread(&(r[0]),1,fileSize,in);
    fclose(in);
    if (textFile)
      make0d0ato0a(r);
    setPalette(0,0xff000000);
    return r;
  }
  return NIL;
}

void stringToFile(const String &string,const String &fileName, bool textFile) {
  fileWasWritten = true;
  FILE *out = fopen(dosFileName(fileName).c_str(),"wb");
  if (out != NULL) {
    setPalette(0,0xffffffff);
    String s2 = string;
    if (textFile)
      make0ato0d0a(s2);
    fwrite(&(s2[0]),1,s2.length(),out);
    fclose(out);
    setPalette(0,0xff000000);
  }
}

bool copyFiles(const String &destPath, Array<String> &sourcePaths) {
  int a = VAlert::box("Copy "+String(sourcePaths.size())+" Files", "YES","NO");
  return a == 1;
}