#ifndef TEXT_UTILS_H
#define TEXT_UTILS_H

/** \file textUtils.h
 * \brief contains an improved string tokenizer and textFunctions class.
 *
 * String tokenizer and various static text processing functions in the 
 * textFunctions class.
 */

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

/** provides static functions for text processing. */
class textFunctions {
 public:
  textFunctions() {};
  //! dumps all the tokens as parsed by the tokenMachine class.
  static void dumpTokens(string k);
  //! reads a text file and returns its contents as a string.
  static string file2string(string fn);
  /** reads from a a string until whitespace is encountered.
   * \param k the input string
   * \param start the index into k at which to start
   * \param end the index into k at which to stop
   * \param nextStart the index of the first encountered whitespace character 
   * \return the non whitespace text before the first whitespace character
   */
  static string readUntilWS(string k,int start,int end,int *nextStart);
  //! tests if a character is considered whitespace.
  static bool isWS(char c);
  //! tests if a character is considered an escape-code ('\').
  static bool isEscape(char c);
  //! returns a string value for an number
  static string num2string(int k);
  static string num2string(double k);
  //! returns the numerical (integer) value of a string.
  static int numval(string q);
  //! returns the unsigned numerical (integer) value of a string.
  static unsigned int unumval(string q);
  //! computes a hash key for a string.
  static int hashKey(string q);
  //! returns either the value of a numerical string or its hash value if it is not numerical.
  static int valueOrHash(string q);
  //! removes quotation marks from a string
  static string dequote(string s);
  //! returns the double-precision numberical value of a string
  static double dubval(string q);
  //! substitutes one character for another in a string
  static string substChar(const string source, char c_was, char c_is);

  static string getFunctor(const string source);
  static string getParameters(const string source);

  //! detokenizes a TokArr for PNL output
  static void dumpTokArrStr(string tas);

  static string chopLastChar(string k) { return k.substr(0,k.length()-1); };
  static string trimBraces(string src);

 private:
  static void adjustProtectLevel(char c,int *protect);

};

/** an improved string tokenizer.
 * provides string tokenizing similar to java's StringTokenizer, with 
 * additional support for parenthesis-delimited substrings.
 */
class tokenMachine {
 public:
  tokenMachine(string source) : src(source),ns(0),end(source.size()),ptr(0) {};
  string nextToken(); //<! gets the next token in the string
  string rest(); //<! returns the unprocessed remainder of the source
  //! true if there is additional text (tokens)
  bool moreTokens();

  //! true if the token 'target' exists in the remainder of the source string
  // starting with ptr
  // restores ptr on exit
  bool containsToken(string target);

  //! returns the next token after the first occurence of 'target'
  // starting with ptr
  // restores ptr on exit
  string keywordValue(string target);

  //! returns a string missing the keyword-pair(s) specified by 'keyword'
  // starting with ptr
  // restores ptr on exit
  string removeKWP(string keyword);

  //! resets the token pointer to the beginning of the source string
  void reset() { ptr = ns; };
  //! removes parens from the beginning and end of the source string
  void trimParens(); 

  void printSource() { cout << "source='" << src << "'" << endl; };
  void printWSource() { cout << "w_source='" << src.substr(ns,end-ns) << "'" << endl; };

  //! produces a vector of strings from a whitespace-delimited source string.
  static vector<string> processAsStringVector(string q);
  //! produces a vector of doubles from a whitespace-delimited source string.
  static vector<double> processAsDoubleVector(string q);

 protected:
  string src;
  int ns,end,ptr;
};

class paramStringProcessor : public tokenMachine {
 public:
  paramStringProcessor(string plist) : tokenMachine(plist) {
    trimParens();
    src= textFunctions::substChar(src, ',', ' ');
  };

  string getParam(int X);
  string getNextParam() { return nextToken(); };
  bool   hasMoreParams() { return moreTokens(); };
  bool   isLegit(string paramVal) { return paramVal != ""; };

 private:

};

#endif
