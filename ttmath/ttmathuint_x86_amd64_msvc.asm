PUBLIC	adc_x64
PUBLIC	addindexed_x64
PUBLIC	addindexed2_x64

PUBLIC	sbb_x64
PUBLIC	subindexed_x64

PUBLIC	rcl_x64
PUBLIC	rcr_x64

PUBLIC	rcl2_x64
PUBLIC	rcr2_x64

public	div_x64

;
;	"rax, rcx, rdx, r8-r11 are volatile."
;	"rbx, rbp, rdi, rsi, r12-r15 are nonvolatile."
;

.CODE
       
        ALIGN       8

;----------------------------------------

adc_x64				PROC
        ; rcx = p1
        ; rdx = p2
        ; r8 = nSize
        ; r9 = nCarry

        xor		rax, rax
        mov		r11, 0
        sub		rax, r9		; sets CARRY if r9 != 0
        
 loop1:	
		mov		rax,qword ptr [rdx + r11 * 8]
		adc		qword ptr [rcx + r11 * 8], rax
		inc		r11
		dec		r8
		
		jnz		loop1
		
		setc	al
		movzx	rax, al
		
        ret

adc_x64				ENDP

;----------------------------------------

        ALIGN       8

;----------------------------------------

addindexed_x64	PROC

        ; rcx = p1
        ; rdx = nSize
        ; r8 = nPos
        ; r9 = nValue

		sub		rdx, r8				; rdx = remaining count of uints
loop1:
		add		qword ptr [rcx + r8 * 8], r9
		jnc		done
		
		inc		r8
		mov		r9, 1
		dec		rdx
		jnz		loop1
		
done:
		setc	al
		movzx	rax, al
		
        ret

addindexed_x64	ENDP

;----------------------------------------

        ALIGN       8

;----------------------------------------

addindexed2_x64	PROC

        ; rcx = p1
        ; rdx = b
        ; r8 = nPos
        ; r9 = nValue1
        ; [esp+0x28] = nValue2

		mov		r11, rcx			; table
		sub		rdx, r8				; rdx = remaining count of uints
		mov		r10, [esp+028h]		; r10 = nValue2

		add		qword ptr [r11 + r8 * 8], r10
		inc		r8
loop1:
		adc		qword ptr [r11 + r8 * 8], r9
		jnc		done
		
		inc		r8
		mov		r9, 0				; set to 0 -> cy still set!
		dec		rdx
		jnz		loop1
		
done:
		setc	al
		movzx	rax, al
		
        ret

addindexed2_x64	ENDP

;----------------------------------------

        ALIGN       8

        ALIGN       8

;----------------------------------------

sbb_x64				PROC

        ; rcx = p1
        ; rdx = p2
        ; r8 = nCount
        ; r9 = nCarry

        xor		rax, rax
        mov		r11, 0
        sub		rax, r9				; sets CARRY if r9 != 0
        
 loop1:	
		mov		rax,qword ptr [rdx + r11 * 8]
		sbb		qword ptr [rcx + r11 * 8], rax
		inc		r11
		dec		r8
		
		jnz		loop1
		
		setc	al
		movzx	rax, al
		
        ret

sbb_x64				ENDP

;----------------------------------------

        ALIGN       8

;----------------------------------------

subindexed_x64	PROC
        ; rcx = p1
        ; rdx = nSize
        ; r8 = nPos
        ; r9 = nValue

		sub		rdx, r8				; rdx = remaining count of uints
loop1:
		sub		qword ptr [rcx + r8 * 8], r9
		jnc		done
		
		inc		r8
		mov		r9, 1
		dec		rdx
		jnz		loop1
		
done:
		setc	al
		movzx	rax, al
		
        ret

subindexed_x64	ENDP

;----------------------------------------

        ALIGN       8

;----------------------------------------

rcl_x64	PROC
        ; rcx = p1
        ; rdx = b
        ; r8 = nLowestBit

		mov		r11, rcx			; table
		xor		r10, r10
		neg		r8					; CY set if r8 <> 0
loop1:
		rcl		qword ptr [r11 + r10 * 8], 1
		inc		r10
		dec		rdx
		jnz		loop1
		
		setc	al
		movzx	rax, al
		
        ret

rcl_x64	ENDP

;----------------------------------------

        ALIGN       8

;----------------------------------------

rcr_x64	PROC
        ; rcx = p1
        ; rdx = nSize
        ; r8 = nLowestBit

		xor		r10, r10
		neg		r8					; CY set if r8 <> 0
loop1:
		rcr		qword ptr -8[rcx + rdx * 8], 1
		dec		rdx
		jnz		loop1
		
		setc	al
		movzx	rax, al
		
        ret

rcr_x64	ENDP

;----------------------------------------

        ALIGN       8

;----------------------------------------

div_x64	PROC

        ; rcx = &Hi
        ; rdx = &Lo
        ; r8 = nDiv
        
        mov		r11, rcx
        mov		r10, rdx

        mov		rdx, qword ptr [r11]
        mov		rax, qword ptr [r10]
        div		r8
        mov		qword ptr [r10], rdx ; remainder
        mov		qword ptr [r11], rax ; value

        ret

div_x64	ENDP

;----------------------------------------

        ALIGN       8

;----------------------------------------

rcl2_x64	PROC
        ; rcx = p1
        ; rdx = nSize
        ; r8 = bits
        ; r9 = c
        
        mov		r10, rcx	; r10 = p1
        xor		rax, rax	
        
        mov		rcx, 64
        sub		rcx, r8
        
        mov		r11, -1
        shr		r11, cl		; r11 = mask

		mov		rcx, r8		; rcx = count of bits

		mov		rbx, rax	; rbx = old value = 0 
		or		r9, r9
		cmovnz	rbx, r11	; if (c) then old value = mask

        mov		r9, rax		; r9 = index (0..nSize-1)

loop1:
		rol		qword ptr [r10+r9*8], cl
		mov		rax, qword ptr [r10+r9*8]
		and		rax, r11
		xor		qword ptr [r10+r9*8], rax
		or		qword ptr [r10+r9*8], rbx
		mov		rbx, rax
		
		inc		r9
		dec		rdx

		jnz		loop1

		and		rax, 1
        ret
				
rcl2_x64	ENDP				

;----------------------------------------

        ALIGN       8

;----------------------------------------

rcr2_x64	PROC
        ; rcx = p1
        ; rdx = nSize
        ; r8 = bits
        ; r9 = c
        
        mov		r10, rcx	; r10 = p1
        xor		rax, rax	
        
        mov		rcx, 64
        sub		rcx, r8
        
        mov		r11, -1
        shl		r11, cl		; r11 = mask

		mov		rcx, r8		; rcx = count of bits

		mov		rbx, rax	; rbx = old value = 0 
		or		r9, r9
		cmovnz	rbx, r11	; if (c) then old value = mask

        mov		r9, rdx		; r9 = index (0..nSize-1)
        dec		r9

loop1:
		ror		qword ptr [r10+r9*8], cl
		mov		rax, qword ptr [r10+r9*8]
		and		rax, r11
		xor		qword ptr [r10+r9*8], rax
		or		qword ptr [r10+r9*8], rbx
		mov		rbx, rax
		
		dec		r9
		dec		rdx

		jnz		loop1
		
		rol		rax, 1
		and		rax, 1
		
        ret
				
rcr2_x64	ENDP				

END
