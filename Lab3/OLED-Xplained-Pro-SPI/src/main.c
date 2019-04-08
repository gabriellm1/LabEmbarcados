#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

#define LED_PIO      PIOC
#define LED_PIO_ID   ID_PIOC
#define LED_IDX      8
#define LED_IDX_MASK (1 << LED_IDX)

// Botão Placa
#define BUT_PIO      PIOA
#define BUT_PIO_ID   ID_PIOA
#define BUT_IDX  11
#define BUT_IDX_MASK (1 << BUT_IDX)
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

volatile Bool but_flag;
volatile Bool but_stop;

volatile int freq;

int display_freq(int hz){
	char hnum[5];
	itoa(hz, hnum, 10);
	gfx_mono_draw_string(hnum,0,16, &sysfont);
}

void but_flag_callback(void){
	but_flag = true;
}
void but_p_freq_callback(void){
	freq+=10;
	display_freq(freq);
}
void but_m_freq_callback(void){
	if (freq-10 <= 0 ){
		freq = 0;
		display_freq(freq);
	} else{
		freq-=10;
		display_freq(freq);
	}
}
void but_stop_callback(void){
	pio_clear(LED_PIO, LED_IDX_MASK);
	freq = 0;
	display_freq(freq);
}


// Inicializa botao SW0 do kit com interrupcao
void io_init(void)
{
	board_init();
	/* Insert system clock initialization code here (sysclk_init()). */
	sysclk_init();

  // Configura led da placa
	pmc_enable_periph_clk(LED_PIO_ID);
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_IDX_MASK, PIO_DEFAULT);
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
  // Inicializa clock do periférico PIO responsavel pelo botao
	pmc_enable_periph_clk(BUT_PIO_ID);
  // Configura PIO para lidar com o pino do botão como entrada
  // com pull-up
	pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP);
  // Configura interrupção no pino referente ao botao e associa
  // função de callback caso uma interrupção for gerada
  // a função de callback é a: but_callback()
	  pio_handler_set(BUT_PIO,
					  BUT_PIO_ID,
					  BUT_IDX_MASK,
					  PIO_IT_RISE_EDGE,
					  but_flag_callback);
	  pio_handler_set(EBUT1_PIO,
					  EBUT1_PIO_ID,
					  EBUT1_PIO_IDX_MASK,
					  PIO_IT_FALL_EDGE,
					  but_p_freq_callback);
	  pio_handler_set(EBUT2_PIO,
					  EBUT2_PIO_ID,
					  EBUT2_PIO_IDX_MASK,
					  PIO_IT_FALL_EDGE,
					  but_m_freq_callback);
	  pio_handler_set(EBUT3_PIO,
					  EBUT3_PIO_ID,
					  EBUT3_PIO_IDX_MASK,
					  PIO_IT_FALL_EDGE,
					  but_stop_callback);
	// Ativa interrup??o
	  pio_enable_interrupt(EBUT1_PIO, EBUT1_PIO_IDX_MASK);
	  pio_enable_interrupt(EBUT2_PIO, EBUT2_PIO_IDX_MASK);
	  pio_enable_interrupt(EBUT3_PIO, EBUT3_PIO_IDX_MASK);

  // Ativa interrupção
  pio_enable_interrupt(BUT_PIO, BUT_IDX_MASK);

  // Configura NVIC para receber interrupcoes do PIO do botao
  // com prioridade 4 (quanto mais próximo de 0 maior)
  NVIC_EnableIRQ(BUT_PIO_ID);
  NVIC_SetPriority(BUT_PIO_ID, 4); // Prioridade 4
  
  NVIC_EnableIRQ(EBUT1_PIO_ID);
  NVIC_SetPriority(EBUT1_PIO_ID, 0); // Prioridade 4
  NVIC_EnableIRQ(EBUT2_PIO_ID);
  NVIC_SetPriority(EBUT2_PIO_ID, 0); // Prioridade 4
  NVIC_EnableIRQ(EBUT3_PIO_ID);
  NVIC_SetPriority(EBUT3_PIO_ID, 0); // Prioridade 4
}





int main (void)
{
	io_init();	
	delay_init();

	gfx_mono_ssd1306_init();
	//gfx_mono_draw_filled_circle(20, 16, 16, GFX_PIXEL_SET, GFX_WHOLE);
	char hnum[5];
	itoa(freq, hnum, 10);
	gfx_mono_draw_string(hnum, 0,16, &sysfont);
	gfx_mono_draw_string("Hz", 60,16, &sysfont);
	// Inicializa clock
	sysclk_init();
	// Desativa watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;

	// configura botao com interrupcao
	pio_set(LED_PIO, LED_IDX_MASK);


  /* Insert application code here, after the board has been initialized. */
	while(1) {

		if (freq==0)
		{
			pio_set(LED_PIO, LED_IDX_MASK);
			pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
		} else{
			pio_set(LED_PIO, LED_IDX_MASK);    // Coloca 1 no pino do LED
			delay_ms(freq);                   // Delay por software de 200 ms
			pio_clear(LED_PIO, LED_IDX_MASK);    // Coloca 0 no pino do LED
			delay_ms(freq); // Delay por software de 200 ms
		}                  
	}
}
