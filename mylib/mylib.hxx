#pragma once

#include <iosfwd>
#include <string>

#include <mylib/export.hxx>

namespace mylib
{
  // Print a greeting for the specified name into the specified
  // stream. Throw std::invalid_argument if the name is empty.
  //
  MYLIB_SYMEXPORT void
  say_hello (std::ostream&, const std::string& name);
}
