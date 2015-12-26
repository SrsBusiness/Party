#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>


static int to_child[2], to_parent[2];
static FILE *in, *out;


void init_engine();
int engine_score(char *fen);

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <training set>\n", argv[0]);
        exit(-1);
    }

    FILE *tset = fopen(argv[1], "r");
    if (tset == NULL) {
        fprintf(stderr, "File %s does not exist\n", argv[1]);
        exit(-1);
    }
    char *out_name = malloc(strlen(argv[1]) + strlen("_score") + 1);
    strcpy(out_name, argv[1]);
    strncat(out_name, "_score", strlen("_score") + 1);
    FILE *tset_scores = fopen(out_name, "w+");
    init_engine();
    
    char fen[512] = {0};
    while (fgets(fen, sizeof(fen), tset) != NULL) {
        /* fen has stray newline */
        fen[strlen(fen) - 1] = '\0';
        printf("FEN: %s\n", fen);
        int score = engine_score(fen);
        fprintf(tset_scores, "%d %s\n", score, fen);  
    }
    fclose(tset);
    fclose(tset_scores);
    return 0;
}

void init_engine() {
    if (pipe(to_child) == -1) { /* Parent writes, child reads */
        fprintf(stderr, "Error pipe: to_child\n");
        perror(strerror(errno));
        exit(-1);
    }
    if (pipe(to_parent) == -1) {/* Parent reads, child writes */
        fprintf(stderr, "Error pipe: to_parent\n");
        perror(strerror(errno));
        exit(-1);
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Fork error\n");
        exit(-1);
    } else if (pid == 0) { /* child */
        if (dup2(to_child[0], STDIN_FILENO) == -1) {
            fprintf(stderr, "Failed to dup to_child[0]\n");
            exit(-1);
        }
        if (dup2(to_parent[1], STDOUT_FILENO) == -1) {
            fprintf(stderr, "Failed to dup to_parent[1]\n");
            exit(-1);
        }
        close(to_child[1]); 
        close(to_parent[0]);
        close(to_child[0]);
        close(to_parent[1]);
        char *argv[] = {"stockfish", NULL};
        execv("/home/bucky/git/Stockfish/src/stockfish", argv);
    } else { /* parent */
        close(to_child[0]);
        close(to_parent[1]);

        in = fdopen(to_parent[0], "r");
        out = fdopen(to_child[1], "w");
        char input[256] = {0};
        fprintf(out, "uci\n");
        fflush(out);
        do { 
            char *status = fgets(input, sizeof(input), in);
            printf("%s\n", input);
            if (status == NULL) {
                fprintf(stderr, "Unexpected EOF\n");
                exit(-1);
            }
        } while (strstr(input, "uciok") == NULL);
        printf("Engine ready\n");
    }
}

int engine_score(char *fen) {
    int score = 0;
    fprintf(out, "ucinewgame\n");
    fprintf(out, "position fen %s\n", fen);
    fprintf(out, "go movetime 10000\n");
    fflush(out);
    char input[512] = {0};
    char tmp[512] = {0};
    do {
        fgets(input, sizeof(input), in);
        if (strstr(input, "score cp")) {
            char *tok = tmp;
            strncpy(tok, input, sizeof(input));
            tok = strtok(tok, " ");
            while (strncmp("cp", tok, strlen("cp") + 1)) {
                tok = strtok(NULL, " ");
            }
            tok = strtok(NULL, " ");
            sscanf(tok, "%d", &score);
        }
    } while (strstr(input, "bestmove") == NULL);
    return score;
}
