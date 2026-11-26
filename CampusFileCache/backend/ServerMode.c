#include "ServerMode.h"
#include "LFUCache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void jsonString(const char *s) {
    putchar('"');
    for (; *s; ++s) {
        unsigned char c = (unsigned char)*s;
        if (c == '"' || c == '\\') { putchar('\\'); putchar(c); }
        else if (c == '\n') fputs("\\n", stdout);
        else if (c == '\r') fputs("\\r", stdout);
        else if (c == '\t') fputs("\\t", stdout);
        else if (c < 32) printf("\\u%04x", c);
        else putchar(c);
    }
    putchar('"');
}

static void emitState(LFUCache *cache, const char *op, const char *status,
                      const char *key, const char *value,
                      const char *evictedKey, int evictedFreq, int tieCount,
                      int hits, int misses, int evictions) {
    FrequencyGroup *groups[MAX_CAPACITY];
    int count = 0;
    for (int i = 0; i < FREQ_HASH_SIZE; ++i) {
        FrequencyEntry *e = cache->frequencies.buckets[i];
        while (e && count < MAX_CAPACITY) { groups[count++] = e->group; e = e->next; }
    }
    for (int i = 1; i < count; ++i) {
        FrequencyGroup *x = groups[i]; int j = i - 1;
        while (j >= 0 && groups[j]->frequency > x->frequency) { groups[j+1] = groups[j]; --j; }
        groups[j+1] = x;
    }
    fputs("{\"type\":\"result\",\"operation\":", stdout); jsonString(op);
    fputs(",\"status\":", stdout); jsonString(status);
    fputs(",\"key\":", stdout); jsonString(key ? key : "");
    fputs(",\"value\":", stdout); jsonString(value ? value : "");
    fputs(",\"evicted\":", stdout);
    if (evictedKey && evictedKey[0]) {
        fputs("{\"key\":", stdout); jsonString(evictedKey);
        printf(",\"frequency\":%d,\"tieCount\":%d,\"reason\":", evictedFreq, tieCount);
        jsonString(tieCount > 1 ? "LFU tie: least recently used" : "Unique lowest frequency");
        putchar('}');
    } else fputs("null", stdout);
    printf(",\"capacity\":%d,\"size\":%d,\"minFreq\":%d", cache->capacity, cache->size, cache->minFreq);
    printf(",\"stats\":{\"hits\":%d,\"misses\":%d,\"evictions\":%d}", hits, misses, evictions);
    fputs(",\"groups\":[", stdout);
    for (int i = 0; i < count; ++i) {
        if (i) putchar(',');
        printf("{\"frequency\":%d,\"items\":[", groups[i]->frequency);
        Node *n = groups[i]->list.head; int first = 1;
        while (n) {
            if (!first) { putchar(','); }
            first = 0;
            fputs("{\"key\":", stdout); jsonString(n->key);
            fputs(",\"value\":", stdout); jsonString(n->value);
            printf(",\"frequency\":%d,\"bucket\":%u}", n->freq, hashFunction(n->key));
            n = n->next;
        }
        fputs("]}", stdout);
    }
    fputs("]}\n", stdout);
    fflush(stdout);
}

int runServerMode(void) {
    LFUCache cache;
    int initialized = 0, hits = 0, misses = 0, evictions = 0;
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        char *cmd = strtok(line, "\t\r\n");
        if (!cmd) continue;
        if (strcmp(cmd, "INIT") == 0 || strcmp(cmd, "RESET") == 0) {
            char *capText = strtok(NULL, "\t\r\n");
            int cap = capText ? atoi(capText) : -1;
            if (cap < 0 || cap > MAX_CAPACITY) {
                puts("{\"type\":\"error\",\"message\":\"Capacity must be between 0 and 200\"}"); fflush(stdout); continue;
            }
            if (initialized) destroyCache(&cache);
            initCache(&cache, cap); initialized = 1;
            hits = misses = evictions = 0;
            emitState(&cache, cmd, "ready", "", "", NULL, 0, 0, hits, misses, evictions);
        } else if (strcmp(cmd, "QUIT") == 0) {
            if (initialized) destroyCache(&cache);
            break;
        } else if (!initialized) {
            puts("{\"type\":\"error\",\"message\":\"Send INIT first\"}"); fflush(stdout);
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = strtok(NULL, "\t\r\n");
            char result[MAX_VALUE] = "";
            if (!key || strlen(key) >= MAX_KEY) { puts("{\"type\":\"error\",\"message\":\"Invalid key\"}"); fflush(stdout); continue; }
            int found = GET(&cache, key, result);
            if (found) ++hits; else ++misses;
            emitState(&cache, "GET", found ? "hit" : "miss", key, result, NULL, 0, 0, hits, misses, evictions);
        } else if (strcmp(cmd, "PUT") == 0) {
            char *key = strtok(NULL, "\t\r\n");
            char *value = strtok(NULL, "\t\r\n");
            if (!key || !value || strlen(key) >= MAX_KEY || strlen(value) >= MAX_VALUE) {
                puts("{\"type\":\"error\",\"message\":\"Invalid key/value. Tabs and spaces are not supported in this demo protocol.\"}"); fflush(stdout); continue;
            }
            char evicted[MAX_KEY] = ""; int freq = 0, ties = 0;
            int result = PUTDetailed(&cache, key, value, evicted, &freq, &ties);
            if (evicted[0]) ++evictions;
            const char *status = result == 1 ? "inserted" : result == 2 ? "updated" : result == 0 ? "rejected-zero-capacity" : "error";
            emitState(&cache, "PUT", status, key, value, evicted, freq, ties, hits, misses, evictions);
        } else if (strcmp(cmd, "STATE") == 0) {
            emitState(&cache, "STATE", "ok", "", "", NULL, 0, 0, hits, misses, evictions);
        } else {
            puts("{\"type\":\"error\",\"message\":\"Unknown command\"}"); fflush(stdout);
        }
    }
    return 0;
}
