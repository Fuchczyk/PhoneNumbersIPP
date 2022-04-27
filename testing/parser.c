#include "../src/phone_forward.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

static unsigned long line = 0;

void my_assertion(const char *num, const char *num2, char *BUF1) {
  if(strcmp(num, num2) != 0) {
    printf("ASSERTION FAILED WITH: NUM1 %s NUM2 %s AT LINE %lu (BUFOR %s)\n", num, num2, line, BUF1);

    assert(false);
  } 
}

int main(void) {
  /**
   * PARSER DO JĘZYKA PROGRAMOWANIA
   * ADD (NUMER1) (NUMER2) -> phfwdAdd(pf, num1, num2)
   * REMOVE (NUMER1) -> phfwdRemove(pf, num)
   * GET (NUMER) (RESULT) 
   */
  PhoneForward *pf = phfwdNew();
  char *BUFOR1 = malloc(sizeof(char) * 100001);
  char *BUFOR2 = malloc(sizeof(char) * 100001);
  while (scanf("%s", BUFOR1) != -1) {
    if (line % 10000 == 0) {
      printf("PRZETWARZANA LINIA: %lu\n", line);
    }

    line++;
    if (strcmp(BUFOR1, "ADD") == 0) {
      scanf("%s", BUFOR1);
      scanf("%s", BUFOR2);
      phfwdAdd(pf, BUFOR1, BUFOR2);
    } else if (strcmp(BUFOR1, "REMOVE") == 0) {
      scanf("%s", BUFOR1);
      phfwdRemove(pf, BUFOR1);
    } else {
      scanf("%s", BUFOR1);
      scanf("%s", BUFOR2);
      PhoneNumbers *ph = phfwdGet(pf, BUFOR1);
      my_assertion(phnumGet(ph, 0), BUFOR2, BUFOR1);
      phnumDelete(ph);
    }
  }

  printf("POMYŚLNIE PRZESZŁO TESTY.\n");
  phfwdDelete(pf);

  free(BUFOR1);
  free(BUFOR2);
  return 0;
}