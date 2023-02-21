import ctypes
from ctypes import *
import ctypes.util
import os
import sys
prog = b'''
@__entry
    r0 <- call main
    exit

func fib
    r0 <- int 2
    blt r1 r0 fib.else fib.then
@fib.then
    ret r1
@fib.else
    r0 <- int 1
    r1 <- sub r1 r0
    r0 <- sub r1 r0
    r1 <- call fib r1
    r0 <- call fib r0
    r0 <- add r0 r1
    ret r0
end

func putn
    r0 <- int 1
    blt r1 r0 putn.digit putn.ret 
@putn.digit
    r0 <- int 10
    r0 <- div r1 r0
    r0 <- call putn r0
    r0 <- int 10
    r1 <- mod r1 r0
    r0 <- int 48
    r1 <- add r1 r0
    putchar r1
@putn.ret
    r0 <- int 0
    ret r0
end

func main
    r0 <- int 35
    r0 <- call fib r0
    r0 <- call putn r0
    r0 <- int 10
    putchar r0
    exit
end'''
def do(prog):
    try:
        path = ctypes.util.find_library("minivm")
        assert path
        assert not os.path.isfile(os.path.join(os.getcwd(),path))
    except:
        path = ctypes.util.find_library(os.path.join(os.getcwd(),"minivm"))
        if not path:
            print('Could not find minivm DLL!')
            return
    vm = CDLL(path)
    vm_opcode_t = c_uint32
    (VM_OPCODE_EXIT,
    VM_OPCODE_REG,
    VM_OPCODE_JUMP,
    VM_OPCODE_FUNC,

    VM_OPCODE_CALL,
    VM_OPCODE_DCALL,
    VM_OPCODE_ADDR,
    VM_OPCODE_RET,

    VM_OPCODE_PUTCHAR,

    VM_OPCODE_INT,
    VM_OPCODE_NEG,
    VM_OPCODE_ADD,
    VM_OPCODE_SUB,
    VM_OPCODE_MUL,
    VM_OPCODE_DIV,
    VM_OPCODE_MOD,
    VM_OPCODE_BB,
    VM_OPCODE_BEQ,
    VM_OPCODE_BLT,

    VM_OPCODE_ARR,
    VM_OPCODE_SET,
    VM_OPCODE_GET,
    VM_OPCODE_LEN,

    VM_OPCODE_XCALL,
    VM_OPCODE_CCALL,

    VM_OPCODE_TYPE,
    VM_OPCODE_TAB,
    VM_OPCODE_NIL,
    VM_OPCODE_TRUE,
    VM_OPCODE_FALSE,

    VM_OPCODE_BOR,
    VM_OPCODE_BAND,
    VM_OPCODE_BXOR,
    VM_OPCODE_BSHL,
    VM_OPCODE_BSHR,

    VM_OPCODE_GETCHAR) = map(vm_opcode_t, range(36))
    class vm_bc_buf_t(Structure):
        _fields_ = [("ops", POINTER(vm_opcode_t)),("nops", c_size_t)]
    
    vm.vm_asm.restype = vm_bc_buf_t
    asm = vm.vm_asm(prog)
    if asm.nops == 0:
        return
    nblocks = buf.nops
    #print(asm.ops.contents)
    #print([asm.ops[x] for x in range(1000)])
    
do(prog)
