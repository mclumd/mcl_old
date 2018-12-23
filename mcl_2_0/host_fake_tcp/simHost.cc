#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <math.h>

#include "umbc/logger.h"

using namespace std;
using namespace umbc;

#define BASE_EXP_GRP 0x11110000

#define FH_NUM_SENSORS 5
#define FH_MCL_KEY     "fakeHost"
#define FH_MCL_CONFIG  "fakeHost"

int mclFD;
const char* nv[FH_NUM_SENSORS] = 
  { "noisyCos","noisySin","noisyConst","randomWalk","sin+rw" };
float  sv[FH_NUM_SENSORS];
string mcl_update_string = "";

string tell_mcl(string cmd) {
  int n=0;
  cout << " >> sending '" << cmd << "'" << endl;
  n = write(mclFD,cmd.c_str(),cmd.size());
  if (n < 0) {
    return "ERROR writing to socket";
  }
  char buffer[1024];
  bzero(buffer,1024);
  n = read(mclFD,buffer,1023);
  if (n < 0) 
    return "ERROR reading from socket";
  else {
    buffer[n-1]='\0';
    if (strncmp(buffer,"fail",4) == 0)
      uLog::annotate(UMBCLOG_ERROR," returned '" + (string)buffer + "'");
    else uLog::annotate(UMBCLOG_MSG," returned '" + (string)buffer + "'");
    return buffer;
  }
}

void initMCL() {
  char commbuff[512];
  sprintf(commbuff,"initialize(%s,8)",FH_MCL_KEY);
  tell_mcl(commbuff);
  sprintf(commbuff,"configure(%s,%s)",FH_MCL_KEY,FH_MCL_CONFIG);
  tell_mcl(commbuff);
}

void updateSensors() {
  static int c=0;
  // noisy cos, breakout high
  sv[0]=cosf((float)c*3.14/(float)100) + (0.1 * (((float)rand()/(float)RAND_MAX)-0.5));
  // noisy sin, breakout low
  sv[1]=sinf((float)c*3.14/(float)100) + (0.1 * (((float)rand()/(float)RAND_MAX)-0.5));
  // noisy const, maintain val
  sv[2]=(((double)rand()/(double)RAND_MAX)*0.26-0.13);
  // random walk
  sv[3]+=((float)rand()/(float)RAND_MAX)-0.5;
  sv[4]=cosf((float)c*3.14/float(200) + sv[3]);
  c++;

  mcl_update_string="";
  for ( int i=0 ; i<FH_NUM_SENSORS ; i++ ) {
    char buff[256];
    bzero(buff,255);
    sprintf(buff,"%s=%.2f",nv[i],sv[i]);
    mcl_update_string+=buff;
    if (i < FH_NUM_SENSORS-1)
      mcl_update_string+=",";
  }
}

string monitor(string key) {
  string sv="monitor("+key+","+mcl_update_string+")";
  return tell_mcl(sv);
}

void declare_self(string key,string name) {
  char commbuff[1024];
  bzero(commbuff,1024);
  sprintf(commbuff,"declareObservableSelf(%s,%s)",key.c_str(),name.c_str());
  tell_mcl(commbuff);
}

void set_prop_self(string key,string name,string prop,string value) {
  char commbuff[1024];
  bzero(commbuff,1024);
  sprintf(commbuff,"setObsPropSelf(%s,%s,%s,%s)",
	  key.c_str(),name.c_str(),prop.c_str(),value.c_str());
  tell_mcl(commbuff);
}

void set_np_self(string key,string name,string profile,double param) {
  char commbuff[1024];
  bzero(commbuff,1024);
  sprintf(commbuff,"setObsNoisePropSelf(%s,%s,%s,%.4lf)",
	  key.c_str(),name.c_str(),profile.c_str(),param);
  tell_mcl(commbuff);
}

void declare_eg(string key,unsigned int group) {
  char commbuff[1024];
  bzero(commbuff,1024);
  sprintf(commbuff,"declareEG(%s,0x%8x)",
	  key.c_str(),group);
  tell_mcl(commbuff);
}

void declare_exp(string key,unsigned int grp,
		 const char* ectype,double arg) {
  char commbuff[1024];
  bzero(commbuff,1024);
  sprintf(commbuff,"declareExp(%s,0x%8x,%s,%.3lf)",
	  key.c_str(),grp,ectype,arg);
  tell_mcl(commbuff);
}

void declare_sexp(string key,unsigned int grp,const char* sname,
		 const char* ectype,double arg) {
  char commbuff[1024];
  bzero(commbuff,1024);
  sprintf(commbuff,"declareSelfExp(%s,0x%8x,%s,%s,%.3lf)",
	  key.c_str(),grp,sname,ectype,arg);
  tell_mcl(commbuff);
}

void registerHIA(string key,string HIA,string activate) {
  char commbuff[1024];
  bzero(commbuff,1024);
  sprintf(commbuff,"registerHIA(%s,%s,%s)",
	  key.c_str(),HIA.c_str(),activate.c_str());
  tell_mcl(commbuff);

}

void set_prop(string key,string prop,string value) {
  char commbuff[1024];
  bzero(commbuff,1024);
  sprintf(commbuff,"setPropertyDefault(%s,%s,%s)",
	  key.c_str(),prop.c_str(),value.c_str());
  tell_mcl(commbuff);

}

void registerSensors() {

  // noisy cos
  declare_self(FH_MCL_KEY,nv[0]);
  set_prop_self(FH_MCL_KEY,nv[0],"prop_sclass","sc_resource");
  set_prop_self(FH_MCL_KEY,nv[0],"prop_dt","dt_rational");
  set_np_self(FH_MCL_KEY,nv[0],"mcl_np_uniform",0.1);

  // noisy sin
  declare_self(FH_MCL_KEY,nv[1]);
  set_prop_self(FH_MCL_KEY,nv[1],"prop_sclass","sc_state");
  set_prop_self(FH_MCL_KEY,nv[1],"prop_dt","dt_rational");
  set_np_self(FH_MCL_KEY,nv[1],"mcl_np_uniform",0.1);

  // noisy const
  declare_self(FH_MCL_KEY,nv[2]);
  set_prop_self(FH_MCL_KEY,nv[2],"prop_sclass","sc_ambient");
  set_prop_self(FH_MCL_KEY,nv[2],"prop_dt","dt_rational");
  set_np_self(FH_MCL_KEY,nv[2],"mcl_np_uniform",0.1);
  
  // random walk
  declare_self(FH_MCL_KEY,nv[3]);
  set_prop_self(FH_MCL_KEY,nv[3],"prop_sclass","sc_spatial");
  set_prop_self(FH_MCL_KEY,nv[3],"prop_dt","dt_rational");
  // set_np_self(FH_MCL_KEY,nv[3],"mcl_np_uniform",0.1);
  
  // sin+rw
  declare_self(FH_MCL_KEY,nv[4]);
  set_prop_self(FH_MCL_KEY,nv[4],"prop_sclass","sc_unspec");
  set_prop_self(FH_MCL_KEY,nv[4],"prop_dt","dt_rational");
  // set_np_self(FH_MCL_KEY,nv[3],"mcl_np_uniform",0.1);

//   for (int i=0; i<FH_NUM_SENSORS; i++) 
//     mclMA::observables::dump_obs_self(FH_MCL_KEY,nv[i]);

//   mclMA::dumpNPT();

}

void declareExps() {

  declare_eg(FH_MCL_KEY,BASE_EXP_GRP);
  declare_sexp(FH_MCL_KEY,BASE_EXP_GRP,nv[0],"ec_stayunder",1.0);
  declare_sexp(FH_MCL_KEY,BASE_EXP_GRP,nv[1],"ec_stayover",-1.0);
  declare_sexp(FH_MCL_KEY,BASE_EXP_GRP,nv[2],"ec_maintainvalue",0);

}

void registerHIAs() {
  registerHIA(FH_MCL_KEY,"inconvenientTruthException","effectorError");
}

void setPV() {
  
  set_prop(FH_MCL_KEY,"pci_intentional","pc_yes");
  set_prop(FH_MCL_KEY,"pci_parameterized","pc_yes");
  set_prop(FH_MCL_KEY,"pci_sensors_can_fail","pc_yes");	
  set_prop(FH_MCL_KEY,"pci_effectors_can_fail","pc_yes");

}

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       return -1;
    }
    portno = atoi(argv[2]);
    mclFD = socket(AF_INET, SOCK_STREAM, 0);
    if (mclFD < 0) {
      cerr << "ERROR opening socket" << endl;
      return -1;
    }
    server = gethostbyname(argv[1]);
    if (server == NULL) {
      cerr << "ERROR, no such host" << endl;
      return -1;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serv_addr.sin_addr.s_addr,
	  server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(mclFD,(const struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
      cerr << "ERROR connecting" << endl;
      return -1;
    }

    initMCL();
    registerSensors();
    setPV();
    declareExps();
    registerHIAs();

    char b[32];
    cout << "press return to start.";
    cin.getline(b,31);

    while (true) {
      usleep(250000);
      updateSensors();
      monitor(FH_MCL_KEY);
    }
    
    return 0;

}
