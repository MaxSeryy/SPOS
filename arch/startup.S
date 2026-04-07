.syntax unified         // unified syntax for armv6-m
.cpu cortex-m0plus      // target cpu
.thumb                  // use thumb instruction set

.global _start          // entry point for linker
.global Reset_Handler
.global isr_vector      // interrupt vector table
.extern SysTick_Handler
.extern PendSV_Handler
.extern SVC_Handler

.section .entry, "ax"   // code section for entry point
.align 2                // align to 4 bytes
.thumb_func
_start:
    ldr r0, =Reset_Handler  // load address of Reset_Handler into r0
    bx r0                   // jump to Reset_Handler

.section .vectors, "a"
.align 2
isr_vector:                 // interrupt vector table
    .word _estack           // 0
    .word Reset_Handler     // 1
    .word Default_Handler   // 2
    .word Default_Handler   // 3
    .word 0                 // 4
    .word 0                 // 5
    .word 0                 // 6
    .word 0                 // 7
    .word 0                 // 8
    .word 0                 // 9
    .word 0                 // 10
    .word SVC_Handler       // 11
    .word 0                 // 12
    .word 0                 // 13
    .word PendSV_Handler    // 14
    .word SysTick_Handler   // 15

.section .text
.align 2
.thumb_func
Reset_Handler:
    ldr r1, =_sbss
    ldr r2, =_ebss
    movs r0, #0
loop_bss:
    cmp r1, r2
    bge call_main
    str r0, [r1]
    adds r1, r1, #4
    b loop_bss

call_main:
    bl main
    b .

.thumb_func
Default_Handler:
    b .