#ifndef SSD1306_H
#define SSD1306_H

int ssd1306_init();
void ssd1306_end();
int ssd1306_write(const char* buf);

#endif