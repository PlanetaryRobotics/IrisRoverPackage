// ======================================================================
// \title  pi.c
// \author Jonathan
// \brief  moved Cedric's weird macro thing to a function, just performs
//         one step of pi controller
//
// ======================================================================
#include "include/pi.h"

void pi_iteration(volatile PI_CONTROLLER* v){
    /* proportional term */
   v->up = v->Ref - v->Fbk;

   /* integral term */
   if (v->Out == v->v1){
       v->ui = (_IQ15mpy(v->Ki, v->up)+ v->i1);
   } else {
       v->ui = v->i1;
   }
   v->i1 = v->ui;

   /* control output */
   v->v1 = _IQ15mpy(v->Kp, v->up) + v->ui;
   v->Out = _IQsat(v->v1, v->Umax, v->Umin);
   v->w1 = (v->v1 != v->Out) ? 1 : 0;
}

/**
 * @brief      Reset the PI controller
 *
 * @param      pi    { controller for either speed or current }
 */
void resetPiController(volatile PI_CONTROLLER *pi){
  pi->i1 = _IQ(0.0);    // reset integrator storage (stores pi->ui from last time step)
  pi->ui = _IQ(0.0);    // reset integral term (sums error over time steps)
  pi->v1 = _IQ(0.0);
  pi->up = _IQ(0.0);
  pi->Umax = _IQ(PI_OUTPUT_BOUNDS);
  pi->Umin = _IQ(-PI_OUTPUT_BOUNDS);
}

