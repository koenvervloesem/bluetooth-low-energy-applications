void service_init();
ssize_t write_characteristic(struct bt_conn *conn,
                             const struct bt_gatt_attr *attr,
                             const void *buf, uint16_t len,
                             uint16_t offset, uint8_t flags);
ssize_t read_characteristic(struct bt_conn *conn,
                            const struct bt_gatt_attr *attr,
                            void *buf, uint16_t len, uint16_t offset);
