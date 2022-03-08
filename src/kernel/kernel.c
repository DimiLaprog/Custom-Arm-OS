#include "clib/printk.h"
#include "utils.h"
#include "timer.h"
#include "irq.h"
#include "sched.h"
#include "fork.h"
#include "mini_uart.h"
#include "sys.h"

void user_process1(char *array)
{
  call_sys_cat((unsigned int) 0);
  call_sys_cat((unsigned int) 1);
  call_sys_cat((unsigned int) 2);
	char buf[2] = {0};
	while (1){
		for (int i = 0; i < 5; i++){
			buf[0] = array[i];
			call_sys_write(buf);
			delay(10000000);
		}
	}
}

void user_process(){
	
  /* char buf[30] = {'U','S','E','R','\40','P','R','O','C','E','S','S','\40','S','T','A','R','T','E','D','\r','\n'}; */
	
	/* call_sys_write(buf); */
	unsigned long stack = call_sys_malloc();
	if (stack < 0) {
		printk("Error while allocating stack for process 1\n\r");
		return;
	}
	int err = call_sys_clone((unsigned long)&user_process1, (unsigned long)"12345", stack);
	if (err < 0){
		printk("Error while clonning process 1\n\r");
		return;
	} 
	stack = call_sys_malloc();
	if (stack < 0) {
		printk("Error while allocating stack for process 1\n\r");
		return;
	}
	err = call_sys_clone((unsigned long)&user_process1, (unsigned long)"abcd", stack);
	if (err < 0){
		printk("Error while clonning process 2\n\r");
		return;
	} 
	call_sys_exit();
}

void kernel_process(){
	printk("Kernel process started. EL %d\r\n", get_el());
	int err = move_to_user_mode((unsigned long)&user_process);
	if (err < 0){
		printk("Error while moving process to user mode\n\r");
	} 
}


void kernel_main(void)
{
	uart_init();
	irq_vector_init();
	timer_init();
	enable_interrupt_controller();
	enable_irq();
  

	int res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0, 0);
	if (res < 0) {
		printk("error while starting kernel process");
		return;
	}

	while (1){
		schedule();
	}	
}