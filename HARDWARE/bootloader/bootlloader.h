/********************************************************************************
 * @author: iLx1
 * @date: 2024-09-08 10:26:08
 * @filepath: \bootloader\HARDWARE\bootloader\bootlloader.h
 * @description: bootloader
 * @email: colorful_ilx1@163.com
 * @copyright: Copyright (c) iLx1, All Rights Reserved.
 ********************************************************************************/

#ifndef BOOTLOADER_H
#define BOOTLOADER_H

/********************************************************************************
 * @brief: 空间分配
 ********************************************************************************/
// flash 页大小
#define PAGE_SIZE FLASH_PAGE_SIZE
// 20k
#define BOOTLOADER_SIZE ((uint32_t)0x5000U)
// 应用程序大小 128k
#define APPLICATION_SIZE ((uint32_t)0x20000U)
// main 应用程序首地址
#define APP_MAIN_ADDRESS ((uint32_t)0x5000U)
// slave 应用程序首地址
#define APP_SLAVE_ADDRESS ((uint32_t)0x25000U)

/********************************************************************************
 * @brief: 启动步骤
 ********************************************************************************/
// 正常启动
#define STARTUP_NORMAL_MODE ((uint32_t)0xFFFFFFFF)
// 固件升级
#define STARTUP_UPDATEL_MODE ((uint32_t)0xAAAAAAAA)
// 重置
#define STARTUP_RESET_MODE ((uint32_t)0xBBBBBBBB)
#endif
