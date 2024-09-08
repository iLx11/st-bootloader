/********************************************************************************
 * @author: iLx1
 * @date: 2024-09-08 10:25:59
 * @filepath: \bootloader\HARDWARE\bootloader\bootloader.c
 * @description: bootloader
 * @email: colorful_ilx1@163.com
 * @copyright: Copyright (c) iLx1, All Rights Reserved.
 ********************************************************************************/

#include <stdio.h>

#include "bootlloader.h"
#include "stm32f1xx_hal.h"

typedef void (*pFunction)(void);

static uint32_t read_startup_mode(void);
static void read_on_chip_flash(uint32_t address, uint32_t* data_buff,
                               uint32_t buffer_size);
static void erase_on_chip_flash(uint32_t address, uint32_t num);
static void move_app_data(uint32_t main_addr, uint32_t slave_addr,
                          uint32_t byte_size);
static void jump_execute_app(uint32_t app_addr);

/********************************************************************************
 * @brief: bootloader 开始
 * @return {*}
 ********************************************************************************/
void bootloader_begin(void) {
  printf("\r\n");
  printf("***********************************\r\n");
  printf("*        bootloader in            *\r\n");
  printf("***********************************\r\n");

  // 读取启动模式
  switch (read_startup_mode()) {
    case STARTUP_NORMAL_MODE: {
      printf("normal mode\r\n");
    } break;
    case STARTUP_UPDATEL_MODE: {
      printf("update mode\r\n");
      // 复制移动应用
      printf("begin to move ---------------");
      move_app_data(APP_MAIN_ADDRESS, APP_SLAVE_ADDRESS, APPLICATION_SIZE);
      printf("success moved ---------------");

    } break;
    case STARTUP_RESET_MODE: {
      printf("reset mode\r\n");
    } break;
    default: {
      printf("get mode error\r\n -> %lX", read_startup_mode());
      return;
    } break;
  }
  // 跳转到 main 应用
  jump_execute_app(APP_MAIN_ADDRESS);
}

/********************************************************************************
 * @brief: 获取启动模式
 * @return {*}
 ********************************************************************************/
static uint32_t read_startup_mode(void) {
  uint32_t mode = 0;
  read_on_chip_flash((APP_SLAVE_ADDRESS + APPLICATION_SIZE - 4), &mode, 1);
  return mode;
}

/********************************************************************************
 * @brief: 读取片上 flash 数据
 * @param {uint32_t} address
 * @param {uint32_t} data_buff
 * @param {uint32_t} buffer_size
 * @return {*}
 ********************************************************************************/
static void read_on_chip_flash(uint32_t address, uint32_t* data_buff,
                               uint32_t buffer_size) {
  for (uint32_t i = 0; i < buffer_size; i++) {
    data_buff[i] = *(__IO uint32_t*)(address + (4 * i));
  }
}

/********************************************************************************
 * @brief: 写入片内 flash
 * @return {*}
 ********************************************************************************/
static void write_on_chip_flash(uint32_t address, uint32_t* data_buff,
                                uint32_t buffer_size) {
  // 解锁 flash
  HAL_FLASH_Unlock();
  for (uint32_t i = 0; i < buffer_size; i++) {
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (address + (4 * i)),
                      data_buff[i]);
  }
  // 锁住 flash
  HAL_FLASH_Lock();
}

/********************************************************************************
 * @brief: 按页擦除 flash
 * @param {uint32_t} address
 * @param {uint32_t} num_page
 * @return {*}
 ********************************************************************************/
static void erase_on_chip_flash(uint32_t address, uint32_t num_page) {
  // 解锁 flash
  HAL_FLASH_Unlock();
  // 配置擦除类型和页号
  FLASH_EraseInitTypeDef EraseInit;
  // 按页擦除
  EraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
  // 地址
  EraseInit.PageAddress = address;
  // 擦除页数
  EraseInit.NbPages = num_page;

  /*设置PageError，调用擦除函数*/
  uint32_t PageError = 0;
  HAL_FLASHEx_Erase(&EraseInit, &PageError);
  // 锁住 flash
  HAL_FLASH_Lock();
}

/********************************************************************************
 * @brief: 移动应用数据
 * @return {*}
 ********************************************************************************/
static void move_app_data(uint32_t main_addr, uint32_t slave_addr,
                          uint32_t byte_size) {
  // 擦除 main 应用
  erase_on_chip_flash(APP_MAIN_ADDRESS, APPLICATION_SIZE);
  // 移动应用数据
  uint32_t temp[256];
  for (uint32_t i = 0; i < (byte_size / 1024); i++) {
    read_on_chip_flash((slave_addr + (i * 1024)), temp, sizeof(temp));
    write_on_chip_flash((main_addr + (i * 1024)), temp, sizeof(temp));
  }
  // 擦除 slave 应用
  erase_on_chip_flash(slave_addr, (byte_size / PAGE_SIZE));
}

/********************************************************************************
 * @brief: 跳转执行应用
 * @param {uint32_t} app_addr
 * @return {*}
 ********************************************************************************/
static void jump_execute_app(uint32_t app_addr) {
  // 关闭所有中断
  __disable_irq();
  // 系统控制块向量表基址寄存器，存储中断向量表的地址
  SCB->VTOR = app_addr;
  // 跳转到 main 应用
  pFunction jump_to_application;
  jump_to_application = (pFunction) * (__IO uint32_t*)(app_addr + 4);
  // 获取栈指针设置 MSP
  __set_MSP(*((__IO uint32_t*)app_addr));
  // 启用中断
  __enable_irq();
  // 执行跳转
  jump_to_application();
}