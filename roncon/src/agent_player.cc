#include "umbc/logger.h"
#include "umbc/token_machine.h"
#include "agent_player.h"
#include "player_configurer.h"
#include "controller.h"

using namespace raccoon;

agents::agent_player::agent_player(string aname,string hostname, int port) :
  agent_basic(aname),my_host(hostname),my_port(port),my_client(NULL) {
  my_client = new PlayerClient(my_host,my_port);
  set_status(player_config::configure(this));
};

bool agents::agent_player::initialize() {
  // get sensor proxies...
  cout << "getting proxies for " << get_name() << endl;
  my_client->RequestDeviceList();
  std::list<playerc_device_info_t> dev_l = my_client->GetDeviceList();
  for (std::list<playerc_device_info_t>::const_iterator dii = dev_l.begin();
       dii != dev_l.end();
       dii++) {
    cout << " device: " << *dii << endl;
  }
  return agent_basic::initialize();
}

bool agents::agent_player::monitor() {
  return agent_basic::monitor();
}

bool agents::agent_player::shutdown() {
  return true;
}

string agents::agent_player::query(string q) {
  umbc::tokenMachine tm(q);
  string req = tm.nextToken();
  cout << " processing query at agent_player level: '"
       << req << "'" << endl;
  if (strncmp(req.c_str(),"describe",8)==0)
    return okay_msg(describe());
  else
    return agents::agent_basic::query(q);
}

string agents::agent_player::command(string c) {
  return agents::agent_basic::command(c);
}

bool agents::agent_player::publish_grammar(umbc::command_grammar& cg) {
  return agent_basic::publish_grammar(cg);
}

string agents::agent_player::describe() {
  string tv="< "+get_name()+" ";
  if (!lasers.empty()) tv+="laser ";
  if (!p2ds.empty()) tv+="posn2d ";
  if (!vfhs.empty()) tv+="vfh ";
  tv+= "(active: ";
  for (list<controller*>::const_iterator aci = active_controllers.begin();
       aci != active_controllers.end();
       aci++) {
    if ((*aci) == NULL)
      tv+= "NULL ";
    else
      tv+= (*aci)->get_name()+" ";
  }
  tv+=")";
  tv+=">";
  return tv;
}

bool agents::agent_player::add_laser(int gIndex) {
  if (gIndex >= lasers.size()) lasers.resize(gIndex+1,NULL);
  if (lasers[gIndex] == NULL) {
    try {
      LaserProxy* nlp = new LaserProxy(my_client,gIndex);
      lasers[gIndex]=nlp;
    }
    catch  (PlayerCc::PlayerError e) {
      umbc::uLog::annotate(umbc::UMBCLOG_ERROR,e.GetErrorStr());
      return false;
    }
    return true;
  }
  else {
    umbc::uLog::annotate(umbc::UMBCLOG_ERROR,
			 "attempt to add a LaserProxy to \"" 
			 + get_name() + "\" at an already-occupied index.");
    return false;
  }
}

bool agents::agent_player::add_pp(int gIndex) {
  if (gIndex >= p2ds.size()) p2ds.resize(gIndex+1,NULL);
  if (p2ds[gIndex] == NULL) {
    try {
      Position2dProxy* nlp = new Position2dProxy(my_client,gIndex);
      p2ds[gIndex]=nlp;
    }
    catch  (PlayerCc::PlayerError e) {
      umbc::uLog::annotate(umbc::UMBCLOG_ERROR,e.GetErrorStr());
      return false;
    }
    return true;
  }
  else {
    umbc::uLog::annotate(umbc::UMBCLOG_ERROR,
			 "attempt to add a Postion2dProxy to \"" 
			 + get_name() + "\" at an already-occupied index.");
    return false;
  }
}

bool agents::agent_player::add_pp_vfh(int gIndex) {
  if ((gIndex < p2ds.size()) && (p2ds[gIndex] != NULL)) {
    umbc::uLog::annotate(umbc::UMBCLOG_ERROR,
			 "attempt to add a Postion2dProxy to \"" 
			 + get_name() + "\" at an already-occupied index.");
    return false;
  }
  else if ((gIndex < p2ds.size()) && (p2ds[gIndex] != NULL)) {
    umbc::uLog::annotate(umbc::UMBCLOG_ERROR,"attempt to add a VFN to \"" 
			 + get_name() + "\" at an already-occupied index.");
    return false;
  }
  else {
    if (gIndex >= p2ds.size()) p2ds.resize(gIndex+1,NULL);
    try {
      Position2dProxy* nlp = new Position2dProxy(my_client,gIndex);
      p2ds[gIndex]=nlp;
    }
    catch  (PlayerCc::PlayerError e) {
      umbc::uLog::annotate(umbc::UMBCLOG_ERROR,e.GetErrorStr());
      return false;
    }
    if (gIndex >= vfhs.size()) vfhs.resize(gIndex+1,NULL);
    try {
      Position2dProxy* vfh = new Position2dProxy(my_client,gIndex+1);
      vfhs[gIndex]=vfh;
    }
    catch  (PlayerCc::PlayerError e) {
      umbc::uLog::annotate(umbc::UMBCLOG_ERROR,e.GetErrorStr());
      return false;
    }
    return true;
  }
}
