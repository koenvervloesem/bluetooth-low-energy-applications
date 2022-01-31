/*
 * Example of a custom BLE service using Secure Connections Only Mode.
 *
 * Copyright (c) 2021 Koen Vervloesem
 *
 * SPDX-License-Identifier: MIT
 *
 * Based on an example of the Bluetooth LE Security Guide.
 */

#include <device.h>
#include <drivers/gpio.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <zephyr.h>
#include <zephyr/types.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>
#include <bluetooth/hci.h>
#include <bluetooth/uuid.h>
#include <settings/settings.h>

#include "service.h"

// Configure GPIO for buttons

// Button Yes
#define SW0_NODE DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button_yes =
    GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});

// Button No
#define SW1_NODE DT_ALIAS(sw1)
#if !DT_NODE_HAS_STATUS(SW1_NODE, okay)
#error "Unsupported board: sw1 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button_no =
    GPIO_DT_SPEC_GET_OR(SW1_NODE, gpios, {0});

static struct gpio_callback gpio_btn_yes_cb;
static struct gpio_callback gpio_btn_no_cb;
static struct k_work button_yes_work;
static struct k_work button_no_work;

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

bool authenticating = false;

struct bt_conn *default_conn;

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS,
                  (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static void pairing_passkey_display(struct bt_conn *conn,
                                    unsigned int passkey) {
  char addr[BT_ADDR_LE_STR_LEN];
  bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
  printk("Passkey for %s: %06u\n", addr, passkey);
}

static void auth_confirm(struct bt_conn *conn, unsigned int passkey) {

  default_conn = conn;
  char addr[BT_ADDR_LE_STR_LEN];
  char passkey_str[7];
  bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
  snprintk(passkey_str, 7, "%06u", passkey);
  printk("\nConfirm passkey for %s: %s\n\n", addr, passkey_str);
  printk("Press button 1 for YES or button 2 for NO\n");
  authenticating = true;
}

static void auth_cancel(struct bt_conn *conn) {
  printk("Pairing cancelled\n");
  authenticating = false;
}

static struct bt_conn_auth_cb pairing_cb_display = {
    .passkey_display = pairing_passkey_display,
    .passkey_confirm = auth_confirm,
    .cancel = auth_cancel,
};

static void connected(struct bt_conn *conn, uint8_t err) {
  if (!err) {
    printk("Connected\n");
    default_conn = bt_conn_ref(conn);

    if (bt_conn_set_security(default_conn, BT_SECURITY_L4)) {
      printk("Failed to set security\n");
    }
  }
}

static void disconnected(struct bt_conn *conn, uint8_t reason) {
  if (default_conn) {
    printk("Disconnected\n");
    bt_conn_unref(default_conn);
    default_conn = NULL;
  }
}

static void security_level_changed(struct bt_conn *conn,
                                   bt_security_t level,
                                   enum bt_security_err err) {
  printk("Security level changed to %d\n", level);
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
    .security_changed = security_level_changed,
};

static void bt_ready(int err) {
  if (err) {
    return;
  }

  service_init();

  if (IS_ENABLED(CONFIG_SETTINGS)) {
    settings_load();
  }

  err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), NULL, 0);
  if (err) {
    return;
  }
}

void button_yes_work_handler(struct k_work *work) {
  if (!authenticating) {
    return;
  }
  printk("User indicated YES\n");
  bt_conn_auth_passkey_confirm(default_conn);
  authenticating = false;
}

void button_no_work_handler(struct k_work *work) {
  if (!authenticating) {
    return;
  }
  printk("User indicated NO\n");
  bt_conn_auth_cancel(default_conn);
  authenticating = false;
}

void button_yes_pressed(const struct device *gpiob,
                        struct gpio_callback *cb, uint32_t pins) {
  printk("Button Yes pressed\n");
  if (!authenticating) {
    return;
  }
  k_work_submit(&button_yes_work);
}

void button_no_pressed(const struct device *gpiob,
                       struct gpio_callback *cb, uint32_t pins) {
  printk("Button No pressed\n");
  k_work_submit(&button_no_work);
}

void configure_buttons(void) {
  int ret;

  // Button Yes
  k_work_init(&button_yes_work, button_yes_work_handler);
  ret = gpio_pin_configure_dt(&button_yes, GPIO_INPUT);
  if (ret != 0) {
    printk("Error %d: failed to configure %s pin %d\n", ret,
           button_yes.port->name, button_yes.pin);
    return;
  }

  ret = gpio_pin_interrupt_configure_dt(&button_yes,
                                        GPIO_INT_EDGE_TO_ACTIVE);
  if (ret != 0) {
    printk("Error %d: failed to configure interrupt on %s pin %d\n",
           ret, button_yes.port->name, button_yes.pin);
    return;
  }

  gpio_init_callback(&gpio_btn_yes_cb, button_yes_pressed,
                     BIT(button_yes.pin));
  gpio_add_callback(button_yes.port, &gpio_btn_yes_cb);
  printk("Set up button at %s pin %d\n", button_yes.port->name,
         button_yes.pin);

  // Button No
  k_work_init(&button_no_work, button_no_work_handler);
  ret = gpio_pin_configure_dt(&button_no, GPIO_INPUT);
  if (ret != 0) {
    printk("Error %d: failed to configure %s pin %d\n", ret,
           button_no.port->name, button_no.pin);
    return;
  }

  ret = gpio_pin_interrupt_configure_dt(&button_no,
                                        GPIO_INT_EDGE_TO_ACTIVE);
  if (ret != 0) {
    printk("Error %d: failed to configure interrupt on %s pin %d\n",
           ret, button_no.port->name, button_no.pin);
    return;
  }

  gpio_init_callback(&gpio_btn_no_cb, button_no_pressed,
                     BIT(button_no.pin));
  gpio_add_callback(button_no.port, &gpio_btn_no_cb);
  printk("Set up button at %s pin %d\n", button_no.port->name,
         button_no.pin);
}

void main(void) {

  printk("Starting application...\n");
  int err;
  configure_buttons();
  err = bt_enable(bt_ready);
  if (err) {
    return;
  }

  bt_conn_cb_register(&conn_callbacks);
  bt_conn_auth_cb_register(&pairing_cb_display);
}
