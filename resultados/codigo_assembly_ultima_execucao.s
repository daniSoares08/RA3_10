.data
.align 3
const_zero_double: .double 0.0
const_one_double: .double 1.0
temp_pow_base: .double 0.0
mem_ACC: .double 0.0
pilha_topo: .word 0
pilha_valores: .space 4096

const_0: .double 100
const_1: .double 50
const_2: .double 2
const_3: .double 7.0
const_4: .double 2.0
const_5: .double 3
const_6: .double 4
const_7: .double 5
const_8: .double 6
const_9: .double 9
const_10: .double 4
const_11: .double 2
const_12: .double 8
const_13: .double 0
const_14: .double 1
const_15: .double 3.14
const_16: .double 2.0
.syntax unified
.fpu vfpv3
.global _start
.text
_start:
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    ldr r0, =const_0
    vldr d0, [r0]
    bl push_d0
    bl pop_d0
    ldr r0, =mem_ACC
    vstr d0, [r0]
    bl push_d0
    ldr r0, =mem_ACC
    vldr d0, [r0]
    bl push_d0
    ldr r0, =mem_ACC
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_1
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    bgt cmp_true_1
    mov r0, #0
    b cmp_done_1
cmp_true_1:
    mov r0, #1
cmp_done_1:
    bl bool_from_r0
    bl push_d0
    bl pop_d0
    ldr r0, =const_zero_double
    vldr d1, [r0]
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    beq if_fim_0
    ldr r0, =mem_ACC
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_2
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vcvt.s32.f64 s0, d0
    vcvt.s32.f64 s1, d1
    vmov r0, s0
    vmov r1, s1
    cmp r1, #0
    beq int_div_zero_2
    bl dividir_inteiros_assinados
    vmov s0, r2
    vcvt.f64.s32 d0, s0
    bl push_d0
    b int_div_fim_2
int_div_zero_2:
    ldr r0, =const_zero_double
    vldr d0, [r0]
    bl push_d0
int_div_fim_2:
if_fim_0:
    ldr r0, =const_3
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_4
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vdiv.f64 d0, d0, d1
    bl push_d0
    ldr r0, =const_5
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_6
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vmul.f64 d0, d0, d1
    bl push_d0
    ldr r0, =const_7
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_8
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vadd.f64 d0, d0, d1
    bl push_d0
    bl pop_d1
    bl pop_d0
    vadd.f64 d0, d0, d1
    bl push_d0
    ldr r0, =const_9
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_10
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vcvt.s32.f64 s0, d0
    vcvt.s32.f64 s1, d1
    vmov r0, s0
    vmov r1, s1
    cmp r1, #0
    beq int_div_zero_3
    bl dividir_inteiros_assinados
    vmov s0, r3
    vcvt.f64.s32 d0, s0
    bl push_d0
    b int_div_fim_3
int_div_zero_3:
    ldr r0, =const_zero_double
    vldr d0, [r0]
    bl push_d0
int_div_fim_3:
    ldr r0, =const_11
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_12
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vcvt.s32.f64 s0, d1
    vmov r0, s0
    ldr r1, =temp_pow_base
    vstr d0, [r1]
    ldr r1, =const_one_double
    vldr d0, [r1]
pow_loop_4:
    cmp r0, #0
    beq pow_fim_4
    ldr r1, =temp_pow_base
    vldr d2, [r1]
    vmul.f64 d0, d0, d2
    sub r0, r0, #1
    b pow_loop_4
pow_fim_4:
    bl push_d0
while_ini_5:
    ldr r0, =mem_ACC
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_13
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    bgt cmp_true_6
    mov r0, #0
    b cmp_done_6
cmp_true_6:
    mov r0, #1
cmp_done_6:
    bl bool_from_r0
    bl push_d0
    bl pop_d0
    ldr r0, =const_zero_double
    vldr d1, [r0]
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    beq while_fim_5
    ldr r0, =mem_ACC
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_14
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vsub.f64 d0, d0, d1
    bl push_d0
    b while_ini_5
while_fim_5:
    @ RES 3: valor recuperado dos resultados anteriores deve ser ligado ao historico
    ldr r0, =const_zero_double
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_15
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_16
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vadd.f64 d0, d0, d1
    bl push_d0
fim:
    b fim

push_d0:
    ldr r1, =pilha_topo
    ldr r2, [r1]
    ldr r3, =pilha_valores
    add r3, r3, r2
    vstr d0, [r3]
    add r2, r2, #8
    str r2, [r1]
    bx lr

pop_d0:
    ldr r1, =pilha_topo
    ldr r2, [r1]
    cmp r2, #0
    beq pop_d0_zero
    sub r2, r2, #8
    str r2, [r1]
    ldr r3, =pilha_valores
    add r3, r3, r2
    vldr d0, [r3]
    bx lr
pop_d0_zero:
    ldr r0, =const_zero_double
    vldr d0, [r0]
    bx lr

pop_d1:
    bl pop_d0
    vmov.f64 d1, d0
    bx lr

bool_from_r0:
    cmp r0, #0
    beq bool_false
    ldr r0, =const_one_double
    vldr d0, [r0]
    bx lr
bool_false:
    ldr r0, =const_zero_double
    vldr d0, [r0]
    bx lr

dividir_inteiros_assinados:
    mov r2, #0
    mov r3, #0
    cmp r0, #0
    bge div_dividendo_ok
    rsb r0, r0, #0
    eor r2, r2, #1
    mov r3, #1
div_dividendo_ok:
    cmp r1, #0
    bge div_divisor_ok
    rsb r1, r1, #0
    eor r2, r2, #1
div_divisor_ok:
    mov r12, #0
div_loop:
    cmp r0, r1
    blt div_fim_loop
    sub r0, r0, r1
    add r12, r12, #1
    b div_loop
div_fim_loop:
    cmp r2, #0
    beq div_sinal_quociente_ok
    rsb r12, r12, #0
div_sinal_quociente_ok:
    cmp r3, #0
    beq div_sinal_resto_ok
    rsb r0, r0, #0
div_sinal_resto_ok:
    mov r2, r12
    mov r3, r0
    bx lr
