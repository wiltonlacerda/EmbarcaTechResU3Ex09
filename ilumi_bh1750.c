#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "bh1750_light_sensor.h"
#include "ssd1306.h"
#include "font.h"
#include <math.h>

// Para o sensor de luz BH1750. Endereço 0x23
#define I2C_PORT i2c0               // i2c0 pinos 0 e 1, i2c1 pinos 2 e 3
#define I2C_SDA 0                   // 0 ou 2
#define I2C_SCL 1                   // 1 ou 3

// Display na I2C
#define I2C_PORT_DISP i2c1
#define I2C_SDA_DISP 14
#define I2C_SCL_DISP 15
#define endereco 0x3C


// Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define botaoB 6
void gpio_irq_handler(uint gpio, uint32_t events)
{
    reset_usb_boot(0, 0);
}

int main()
{
    // Para ser utilizado o modo BOOTSEL com botão B
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
   // Fim do trecho para modo BOOTSEL com botão B

   
    stdio_init_all();

    // I2C do Display pode ser diferente dos sensores. Funcionando em 400Khz.
    i2c_init(I2C_PORT_DISP, 400 * 1000);

    gpio_set_function(I2C_SDA_DISP, GPIO_FUNC_I2C);                    
    gpio_set_function(I2C_SCL_DISP, GPIO_FUNC_I2C);                    
    gpio_pull_up(I2C_SDA_DISP);                                        
    gpio_pull_up(I2C_SCL_DISP);                                        
    ssd1306_t ssd;                                                     
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT_DISP); 
    ssd1306_config(&ssd);                                              
    ssd1306_send_data(&ssd);                                           

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Inicializa o I2C0
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o sensor de luz BH1750
    bh1750_power_on(I2C_PORT);

    char str_lux[10];  // Buffer para armazenar a string
    
    bool cor = true;
    while (1)
    {   
        // Leitura do sensor de Luz BH1750
        uint16_t lux = bh1750_read_measurement(I2C_PORT);
        printf("Lux = %d\n", lux);

 
        sprintf(str_lux, "%d Lux", lux);  // Converte o inteiro em string
 
    
        // cor = !cor;
        //  Atualiza o conteúdo do display com animações
        ssd1306_fill(&ssd, !cor);                           // Limpa o display
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);       // Desenha um retângulo
        ssd1306_line(&ssd, 3, 25, 123, 25, cor);            // Desenha uma linha
        ssd1306_line(&ssd, 3, 37, 123, 37, cor);            // Desenha uma linha
        ssd1306_draw_string(&ssd, "CEPEDI   TIC37", 8, 6);  // Desenha uma string
        ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 16);   // Desenha uma string
        ssd1306_draw_string(&ssd, "Sensor  BH1750", 10, 28);// Desenha uma string
        ssd1306_line(&ssd, 63, 25, 63, 37, cor);            // Desenha uma linha vertical
        ssd1306_draw_string(&ssd, str_lux, 14, 41);         // Desenha uma string
        ssd1306_send_data(&ssd);                            // Atualiza o display

        sleep_ms(500);
    }

    return 0;
}