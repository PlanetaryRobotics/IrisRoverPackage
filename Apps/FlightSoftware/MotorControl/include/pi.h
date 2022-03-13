// ======================================================================
// \title  pi.h
// \author cedric
// \edited by Jonathan
// \brief  defines PI controller object that is used to control speed
//         and current
//
// ======================================================================
#ifndef __PI_H__
#define __PI_H__

#include "IQmathLib.h"

// control related constants (gains, thresholds, setpoints, default values)
#define KP_SPD                  1.5
#define KI_SPD                  0.0009
#define KP_CUR                  0.95
#define KI_CUR                  0.002
#define PI_OUTPUT_BOUNDS        1.0 // will be converted to _IQ

typedef struct PI_CONTROLLER{  _iq  Ref;   			// Input: reference set-point
				  _iq  Fbk;   			// Input: feedback
				  _iq  Out;   			// Output: controller output
				  _iq  Kp;				// Parameter: proportional loop gain
				  _iq  Ki;			    // Parameter: integral gain
				  _iq  Umax;			// Parameter: upper saturation limit
				  _iq  Umin;			// Parameter: lower saturation limit
				  _iq  up;				// Data: proportional term
				  _iq  ui;				// Data: integral term
				  _iq  v1;				// Data: pre-saturated controller output
				  _iq  i1;				// Data: integrator storage: ui(k-1)
				  uint8_t  w1;		    // Data: saturation record: [u(k-1) - v(k-1)]
				} PI_CONTROLLER;

void pi_iteration();

void pi_iteration(volatile PI_CONTROLLER* v);

void resetPiController(volatile PI_CONTROLLER *pi);

#endif // __PI_H__

