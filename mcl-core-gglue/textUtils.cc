#include "textUtils.h"
#include <ctype.h>
#include <errno.h>

//////////////////////////////////////////////////////////////////
//
//                     THIS IS TEXT UTILITIES !
//
//////////////////////////////////////////////////////////////////

double textFunctions::dubval(string q) {
  double r;
  char * poo;
  r=strtod(q.c_str(),&poo);
  return r;
}

string textFunctions::num2string(int q) {
  char buff[64];
  sprintf(buff,"%d",q);
  return buff;
}

string textFunctions::num2string(double q) {
  char buff[64];
  sprintf(buff,"%f",q);
  return buff;
}

int textFunctions::numval(string q) {
  errno = 0;
  long qv = strtol(q.c_str(),NULL,0);
  return qv;
}

unsigned int textFunctions::unumval(string q) {
  long qv = strtol(q.c_str(),NULL,0);
  return (unsigned int)qv;
}

int textFunctions::valueOrHash(string q) {
  int rv = numval(q);
  if (rv == 0)
    return hashKey(q);
  else
    return rv;
}

// int textFunctions::string2intSmart(string q) {
//   if (q[0] == '$')
//     // figure out how to get the key
//     return key_for_alltags(q.substr(1,q.size()-1));
//   else
//     return valueOrHash(q);
// }

int textFunctions::hashKey(string q) {
  int hk=0;
  for (int k=2; k < (int)q.length(); k++)
    hk = (hk<<4)^(hk>>28)^q[k];
  hk &= 0x0FFFFFFF;
  return hk;
}

void textFunctions::adjustProtectLevel(char c,int *p) {
  if (c == '(')
    (*p)++;
  else if (c == ')')
    (*p)--;
  if (*p < 0) *p=0;
}

bool textFunctions::isWS(char c) {
  return (isspace(c) != 0);
}

bool textFunctions::isEscape(char c) {
  return (c == '\\');
}

string textFunctions::readUntilWS(string k,int start,int end,int *nextStart) {
  // reads until ws char is found
  bool reading=false,escape=false;
  int protect=0,tstart=end;
  for (*nextStart=start;*nextStart < end;(*nextStart)++) {
    if (escape)     // eat the escape character
      escape=false;
    else {
      adjustProtectLevel(k[*nextStart],&protect); // up or down based on parens
      if (isEscape(k[*nextStart]))
	{ reading=true; escape=true; }
      else {
	if (isWS(k[*nextStart])) {
	  if (reading) {
	    if (protect == 0) { // terminate condition (isWS(char) && !protect)
	      return k.substr(tstart,*nextStart-tstart);
	    }
	  }
	}
	else if (!reading) {
	  reading = true;
	  tstart=*nextStart;
	}
      }
    }
  }
  if ((tstart == *nextStart) || (*nextStart > (int) k.size()))
    return "";
  else {
    return k.substr(tstart,*nextStart-tstart);
  }
}

void textFunctions::dumpTokens(string k) {
  int foo=0,x=0;
  while ((unsigned int)foo != k.size()) {
    x++;
    string q = readUntilWS(k,foo,k.size(),&foo);
  }
}

string textFunctions::file2string(string fn) {
  string rv="";
  ifstream in;
  in.open(fn.c_str());
  int x=0;
  if (in.is_open()) {
    string line;
    while (!in.eof()) {
      x++;
      getline (in,line);
      if (line.empty() || (line[0] == '#')) {
      }
      else if (rv.empty())
	rv=line;
      else rv+=" "+line;
    }
    in.close();
    return rv;
  }
  else return "";
}

string textFunctions::dequote(string src) {
  if ((src.size() >= 2) && (src[0] == '"') && (src[src.size()-1] == '"')) {
    src = src.substr(1,src.size()-2);
  }
  return src;
}

void textFunctions::dumpTokArrStr(string tas) {
  tokenMachine tm(tas);
  while (tm.moreTokens()) {
    string tt = tm.nextToken();
    std::replace( tt.begin(), tt.end(), '^', ' ' );
    cout << " " << tt << endl;
  }
}

string textFunctions::substChar(const string source, char c_was, char c_is) {
  string rv=source;
  for (int i=0;i<source.length();i++) {
    // printf("%d: %c versus %c\n",i,source.at(i),c_was);
    if (source.at(i) == c_was) {
      // printf("equals.\n");
      rv.replace(i,1,1,c_is);
    }
  }
  return rv;
}

string textFunctions::getFunctor(const string source) {
  for (int i=0; i<source.length(); i++) {
    if (source.at(i) == '(') {
      return source.substr(0,i);
    }
  }
  return source;
}

string textFunctions::getParameters(const string source) {
  int start=0,end=0;
  bool open=false;
  for (int i=0; i<source.length(); i++) {
    if (!open && (source.at(i) == '(')) {
      open=true;
      start=i;
    }
    else if (open && (source.at(i) == ')')) {
      end = i;
      return source.substr(start,end);
    } 
  }
  return "";
}

string textFunctions::trimBraces(string src) {
  if ((src[0] == '[') && (src[src.length()-1] == ']')) {
    // printf("parens mashed.\n");
    return src.substr();
  }
  return src;
}

///////////////////////////////////////////////////////////////////
///
///                  THIS IS THE TOKEN MACHINE !
///
///////////////////////////////////////////////////////////////////

void tokenMachine::trimParens() {
  int offset = ptr-ns;
  if ((src[ns] == '(') && (src[end-1] == ')')) {
    // printf("parens mashed.\n");
    ns++;
    end--;
  }
  ptr=ns+offset;
}

string tokenMachine::nextToken() {
  return textFunctions::readUntilWS(src,ptr,end,&ptr);
}

string tokenMachine::rest() {
  return src.substr(ptr,end);
}

bool tokenMachine::moreTokens() {
  int q=-1;
  string j = textFunctions::readUntilWS(src,ptr,end,&q);
  return (j.length() > 0);
}

bool tokenMachine::containsToken(string target) {
  int ptr_save=ptr;
  while (moreTokens()) {
    string j = nextToken();
    if (j == target) {
      ptr=ptr_save;
      return true;
    }
  }
  ptr=ptr_save;
  return false;
}

string tokenMachine::keywordValue(string keyword) {
  int ptr_save=ptr;
  while (moreTokens()) {
    string j = nextToken();
    if (j == keyword) {
      string rv;
      if (moreTokens())
	rv = nextToken();
      else 
	rv = "";
      ptr=ptr_save;
      return rv;
    }
  }
  ptr=ptr_save;
  return "";  
}

string tokenMachine::removeKWP(string keyword) {
  int ptr_save=ptr;
  string rv="";
  while (moreTokens()) {
    string j = nextToken();
    if (j == keyword) {
      if (moreTokens())
	nextToken();
    }
    else {
      rv+=j;
      if (moreTokens())
	rv+=" ";
    }
  }
  ptr=ptr_save;
  return rv;  
}

vector<string> tokenMachine::processAsStringVector(string q) {
  tokenMachine tm(q);
  tm.trimParens();
  bool running=true;
  vector<string> rv;
  while (running) {
    string nst = tm.nextToken();
    if (nst.size() > 0)
      rv.push_back(textFunctions::dequote(nst));
    else running=false;
  }
  return rv;
}

vector<double> tokenMachine::processAsDoubleVector(string q) {
  tokenMachine tm(q);
  tm.trimParens();
  bool running=true;
  vector<double> rv;
  while (running) {
    string nst = tm.nextToken();
    if (nst.size() > 0)
      rv.push_back(textFunctions::dubval(nst));
    else running=false;
  }
  return rv;
}

////////////////////////////////////////////////////////////////
// extension of the tokenMachine to do param list processing

string paramStringProcessor::getParam(int X) {
  int op = ptr;
  string rv="";
  reset();
  for (int k=0; ((k<=X) && moreTokens()); k++) {
    // cout << ".";
    if (k==X) rv=nextToken();
    else nextToken();
  }
  ptr=op;
  return rv;
}
