// SPDX-License-Identifier: GPL-2.0-only
/*
 * gash/mandelbrot.c
 *
 * A mandelbrot set. A neat way to test Goldspace's
 * speed. Or something. I don't know.
 *
 * Copyright (C) 2025 Goldside543
 *
 */

#include "shell.h"
#include "../kernel/print.h"
#include "../mm/memory.h"

#define WIDTH 80
#define HEIGHT 25
#define MAX_ITER 30

int mandelbrot(double cr, double ci) {
    double zr = 0.0, zi = 0.0;
    int iter = 0;
    while (zr*zr + zi*zi <= 4.0 && iter < MAX_ITER) {
        double tmp = zr*zr - zi*zi + cr;
        zi = 2.0*zr*zi + ci;
        zr = tmp;
        iter++;
    }
    return iter;
}

int shell_mandelbrot() {
    char frame[(WIDTH + 1) * HEIGHT + 1];
    kmemset(frame, 0, sizeof(frame));

    // Start zoom window
    double centerX = -0.7436438870371587; // some juicy zoom spot
    double centerY = 0.13182590420531198;

    double zoom = 20.0; // zoom factor, 1 = default view

    while (1) {
        // Shrink the window to zoom in over time
        zoom *= 0.95;  // zoom speed (0.95 = 5% zoom per frame)

        double rangeX = 3.0 * zoom; // original range from -2 to 1 is 3 units
        double rangeY = 2.0 * zoom; // original range from -1 to 1 is 2 units

        double xmin = centerX - rangeX / 2;
        double xmax = centerX + rangeX / 2;
        double ymin = centerY - rangeY / 2;
        double ymax = centerY + rangeY / 2;

        int pos = 0;
        for (int y = 0; y < HEIGHT; y++) {
            double ci = ymin + (ymax - ymin) * y / (HEIGHT - 1);
            for (int x = 0; x < WIDTH; x++) {
                double cr = xmin + (xmax - xmin) * x / (WIDTH - 1);
                int iter = mandelbrot(cr, ci);
                frame[pos++] = (iter == MAX_ITER) ? '#' : ' ';
            }
            frame[pos++] = '\n';
        }
        frame[pos] = '\0';

        shell_clear();
        print(frame);
    }

    return 0;
}
