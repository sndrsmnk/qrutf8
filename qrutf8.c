// Copyright 2014 S. Smeenk <ssmeenk@freshdot.net>
// Based on code from google-authenticator.c
// Copyright 2010 Google Inc. Markus Gutschke
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#define ANSI_RESET        "\x1B[0m"
#define ANSI_BLACKONGREY  "\x1B[30;47;27m"
#define ANSI_WHITE        "\x1B[27m"
#define ANSI_BLACK        "\x1B[7m"
#define UTF8_BOTH         "\xE2\x96\x88"
#define UTF8_TOPHALF      "\xE2\x96\x80"
#define UTF8_BOTTOMHALF   "\xE2\x96\x84"

int main(int argc, char **argv) {

    if (argc == 1) {
        printf("Usage: %s 'text'\n", argv[0]);
        printf("Creates TEXT-type QR-code for 'text' in UTF-8/ANSI output.\n");
        exit(1);
    }

    const char *url = argv[1];

    void *qrencode = dlopen("libqrencode.so.2", RTLD_NOW | RTLD_LOCAL);
    if (!qrencode) {
        qrencode = dlopen("libqrencode.so.3", RTLD_NOW | RTLD_LOCAL);
    }
    if (!qrencode) {
        printf("dlopen: %s\n", dlerror());
        printf("Make sure libqrencode is installed.\n");
        exit(1);
    }

    typedef struct {
        int version;
        int width;
        unsigned char *data;
    } QRcode;

    QRcode *(*QRcode_encodeString8bit)(const char *, int, int) =
        (QRcode *(*)(const char *, int, int))dlsym(qrencode, "QRcode_encodeString8bit");
    void (*QRcode_free)(QRcode *qrcode) =
        (void (*)(QRcode *))dlsym(qrencode, "QRcode_free");

    if (QRcode_encodeString8bit && QRcode_free) {
        QRcode *qrcode = QRcode_encodeString8bit(url, 0, 1);

        printf(ANSI_BLACKONGREY);
        for (int i = 0; i < qrcode->width + 4; ++i) {
            printf(" ");
        }
        puts(ANSI_RESET);
        for (int y = 0; y < qrcode->width; y += 2) {
            printf(ANSI_BLACKONGREY"  ");
            for (int x = 0; x < qrcode->width; ++x) {
                int top = qrcode->data[y*qrcode->width + x] & 1;
                int bottom = 0;
                if (y+1 < qrcode->width) {
                    bottom = qrcode->data[(y+1)*qrcode->width + x] & 1;
                }
                if (top) {
                    if (bottom) {
                        printf(UTF8_BOTH);
                    } else {
                        printf(UTF8_TOPHALF);
                    }
                } else {
                    if (bottom) {
                        printf(UTF8_BOTTOMHALF);
                    } else {
                        printf(" ");
                    }
                }
            }
            puts("  "ANSI_RESET);
        }
        printf(ANSI_BLACKONGREY);
        for (int i = 0; i < qrcode->width + 4; ++i) {
            printf(" ");
        }
        puts(ANSI_RESET);
        
        QRcode_free(qrcode);
    } else {
        printf("dlsym: %s\n", dlerror());
        printf("Make sure libqrencode is installed properly.\n");
        exit(1);
    }

    dlclose(qrencode);
}
