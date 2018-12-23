#include "controller.h"
#include "agent_player.h"

using namespace raccoon;

controllers::controller::controller(string nm,agents::agent* a) : 
  has_name(nm),my_agent(a),publishes_grammar() {};


controllers::player_controller::player_controller(string nm, 
						  agents::agent_player* ape) :
  controller(nm,ape),run(false),self_deac(false) {
  my_client = ape->get_client();
  cout << " creating pc controller named '" << get_name() << "'" << endl;
}
