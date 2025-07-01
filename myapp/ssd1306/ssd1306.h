#ifndef SSD1306_H
#define SSD1306_H

#ifdef __cplusplus
extern "C" {
#endif

int ssd1306_init();
void ssd1306_end();
int ssd1306_write(const char* buf);

#ifdef __cplusplus
}
#endif

#endif // SSD1306_H