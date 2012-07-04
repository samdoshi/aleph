/* ctl_op.c
 * aleph-avr32
 *
 * operator implmentation (base and derived classes)
 */

#include <stdio.h>

#include "../common/types.h"
#include "net.h"
//#include "ui.h"
#include "param.h"
#include "op.h"


// operator class registry
// must be laid out idenitcally to eOpId enum
const op_desc_t op_registry[numOpClasses] = {
{ "SWITCH", sizeof(op_sw_t) },
{ "ENCODER", sizeof(op_enc_t) },
{ "ADD", sizeof(op_add_t) },
{ "MULTIPLY", sizeof(op_mul_t) },
{ "GATE", sizeof(op_gate_t) },
{ "ACCUMULATE", sizeof(op_accum_t) },
{ "SELECT", sizeof(op_sel_t) },
{ "LINEAR MAP", sizeof(op_lin_t) },
//{ "PARAMETER", sizeof(op_param_t) },
{ "PRESET", sizeof(op_preset_t) }
};


// input and output strings are all the same length, concatenated
// this is lazy, but also efficient.
static const U8 inStringChars = 8;
static const U8 outStringChars = 8;

//===============================================
//===  base class definittions
const char* op_in_name(op_t* op, const U8 idx) {
  static char str[64];
  U8 i;
  // str = (op->inString + (inStringChars * idx));
  for(i=0; i<inStringChars; i++) {
    str[i] = *(op->inString + (inStringChars * idx) + i);
  }
  str[inStringChars] = '\0';
  return str;
}

const char* op_out_name(op_t* op, const U8 idx) {
  static char str[64];
  U8 i;
//  str = (op->outString + (outStringChars * idx));
  
  for(i=0; i<outStringChars; i++) {
    str[i] = *(op->outString + (outStringChars * idx) + i);
  }
  str[outStringChars] = '\0';
  return str;
}

// get input value
S32 op_get_in_val(op_t* op, s16 idx) {
  return op->in_val[idx];
}

// set input value
void op_set_in_val(op_t* op, s16 idx, s32 val) {
  S32 * const pIn = &(op->in_val[idx]);
  *pIn = val;
  (*(op->in_func[idx]))(op, pIn);
}

//============================================
//=== subclass definitions

//-------------------------------------------------
//----- switch
static const char* op_sw_instring = "STATE   TOGGLE  MULT    ";
static const char* op_sw_outstring = "STATE  ";
static const char* op_sw_opstring = "SWITCH";

static void op_sw_in_state(op_sw_t* sw, const S32* v) {
  if (sw->tog) {
    // toggle mode, sw state toggles on positive input
    if ( *v > 0) {
      if (sw-> val > 0) { 
	sw->val = 0; 
      } else {
	sw->val = sw->mul;
      }
      net_activate(sw->outs[0], sw->val);
    }
  } else {
    // momentary mode, sw value takes input
    sw->val = ((*v) > 0) * sw->mul;
    net_activate(sw->outs[0], sw->val);
  }
}

static void op_sw_in_tog(op_sw_t* sw, const S32* v) {
  sw->tog = (*v > 0);
  /*
  if (sw-> val > 0) { sw->val = 0; }
  else { sw->val = sw->mul; }
  net_activate(sw->outs[0], sw->val);
  */
 }

static void op_sw_in_mul(op_sw_t* sw, const S32* v) {
  sw->mul = *v;
  sw->val = (sw->val > 0) * sw->mul;
  net_activate(sw->outs[0], sw->val);
 }

static op_in_func_t op_sw_in_func[3] = {
  (op_in_func_t)&op_sw_in_state,
  (op_in_func_t)&op_sw_in_tog,
  (op_in_func_t)&op_sw_in_mul
};

static S32 op_sw_in_val[3] = { 0, 0, 1 };

void op_sw_init(op_sw_t* sw) {
  sw->super.numInputs = 3;
  sw->super.numOutputs = 1;
  sw->outs[0] = -1;
  sw->super.in_func = op_sw_in_func;
  sw->super.in_val = op_sw_in_val;
  sw->super.out = sw->outs;
  sw->super.opString = op_sw_opstring;
  sw->super.inString = op_sw_instring;
  sw->super.outString = op_sw_outstring;
  sw->super.type = eOpSwitch;
  sw->super.status = eSysCtlOp;
//  sw->super.size = sizeof(op_sw_t);
}

//-------------------------------------------------
//----- encoder

static const char* op_enc_instring = "MOVE    MIN     MAX     STEP    WRAP    ";
static const char* op_enc_outstring = "VALUE   WRAP    ";
static const char* op_enc_opstring = "ENCODER";
static void op_enc_perform(op_enc_t* enc);

// step
static void op_enc_in_step(op_enc_t* enc, const S32* v) {
  enc->step = *v;
  // op_enc_perform(enc);
}

// move
static void op_enc_in_move(op_enc_t* enc, const S32* v) {
  enc->val += enc->step * (*v); 
  op_enc_perform(enc);
}
// max
static void op_enc_in_min(op_enc_t* enc, const S32* v) {
  enc->min = *v;
  op_enc_perform(enc);
}

// max
static void op_enc_in_max(op_enc_t* enc, const S32* v) {
  enc->max = *v;
  op_enc_perform(enc);
}

// wrap behavior
static void op_enc_in_wrap(op_enc_t* enc, const S32* v) {
  enc->wrap = (*v > 0);
  //  op_enc_perform(enc);
}

// perform wrapping and output
static void op_enc_perform(op_enc_t* enc) {
  s32 wrap = 0;
  s32 dif = 0;
  if (wrap) { // wrapping...
    // if value needs wrapping, output the applied difference
    while (enc->val > enc->max) { 
      dif = enc->min - enc->max;
      wrap += dif;
      enc->val += dif;
    }
    while (enc->val < enc->min) { 
      dif = enc->max - enc->min;
      wrap += dif;
      enc->val += dif;
    }
  } else { // saturating...
    if (enc->val > enc->max) {
      enc->val = enc->max;
      dif = 1; // force wrap output with value 0
    }
    if (enc->val < enc->min) {
      enc->val = enc->min;
      dif = -1; // force wrap output with value 0
    }
  }
  // output the value
  net_activate(enc->outs[0], enc->val);

  // output the wrap amount
  if (dif != 0) {
    net_activate(enc->outs[1], wrap);  
  }
}

// input functions
static op_in_func_t op_enc_in_func[5] = {
  (op_in_func_t)&op_enc_in_move,
  (op_in_func_t)&op_enc_in_min,
  (op_in_func_t)&op_enc_in_max,
  (op_in_func_t)&op_enc_in_step,
  (op_in_func_t)&op_enc_in_wrap,
};

// default value
static S32 op_enc_in_val[5] = { 0, PARAM_MIN, PARAM_MAX, 1, 1 };

void op_enc_init(op_enc_t* enc) {
  enc->super.numInputs = 5;
  enc->super.numOutputs = 2;
  enc->outs[0] = -1;
  enc->outs[1] = -1;
  enc->super.in_func = op_enc_in_func;
  enc->super.in_val = op_enc_in_val;
  enc->super.out = enc->outs;
  enc->super.opString = op_enc_opstring;
  enc->super.inString = op_enc_instring;
  enc->super.outString = op_enc_outstring;
  enc->super.type = eOpEnc;
  enc->super.status = eSysCtlOp;
  enc->min = PARAM_MIN;
  enc->max = PARAM_MAX;
  enc->val = 0;
  enc->step = 1;
  enc->wrap = 1;
//  enc->super.size = sizeof(op_enc_t);
}

//-------------------------------------------------
//---- adder
static const char* op_add_instring = "A       B       B_TRIG  ";
static const char* op_add_outstring = "SUM     ";
static const char* op_add_opstring = "ADDER";

static void op_add_in_a(op_add_t* add, const S32* v) {
  printf("add at %d received A %d\n", (int)add, (int)*v);
  add->a = *v;
  add->val = add->a + add->b;
  net_activate(add->outs[0], add->val);
}

static void op_add_in_b(op_add_t* add, const S32* v) {
  printf("add at %d received B %d\n", (int)add, (int)*v);
  add->b = *v;
  add->val = add->a + add->b;
  if(add->btrig) {
    net_activate(add->outs[0], add->val);
  }
}

static void op_add_in_btrig(op_add_t* add, const S32* v) {
  printf("add at %d received BTRIG %d\n", (int)add, (int)*v);
  add->btrig = (v != 0);
}


static op_in_func_t op_add_in_func[3] = {
  (op_in_func_t)&op_add_in_a,
  (op_in_func_t)&op_add_in_b, 
  (op_in_func_t)&op_add_in_btrig
};

static S32 op_add_in_val[3] = { 0, 0, 0 };

void op_add_init(op_add_t* add) {
  add->super.numInputs = 3;
  add->super.numOutputs = 1;
  add->outs[0] = -1;
  add->super.in_func = op_add_in_func;
  add->super.in_val = op_add_in_val;
  add->super.out = add->outs;
  add->super.opString = op_add_opstring;
  add->super.inString = op_add_instring;
  add->super.outString = op_add_outstring;
  add->super.type = eOpAdd;  
  add->super.status = eUserOp;  
//  add->super.size = sizeof(op_add_t);
}

//-------------------------------------------------
//----- multiplier
static const char* op_mul_instring = "A       B       B_TRIG  ";
static const char* op_mul_outstring = "PRODUCT ";
static const char* op_mul_opstring = "MULTIPLIER";

static void op_mul_in_a(op_mul_t* mul, const S32* v) {
  printf("mul at %d received A %d\n", (int)mul, (int)*v);
  mul->a = *v;
  mul->val = mul->a * mul->b;
  net_activate(mul->outs[0], mul->val);
}

static void op_mul_in_b(op_mul_t* mul, const S32* v) {
  printf("mul at %d received B %d\n", (int)mul, (int)*v);
  mul->b = *v;
  mul->val = mul->a * mul->b;
  if(mul->btrig) {
    net_activate(mul->outs[0], mul->val);
  }
}

static void op_mul_in_btrig(op_mul_t* mul, const S32* v) {
  printf("mul at %d received BTRIG %d\n", (int)mul, (int)*v);
  mul->btrig = (v != 0);
}

static op_in_func_t op_mul_in_func[3] = {
  (op_in_func_t)&op_mul_in_a,
  (op_in_func_t)&op_mul_in_b, 
  (op_in_func_t)&op_mul_in_btrig
};

static S32 op_mul_in_val[3] = { 1, 1, 0 };

void op_mul_init(op_mul_t* mul) {
  mul->super.numInputs = 3;
  mul->super.numOutputs = 1;
  mul->outs[0] = -1;
  mul->super.in_func = op_mul_in_func;
  mul->super.in_val = op_mul_in_val;
  mul->super.out = mul->outs;
  mul->super.opString = op_mul_opstring;
  mul->super.inString = op_mul_instring;
  mul->super.outString = op_mul_outstring;
  mul->super.type = eOpMul;  
  mul->super.status = eUserOp;  
//  mul->super.size = sizeof(op_mul_t);
}

//-------------------------------------------------
//----- gate
static const char* op_gate_instring = "VALUE   GATE    STORE   ";
static const char* op_gate_outstring = "GATED   ";
static const char* op_gate_opstring = "GATE";

static void op_gate_in_value(op_gate_t* gate, const S32* v) {
  printf("gate at %d received VALUE %d\n", (int)gate, (int)*v);
  gate->val = *v;
  if(gate->gate) {
    net_activate(gate->outs[0], gate->val);
  }
}

static void op_gate_in_gate(op_gate_t* gate, const S32* v) {
  printf("gate at %d received GATE %d\n", (int)gate, (int)*v);
  gate->val = (*v != 0);
  if (gate->val) {
    if (gate->store) {
      net_activate(gate->outs[0], gate->val);
    }
  }
}

static void op_gate_in_store(op_gate_t* gate, const S32* v) {
  printf("gate at %d received STORE %d\n", (int)gate, (int)*v);
  gate->store = (*v != 0);
}

static op_in_func_t op_gate_in_func[3] = {
  (op_in_func_t)&op_gate_in_value,
  (op_in_func_t)&op_gate_in_gate, 
  (op_in_func_t)&op_gate_in_store
};

static S32 op_gate_in_val[3] = { 0, 1, 0 };

void op_gate_init(op_gate_t* gate) {
  gate->super.numInputs = 3;
  gate->super.numOutputs = 1;
  gate->outs[0] = -1;
  gate->super.in_func = op_gate_in_func;
  gate->super.in_val = op_gate_in_val;
  gate->super.out = gate->outs;
  gate->super.opString = op_gate_opstring;
  gate->super.inString = op_gate_instring;
  gate->super.outString = op_gate_outstring;
  gate->super.type = eOpGate;  
  gate->super.status = eUserOp;  
//  gate->super.size = sizeof(op_gate_t);
}

//-------------------------------------------------
//------ accumulator
static const char* op_accum_instring = "VALUE   COUNT   MIN     MAX     CARRY   ";
static const char* op_accum_outstring = "VALUE   CARRY";
static const char* op_accum_opstring = "ACCUMULATOR";

static void op_accum_boundscheck(op_accum_t* accum) {
  if(accum->val > accum->max) {
    if(accum->carry) {
      while(accum->val > accum->max) {
        accum->val -= (accum->max > 0 ? accum->max : accum->max * -1);
      }
      net_activate(accum->outs[1], accum->val); // carry output with wrapped value
    } else {
      accum->val = accum->max;
    }
  }
  if(accum->val < accum->min) {
    if(accum->carry) {
      while(accum->val < accum->min) {
        accum->val += (accum->min > 0 ? accum->min : accum->min * -1);
      }
      net_activate(accum->outs[1], accum->val); // carry output with wrapped value
    } else {
      accum->val = accum->min;
    }
  }  
}

static void op_accum_in_value(op_accum_t* accum, const S32* v) {
  printf("accum at %d received VALUE %d\n", (int)accum, (int)*v);
  accum->val = *v;
  op_accum_boundscheck(accum);
  net_activate(accum->outs[0], accum->val);
}

static void op_accum_in_count(op_accum_t* accum, const S32* v) {
  printf("accum at %d received COUNT %d\n", (int)accum, (int)*v);
  accum->val += *v;
  op_accum_boundscheck(accum);
  net_activate(accum->outs[0], accum->val);
}

static void op_accum_in_min(op_accum_t* accum, const S32* v) {
  printf("accum at %d received MIN %d\n", (int)accum, (int)*v);
  accum->min = *v;
}

static void op_accum_in_max(op_accum_t* accum, const S32* v) {
  printf("accum at %d received MAX %d\n", (int)accum, (int)*v);
  accum->max = *v;
}

static void op_accum_in_carry(op_accum_t* accum, const S32* v) {
  printf("accum at %d received CARRY %d\n", (int)accum, (int)*v);
  accum->carry = (v != 0);
}

static op_in_func_t op_accum_inputs[5] = {
(op_in_func_t)&op_accum_in_value,
(op_in_func_t)&op_accum_in_count,
(op_in_func_t)&op_accum_in_min, 
(op_in_func_t)&op_accum_in_max,
(op_in_func_t)&op_accum_in_carry
};

void op_accum_init(op_accum_t* accum) {
  accum->super.numInputs = 5;
  accum->super.numOutputs = 2;
  accum->outs[0] = -1;
  accum->outs[1] = -1;
  accum->super.in_func = op_accum_inputs;
  accum->super.out = accum->outs;
  accum->super.opString = op_accum_opstring;
  accum->super.inString = op_accum_instring;
  accum->super.outString = op_accum_outstring;
  accum->super.type = eOpAccum; 
  accum->super.status = eUserOp;  
//  accum->super.size = sizeof(op_accum_t);
}

//-------------------------------------------------
//------ range selector
void op_sel_initg(op_sel_t* sel);

//-------------------------------------------------
//----- linear map
void op_lin_init(op_lin_t* lin);

//-------------------------------------------------
//---- param value receiver
/*
static const char* op_param_instring = "INDEX   VALUE   ";
static const char* op_param_opxstring = "PARAM";

static void op_param_in_idx(op_param_t* param, const S32* v) {
  // FIXME: limit by DSP param count
  param->idx = (U32)(*v);
}

static void op_param_in_val(op_param_t* param, const S32* v) {
  // FIXME: set DSP param
 
}

static op_in_func_t op_param_inputs[2] = {
  (op_in_func_t)&op_param_in_idx,
  (op_in_func_t)&op_param_in_val
};

void op_param_init(op_param_t* op) {
  op->super.numInputs = 2;
  op->super.numOutputs = 0;
  op->super.in_func = op_param_inputs;
  op->super.opString = op_param_opstring;
  op->super.inString = op_param_instring;
  op->super.type = eOpParam;  
  op->super.status = eSysRecOp;  
}

*/

//-------------------------------------------------
//----- preset manipulator
void op_preset_init(op_preset_t* preset);
