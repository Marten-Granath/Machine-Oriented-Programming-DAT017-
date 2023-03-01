/*
 * 	startup.c
 */
 
__attribute__((naked)) 
__attribute__((section (".start_section")) )

void startup ( void )
{
__asm__ volatile(" LDR R0,=0x2001C000\n");		/* set stack */
__asm__ volatile(" MOV SP,R0\n");
__asm__ volatile(" BL main\n");					/* call main */
__asm__ volatile(".L1: B .L1\n");				/* never return */
}


                // -- Definitions -- //


// Port Definition Adresses 
#define GPIO_D          ((volatile unsigned int   *) 0x40020C00)
#define GPIO_D_MODER    ((volatile unsigned int   *) 0x40020C00) 
#define GPIO_D_OTYPER   ((volatile unsigned int   *) 0x40020C04) 
#define GPIO_D_OSPEEDR  ((volatile unsigned int   *) 0x40020C08) 
#define GPIO_D_PUPDR    ((volatile unsigned int   *) 0x40020C0C)
#define GPIO_D_IDR      ((volatile unsigned short *) 0x40020C10)
#define GPIO_D_ODR_LOW  ((volatile unsigned char  *) 0x40020C14)
#define GPIO_D_ODR_HIGH ((volatile unsigned char  *) 0x40020C15)


// Memory Adress Definitions
// * For Timer 6
#define TIM6        ((volatile unsigned short * ) 0x40001000)
#define TIM6_CR1    ((volatile unsigned short * ) 0x40001000)
#define TIM6_SR     ((volatile unsigned short * ) 0x40001010)
#define TIM6_DIER   ((volatile unsigned short * ) 0x4000100C)
#define TIM6_CNT    ((volatile unsigned short * ) 0x40001024)
#define TIM6_PSC    ((volatile unsigned short * ) 0x40001028)
#define TIM6_ARR    ((volatile unsigned short * ) 0x4000102C)

// * For SCB Vector Table
#define SCB_VTOR ((volatile unsigned int *) 0xE000ED08)

// * For NVIC Module Functions
#define NVIC_TIM6_ISER ((volatile unsigned int *) 0xE000E104)

// * For Exception Handler
#define TIM6_IRQVEC ((void(**)(void)) 0x2001C118)

// Register Bit Definitions
#define UDIS (1<<1)
#define CEN  (1<<0)
#define UIE  (1<<0)
#define UIF  (1<<0)

#define NVIC_TIM6_IRQ_BPOS (1<<22) 


// Global Variables
volatile int ticks;
volatile int seconds;

                // -- Functions -- // 
         
void app_init(void)
{
    // Vector table on writable adress
    *SCB_VTOR = 0x2001C000;
}
       
void gpio_init(void)
{
    *GPIO_D_MODER = 0x00005555;
}

void timer6_interrupt(void)
{
    // 100 ms period
    *TIM6_SR &= ~UIF;
    ticks++;
    
    if (ticks > 9)
    {
        ticks = 0;
        seconds ++;
    }
}

void timer6_init(void)
{
    ticks = 0;
    seconds = 0;
    *TIM6_CR1 &= ~CEN;
    *TIM6_IRQVEC = timer6_interrupt;
    *NVIC_TIM6_ISER |= NVIC_TIM6_IRQ_BPOS;
    
    // 100 ms tidbas (0,1 s)
    *TIM6_PSC = 839;
    *TIM6_ARR = 999;
    *TIM6_DIER |= UIE;
    *TIM6_CR1  |= CEN;
}

// Main Function
void main(void)
{
    char nbcd;
    app_init();
    gpio_init();
    timer6_init();
    
    while (1)
    {
        nbcd = (seconds/10) << 4;
        nbcd |= (seconds % 10) & 0xF;
        *GPIO_D_ODR_LOW = nbcd;
    }
}

