    mov r0 1000000
    mov r5 1
    mov r1 1
    j [sum.body]
[sum.redo]
    add r1 2
[sum.body]
    mov r3 3
[mods.redo]
    mod r2 r1 r3
    jeq [sum.check] r2 0
    mul r2 r3 r3
    add r3 2
    jle [mods.redo] r2 r1
    add r5 1
[sum.check]
    jlt [sum.redo] r1 r0
    println r5
    