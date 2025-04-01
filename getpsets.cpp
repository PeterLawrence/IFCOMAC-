
#include <string>
#include <vector>
#include <iomanip>

#pragma warning(disable : 4018 4267 4250 4984 4985 4307)
#include "ifcparse/IfcFile.h"
#include "ifcparse/Ifc4x3.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ExodusIFCClasses.h"
#include "extractgeom.h"
#include "exodusifcutils.h"
#include "getpset.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::string format_string(const AttributeValue& argument) {
    // Argument is a runtime tagged variant for the various data types in a IFC model,
    // in this particular case we only care about flattening it to a string.
    // @todo mostly duplicated from XmlSerializer.cpp
    if (argument.isNull()) {
        return "-";
    }
    auto argument_type = argument.type();
    switch (argument_type) {
    case IfcUtil::Argument_BOOL: {
        const bool b = argument;
        return b ? "true" : "false";
    }
    case IfcUtil::Argument_DOUBLE: {
        const double d = argument;
        std::stringstream stream;
        stream << std::setprecision(std::numeric_limits< double >::max_digits10) << d;
        return stream.str();
        break; }
    case IfcUtil::Argument_STRING:
    case IfcUtil::Argument_ENUMERATION: {
        return static_cast<std::string>(argument);
        break; }
    case IfcUtil::Argument_INT: {
        const int v = argument;
        std::stringstream stream;
        stream << v;
        return stream.str();
        break; }
    }
    return "?";
}

