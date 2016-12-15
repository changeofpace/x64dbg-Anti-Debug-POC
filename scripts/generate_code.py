import random
#==============================================================================
out_header_file_path = r".\generated_code.h"
out_source_file_path = r".\generated_code.cpp"
#==============================================================================
def write_function_open(out):   out.write("\n{\n")
#==============================================================================
def write_function_close(out):  out.write("}\n\n")
#==============================================================================
def write_semicolon(out):       out.write(";\n")
#==============================================================================
def generate_function_prototype(out, i):
    out.write("void generated_function_%d()" % i)
#==============================================================================
def generate_function_body(out, i):
    write_function_open(out)

    out.write("    const char* const str = \"generate_function_body_%i\";\n" % i)
    out.write("    int i = 0;\n")
    limit = random.randrange(i, 2000)
    # for x in range(0, limit):
    for x in range(0, 500):
        if x % 2 is 0:
            out.write("    i += %d;\n" % x)
        else:
            out.write("    i -= %d;\n" % x)

    write_function_close(out)
#==============================================================================
def generate_function(out, i):
    generate_function_prototype(out, i)
    generate_function_body(out, i)
#==============================================================================
def generate_functions(count):
    header = open(out_header_file_path, 'w+')
    for x in range(0, count):
        generate_function_prototype(header, x)
        write_semicolon(header)
    header.close()

    source = open(out_source_file_path, 'w+')

    source.write("#include \"generated_code.h\"\n\n\n")

    for x in range(0, count):
        generate_function(source, x)

    source.close()
#==============================================================================
def main():
    generate_functions(10)
#==============================================================================
if __name__ == "__main__":
    main()