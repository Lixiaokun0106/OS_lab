;***
;
; Copyright (c) 2008 北京英真时代科技有限公司。保留所有权利。
;
; 只有您接受 EOS 核心源代码协议（参见 License.txt）中的条款才能使用这些代码。
; 如果您不接受，不能使用这些代码。
;
; 文件名: boot.asm
;
; 描述: 引导扇区。
;
; 
;
;*******************************************************************************/

; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               boot.asm
;
;     PC 机加电后，CPU 进入实模式，分段管理内存，最多访问 1M 地址空间（没
; 有打开 A20 的情况下）。CPU 首先执行 BIOS 程序，在 BIOS 完成设备检测等工
; 作后，如果 BIOS 被设置为从软盘启动，则 BIOS 会将软盘的引导扇区（512 字节）
; 加载到物理地址 0x7C00 - 0x7DFF 处，然后将 CPU 的 CS 寄存器设置为 0x0000,
; 将 IP 寄存器设置为 0x7C00，接下来 CPU 就开始执行引导扇区中的程序。
;     由于段界限为 64K，所以在不修改段寄存器的情况下只能访问 0x0000 到 0xFFFF
; 的地址空间，软盘引导扇区就被加载到了此范围内，所以在软盘引导扇区程序中一般
; 不需要修改段寄存器。
;     此时的物理内存应该是下面的样子：
;
;                 +-------------------------------------+----------------------
;          0x0000 |                                     |
;                 |   BIOS 中断向量表 (1K)              |
;                 |   BIOS Interrupt Vector Table       |
;                 |                                     |
;                 +-------------------------------------+
;          0x0400 |   BIOS 数据区 (512 Bytes)           |
;                 |   BIOS Data Area                    |
;                 +-------------------------------------+
;          0x0600 |                                     |
;                 |                                     |
;                 |             用户可用(1)             |   常规内存 (640K)
;                 |                                     |  Conventional Memory
;                 |                                     |
;                 +-------------------------------------+
;          0x7C00 |   软盘引导扇区 (512 Bytes)          |
;                 |   Floppy Boot Sector                |
;                 +-------------------------------------+
;          0x7E00 |                                     |
;                 |                                     |
;                 |             用户可用(2)             |
;                 |                                     |
;                 |                                     |
;                 +-------------------------------------+----------------------
;         0xA0000 |                                     |
;                 |                                     |
;                 |   系统占用 (384K)                   |   上位内存 (384K)
;                 |                                     |   Upper Memory
;                 |                                     |
;                 +-------------------------------------+----------------------
;        0x100000 |                                     |
;                 |                                     |   扩展内存（只有进入保护模式才能访问）
;                 |               不可用                |  Extended Memory
;                 Z                                     Z
;                 |                                     |
;    物理内存结束 |                                     |
;                 +-------------------------------------+----------------------
;
;     EOS 的软盘引导扇区程序选择将 Loader.bin 从第一个用户可用区域的 0x1000 处开始
; 加载，即从 0x1000 到 0x7BFF，所以 Loader 最大只能为 0x7C00 - 0x1000 = 0x6C00
; 个字节。如果在保护模式中按照 4K 大小进行分页，则 Loader 就在一个页面的开始处。
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	org 0x7C00
	jmp short Start
	nop					; 这个 nop 不可少

LOADER_ORG				equ	0x1000				; Loader.bin 的起始地址

Start:
	; 初始化 CPU 的段寄存器为 CS 的值(0)，堆栈从 64K 向下增长
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	xor sp, sp
	mov bp, sp

	; 初始化屏幕
	mov	ax, 0x0600		; AH = 0x06,  AL = 0x00
	mov	bx, 0x0700		; 黑底白字(BH = 0x07)
	xor cx, cx			; 左上角: (列号  0, 行号  0)
	mov	dx, 0x184F		; 右下角: (列号 79, 行号 24)
	int	0x10
	
	; 软驱复位
	xor	ah, ah
	xor	dl, dl
	int	0x13
	
	; 将floppy.img中第2~5扇区（loader）的内容加载到0x1000处，共4个扇区
	
	mov bx, LOADER_ORG			     ; es:bx 指向 0:0x1000
	
	mov al, 4                        ;读取的扇区数（拟定loader占用4个扇区）
	mov ch, 0                        ;磁道
    mov cl, 2                        ;扇区号（读第二个扇区）
    
	mov dl, 0                        ;软驱A
	mov dh, 0                        ;0面
	
	mov ah,2                         ;读扇区
    int 13h
	
	; 文件读取完毕，关闭软驱马达
	mov	dx, 0x03F2
	xor	al, al
	out	dx, al
	
	; Loader.bin 加载完毕，跳转到 Loader.bin 执行
	jmp	0:LOADER_ORG

	;
	; 引导扇区代码结束，填充剩下的空间，使生成的二进制代码恰好为 512 字节
	;
	times 	510-($-$$)	db	0
	dw 	0xaa55					; 引导扇区激活标志
