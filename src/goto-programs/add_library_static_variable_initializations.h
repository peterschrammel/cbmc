/// \file
/// \author Peter Schrammel
/// Adds initializations for static variables from the library.

// NOLINT(whitespace/line_length)
#ifndef CPROVER_GOTO_PROGRAMS_ADD_LIBBRARY_STATIC_VARIABLE_INITIALIZATIONS_H
// NOLINT(whitespace/line_length)
#define CPROVER_GOTO_PROGRAMS_ADD_LIBBRARY_STATIC_VARIABLE_INITIALIZATIONS_H

class goto_modelt;
class message_handlert;

/// Static variables added by the library are never initialized because
/// static lifetime initialization happens before adding the library.
/// Thus we are going to initialize them in a separate pass.
void add_library_static_variable_initializations(
    goto_modelt &goto_model, message_handlert &message_handler);

// NOLINT(whitespace/line_length)
#endif // CPROVER_GOTO_PROGRAMS_ADD_LIBBRARY_STATIC_VARIABLE_INITIALIZATIONS_H
