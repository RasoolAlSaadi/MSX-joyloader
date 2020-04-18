
;******************************************************************************
; Joystick I/O port ROM loader v0.3
; MSX ROM loader through general purpose I/O (Joystick) port
; Developed by Rasool Al-Saadi (2020)
; works with 16KB and 32KB roms
; Some ideas/code had been taken from https://github.com/rolandvans/msxvdisk 
;******************************************************************************

chput  equ 00a2h
chget  equ 009fh

enaslt equ 0024h
exptbl equ 0fcc1h
rslreg equ 0138h


;db $fe
;dw start, end, start

	org 0d000h

start:
	ld hl, message
	call putstr
 
	ld hl,20000
delay:
	nop
	nop
	nop
	nop
	nop
	nop
	dec hl
	ld a,h
	or l
	jp nz, delay

	di
	call seljoy2
	call pin8low
	
	ld c,1
	ld hl, menudata
	call read512bytes
	call pin8low
	ei
	
	ld hl,menudata
.mloop:
	ld a,(hl)
	and a
	jp z,mdone
	
	push hl
	call putstr
	ld a,13
	call chput
	ld a,10
	call chput
	pop hl
	ld de,16
	add hl,de
	jp .mloop
mdone:
		
	ld hl, input 
	call readnumber

	; send user choice
	di
	call seljoy2
	ld hl, input
	ld de,4
	call sendbytes
	call pin8low

	;receive the selected rom info
	ld c,1
	ld hl, menudata
	call read512bytes
	call pin8low

	ld hl,loading
	call putstr

	; change slot
	call chgslot
	
	di
	call seljoy2
	;ld c,64 ; 64 * 512 bytes
	ld a,(menudata)
	ld c,a
	ld hl, 04000h ; loading address
readall
	call read512bytes
	dec c
	jp nz, readall

	; start the game
	ei
	ld hl, startgame
	call putstr
		
	ld hl, (04002h)
	jp (hl)
	ret

;------------------------------------
read512bytes:

	ld a, 14
	out (0a0h),a  ; select reg #14
notready:
	in a,(0a2h) ; read reg #14 (data)
	and 010h
	jp nz, notready

	ld a,15
	out (0a0h),a

	in a,(0a2h)
	and 11011111b
	ld d,a ; pin 8 low in d

	or 00100000b; set 5,a
	ld e,a	; pin 8 high in e

	call read256bytes

read256bytes:
	ld b,0
receiveloop:
	
	ld a,e
	out (0a1h),a			;set pin8 high
	
	ld a,14
	out (0a0h),a
	in a,(0a2h)		
	rrd					;store low 4-bits

	ld a,15
	out (0a0h),a
	
	ld a,d
	out (0a1h),a		;pin8 low
	
	ld a,14
	out (0a0h),a
	in a,(0a2h)		
	rrd				;store high 4-bits
	
	ld a,15
	out (0a0h),a
	   
	inc hl
	djnz receiveloop

	ld a,e
	out (0a1h),a		;pin8 high
ret

;------------------------------------
sendbytes:

	;ld hl,4000h
	;ld de,10

	ld a,15
	out (0a0h),a ; seletct reg #15
	in a, (0a2h)

outersendloop:
	ld b,8
	ld c,(hl)
    
sendloop:
	and 11011111b  ; set pin8 to low
	out (0a1h),a
		
	or 1000b  ; set pin7 to high
	rrc c
	jp c, nzero

	and 11110111b  ; set pin7 to low
nzero:
	or 100000b ;set pin8 to high
	out (0a1h),a ; wrire to reg #15

	djnz sendloop 
	   
	inc hl
	dec de

	ld b,a ;;
	ld a,d 
	or e 
	ld a,b ;;
	jp nz, outersendloop

	and 11011111b  ; set pin8 to low
	or 1100b	; pins 6,7 high
	out (0a1h),a ; wrire to reg #15

ret

;---------------
pin8low:
	ld a,15
	out (0a0h),a
	in a,(0a2h)
	and 11011111b
	;or 1100b
	out (0a1h),a
ret
    
; -----------------------------------

seljoy1:
	ld a,15
	out (0a0h),a ; seletct reg #15
	in a, (0a2h); read reg #15
	and 10111111b ;select joy1 (reset bit 6) 0bfh
	out (0a1h),a ; wrire to reg #15
	ret           

; -----------------------------------
seljoy2:
	ld a,15
	out (0a0h),a ; seletct reg #15
	in a, (0a2h); read reg #15
	or 01000000b ;select joy2 (set bit 6)
	out (0a1h),a ; wrire to reg #15
	ret    
;------------------------------------


chgslot:
	call rslreg ; read primary slot reg
	rrca
	rrca
	rrca
	rrca
	rrca
	rrca 
	and 03h ; get slot no for page 3 (ram)
	ld c,a 
	ld b,0
	ld hl, exptbl
	add hl,bc
	or (hl) ; (hl) = 080h if ext
	ld c,a
	inc hl
	inc hl
	inc hl
	inc hl
	ld a,(hl) ; secondary reg
	and 0c0h ; page 3
	rrca
	rrca
	rrca
	rrca
	or c
	ld h, 40h ; 0100 0000 (bits 6,7 include page number to exchange)
	jp enaslt

;--------------------------------------------
putstr:
	ld a, (hl)
	and a
	ret z
	call chput
	inc hl
	jp putstr

;--------------------------

readnumber:
    ld b,0
.readnumberloop
	call chget
	ld c, a

	cp 13
	jp z, done

	cp 8
	jp nz,	char
	ld a,b
	and a
	jp z,.readnumberloop

	ld a,8
	call	chput
	ld a,' '
	call	chput
	ld a, 8
	call	chput
	dec b
	dec hl
	jp .readnumberloop
char:
	ld a, b
	cp 3
	jp nc, .readnumberloop

	ld a,c
	cp '0'
	jp c, .readnumberloop
	cp '9'+1
	jp nc, .readnumberloop

	ld (hl),a
	inc hl
	inc b

	call	chput
	jp .readnumberloop
done:
	ld a, b
	and a
	jp z, .readnumberloop ; no number entered

	ld (hl),0
ret


message:
	db "Joystick I/O port ROM loader v0.3",13,10
	db "Developed by Rasool AL-Saadi (2020)",13,10,0
loading:
	db "Loading...",13,10,0
startgame: 
	db "starting the game",0
input:
	db 0,0,0,0

menudata:


end start
