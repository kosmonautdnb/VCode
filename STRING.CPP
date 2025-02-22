#include "string.hpp"

String String::add(const String &a, const String &b) {
        if (a.data == NULL) return b.data == NULL ? String("") : String(b);
        if (b.data == NULL) return String(a);
        String r;
        r.resize(a.length()+b.length());
        memcpy(r.data,a.data,a.length());
        memcpy(r.data+a.length(),b.data,b.length()+1);
        return r;
}

String operator+(const String &a, const String &b) {
        return String::add(a,b);
}

String operator+(const char *a, const String &b) {
        return String(a) + b;
}

String operator+(const String &a, const char *b) {
        return a + String(b);
}

String operator+(const long a, const String &b) {
        return String(a) + b;
}

String operator+(const String &a, const long b) {
        return a + String(b);
}

String toLower(const String &v) {
  String r = v;
  for (int64_t i = 0; i < r.length(); i++) {
    if (r[i]>='A'&&r[i]<='Z') r[i]=r[i]-'A'+'a';
  }
  return r;
}