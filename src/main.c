/*************** t.c file *************/
#include <unistd.h>

#include "defines.h"
#include "exceptions.h"
#include "kbd.h"
#include "signal.h"
#include "string.h"
#include "timer.h"
#include "vid.h"

extern char _binary____image0_bmp_start;

void copy_vectors() {
  extern u32 vectors_start, vectors_end;
  u32* vectors_src = &vectors_start;
  u32* vectors_dst = (u32*)0;
  while (vectors_src < &vectors_end) *vectors_dst++ = *vectors_src++;
}

void IRQ_handler() {  // IRQ interrupt handler in C
  // read VIC status registers to find out which interrupt
  int vicstatus = VIC_STATUS;
  int sicstatus = SIC_STATUS;
  // VIC status BITs: timer0,1=4, uart0=13, uart1=14
  if (vicstatus & (1 << 4)) {  // bit4=1:timer0,1
    timer_handler(0);          // timer0 only
  }
  if (vicstatus & (1 << 31)) {   // PIC.bit31= SIC interrupts
    if (sicstatus & (1 << 3)) {  // SIC.bit3 = KBD interrupt
      kbd_handler();
    }
  }
}

int main() {
  int i;
  char line[128];
  color = RED;  // int color in vid.c file
  fbuf_init();  // initialize LCD display

  char* p = &_binary____image0_bmp_start;
  show_bmp(p, 0, 0);  // display a logo


  /* enable VIC interrupts: timer0 at IRQ3, SIC at IRQ31 */
  VIC_INTENABLE = 0;
  VIC_INTENABLE |= (1 << 4);   // timer0,1 at PIC.bit4
  VIC_INTENABLE |= (1 << 5);   // timer0,1 at PIC.bit4
  VIC_INTENABLE |= (1 << 31);  // SIC to PIC.bit31

  /* enable KBD IRQ on SIC */
  SIC_INTENABLE = 0;
  SIC_INTENABLE |= (1 << 3);  // KBD int=SIC.bit3
  timer_init();               // initialize timer
  timer_start(0);             // start timer0

  kbd_init();  // initialize keyboard driver
  printf("C3.2 start: test KBD and TIMER drivers\n");
  while (1) {
    color = CYAN;
    printf("Enter a line from KBD\n");
    kgets(line);
    printf("line = %s\n", line);
  }
}