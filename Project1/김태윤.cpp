#define STRING_SIZE 12800
#define MAX_DIGITS   256

// 1) 문자열 길이 구하기 (널문자 사용)
int str_len(const char* s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

// 2) 26진수 문자열 -> big-int (LSB first) 변환
//    digits 배열에 0~25 값들, len 반환
void parse_alpha26(const char* s, int digits[], int* len) {
    // 초기화: 0
    digits[0] = 0;
    *len = 1;
    int i = 0;
    while (s[i]) {
        int val = s[i] - 'A';  // 0..25
        // 곱하기 26
        int carry = 0;
        for (int j = 0; j < *len; j++) {
            int prod = digits[j] * 26 + carry;
            digits[j] = prod % 26;
            carry = prod / 26; // 26 이상이면 carry가 일어난다.
        }
        while (carry) {
            digits[*len] = carry % 26;
            carry /= 26;
            (*len)++;
        }
        // 더하기 val
        int idx = 0;
        digits[0] += val;
        while (digits[idx] >= 26) {
            digits[idx] -= 26;
            idx++;
            if (idx < *len) digits[idx]++; else { digits[idx] = 1; (*len)++; }
        }
        i++;
    }
}

// 3) big-int 비교: A>B?1, A==B?0, A<B?-1
int cmp_alpha26(const int A[], int lenA, const int B[], int lenB) {
    if (lenA != lenB) return lenA > lenB ? 1 : -1;
    for (int i = lenA - 1; i >= 0; i--) {
        if (A[i] != B[i]) return A[i] > B[i] ? 1 : -1;
    }
    return 0;
}

// 4) big-int 덧셈 (C = A + B)
void add_alpha26(const int A[], int lenA, const int B[], int lenB, int C[], int* lenC) {
    int carry = 0;
    int n = lenA > lenB ? lenA : lenB;
    int i;
    for (i = 0; i < n; i++) {
        int a = i < lenA ? A[i] : 0;
        int b = i < lenB ? B[i] : 0;
        int sum = a + b + carry;
        C[i] = sum % 26;
        carry = sum / 26;
    }
    while (carry) {
        C[i] = carry % 26;
        carry /= 26;
        i++;
    }
    *lenC = i;
}

// 5) big-int 뺄셈 (전제: A >= B)
void sub_big(const int A[], int lenA, const int B[], int lenB, int C[], int* lenC) {
    int borrow = 0;
    int i;
    for (i = 0; i < lenA; i++) {
        int a = A[i] - borrow;
        int b = i < lenB ? B[i] : 0;
        if (a < b) {
            a += 26;
            borrow = 1;
        }
        else {
            borrow = 0;
        }
        C[i] = a - b;
    }
    // 앞자리 0(trim)
    while (i > 1 && C[i - 1] == 0) i--;
    *lenC = i;
}

// 6) big-int -> 26진수 문자열 (MSB first)
void big_to_str(const int digits[], int len, char* s) {
    int i = 0;
    // 앞자리 0 제거
    while (len > 1 && digits[len - 1] == 0) len--;
    for (int j = len - 1; j >= 0; j--) {
        s[i++] = 'A' + digits[j];
    }
    s[i] = 0;
}

// 7) test_main: question 파싱하며 즉시 계산
void test_main(char answer[STRING_SIZE], const char question[STRING_SIZE]) {
    int acc_d[MAX_DIGITS], acc_len;
    int term_d[MAX_DIGITS], term_len;
    int tmp_d[MAX_DIGITS], tmp_len;
    char term_str[128];
    int qi = 0, ti;
    char op;
    int accNeg = 0;

    // 첫 숫자 파싱
    ti = 0;
    while (question[qi] && question[qi] != '+' && question[qi] != '-') {
        term_str[ti++] = question[qi++];
    }
    term_str[ti] = 0;
    parse_big(term_str, acc_d, &acc_len);
    accNeg = 0;

    // 나머지 연산
    while (question[qi]) {
        op = question[qi++];
        // 다음 숫자 파싱
        ti = 0;
        while (question[qi] && question[qi] != '+' && question[qi] != '-') {
            term_str[ti++] = question[qi++];
        }
        term_str[ti] = 0;
        parse_big(term_str, term_d, &term_len);

        if (op == '+') {
            if (!accNeg) {
                add_big(acc_d, acc_len, term_d, term_len, tmp_d, &tmp_len);
                accNeg = 0;
            }
            else {
                // (-acc) + term = term - acc
                if (cmp_big(term_d, term_len, acc_d, acc_len) >= 0) {
                    sub_big(term_d, term_len, acc_d, acc_len, tmp_d, &tmp_len);
                    accNeg = 0;
                }
                else {
                    sub_big(acc_d, acc_len, term_d, term_len, tmp_d, &tmp_len);
                    accNeg = 1;
                }
            }
        }
        else {
            if (!accNeg) {
                // acc - term
                if (cmp_big(acc_d, acc_len, term_d, term_len) >= 0) {
                    sub_big(acc_d, acc_len, term_d, term_len, tmp_d, &tmp_len);
                    accNeg = 0;
                }
                else {
                    sub_big(term_d, term_len, acc_d, acc_len, tmp_d, &tmp_len);
                    accNeg = 1;
                }
            }
            else {
                // (-acc) - term = -(acc + term)
                add_big(acc_d, acc_len, term_d, term_len, tmp_d, &tmp_len);
                accNeg = 1;
            }
        }
        // 누적 반영
        acc_len = tmp_len;
        for (int i = 0; i < acc_len; i++) acc_d[i] = tmp_d[i];
    }

    // 결과 문자열화
    char out[256];
    big_to_str(acc_d, acc_len, out);
    int ai = 0;
    if (accNeg) answer[ai++] = '-';
    for (int i = 0; out[i]; i++) answer[ai++] = out[i];
    answer[ai] = 0;
}

///////////////////////////////////////////////////////////////////

//#define STRING_SIZE 12800
//#define MAX_DIGITS   256
//
//typedef struct { int d[MAX_DIGITS]; int len; } Big;
//
//// 문자열 길이
//static int str_len(const char* s) {
//    int i = 0;
//    while (s[i] != '\0') i++;
//    return i;
//}
//
//// 문자열 → Big (LSB first)
//static void parse_big(const char* s, Big* x) {
//    x->len = 1;
//    x->d[0] = 0;
//    for (int i = 0; s[i] != '\0'; i++) {
//        int carry = 0;
//        for (int j = 0; j < x->len; j++) {
//            int v = x->d[j] * 26 + carry;
//            x->d[j] = v % 26;
//            carry = v / 26;
//        }
//        while (carry) {
//            x->d[x->len] = carry % 26;
//            carry /= 26;
//            x->len++;
//        }
//        int add = s[i] - 'A';
//        for (int j = 0; add != 0; j++) {
//            int v = x->d[j] + add;
//            x->d[j] = v % 26;
//            add = v / 26;
//        }
//    }
//}
//
//// Big 비교
//static int cmp_big(const Big* a, const Big* b) {
//    if (a->len != b->len) return (a->len > b->len) ? 1 : -1;
//    for (int i = a->len - 1; i >= 0; i--) {
//        if (a->d[i] != b->d[i]) return (a->d[i] > b->d[i]) ? 1 : -1;
//    }
//    return 0;
//}
//
//// 덧셈 C = A + B
//static void add_big(const Big* a, const Big* b, Big* c) {
//    int carry = 0;
//    int n = (a->len > b->len) ? a->len : b->len;
//    int i;
//    for (i = 0; i < n; i++) {
//        int v = ((i < a->len) ? a->d[i] : 0)
//            + ((i < b->len) ? b->d[i] : 0)
//            + carry;
//        c->d[i] = v % 26;
//        carry = v / 26;
//    }
//    while (carry != 0) {
//        c->d[i] = carry % 26;
//        carry /= 26;
//        i++;
//    }
//    c->len = i;
//}
//
//// 뺄셈 C = A - B (전제: A >= B)
//static void sub_big(const Big* a, const Big* b, Big* c) {
//    int borrow = 0;
//    int i;
//    for (i = 0; i < a->len; i++) {
//        int v = a->d[i] - ((i < b->len) ? b->d[i] : 0) - borrow;
//        if (v < 0) {
//            v += 26;
//            borrow = 1;
//        }
//        else {
//            borrow = 0;
//        }
//        c->d[i] = v;
//    }
//    while (i > 1 && c->d[i - 1] == 0) i--;
//    c->len = i;
//}
//
//// Big → 문자열
//static void big_to_str(const Big* x, char* s) {
//    int k = 0;
//    for (int i = x->len - 1; i >= 0; i--) {
//        s[k++] = 'A' + x->d[i];
//    }
//    s[k] = '\0';
//}
//
//// 즉시 계산
//void test_main(char answer[STRING_SIZE], const char question[STRING_SIZE]) {
//    Big acc, term, tmp;
//    int accNeg = 0;
//    char buf[128];
//    int qi = 0;
//    char op;
//
//    // 첫 숫자 파싱
//    int bi = 0;
//    while (question[qi] != '\0' && question[qi] != '+' && question[qi] != '-') {
//        buf[bi++] = question[qi++];
//    }
//    buf[bi] = '\0';
//    parse_big(buf, &acc);
//
//    // 연산자+피연산자 반복
//    while (question[qi] != '\0') {
//        op = question[qi++];
//        bi = 0;
//        while (question[qi] != '\0' && question[qi] != '+' && question[qi] != '-') {
//            buf[bi++] = question[qi++];
//        }
//        buf[bi] = '\0';
//        parse_big(buf, &term);
//
//        if (op == '+') {
//            if (accNeg == 0) {
//                add_big(&acc, &term, &tmp);
//                accNeg = 0;
//            }
//            else if (cmp_big(&term, &acc) >= 0) {
//                sub_big(&term, &acc, &tmp);
//                accNeg = 0;
//            }
//            else {
//                sub_big(&acc, &term, &tmp);
//                accNeg = 1;
//            }
//        }
//        else {
//            if (accNeg == 0) {
//                if (cmp_big(&acc, &term) >= 0) {
//                    sub_big(&acc, &term, &tmp);
//                    accNeg = 0;
//                }
//                else {
//                    sub_big(&term, &acc, &tmp);
//                    accNeg = 1;
//                }
//            }
//            else {
//                add_big(&acc, &term, &tmp);
//                accNeg = 1;
//            }
//        }
//
//        /* copy tmp into acc manually */
//        acc.len = tmp.len;
//        for (int ci = 0; ci < acc.len; ci++) acc.d[ci] = tmp.d[ci];
//
//    }
//
//    // 결과 문자열화
//    int idx = 0;
//    if (accNeg) {
//        answer[idx++] = '-';
//    }
//    big_to_str(&acc, answer + idx);
//}