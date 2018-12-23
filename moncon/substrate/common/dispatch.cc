#include "substrate/dispatch.h"
#include "umbc/settings.h"
#include "umbc/text_utils.h"

using namespace MonCon;
using namespace umbc;

string dispatch::success_prefix = "ok";
string dispatch::ignored_prefix = "ignored";
string dispatch::fail_prefix = "fail";

string dispatch::generate_success_msg() {
  if (settings::getSysPropertyString("moncon.reply","telnet") == "prolog")
    return success_prefix + "(1)";
  else return success_prefix + "()";
}

string dispatch::generate_success_msg(string annotation) {
  if ((annotation == "") &&
      (settings::getSysPropertyString("moncon.reply","telnet") == "prolog"))
    return success_prefix + "(1)";
  else return success_prefix + "(" + annotation + ")";
}

string dispatch::generate_ignored_msg() {
  if (settings::getSysPropertyString("moncon.reply","telnet") == "prolog")
    return ignored_prefix + "(1)";
  else return ignored_prefix + "()";
}

string dispatch::generate_ignored_msg(string annotation) {
  if ((annotation == "") &&
      (settings::getSysPropertyString("moncon.reply","telnet") == "prolog"))
    return ignored_prefix + "(1)";
  else return ignored_prefix + "(" + annotation + ")";
}

string dispatch::generate_fail_msg() {
  if (settings::getSysPropertyString("moncon.reply","telnet") == "prolog")
    return fail_prefix + "(1)";
  else return fail_prefix + "()";
}

string dispatch::generate_fail_msg(string annotation) {
  if ((annotation == "") &&
      (settings::getSysPropertyString("moncon.reply","telnet") == "prolog"))
    return fail_prefix + "(1)";
  else return fail_prefix + "(" + annotation + ")";
}

bool dispatch::is_success_msg(string msg) { 
  return ((msg.length() >= success_prefix.length()) &&
	  (msg.substr(0,success_prefix.length()) == success_prefix));
}

bool dispatch::is_ignored_msg(string msg) { 
  return ((msg.length() >= ignored_prefix.length()) &&
	  (msg.substr(0,ignored_prefix.length()) == ignored_prefix));
}

bool dispatch::is_fail_msg(string msg) { 
  return ((msg.length() >= fail_prefix.length()) &&
	  (msg.substr(0,fail_prefix.length()) == fail_prefix));
}

string dispatch::prologify(string msg) {
  string pmsg = textFunctions::substChar(msg,'.',' ');
  return "term(af(message_from_domain("+pmsg+"))).";
}
