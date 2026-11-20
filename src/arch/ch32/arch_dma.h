/*!
 * @file arch_dma.h
 * Blocking SPI1 DMA pixel output for the community CH32V407/467 Arduino core.
 */
#ifndef _ADAFRUIT_SPITFT_ARCH_CH32_DMA_H_
#define _ADAFRUIT_SPITFT_ARCH_CH32_DMA_H_

#if defined(ARDUINO_ARCH_CH32) && defined(CH32V4x7) &&                         \
    defined(CH32_CORE_VERSION)
#define ADAGFX_CH32_DMA ///< CH32V407/467 community-core SPI1 fast path.

/*!
    @brief Write pixels or a solid fill using otherwise unconfigured SPI1 DMA
           channels. Return false before writing if ordinary SPI must be used.
           Transactions, chip select and data/command stay with SPITFT.
    @param spi Arduino SPI object whose transaction is already active.
    @param colors Source pixels, unchanged; unused for a solid fill.
    @param len Number of pixels.
    @param bigEndian True if source bytes are already in display order.
    @param fill True for a repeated color rather than source pixels.
    @param color RGB565 solid-fill color.
    @return True if handled, including an aborted transfer. A partial DMA
            transfer must never be replayed at the now-advanced display cursor.
*/
static bool ch32DmaWrite(SPIClass *spi, const uint16_t *colors, uint32_t len,
                         bool bigEndian, bool fill, uint16_t color) {
  if (spi->getHandle()->Instance != SPI1 || len < 16)
    return false;
  // The library cannot allocate channels through this core. Only borrow the
  // SPI1 RX/TX channels when they have no configuration or pending flags at
  // all, not merely when their EN bit is clear. Leave other users untouched.
  RCC_AHBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);
  if (DMA1_Channel2->CFGR || DMA1_Channel2->CNTR || DMA1_Channel2->PADDR ||
      DMA1_Channel2->MADDR || DMA1_Channel3->CFGR || DMA1_Channel3->CNTR ||
      DMA1_Channel3->PADDR || DMA1_Channel3->MADDR ||
      DMA_GetFlagStatus(DMA1_FLAG_GL2 | DMA1_FLAG_GL3) ||
      (SPI1->CTLR2 & (SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx)) ||
      (SPI1->CTLR1 & (SPI_CTLR1_DFF | SPI_CTLR1_LSBFIRST)))
    return false;

  // Staging also makes flash/PSRAM source buffers safe and preserves endian
  // order without modifying caller memory. DMA always reads internal SRAM.
  uint8_t bytes[512];
  volatile uint8_t received;
  while (len) {
    uint32_t count = min(len, (uint32_t)256);
    for (uint32_t i = 0; i < count; i++) {
      if (fill) {
        bytes[2 * i] = color >> 8;
        bytes[2 * i + 1] = color;
      } else if (bigEndian) {
        memcpy(bytes + 2 * i, colors + i, 2);
      } else {
        bytes[2 * i] = colors[i] >> 8;
        bytes[2 * i + 1] = colors[i];
      }
    }
    DMA_InitTypeDef config = {};
    config.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DATAR;
    config.DMA_MemoryBaseAddr = (uint32_t)&received;
    config.DMA_DIR = DMA_DIR_PeripheralSRC;
    config.DMA_BufferSize = count * 2;
    config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    config.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_Init(DMA1_Channel2, &config);
    config.DMA_MemoryBaseAddr = (uint32_t)bytes;
    config.DMA_MemoryInc = DMA_MemoryInc_Enable;
    config.DMA_DIR = DMA_DIR_PeripheralDST;
    config.DMA_Priority = DMA_Priority_High;
    DMA_Init(DMA1_Channel3, &config);
    DMA_ClearFlag(DMA1_FLAG_GL2 | DMA1_FLAG_GL3);
    asm volatile("fence rw,rw" ::: "memory");
    DMA_Cmd(DMA1_Channel2, ENABLE);
    DMA_Cmd(DMA1_Channel3, ENABLE);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);
    uint32_t started = micros();
    bool complete = true;
    // RX drains every transmitted byte; no stale RXNE/overrun is left for the
    // next register read. The SPI busy check keeps CS asserted to the last bit.
    while (!DMA_GetFlagStatus(DMA1_FLAG_TC2) ||
           !DMA_GetFlagStatus(DMA1_FLAG_TC3) ||
           SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY)) {
      if (DMA_GetFlagStatus(DMA1_FLAG_TE2 | DMA1_FLAG_TE3) ||
          micros() - started >= 20000) {
        complete = false;
        break;
      }
    }
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);
    DMA_DeInit(DMA1_Channel2);
    DMA_DeInit(DMA1_Channel3);
    asm volatile("fence rw,rw" ::: "memory");
    if (!complete) {
      // Stop the shifter before the caller can deassert CS; leave the original
      // SPI mode/clock intact for a subsequent, explicit redraw.
      SPI_Cmd(SPI1, DISABLE);
      (void)SPI1->DATAR;
      (void)SPI1->STATR;
      SPI_Cmd(SPI1, ENABLE);
      return true;
    }
    if (!fill)
      colors += count;
    len -= count;
  }
  return true;
}
#endif
#endif
