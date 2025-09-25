/*!
 * \file       FSM.c
 * \author     Horak, Jurena
 * \date       2019.6
 * \brief      Implementation of function.h header file
 * ******************************************
 * \attention
 * &copy; Copyright (c) 2022 FAI UTB. All rights reserved.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/* Includes ------------------------------------------------------------------*/
#include "FSM.h"

#include <linux/limits.h>

/* Private types -------------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function declarations ---------------------------------------------*/
/* Exported functions definitions --------------------------------------------*/
enum comment_state {
    NONE,
    SINGLEQUOTE,
    DOUBLEQUOTE,
    COMMENT_START,
    COMMENT_END,
    SINGLELINE,
    MULTILINE,
};

void FSM_RemoveComments(FILE* input, FILE* output) {
    (void)input;
    (void)output;

    char c;
    int escape = 0;
    enum comment_state state = NONE;

    while ((c = (char)fgetc(input)) != EOF) {
        if (state == NONE) {
            if (c == '"') {
                state = DOUBLEQUOTE;
                fputc(c, output);
            } else if (c == '\'') {
                state = SINGLEQUOTE;
                fputc(c, output);
            } else if (c == '/') {
                state = COMMENT_START;
            } else {
                fputc(c, output);
            }
            continue;
        }

        if (state == DOUBLEQUOTE && !escape && c == '"') {
            state = NONE;
            fputc(c, output);
            continue;
        }

        if (state == SINGLEQUOTE && !escape && c == '\'') {
            state = NONE;
            fputc(c, output);
            continue;
        }

        if ((state == DOUBLEQUOTE || state == SINGLEQUOTE) && escape) {
            escape = 0;
            fputc(c, output);
            continue;
        }

        if ((state == DOUBLEQUOTE || state == SINGLEQUOTE) && c == '\\') {
            escape = 1;
            fputc(c, output);
            continue;
        }

        if ((state == DOUBLEQUOTE || state == SINGLEQUOTE)) {
            fputc(c, output);
            continue;
        }

        if (state == COMMENT_START) {
            if (c == '/') {
                state = SINGLELINE;
            } else if (c == '*') {
                state = MULTILINE;
            } else {
                state = NONE;
                fputc('/', output);
                fputc(c, output);
            }
            continue;
        }

        if (state == SINGLELINE && c == '\n') {
            fputc(c, output);
            state = NONE;
            continue;
        }

        if (state == MULTILINE && c == '*') {
            state = COMMENT_END;
            continue;
        }

        if (state == COMMENT_END) {
            if (c == '/') {
                state = NONE;
            } else {
                state = MULTILINE;
            }
            continue;
        }
    }
}
