# EasyMol++

c++ version of EasyMol molecular viewer.

To compile and test the program, enter the source dir and type the following command in terminal:
 
    g++ main.cxx MyDrawArea.cxx MolecularModel.cxx parser.cxx `pkg-config --cflags gtkglext-1.0 --libs gtk+-2.0` -lglut -o main -std=c++11
 
 then use the binary file created.
