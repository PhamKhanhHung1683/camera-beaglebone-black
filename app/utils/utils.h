#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

void get_cpu_usage(int *cpu_usage);
void get_memory_usage(int *used_mb, int *total_mb);

#ifdef __cplusplus
}
#endif

#endif // UTILS_H