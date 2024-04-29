// ---------------------------- Testing for the IC reentry bug - IC reentry print is located when a exception is launched (only if the reentry variable has been set, determining there has been a bug)

// Logging reentry using the variable used to catch the IC reentry
The running thread will now be terminated!
 IC Reentry detected. This will cause problems in the execution of the program
IC::Exception(10) => {thread=0x003fa7970007ba03,sp=0x0000000080400238,status=0x4
0100,cause=5,epc=0x0000000080005d4a,tval=0x0000000000000000} => bad ecall
The running thread will now be terminated!
 IC Reentry detected. This will cause problems in the execution of the program
IC::Exception(10) => {thread=0x003fa7970007ba03,sp=0x0000000080400238,status=0x4
0100,cause=5,epc=0x0000000080005d4a,tval=0x0000000000000000} => bad ecall
The running thread will now be terminated!
 IC Reentry detected. This will cause problems in the execution of the program
IC::Exception(10) => {thread=0x003fa7970007ba03,sp=0x0000000080400238,status=0x4
0100,cause=5,epc=0x0000000080005d4a,tval=0x0000000000000000} => bad ecall
The running thread will now be terminated!
 IC Reentry detected. This will cause problems in the execution of the program
IC::Exception(��) => {thread=0x���,sp=0x��������        ������  ,status=0x
�����,cause=�,epc=0x��������    ����,tval=0x����������������}make[2]: Leaving directory '/home/bridge/Projects/so2/osdi_epos/img'

// Looking at logs to see where the reentry starts to show... 1500hz for the timer
// Is possbile to see this behavior in the ceiling test log as well

riscv_cpu_do_interrupt: hart:1, async:0, cause:0000000000000009, epc:0x0000000080005c3a, tval:0x0000000000000000, desc=supervisor_ecall
riscv_cpu_do_interrupt: hart:1, async:1, cause:0000000000000007, epc:0x00000000800051fa, tval:0x0000000000000000, desc=m_timer
riscv_cpu_do_interrupt: hart:1, async:1, cause:0000000000000005, epc:0x00000000800051fa, tval:0x0000000000000000, desc=s_timer
riscv_cpu_do_interrupt: hart:1, async:0, cause:0000000000000009, epc:0x0000000080005c3a, tval:0x0000000000000000, desc=supervisor_ecall
riscv_cpu_do_interrupt: hart:1, async:1, cause:0000000000000007, epc:0x00000000800051fa, tval:0x0000000000000000, desc=m_timer
riscv_cpu_do_interrupt: hart:1, async:1, cause:0000000000000005, epc:0x00000000800051fa, tval:0x0000000000000000, desc=s_timer
riscv_cpu_do_interrupt: hart:1, async:0, cause:0000000000000009, epc:0x0000000080005c3a, tval:0x0000000000000000, desc=supervisor_ecall
riscv_cpu_do_interrupt: hart:1, async:1, cause:0000000000000007, epc:0x00000000800051fa, tval:0x0000000000000000, desc=m_timer
riscv_cpu_do_interrupt: hart:1, async:1, cause:0000000000000005, epc:0x00000000800051fa, tval:0x0000000000000000, desc=s_timer
riscv_cpu_do_interrupt: hart:1, async:0, cause:0000000000000009, epc:0x0000000080005c3a, tval:0x0000000000000000, desc=supervisor_ecall
riscv_cpu_do_interrupt: hart:1, async:1, cause:0000000000000007, epc:0x00000000800051fa, tval:0x0000000000000000, desc=m_timer
riscv_cpu_do_interrupt: hart:1, async:1, cause:0000000000000005, epc:0x00000000800051fa, tval:0x0000000000000000, desc=s_timer
riscv_cpu_do_interrupt: hart:1, async:0, cause:0000000000000009, epc:0x0000000080005c3a, tval:0x0000000000000000, desc=supervisor_ecall
riscv_cpu_do_interrupt: hart:1, async:1, cause:0000000000000007, epc:0x000000008000389c, tval:0x0000000000000000, desc=m_timer
riscv_cpu_do_interrupt: hart:1, async:1, cause:0000000000000005, epc:0x00000000800038b2, tval:0x0000000000000000, desc=s_timer
riscv_cpu_do_interrupt: hart:1, async:0, cause:0000000000000009, epc:0x0000000080005c3a, tval:0x0000000000000000, desc=supervisor_ecall
riscv_cpu_tlb_fill ad 2b rw 0 mmu_idx 1
riscv_cpu_tlb_fill address=2b ret 0 physical 000000000000002b prot 7
riscv_cpu_tlb_fill PMP address=000000000000002b ret 0 prot 7 tlb_size 1
riscv_cpu_tlb_fill ad 28 rw 0 mmu_idx 1
riscv_cpu_tlb_fill address=28 ret 0 physical 0000000000000028 prot 7
riscv_cpu_tlb_fill PMP address=0000000000000028 ret 0 prot 7 tlb_size 1
riscv_cpu_do_interrupt: hart:1, async:0, cause:0000000000000005, epc:0x000000008000399e, tval:0x0000000000000028, desc=fault_load
riscv_cpu_do_interrupt: hart:1, async:1, cause:0000000000000007, epc:0x0000000080005dd6, tval:0x0000000000000000, desc=m_timer
riscv_cpu_do_interrupt: hart:1, async:1, cause:0000000000000007, epc:0x00000000800049a4, tval:0x0000000000000000, desc=m_timer
...
// ----------------------------------------- Testing for machine mode (gdb) -- running the same philosophers dinner test - expecting the test to run normally - and printing out mstatus

Thread 2.1 hit Breakpoint 1, 0x0000000080004c82 in EPOS::S::IC::dispatch() ()
(gdb) c

// info mem () 
Thread 2.1 hit Breakpoint 1, 0x0000000080004c82 in EPOS::S::IC::dispatch() ()
(gdb) info mem
Using memory regions provided by the target.
There are no memory regions defined.
(gdb) 

