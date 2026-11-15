/*!
 * @file arch_dma.h
 *
 * ESP32-S2 and ESP32-S3 hardware SPI DMA for Adafruit_SPITFT.
 * Only included by Adafruit_SPITFT.cpp. Expands to nothing on other chips.
 */

#ifndef _ADAFRUIT_SPITFT_ARCH_ESP32_DMA_H_
#define _ADAFRUIT_SPITFT_ARCH_ESP32_DMA_H_

#if defined(ADAGFX_ESP32_DMA)

#include <esp_heap_caps.h>
#include <esp_memory_utils.h>
#include <hal/spi_ll.h>

#define ESP32_DMA_HOST SPI2_HOST     ///< The Arduino SPI object (FSPI)
#define ESP32_DMA_BUFFER_PIXELS 1024 ///< Pixels in each of the two buffers
#define ESP32_DMA_MAX_PIXELS 16384   ///< Longest transfer, the ESP32-S3 limit

// The Arduino SPI driver and the ESP-IDF driver share one SPI peripheral.
// These are the registers the ESP-IDF driver changes that the Arduino
// driver needs. They are saved before DMA starts and put back after.
static volatile uint32_t *const arduinoSpiRegisters[] = {
    &GPSPI2.ctrl.val,      &GPSPI2.clock.val,        &GPSPI2.user.val,
    &GPSPI2.user1.val,     &GPSPI2.user2.val,        &GPSPI2.misc.val,
    &GPSPI2.dma_conf.val,
#if defined(CONFIG_IDF_TARGET_ESP32S2)
    &GPSPI2.ctrl1.val,     &GPSPI2.ctrl2.val,        &GPSPI2.mosi_dlen.val,
    &GPSPI2.miso_dlen.val, &GPSPI2.dma_out_link.val, &GPSPI2.dma_in_link.val,
#else
    &GPSPI2.ms_dlen.val,
#endif
};
#define ARDUINO_SPI_REGISTER_COUNT                                             \
  (sizeof(arduinoSpiRegisters) / sizeof(arduinoSpiRegisters[0]))
static uint32_t arduinoSpiValues[ARDUINO_SPI_REGISTER_COUNT];

static void saveArduinoSpi(void) {
  for (size_t i = 0; i < ARDUINO_SPI_REGISTER_COUNT; i++) {
    arduinoSpiValues[i] = *arduinoSpiRegisters[i];
  }
}

static void restoreArduinoSpi(void) {
  for (size_t i = 0; i < ARDUINO_SPI_REGISTER_COUNT; i++) {
    *arduinoSpiRegisters[i] = arduinoSpiValues[i];
  }
#if !defined(CONFIG_IDF_TARGET_ESP32S2)
  GPSPI2.cmd.update = 1; // Latch the restored settings
  while (GPSPI2.cmd.update)
    ;
#endif
}

/*!
    @brief  Set up DMA on the Arduino SPI bus. If anything fails, dmaDevice
            stays NULL and the Arduino SPI driver is used as before.
    @param  freq     SPI clock in Hz.
    @param  spiMode  SPI mode, 0 to 3.
*/
inline void Adafruit_SPITFT::dmaBegin(uint32_t freq, uint8_t spiMode) {
  if ((hwspi._spi != &SPI) || dmaDevice) {
    return;
  }
  pixelBuf[0] =
      (uint16_t *)heap_caps_malloc(ESP32_DMA_BUFFER_PIXELS * 4, MALLOC_CAP_DMA);
  if (!pixelBuf[0]) {
    return;
  }
  pixelBuf[1] = pixelBuf[0] + ESP32_DMA_BUFFER_PIXELS;
  spi_bus_config_t bus = {};
  bus.mosi_io_num = bus.miso_io_num = bus.sclk_io_num = -1; // Arduino's pins
  bus.quadwp_io_num = bus.quadhd_io_num = -1;
  bus.max_transfer_sz = ESP32_DMA_MAX_PIXELS * 2;
  spi_device_interface_config_t device = {};
  device.clock_speed_hz = freq;
  device.mode = spiMode;
  device.spics_io_num = -1; // Chip select stays with this library
  device.queue_size = 1;
  saveArduinoSpi(); // Bus init resets the peripheral
  if (spi_bus_initialize(ESP32_DMA_HOST, &bus, SPI_DMA_CH_AUTO) == ESP_OK) {
    if (spi_bus_add_device(ESP32_DMA_HOST, &device, &dmaDevice) != ESP_OK) {
      dmaDevice = NULL;
      spi_bus_free(ESP32_DMA_HOST);
    }
  }
  restoreArduinoSpi();
  if (!dmaDevice) {
    heap_caps_free(pixelBuf[0]);
    pixelBuf[0] = pixelBuf[1] = NULL;
  }
}

/*!
    @brief  Start sending big-endian pixels from DMA-capable memory. If a
            transfer is still running, wait for it first.
    @param  buffer  Pixels to send. Must stay unchanged until sent.
    @param  pixels  Number of pixels, at most ESP32_DMA_MAX_PIXELS.
*/
inline void Adafruit_SPITFT::dmaStart(const uint16_t *buffer, uint32_t pixels) {
  if (dmaActive) {
    spi_device_polling_end(dmaDevice, portMAX_DELAY);
  } else {
    saveArduinoSpi();
  }
  memset(&dmaTransaction, 0, sizeof(dmaTransaction));
  dmaTransaction.length = pixels * 16; // In bits
  dmaTransaction.tx_buffer = buffer;
  spi_device_polling_start(dmaDevice, &dmaTransaction, portMAX_DELAY);
  dmaActive = true;
}

/*!
    @brief  Wait for the last DMA transfer, then give the bus back to the
            Arduino SPI driver.
*/
inline void Adafruit_SPITFT::dmaFinish(void) {
  if (dmaActive) {
    spi_device_polling_end(dmaDevice, portMAX_DELAY);
    restoreArduinoSpi();
    dmaActive = false;
  }
}

/*!
    @brief  writePixels() over DMA. Big-endian pixels already in DMA-capable
            memory are sent in place. Other pixels are copied into two
            buffers, and one fills while the other one is sent.
    @param  colors     Pixels to send.
    @param  len        Number of pixels.
    @param  block      If false, return once the last transfer has started.
    @param  bigEndian  True if the pixels are already big-endian.
*/
inline void Adafruit_SPITFT::dmaWritePixels(uint16_t *colors, uint32_t len,
                                            bool block, bool bigEndian) {
  bool sendInPlace = bigEndian && esp_ptr_dma_capable(colors);
  while (len) {
    uint32_t count;
    if (sendInPlace) {
      count = (len < ESP32_DMA_MAX_PIXELS) ? len : ESP32_DMA_MAX_PIXELS;
      dmaStart(colors, count);
    } else {
      count = (len < ESP32_DMA_BUFFER_PIXELS) ? len : ESP32_DMA_BUFFER_PIXELS;
      uint16_t *buffer = pixelBuf[pixelBufIndex];
      if (bigEndian) {
        memcpy(buffer, colors, count * 2);
      } else {
        for (uint32_t i = 0; i < count; i++) {
          buffer[i] = __builtin_bswap16(colors[i]);
        }
      }
      dmaStart(buffer, count);
      pixelBufIndex ^= 1;
    }
    colors += count;
    len -= count;
  }
  if (block) {
    dmaFinish();
  }
}

/*!
    @brief  writeColor() over DMA. Fills one buffer with the color, then
            sends it as many times as needed.
    @param  color  16-bit 5-6-5 color.
    @param  len    Number of pixels.
*/
inline void Adafruit_SPITFT::dmaWriteColor(uint16_t color, uint32_t len) {
  uint32_t count =
      (len < ESP32_DMA_BUFFER_PIXELS) ? len : ESP32_DMA_BUFFER_PIXELS;
  uint16_t *buffer = pixelBuf[pixelBufIndex];
  uint16_t swapped = __builtin_bswap16(color);
  for (uint32_t i = 0; i < count; i++) {
    buffer[i] = swapped;
  }
  pixelBufIndex ^= 1;
  while (len) {
    uint32_t send = (len < count) ? len : count;
    dmaStart(buffer, send);
    len -= send;
  }
  dmaFinish();
}

#endif // end ADAGFX_ESP32_DMA
#endif // end _ADAFRUIT_SPITFT_ARCH_ESP32_DMA_H_
