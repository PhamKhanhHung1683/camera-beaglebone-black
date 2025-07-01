#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "utils.h"

void get_cpu_usage(int *cpu_usage) {
    static unsigned long long last_total = 0, last_idle = 0;
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) {
        *cpu_usage = 0;
        return;
    }

    char buffer[256];
    if (!fgets(buffer, sizeof(buffer), fp)) {
        fclose(fp);
        *cpu_usage = 0;
        return;
    }
    fclose(fp);

    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    if (sscanf(buffer, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice) != 10) {
        *cpu_usage = 0;
        return;
    }

    unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
    unsigned long long idle_time = idle + iowait;
    int usage = 0;
    if (last_total != 0) {
        double total_diff = total - last_total;
        double idle_diff = idle_time - last_idle;
        if (total_diff > 0) {
            usage = (int)round(100.0 * (total_diff - idle_diff) / total_diff);
        }
    }
    last_total = total;
    last_idle = idle_time;
    *cpu_usage = usage;
}

void get_memory_usage(int *used_mb, int *total_mb) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        *used_mb = 0;
        *total_mb = 0;
        return;
    }

    unsigned long mem_total = 0, mem_free = 0, buffers = 0, cached = 0;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp)) {
        unsigned long value;
        if (sscanf(buffer, "MemTotal: %lu kB", &value) == 1) {
            mem_total = value;
        } else if (sscanf(buffer, "MemFree: %lu kB", &value) == 1) {
            mem_free = value;
        } else if (sscanf(buffer, "Buffers: %lu kB", &value) == 1) {
            buffers = value;
        } else if (sscanf(buffer, "Cached: %lu kB", &value) == 1) {
            cached = value;
        }
    }
    fclose(fp);

    *used_mb = (int)round((mem_total - mem_free - buffers - cached) / 1024.0);
    *total_mb = (int)round(mem_total / 1024.0);
}