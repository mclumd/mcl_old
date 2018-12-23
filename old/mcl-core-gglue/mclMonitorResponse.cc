#include "mclMonitorResponse.h"
#include "APICodes.h"

string crc_names[CRC_MAX]=
  { "ignore violation", "do nothing",
    "try again",
    "solicit help from user", "relinquish control to user",
    "run sensor diagnostic", "run effector diagnostic",
    "reset a sensor","reset an effector",
    "activate learning", "adjust parameters", "rebuild models",
    "revisit assumptions", "amend controller",
    "revise expectations", "swap algorithm", "change hlc",
  };

string respName(pkType crc) {
  return crc_names[crc_offset_base_zero(crc)]; 
}

