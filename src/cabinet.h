#ifndef CABINET_H
#define CABINET_H

typedef enum { ST_WAITING, ST_ACTIVE, ST_END } CabState;

extern volatile CabState cab_state;

#endif
