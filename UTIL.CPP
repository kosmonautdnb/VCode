#include "util.hpp"
#include "types.hpp"
#include "dos.hpp"
#include <conio.h>

String dosFileName(const String &fileName);

const char *weekdays[8] = {"non","sun","mon","tue","wed","thu","fri","sat"};

String hex(int v) {
  char *h="0123456789ABCDEF";
  String r;
  r.resize(2);
  r[0] = h[v / 16];
  r[1] = h[v & 15];
  return r;
}

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

String stringFromFile(const String &fileName) {
  FILE *in = fopen(dosFileName(fileName).c_str(),"rb");
  if (in != NULL) {
    fseek(in,0,SEEK_END);
    long fileSize = ftell(in);
    fseek(in,0,SEEK_SET);
    String r;
    r.resize(fileSize);
    fread(&(r[0]),1,fileSize,in);
    fclose(in);
    for(int i = 0; i < r.length(); i++) {
      if (r[i]==0x0d) r.erase(i,1);
    }
    return r;
  }
  return NIL;
}