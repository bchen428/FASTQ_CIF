#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

void removealltabs (char *string);

int main (int argc, char *argv[]) {

  char *inputfilepath = argv[1];
  FILE *inputfile = fopen(inputfilepath, "r");
  if (!inputfile) {
    printf("Error opening %s\n", inputfilepath);
    exit(0);
  }

  u_int32_t i;

  u_int16_t index = (u_int16_t) strtoul(argv[2], NULL, 10) - 1;

  size_t line_size = 200;
  char *line = malloc(line_size * sizeof(char));

  bool notempty = false;

  u_int16_t skipcount = 0;

  while (getline(&line, &line_size, inputfile) != -1) {

    if (line[0] == '@') {
      notempty = true;
      break;
    }

    ++skipcount;

  }

  if (notempty) {
    rewind(inputfile);
  } else {
    printf("Did not detect correct file input.\n");
    exit(0);
  }

  //skip header info
  for (i = 0; i < skipcount; ++i) {
    getline(&line, &line_size, inputfile);
  }

  u_int32_t linecount = 0;
  u_int32_t acount = 0, tcount = 0, ccount = 0, gcount = 0, ncount = 0;
  int32_t aint[] = {0, 0, 0, 0}, tint[] = {0, 0, 0, 0}, cint[] = {0, 0, 0, 0}, gint[] = {0, 0, 0, 0}, nint[] = {0, 0, 0, 0};

  int8_t type;
  int16_t values[4];
  while (getline(&line, &line_size, inputfile) != -1) {

    if (line[0] == '\n'){
      continue;
    }
    ++linecount;

    if (linecount % 3 == 2) { //sequence line

      removealltabs(line);

      switch (line[index]) {
        case 'A':
        type = 0;
        ++acount;
        break;
        case 'T':
        type = 1;
        ++tcount;
        break;
        case 'C':
        type = 2;
        ++ccount;
        break;
        case 'G':
        type = 3;
        ++gcount;
        break;
        case 'N':
        type = 4;
        ++ncount;
        break;
        default:
        printf("unexpected char '%c'\n", line[index]);
        exit(0);
      }

    }

    if (linecount % 3 == 0) {
      for (i = 0; i < 4; ++i) {
        if (i == 0) {
          values[i] = (int16_t) strtoul(strtok(line, "\t"), NULL, 10);
          //printf("%u\n", values[i]);
        } else {
          values[i] = (int16_t) strtoul(strtok(NULL, "\t"), NULL, 10);
        }
      }

      if (type == 0) {
        for (i = 0; i < 4; ++i) {
          aint[i] += values[i];
        }
      } else if (type == 1) {
        for (i = 0; i < 4; ++i) {
          tint[i] += values[i];
        }
      } else if (type == 2) {
        for (i = 0; i < 4; ++i) {
          cint[i] += values[i];
        }
      } else if (type == 3) {
        for (i = 0; i < 4; ++i) {
          gint[i] += values[i];
        }
      } else if (type == 4) {
        for (i = 0; i < 4; ++i) {
          nint[i] += values[i];
        }
      } else {
        printf("error not a valid type: %hhu\n", type);
        exit(0);
      }
    }

  }

  printf("A: count: %u\n", acount);
  printf("%.2f\t%.2f\t%.2f\t%.2f\n", (float) aint[0] / (float) acount, (float) aint[1] / (float) acount,(float) aint[2] / (float) acount,(float) aint[3] / (float) acount);
  printf("T: count: %u\n", tcount);
  printf("%.2f\t%.2f\t%.2f\t%.2f\n", (float) tint[0] / (float) tcount, (float) tint[1] / (float) tcount,(float) tint[2] / (float) tcount,(float) tint[3] / (float) tcount);
  printf("C: count: %u\n", ccount);
  printf("%.2f\t%.2f\t%.2f\t%.2f\n", (float) cint[0] / (float) ccount, (float) cint[1] / (float) ccount,(float) cint[2] / (float) ccount,(float) cint[3] / (float) ccount);
  printf("G: count: %u\n", gcount);
  printf("%.2f\t%.2f\t%.2f\t%.2f\n", (float) gint[0] / (float) gcount, (float) gint[1] / (float) gcount,(float) gint[2] / (float) gcount,(float) gint[3] / (float) gcount);
  printf("N: count: %u\n", ncount);
  printf("%.2f\t%.2f\t%.2f\t%.2f\n", (float) nint[0] / (float) ncount, (float) nint[1] / (float) ncount,(float) nint[2] / (float) ncount,(float) nint[3] / (float) ncount);

  free(line);

  fclose(inputfile);

  return 1;
}

void removealltabs (char *string) {

  u_int16_t stringlength = strlen(string);
  char temp[stringlength + 1];
  u_int16_t j = 0;

  for (u_int16_t i = 0; i < stringlength; i++) {

    if (string[i] != '\t') {
      temp[j] = string[i];
      ++j;
    }

  }

  temp[j] = '\0';

  size_t temp_size = strlen(temp) + 1;

  snprintf(string, temp_size, "%s", temp);

}
