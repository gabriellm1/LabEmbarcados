/**
 * 5 semestre - Eng. da Computação - Insper
 * Rafael Corsi - rafael.corsi@insper.edu.br
 *
 * Projeto 0 para a placa SAME70-XPLD
 *
 * Objetivo :
 *  - Introduzir ASF e HAL
 *  - Configuracao de clock
 *  - Configuracao pino In/Out
 *
 * Material :
 *  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
 */

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"


/************************************************************************/
/* defines                                                              */
/************************************************************************/
// Configurando Led
#define LED_PIO           PIOC                  // periferico que controla o LED
#define LED_PIO_ID        12                    // ID do periférico PIOC (controla LED)
#define LED_PIO_IDX       8u                    // ID do LED no PIO
#define LED_PIO_IDX_MASK  (1u << LED_PIO_IDX)   // Mascara para CONTROLARMOS o LED
// Configuracoes do botao
#define BUT_PIO           PIOA
#define BUT_PIO_ID		  10
#define BUT_PIO_IDX		  11u
#define BUT_PIO_IDX_MASK (1u << BUT_PIO_IDX)
/*  Default pin configuration (no attribute). */
#define _PIO_DEFAULT             (0u << 0)
/*  The internal pin pull-up is active. */
#define _PIO_PULLUP              (1u << 0)
/*  The internal glitch filter is active. */
#define _PIO_DEGLITCH            (1u << 1)
/*  The pin is open-drain. */
#define _PIO_OPENDRAIN           (1u << 2)
/*  The internal debouncing filter is active. */
#define _PIO_DEBOUNCE            (1u << 3)
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

void init(void);

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/
/**
 * \brief Set a high output level on all the PIOs defined in ul_mask.
 * This has no immediate effects on PIOs that are not output, but the PIO
 * controller will save the value if they are changed to outputs.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 */
void _pio_set(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_SODR = ul_mask;
}
/**
 * \brief Set a low output level on all the PIOs defined in ul_mask.
 * This has no immediate effects on PIOs that are not output, but the PIO
 * controller will save the value if they are changed to outputs.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 */

void _pio_clear(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_CODR = ul_mask;
}
/**
 * \brief Configure PIO internal pull-up.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 * \param ul_pull_up_enable Indicates if the pin(s) internal pull-up shall be
 * configured.
 */
void _pio_pull_up(Pio *p_pio, const uint32_t ul_mask,
		const uint32_t ul_pull_up_enable){
		/* Enable the pull-up(s) if necessary */
		if (ul_pull_up_enable) {
			p_pio->PIO_PUER = ul_mask;
			} else {
			p_pio->PIO_PUDR = ul_mask;
		}
 }
 /**
 * \brief Configure one or more pin(s) or a PIO controller as inputs.
 * Optionally, the corresponding internal pull-up(s) and glitch filter(s) can
 * be enabled.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask indicating which pin(s) to configure as input(s).
 * \param ul_attribute PIO attribute(s).
 */
void _pio_set_input(Pio *p_pio, const uint32_t ul_mask,
		const uint32_t ul_attribute)
{
	
	pio_pull_up(p_pio, ul_mask, ul_attribute & PIO_PULLUP);

	/* Enable Input Filter if necessary */
	if (ul_attribute & (PIO_DEGLITCH | PIO_DEBOUNCE)) {
		//ativa debounce
		p_pio->PIO_IFER = ul_mask;
		} else {
		//ativa deglitch
		p_pio->PIO_IFDR = ul_mask;
	}	
}

void _pio_set_output(Pio *p_pio, const uint32_t ul_mask,
const uint32_t ul_default_level,
const uint32_t ul_multidrive_enable,
const uint32_t ul_pull_up_enable)
{
	if (ul_multidrive_enable) {
		p_pio->PIO_MDER = ul_mask;
		} else {
		p_pio->PIO_MDDR = ul_mask;
	}

	if (ul_default_level) {
		p_pio->PIO_SODR = ul_mask;
		} else {
		p_pio->PIO_CODR = ul_mask;
	}

	
	p_pio->PIO_OER = ul_mask;
	p_pio->PIO_PER = ul_mask;
}
// Função de inicialização do uC
void init(void)
{
  // Initialize the board clock
  sysclk_init();
  
  // Desativa WatchDog Timer
  WDT->WDT_MR = WDT_MR_WDDIS;
  
  // Ativa o PIO na qual o LED foi conectado
  // para que possamos controlar o LED.
  pmc_enable_periph_clk(LED_PIO_ID);
  //Inicializa PC8 como saída
  _pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 1, 0, 0);
  
  // Inicializa PIO do botao
  pmc_enable_periph_clk(BUT_PIO_ID);
  
  // configura pino ligado ao botão como entrada com um pull-up.
	//pio_set_input(BUT_PIO, BUT_PIO_IDX_MASK, _PIO_PULLUP);
	_pio_set_input(BUT_PIO, BUT_PIO_IDX_MASK, _PIO_PULLUP);
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
int main(void)
{
  // inicializa sistema e IOs
  init();

  // super loop
  // aplicacoes embarcadas não devem sair do while(1).
  while (1)
  {		
	  
	  if(pio_get(BUT_PIO,PIO_INPUT ,BUT_PIO_IDX_MASK)==0){
		for (int i=1;i<=5;i++)
		{
					_pio_set(PIOC, LED_PIO_IDX_MASK);    // Coloca 1 no pino do LED
					delay_ms(1000);                   // Delay por software de 200 ms
					_pio_clear(PIOC, LED_PIO_IDX_MASK);    // Coloca 0 no pino do LED
					delay_ms(1000);                   // Delay por software de 200 ms
		}
	  }
  }
  return 0;
}
