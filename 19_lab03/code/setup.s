        .code16

        .global _start, begtext, begdata, begbss, endtext, enddata, endbss
	.text
	begtext:
	.data
	begdata:
	.bss
	begbss:
	.text

	.equ INITSEG, 0x9000		# we move boot here - out of the way
	.equ SETUPSEG, 0x9020		# setup starts here

	ljmp    $SETUPSEG, $_start
_start:
	mov	%cs, %ax
	mov	%ax, %ds
	mov	%ax, %es

# Print message

	mov	$0x03, %ah		# read cursor pos
	xor	%bh, %bh
	int	$0x10
	
	mov	$28, %cx
	mov	$0x0007, %bx		# page 0, attribute 7 (normal)
	#lea	msg1, %bp
	mov     $msg1, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10

# ok, the read went well so we get current cursor position and save it for
# posterity.

	mov	$INITSEG, %ax	        # this is done in bootsect already, but...
	mov	%ax, %ds
	mov	$0x03, %ah	        # read cursor pos
	xor	%bh, %bh
	int	$0x10		        # save it in known place, con_init fetches
	mov	%dx, %ds:0	        # it from 0x90000.

# Get memory size (extended mem, kB)

	mov	$0x88, %ah 
	int	$0x15
	mov	%ax, %ds:2

# Get video-card data

	mov	$0x0f, %ah
	int	$0x10
	mov	%bx, %ds:4	        # bh = display page
	mov	%ax, %ds:6	        # al = video mode, ah = window width

# Get hd0 data

	mov	$0x0000, %ax
	mov	%ax, %ds
	lds	%ds:4*0x41, %si
	mov	$INITSEG, %ax
	mov	%ax, %es
	mov	$0x0080, %di
	mov	$0x10, %cx
	rep
	movsb

# Restore register ds & es

        mov     $INITSEG, %ax
        mov     %ax, %ds
        mov     $SETUPSEG, %ax
        mov     %ax, %es

# Print Cursor POS

        mov     $0x03, %ah              # read cursor pos
        xor     %bh, %bh
        int     $0x10
        mov     $11, %cx
        mov     $0x0007, %bx            # page 0, attribute 7
        mov     $cur, %bp
        mov     $0x1301, %ax            # write string, move cursor
        int     $0x10

        mov     %ds:0, %bp
        call    print_hex
        call    print_nl

# Print Memory SIZE
       
        mov     $0x03, %ah
        xor     %bh, %bh
        int     $0x10
        mov     $12, %cx
        mov	$0x0007, %bx		# page 0, attribute 7 (normal)
	mov     $mem, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10     
        
        mov     %ds:2, %bp
        call    print_hex
        
        mov     $0x03, %ah
        xor     %bh, %bh
        int     $0x10
        mov     $4, %cx
        mov	$0x0007, %bx		# page 0, attribute 7 (normal)
	mov     $kb, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10
  
        call    print_nl

# Print Video-card Data
        
        mov     $0x03, %ah
        xor     %bh, %bh
        int     $0x10
        mov     $26, %cx
        mov	$0x0007, %bx		# page 0, attribute 7 (normal)
	mov     $vid1, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10     
        
        mov     %ds:4, %bp
        call    print_hex
        call    print_nl
       
        mov     $0x03, %ah
        xor     %bh, %bh
        int     $0x10
        mov     $24, %cx
        mov	$0x0007, %bx		# page 0, attribute 7 (normal)
	mov     $vid2, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10  
        
        mov     %ds:6, %bp
        call    print_hex
        call    print_nl


# Print Hd0 Data
       
        mov     $0x03, %ah
        xor     %bh, %bh
        int     $0x10
        mov     $22, %cx
        mov	$0x0007, %bx		# page 0, attribute 7 (normal)
	mov     $cyl, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10     
        
        mov     %ds:0x80, %bp
        call    print_hex
        call    print_nl

        mov     $0x03, %ah
        xor     %bh, %bh
        int     $0x10
        mov     $8, %cx
        mov	$0x0007, %bx		# page 0, attribute 7 (normal)
	mov     $head, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10
        
        mov     %ds:0x80+0x02, %bp
        call    print_hex
        call    print_nl

        mov     $0x03, %ah
        xor     %bh, %bh
        int     $0x10
        mov     $8, %cx
        mov	$0x0007, %bx		# page 0, attribute 7 (normal)
	mov     $sect, %bp
	mov	$0x1301, %ax		# write string, move cursor
	int	$0x10
        
        mov     %ds:0x80+0x0e, %bp
        call    print_hex
        call    print_nl

inf_loop:
        jmp     inf_loop 

msg1:
	.byte 13,10
	.ascii "Now we are in SETUP..."
	.byte 13,10,13,10
	
cur:
        .ascii "Cursor POS:"

mem:
        .ascii "Memory SIZE:"

kb:     
        .ascii "KB"
        .byte 13,10

vid1:
        .ascii "Video-card:"
        .byte 13,10
        .ascii "display page:"

vid2: 
        .ascii "video mode/window width:"

cyl:
        .byte 13,10
        .ascii "HD0 Info"
        .byte 13,10
        .ascii "Cylinders:"

head:
        .ascii "Headers:"

sect:
        .ascii "Sectors:"

print_hex:
        mov    $4, %cx
        mov    %bp, %dx

print_digit:
        rol    $4, %dx
        mov    $0xe0f, %ax
        and    %dl, %al
        add    $0x30, %al
        cmp    $0x3a, %al
        jl     outp
        add    $0x07, %al

outp:
        int    $0x10
        loop   print_digit
        ret

print_nl:
        mov    $0xe0d, %ax
        int    $0x10
        mov    $0xa, %al
        int    $0x10
        ret

	.text
	endtext:
	.data
	enddata:
	.bss
	endbss:
