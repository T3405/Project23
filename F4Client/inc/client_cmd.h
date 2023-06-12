
/***
 * Read fifo
 * @param fd id of the fifo
 * @param code > 0 = cmd_id or 0 if there is nothing on the fifo
 * @return pointer to the data
 */
int cmd_read_code(int fd);