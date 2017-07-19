#ifndef CESCAPE_TD_H
#define CESCAPE_TD_H

#include <string>

#include "util/util.h"
#include "re2/stringpiece.h"

namespace re2 
{
  string CEscape_renamed(const StringPiece& src);
}

#endif 
