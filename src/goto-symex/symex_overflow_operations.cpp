/*******************************************************************\

Module: Symex Operations-With-Overflow Instrumentation

           Author: Giulio Garbi

\*******************************************************************/

/// \file
/// Symex Operations-With-Overflow Instrumentation

#include "arith_tools.h"
#include "bitvector_expr.h"
#include "bitvector_types.h"
#include "byte_operators.h"
#include "c_types.h"
#include "expr_util.h"
#include "fresh_symbol.h"
#include "goto_symex.h"
#include "symex_shadow_memory_util.h"

void goto_symext::symex_add_bits(
  goto_symex_statet &state,
  const exprt::operandst &arguments)
{
  // parse set_field call
  INVARIANT(
    arguments.size() == 5, CPROVER_PREFIX "add_bits requires 5 arguments");

  const exprt& a = arguments[0];
  const exprt& b = arguments[1];
  exprt c = arguments[2];
  typet c_type = c.type();
  DATA_INVARIANT_WITH_DIAGNOSTICS(
    c_type.id() == ID_pointer,
    "destination requires a pointer expression",
    irep_pretty_diagnosticst{c_type});

  exprt o = arguments[3];
  typet o_type = o.type();
  DATA_INVARIANT_WITH_DIAGNOSTICS(
    o_type.id() == ID_pointer,
    "overflow requires a pointer expression",
    irep_pretty_diagnosticst{c_type});
  exprt w = arguments[4];

  DATA_INVARIANT_WITH_DIAGNOSTICS(
    w.id() == ID_constant,
    "bitwidth must be an integer constant",
    irep_pretty_diagnosticst{w});

  // remove useless (harmful) casts
  const auto a_clean = remove_casts(a);
  const auto b_clean = remove_casts(b);

  // get the symbol to write on
  const auto c_deref = deref_expr(c);
  DATA_INVARIANT_WITH_DIAGNOSTICS(
    is_assignable(c_deref),
    "destination requires a pointer to an assignable",
    irep_pretty_diagnosticst{c_deref});

  // get the overflow bit
  const auto o_deref = deref_expr(o);
  DATA_INVARIANT_WITH_DIAGNOSTICS(
    is_assignable(o_deref),
    "overflow requires a pointer to an assignable",
    irep_pretty_diagnosticst{o_deref});

  const auto a_type = to_integer_bitvector_type(a_clean.type());
  const auto b_type = to_integer_bitvector_type(b_clean.type());

  const auto a_sign = a_type.smallest().is_negative();
  const auto b_sign = b_type.smallest().is_negative();

  bool c_sign;
  if(a_sign == b_sign) {
    c_sign = a_sign;
  } else if(!a_sign && a_type.get_width() >= b_type.get_width()){
    c_sign = false;
  } else if(!b_sign && b_type.get_width() >= a_type.get_width()){
    c_sign = false;
  } else if(!a_sign && b_type.get_width() >= a_type.get_width()+1){
    c_sign = true;
  } else if(!b_sign && a_type.get_width() >= b_type.get_width()+1){
    c_sign = true;
  } else {
    c_sign = false;
  }

  mp_integer w_mpint;
  to_integer(to_constant_expr(w), w_mpint);
  std::size_t w_ = w_mpint.to_long();
  if(c_sign){
    const signedbv_typet bvtype = signedbv_typet(w_);
    const signedbv_typet bvtype1 = signedbv_typet(w_+1);
    const auto a_bits = extractbits_exprt{a_clean, w_-1, 0, bvtype};
    const auto b_bits = extractbits_exprt{b_clean, w_-1, 0, bvtype};
    const auto rhs = plus_with_overflow_exprt{a_bits, b_bits, bvtype1};
    const auto aux = get_fresh_aux_symbol(bvtype1, "aux_add_bits","aux_add_bits",state.source.pc->source_location(),ID_C,state.symbol_table).symbol_expr();
    symex_assign(state, aux, rhs, false);
    symex_assign(state, c_deref, make_byte_update(c_deref, from_integer(0, c_index_type()), extractbits_exprt{aux, w_-1, 0, bvtype}), false);
    symex_assign(state, o_deref, make_byte_update(o_deref, from_integer(0, c_index_type()), extractbit_exprt{aux, w_}), false);
  } else {
    const unsignedbv_typet bvtype = unsignedbv_typet(w_);
    const unsignedbv_typet bvtype1 = unsignedbv_typet(w_+1);
    const auto a_bits = extractbits_exprt{a_clean, w_-1, 0, bvtype};
    const auto b_bits = extractbits_exprt{b_clean, w_-1, 0, bvtype};
    const auto rhs = plus_with_overflow_exprt{a_bits, b_bits, bvtype1};
    const auto aux = get_fresh_aux_symbol(bvtype1, "aux_add_bits","aux_add_bits",state.source.pc->source_location(),ID_C,state.symbol_table).symbol_expr();
    symex_assign(state, aux, rhs, false);
    symex_assign(state, c_deref, make_byte_update(c_deref, from_integer(0, c_index_type()), extractbits_exprt{aux, w_-1, 0, bvtype}), false);
    symex_assign(state, o_deref, make_byte_update(o_deref, from_integer(0, c_index_type()), extractbit_exprt{aux, w_}), false);
  }
}