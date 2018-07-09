#include "io.h"
#include "esp.h"
#include "mcu.h"
#include "nvic.h"

// Alias
#define DMA1_C2                 DMA1_Channel2
#define DMA1_C3                 DMA1_Channel3

// ������ ����� ���������� ����
#define ESP_DLL_SIZE            4
// ������������ ������ ����������� ����
#define ESP_APL_SIZE            28

// ����������� ��� ������������ ������
#define ESP_SPI_FILLER          0xFF
// ������� ESP8266 TX & RX ������������ ����������
#define ESP_SPI_CMD_RD_WR       0x06

// --- ���� ������ --- //

// ��� �������� (0 ����)
#define ESP_CMD_NOP             0x00

// ��������� ���� ������ �� SPI
FIELD_ALIGN_ONE
typedef struct
{
    // ��������� ����
    struct
    {
        // ���� ������� ESP8266
        uint8_t status;
        // ����� �������
        uint8_t command;
        // ���������� ����������� ������
        uint8_t length;
        // ���� ����������� �����
        uint8_t checksum;
    } dll;
    // ��������� ����
    union
    {
        uint8_t u8[ESP_APL_SIZE / sizeof(uint8_t)];
        uint16_t u16[ESP_APL_SIZE / sizeof(uint16_t)];
    } apl;
    
    // ���������� ������ (������� ����������� �����, ���������� ������ ����)
    void finalize(uint8_t status, uint8_t length)
    {
        // �������� ����������
        assert(length <= ESP_APL_SIZE);
        // ���������� DLL
        dll.status = status;
        dll.length = length;
        // ���������� ������ ������
        memset(&apl, ESP_SPI_FILLER, ESP_APL_SIZE - length);
        // ����������� �����
        for (uint8_t i = dll.checksum = 0; i < length; i++)
            dll.checksum += apl.u8[i];
    }
    
    // ���������� ���� ���� ������ �������
    void clear(void)
    {
        memset(this, ESP_SPI_FILLER, ESP_DLL_SIZE + ESP_APL_SIZE);
    }
} esp_packet_phase_t;

// ��������� ������ SPI
FIELD_ALIGN_ONE
typedef union
{
    // ����� ������
    uint8_t raw[ESP_DLL_SIZE + ESP_APL_SIZE];
    // ���� �������� � �����
    struct
    {
        esp_packet_phase_t tx;
        esp_packet_phase_t rx;
    };
} esp_packet_t;

// ����� ������ ��� DMA
DATA_ALIGN_HOUR
static __no_init esp_packet_t esp_packet;

// ������������� ������ DMA
static void esp_dma_channel_init(DMA_Channel_TypeDef *channel, uint32_t flags)
{
    // ���������������� DMA
    mcu_dma_channel_setup_pm(channel,
        // �� SPI
        SPI1->DR, 
        // � ������
        &esp_packet);
    channel->CCR |= DMA_CCR_MINC | flags;                        // Direction (flags), Memory increment (8-bit), Peripheral size 8-bit, High priority
}

void esp_init(void)
{
    // ������������ � ����� SPI
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;                                         // SPI1 clock enable
    RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;                                      // SPI1 reset
    RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;                                     // SPI1 unreset
    // ���������������� SPI
    SPI1->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR | SPI_CR1_BR_1 | SPI_CR1_BR_2;                     // SPI disable, CPHA first, CPOL low, Master, Clock /128, MSB, SSM off, 8-bit, No CRC, Full Duplex
    SPI1->CR2 = /*SPI_CR2_SSOE | */SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN;           // (Hardware SS output enable, ) DMA TX RX enable
    SPI1->SR = 0;                                                               // Clear status
    SPI1->CR1 |= SPI_CR1_SPE;                                                   // SPI1 enable

    // ���������������� DMA
    DMA1->IFCR |= DMA_IFCR_CTCIF2;                                              // Clear CTCIF
    // ����� 2 (RX)
    esp_dma_channel_init(DMA1_C2, DMA_CCR_TCIE | DMA_CCR_PL_1);                                // Transfer complete IRQ enable
    // ����� 3 (TX)
    esp_dma_channel_init(DMA1_C3, DMA_CCR_DIR | DMA_CCR_PL_0);                                 // Memory to peripheral
    // �������� ���������� ������ DMA
    nvic_irq_enable_set(DMA1_Channel2_IRQn, true);
}

uint8_t k = 0;

void esp_transaction(void)
{
    // ���������� ������
    esp_packet.rx.clear();
    esp_packet.tx.dll.command = ESP_CMD_NOP;

    k++;
    // ��������� ��������� �������
    for (uint8_t i = 0; i < ESP_APL_SIZE; i++)
        esp_packet.tx.apl.u8[i] = i;
//        esp_packet.tx.apl.u8[i] = i;
//        esp_packet.tx.apl.u8[i] = i ? i + k : 0;
    
    esp_packet.tx.finalize(ESP_SPI_CMD_RD_WR, ESP_APL_SIZE);
    
    IO_PORT_RESET(IO_ESP_CS);                                                   // Slave select
    
    SPI1->DR = esp_packet.raw[0];
    for (uint8_t i = 0; i < sizeof(esp_packet); i++)
    {
        if (i != sizeof(esp_packet) - 1)
        {
            WAITFOR(SPI1->SR & SPI_SR_TXE);
            SPI1->DR = esp_packet.raw[i + 1];
        }
        
        WAITFOR(SPI1->SR & SPI_SR_RXNE);
        esp_packet.raw[i] = SPI1->DR;
    }
    IO_PORT_SET(IO_ESP_CS);                                                     // Slave deselect    
    return;
    
    // DMA
    DMA1_C2->CNDTR = DMA1_C3->CNDTR = sizeof(esp_packet);                       // Transfer data size
    DMA1_C2->CCR |= DMA_CCR_EN;                                                 // Channel enable
    DMA1_C3->CCR |= DMA_CCR_EN;                                                 // Channel enable
}

IRQ_HANDLER
void esp_interrupt_dma(void)
{
    // DMA
    DMA1->IFCR |= DMA_IFCR_CTCIF2;                                              // Clear CTCIF
    DMA1_C2->CCR &= ~DMA_CCR_EN;                                                // Channel disable
    DMA1_C3->CCR &= ~DMA_CCR_EN;                                                // Channel disable
    // SPI
    WAITWHILE(SPI1->SR & SPI_SR_BSY);                                           // Wait for idle
    IO_PORT_SET(IO_ESP_CS);                                                     // Slave deselect
}
