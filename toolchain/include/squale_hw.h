// Apollo Squale memory mapping
// (c) 2016-2019 HxC2001 / Jean-François DEL NERO

#define HW_RAM_BASE               0x0000
#define HW_EF9365                 0xF000

#define HW_CTLHRD_REG             0xF010 // (U66 - 74LS274)
////////////////////////////////////////////////////////////
// Control register (U66)
//         Bit 6: Keyboard LED
//         Bit 5: Cas modem
//         Bit 4: K7 modem
//         Bit 3: HRI
//         Bit 2: HRR
//         Bit 1: HRV
//         Bit 0: HRB
////////////////////////////////////////////////////////////

#define HW_VIDMEM_RD1             0xF020
#define HW_VIDMEM_RD2             0xF030
#define HW_EF6821_PA_KEYB         0xF044 // PIA CONTROLLER 1 (U75 PA)
#define HW_EF6821_PB_KEYB         0xF046 // PIA CONTROLLER 2 (U75 PB)
#define HW_EF6821_PA_CARTRIDGE    0xF048 // PIA Cartridge    (U72 PA)
#define HW_EF6821_PB_PRINTER      0xF04A // PIA Printer      (U72 PB)
#define HW_EF6850_MODEM_TAPE      0xF050 // ACIA MODEM/TAPE
#define HW_AY_3_8910A_SOUND       0xF060 // Sound + Joystick 1/2 + Keyboard Ctrl/Shift/Shift lock
#define HW_EXTENSION_BASE         0xF080

////////////////////////////////////////////////////////////
// Floppy controller
#define HW_WD1770_FDC_CTL         0xF080
#define HW_WD1770_FDC_TRK         0xF081
#define HW_WD1770_FDC_SEC         0xF082
#define HW_WD1770_FDC_DAT         0xF083

#define HW_EF6821_PB_FDC_SEL      0xF08A // Reg B (EF6821 base address -> 0xF088)
////////////////////////////////////////////////////////////
// FDC Select Register (EF6821 port B)
// Reg B : Bit 5: /DDEN (/DOUBLE DENSITY ENABLE)
//         Bit 4: /SIDE
//         Bit 3: /DS0
//         Bit 2: /DS1
//         Bit 1: /DS2
//         Bit 0: /DS3
////////////////////////////////////////////////////////////

#define HW_AD_JOY                 0xF090

#define HW_ROM_BASE               0xF100

#define HW_RAM_SIZE               0xF000
#define HW_EXTENSION_SIZE         0x0080

#define FIRQ_VECTOR               0xEFF8
#define IRQ_VECTOR                0xEFFA
#define NMI_VECTOR                0xEFFE

#define WR_BYTE(adr,data) *((volatile unsigned char *)adr) = ((unsigned char)data)
#define RD_BYTE(adr) *((volatile unsigned char *)adr)

#define WR_WORD(adr,data) *((volatile unsigned short *)adr) = ((unsigned short)data)
#define RD_WORD(adr) *((volatile unsigned short *)adr)
