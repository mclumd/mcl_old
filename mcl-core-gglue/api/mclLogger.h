#ifndef MCL_LOG_HEADER
#define MCL_LOG_HEADER

#include <iostream>
#include <string>
using namespace std;

/** \file 
 * \brief a class that provides access to a flexible log stream.
 * a limited logger class that provides a static log stream called 
 * mclLog that can be set to cout, cerr, or a file.
 * everything being static introduces some serious limitations, the 
 * main one being that you can't write to more than one file.
 */

enum annotationMode { MCLA_TEXT, MCLA_HTML };

enum annotationType { MCLA_NONE,
		      MCLA_PRE,
		      MCLA_WARNING,   MCLA_ERROR,
		      MCLA_SUCCESS, MCLA_VIOLATION,
		      MCLA_LIST,
		      MCLA_MSG, MCLA_DBG, MCLA_VRB,

		      MCLA_HOSTMSG, MCLA_HOSTEVENT, MCLA_HOSTDBG, MCLA_HOSTVRB,
		      MCLA_HOSTERR,

		      MCLA_SEPERATOR,MCLA_BREAK
};

/** The class of flexible MCL log streams
 */
class mclLogger {
 public:
  mclLogger() {};

  //! sets the static log object to cout
  static void setLogToStdOut();
  //! sets the static log object to cerr
  static void setLogToStdErr();
  //! sets the static log object to a file with a dynamically generated name
  static void setLogToFile();
  //! sets the log file to the specified filename
  static void setLogToFile(string filename);

  //! convenience function that checks !mclSettings::quiet
  static void ifVerbose(string msg);

  //! annotation modes
  static void setAnnotateMode(annotationMode amode);

  //! annotation mode initializer
  static void annotateHeader();

  //! annotation mode filtering
  void setAnnotateFilter  (annotationType atype);
  //! annotation mode filtering
  void unsetAnnotateFilter(annotationType atype);

  //! convenience function that checks mclSettings::annotate
  static void annotate(annotationType atype);
  static void annotate(string annotation);
  static void annotate(annotationType atype, string annotation);
  //! convenience function that checks mclSettings::annotate
  //! @param lf if true, inserts a LineFeed after MSG
  static void annotate(annotationType atype, string annotation,bool lf);
  //! convenience function that checks mclSettings::annotate
  static void annotateStart(annotationType atype);
  static void annotateEnd(annotationType atype);
  //! uses annotateBuffer to dump to annotation stream
  static void annotateFromBuffer(annotationType atype);

  //! convenience function that checks mclSettings::debug
  static void ifDebug(string dmsg);
  //! convenience function that checks !mclSettings::quiet
  //! @param lf if true, inserts a LineFeed after MSG
  static void ifVerbose(string msg,bool lf);
  //! convenience function that checks mclSettings::debug
  //! @param lf if true, inserts a LineFeed after MSG
  static void ifDebug(string dmsg,bool lf);

  //! the stream to write to.
  //! should be dereferenced as
  //! *mclLogger::mclLog << "hello!";
  static ostream *mclLog;

  static char     annotateBuffer[1023];

 private:
  
  //! fileStream object in case we are logging to file
  static ofstream fileStream;
  //! closes fileStream if it is open
  static void closeIfFile();

};

#endif
