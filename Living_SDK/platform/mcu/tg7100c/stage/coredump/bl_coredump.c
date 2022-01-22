#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <FreeRTOS.h>
//#include <task.h>
#include <aos/kernel.h>
#include <k_api.h>
//#include <cli.h>
#include <hal/soc/soc.h>
#include <hal/soc/uart.h>
#include <utils_base64.h>
#include <utils_crc.h>
#include <utils_hex.h>
#include <bl_coredump.h>
#include <utils_base64.h>

#define REVERSE(a) (((a)&0xff) << 24 | ((a)&0xff00) << 8 | ((a)&0xff0000) >> 8 | ((a)&0xff000000) >> 24)
#define COREDUM_CMD_BUF_LEN (128)

#define COREDUMP_VERSION "0.0.1"

#define COREDUMP_BLOCK_START_STR "\r\n------ DATA BEGIN "
#define COREDUMP_BLOCK_END_STR "\r\n------ END "
#define COREDUMP_BLOCK_CLOSE_STR " ------\r\n"

#define COREDUMP_START_STR "\r\n-+-+-+- BFLB COREDUMP v" COREDUMP_VERSION " +-+-+-+\r\n"

#define BASE64_LINE_WRAP  100

/**
 * ------ DATA BEGIN addr@len@desc ------
 * contents ....
 * ------ END crc ------
 */

/* For stack check */
extern uintptr_t _sp_main, _sp_base;
extern uint8_t _ld_ram_size1, _ld_ram_addr1;
extern uint8_t _ld_ram_size2, _ld_ram_addr2;

//#define DEBUG

#ifdef DEBUG

uint32_t test_data[] = {
    0x11111111,
    0x22222222,
    0x33333333,
    0x44444444,
    0x55555555,
    0x66666666,
    0x77777777,
    0x88888888,
    0x99999999,
    0xaaaaaaaa,
    0xbbbbbbbb,
    0xcccccccc,
    0xdddddddd,
    0xeeeeeeee,
    0xffffffff,
    0xdeadbeef,
    0xa5a5a5a5,
    0x5a5a5a5a,
    0x0000ffff,
    0xffff0000};

#endif

/* Coredump state machine */
enum coredump_status {
  COREDUMP_IDLE = 0,
  COREDUMP_ACTIVE,
  COREDUMP_PARSE,
};

/* Dump type of given content */
enum dump_type {
  DUMP_ASCII,       /* Dump string */
  DUMP_BASE64_BYTE, /* Dump memory in byte units, in base64 format. */
  DUMP_BASE64_WORD, /* Dump memory in word units, in base64 format. */
  DUMP_TYPE_MAX,
};

typedef void (*dump_handler_t)(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx);

static void dump_ascii(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx);
static void dump_base64_byte(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx);
static void dump_base64_word(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx);

static const dump_handler_t dump_handler_list[DUMP_TYPE_MAX] = {
    dump_ascii,
    dump_base64_byte,
    dump_base64_word,
};

/* Define default ram dump list */
static const struct mem_hdr {
  uintptr_t addr;
  unsigned int length;
  enum dump_type type;
  const char *desc;
} mem_hdr[] = {
#ifdef DEBUG
    {(uintptr_t)test_data, (unsigned int)sizeof(test_data), DUMP_BASE64_BYTE, "test_data_byte"},
    {(uintptr_t)test_data, (unsigned int)sizeof(test_data), DUMP_BASE64_WORD, "test_data_word"},
    {(uintptr_t) "asdasdasdasdasdsada", 0, DUMP_ASCII, "test_string"},
#else
    {(uintptr_t)&_ld_ram_addr1, (unsigned int)&_ld_ram_size1, DUMP_BASE64_BYTE, "ram"},
    {(uintptr_t)&_ld_ram_addr2, (unsigned int)&_ld_ram_size2, DUMP_BASE64_BYTE, "wifi_ram"},
#endif
};

/* Get current stack top */
static inline uintptr_t cd_getsp(void) {
  register uintptr_t sp;
  __asm__("add %0, x0, sp"
          : "=r"(sp));
  return sp;
}

/**
 * Coredump initialize.
 *
 * @return result
 */
static int cd_getchar(char *inbuf) {
    int ret = -1;
    uint32_t   recv_size = 0;
    uart_dev_t uart_stdio;
    
    memset(&uart_stdio, 0, sizeof(uart_stdio));
    uart_stdio.port = 0;
    ret = hal_uart_recv_II(&uart_stdio, inbuf, 1, &recv_size, HAL_WAIT_FOREVER);
    
    if ((ret == 0) && (recv_size == 1)) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Coredump initialize.
 *
 * @return result
 */
static void cd_putchar(const char *buf, size_t len) {
    uart_dev_t uart_stdio;
    memset(&uart_stdio, 0, sizeof(uart_stdio));
    
    uart_stdio.port = 0; 
    hal_uart_send(&uart_stdio, buf, len, HAL_WAIT_FOREVER);
}

static void cd_base64_wirte_block(const uint8_t buf[4], void *opaque) {
    int *line_wrap = (int *)opaque;
    uart_dev_t uart_stdio;
    memset(&uart_stdio, 0, sizeof(uart_stdio));
    
    uart_stdio.port = 0; 
    hal_uart_send(&uart_stdio, buf, 4, HAL_WAIT_FOREVER); 

  if (++(*line_wrap) > (BASE64_LINE_WRAP >> 2)) {
    hal_uart_send(&uart_stdio, "\r\n", 2, HAL_WAIT_FOREVER);
    *line_wrap = 0;
  }
}

static void dump_ascii(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx) {
  /* reuse len as index here, for calculate the crc */
  for (len = 0; len < strlen((const char *)data); len++) {
    utils_crc32_stream_feed(crc_ctx, *((const char *)data + len));
  }

  cd_putchar(data, strlen((const char *)data));
}

struct base64_byte_ctx {
  /* for output */
  int line_wrap;  /* Base64 line wrap */

  /* for input */
  uintptr_t addr_curr;
  uintptr_t addr_end;
  struct crc32_stream_ctx *crc_ctx;
};

static int read_byte_cb(uint8_t *data, void *opaque) {
  struct base64_byte_ctx *ctx = (struct base64_byte_ctx *)opaque;
  if (ctx->addr_curr < ctx->addr_end) {
    *data = *(uint8_t *)ctx->addr_curr++;

    /* update crc checksum */
    utils_crc32_stream_feed(ctx->crc_ctx, *data);

    return 0;
  } else {
    return 1;
  }
}

static void dump_base64_byte(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx) {
  struct base64_byte_ctx ctx = {0};

  ctx.addr_curr = (uintptr_t)data;
  ctx.addr_end = (uintptr_t)data + len;
  ctx.crc_ctx = crc_ctx;

  utils_base64_encode_stream(read_byte_cb, cd_base64_wirte_block, (void *)&ctx);
}

struct base64_word_ctx {
  /* for output */
  int line_wrap; /* base64 line wrap */

  /* for input */
  uintptr_t addr_base;
  uintptr_t addr_curr;
  uintptr_t addr_end;
  uint8_t buf[4] __attribute__((aligned(4))); /* cache */
  struct crc32_stream_ctx *crc_ctx;
};

static int read_word_cb(uint8_t *data, void *opaque) {
  struct base64_word_ctx *ctx = (struct base64_word_ctx *)opaque;
  uintptr_t base;

  if (ctx->addr_curr < ctx->addr_end) {
    base = (ctx->addr_curr >> 2) << 2;
    if (base != ctx->addr_base) {
      ctx->addr_base = base;
      *(uint32_t *)ctx->buf = *(uint32_t *)base;
    }

    *data = ctx->buf[ctx->addr_curr & 0x3];
    ctx->addr_curr++;

    /* update crc checksum */
    utils_crc32_stream_feed(ctx->crc_ctx, *data);

    return 0;
  } else {
    return 1;
  }
}

static void dump_base64_word(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx) {
  struct base64_word_ctx ctx = {0};

  ctx.addr_base = ((uintptr_t)data >> 2) << 2;
  ctx.addr_curr = (uintptr_t)data;
  ctx.addr_end = ctx.addr_base + len;
  *(uint32_t *)ctx.buf = *(uint32_t *)ctx.addr_base;
  ctx.crc_ctx = crc_ctx;

  utils_base64_encode_stream(read_word_cb, cd_base64_wirte_block, (void *)&ctx);
}

/**
 * Coredump initialize.
 *
 * @return result
 */
static void bl_coredump_print(uintptr_t addr, uint32_t len, const char *desc, enum dump_type type) {
  struct crc32_stream_ctx crc_ctx;
  uint32_t crc;

  uintptr_t tmp;
  char tmp_buf[(sizeof(tmp) << 1)] = {0};

  /* XXX stack overflow check */

  while (cd_getsp() <= (uintptr_t)&_sp_base)
    ;

  /* Print block header */
  cd_putchar(COREDUMP_BLOCK_START_STR, strlen(COREDUMP_BLOCK_START_STR));

  /* print address */
  tmp = REVERSE(addr);
  utils_bin2hex(tmp_buf, &tmp, sizeof(uintptr_t));
  cd_putchar(tmp_buf, sizeof(uintptr_t) << 1);

  cd_putchar("@", 1);

  tmp = REVERSE(len);
  utils_bin2hex(tmp_buf, &tmp, sizeof(uint32_t));
  cd_putchar(tmp_buf, sizeof(uint32_t) << 1);

  if (desc != NULL) {
    cd_putchar("@", 1);
    cd_putchar(desc, strlen(desc));
  }

  cd_putchar(COREDUMP_BLOCK_CLOSE_STR, strlen(COREDUMP_BLOCK_CLOSE_STR));

  /* Init crc context */
  utils_crc32_stream_init(&crc_ctx);

  /* Dump block content */
  dump_handler_list[type]((const void *)addr, (ssize_t)len, &crc_ctx);

  /* Dump end */
  cd_putchar(COREDUMP_BLOCK_END_STR, strlen(COREDUMP_BLOCK_END_STR));

  crc = utils_crc32_stream_results(&crc_ctx);

  dump_handler_list[DUMP_BASE64_WORD]((const void *)&crc, (ssize_t)sizeof(uint32_t), &crc_ctx);

  cd_putchar(COREDUMP_BLOCK_CLOSE_STR, sizeof(COREDUMP_BLOCK_CLOSE_STR));
}

/**
 * Coredump initialize.
 *
 * @return result
 */
void bl_coredump_parse(const uint8_t *buf, unsigned int len) {
  char command;
  int i = 0;

  command = buf[i++];

  switch (command) {
  case 'x':
    do {
      unsigned int addr, length = 0;

      if (len - i >= 8) {
        utils_hex2bin((char *)&buf[i], 8, (uint8_t *)&addr, sizeof(addr));
        addr = REVERSE(addr);
        i += 8;

        command = buf[i++];
        if (command == 'l' && (len - i) >= 8) {
          utils_hex2bin((char *)&buf[i], 8, (uint8_t *)&length, sizeof(length));
          length = REVERSE(length);
        } else {
          length = 0x1000;
        }
        bl_coredump_print(addr, length, NULL, DUMP_BASE64_WORD);
      }
    } while (0);
    return;

  case 'd':
    do {
      for (i = 0; i < (sizeof(mem_hdr) / sizeof(mem_hdr[0])); i++) {
        bl_coredump_print(mem_hdr[i].addr, mem_hdr[i].length, mem_hdr[i].desc, mem_hdr[i].type);
      }
    } while (0);
    return;

  default:
    return;
  }
}

/**
 * Coredump initialize.
 *
 * @return result
 */
void bl_coredump_run() {
  char c;
  uint8_t cmd_buf[COREDUM_CMD_BUF_LEN + 1] = {0};
  enum coredump_status status = COREDUMP_IDLE;
  uint8_t cmd_pos = 0;

  /* XXX change sp to irq stack base */

  __asm__ volatile("add sp, x0, %0" ::"r"(&_sp_main));

  cd_putchar(COREDUMP_START_STR, strlen(COREDUMP_START_STR));

  /* Dump all pre-defined memory region by default */
  for (cmd_pos = 0; cmd_pos < (sizeof(mem_hdr) / sizeof(mem_hdr[0])); cmd_pos++) {
    bl_coredump_print(mem_hdr[cmd_pos].addr, mem_hdr[cmd_pos].length, mem_hdr[cmd_pos].desc, mem_hdr[cmd_pos].type);
  }

  while (1) {
    if (cd_getchar(&c) == 0) {
      continue;
    }

  _reactive: /* In some scenarios we need to reactivate the state machine */
    switch (status) {
    case COREDUMP_IDLE:
      if (c == '&') {
        status = COREDUMP_ACTIVE;
        cmd_pos = 0;
      }
      continue;

    case COREDUMP_ACTIVE:
      if (c == '#') {
        status = COREDUMP_PARSE;
        cmd_buf[cmd_pos] = 0;
        goto _reactive;
      }

      cmd_buf[cmd_pos++] = c;
      if (cmd_pos > COREDUM_CMD_BUF_LEN) {
        status = COREDUMP_IDLE;
      }
      continue;

    case COREDUMP_PARSE:
      status = COREDUMP_IDLE;
      bl_coredump_parse(cmd_buf, cmd_pos);
      continue;
    }
  }
}
