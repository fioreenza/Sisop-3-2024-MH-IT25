#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_LENGTH 100

void numberToWords(int num, char *words) {
    char *units[] = {"nol", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan", "sepuluh", "sebelas", "dua belas", "tiga belas", "empat belas", "lima belas", "enam belas", "tujuh belas", "delapan belas", "sembilan belas"};
    char *tens[] = {"", "", "dua puluh", "tiga puluh", "empat puluh", "lima puluh", "enam puluh", "tujuh puluh", "delapan puluh", "sembilan puluh"};

    if (num < 20) {
        strcpy(words, units[num]);
    } else if (num < 100) {
        strcpy(words, tens[num / 10]);
        if (num % 10 != 0) {
            strcat(words, " ");
            strcat(words, units[num % 10]);
        }
    }
}

int word_to_number(const char* word) {
    if (strcmp(word, "nol") == 0) {
        return 0;
    }
    const char* numbers[] = {"satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan"};
    for (int i = 0; i < 9 ; ++i) {
        if (strcmp(word, numbers[i]) == 0) {
            return i + 1;
        }
    }
    return -1; // Return -1 for invalid input
}

// Function to write log to histori.log file
void writeLog(const char *type, const char *message) {
    FILE *log_file = fopen("histori.log", "a");
    if (log_file == NULL) {
        perror("histori.log");
        exit(EXIT_FAILURE);
    }
    time_t now;
    struct tm *local_time;
    time(&now);
    local_time = localtime(&now);
    fprintf(log_file, "[%02d/%02d/%02d %02d:%02d:%02d] [%s] %s\n", local_time->tm_mday, local_time->tm_mon + 1, local_time->tm_year % 100, local_time->tm_hour, local_time->tm_min, local_time->tm_sec, type, message);
    fclose(log_file);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <operation> <num1> <num2>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *operation = argv[1];
    char *num1_word = argv[2];
    char *num2_word = argv[3];

    int fd[2];
    if (pipe(fd) == -1) {
        perror("Pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        close(fd[0]); // Close unused read end

        int num1, num2, result;
        char result_word[MAX_LENGTH];
        char message[MAX_LENGTH * 2];

        num1 = word_to_number(num1_word);
    	num2 = word_to_number(num2_word);
    	
    	if (num1 == -1 && num2 == -1) {
    	   fprintf(stderr, "Input tidak valid.\n");
    	   return EXIT_FAILURE;
	}

        // Perform arithmetic operation
        if (strcmp(operation, "-kali") == 0) {
            result = num1 * num2;
            numberToWords(result, result_word);
            snprintf(message, sizeof(message), "hasil perkalian %s dan %s adalah %s.", num1_word, num2_word, result_word);
            writeLog("KALI", message);
        } else if (strcmp(operation, "-tambah") == 0) {
            result = num1 + num2;
            numberToWords(result, result_word);
            snprintf(message, sizeof(message), "hasil penjumlahan %s dan %s adalah %s.", num1_word, num2_word, result_word);
            writeLog("TAMBAH", message);
        } else if (strcmp(operation, "-kurang") == 0) {
            result = num1 - num2;
            if (result < 0) {
                printf("ERROR.");
                return EXIT_FAILURE;
            }
            numberToWords(result, result_word);
            snprintf(message, sizeof(message), "hasil pengurangan %s dan %s adalah %s.", num1_word, num2_word, result_word);
            writeLog("KURANG", message);
        } else if (strcmp(operation, "-bagi") == 0) {
            result = num1/num2 ;
            if (num2 == 0) {
        	printf("ERROR.");
            	return EXIT_FAILURE;
    	    }
            numberToWords(result, result_word);
            snprintf(message, sizeof(message), "hasil pembagian %s dan %s adalah %s.", num1_word, num2_word, result_word);
            writeLog("BAGI", message);
        } else {
            fprintf(stderr, "Operasi tidak valid: %s\n", operation);
            return EXIT_FAILURE;
        }

        // Write result to pipe
        write(fd[1], message, strlen(message) + 1);
        close(fd[1]); // Close write end
        exit(EXIT_SUCCESS);
    } else { // Parent process
        close(fd[1]); // Close unused write end

        wait(NULL); // Wait for child to finish

        char message[MAX_LENGTH * 2];
        // Read result from pipe
        read(fd[0], message, sizeof(message));
        close(fd[0]); // Close read end

        printf("%s\n", message);
    }

    return EXIT_SUCCESS;
}

