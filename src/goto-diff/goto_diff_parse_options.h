/*******************************************************************\

Module: GOTO-DIFF Command Line Option Processing

Author: Peter Schrammel

\*******************************************************************/

#ifndef CPROVER_GOTO_DIFF_PARSEOPTIONS_H
#define CPROVER_GOTO_DIFF_PARSEOPTIONS_H

#include <util/ui_message.h>
#include <util/parse_options.h>

#include <langapi/language_ui.h>

#include <goto-programs/goto_model.h>

class goto_modelt;
class optionst;

#define GOTO_DIFF_OPTIONS \
  "(program-only)(function):(preprocess)(slice-by-trace):" \
  "(no-simplify)(unwind):(unwindset):(slice-formula)(full-slice)" \
  "(debug-level):(no-propagation)(no-simplify-if)" \
  "(document-subgoals)(outfile):(test-preprocessor)" \
  "D:I:(std89)(std99)(std11)" \
  "(classpath):" \
  "(depth):(partial-loops)(no-unwinding-assertions)(unwinding-assertions)" \
  "(bounds-check)(pointer-check)(div-by-zero-check)(memory-leak-check)" \
  "(signed-overflow-check)(unsigned-overflow-check)(float-overflow-check)(nan-check)" \
  "(no-assertions)(no-assumptions)" \
  "(xml-ui)(xml-interface)(vcd):" \
  "(smt1)(smt2)(fpa)(cvc3)(cvc4)(boolector)(yices)(z3)(opensmt)(mathsat)" \
  "(no-sat-preprocessor)" \
  "(no-pretty-names)(beautify)" \
  "(floatbv)(fixedbv)" \
  "(dimacs)(refine)(max-node-refinement):(refine-arrays)(refine-arithmetic)(aig)" \
  "(16)(32)(64)(LP64)(ILP64)(LLP64)(ILP32)(LP32)" \
  "(little-endian)(big-endian)" \
  "(show-goto-functions)(show-loops)" \
  "(show-symbol-table)(show-parse-tree)(show-vcc)" \
  "(show-claims)(claim):(show-properties)(show-reachable-properties)(property):" \
  "(all-claims)(all-properties)" \
  "(error-label):(verbosity):(no-library)" \
  "(version)" \
  "(cover):" \
  "(mm):" \
  "(i386-linux)(i386-macos)(i386-win32)(win32)(winx64)(gcc)" \
  "(ppc-macos)(unsigned-char)" \
  "(arrays-uf-always)(arrays-uf-never)" \
  "(string-abstraction)(no-arch)(arch):" \
  "(round-to-nearest)(round-to-plus-inf)(round-to-minus-inf)(round-to-zero)" \
  "(graphml-cex):(json-cex):" \
  "(decide)" // legacy, and will eventually disappear

class goto_diff_parse_optionst:
  public parse_options_baset,
  public language_uit
{
public:
  virtual int doit();
  virtual void help();

  goto_diff_parse_optionst(int argc, const char **argv);
  goto_diff_parse_optionst(
    int argc,
    const char **argv,
    const std::string &extra_options);

protected:
  virtual void register_languages();

  virtual void get_command_line_options(optionst &options);

  virtual int get_goto_programs(
    const optionst &options,
    goto_modelt &goto_model1,
    goto_modelt &goto_model2);

  virtual bool process_goto_program(
    const optionst &options,
    goto_modelt &goto_model);
    
  void eval_verbosity();
  
  void preprocessing();
};

#endif
