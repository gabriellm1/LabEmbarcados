/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"
// defines
// LED
#define LED_PIO      PIOC
#define LED_PIO_ID   ID_PIOC
#define LED_IDX      8
#define LED_IDX_MASK (1 << LED_IDX)
//butao 1 oled
#define EBUT1_PIO PIOD //start EXT 9 PD28
#define EBUT1_PIO_ID 16
#define EBUT1_PIO_IDX 28
#define EBUT1_PIO_IDX_MASK (1u << EBUT1_PIO_IDX)
//butao 2 oled
#define EBUT2_PIO PIOA //pause  Ext 4 PA19 PA = 10
#define EBUT2_PIO_ID 10
#define EBUT2_PIO_IDX 19
#define EBUT2_PIO_IDX_MASK (1u << EBUT2_PIO_IDX)
//butao 3 oled
#define EBUT3_PIO PIOC //sei la EXT 3 PC31
#define EBUT3_PIO_ID 12 // piod ID
#define EBUT3_PIO_IDX 31
#define EBUT3_PIO_IDX_MASK (1u << EBUT3_PIO_IDX)
////////////////////////////////////////////////////

// handlers / callbacks
volatile Bool but_flag_1;
volatile Bool but_flag_2;
volatile int freq = 500;
volatile Bool pisca = false;

void but1_callback(void){
	freq *= 2;
	but_flag_1 = true;
}
void but2_callback(void){
	freq = freq/2;
	but_flag_2 = true;
}
void but3_callback(void){
	pisca = !pisca;
}
// functions


void pisca_led(Pio *PIO, int MASK,int freq,Bool pisca){
	while(!pisca)
	{
		pio_clear(PIO, MASK);
		delay_ms(freq);
		pio_set(PIO, MASK);
		delay_ms(freq);	
	}
}


void init(){
	/* Insert system clock initialization code here (sysclk_init()). */
	board_init();
	sysclk_init();
	// inicia led da placa
	pmc_enable_periph_clk(LED_PIO_ID);
	// configura botoes do oled
	pmc_enable_periph_clk(EBUT1_PIO_ID);
	pmc_enable_periph_clk(EBUT2_PIO_ID);
	pmc_enable_periph_clk(EBUT3_PIO_ID);
	// configura botoes do oled como input
	pio_set_input(EBUT1_PIO,EBUT1_PIO_IDX_MASK,PIO_DEFAULT);
	pio_pull_up(EBUT1_PIO,EBUT1_PIO_IDX_MASK,PIO_PULLUP);
	pio_set_input(EBUT2_PIO,EBUT2_PIO_IDX_MASK,PIO_DEFAULT);
	pio_pull_up(EBUT2_PIO,EBUT2_PIO_IDX_MASK,PIO_PULLUP);
	pio_set_input(EBUT3_PIO,EBUT3_PIO_IDX_MASK,PIO_DEFAULT);
	pio_pull_up(EBUT3_PIO,EBUT3_PIO_IDX_MASK,PIO_PULLUP);
	// Configura interrup??o no pino referente ao botao e associa
	// fun??o de callback caso uma interrup??o for gerada
	// a fun??o de callback ? a: but_callback()
	pio_handler_set(EBUT1_PIO,
	EBUT1_PIO_ID,
	EBUT1_PIO_IDX_MASK,
	PIO_IT_FALL_EDGE,
	but1_callback);
	pio_handler_set(EBUT2_PIO,
	EBUT2_PIO_ID,
	EBUT2_PIO_IDX_MASK,
	PIO_IT_FALL_EDGE,
	but2_callback);
	pio_handler_set(EBUT3_PIO,
	EBUT3_PIO_ID,
	EBUT3_PIO_IDX_MASK,
	PIO_IT_FALL_EDGE,
	but3_callback);
	// Ativa interrup??o
	pio_enable_interrupt(EBUT1_PIO, EBUT1_PIO_IDX_MASK);
	pio_enable_interrupt(EBUT2_PIO, EBUT2_PIO_IDX_MASK);
	pio_enable_interrupt(EBUT3_PIO, EBUT3_PIO_IDX_MASK);
	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais pr?ximo de 0 maior)
	NVIC_EnableIRQ(EBUT1_PIO_ID);
	NVIC_SetPriority(EBUT1_PIO_ID, 4); // Prioridade 4
	NVIC_EnableIRQ(EBUT2_PIO_ID);
	NVIC_SetPriority(EBUT2_PIO_ID, 4); // Prioridade 4
	NVIC_EnableIRQ(EBUT3_PIO_ID);
	NVIC_SetPriority(EBUT3_PIO_ID, 2); // Prioridade 4
	


}



int main (void)
{

	init();
	//int freq = 1200;
	delay_init();
	gfx_mono_ssd1306_init();

  /* Insert application code here, after the board has been initialized. */
	while(1) {

		
	
			pio_clear(LED_PIO, LED_IDX_MASK);
			delay_ms(freq);
			pio_set(LED_PIO, LED_IDX_MASK);
			delay_ms(freq);

			// Entra em sleep mode
			// C?digo 'trava' aqui at? ser
			// 'acordado'
			//pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);

		
		

	}
}
