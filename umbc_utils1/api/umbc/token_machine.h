#ifndef UMBC_TOKEN_MACHINE_H
#define UMBC_TOKEN_MACHINE_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

#include "text_utils.h"

/** \file token_machine.h
 * \brief contains an improved string tokenizer
 *
 * String tokenizer and related functions for manipulating token stream.
 * Also subclass of token machine for parsing parenthesized parameter strings.
 *
 */

namespace umbc {

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
    void trimBraces();
    void trimWS();
    
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
      trimWS();
      trimParens();
      src= textFunctions::substChar(src, ',', ' ');
    };
    
    string getParam(int X);
    string getNextParam() { return nextToken(); };
    bool   hasMoreParams() { return moreTokens(); };
    bool   isLegit(string paramVal) { return paramVal != ""; };
    
  private:
    
  };

  class keywordParamMachine : public paramStringProcessor {
  public:
    keywordParamMachine(string plist) : paramStringProcessor(plist) { 
      trimBraces();
    };
    string getNextKWP() { return nextToken(); };
    string keyword_of(string token);
    string value_of(string token);

    string getKWV(string kw);

  private:

  };
  
};

#endif
