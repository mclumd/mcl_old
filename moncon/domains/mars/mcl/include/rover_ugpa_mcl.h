#ifndef MARS_ROVER_UGPA_MCL_H
#define MARS_ROVER_UGPA_MCL_H

/*

  // direct_mcl means that the try again was suggested by MCL and as such
  // feedback on failure/abort/ignore should go to MCL
  virtual void handleTryAgain(mclMonitorResponse *mmr,bool direct_mcl);
  virtual void handleSensorDiag(mclMonitorResponse *mmr);
  virtual void handleSensorReset(mclMonitorResponse *mmr);
  virtual void handleEffectorDiag(mclMonitorResponse *mmr);
  virtual void handleRebuild(mclMonitorResponse *mmr);
  virtual void handleAmend(mclMonitorResponse *mmr);

 */

/* code

okay, the wrapper around the controller will need to set these properties...

declare_mcl() {
  mclMA::setPropertyDefault(mcl_key,PCI_SENSORS_CAN_FAIL,PC_NO);
  mclMA::setPropertyDefault(mcl_key,PCI_EFFECTORS_CAN_FAIL,PC_NO);

    mclMA::setPropertyDefault(mcl_agentspec(),CRC_SENSOR_RESET,PC_YES);
    mclMA::setPropertyDefault(mcl_agentspec(),CRC_SENSOR_DIAG,PC_YES);

    mclMA::setPropertyDefault(mcl_agentspec(),PCI_RETRAINABLE,PC_YES);

}

this might be unnecessary if we set up an oracle and use the MCL wrapper to
grab the models...

model_grabbing() {
  // okay, so now we are going to push through the action models
  for (action_list::iterator ali = rover->actions()->begin();
       ali != rover->actions()->end();
       ali++) {
    // uh oh, have to assume it's a mars domain action
    action* true_action = action_wrappers::get_core_action(*ali);
    mars_domain_action* mda = dynamic_cast<mars_domain_action*>(true_action);
    if (mda != NULL)
      set_model(mda,mda->cheat_get_model());
    else 
      perror("rover_ugpa cannot work without mars_domain_actions.");
  }
}

 */

#endif
