#ifndef __STRING_HPP__
#define __STRING_HPP__

#include <string.h>
#include <stdlib.h>
#include "charset.hpp"
#include <stdio.h>

class String {
public:
        char *data;
        int dataLength;

        String() {
                data = NULL;
                dataLength = 0;
        }

        const String &operator=(const String &b) {
                if (&b == this) return *this;
                if (b.data != NULL) {
                        dataLength = b.dataLength;
                        data = (char*)malloc(dataLength);
                        memcpy(data,b.data,dataLength);
                } else {
                        data = NULL;
                }
                return *this;
        }

        const bool operator==(const String &b) {
                if (b.length() != length())
                        return false;
                for (int i = 0; i < length(); i++) {
                        if (b[i] != (*this)[i])
                                return false;
                }
                return true;
        }

        const bool operator!=(const String &b) {
                return !(*this == b);
        }

        const bool operator<(const String &b) {
                int l = length();
                if (b.length()<l) l = b.length();
                for (int i = 0; i < l; i++) {
                        if (b[i]==(*this)[i]) continue;
                        return b[i]<(*this)[i];
                }
                return b.length()<length();
        }

        const bool operator>(const String &b) {
                int l = length();
                if (b.length()<l) l = b.length();
                for (int i = 0; i < l; i++) {
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

        const String &operator+=(int b) {
                add(String(b));
                return *this;
        }

        String(const String &b) {
                if (b.data != NULL)
                {
                        dataLength = b.dataLength;
                        data = (char*)malloc(dataLength);
                        memcpy(data,b.data,dataLength);
                } else {
                        data = NULL;
                }
        }

        String(const char *_data) {
                if (_data != NULL)
                {
                        dataLength = strlen(_data)+1;
                        data = (char*)malloc(dataLength);
                        memcpy(data,_data,dataLength);
                } else {
                        data = NULL;
                }
        }

        String(const int v) {
                data = (char*)malloc(11);
                sprintf(data,"%d",v);
                dataLength = strlen(data)+1;
        }

        ~String() {
              if (data != NULL) free(data);
        }

        void resize(int newLength) {
                if (data == NULL) {
                        dataLength = newLength + 1;
                        data = (char*)malloc(dataLength);
                        data[dataLength-1] = 0;
                } else {
                        if (newLength < dataLength) {
                                data[newLength] = 0;
                                dataLength = newLength + 1;
                        } else {
                                char *data2 = (char*)malloc(newLength + 1);
                                memcpy(data2,data,dataLength);
                                dataLength = newLength + 1;
                                data2[dataLength - 1] = 0;
                                free(data);
                                data = data2;
                        }
                }
        }

        int length() const {
                if (data == NULL) return 0;
                return dataLength - 1;
        }

        int pixelWidth() const {
                int width = 0;
                for (int l = 0; l < length(); l++) {
                        Letter *lt = getLetter((*this)[l]);
                        width += 1 + lt->width + lt->preAddX + lt->postAddX;
                }
                return width;

        }

        char &operator[](int index) const {
                return data[index];
        }

        bool empty() const {
                return length()==0;
        }

        static String chr(const char c) {
                String k = " ";
                k[0] = c;
                return k;
        }

        static String hex(long l, int len) {
                const char *hex="0123456789ABCDEF";
                String k; k.resize(len);
                for (int i = 0; i < len; i++) {
                        k[len-1-i] = hex[l & 15];
                        l>>=4;
                }
                return k;
        }

        char *c_str() const {
                return data;
        }

        void erase(int pos, int count) {
                memcpy(data+pos,data+pos+count,length()-(pos+count));
                dataLength -= count;
        }

        void insert(int pos, const String &str) {
                int l = length();
                dataLength = l+str.length()+1;
                char *newData = (char*)malloc(dataLength);
                memcpy(newData,data,pos);
                memcpy(newData+pos,str.data,str.length());
                memcpy(newData+pos+str.length(),data+pos,l-pos);
                newData[dataLength-1] = 0;
                if (data != NULL) free(data);
                data = newData;
        }

        void insert(int pos, const char c) {
                String k=" ";
                k[0]=c;
                insert(pos,k);
        }

        String substr(int firstIndex) const {
                if (firstIndex == -1) return "";
                String r;
                r.resize(length()-firstIndex);
                memcpy(&(r[0]),&((*this)[firstIndex]),r.length());
                return r;
        }

        String substr(int firstIndex, int lastIndex) const {
                if (lastIndex == -1) return substr(firstIndex);
                if (firstIndex == -1) return "";
                String r;
                r.resize(lastIndex-firstIndex);
                memcpy(&(r[0]),&((*this)[firstIndex]),r.length());
                return r;
        }

        int findLast(String v) const {
                int l1 = length();
                int l2 = v.length();
                for (int i = l1 - l2; i >= 0; i--) {
                        bool found = true;
                        for (int j = 0; j < l2; j++) {
                                if ((*this)[i+j]!=v[j]) {
                                        found = false;
                                        break;
                                }
                        }
                        if (found) return i;
                }
                return -1;
        }

        int findLast(int pos, String v) const {
                int l1 = pos;
                int l2 = v.length();
                for (int i = l1 - l2; i >= 0; i--) {
                        bool found = true;
                        for (int j = 0; j < l2; j++) {
                                if ((*this)[i+j]!=v[j]) {
                                        found = false;
                                        break;
                                }
                        }
                        if (found) return i;
                }
                return -1;
        }

        String last(int letters) const {
          if (length()<letters)
            return String(*this);
          return substr(length()-letters);
        }

        static String add(const String &a, const String &b);
        void add(const String &b) {
                if (b.data == NULL) return;
                int l = length();
                resize(l+b.length());
                memcpy(data+l,b.data,b.length()+1);
        }
};

String operator+(const String &a, const String &b);
String operator+(const char *a, const String &b);
String operator+(const String &a, const char *b);
String operator+(const int a, const String &b);
String operator+(const String &a, const int b);
String toLower(const String &v);
#endif //__STRING_HPP__