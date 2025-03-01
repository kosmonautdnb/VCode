#ifndef __STRING_HPP__                  
#define __STRING_HPP__

#include <string.h>
#include <stdlib.h>
#include "charset.hpp"
#include <stdio.h>
#include "types.hpp"

class String {
public:
  char *data;
  int64_t dataLength;
  
  String() {
    data = NULL;
    dataLength = 0;
  }
  
  const String &operator=(const String &b) {
    if (&b == this) return *this;
    if (b.data != NULL) {
            if (data != NULL) {
              delete[] data;
            }
            dataLength = b.dataLength;
            data = new char[dataLength];
            memcpy(data,b.data,dataLength);
    } else {
            data = NULL;
    }
    return *this;
  }
  
  const bool operator==(const String &b) {
    if (b.length() != length())
      return false;
    for (int64_t i = 0; i < length(); i++) {
      if (b[i] != (*this)[i])
              return false;
    }
    return true;
  }
  
  const bool operator!=(const String &b) {
    return !(*this == b);
  }
  
  const bool operator<(const String &b) {
    int64_t l = length();
    if (b.length()<l) l = b.length();
    for (int64_t i = 0; i < l; i++) {
      if (b[i]==(*this)[i]) continue;
      return b[i]<(*this)[i];
    }
    return b.length()<length();
  }
  
  const bool operator>(const String &b) {
    int64_t l = length();
    if (b.length()<l) l = b.length();
    for (int64_t i = 0; i < l; i++) {
      if (b[i]==(*this)[i]) continue;
      return b[i]>(*this)[i];
    }
    return b.length()>length();
  }
  
  const String &operator+=(const String &b) {
    add(b);
    return *this;
  }
  
  const String &operator+=(const char *b) {
    add(String(b));
    return *this;
  }
  
  const String &operator+=(int64_t b) {
    add(String(b));
    return *this;
  }
  
  String(const String &b) {
    if (b.data != NULL)
    {
      dataLength = b.dataLength;
      data = new char[dataLength];
      memcpy(data,b.data,dataLength);
    } else {
      data = NULL;
    }
  }
  
  String(const char *_data) {
    if (_data != NULL)
    {
      dataLength = strlen(_data)+1;
      data = new  char[dataLength];
      memcpy(data,_data,dataLength);
    } else {
      data = NULL;
    }
  }
  
  String(const int64_t v) {
    String b;
    b.resize(32);
    uint64_t va = v;
    if (v < 0) va = -v;
    int i = b.length();
    do {
      i--;
      b[i]=(va % 10)+'0';
      va /= 10;
    } while(va>0);
    if (v < 0) {i--;b[i]='-';}
    dataLength = b.dataLength-i;
    data = new char[dataLength];
    memcpy(data,b.data+i,dataLength);
  }
  
  ~String() {
    if (data != NULL) delete[] data;
  }

  void clear() {
    if (data != NULL) delete[] data;
    data = NULL;
    dataLength = 0;
  }
  
  void resize(int64_t newLength) {
    if (data == NULL) {
      dataLength = newLength + 1;
      data = new char[dataLength];
      data[dataLength-1] = 0;
    } else {
      if (newLength < dataLength) {
        data[newLength] = 0;
        dataLength = newLength + 1;
      } else {
        char *data2 = new char[newLength + 1];
        memcpy(data2,data,dataLength);
        dataLength = newLength + 1;
        data2[dataLength - 1] = 0;
        delete[] data;
        data = data2;
      }
    }
  }
  
  int64_t length() const {
    if (data == NULL) return 0;
    return dataLength - 1;
  }
  
  int64_t pixelWidth() const {
    int64_t width = 0;
    for (int64_t l = 0; l < length(); l++) {
      Letter *lt = getLetter((*this)[l]);
      if (l != 0) width += 1;
      width += lt->width + lt->preAddX + lt->postAddX;
    }
    return width;
  }
  
  char &operator[](int64_t index) const {
    return data[index];
  }
  
  bool empty() const {
    return length()==0;
  }
  
  int64_t toInt() {
    if (empty()) return 0;
    int64_t number = 0;
    bool sign = false;
    for(int64_t i = 0; i < length(); i++) {
      char c = (*this)[i];
      if (c>='0'&&c<='9') {
        number *= 10;
        number += c - '0';
      }
      if (number==0&&c=='-') sign=true;
    }
    if (sign) number=-number;
    return number;
  }

  int64_t fromHex() {
    if (empty()) return 0;
    int64_t number = 0;
    bool sign = false;
    for(int64_t i = 0; i < length(); i++) {
      char c = (*this)[i];
      if (c>='0'&&c<='9') {
        number *= 16;
        number += c - '0';
      }
      if (c>='a'&&c<='f') {
        number *= 16;
        number += c - 'a' + 10;
      }
      if (c>='A'&&c<='F') {
        number *= 16;
        number += c - 'A' + 10;
      }
      if (number==0&&c=='-') sign=true;
    }
    if (sign) number=-number;
    return number;
  }
  
  static String chr(const char c) {
    String k = " ";
    k[0] = c;
    return k;
  }
  
  static String hex(int64_t l, int64_t len) {
    const char *hex="0123456789ABCDEF";
    String k; k.resize(len);
    for (int64_t i = 0; i < len; i++) {
      k[len-1-i] = hex[l & 15];
      l>>=4;
    }
    return k;
  }
  
  static String flt(float v) {
    String k;
    k.resize(16);
    sprintf(&(k[0]),"%f",v);
    k.resize(strlen(k.c_str()));
    return k;
  }

  float toFloat() {
    return atof(c_str());
  }  

  char *c_str() const {
    return data;
  }
  
  void erase(int64_t pos, int64_t count) {
    memcpy(data+pos,data+pos+count,length()-(pos+count));
    dataLength -= count;
  }
  
  void insert(int64_t pos, const String &str) {
    int64_t l = length();
    dataLength = l+str.length()+1;
    char *newData = new char[dataLength];
    if (pos>0)  memcpy(newData,data,pos);
    memcpy(newData+pos,str.data,str.length());
    memcpy(newData+pos+str.length(),data+pos,l-pos);
    newData[dataLength-1] = 0;
    if (data != NULL) delete[] data;
    data = newData;
  }
  
  void insert(int64_t pos, const char c) {
    insert(pos,String::chr(c));
  }
  
  String substr(int64_t firstIndex) const {
    if (firstIndex == -1) return "";
    String r;
    r.resize(length()-firstIndex);
    memcpy(&(r[0]),&((*this)[firstIndex]),r.length());
    return r;
  }
  
  String substr(int64_t firstIndex, int64_t lastIndex) const {
    if (lastIndex == -1) return substr(firstIndex);
    if (firstIndex == -1) return "";
    String r;
    r.resize(lastIndex-firstIndex);
    memcpy(&(r[0]),&((*this)[firstIndex]),r.length());
    return r;
  }
  
  int64_t findLast(String v) const {
    int64_t l1 = length();
    int64_t l2 = v.length();
    for (int64_t i = l1 - l2; i >= 0; i--) {
      bool found = true;
      for (int64_t j = 0; j < l2; j++) {
        if ((*this)[i+j]!=v[j]) {
          found = false;
          break;
        }
      }
      if (found) return i;
    }
    return -1;
  }
  
  int64_t findLast(int64_t pos, String v) const {
    int64_t l1 = pos;
    int64_t l2 = v.length();
    for (int64_t i = l1 - l2; i >= 0; i--) {
      bool found = true;
      for (int64_t j = 0; j < l2; j++) {
        if ((*this)[i+j]!=v[j]) {
          found = false;
          break;
        }
      }
      if (found) return i;
    }
    return -1;
  }
  
  String last(int64_t letters) const {
    if (length()<letters)
      return String(*this);
    return substr(length()-letters);
  }
  
  static String add(const String &a, const String &b);
  void add(const String &b) {
    if (b.data == NULL) return;
    int64_t l = length();
    resize(l+b.length());
    memcpy(data+l,b.data,b.length()+1);
  }
};

String operator+(const String &a, const String &b);
String operator+(const char *a, const String &b);
String operator+(const String &a, const char *b);
String operator+(const int64_t a, const String &b);
String operator+(const String &a, const int64_t b);
String toLower(const String &v);
#endif //__STRING_HPP__