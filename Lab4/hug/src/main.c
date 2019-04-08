#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

#define LED_PIO      PIOC
#define LED_PIO_ID   ID_PIOC
#define LED_IDX      8
#define LED_IDX_MASK (1 << LED_IDX)

// Bot?o Placa
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
int freq = 0;
int minu = 0;
int hora = 0;
volatile Bool but_p_freq;
volatile Bool but_m_freq;
volatile Bool but_stop;

void but_flag_callback(void){
	but_flag = true;
}
void but_p_freq_callback(void){
	but_p_freq = true;
}
void but_m_freq_callback(void){
	but_m_freq = true;
}
void but_stop_callback(void){
	but_stop = true;
}

void TC1_Handler(void){
	volatile uint32_t ul_dummy;

	/****************************************************************
	* Devemos indicar ao TC que a interrup??o foi satisfeita.
	******************************************************************/
	ul_dummy = tc_get_status(TC0, 1);

	/* Avoid compiler warning */
	UNUSED(ul_dummy);

	freq+=1;
	//display_freq(freq);


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
	// Inicializa clock do perif?rico PIO responsavel pelo botao
	pmc_enable_periph_clk(BUT_PIO_ID);
	// Configura PIO para lidar com o pino do bot?o como entrada
	// com pull-up
	pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP);
	// Configura interrup??o no pino referente ao botao e associa
	// fun??o de callback caso uma interrup??o for gerada
	// a fun??o de callback ? a: but_callback()
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

	// Ativa interrup??o
	pio_enable_interrupt(BUT_PIO, BUT_IDX_MASK);

	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais pr?ximo de 0 maior)
	NVIC_EnableIRQ(BUT_PIO_ID);
	NVIC_SetPriority(BUT_PIO_ID, 4); // Prioridade 4

	NVIC_EnableIRQ(EBUT1_PIO_ID);
	NVIC_SetPriority(EBUT1_PIO_ID, 0); // Prioridade 4
	NVIC_EnableIRQ(EBUT2_PIO_ID);
	NVIC_SetPriority(EBUT2_PIO_ID, 0); // Prioridade 4
	NVIC_EnableIRQ(EBUT3_PIO_ID);
	NVIC_SetPriority(EBUT3_PIO_ID, 0); // Prioridade 4
}

void pisca_led(int hz){

	for (int i=0;i<hz;i++){
		pio_clear(LED_PIO, LED_IDX_MASK);
		delay_ms(200);
		pio_set(LED_PIO, LED_IDX_MASK);
		delay_ms(200);
	}
	but_flag = false;


}

int display_freq(int hz, int minu, int hora){
	//gfx_mono_draw_string("      ",0,16, &sysfont);

	char hnum[5];
	itoa(hora, hnum, 10);
	gfx_mono_draw_string(hnum,0,16, &sysfont);

	char hnum1[5];
	itoa(minu, hnum1, 10);
	gfx_mono_draw_string(hnum1,30,16, &sysfont);

	char hnum2[5];
	itoa(hz, hnum2, 10);
	gfx_mono_draw_string(hnum2, 60,16, &sysfont);
}


void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq){
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();

	uint32_t channel = 1;

	/* Configura o PMC */
	/* O TimerCounter ? meio confuso
	o uC possui 3 TCs, cada TC possui 3 canais
	TC0 : ID_TC0, ID_TC1, ID_TC2
	TC1 : ID_TC3, ID_TC4, ID_TC5
	TC2 : ID_TC6, ID_TC7, ID_TC8
	*/
	pmc_enable_periph_clk(ID_TC);

	/** Configura o TC para operar em  4Mhz e interrup?c?o no RC compare */
	tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
	tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

	/* Configura e ativa interrup?c?o no TC canal 0 */
	/* Interrup??o no C */
	NVIC_EnableIRQ((IRQn_Type) ID_TC);
	tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);

	/* Inicializa o canal 0 do TC */
	tc_start(TC, TC_CHANNEL);
}




int main (void)
{
	io_init();
	delay_init();

	gfx_mono_ssd1306_init();

	//gfx_mono_draw_filled_circle(20, 16, 16, GFX_PIXEL_SET, GFX_WHOLE);
	//char hnum[5];
	//itoa(freq, hnum, 10);
	//gfx_mono_draw_string(hnum, 0,16, &sysfont);
	//gfx_mono_draw_string("Hz", 60,16, &sysfont);

	gfx_mono_draw_string(":",50,16, &sysfont);
	gfx_mono_draw_string(":",20,16, &sysfont);

	// Inicializa clock
	sysclk_init();
	// Desativa watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;

	// configura botao com interrupcao
	pio_set(LED_PIO, LED_IDX_MASK);
	TC_init(TC0, ID_TC1, 1, 1);

	/* Insert application code here, after the board has been initialized. */
	while(1) {
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
		display_freq(freq, minu,hora);
		if(freq == 60){
			freq = 0;
			minu += 1;
			gfx_mono_draw_string("      ",60,16, &sysfont);
			display_freq(freq, minu,hora);
		}
		if(minu == 60){
			minu = 0;
			hora += 1;
			gfx_mono_draw_string("      ",15,16, &sysfont);
			gfx_mono_draw_string(":",50,16, &sysfont);
			display_freq(freq, minu,hora);
		}

		if(hora == 24){
			hora = 0;
			gfx_mono_draw_string("  ",0,16, &sysfont);
			gfx_mono_draw_string(":",20,16, &sysfont);
			display_freq(freq, minu,hora);
		}

		if(but_stop){
			minu += 1;
			but_stop = false;

		}
		else if(but_p_freq){
			hora += 1;
			but_p_freq=false;
		}
		else if(but_m_freq){
			freq += 1;
			but_m_freq=false;
		}
	}
}
