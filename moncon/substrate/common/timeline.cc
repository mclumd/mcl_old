#include "substrate/timeline.h"
#include "substrate/moncon_logging.h"
#include "mcl/mcl_api.h"
#include "umbc/token_machine.h"
#include <errno.h>
#include <algorithm>

char timeline_error_msg[1024];

using namespace MonCon;
using namespace umbc;

///////////////////////////////////////////////////////////
// timeline entry base class

void timeline_entry::repeat() {
  tline->delete_entry(this);
  fires+=fireorig;
  tline->add_entry(this);
}

///////////////////////////////////////////////////////////
// timeline base class

timeline::timeline(domain* d) : 
  simEntity("timeline"),knows_domain(d),understands_timeline_entries(),
  header("") { tl_index = line.begin(); }


void timeline::delete_entry(timeline_entry* tle) {
  tl_index = find(line.begin(),line.end(),tle);
  if (tl_index != line.end())
    line.erase(tl_index);
  update_tl_index();
}

void timeline::add_entry(timeline_entry* tle) {
  bool inserted = false;
  for (vector<timeline_entry*>::iterator tli = line.begin();
       tli != line.end();
       tli++) {
    if ((*tli)->firetime() > tle->firetime()) {
      line.insert(tli,tle);
      inserted = true;
      continue;
    }
  }
  if (!inserted) line.push_back(tle);
  update_tl_index();
}

void timeline::update_tl_index() {
  tl_index = line.begin();
  simTimeType now = umbc::declarations::get_declaration_count("simStep");
  while ((tl_index != line.end()) &&
	 ((*tl_index)->firetime() < now)) {
    tl_index++;
  }
}

void timeline::update() {
  simTimeType now = umbc::declarations::get_declaration_count("simStep");
  while ((tl_index != line.end()) &&
	 (now >= (*tl_index)->firetime())) {
    uLog::annotate(MONL_HOSTMSG,"FIREFIREFIREFIREFIREFIREFIREFIREFIRE");
    (*tl_index)->fire();
    tl_index++;
  }
}

timeline::~timeline() {
  //! behavior is to destroy on the way out... is that bad practice?
  vector<timeline_entry*>::iterator q = line.begin();
  while (q != line.end()) {
    timeline_entry* hp = *q;
    delete hp;
    q=line.erase(q);
  }
}

timeline_entry* timeline::parse_tle_spec( simTimeType firetime,
							  timeline* tl, 
							  string tles ) {
  // the timeline understands "exit"
  tokenMachine q(tles);
  string command = q.nextToken();
  if (command == "exit")
    return new exit_timeline_entry(firetime,tl);

  return NULL;
}

string timeline::describe() {
  char vv[128];
  sprintf(vv, "<timeline: %ld events>",(unsigned long)line.size());
  return vv;
}

///////////////////////////////////////////////////////////
// timeline entry subclasses

void unparsable_timeline_entry::fire() {
  uLog::annotate(MONL_HOSTMSG,"[mars/timeline]::unparsable entry firing: '"+msg+"'");
}

string unparsable_timeline_entry::describe() { 
  char vv[128];
  sprintf(vv, "<unparsed_entry '%s' t=%d>",msg.c_str(),fires);
  return vv;
}

string exit_timeline_entry::describe() { 
  char vv[128];
  sprintf(vv, "<exit_entry t=%d>",fires);
  return vv;
}

///////////////////////////////////////////////////////////
// Timeline Factory
// for restoring from file

bool exit_on_parsefail = true;

void tlFactory::do_not_exit_on_parse_failure() {
  exit_on_parsefail=false;
}

timeline* tlFactory::create_timeline(domain* d) {
  return new timeline(d);
}

int global_parse_line = 0;

timeline* tlFactory::read_timeline(domain* d,string filename) {
  timeline* ntl = new timeline(d);
  read_to_timeline(*ntl,filename);
  return ntl;
}

void tlFactory::read_to_timeline(timeline& tl, string filename) {
  global_parse_line=0;
  ifstream in;
  in.open(filename.c_str());
  if (in.is_open()) {
    string line;
    // read the timeline header, <run checks??>
    getline(in,line);
    tl.set_header(line);
    while (!in.eof()) {
      global_parse_line++;
      getline (in,line);
      if (line.empty() || (line[0] == '#')) { }
      else {
	timeline_entry* tlent = parse_entry(tl,line);
	if ( tlent != NULL ) {
	  tl.add_entry( tlent );
	  cout << tlent->describe() << endl;
	}
      }
    }
    in.close();
  }
  else {
    uLog::annotate(MONL_HOSTERR,"could not recover timeline from '" +
			filename + "'");
  }
}

bool tlFactory::read_entry_to_timeline(timeline& tl, string entry) {
  timeline_entry* tlent = parse_entry(tl,entry);
  if ( tlent != NULL ) {
    tl.add_entry( tlent );
    cout << tlent->describe() << endl;
    return true;
  }
  else return false;
}

timeline_entry * tlFactory::parse_entry( timeline& tl, string entry ) {
  timeline_entry* rv=NULL;
  sprintf(timeline_error_msg,"unspecified error.");
  // start by pulling off "at xxx"
  tokenMachine q(entry);
  string at = q.nextToken();
  //
  // (check to see that it is 'at' or 'every'
  bool repeating = false;
  if (at == "at") {}
  else if (at == "every") repeating=true;
  else {
    if (exit_on_parsefail) {
      cerr << "Error parsing timeline: [frequency] (line " << global_parse_line 
	   << "): schedule items must be 'at' or 'every'"  << endl;
      exit(EXIT_FAILURE);
    }
    // else keep going and it will be unparsable event
  }
  // 
  // determine event time
  string when_s = q.nextToken();
  int when_n = textFunctions::numval(when_s);
  if (errno != 0) {
    if (exit_on_parsefail) {
      cerr << "Error parsing timeline [numval]: (line " << global_parse_line 
	   << "): firetime '"+when_s+"' not parseable as a number." << endl;
      exit(EXIT_FAILURE);
    }
    // else keep going and it will be unparsable event
  }
  //
  // parse / delegate timeline event
  string enspec = q.rest();
  //
  // see if the timeline class can parse it
  rv = tl.parse_tle_spec( when_n , &tl, enspec );
  if ( rv == NULL ) {
    //
    // next see if the domain can parse it
    // the domain should delegate to agents if it cannot
    understands_timeline_entries* daute =
      dynamic_cast<understands_timeline_entries*>(tl.domain_of());
    if (daute != NULL) {
      rv = daute->parse_tle_spec( when_n, &tl, enspec );
    }
    else {
      // should never happen
      sprintf(timeline_error_msg,"[mars/timeline]:: domain cannot be cast to understands_timeline_entries.");      
    }
  }
  //
  // assume unparsable...
  if (rv == NULL) {
    if (exit_on_parsefail) {
      cerr << "Error parsing timeline [unparsed]: (line " << global_parse_line 
	   << "): " << enspec << endl << " > " << timeline_error_msg << endl;
      exit(EXIT_FAILURE);
    }
    else rv = new unparsable_timeline_entry( when_n, &tl, enspec );
  }
  if (rv != NULL)
    rv->set_repeating(repeating);
  return rv;
}

