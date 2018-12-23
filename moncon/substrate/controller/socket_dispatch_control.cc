#include "substrate/socket_dispatch_control.h"
#include "substrate/dispatch_agent.h"
#include "substrate/moncon_logging.h"
#include "umbc/settings.h"
#include "umbc/text_utils.h"
#include "mcl/mcl_api.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>

using namespace std;
using namespace MonCon;
using namespace umbc;

#define SYNCH_USLEEP_PER_WAIT 100000

int  socket_dispatch_control::DEFAULT_PORT_NUMBER = 8400;

void *_sdc_start_server(void *argptr);
void  _sdc_error(const char *msg);
enum process_return_code_t 
  { UNPROCESSED_COMMAND_BADCOMMAND,
    UNPROCESSED_COMMAND_BADARG,
    UNPROCESSED_COMMAND_IOFAIL,
    INSTRUCTION_ACCEPTED,
    REQUEST_PROCESSED,
    DONOT_ACK
  };

string waiting_reply;    

socket_dispatch_control::socket_dispatch_control
 (basic_dispatch_agent* a,string c, bool blocking) :
   dispatch_control_core(a,c),block(blocking),synchStepOK(false),
   stepWhileBusy(false) {
  int pthread_res = pthread_create(&server_thread,NULL,_sdc_start_server,this);
  if (pthread_res < 0)
    _sdc_error("failed to create server thread (exiting)...");
  pthread_detach(server_thread);
}

bool socket_dispatch_control::control_in() {
  if (block) { 
    while (!synchStepOK) {
      usleep(SYNCH_USLEEP_PER_WAIT);
    }
    if (stepWhileBusy) {
      synchStepOK = false;
      for (agent_list::iterator ali = domain_of()->al_start();
	   ali != domain_of()->al_end();
	   ali++) {
	if ((*ali)->busy()) synchStepOK = true;
      }
    }
    else {
      stepWhileBusy = synchStepOK = false;
    }
  }
  return true;
}

bool socket_dispatch_control::control_out()   { return true; }

// local stuff....

void _sdc_error(const char *msg)
{
    perror(msg);
    exit(1);
}

void *_sdc_start_server(void *argptr) {
  socket_dispatch_control* sbc = static_cast<socket_dispatch_control*>(argptr);
  int sockfd, newsockfd, clilen;
  int use_port=settings::getSysPropertyInt("moncon.port",socket_dispatch_control::DEFAULT_PORT_NUMBER);
  bool up=false;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    _sdc_error("ERROR opening socket");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(use_port);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
	   sizeof(serv_addr)) < 0) 
    _sdc_error("ERROR on binding");
  cout << "[mcs/sock-d]:: listening to port " << dec << use_port << endl;
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  while (1) {
    newsockfd = accept(sockfd, 
		       (struct sockaddr *) &cli_addr, 
		       ((socklen_t*)&clilen));
    cout << "[mcs/sock-d]:: connection from client initiated..." << endl;
    if (newsockfd < 0) 
      _sdc_error("ERROR on accept");
    else up=true;
    while (up) {
      bzero(buffer,256);
      n = read(newsockfd,buffer,255);
      if (n < 0) _sdc_error("ERROR reading from socket");
      int q = 1;
      uLog::annotate(MONL_HOSTEVENT,"[mcs/sock-d]::command received on socket = '"+textFunctions::chopLastChar((string)buffer)+"'");
      if (strncmp(buffer,"close",5) == 0) {
	uLog::annotate(MONL_HOSTEVENT,"[mcs/sock-d]:: connection closed by client.");
	up=false;
      }
      else {
	q = sbc->process_socket_command(buffer,newsockfd);
      }
      if (q < 0) {
	char errbuff[512];
	string exstr = strerror_r(errno,errbuff,512);
#ifdef BREAK_ON_SOCKET_ERROR	
	_sdc_error("[mcs/sock-d]::"+exstr);
#else
	uLog::annotate(MONL_HOSTEVENT,"[mcs/sock-d]::"+exstr);
	up=false;
#endif
      }
    }
    close(newsockfd);
  }
  pthread_exit((void*)0);
}

int socket_dispatch_control::process_socket_command(char *command,
						    int newsockfd) {
  string acdv = ignore_msg();
  if (strncmp(command,"step",4) == 0) {
    if (block) {
      synchStepOK=true;
      acdv = success_msg();
    }
    else {
      acdv = fail_msg("can't step a non-blocking dispatcher.");
    }
  }
  else if (strncmp(command,"swb",3) == 0) {
    if (block) {
      synchStepOK=true;
      stepWhileBusy=true;
      acdv = success_msg();
    }
    else {
      acdv = fail_msg("can't step a non-blocking dispatcher.");
    }
  }
  else if (strncmp(command,"agents",6) == 0) {
    acdv = success_msg(show_agents());
  }
  else if (strncmp(command,"declarations",12) == 0) {
    acdv = success_msg(umbc::declarations::dec_cnts_as_str());
  }
  else if (strncmp(command,"grammar",7) == 0) {
    acdv = success_msg(generate_grammar());    
  }
  else if (strncmp(command,"send",4) == 0)
    acdv = send_agent_command(command);
  else 
    acdv = fail_msg("unknown directive.");
  if (settings::getSysPropertyString("moncon.reply","telnet") == "prolog")
    acdv = dispatch::prologify(acdv);
  acdv+="\n";
  int q = send(newsockfd,acdv.c_str(),acdv.length(),MSG_NOSIGNAL);
  return q;
}

bool socket_dispatch_control::publish_grammar(umbc::command_grammar& grammar) {
  grammar.add_production(grammar_commname(),"agents");
  grammar.add_production(grammar_commname(),"close");
  grammar.add_production(grammar_commname(),"declarations");
  grammar.add_production(grammar_commname(),"grammar"); 
  grammar.add_production(grammar_commname(),"send agent_directive");
  if (synchronous) grammar.add_production(grammar_commname(),"step");
  if (synchronous) grammar.add_production(grammar_commname(),"swb");

  // add agent/controller commands
  agent_list al = agent_of()->domain_of()->list_agents();
  for (agent_list::iterator ali = al.begin();
       ali != al.end();
       ali++) {
    if (((*ali)!= agent_of()) && (*ali)->publish_grammar(grammar))
      grammar.add_production("agent_directive",(*ali)->get_name()+" "+(*ali)->grammar_commname());
  }

  return true;
}
