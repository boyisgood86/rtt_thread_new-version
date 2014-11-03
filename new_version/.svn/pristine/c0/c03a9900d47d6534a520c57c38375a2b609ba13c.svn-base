#ifndef YM_UART_APP_H_
#define YM_UART_APP_H_

#define UART_USING

#define UART_RECV_SIZE  (12)
#define UART_SEND_SIZE   (12)

#include "Interface.h"
#include "dev_opt.h"


/* recv type */
#define YM_QUER_SHORT_ADDR_MSG  (0x01)          /* quer end device short address */
#define YM_DEVICE_OFF_LINE_MSG  (0x02)          /* zigbee end device off line  msg */
#define YM_DEVICE_ON_LINE_MSG   (0x03)           /* zigbee end device on line msg */
#define YM_DEVICE_CONTROL_MSG   (0x04)          /* user control zigbee device msg */
#define YM_DEVICE_MAC_SINGLE_MSG (0x05)     /* quer single device mac address */


typedef struct msg
{
    unsigned char value;
    char short_address[2];
    char long_address[8];
}ym_msg_t;


typedef struct uart_msg
{
    char msg_type;
    union {
      ym_msg_t quer_msg;
      ym_msg_t off_line_msg;
      ym_msg_t on_line_msg;
      ym_msg_t ctrl_msg;
    }type;
 
}ym_uart_msg_t;


typedef struct uart_device
{
    rt_device_t uart;
    struct rt_mutex mutex;
    ym_uart_msg_t rx_msg;
    
}ym_uart_t;

enum uart_error {
    uart_err = -1,
    uart_ok = 0,
};


int uart_read_from_zigbee(void);
int uart_send_cmd_to_zigbee(void);
int uart1_user_cmd_do(void);

int uart2_init(void);

int uart_send(Send_Recv_CMDA_t * zigbee_dev);
int uart_recv(char * recv_buff);
int quer_device_short_addr();
int quer_device_short_addr_single(char *buff);
char check_uart_msg_type(char *buff);
int write_to_file(char *buff);
int user_ctrl_msg(char *buff);
int quer_device_statu(char *buff);

#endif /* YM_UART_APP_H_ */
