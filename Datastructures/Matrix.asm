;048C
;159D
;26AE
;37BF

;xmm0	01 23
;xmm1	45 67
;xmm2	89 AB
;xmm3	CD EF

;Shuffle
;dest[0] = src1[x]
;dest[1] = src1[y]
;dest[2] = src2[z]
;dest[3] = src2[w]
segment .text
; public: static void __cdecl Matrix<float,4,4>::Mul<4,4,4>(
;				struct _Matrix<float,4,4> &,
;				struct _Matrix<float,4,4> const &,
;				struct _Matrix<float,4,4> const &)
global ??$Mul@$03$03$03@?$Matrix@M$03$03@@SAXAEAU0@AEBU0@1@Z
??$Mul@$03$03$03@?$Matrix@M$03$03@@SAXAEAU0@AEBU0@1@Z:

;dest	= RCX
;a 		= RDX
;b		= R8

; xmm0 - xmm3 original A
; xmm4 - xmm11 temp storage for A (During transpose)
; xmm0 - xmm3 B
; xmm4 - xmm7 A transpose
; xmm8 - xmm11 Dest

; xmm6-15 must be preserved

; Required data -- this will likely stall us if the caller hasn't touched them (unavoidable)
movups xmm0, [rdx]
movups xmm2, [rdx+32]

sub rsp, 40 ; Space for xmm6/7
movdqu [rsp+16], xmm6 ; Save xmm6 (non-volatile)

; xmm8 = a[0123], xmm10 = a[89AB]

; Prefetch 'b' Matrix.
prefetchnta [r8]

; Preserve these registers
movdqu [rsp], xmm7 ; Save xmm7 (non-volatile)

; By the time those previous loads are done, the following data 
; is already in L1. Start the moves early anyway though.

sub rsp, 32 ; space for xmm8/9

movups xmm1, [rdx+16]

; We're in the process of getting the following
; xmm0	0123
; xmm1	4567
; xmm2	89AB
; xmm3	CDEF


;vshufps = lat 1 tp 1
vshufps xmm4, xmm0, xmm2, 0x00 ; 00000000 xmm4 = vshufps(xmm0, xmm2) [0, 0, 0, 0] [0080]
movdqu [rsp+16], xmm8 ; Save xmm8 (non-volatile)
vshufps xmm5, xmm0, xmm2, 0x11 ; 00010001 xmm5 = vshufps(xmm0, xmm2) [1, 0, 1, 0] [1090]
movups xmm3, [rdx+48]
vshufps xmm6, xmm0, xmm2, 0x22 ; 00100010 xmm6 = vshufps(xmm0, xmm2) [2, 0, 2, 0] [20A0]
movdqu [rsp], xmm9 ; Save xmm9 (non-volatile)
vshufps xmm7, xmm0, xmm2, 0x33 ; 00110011 xmm7 = vshufps(xmm0, xmm2) [3, 0, 3, 0] [30B0]

; xmm0 & xmm2 are not longer required.
; Begin loading B
movups xmm0, [r8]

; xmm1= a[4567], xmm3 = a[CDEF]
sub rsp, 32

vshufps xmm8, xmm1, xmm3, 0x00 ; 00000000 xmm8 = vshuffle(xmm1, xmm3) [0, 0, 0, 0] [040C]
movdqu [rsp-16], xmm10 ; Save xmm10 (non-volatile)
vshufps xmm9, xmm1, xmm3, 0x44 ; 01000100 xmm9 = vshuffle(xmm1, xmm3) [0, 1, 0, 1] [050D]
movups xmm2, [r8+32]
vshufps xmm10, xmm1, xmm3, 0x88 ; 10001000 xmm10 = vshuffle(xmm1, xmm3) [0, 2, 0, 2] [060E]
movdqu [rsp], xmm11 ; Save xmm11 (non-volatile)
vshufps xmm11, xmm1, xmm3, 0xCC ; 11001100 xmm11 = vshuffle(xmm1, xmm3) [0, 3, 0, 3] [070F]
movups xmm1, [r8+16]

; Load the rest of B

; We now have the following
; xmm4	0?8?
; xmm5	1?9?
; xmm6	2?A?
; xmm7	3?B?
; xmm8	?4?C
; xmm9	?5?D
; xmm10	?6?E
; xmm11	?7?F
; We need to blend them together to get the final result
; vblendps lat 1, tp 0.33-0.5
vblendps xmm4, xmm4, xmm8, 0x0A ; 0000 1010
vblendps xmm5, xmm5, xmm9, 0x0A ; 0000 1010
movups xmm3, [r8+48]
vblendps xmm6, xmm6, xmm10, 0x0A ; 0000 1010
vblendps xmm7, xmm7, xmm11, 0x0A ; 0000 1010

; xmm4-xmm7, A
; xmm0-xmm3 B
; xmm8-xmm11 Dest

;#REQUIRE xmm0 = b[0123], xmm1 = b[4567],
;	 xmm2 = b[89AB], xmm3 = b[CDEF]

;//xmm4-7 contain A [048C] [159D] [26AE] [37BF]
;//xmm0-3 contain B [0123] [4567] [89AB] [CDEF]

;//We can issue a DPPS ever 2nd cycle.
;//Use registers 8-11 as final result.
;//xmm8-11 as result[0123] [4567] [89AB] [CDEF]
;//xmm8  = [DOT(xmm0, xmm4), DOT(xmm1 xmm4), DOT(xmm2, xmm4), DOT(xmm3, xmm4)]
;//xmm9  = [DOT(xmm0, xmm5), DOT(xmm1 xmm5), DOT(xmm2, xmm5), DOT(xmm3, xmm5)]
;//xmm10 = [DOT(xmm0, xmm6), DOT(xmm1 xmm6), DOT(xmm2, xmm6), DOT(xmm3, xmm6)]
;//xmm11 = [DOT(xmm0, xmm7), DOT(xmm1 xmm7), DOT(xmm2, xmm7), DOT(xmm3, xmm7)]

;//Use registers 12-15 as temporary storage.
;vdpps will broadcast it's result -- all elements in dest are set.
vdpps xmm8, xmm4, xmm0, 0xFF	; xmm8 = Result[0000]
vdpps xmm9, xmm5, xmm0, 0xFF	; xmm9 = Result[4444]
vdpps xmm10, xmm6, xmm0, 0xFF	; xmm10 = Result[8888]
vdpps xmm11, xmm7, xmm0, 0xFF	; xmm11 = Result[CCCC]
vblendps xmm8, xmm8, xmm9, 0x02; xxxxxx10 xmm8 = Result[04xx]
vblendps xmm8, xmm8, xmm10, 0x04; xxxxx100 xmm8 = Result[048x]
vblendps xmm8, xmm8, xmm11, 0x08; xxxx1000 xmm8 = Result[048C]

;dest	= RCX
movups [rcx], xmm8

;#FREE xmm0
; xmm8

vdpps xmm9, xmm4, xmm1, 0xFF	; xmm9 = Result[1111]
vdpps xmm0, xmm5, xmm1, 0xFF	; xmm0 = Result[5555]
vdpps xmm10, xmm6, xmm1, 0xFF	; xmm10 = Result[9999]
vdpps xmm11, xmm7, xmm1, 0xFF	; xmm11 = Result[DDDD]
vblendps xmm9, xmm9, xmm0, 0x02 ; xxxxxx10 xmm9 = Result[15xx]
vblendps xmm9, xmm9, xmm10, 0x04; xxxxx100 xmm9 = Result[159x]
vblendps xmm9, xmm9, xmm11, 0x08; xxxx1000 xmm9 = Result[159D]

; asasd
movdqu xmm11, [rsp] ; Restore xmm11
add rsp, 32

movups [rcx+16], xmm9

;#FREE xmm1

vdpps xmm0, xmm4, xmm2, 0xFF	; xmm0 = Result[2222]
vdpps xmm1, xmm5, xmm2, 0xFF	; xmm1  = Result[6666]
movdqu xmm10, [rsp-16] ; Restore xmm10
vdpps xmm8, xmm6, xmm2, 0xFF	; xmm8 = Result[AAAA]
vdpps xmm9, xmm7, xmm2, 0xFF	; xmm9 = Result[EEEE]
vblendps xmm0, xmm0, xmm1, 0x02 ; xxxxxx10 xmm0 = Result[26xx]
vblendps xmm0, xmm0, xmm8, 0x04; xxxxx100 xmm0 = Result[26Ax]
vblendps xmm0, xmm0, xmm9, 0x08; xxxx1000 xmm0 = Result[26AE]

movups [rcx+32], xmm0
movdqu xmm9, [rsp] ; Restore xmm9

;#FREE xmm2

vdpps xmm1, xmm4, xmm3, 0xFF	; xmm1 = Result[3333]
vdpps xmm2, xmm5, xmm3, 0xFF	; xmm2  = Result[7777]
vdpps xmm8, xmm6, xmm3, 0xFF	; xmm8 = Result[BBBB]
vdpps xmm0, xmm7, xmm3, 0xFF	; xmm0 = Result[FFFF]
vblendps xmm1, xmm1, xmm2, 0x02 ; xxxxxx10 xmm1 = Result[37xx]
vblendps xmm1, xmm1, xmm8, 0x04 ; xxxxx100 xmm1 = Result[37Bx]
vblendps xmm1, xmm1, xmm0, 0x08 ; xxxx1000 xmm1 = Result[37BF]

movdqu xmm8, [rsp-16] ; Restore xmm8
add rsp, 32
movups [rcx+48], xmm1

movdqu xmm7, [rsp] ; Restore xmm7
movdqu xmm6, [rsp-16] ; Restore xmm6
add rsp, 40

ret

