#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>

#include "service.h"

// Custom service
#define BT_UUID_SERVICE                                              \
  BT_UUID_DECLARE_128(0x50, 0x22, 0xe5, 0xd4, 0x9c, 0xc1, 0x4e,      \
                      0x9e, 0xbe, 0x6e, 0xda, 0x13, 0x87, 0x87,      \
                      0x07, 0x77)
// Characteristic to write to the peripheral device
#define BT_UUID_INPUT                                                \
  BT_UUID_DECLARE_128(0x51, 0x22, 0xe5, 0xd4, 0x9c, 0xc1, 0x4e,      \
                      0x9e, 0xbe, 0x6e, 0xda, 0x13, 0x87, 0x87,      \
                      0x07, 0x77)
// Characteristic to read from the peripheral device
#define BT_UUID_OUTPUT                                               \
  BT_UUID_DECLARE_128(0x52, 0x22, 0xe5, 0xd4, 0x9c, 0xc1, 0x4e,      \
                      0x9e, 0xbe, 0x6e, 0xda, 0x13, 0x87, 0x87,      \
                      0x07, 0x77)

static struct bt_gatt_attr attrs[] = {
    BT_GATT_PRIMARY_SERVICE(BT_UUID_SERVICE),
    BT_GATT_CHARACTERISTIC(BT_UUID_INPUT, BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_WRITE_AUTHEN, NULL,
                           write_characteristic, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_OUTPUT, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ_AUTHEN,
                           read_characteristic, NULL, NULL),
};

static struct bt_gatt_service service = BT_GATT_SERVICE(attrs);

void service_init() { bt_gatt_service_register(&service); }

uint8_t saved_number = 0;

ssize_t write_characteristic(struct bt_conn *conn,
                             const struct bt_gatt_attr *attr,
                             const void *buf, uint16_t len,
                             uint16_t offset, uint8_t flags) {
  const uint8_t *new_number = buf;
  if (!len) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
  }

  saved_number = *new_number;
  printk("Write characteristic: %d\n", saved_number);
  return len;
}

ssize_t read_characteristic(struct bt_conn *conn,
                            const struct bt_gatt_attr *attr,
                            void *buf, uint16_t len,
                            uint16_t offset) {
  printk("Read characteristic\n");
  return bt_gatt_attr_read(conn, attr, buf, len, offset,
                           &saved_number, sizeof(saved_number));
}
