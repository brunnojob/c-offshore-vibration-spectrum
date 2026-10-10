#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <assert.h>

static void fail(const char *message) { fprintf(stderr, "%s\n", message); exit(2); }
double number(const char *s) {
    char *end; errno = 0; double value = strtod(s, &end);
    if (errno || end == s || *end || !isfinite(value)) fail("invalid finite number");
    return value;
}
static size_t fields(char *line, char **out, size_t capacity) {
    line[strcspn(line, "\r\n")] = 0;
    size_t count = 0;
    char *p = line;
    while (*p) {
        if (count == capacity) fail("too many fields");
        if (*p == ',') fail("empty field");
        out[count++] = p;
        char *separator = strchr(p, ',');
        if (!separator) break;
        *separator = 0; p = separator + 1;
        if (!*p) fail("empty field");
    }
    return count;
}

typedef struct { double rms, crest, frequency, amplitude; } Result;
static Result spectrum(const double *x, size_t n, double rate) {
    double mean = 0, energy = 0, peak = 0, best = 0; size_t bin = 0;
    for (size_t i = 0; i < n; i++) mean += x[i];
    mean /= (double)n;
    for (size_t i = 0; i < n; i++) { double v = x[i] - mean; energy += v * v; if (fabs(v) > peak) peak = fabs(v); }
    for (size_t k = 1; k <= n / 2; k++) {
        double real = 0, imaginary = 0, weight = 0;
        for (size_t i = 0; i < n; i++) {
            double window = 0.5 - 0.5 * cos(2 * acos(-1) * i / (n - 1));
            double angle = 2 * acos(-1) * k * i / n;
            real += (x[i] - mean) * window * cos(angle); imaginary -= (x[i] - mean) * window * sin(angle); weight += window;
        }
        double amplitude = hypot(real, imaginary) * (k == n / 2 ? 1 : 2) / weight;
        if (amplitude > best) { best = amplitude; bin = k; }
    }
    double rms = sqrt(energy / n);
    return (Result){rms, rms > 0 ? peak / rms : 0, bin * rate / n, best};
}
int main(int argc, char **argv) {
    if (argc > 1 && !(argc == 2 && !strcmp(argv[1], "--self-test"))) fail("unsupported command arguments");
    if (argc == 2 && !strcmp(argv[1], "--self-test")) {
        double x[128]; for (size_t i = 0; i < 128; i++) x[i] = 3 + 2 * sin(2 * acos(-1) * 8 * i / 128);
        Result r = spectrum(x, 128, 128); assert(fabs(r.frequency - 8) < 1e-9); assert(fabs(r.rms - sqrt(2)) < 1e-9);
        memset(x, 0, sizeof x); assert(spectrum(x, 128, 128).crest == 0); puts("sine and zero tests passed"); return 0;
    }
    double x[4096], time[4096]; size_t n = 0; char line[256];
    while (fgets(line, sizeof line, stdin)) {
        if (strlen(line) == sizeof line - 1 && line[sizeof line - 2] != '\n') fail("input record too large");
        char *f[2]; if (fields(line, f, 2) != 2 || n == 4096) fail("expected time,acceleration within 4096 samples");
        time[n] = number(f[0]); x[n] = number(f[1]);
        if (n && time[n] <= time[n - 1]) fail("time must increase");
        n++;
    }
    if (n < 8 || ferror(stdin)) fail("at least eight samples required");
    double dt = time[1] - time[0];
    for (size_t i = 2; i < n; i++) if (fabs(time[i] - time[i - 1] - dt) > dt * 1e-4) fail("uniform sampling required");
    Result r = spectrum(x, n, 1 / dt);
    printf("rms=%.6f crest=%.6f dominant_hz=%.6f amplitude=%.6f\n", r.rms, r.crest, r.frequency, r.amplitude); return 0;
}
