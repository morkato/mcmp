#include "monarch/statos.h"

#ifdef MCMCHST_THREAD
  MCMCHST_THREAD mcmchst_soexc_frame_t mchst_so_frame_v;
  MCMCHST_THREAD mcmchst               mchst_last_ret;
#endif