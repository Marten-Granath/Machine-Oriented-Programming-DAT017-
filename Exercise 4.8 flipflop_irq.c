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


                // -- Declarations -- //

                
// Port Definition Adresses 
// * Port D
#define GPIO_D          ((volatile unsigned int   *) 0x40020C00)
#define GPIO_D_MODER    ((volatile unsigned int   *) 0x40020C00) 
#define GPIO_D_OTYPER   ((volatile unsigned int   *) 0x40020C04) 
#define GPIO_D_OSPEEDR  ((volatile unsigned int   *) 0x40020C08) 
#define GPIO_D_PUPDR    ((volatile unsigned int   *) 0x40020C0C)
#define GPIO_D_IDR      ((volatile unsigned short *) 0x40020C10)
#define GPIO_D_ODR_LOW  ((volatile unsigned char  *) 0x40020C14)
#define GPIO_D_ODR_HIGH ((volatile unsigned char  *) 0x40020C15)

// * Port E
#define GPIO_E          ((volatile unsigned int  *) 0x40021000)
#define GPIO_E_MODER    ((volatile unsigned int  *) 0x40021000)
#define GPIO_E_OTYPER   ((volatile unsigned int  *) 0x40021004)
#define GPIO_E_OSPEEDR  ((volatile unsigned int  *) 0x40021008)
#define GPIO_E_PUPDR    ((volatile unsigned int  *) 0x4002100C)
#define GPIO_E_IDR_LOW  ((volatile unsigned char *) 0x40021010)
#define GPIO_E_IDR_HIGH ((volatile unsigned char *) 0x40021011)
#define GPIO_E_ODR_LOW  ((volatile unsigned char *) 0x40021014)
#define GPIO_E_ODR_HIGH ((volatile unsigned char *) 0x40021015)

// System Configuration Register Adresses (Exceptions)
#define SYSCFG          ((unsigned int  *) 0x40013800)
#define SYSCFG_EXTICR1  ((unsigned int  *) 0x40013808)

// External Interrupt & Event Controller Register Adresses
#define EXTI            ((unsigned int  *) 0x40013C00)
#define EXTI_IMR        ((unsigned int  *) 0x40013C00)
#define EXTI_EMR        ((unsigned int  *) 0x40013C04)
#define EXTI_RTSR       ((unsigned int  *) 0x40013C08)
#define EXTI_FTSR       ((unsigned int  *) 0x40013C0C)
#define EXTI_SWIER      ((unsigned int  *) 0x40013C10)
#define EXTI_PR         ((unsigned int  *) 0x40013C14)

// NVIC Module Adresses
#define NVIC            ((unsigned int  *) 0xE000E100)
#define NVIC_ISER_0     ((unsigned int  *) 0xE000E100)

// NVIC Module Bit Position
#define NVIC_IRQ_BPOS   (1<<9)



// Exception Vector Table
#define SCB_VTOR ((volatile unsigned int *) 0xE000ED08)
#define EXTI3_IRQVEC ((void (**)(void)) (0x2001C064))

// Global Variables
static int volatile count = 0;


                // -- Functions -- //
                
void interrupt_handler(void)
{
        count ++;
        *EXTI_PR |= 8; 
}


void init_app(void)
{
    // Set Port D:0-7 to Outport
    *GPIO_D_MODER &= 0xFFFF0000;
    *GPIO_D_MODER |= 0x00005555;
    
    // Set Port E:0-7 to Inport
    *GPIO_E_MODER &= 0xFFFF0000;
    *GPIO_E_MODER |= 0x00000000;
    
    // Set Port E, Pin 3 to Exception Line 3
    *SYSCFG_EXTICR1 &= 0x0FFF;
    *SYSCFG_EXTICR1 |= 0x4000;
    
    // Configure Exception Line 3 to Handle Exceptions
    *EXTI_IMR  |=  8; 
    *EXTI_RTSR |=  8;
    *EXTI_FTSR &= ~8;
    
    // Set Exception Vector Table in Configurable Memory Space
    *SCB_VTOR = 0x2001C000;
    
    // Initiate Exception Vector
    *EXTI3_IRQVEC = interrupt_handler;
    
    // Configure NVIC Module for specific exception
    *NVIC_ISER_0 |= NVIC_IRQ_BPOS; 
    
}

void main(void)
{
    init_app();
    while(1)
    {
        *GPIO_D_ODR_LOW = count;
    }
}

