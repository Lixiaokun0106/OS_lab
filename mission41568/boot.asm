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
	
	mov ax,cs	;初始化寄存器
	mov	ds,ax
	mov	ds,ax
	mov	es,ax
	
	mov	ah,0x42
	mov	dl,0x80
	mov	si,packet
	int 0x13
	
	jc	error
	
	;将写入的内存地址后移512字节
    ;以下是将地址数据结构中的起始LBA后移1块继续写入下一个扇区

	;将packet的内存偏移512字节
	mov	ax,[bufferoff]
	mov	bx,bufferoff
	add	ax,0x0200
	mov	[bx],ax

	;将起始LBA加1，以读取下一个扇区
	mov	ax,[blockNum]
	mov	bx,blockNum
	add	ax,1
	mov	[bx],ax
	
	cmp	ax,5	;连续读取4个扇区
	je 	finish
	
	jmp	Start

;地址数据结构体
packet:
packet_size:     db	10h             ;packet大小，16个字节
reserved:	 	 db 0
count:		     dw	1		        ;读1个扇区(尝试过将这里调整为4，但是没效果，可能是循环和地址变化没设置好)
bufferoff:       dw	0x1000          ;读到内存0x1000处，偏移地址
bufferseg:	     dw	0		        ;段地址
blockNum:	 	 dd	1               ;起始LBA块，逻辑扇区号，boot是0号扇区，loader是1~4
		         dd 0
	
error:
	mov ax, 0xffff	
	
finish:	
	; Loader.bin 加载完毕，跳转到 Loader.bin 执行
	jmp	0:LOADER_ORG

	;
	; 引导扇区代码结束，填充剩下的空间，使生成的二进制代码恰好为 512 字节
	;
	
	times 443- ($-Start) db 0       ;前446字节的内容不满时，用0填充
	
	;硬盘分区表(共64字节，每16字节为一项，每项表示一个分区，共4项)
	;每项中第一个字节表示引导指示符，80h表示活动分区，00h表示无效分区
	db 80h
	;第二、三、四个字节在这里没有用到。
	db 00h, 00h, 00h
	;第五个字节定义分区类型。01h表示未定义的分区类型。
	db 01h
	;第六、七、八个字节在这里没有用到。
	db 00h, 00h, 00h
	;第九到第十二字节表示逻辑起始扇区号（相对于逻辑0扇区的扇区数）。一个扇区大小为 512 字节。
	dd 00010000h	;第一个分区前有 32MB 空白区域
	;第十三到第十六字节表示该分区所占用的扇区数（总扇区数）
	dd 00010000h	;第一个分区共 32MB 大小
	
	;其余三项全部为0，表示没有其他分区。
	times 16 db 0
	times 16 db 0
	times 16 db 0
	
	dw 	0xaa55					; 引导扇区激活标志
