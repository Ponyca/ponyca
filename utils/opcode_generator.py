#!/usr/bin/env python3

import os
import sys
import yaml

HEADER_H = """
#ifndef PONYCA_PROTOCOL_H
#define PONYCA_PROTOCOL_H

#include <stdint.h>

namespace Ponyca::Protocol
{
    class Structure
    {
        serialize(std::string &buffer) = 0;
        static Structure unserialize(std::string &buffer) = 0;
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

def parse_field(field, type_, namespace, types, offset):
    out_h = out_cpp = ''
    if type_ not in types:
        raise InvalidType('%r is not a valid type (undefined)' % type_)
    cpp_type = types[type_][0]
    out_h += '\n        public %s %s;' % (cpp_type, field)
    out_serialize = '\n    buffer += Ponyca::Structure::serialize'
    out_serialize += '(%s);' % field
    out_unserialize = '\n    %s += ' % field
    out_unserialize += 'Ponyca::Structure::serialize(buffer, offset + %i);' % \
            offset
    return (out_h, out_serialize, out_unserialize)

def main(infile):
    outfile_h = HEADER_H
    outfile_cpp = HEADER_CPP
    document = yaml.load(infile)
    structures = document['structures']
    opcodes = document['opcodes']

    types = {}
    for (type_, attributes) in document['types'].items():
        if 'type' in attributes:
            types[type_] = (attributes['type'], attributes['size'])
        else:
            types[type_] = (type_, attributes['size'])

    if not isinstance(structures, list):
        raise InvalidSyntax('Structures list is not a list')
    for structure in structures:
        (structure, fields) = list(structure.items())[0]
        if not isinstance(fields, list):
            raise InvalidSyntax('Fields of structure %r is not a list' % structure)
        outfile_h += '\n    class %s : public Structure\n    {' % upfirst(structure)
        size = 0
        serialize = '\nPonyca::%s::serialize(std::string &buffer' % \
                upfirst(structure)
        serialize += ')\n{'
        unserialize = 'Ponyca::%s::unserialize(std::string &buffer)\n{' % \
                upfirst(structure)
        for field in fields:
            (field, type_) = list(field.items())[0]
            (out_h, out_serialize, out_unserialize) = parse_field(field, type_,
                    'Ponyca::' + structure, types, size)
            size += types[type_][1]
            outfile_h += out_h
            serialize += out_serialize
            unserialize += out_unserialize
        outfile_h += '\n        public uint32_t packetSize = %i;' % size
        outfile_h += '\n    };\n'
        outfile_cpp += serialize + '\n}\n'
        outfile_cpp += unserialize + '\n}\n'
        types[structure] = ('Ponyca::%s' % upfirst(structure), size)
    for opcode in opcodes:
        (opcode, data) = list(opcode.items())[0]
        fields = data['fields']
        name = data['name']
        outfile_h += '\n    class %s : public Packet\n    {' % upfirst(name)
        size = 0
        for field in fields:
            (field, attributes) = list(field.items())[0]
            (out_h, out_serialize, out_unserialize) = parse_field(field, attributes,
                    'Ponyca::' + upfirst(name), types, size)
            size += types[type_][1]
            outfile_h += out_h
        outfile_h += '\n        public uint32_t packetSize = %i;' % size
        outfile_h += '\n        public uint32_t opcode = %s;' % opcode
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
