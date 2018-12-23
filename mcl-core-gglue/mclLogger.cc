#include "mclLogger.h"
#include <iostream>
#include <fstream>
#include "mclTimer.h"
#include "mclSettings.h"
#include <list>

ostream *mclLogger::mclLog = &cout;
ofstream mclLogger::fileStream ;

void mclLogger::setLogToStdOut() {
  closeIfFile();
  mclLog = &cout;
}

void mclLogger::setLogToStdErr() {
  closeIfFile();
  mclLog = &cerr;
}

void mclLogger::setLogToFile() {
  char fname[255];
  sprintf(fname,"logs/%9lf.log",mclTimer::getTimeDouble());
  string poop = fname;
  setLogToFile(poop);
}

void mclLogger::setLogToFile(string filename) {
  closeIfFile();
  fileStream.open(filename.c_str(),ios::out);
  if (fileStream.is_open()) {
    mclLog = &fileStream;
    setAnnotateMode(MCLA_HTML);
    annotateHeader();
  }
  else {
    cerr << "WARNING: could not open " << filename << " for logging." << endl;
    setLogToStdErr();
  }
}

void mclLogger::closeIfFile() {
  if (fileStream.is_open()) {
    fileStream.close();
  }
}

void mclLogger::ifVerbose(string msg) { ifVerbose(msg); }

void mclLogger::ifVerbose(string msg,bool lf) {
  if (!mclSettings::quiet) {
    *mclLogger::mclLog << msg;
    if (lf) *mclLogger::mclLog << endl;
  }
}

void mclLogger::ifDebug(string dmsg) { ifDebug(dmsg,false); }

void mclLogger::ifDebug(string dmsg,bool lf) {
  if (mclSettings::debug) {
    *mclLogger::mclLog << dmsg;  
    if (lf) *mclLogger::mclLog << endl;
  }
}

////////////////////////////////////////////////////////////////////
//// annotation stuff...

string annote_open(annotationType atype);
string annote_close(annotationType atype);
string annote_lf();
unsigned int mcla_filter = 0;
annotationMode annotate_mode = MCLA_TEXT;
list<annotationType> annotate_suppress;
int   indentLevel = 0;
char  mclLogger::annotateBuffer[1023];

void mclLogger::setAnnotateMode(annotationMode amode) {
  annotate_mode=amode;
}

void mclLogger::setAnnotateFilter(annotationType atype) {
  mcla_filter |= (1 << atype);
}

void mclLogger::unsetAnnotateFilter(annotationType atype) {
  mcla_filter &= !(1 << atype); 
}

inline bool filter(annotationType atype) {
  return (0 != (mcla_filter & (1 << atype)));
}

void mclLogger::annotateHeader() {
  *mclLog << "<HEAD>\n"
	  << " <STYLE type=\"text/css\">\n"
	  << "  span { font-family: courier }\n"
	  << "  .error { font-weight : bold; font-size : 120%; color : red }\n"
	  << "  .warning { font-weight : bold; color : orange }\n"
	  << "  .vio     { background : #CCCCFF; color : #440000 }\n"
	  << "  .success { color : green }\n"
	  << "  .host { color : blue }\n"
	  << "  .hostx { font-weight : bold; color : blue }\n"
	  << "  .dbghost { font-size : 75%; color : blue }\n"
	  << " </STYLE>\n"
	  << "</HEAD>\n";
}

bool _suppress(annotationType atype) {
  // suppress if:
  //   annotate settings is off
  //   atype is filtered
  //   there is a type on the suppression vector
  return (!mclSettings::annotate ||
	  filter(atype) ||
	  (annotate_suppress.size() > 0));
}

void mclLogger::annotateFromBuffer(annotationType atype) { 
  annotate(atype,annotateBuffer,true); 
}

void mclLogger::annotate(annotationType atype) { 
  annotate(atype,"",false); 
}

void mclLogger::annotate(string annotation) { 
  annotate(MCLA_MSG,annotation,false); 
}

void mclLogger::annotate(annotationType atype, string annotation) { 
  annotate(atype,annotation,true); 
}

void mclLogger::annotate(annotationType atype, string annotation, bool lf) {
  if (!_suppress(atype)) {
    // bizarre, but it helps in debug
    string out = annote_open(atype);
    out += annotation;
    out += annote_close(atype);
    *mclLog << out;
    if (lf) {
      *mclLog << annote_lf() << endl;
    }
  }
}

void mclLogger::annotateStart(annotationType atype) {
  if (filter(atype)) annotate_suppress.push_front(atype);
  if (!_suppress(atype)) {
    *mclLog << annote_open(atype) << endl;
  }
}

void mclLogger::annotateEnd(annotationType atype) {
  if (annotate_suppress.size() > 0) {
    if (annotate_suppress.front() == atype) 
      annotate_suppress.pop_front();
    else {
      cerr << "MCL_LOG_ERROR ~~> failed to match end " << atype 
	   << " with start " << annotate_suppress.front() << endl
	   << "                  annotation may become non-functional."
	   << endl;
    }
  }
  if (!_suppress(atype)) {
    *mclLog << annote_close(atype) << endl;
  }  
}

string annote_open(annotationType atype) { 
  if (annotate_mode == MCLA_HTML) {
    switch (atype) {
    case MCLA_PRE:
      return "<pre size=\"-1\">";
    case MCLA_ERROR:
    case MCLA_HOSTERR:
      return "<span class=\"error\">";
    case MCLA_WARNING:
      return "<span class=\"warning\">";
    case MCLA_SUCCESS:
      return "<span class=\"success\">";
    case MCLA_VIOLATION:
      return "<span class=\"vio\">";
    case MCLA_SEPERATOR:
      return "<hr>";
    case MCLA_BREAK:
      return "<br>";
    case MCLA_MSG:
    case MCLA_VRB:
      return "<span class=\"msg\">";      
    case MCLA_HOSTMSG:
    case MCLA_HOSTVRB:
    case MCLA_HOSTDBG:
      return "<span class=\"host\">";      
    case MCLA_HOSTEVENT:
      return "<span class=\"hostx\">";      
    default:
      // cout << "open returning msg span." << endl;
      return "<span>";
    }
  }
  else return "";
}

string annote_close(annotationType atype) { 
  if (annotate_mode == MCLA_HTML) {
    switch (atype) {
    case MCLA_PRE:
      return "</pre>";
    default:
      return "</span>";
    }
  }
  else return "";
}

string annote_lf() { 
  if (annotate_mode == MCLA_HTML)
    return "<br>\n";
  else return ""; 
}
