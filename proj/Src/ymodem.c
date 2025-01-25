#include <string.h>
#include "ymodem.h"

/* 符号定义 */
#define SOH   0x01
#define STX   0x02
#define EOT   0x04
#define ACK   0x06
#define NAK   0x15
#define CAN   0x18
#define CTRLZ 0x1A

/**
 * crc计算参考https://www.iar.com/knowledge/support/technical-notes/general/checksum-generation/
*/
static const uint16_t t[] = 
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
};

static uint16_t crc_nibble_rom(uint16_t sum, uint8_t* p, uint32_t len)
{
    while (len--) 
    {
        // hi nibble
        sum = t[(sum>>12)^(*p >> 4)]^(sum<<4);
        // lo nibble
        sum = t[(sum>>12)^(*p++ & 0xF)]^(sum<<4);
    }
    return sum;
}

/**
 * \fn ymodem_check
 * \paran[in] crc 0使用checksum 1使用crc
 * \paran[in] buf 待计算数据
 * \paran[in] sz  待计算数据长度
 * \retval 0 校验正确
 * \retval 其他值校验错误
*/
static int ymodem_check(uint16_t crc, uint8_t *buf, uint32_t sz)
{
    if (crc != 0) 
    {
        uint16_t crc = crc_nibble_rom(0, buf, sz);
        uint16_t tcrc = ((uint16_t)(buf[sz]) << 8) + (uint16_t)(buf[sz + 1]);
        if (crc == tcrc) 
        {
            return 0;
        }
    } 
    else 
    {
        uint8_t cks = 0;
        for (uint32_t i = 0; i < sz; i++) 
        {
            cks += buf[i];
        }
        if (cks == buf[sz]) 
        {
            return 0;
        }
    }
    return -1;
}

#if YMODEM_RX_ENABLE
static ymodem_rx_cfg_st* s_rx_cfg_pst = 0; /* 接口指针,ymodem_rx_init初始化 */

static unsigned int ymodem_atoi(uint8_t* str, uint32_t len)
{
  unsigned int i = 0U;
  while(len--)
  {
    if ((*str >= '0') && (*str <= '9')) 
    {
        i = i * 10U + (unsigned int)(*str++ - '0');
    }
    else
    {
        break;
    }
  }
  return i;
}

static uint32_t ymodem_get_len(uint8_t* str,uint32_t len)
{
    uint8_t* p = str;
    if(*p== 0)
    {
        /* 无效设置最大值 */
        return 0xFFFFFFFF;
    }

    while(len--)
    {
        /* 略去前面的文件名 */
        if(*p++ == 0)
        {
            break;
        }
    }

    if(len >= 2)
    {
        return ymodem_atoi(p,len);
    }
    else
    {
        /* 后面整数至少也要2个字符,一个数字一个NULL,否则无效设置最大值 */
        return 0xFFFFFFFF;
    }
}

/**
 * \fn ymodem_rx
 * 接收处理, 在此之前必须要先调用xymodem_rx_init初始化.
 * 返回XYMODEM_RX_ERR_NEED_CONTINUE则需要继续重复调用,直到返回其他值.
 * \return \ref xymodem_rx_err_e
*/
ymodem_rx_err_e ymodem_rx(void)
{
    ymodem_rx_err_e res = YMODEM_RX_ERR_NEED_CONTINUE;
    uint8_t tmp = NAK;
    uint32_t t;
    uint8_t* buf;
    uint16_t len;
    uint32_t getlen;
#if YMODEM_CHECK_PARAM
    if(s_rx_cfg_pst == (ymodem_rx_cfg_st*)0)
    {
        return YMODEM_RX_ERR_UNINIT;
    }
    if((s_rx_cfg_pst->io_read == 0) || \
        (s_rx_cfg_pst->io_write == 0) || \
        (s_rx_cfg_pst->mem_write == 0) || \
        (s_rx_cfg_pst->mem_start == 0) || \
        (s_rx_cfg_pst->mem_done == 0) || \
        (s_rx_cfg_pst->buffer == 0) || \
        (s_rx_cfg_pst->getms == 0))
    {
        return YMODEM_RX_ERR_UNINIT;
    }
#endif
    t = s_rx_cfg_pst->getms();
    switch(s_rx_cfg_pst->state.state_e)
    {
        case YMODEM_STATE_RX_IDLE:
            s_rx_cfg_pst->state.getlen_u32 = 0;
            s_rx_cfg_pst->state.pnum_u8 = 0;
            s_rx_cfg_pst->xferlen = 0;
            s_rx_cfg_pst->totallen = 0;
            /* YMODEM使用'C' 启动, 使用CRC16
            */
            tmp = 'C';
            s_rx_cfg_pst->io_write(&tmp,1);
            
            /**
             * 等待接收P0的头
            */
            s_rx_cfg_pst->state.state_e = YMODEM_STATE_RX_WAIT_P0_HEAD;
            s_rx_cfg_pst->state.ms_u32 = t;  /* 状态切换更新时间 */
        break;
        case YMODEM_STATE_RX_WAIT_P0_HEAD:
            /* 这里初始化,接收多个文件时,新的文件从0开始,已经接收长度也重新开始计算 */
            s_rx_cfg_pst->state.pnum_u8 = 0;   
            s_rx_cfg_pst->xferlen = 0;    

            buf = s_rx_cfg_pst->buffer;
            buf[0] = 0; /* 必须初始化,否则加入未收到数据, 后面超时判断要判断该值，则判断的可能是上一帧的值，导入不进入超时判断 */
            if(0 != s_rx_cfg_pst->io_read(buf,1))
            {
                /* 读到头,切换状态到读数据 */
                if((buf[0] == SOH) || (buf[0] == STX))
                {
                    s_rx_cfg_pst->plen = (buf[0] == SOH) ? (uint16_t)128 : (uint16_t)1024; /* 根据头决定接收包长 */
                    s_rx_cfg_pst->state.ms_u32 = t;
                    s_rx_cfg_pst->state.state_e = YMODEM_STATE_RX_WAIT_P0_DATA;
    
                    s_rx_cfg_pst->state.getlen_u32 = 1;
                    s_rx_cfg_pst->is_start = 1;
                }
                else
                {
                    /* 其他数据忽略 */
                }
            }
            else
            {
                /* 没有读到数据 */
            }

            if((buf[0] != SOH) && (buf[0] != STX))
            {
                /* 未收到头,进行超时判断 */
                if((t - s_rx_cfg_pst->state.ms_u32) >= s_rx_cfg_pst->ack_timeout)
                {
                    s_rx_cfg_pst->state.ms_u32 = t;  /* 更新时间以便下一次继续判断超时 */
                    /* 如果是传输第一个文件时,即刚开始超时未收到响应,继续重复发送启动字符,如果超过了设置的超时时间则退出
                     * 只有第一次is_start=0时,才等待start_timeout超时.
                    */
                    if(s_rx_cfg_pst->is_start == 0)
                    {
                        if(s_rx_cfg_pst->start_timeout <= 1)
                        {
                            /* 超时start_timeout次后退出 */
                            res = YMODEM_RX_ERR_WAIT_P0_HEAD_TO;
                        }
                        else
                        {
                            /* 回到IDLE重新发送启动 */
                            s_rx_cfg_pst->start_timeout--;
                            s_rx_cfg_pst->state.state_e = YMODEM_STATE_RX_IDLE;
                        }
                    }
                    else
                    {
                        /* 非最开始启动时,超时即退出 */
                        res = YMODEM_RX_ERR_WAIT_P0_HEAD_TO;
                    }
                }
                else
                {
                    /* 未超时继续等待*/
                }
            }
        break;
        case YMODEM_STATE_RX_WAIT_P0_DATA:
            buf = s_rx_cfg_pst->buffer;
            len = s_rx_cfg_pst->plen + 3 + 2;  /* 总共需要接收的长度 */
            getlen = s_rx_cfg_pst->io_read(buf+s_rx_cfg_pst->state.getlen_u32, len - s_rx_cfg_pst->state.getlen_u32);  /* 尝试接收剩余需要接收的长度 */
            s_rx_cfg_pst->state.getlen_u32 += getlen;
            if(s_rx_cfg_pst->state.getlen_u32 >= len)
            {
                /* 接收完,准备判断合法性 */
                if(((uint8_t)0 == buf[1]) && ((buf[1] + buf[2]) == (uint8_t)255))
                {
                    /* 校验正确 */
                    if(0 == ymodem_check(1, buf+3, s_rx_cfg_pst->plen))
                    {
                        s_rx_cfg_pst->state.pnum_u8 = buf[1];
                        s_rx_cfg_pst->state.ms_u32 = t;
                        
                        if(buf[3] == 0)
                        {
                            /* 传输结束 回ACK */
                            tmp = ACK;
                            s_rx_cfg_pst->io_write(&tmp,1);
                            res = YMODEM_RX_ERR_NONE;
                        }
                        else
                        {
                            /* 文件开始 解析文件大小 */
                            s_rx_cfg_pst->totallen = ymodem_get_len(&(buf[3]),s_rx_cfg_pst->plen);

                            /* 调用启动接口 */
                            uint8_t* fname = &(buf[3]);
                            if(0 == s_rx_cfg_pst->mem_start(s_rx_cfg_pst->addr,&fname,0))
                            {
                                /* 回ACK */
                                tmp = ACK;
                                s_rx_cfg_pst->io_write(&tmp,1);

                                /* 回'C' */
                                tmp = 'C';  
                                s_rx_cfg_pst->io_write(&tmp,1);
                                s_rx_cfg_pst->state.state_e = YMODEM_STATE_RX_WAIT_DATA_HEAD;
                                s_rx_cfg_pst->state.ms_u32 = t;
                                
                            }
                            else
                            {
                                /* 不成功回取消 */
                                for(int i=0;i<10;i++)
                                {
                                    tmp = CAN;
                                    s_rx_cfg_pst->io_write(&tmp,1);
                                }
                                res = YMODEM_RX_ERR_START;
                            }  
                        }
                    }
                    else
                    {
                        /* NAK让对方重发 @todo需要flush输入? */
                        tmp = NAK;
                        s_rx_cfg_pst->io_write(&tmp,1);
                        res = YMODEM_RX_ERR_WAIT_P0_DATA_TO;
                    }
                }
            }
            else
            {
            }

            /* 超时处理 */
            if((t - s_rx_cfg_pst->state.ms_u32) >= s_rx_cfg_pst->packet_timeout)
            {
                /* 超时,NAK让对方重发 */
                tmp = NAK;
                s_rx_cfg_pst->io_write(&tmp,1);
                res = YMODEM_RX_ERR_WAIT_P0_DATA_TO;
            }
            else
            {
                /* 未超时继续等待*/
            }
        break;
        case YMODEM_STATE_RX_WAIT_DATA_HEAD:
            buf = s_rx_cfg_pst->buffer;
            buf[0] = 0; /* 必须初始化,否则加入未收到数据, 后面超时判断要判断该值，则判断的可能是上一帧的值，导入不进入超时判断 */
            if(0 != s_rx_cfg_pst->io_read(buf,1))
            {
                /* 读到头,切换状态到读数据 */
                if((buf[0] == SOH) || (buf[0] == STX))
                {
                    s_rx_cfg_pst->plen = (buf[0] == SOH) ? (uint16_t)128 : (uint16_t)1024; /* 根据头决定接收包长 */
                    s_rx_cfg_pst->state.ms_u32 = t;
                    s_rx_cfg_pst->state.state_e = YMODEM_STATE_RX_WAIT_DATA_DATA;
    
                    s_rx_cfg_pst->state.getlen_u32 = 1;
                }
                else if(buf[0] == EOT)
                {
                    /* NAK等待下一个EOT */
                    tmp = NAK;
                    s_rx_cfg_pst->io_write(&tmp,1);
                    s_rx_cfg_pst->state.state_e = YMODEM_STATE_RX_WAIT_EOT;
                }
                else if(buf[0] == CAN)
                {
                    /* 提前取消 */
                    res = YMODEM_RX_ERR_CAN;
                    s_rx_cfg_pst->mem_done();
                }
                else
                {
                    /* 其他数据忽略 */
                }
            }
            else
            {
                /* 没有读到数据 */
            }

            if((buf[0] != SOH) && (buf[0] != STX))
            {
                /* 未收到头,进行超时判断 */
                if((t - s_rx_cfg_pst->state.ms_u32) >= s_rx_cfg_pst->ack_timeout)
                {
                    s_rx_cfg_pst->state.ms_u32 = t;  /* 更新时间以便下一次继续判断超时 */
                    res = YMODEM_RX_ERR_WAIT_DATA_HEAD_TO;
                }
                else
                {
                    /* 未超时继续等待*/
                }
            }
        break;
        case YMODEM_STATE_RX_WAIT_DATA_DATA:
            buf = s_rx_cfg_pst->buffer;
            len = s_rx_cfg_pst->plen + 3 + 2;  /* 总共需要接收的长度 */
            getlen = s_rx_cfg_pst->io_read(buf+s_rx_cfg_pst->state.getlen_u32, len - s_rx_cfg_pst->state.getlen_u32);  /* 尝试接收剩余需要接收的长度 */
            s_rx_cfg_pst->state.getlen_u32 += getlen;
            if(s_rx_cfg_pst->state.getlen_u32 >= len)
            {
                /* 接收完,准备判断合法性 */
                if(((uint8_t)((s_rx_cfg_pst->state.pnum_u8+(uint8_t)1)) == buf[1]) && ((buf[1] + buf[2]) == (uint8_t)255))
                {
                    /* 校验正确 */
                    if(0 == ymodem_check(1, buf+3, s_rx_cfg_pst->plen))
                    {
                        s_rx_cfg_pst->state.pnum_u8 = buf[1];
                        s_rx_cfg_pst->state.ms_u32 = t;
                        /* 调用写接口 */
                        if(s_rx_cfg_pst->xferlen >= s_rx_cfg_pst->totallen)
                        {
                            /* 上一次已经接收了足够的数据,本次无需再写入, 取消 */
                            for(int i= 0; i<10; i++)
                            {
                                tmp = CAN;
                                s_rx_cfg_pst->io_write(&tmp,1);
                            }
                            res = YMODEM_RX_ERR_NONE;
                        }
                        else
                        {
                            /* 本次可能是最后一包,有可能有填充，如果超过长度,则只接收指定长度 */
                            if((s_rx_cfg_pst->xferlen + s_rx_cfg_pst->plen) > s_rx_cfg_pst->totallen)
                            {
                                s_rx_cfg_pst->mem_write(s_rx_cfg_pst->addr,&(buf[3]),s_rx_cfg_pst->totallen - s_rx_cfg_pst->xferlen);
                                s_rx_cfg_pst->xferlen += s_rx_cfg_pst->totallen - s_rx_cfg_pst->xferlen;
                            }
                            else
                            {
                                s_rx_cfg_pst->mem_write(s_rx_cfg_pst->addr,&(buf[3]),s_rx_cfg_pst->plen);
                                s_rx_cfg_pst->xferlen += s_rx_cfg_pst->plen;
                            }
                            s_rx_cfg_pst->state.state_e = YMODEM_STATE_RX_WAIT_DATA_HEAD;
                            tmp = ACK;
                            s_rx_cfg_pst->io_write(&tmp,1);
                            s_rx_cfg_pst->state.ms_u32 = t;
                        }
                    }
                }
            }
            else
            {
            }

            /* 未接收完判断超时 */
            if((t - s_rx_cfg_pst->state.ms_u32) >= s_rx_cfg_pst->packet_timeout)
            {
                /* 超时退出 */
                for(int i= 0; i<10; i++)
                {
                    tmp = CAN;
                    s_rx_cfg_pst->io_write(&tmp,1);
                }
                res = YMODEM_RX_ERR_WAIT_DATA_DATA_TO;
            }
            else
            {
                /* 未超时继续等待*/
            }
        break;
        case YMODEM_STATE_RX_WAIT_EOT:
            buf = s_rx_cfg_pst->buffer;
            buf[0] = 0; /* 必须初始化,否则加入未收到数据, 后面超时判断要判断该值，则判断的可能是上一帧的值，导入不进入超时判断 */
            if(0 != s_rx_cfg_pst->io_read(buf,1))
            {
                /* 读到头,切换状态到读数据 */
                if((buf[0] == EOT))
                {
                    tmp = ACK;
                    s_rx_cfg_pst->io_write(&tmp,1);

                    tmp = 'C';
                    s_rx_cfg_pst->io_write(&tmp,1);

                    s_rx_cfg_pst->state.state_e = YMODEM_STATE_RX_WAIT_P0_HEAD;
                    s_rx_cfg_pst->state.ms_u32 = t; /* 切换状态更新时间 */

                    s_rx_cfg_pst->mem_done();
                }
                else
                {
                    /* 其他数据忽略 */
                }
            }
            else
            {
                /* 没有读到数据 */
            }

            if((buf[0] != EOT))
            {
                /* 未收到头,进行超时判断 */
                if((t - s_rx_cfg_pst->state.ms_u32) >= s_rx_cfg_pst->ack_timeout)
                {
                    /* 超时退出 */
                    res = YMODEM_RX_ERR_WAIT_EOT_TO;
                }
                else
                {
                    /* 未超时继续等待*/
                }
            }
        break;
        default:
            return YMODEM_RX_ERR_UNINIT;
        break;
    }
    return res;
}

/**
 * \fn ymodem_rx_init
 * 接收初始化
 * \param[in] cfg \ref ymodem_rx_cfg_st 配置信息
 * \retval 0 成功
 * \retval 其他值 失败
*/
int ymodem_rx_init(ymodem_rx_cfg_st* cfg)
{
#if YMODEM_CHECK_PARAM
    if(cfg == (ymodem_rx_cfg_st*)0)
    {
        return -1;
    }
#endif
    s_rx_cfg_pst = cfg;
    s_rx_cfg_pst->state.state_e = YMODEM_STATE_RX_IDLE;
    return 0;
}

#endif

#if YMODEM_TX_ENABLE
static ymodem_tx_cfg_st* s_tx_cfg_pst = 0; /* 接口指针,ymodem_tx_init初始化 */

/**
 * \fn ymodem_tx
 * 发送处理, 在此之前必须要先调用xymodem_tx_init初始化.
 * 返回XYMODEM_TX_ERR_NEED_CONTINUE则需要继续重复调用,直到返回其他值.
 * \return \ref xymodem_tx_err_e
*/
ymodem_tx_err_e ymodem_tx(void)
{
    ymodem_tx_err_e res = YMODEM_TX_ERR_NEED_CONTINUE;
    uint8_t tmp = 0;
    uint32_t t;
    uint8_t* buf;
#if YMODEM_CHECK_PARAM
    if(s_tx_cfg_pst == (ymodem_tx_cfg_st*)0)
    {
        return YMODEM_TX_ERR_UNINIT;
    }
    if((s_tx_cfg_pst->io_read == 0) || \
        (s_tx_cfg_pst->io_write == 0) || \
        (s_tx_cfg_pst->mem_read == 0) || \
        (s_tx_cfg_pst->mem_start == 0) || \
        (s_tx_cfg_pst->mem_done == 0) || \
        (s_tx_cfg_pst->buffer == 0) || \
        (s_tx_cfg_pst->getms == 0))
    {
        return YMODEM_TX_ERR_UNINIT;
    }
#endif
    t = s_tx_cfg_pst->getms();
    switch(s_tx_cfg_pst->state.state_e)
    {
        case YMODEM_STATE_TX_IDLE:

        break;
        case YMODEM_STATE_TX_WAIT_P0_START:
            /* 这里初始化,发多个文件时,新的文件包从ID0开始 */
            s_tx_cfg_pst->state.pnum_u8 = 0;      
            if(0 != s_tx_cfg_pst->io_read(&tmp,1))
            {
                /* 读到头,切换状态到读数据 */
                if(tmp == 'C')
                {
                    uint8_t* str;
                    uint32_t strlen;
                    s_tx_cfg_pst->state.ms_u32 = t;
                    s_tx_cfg_pst->state.state_e = YMODEM_STATE_TX_WAIT_P0_ACK;
                    s_tx_cfg_pst->is_start = 1;
                    buf = s_tx_cfg_pst->buffer;
                    uint16_t crc;
                    if(0 == s_tx_cfg_pst->mem_start(0,&str,&strlen))
                    {
                        /* 填充P0包 */
                        memset(buf,0,s_tx_cfg_pst->plen+5);
                        buf[0] = (s_tx_cfg_pst->plen == 128) ? SOH : STX;
                        buf[1] = 0;
                        buf[2] = 0xFF;
                        memcpy(&(buf[3]),str,strlen);

                        crc = crc_nibble_rom(0,&(buf[3]),s_tx_cfg_pst->plen);
                        buf[s_tx_cfg_pst->plen+4] = crc&0xFF;
                        buf[s_tx_cfg_pst->plen+3] = (crc>>8)&0xFF;

                        s_tx_cfg_pst->io_write(buf,s_tx_cfg_pst->plen+5);

                        s_tx_cfg_pst->state.state_e = YMODEM_STATE_TX_WAIT_P0_ACK;
                        s_tx_cfg_pst->state.ms_u32 = t;
                    }
                    else
                    {
                        /* 发送结束P0包 */
                        memset(buf,0,128+5);
                        buf[0] = SOH;
                        buf[1] = 0;
                        buf[2] = 0xFF;
                        buf[3] = 0;
                        buf[4] = 0x30;
                        buf[5] = 0x20;
                        buf[6] = 0x30;
                        buf[7] = 0x20;
                        buf[8] = 0x30;
                        crc = crc_nibble_rom(0,&(buf[3]),128);
                        buf[128+4] = crc&0xFF;
                        buf[128+3] = (crc>>8)&0xFF;
                        s_tx_cfg_pst->io_write(buf,s_tx_cfg_pst->plen+5);
                        s_tx_cfg_pst->state.state_e = YMODEM_STATE_TX_WAIT_P0_DONE_ACK;
                        s_tx_cfg_pst->state.ms_u32 = t;
                    }
                }
                else if(tmp == CAN)
                {
                    /* 取消 */
                    res = YMODEM_TX_ERR_CAN;
                }
                else
                {

                }
            }
            else
            {
                /* 没有读到数据 */
            }

            if(tmp != 'C')
            {
                /* 未收到头,进行超时判断 */
                if((t - s_tx_cfg_pst->state.ms_u32) >= s_tx_cfg_pst->ack_timeout)
                {
                    s_tx_cfg_pst->state.ms_u32 = t;  /* 更新时间以便下一次继续判断超时 */
                    /* 如果是传输第一个文件时,即刚开始超时未收到启动'C',继续等待,如果超过了设置的超时时间则退出
                     * 只有第一次is_start=0时,才等待start_timeout超时.
                    */
                    if(s_tx_cfg_pst->is_start == 0)
                    {
                        if(s_tx_cfg_pst->start_timeout <= 1)
                        {
                            /* 超时start_timeout次后退出 */
                            res = YMODEM_TX_EER_WAIT_P0_START_TO;
                        }
                        else
                        {
                            /* 递减超时次数,继续等待 */
                            s_tx_cfg_pst->start_timeout--;
                        }
                    }
                    else
                    {
                        /* 非最开始启动时,超时即退出 */
                        res = YMODEM_TX_EER_WAIT_P0_START_TO;
                    }
                }
                else
                {
                    /* 未超时继续等待*/
                }
            }
        break;
        case YMODEM_STATE_TX_WAIT_P0_ACK:
            if(0 != s_tx_cfg_pst->io_read(&tmp,1))
            {
                /* 读到头,切换状态到读数据 */
                if(tmp == ACK)
                {
                    s_tx_cfg_pst->state.state_e = YMODEM_STATE_TX_WAIT_DATA_START;
                    s_tx_cfg_pst->state.ms_u32 = t;
                }
            }
            if(tmp != ACK)
            {
                /* 未收到ACK */
                if((t - s_tx_cfg_pst->state.ms_u32) >= s_tx_cfg_pst->ack_timeout)
                {
                    res = YMODEM_TX_EER_WAIT_P0_ACK_TO;
                }
                else
                {
                    /* 未超时继续等待*/
                }
            }
            
        break;
        case YMODEM_STATE_TX_WAIT_DATA_START:
            buf = s_tx_cfg_pst->buffer;
            if(0 != s_tx_cfg_pst->io_read(&tmp,1))
            {
                /* 读到头,切换状态到发送数据 */
                if(tmp == 'C')
                {
                    uint32_t rdlen;
                    uint16_t crc;
                    /* 发送一包数据 */
                    s_tx_cfg_pst->state.pnum_u8++;
                    buf[0] = (s_tx_cfg_pst->plen == 128) ? SOH : STX;
                    buf[1] = s_tx_cfg_pst->state.pnum_u8;
                    buf[2] = ~buf[1];
                    rdlen = s_tx_cfg_pst->mem_read(s_tx_cfg_pst->addr,&(buf[3]),s_tx_cfg_pst->plen);

                    if(rdlen > 0)
                    {
                        s_tx_cfg_pst->state.state_e = YMODEM_STATE_TX_WAIT_DAT_ACK;
                        if(rdlen<s_tx_cfg_pst->plen)
                        {
                            memset(&(buf[3+rdlen]),CTRLZ,s_tx_cfg_pst->plen-rdlen);
                        }
                        crc = crc_nibble_rom(0,&(buf[3]),s_tx_cfg_pst->plen);
                        buf[s_tx_cfg_pst->plen+4] = crc&0xFF;
                        buf[s_tx_cfg_pst->plen+3] = (crc>>8)&0xFF;
                        s_tx_cfg_pst->io_write(buf,s_tx_cfg_pst->plen+5);
                    }
                    else
                    {
                        /* 未读到数据可能是读失败胡总和读完，发EOT */
                        s_tx_cfg_pst->state.state_e = YMODEM_STATE_TX_WAIT_EOT_NACK;
                        tmp = EOT;
                        s_tx_cfg_pst->io_write(&tmp,1);
                    }
                    s_tx_cfg_pst->state.ms_u32 = t;
                }
            }
            if(tmp != ACK)
            {
                /* 未收到'C' */
                if((t - s_tx_cfg_pst->state.ms_u32) >= s_tx_cfg_pst->ack_timeout)
                {
                    res = YMODEM_TX_EER_WAIT_DATA_START_TO;
                }
                else
                {
                    /* 未超时继续等待*/
                }
            }
        break;
        case YMODEM_STATE_TX_WAIT_DAT_ACK:
            buf = s_tx_cfg_pst->buffer;
            if(0 != s_tx_cfg_pst->io_read(&tmp,1))
            {
                /* 读到ACK,继续发送数据 */
                if(tmp == ACK)
                {
                    uint32_t rdlen;
                    uint16_t crc;
                    /* 发送一包数据 */
                    s_tx_cfg_pst->state.pnum_u8++;
                    buf[0] = (s_tx_cfg_pst->plen == 128) ? SOH : STX;
                    buf[1] = s_tx_cfg_pst->state.pnum_u8;
                    buf[2] = ~buf[1];
                    rdlen = s_tx_cfg_pst->mem_read(s_tx_cfg_pst->addr,&(buf[3]),s_tx_cfg_pst->plen);

                    if(rdlen > 0)
                    {
                        if(rdlen<s_tx_cfg_pst->plen)
                        {
                            memset(&(buf[3+rdlen]),CTRLZ,s_tx_cfg_pst->plen-rdlen);
                        }
                        crc = crc_nibble_rom(0,&(buf[3]),s_tx_cfg_pst->plen);
                        buf[s_tx_cfg_pst->plen+4] = crc&0xFF;
                        buf[s_tx_cfg_pst->plen+3] = (crc>>8)&0xFF;
                        s_tx_cfg_pst->io_write(buf,s_tx_cfg_pst->plen+5);
                    }
                    else
                    {
                        /* 未读到数据可能是读失败或者读完，发EOT */
                        s_tx_cfg_pst->state.state_e = YMODEM_STATE_TX_WAIT_EOT_NACK;
                        tmp = EOT;
                        s_tx_cfg_pst->io_write(&tmp,1);
                    }
                    s_tx_cfg_pst->state.ms_u32 = t;
                }
            }
            if(tmp != ACK)
            {
                /* 未收到ACK */
                if((t - s_tx_cfg_pst->state.ms_u32) >= s_tx_cfg_pst->ack_timeout)
                {
                    res = YMODEM_TX_EER_WAIT_DATA_ACK_TO;
                }
                else
                {
                    /* 未超时继续等待*/
                }
            }
        break;
        case YMODEM_STATE_TX_WAIT_EOT_NACK:
            if(0 != s_tx_cfg_pst->io_read(&tmp,1))
            {
                if(tmp == NAK)
                {
                    /* 读到NAK 发送EOT继续等待ACK  */
                    s_tx_cfg_pst->state.state_e = YMODEM_STATE_TX_WAIT_EOT_ACK;
                    s_tx_cfg_pst->state.ms_u32 = t;
                    tmp = EOT;
                    s_tx_cfg_pst->io_write(&tmp,1);
                }
            }
            if(tmp != NAK)
            {
                /* 未收到NAK */
                if((t - s_tx_cfg_pst->state.ms_u32) >= s_tx_cfg_pst->ack_timeout)
                {
                    /* 超时返回错误 */
                    res = YMODEM_TX_EER_WAIT_EOT_NACK_TO;
                }
                else
                {
                    /* 未超时继续等待*/
                }
            }
        break;
        case YMODEM_STATE_TX_WAIT_EOT_ACK:
            if(0 != s_tx_cfg_pst->io_read(&tmp,1))
            {
                if(tmp == ACK)
                {
                    /* 读到ACK 等待下一个启动'C'  */
                    s_tx_cfg_pst->state.state_e = YMODEM_STATE_TX_WAIT_P0_START;
                    s_tx_cfg_pst->state.ms_u32 = t;
                }
            }
            if(tmp != ACK)
            {
                /* 未收到ACK */
                if((t - s_tx_cfg_pst->state.ms_u32) >= s_tx_cfg_pst->ack_timeout)
                {
                    /* 超时返回错误 */
                    res = YMODEM_TX_EER_WAIT_EOT_ACK_TO;
                }
                else
                {
                    /* 未超时继续等待*/
                }
            }
        break;
        case YMODEM_STATE_TX_WAIT_P0_DONE_ACK:
            if(0 != s_tx_cfg_pst->io_read(&tmp,1))
            {
                if(tmp == ACK)
                {
                    /* 读到ACK 正常结束 */
                    s_tx_cfg_pst->state.state_e = YMODEM_STATE_TX_WAIT_DATA_START;
                    res = YMODEM_TX_ERR_NONE;
                }
            }
            if(tmp != ACK)
            {
                /* 未收到ACK */
                if((t - s_tx_cfg_pst->state.ms_u32) >= s_tx_cfg_pst->ack_timeout)
                {
                    /* 超时返回错误 */
                    res = YMODEM_TX_EER_WAIT_P0_DONE_ACK_TO;
                }
                else
                {
                    /* 未超时继续等待*/
                }
            }
        break;
        default:
            return YMODEM_TX_ERR_UNINIT;
        break;
    }
    return res;
}

/**
 * \fn ymodem_tx_init
 * 发送初始化
 * \param[in] cfg \ref ymodem_tx_cfg_st 配置信息
 * \retval 0 成功
 * \retval 其他值 失败
*/
int ymodem_tx_init(ymodem_tx_cfg_st* cfg)
{
#if YMODEM_CHECK_PARAM
    if(cfg == (ymodem_tx_cfg_st*)0)
    {
        return -1;
    }
#endif
    s_tx_cfg_pst = cfg;
    s_tx_cfg_pst->state.state_e = YMODEM_STATE_TX_WAIT_P0_START;
    return 0;
}

#endif
