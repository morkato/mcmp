#include "mcmp/statos.h"

#ifdef MCMPST_THREAD
  MCMPST_THREAD mcmpst_soexc_frame_t mcmpst_so_frame_v;
  MCMPST_THREAD mcmpst               mcmpst_last_ret;
#endif