#ifndef YMODEM_H
#define YMODEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define YMODEM_CHECK_PARAM 1

#define YMODEM_RX_ENABLE 1  /**< 配置为1使能RX相关实现, 配置为0不使能 */
#define YMODEM_TX_ENABLE 1  /**< 配置为1使能TX相关实现, 配置为0不使能 */

typedef uint32_t (*ymodem_io_read_pf)(uint8_t* buffer, uint32_t len);              /**< 通讯接收接口,返回实际读到的长度返回0表示未读到 */
typedef uint32_t (*ymodem_io_write_pf)(uint8_t* buffer, uint32_t len);             /**< 通讯发送接口                                */
typedef int (*ymodem_mem_start_pf)(uint32_t addr, uint8_t** name, uint32_t* len);  /**< 启动文件传输回调接口,返回非0表示失败          */
typedef int (*ymodem_mem_done_pf)(void);                                           /**< 结束文件传输回调接口                         */
typedef uint32_t (*ymodem_getms_pf)(void);                                         /**< 获取mS时间戳                                */

#if YMODEM_RX_ENABLE
typedef uint32_t (*ymodem_mem_write_pf)(uint32_t addr, uint8_t* buffer, uint32_t len);  /**< 写存储接口,接收才需要     */

/**
 * \enum ymodem_rx_state_e
 * 接收状态枚举
*/
typedef enum
{  
    YMODEM_STATE_RX_IDLE = 0,                 /**< 初始状态               */

    YMODEM_STATE_RX_WAIT_P0_HEAD = 1,         /**< 等待接收P0包的头        */
    YMODEM_STATE_RX_WAIT_P0_DATA = 2,         /**< 等待接收P0包的数据      */

    YMODEM_STATE_RX_WAIT_DATA_HEAD = 3,       /**< 等待接收数据包的头      */
    YMODEM_STATE_RX_WAIT_DATA_DATA = 4,       /**< 等待接收数据包的数据    */

    YMODEM_STATE_RX_WAIT_EOT = 5,             /**< 等待接收第二个EOT 数据传完发送端发两个EOT,接收端对第一个NAK,对第二个ACK  */
} ymodem_rx_state_e;

/**
 * \enum ymodem_rx_err_e
 * 接收错误枚举
*/
typedef enum
{  
    YMODEM_RX_ERR_NEED_CONTINUE = 0,        /**< 需要继续处理            */
    YMODEM_RX_ERR_NONE,                     /**< 成功结束无错误          */
    YMODEM_RX_ERR_UNINIT,                   /**< 未初始化                */
    YMODEM_RX_ERR_WAIT_P0_HEAD_TO,          /**< 等待接收P0的包头超时     */
    YMODEM_RX_ERR_WAIT_P0_DATA_TO,          /**< 等待接收P0包的数据超时   */
    YMODEM_RX_ERR_WAIT_DATA_HEAD_TO,        /**< 等待接收数据包的头超时   */
    YMODEM_RX_ERR_WAIT_DATA_DATA_TO,        /**< 等待接收数据包的数据超时  */
    YMODEM_RX_ERR_WAIT_EOT_TO,              /**< 等待ID第二个EOT超时     */  

    YMODEM_RX_ERR_CAN,                      /**< 发送方取消                  */
    YMODEM_RX_ERR_START,                    /**< 启动文件传输回调接口返回失败  */
} ymodem_rx_err_e;

/**
 * \struct ymodem_rx_state_st
 * 接收状态机结构体
*/
typedef struct
{  
    ymodem_rx_state_e  state_e;    /**< 状态机的状态           */  
    uint32_t getlen_u32;           /**< 本包接收到的数据个数    */
    uint8_t  pnum_u8;              /**< 上一包的包ID           */  
    uint32_t ms_u32;               /**< 上一个状态mS时间戳      */   
} ymodem_rx_state_st;

/**
 * \struct ymodem_rx_cfg_st
 * 接收参数配置结构体
 * 
 * 设计思想是模块本身除非必要,尽量不使用全局(静态)变量,这样模块不使用时不占用对应的RAM资源.
 * 需要的资源全部由调用者提供,即本结构体提供.
 * 
*/
typedef struct
{ 
    /* 以下需要用实例提供 */
    ymodem_io_read_pf   io_read;             /**< 通讯接收接口 */
    ymodem_io_read_pf   io_write;            /**< 通讯发送接口 */
    ymodem_mem_start_pf mem_start;           /**< 启动回调接口 */
    ymodem_mem_done_pf  mem_done;            /**< 停止回调接口 */
    ymodem_mem_write_pf mem_write;           /**< 写存储接口   */
    ymodem_getms_pf    getms;                /**< 获取mS时间戳接口 */
    uint32_t start_timeout;                  /**< 等待启动超时次数,单位每次超时为ack_timeout  */
    uint32_t packet_timeout;                 /**< 等待数据包超时时间mS */
    uint32_t ack_timeout;                    /**< 等待响应(头)超时时间mS*/
    uint8_t* buffer;                         /**< 包缓冲区,用户提供,1024字节包需要1024+5     */
    uint32_t addr;                           /**< 存储地址                                 */
    uint32_t totallen;                       /**< 传输长度                                 */
    uint32_t xferlen;                        /**< 已经传输长度                             */

    /* 以下调用rx_init接口初始化 */
    ymodem_rx_state_st state;                /**< 状态机                                   */

    /* 以下内部使用无需初始化 */
    uint16_t plen;                           /**< 包长:128或1024                           */
    uint8_t is_start;                        /**< 是否已经启动标志,第一次启动时等待start_timeout次超时时间  */
} ymodem_rx_cfg_st;

/**
 * \fn ymodem_rx
 * 接收处理, 在此之前必须要先调用ymodem_rx_init初始化.
 * 返回YMODEM_RX_ERR_NEED_CONTINUE则需要继续重复调用,直到返回其他值.
 * \return \ref ymodem_rx_err_e
*/
ymodem_rx_err_e ymodem_rx(void);

/**
 * \fn ymodem_rx_init
 * 接收初始化
 * \param[in] cfg \ref ymodem_rx_cfg_st 配置信息
 * \retval 0 成功
 * \retval 其他值 失败
*/
int ymodem_rx_init(ymodem_rx_cfg_st* cfg);

#endif

#if YMODEM_TX_ENABLE

/**
 * \enum ymodem_tx_state_e
 * 发送状态枚举
*/
typedef enum
{  
    YMODEM_STATE_TX_IDLE = 0,                 /**< 初始状态               */

    YMODEM_STATE_TX_WAIT_P0_START = 1,        /**< 等待发送P0之前的'C'     */
    YMODEM_STATE_TX_WAIT_P0_ACK = 2,          /**< 等待发送P0之后的ACK     */
    YMODEM_STATE_TX_WAIT_DATA_START = 3,      /**< 等待发送DATA之前的'C'   */
    YMODEM_STATE_TX_WAIT_DAT_ACK = 4,         /**< 等待发送数据之后的ACK   */
    YMODEM_STATE_TX_WAIT_EOT_NACK = 5,        /**< 等待发送EOT之后的NACK   */
    YMODEM_STATE_TX_WAIT_EOT_ACK = 6,         /**< 等待发送EOT之后的ACK    */

    YMODEM_STATE_TX_WAIT_P0_DONE_ACK = 7,     /**< 等待发送结束P0之后的ACK  */

} ymodem_tx_state_e;

/**
 * \enum ymodem_tx_err_e
 * 发送错误枚举
*/
typedef enum
{  
    YMODEM_TX_ERR_NEED_CONTINUE = 0,        /**< 需要继续处理            */
    YMODEM_TX_ERR_NONE,                     /**< 成功结束无错误          */
    YMODEM_TX_ERR_UNINIT,                   /**< 未初始化                */

    YMODEM_TX_EER_WAIT_P0_START_TO,
    YMODEM_TX_EER_WAIT_P0_ACK_TO,
    YMODEM_TX_EER_WAIT_DATA_START_TO,
    YMODEM_TX_EER_WAIT_DATA_ACK_TO,
    YMODEM_TX_EER_WAIT_EOT_NACK_TO,
    YMODEM_TX_EER_WAIT_EOT_ACK_TO,
    YMODEM_TX_EER_WAIT_P0_DONE_ACK_TO,

    YMODEM_TX_ERR_CAN,                      /**< 接收方取消                  */
    YMODEM_TX_ERR_START,                    /**< 启动文件传输回调接口返回失败  */
} ymodem_tx_err_e;

/**
 * \struct ymodem_tx_state_st
 * 发送状态机结构体
*/
typedef struct
{  
    ymodem_tx_state_e  state_e;    /**< 状态机的状态           */  
    uint8_t  pnum_u8;              /**< 上一包的包ID           */  
    uint32_t ms_u32;               /**< 上一个状态mS时间戳      */   
} ymodem_tx_state_st;

/**
 * \struct ymodem_tx_cfg_st
 * 发送参数配置结构体
 * 
 * 设计思想是模块本身除非必要,尽量不使用全局(静态)变量,这样模块不使用时不占用对应的RAM资源.
 * 需要的资源全部由调用者提供,即本结构体提供.
 * 
*/
typedef struct
{ 
    /* 以下需要用实例提供 */
    ymodem_io_read_pf   io_read;             /**< 通讯接收接口 */
    ymodem_io_read_pf   io_write;            /**< 通讯发送接口 */
    ymodem_mem_start_pf mem_start;           /**< 启动回调接口 */
    ymodem_mem_done_pf  mem_done;            /**< 停止回调接口 */
    ymodem_mem_write_pf mem_read;            /**< 读存储接口   */
    ymodem_getms_pf    getms;                /**< 获取mS时间戳接口 */
    uint32_t start_timeout;                  /**< 等待启动超时次数,单位每次超时为ack_timeout  */
    uint32_t packet_timeout;                 /**< 等待数据包超时时间mS */
    uint32_t ack_timeout;                    /**< 等待响应(头)超时时间mS*/
    uint8_t* buffer;                         /**< 包缓冲区,用户提供,1024字节包需要1024+5     */
    uint32_t addr;                           /**< 存储地址                                 */
    uint16_t plen;                           /**< 包长:128或1024                           */
    
    /* 以下调用rx_init接口初始化 */
    ymodem_tx_state_st state;                /**< 状态机                                   */

    /* 以下内部使用无需初始化 */
    uint8_t is_start;                        /**< 是否已经启动标志,第一次启动时等待start_timeout次超时时间  */
} ymodem_tx_cfg_st;


typedef uint32_t (*ymodem_mem_read_pf)(uint32_t addr, uint8_t* buffer, uint32_t len);  /**< 读存储接口,发送才需要     */

/**
 * \fn ymodem_tx
 * 接收处理, 在此之前必须要先调用ymodem_tx_init初始化.
 * 返回YMODEM_TX_ERR_NEED_CONTINUE则需要继续重复调用,直到返回其他值.
 * \return \ref ymodem_tx_err_e
*/
ymodem_tx_err_e ymodem_tx(void);

/**
 * \fn ymodem_tx_init
 * 发送初始化
 * \param[in] cfg \ref ymodem_tx_cfg_st 配置信息
 * \retval 0 成功
 * \retval 其他值 失败
*/
int ymodem_tx_init(ymodem_tx_cfg_st* cfg);

#endif


#ifdef __cplusplus
}
#endif

#endif 
