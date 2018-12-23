#include "substrate/simEntity.h"
#include <stdio.h>

simEntity::simEntity() : status("ok") {
  char nb[1024];
  sprintf(nb,"entity-0x%08lx",(unsigned long)this);
  name=nb;
}
