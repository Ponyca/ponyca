#!/usr/bin/env python3

import os
import sys
import yaml

HEADER_H = """
#ifndef PONYCA_PROTOCOL_H
#define PONYCA_PROTOCOL_H

#include <stdint.h>
#include <string>
#include <vector>
#include <msgpack.hpp>

namespace Ponyca
{
    class Structure
    {
    public:
        virtual std::string& serialize();
        virtual uint64_t unserialize(std::string &buffer);
%s
    };
    class Packet : public Structure
    {
    };

"""

FOOTER_H = """
}

#endif // PONYCA_PROTOCOL_H
"""

HEADER_CPP = """
#include "protocol.h"

"""

FOOTER_CPP = """
"""

class InvalidSyntax(Exception):
    pass

class InvalidType(Exception):
    pass

def upfirst(name):
    return name[0].upper() + name[1:]

def parse_field(field, type_, namespace, types):
    out_h = out_cpp = ''
    if type_ not in types:
        raise InvalidType('%r is not a valid type (undefined)' % type_)
    cpp_type = types[type_][0]
    is_structure = types[type_][1]
    out_h += '\n        %s %s;' % (cpp_type, field)
    if is_structure:
        out_serialize = '\n    buffer += %s.serialize();' % field
        out_unserialize = '\n    size += %s.unserialize(buffer);' % field
    else:
        out_serialize = '\n    buffer += Ponyca::Structure::serialize%s(%s);' % \
                (upfirst(type_), field)
        out_unserialize = '\n    size += Ponyca::Structure::unserialize%s(buffer, %s);' % \
                (upfirst(type_), field)
    return (out_h, out_serialize, out_unserialize)

def handle_fields(fields, namespace, types):
    out_h = '\n        virtual std::string& serialize();' + \
            '\n        virtual uint64_t unserialize(std::string &buffer);'
    out_cpp = ''
    serialize = '\nstd::string& %s::serialize()\n{' % namespace
    serialize += '\n    std::string buffer;'
    unserialize = 'uint64_t %s::unserialize(std::string &buffer)\n{' % \
            namespace
    unserialize += '\n    uint64_t size = 0;'
    for field in fields:
        (field, type_) = list(field.items())[0]
        (out_h2, out_serialize, out_unserialize) = parse_field(field, type_,
                namespace, types)
        out_h += out_h2
        serialize += out_serialize
        unserialize += out_unserialize
    out_cpp += serialize + '\n}\n'
    out_cpp += unserialize + '\n    return size;\n}\n'
    return (out_h, out_cpp)

def main(infile):
    outfile_h = HEADER_H
    outfile_cpp = HEADER_CPP
    document = yaml.load(infile)
    structures = document['structures']
    opcodes = document['opcodes']

    types = {}
    converters = ''
    for (type_, attributes) in document['types'].items():
        if 'type' in attributes:
            cpptype = attributes['type']
        else:
            cpptype = type_
        types[type_] = (cpptype, False)
        percent = (upfirst(type_), cpptype)
        converters += '\n        uint64_t unserialize%s(std::string&, %s);' % percent
        converters += '\n        std::string& serialize%s(%s);' % percent
    outfile_h %= converters

    if not isinstance(structures, list):
        raise InvalidSyntax('Structures list is not a list')
    for structure in structures:
        (structure, fields) = list(structure.items())[0]
        if not isinstance(fields, list):
            raise InvalidSyntax('Fields of structure %r is not a list' % structure)
        outfile_h += '\n    class %s : public Structure\n    {' % upfirst(structure)
        outfile_h += '\n    public:'
        (out_h, out_cpp) = handle_fields(fields,
                'Ponyca::' + upfirst(structure), types)
        outfile_h += out_h + '\n    };\n'
        outfile_cpp += out_cpp
        types[structure] = ('Ponyca::%s' % upfirst(structure), True)
    for opcode in opcodes:
        (opcode, data) = list(opcode.items())[0]
        fields = data['fields']
        name = data['name']
        outfile_h += '\n    class %s : public Packet\n    {' % upfirst(name)
        outfile_h += '\n    public:'
        (out_h, out_cpp) = handle_fields(fields,
                'Ponyca::' + upfirst(name), types)
        outfile_cpp += out_cpp
        outfile_h += out_h
        outfile_h += '\n        const static uint32_t opcode = %s;' % opcode
        outfile_h += '\n    };\n'

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
