#Name: Ana Bindiu
#UCID: 30062098
#this code was referenced from the code provided in class: hello.pyx from 2-cython-example and us100.pyx from 5-us100

#import the functions from the C file
cdef extern from "GPIO_Example.c" : 
    int initSNES();
cdef extern from "GPIO_Example.c":
    void timedWait(int delay );
cdef extern from "GPIO_Example.c":
    int readSnesController();
cdef extern from "GPIO_Example.c":
    int peek(int peek_element);
cdef extern from "GPIO_Example.c":
    void poke(int poke_element, int poke_result);

#redefine the functions from C in python with different names
def initialize() :
    initSNES();
def wait(delay):
    timedWait(delay);
def read_controller():
    readSnesController();
def peek_function(peek_element):
    return peek(peek_element);
def poke_function(poke_element, poke_result):
    poke(poke_element, poke_result);

