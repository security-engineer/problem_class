#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEPTH_SIZE 127
#define PARAM_SIZE 100
#define STRING_SIZE 12800

void test_main(char answer[STRING_SIZE], const char question[STRING_SIZE]);


static bool check_data(char answer[STRING_SIZE], const char question[STRING_SIZE]) {
	return true;
}

static char* make_rand_number_to_string(void) {
	static char str[DEPTH_SIZE + 1];
	str[0] = (rand() % 25) + 'B';
	int depth = (rand() % DEPTH_SIZE) + 1;

	for (int i = 1; i < depth; i++) {
		str[i] = (rand() % 26) + 'A';
	}
	str[depth] = 0;

	return str;
}

static void build_data(char question[STRING_SIZE]) {
	strcpy_s(question, STRING_SIZE, make_rand_number_to_string());
	int count = (rand() % (PARAM_SIZE - 1)) + 2;

	for (int i = 1; i < count; i++) {
		(rand() % 2 == 0) ? strcat_s(question, STRING_SIZE, "+") : strcat_s(question, STRING_SIZE, "-");
		strcat_s(question, STRING_SIZE, make_rand_number_to_string());
	}
}

int main(void) {
	unsigned int SCORE = 0;

	for (int i = 0; i < 100; i++) {
		char question[STRING_SIZE];
		// strcpy_s(question, 12 , "ABC+DEF-GHI");
		build_data(question);
		char answer[STRING_SIZE];
		char answer2[STRING_SIZE];
		unsigned int t = clock();
		test_main(answer, question);
		printf("%s \n", question);
		printf("Answer: %s \n", answer);
		printf("====================================== \n");
		SCORE += (clock() - t);

		if (check_data(answer, question) == false) {
			SCORE += 1000000;
		}
	}

	printf("SCORE = %d\n", SCORE);
	return 0;
}
