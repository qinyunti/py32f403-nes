/**
  ******************************************************************************
  * @file    py32f403_ll_bus.h
  * @author  MCU Application Team
  * @brief   Header file of BUS LL module.

  @verbatim
                      ##### RCC Limitations #####
  ==============================================================================
    [..]
      A delay between an RCC peripheral clock enable and the effective peripheral
      enabling should be taken into account in order to manage the peripheral read/write
      from/to registers.
      (+) This delay depends on the peripheral mapping.
        (++) AHB & APB peripherals, 1 dummy read is necessary

    [..]
      Workarounds:
      (#) For AHB & APB peripherals, a dummy read to the peripheral register has been
          inserted in each LL_{BUS}_GRP{x}_EnableClock() function.

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by Puya under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PY32F403_LL_BUS_H
#define PY32F403_LL_BUS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "py32f4xx.h"

/** @addtogroup PY32F403_LL_Driver
  * @{
  */

#if defined(RCC)

/** @defgroup BUS_LL BUS
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup BUS_LL_Exported_Constants BUS Exported Constants
  * @{
  */

/** @defgroup BUS_LL_EC_AHB1_GRP1_PERIPH  AHB1 GRP1 PERIPH
  * @{
  */
#define LL_AHB1_GRP1_PERIPH_ALL            0xFFFFFFFFU
#define LL_AHB1_GRP1_PERIPH_DMA1           RCC_AHB1ENR_DMA1EN
#define LL_AHB1_GRP1_PERIPH_DMA2           RCC_AHB1ENR_DMA2EN
#define LL_AHB1_GRP1_PERIPH_SRAM           RCC_AHB1ENR_SRAMEN
#define LL_AHB1_GRP1_PERIPH_FMC            RCC_AHB1ENR_FMCEN
#define LL_AHB1_GRP1_PERIPH_CRC            RCC_AHB1ENR_CRCEN
#define LL_AHB1_GRP1_PERIPH_SDIO           RCC_AHB1ENR_SDIOEN
#define LL_AHB1_GRP1_PERIPH_ESMC           RCC_AHB1ENR_ESMCEN
/**
  * @}
  */

/** @defgroup BUS_LL_EC_AHB2_GRP1_PERIPH  AHB2 GRP1 PERIPH
  * @{
  */
#define LL_AHB2_GRP1_PERIPH_ALL            0xFFFFFFFFU
#define LL_AHB2_GRP1_PERIPH_GPIOA          RCC_AHB2ENR_IOPAEN
#define LL_AHB2_GRP1_PERIPH_GPIOB          RCC_AHB2ENR_IOPBEN
#define LL_AHB2_GRP1_PERIPH_GPIOC          RCC_AHB2ENR_IOPCEN
#define LL_AHB2_GRP1_PERIPH_GPIOD          RCC_AHB2ENR_IOPDEN
#define LL_AHB2_GRP1_PERIPH_GPIOE          RCC_AHB2ENR_IOPEEN
/**
  * @}
  */


/** @defgroup BUS_LL_EC_APB1_GRP1_PERIPH  APB1 GRP1 PERIPH
  * @{
  */
#define LL_APB1_GRP1_PERIPH_ALL            0xFFFFFFFFU
#define LL_APB1_GRP1_PERIPH_TIM2           RCC_APB1ENR_TIM2EN
#define LL_APB1_GRP1_PERIPH_TIM3           RCC_APB1ENR_TIM3EN
#define LL_APB1_GRP1_PERIPH_TIM4           RCC_APB1ENR_TIM4EN
#define LL_APB1_GRP1_PERIPH_TIM5           RCC_APB1ENR_TIM5EN
#define LL_APB1_GRP1_PERIPH_TIM6           RCC_APB1ENR_TIM6EN
#define LL_APB1_GRP1_PERIPH_TIM7           RCC_APB1ENR_TIM7EN
#define LL_APB1_GRP1_PERIPH_TIM12          RCC_APB1ENR_TIM12EN
#define LL_APB1_GRP1_PERIPH_TIM13          RCC_APB1ENR_TIM13EN
#define LL_APB1_GRP1_PERIPH_TIM14          RCC_APB1ENR_TIM14EN
#define LL_APB1_GRP1_PERIPH_WWDG           RCC_APB1ENR_WWDGEN
#define LL_APB1_GRP1_PERIPH_SPI2           RCC_APB1ENR_SPI2EN
#define LL_APB1_GRP1_PERIPH_SPI3           RCC_APB1ENR_SPI3EN
#define LL_APB1_GRP1_PERIPH_USART2         RCC_APB1ENR_USART2EN
#define LL_APB1_GRP1_PERIPH_USART3         RCC_APB1ENR_USART3EN
#define LL_APB1_GRP1_PERIPH_USART4         RCC_APB1ENR_USART4EN
#define LL_APB1_GRP1_PERIPH_USART5         RCC_APB1ENR_USART5EN
#define LL_APB1_GRP1_PERIPH_I2C1           RCC_APB1ENR_I2C1EN
#define LL_APB1_GRP1_PERIPH_I2C2           RCC_APB1ENR_I2C2EN
#define LL_APB1_GRP1_PERIPH_USB            RCC_APB1ENR_USBEN
#define LL_APB1_GRP1_PERIPH_CAN            RCC_APB1ENR_CANEN
#define LL_APB1_GRP1_PERIPH_BKP            RCC_APB1ENR_BKPEN
#define LL_APB1_GRP1_PERIPH_PWR            RCC_APB1ENR_PWREN
#define LL_APB1_GRP1_PERIPH_CTC            RCC_APB1ENR_CTCEN
/**
  * @}
  */

/** @defgroup BUS_LL_EC_APB2_GRP1_PERIPH  APB2 GRP1 PERIPH
  * @{
  */
#define LL_APB2_GRP1_PERIPH_ALL            0xFFFFFFFFU
#define LL_APB2_GRP1_PERIPH_SYSCFG         RCC_APB2ENR_SYSCFGEN
#define LL_APB2_GRP1_PERIPH_ADC1           RCC_APB2ENR_ADC1EN
#define LL_APB2_GRP1_PERIPH_ADC2           RCC_APB2ENR_ADC2EN
#define LL_APB2_GRP1_PERIPH_TIM1           RCC_APB2ENR_TIM1EN
#define LL_APB2_GRP1_PERIPH_SPI1           RCC_APB2ENR_SPI1EN
#define LL_APB2_GRP1_PERIPH_TIM8           RCC_APB2ENR_TIM8EN
#define LL_APB2_GRP1_PERIPH_USART1         RCC_APB2ENR_USART1EN
#define LL_APB2_GRP1_PERIPH_ADC3           RCC_APB2ENR_ADC3EN
#define LL_APB2_GRP1_PERIPH_TIM9           RCC_APB2ENR_TIM9EN
#define LL_APB2_GRP1_PERIPH_TIM10          RCC_APB2ENR_TIM10EN
#define LL_APB2_GRP1_PERIPH_TIM11          RCC_APB2ENR_TIM11EN
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @defgroup BUS_LL_Exported_Functions BUS Exported Functions
  * @{
  */

/** @defgroup BUS_LL_EF_AHB1_GRP1 AHB1 GRP1
  * @{
  */

/**
  * @brief  Enable AHB1 peripherals clock.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_SRAM
  *         @arg @ref LL_AHB1_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB1_GRP1_PERIPH_CRC
  *         @arg @ref LL_AHB1_GRP1_PERIPH_SDIO
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ESMC
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval None
  */
__STATIC_INLINE void LL_AHB1_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->AHB1ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->AHB1ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if AHB1 peripheral clock is enabled or not
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_SRAM
  *         @arg @ref LL_AHB1_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB1_GRP1_PERIPH_CRC
  *         @arg @ref LL_AHB1_GRP1_PERIPH_SDIO
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ESMC
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval State of Periphs (1 or 0).
  */
__STATIC_INLINE uint32_t LL_AHB1_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHB1ENR, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable AHB1 peripherals clock.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_SRAM
  *         @arg @ref LL_AHB1_GRP1_PERIPH_FMC
  *         @arg @ref LL_AHB1_GRP1_PERIPH_CRC
  *         @arg @ref LL_AHB1_GRP1_PERIPH_SDIO
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ESMC
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval None
  */
__STATIC_INLINE void LL_AHB1_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB1ENR, Periphs);
}

/**
  * @brief  Force AHB1 peripherals reset.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ALL
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_CRC
  *         @arg @ref LL_AHB1_GRP1_PERIPH_SDIO
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ESMC
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval None
  */
__STATIC_INLINE void LL_AHB1_GRP1_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->AHB1RSTR, Periphs);
}

/**
  * @brief  Release AHB1 peripherals reset.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ALL
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA1
  *         @arg @ref LL_AHB1_GRP1_PERIPH_DMA2
  *         @arg @ref LL_AHB1_GRP1_PERIPH_CRC
  *         @arg @ref LL_AHB1_GRP1_PERIPH_SDIO
  *         @arg @ref LL_AHB1_GRP1_PERIPH_ESMC
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval None
  */
__STATIC_INLINE void LL_AHB1_GRP1_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB1RSTR, Periphs);
}
/**
  * @}
  */

/** @defgroup BUS_LL_EF_AHB2_GRP1 AHB2 GRP1
  * @{
  */

/**
  * @brief  Enable AHB2 peripherals clock.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOE
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval None
  */
__STATIC_INLINE void LL_AHB2_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->AHB2ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->AHB2ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if AHB2 peripheral clock is enabled or not
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOE
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval State of Periphs (1 or 0).
  */
__STATIC_INLINE uint32_t LL_AHB2_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHB2ENR, Periphs) == Periphs) ? 1UL : 0UL);
}

/**
  * @brief  Disable AHB2 peripherals clock.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOE
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval None
  */
__STATIC_INLINE void LL_AHB2_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB2ENR, Periphs);
}

/**
  * @brief  Force AHB2 peripherals reset.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_ALL  
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOE
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval None
  */
__STATIC_INLINE void LL_AHB2_GRP1_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->AHB2RSTR, Periphs);
}

/**
  * @brief  Release AHB2 peripherals reset.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB2_GRP1_PERIPH_ALL  
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOA
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOB
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOC
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOD
  *         @arg @ref LL_AHB2_GRP1_PERIPH_GPIOE
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval None
  */
__STATIC_INLINE void LL_AHB2_GRP1_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB2RSTR, Periphs);
}
/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB1_GRP1 APB1 GRP1
  * @{
  */

/**
  * @brief  Enable APB1 GRP1 peripherals clock.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12 
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13 
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14 
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG  
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2  
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3  
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1  
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2  
  *         @arg @ref LL_APB1_GRP1_PERIPH_USB   
  *         @arg @ref LL_APB1_GRP1_PERIPH_CAN   
  *         @arg @ref LL_APB1_GRP1_PERIPH_BKP   
  *         @arg @ref LL_APB1_GRP1_PERIPH_PWR   
  *         @arg @ref LL_APB1_GRP1_PERIPH_CTC   
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval None
  */
__STATIC_INLINE void LL_APB1_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->APB1ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->APB1ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if APB1 GRP1 peripheral clock is enabled or not
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12 
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13 
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14 
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG  
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2  
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3  
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1  
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2  
  *         @arg @ref LL_APB1_GRP1_PERIPH_USB   
  *         @arg @ref LL_APB1_GRP1_PERIPH_CAN   
  *         @arg @ref LL_APB1_GRP1_PERIPH_BKP   
  *         @arg @ref LL_APB1_GRP1_PERIPH_PWR   
  *         @arg @ref LL_APB1_GRP1_PERIPH_CTC   
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval State of Periphs (1 or 0).
  */
__STATIC_INLINE uint32_t LL_APB1_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB1ENR, Periphs) == (Periphs)) ? 1UL : 0UL);
}

/**
  * @brief  Disable APB1 GRP1 peripherals clock.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12 
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13 
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14 
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG  
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2  
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3  
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1  
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2  
  *         @arg @ref LL_APB1_GRP1_PERIPH_USB   
  *         @arg @ref LL_APB1_GRP1_PERIPH_CAN   
  *         @arg @ref LL_APB1_GRP1_PERIPH_BKP   
  *         @arg @ref LL_APB1_GRP1_PERIPH_PWR   
  *         @arg @ref LL_APB1_GRP1_PERIPH_CTC   
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval None
  */
__STATIC_INLINE void LL_APB1_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB1ENR, Periphs);
}

/**
  * @brief  Force APB1 GRP1 peripherals reset.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_ALL   
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12 
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13 
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14 
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG  
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2  
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3  
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1  
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2  
  *         @arg @ref LL_APB1_GRP1_PERIPH_USB   
  *         @arg @ref LL_APB1_GRP1_PERIPH_CAN   
  *         @arg @ref LL_APB1_GRP1_PERIPH_PWR   
  *         @arg @ref LL_APB1_GRP1_PERIPH_CTC   
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval None
  */
__STATIC_INLINE void LL_APB1_GRP1_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->APB1RSTR, Periphs);
}

/**
  * @brief  Release APB1 GRP1 peripherals reset.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB1_GRP1_PERIPH_ALL   
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM2  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM3  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM4  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM5  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM6  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM7  
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM12 
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM13 
  *         @arg @ref LL_APB1_GRP1_PERIPH_TIM14 
  *         @arg @ref LL_APB1_GRP1_PERIPH_WWDG  
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI2  
  *         @arg @ref LL_APB1_GRP1_PERIPH_SPI3  
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART2
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART3
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART4
  *         @arg @ref LL_APB1_GRP1_PERIPH_USART5
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C1  
  *         @arg @ref LL_APB1_GRP1_PERIPH_I2C2  
  *         @arg @ref LL_APB1_GRP1_PERIPH_USB   
  *         @arg @ref LL_APB1_GRP1_PERIPH_CAN   
  *         @arg @ref LL_APB1_GRP1_PERIPH_PWR   
  *         @arg @ref LL_APB1_GRP1_PERIPH_CTC   
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval None
  */
__STATIC_INLINE void LL_APB1_GRP1_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB1RSTR, Periphs);
}
/**
  * @}
  */

/** @defgroup BUS_LL_EF_APB2_GRP1 APB2 GRP1
  * @{
  */

/**
  * @brief  Enable APB2 GRP1 peripherals clock.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB2_GRP1_PERIPH_ADC1  
  *         @arg @ref LL_APB2_GRP1_PERIPH_ADC2  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1  
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8  
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_ADC3  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM9  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM10 
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM11
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval None
  */
__STATIC_INLINE void LL_APB2_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->APB2ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->APB2ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Check if APB2 GRP1 peripheral clock is enabled or not
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB2_GRP1_PERIPH_ADC1  
  *         @arg @ref LL_APB2_GRP1_PERIPH_ADC2  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1  
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8  
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_ADC3  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM9  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM10 
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM11
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval State of Periphs (1 or 0).
  */
__STATIC_INLINE uint32_t LL_APB2_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APB2ENR, Periphs) == (Periphs)) ? 1UL : 0UL);
}

/**
  * @brief  Disable APB2 GRP1 peripherals clock.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB2_GRP1_PERIPH_ADC1  
  *         @arg @ref LL_APB2_GRP1_PERIPH_ADC2  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1  
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8  
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_ADC3  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM9  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM10 
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM11
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval None
  */
__STATIC_INLINE void LL_APB2_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB2ENR, Periphs);
}

/**
  * @brief  Force APB2 GRP1 peripherals reset.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_ALL   
  *         @arg @ref LL_APB2_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB2_GRP1_PERIPH_ADC1  
  *         @arg @ref LL_APB2_GRP1_PERIPH_ADC2  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1  
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8  
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_ADC3  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM9  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM10 
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM11
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @retval None
  */
__STATIC_INLINE void LL_APB2_GRP1_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->APB2RSTR, Periphs);
}

/**
  * @brief  Release APB2 GRP1 peripherals reset.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_APB2_GRP1_PERIPH_ALL   
  *         @arg @ref LL_APB2_GRP1_PERIPH_SYSCFG
  *         @arg @ref LL_APB2_GRP1_PERIPH_ADC1  
  *         @arg @ref LL_APB2_GRP1_PERIPH_ADC2  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM1  
  *         @arg @ref LL_APB2_GRP1_PERIPH_SPI1  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM8  
  *         @arg @ref LL_APB2_GRP1_PERIPH_USART1
  *         @arg @ref LL_APB2_GRP1_PERIPH_ADC3  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM9  
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM10 
  *         @arg @ref LL_APB2_GRP1_PERIPH_TIM11
  * @note   Depending on devices and packages, some peripherals may not be available.
  *         Refer to device datasheet for peripherals availability.
  * @note (*) peripheral not available on all devices
  * @retval None
  */
__STATIC_INLINE void LL_APB2_GRP1_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APB2RSTR, Periphs);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* RCC */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* PY32F403_LL_BUS_H */

/************************ (C) COPYRIGHT Puya *****END OF FILE****/
