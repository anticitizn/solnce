
#pragma once

#include <src/utils/toString.hpp>
#include <external/pugixml/pugixml.hpp>

// Gives the name of the variable as a string
#define VAR_STR(Variable) (void(Variable),#Variable)

// Macro to append a variable's name and value to an XML node
// It has to be a macro and not a function because otherwise the
// variable name is lost
// Used for serialization purposes
#define ARCHIVE_VAR(root, var) {                                                 \
    pugi::xml_node nodeChild = root.append_child(VAR_STR(var));                  \
    nodeChild.append_child(pugi::node_pcdata).set_value(toString(var).c_str());  \
}
