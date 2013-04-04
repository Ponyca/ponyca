#!/usr/bin/env python3

import os
import sys
import yaml

HEADER_H = """
#ifndef PONYCA_PROTOCOL_H
#define PONYCA_PROTOCOL_H

#include <cstdint>
#include <string>
#include <vector>
#include "common/net/network.h"

namespace Ponyca {
namespace Net {
"""

FOOTER_H = """
}
}

#endif // PONYCA_PROTOCOL_H
"""

HEADER_CPP = """
#include "build/opcodes.h"

"""

FOOTER_CPP = """
"""

class InvalidSyntax(Exception):
    pass

class InvalidType(Exception):
    pass

def upfirst(name):
    return name[0].upper() + name[1:]

def parse_template_field(types, type_, cpp_type):
    p = type_.find(':')
    if p != -1:
        template_type = types[type_[p+1:]][1]
        cpp_type += '<' + template_type + '>'
    return cpp_type

def ignore_template(type_):
    p = type_.find(':')
    if p != -1:
        type_ = type_[:p]
    return type_

def parse_field(field, type_, namespace, types, indent_h):
    """Return the body of the serialize() and unserialize() methods
    of a Structure."""
    out_h = out_cpp = ''
    if ignore_template(type_) not in types:
        raise InvalidType('%r is not a valid type (undefined)' % type_)
    realname = types[ignore_template(type_)][0]
    cpp_type = types[ignore_template(type_)][1]
    cpp_type = parse_template_field(types, type_, cpp_type)

    is_structure = types[ignore_template(type_)][2]
    out_h += '\n%s%s %s;' % (indent_h, cpp_type, field)
    out_serialize = '\n    buffer += %s.serialize();' % field
    out_unserialize = '\n    offset += %s.unserialize(buffer, availableBytes-offset);' % field
    return (out_h, out_serialize, out_unserialize)

def handle_fields(fields, namespace, class_, types, indent_h, ctor=None):
    """Return the body (ie. the fields) of a Structure and the implementation
    of the serialize() and unserialize() methods of this structure."""
    out_h = ''
    if fields:
        ctor_s = ': '+ctor if ctor else ''
        out_h += '\n%s%s() %s {}' % (indent_h, class_, ctor_s)
    out_h += '\n%s%s(%%s);' % (indent_h, class_) + \
            '\n%svirtual std::string serialize() const;' % indent_h + \
            '\n%svirtual uint16_t unserialize(char const *buffer, uint16_t availableBytes);' % indent_h
    out_cpp = ''
    serialize = '\nstd::string %s::serialize() const {\n    std::string buffer = "";' % namespace
    unserialize = 'uint16_t %s::unserialize(char const *buffer, uint16_t availableBytes) {' % \
            namespace
    unserialize += '\n    uint16_t offset = 0;'
    fields = fields or []
    constructor = '\n\n%s::%s(%%s)' % (namespace, class_)
    if ctor:
        constructor += ': ' + ctor
        if fields:
            constructor += ', '
    else:
        if fields:
            constructor += ': '
    constructor_params = []
    for field in fields:
        # This is the only way yaml allows us to access those data.
        (field, type_) = list(field.items())[0]
        constructor += '\n%s%s(%s_),' % (indent_h, field, field)
        constructor_params.append('%s %s_' % (parse_template_field(types, type_, types[ignore_template(type_)][1]), field))
        (out_h2, out_serialize, out_unserialize) = parse_field(field, type_,
                namespace, types, indent_h)
        out_h += out_h2
        serialize += out_serialize
        unserialize += out_unserialize
    out_h %= ', '.join(constructor_params)
    out_cpp += constructor[0:-1] % (', '.join(constructor_params)) + ' {\n}'
    out_cpp += serialize + '\n    return buffer;\n}\n'
    out_cpp += unserialize + '\n    return offset;\n}\n'
    return (out_h, out_cpp)

def main(infile):
    outfile_h = HEADER_H
    outfile_cpp = HEADER_CPP
    document = yaml.load(infile)
    structures = document['structures']
    opcodes = document['opcodes']

    types = {} # Format: {name: (cpptype, is_structure)}

    # First, we read all types and declare their associated (un)serialization
    # functions.
    for (type_, attributes) in document['types'].items():
        if 'type' in attributes:
            cpptype = attributes['type']
        else:
            cpptype = type_
        types[type_] = (upfirst(type_), cpptype, False)

    # This is useful for consistency
    for (key, value) in document['aliases'].items():
        types[key] = types[value]

    if not isinstance(structures, list):
        raise InvalidSyntax('Structures list is not a list')
    # We now declare the structures and implement them.
    for structure in structures:
        (structure, fields) = list(structure.items())[0]
        if not isinstance(fields, list):
            raise InvalidSyntax('Fields of structure %r is not a list' % structure)
        outfile_h += '\n    class %s : public Net::AbstractSerializable {' % upfirst(structure)
        outfile_h += '\n    public:'
        (out_h, out_cpp) = handle_fields(fields,
                'Ponyca::Net::' + upfirst(structure), upfirst(structure), types, '        ')
        outfile_h += out_h + '\n    };\n'
        outfile_cpp += out_cpp
        types[structure] = (upfirst(structure), 'Ponyca::Net::%s' % upfirst(structure), True)

    # Finally, we do the same to opcodes.
    outfile_h += '\n\n    namespace Packets {'
    for (opcode, data) in opcodes.items():
        fields = data['fields']
        name = data['name']
        outfile_h += '\n        class %s : public Net::AbstractPacket {' % upfirst(name)
        outfile_h += '\n        public:'
        (out_h, out_cpp) = handle_fields(fields,
                'Ponyca::Net::Packets::' + upfirst(name), upfirst(name), types,
                '            ', 'AbstractPacket(%s) '%opcode)
        outfile_cpp += out_cpp
        outfile_h += out_h
        outfile_h += '\n            static const uint16_t Opcode = %s;' % opcode
        outfile_h += '\n        };\n'
    outfile_h += '\n    }'
    outfile_h += '\n\n    enum class Opcodes {'
    for (opcode, data) in opcodes.items():
        name = data['name']
        outfile_h += '\n        %s = %s,' % (name, opcode)
    outfile_h += '\n    };'

    # We implement makePacket(opcode)
    outfile_cpp += '\n\nPonyca::Net::AbstractPacket* Ponyca::Net::makePacket(int16_t opcode) {'
    outfile_cpp += '\n    switch(opcode) {'
    for (opcode, data) in opcodes.items():
        name = data['name']
        outfile_cpp += '\n    case %s: return new Ponyca::Net::Packets::%s;' % (opcode, upfirst(name))
    outfile_cpp += '\n    default: return NULL;'
    outfile_cpp += '\n    }'
    outfile_cpp += '\n}'

    outfile_h += FOOTER_H
    outfile_cpp += FOOTER_CPP
    return (outfile_h, outfile_cpp)



if __name__ == '__main__':
    if len(sys.argv) < 4:
        print('Syntax: ./opcode_generator.py infile.yml outfile.h outfile.cpp')
        exit()
    with open(sys.argv[1], 'r') as fd:
        (outfile_h, outfile_cpp) = main(fd)
    try:
        os.unlink(sys.argv[2])
    except OSError:
        pass
    try:
        os.unlink(sys.argv[3])
    except OSError:
        pass
    with open(sys.argv[2], 'a') as fd:
        fd.write(outfile_h)
    with open(sys.argv[3], 'a') as fd:
        fd.write(outfile_cpp)
