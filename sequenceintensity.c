#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<dirent.h>
#include<stdbool.h>

int64_t* findindices (u_int16_t *arr1, u_int64_t length, u_int16_t target);
void swap_int16 (int16_t* a, int16_t* b);
void swap_uint16 (u_int16_t* a, u_int16_t* b);
int32_t partition (u_int16_t** arr1, int16_t** arr2, int32_t l, int32_t h);
void quickSortIterative (u_int16_t** arr1, int16_t** arr2, int32_t l, int32_t h);

int main (int argc, char* argv[]) {

  u_int32_t i, j, k;

  //open sequence file
  char *sequencefilepath = argv[1];
  FILE *sequencefile = fopen(sequencefilepath, "r");
  if (!sequencefile) {
    printf("Error opening '%s'.\n", sequencefilepath);
    exit(0);
  }

  //open coordinate file
  char *coordsfilepath = argv[2];
  FILE *coordsfile = fopen(coordsfilepath, "r");
  if (!coordsfile) {
    printf("Error opening '%s'.\n", coordsfilepath);
    exit(0);
  }

  //prepare to open locs directory
  char *locsdirectorypath = argv[3];

  size_t locsfilepath_size = strlen(locsdirectorypath) + 200;
  char *locsfilepath = malloc(locsfilepath_size * sizeof(char));
  locsfilepath[0] = '\0';

  struct dirent *ent;
  DIR *locsdir;
  FILE *locsfile;

  u_int8_t filenamelength;
  u_int8_t longestfilenamelength = 0;
  size_t locscount = 0;

  //allocate some memory to store locs info
  size_t locsnames_size;
  char **locsnames;
  u_int32_t *locslengths;
  u_int16_t ***locs;
  size_t dimensions = 2;
  size_t coordsperdimension;

  u_int32_t *four;
  float *ffour;

  //open locs directory
  if (((locsdir = opendir(locsdirectorypath)) != NULL)) {

    //this loop gets the length of longest locs file name and the count of locs files
    while (((ent = readdir (locsdir)) != NULL)) {

      if (ent->d_type == 8) {

        filenamelength = strlen(ent->d_name);

        if (filenamelength > 5 && strcmp(ent->d_name + filenamelength - 5, ".locs") == 0) {

          ++locscount;

          if (filenamelength > longestfilenamelength) {

            longestfilenamelength = filenamelength;

          }

        }

      }

    }

    //reset locsdir
    rewinddir(locsdir);

    //allocate some memory to store locs info
    locsnames_size = longestfilenamelength - 4; // - '.locs' + '\0'
    locsnames = malloc(locscount * sizeof(char*));
    for (i = 0; i < locscount; ++i) {
      locsnames[i] = malloc(locsnames_size * sizeof(char*));
    }

    locslengths = malloc(locscount * sizeof(u_int32_t));

    locs = malloc(locscount * sizeof(u_int16_t**));
    dimensions = 2;
    for (i = 0; i < locscount; ++i) {
      locs[i] = malloc(dimensions * sizeof(u_int16_t*));
    }

    four = malloc(sizeof(u_int32_t));
    ffour = malloc(sizeof(float));

    //reset this for next loop
    i = 0;

    //this loop fills in the locs file names, the number of coords per file, and the individual coordinate values
    while (((ent = readdir (locsdir)) != NULL)) {

      if (ent->d_type == 8) {

        filenamelength = strlen(ent->d_name);

        if (filenamelength > 5 && strcmp(ent->d_name + filenamelength - 5, ".locs") == 0) {

          snprintf(locsfilepath, locsfilepath_size, "%s/%s", locsdirectorypath, ent->d_name);
          locsfile = fopen(locsfilepath, "rb");
          if (!locsfile) {
            printf("Error opening '%s'.\n", locsfilepath);
            exit(0);
          }

          //store name
          snprintf(locsnames[i], filenamelength - 4, "%s", ent->d_name); //copy except the '.locs' at end, -4 since need 1 spot for '\0'

          //store number of coords
          fread(four, sizeof(u_int32_t), 1, locsfile); //skip, should just be "1"
          fread(ffour, sizeof(float), 1, locsfile); //skip, should be just be "1"
          fread(four, sizeof(u_int32_t), 1, locsfile); //note that number of coords = dimensions * this value

          locslengths[i] = *four;

          //finish allocating memory
          coordsperdimension = (size_t) locslengths[i];
          for (j = 0; j < dimensions; ++j) {
            locs[i][j] = malloc(coordsperdimension * sizeof(u_int16_t));
          }

          //reset
          j = 0;
          k = 0;

          //store coords themselves
          while (fread(ffour, sizeof(float), 1, locsfile) == 1) {

            if (k > coordsperdimension) {

              printf("An error occurred when reading '%s' into memory. There appear to be more bytes than expected.\n", locsfilepath);
              printf("k = %u\ncoordsperdimension = %lu\n", k, coordsperdimension);
              exit(0);

            }

            locs[i][j][k] = (u_int16_t) roundf((10 * *ffour) + 1000);

            if (j == 1) {
              ++k;
              j = 0;
            } else if (j == 0) {
              j = 1;
            } else {
              printf("This should never be reachable. J value was neither 0 nor 1.\n");
              exit(0);
            }

            //j = j ? 0 : 1; //set 1 if 0, 0 if 1

          }

          //close file
          fclose(locsfile);

          //confirm that locs file was loaded properly
          if (k == coordsperdimension) {

            printf("Loaded '%s' into memory.\n", locsfilepath);

          } else {

            printf("POSSIBLE ERROR: '%s' had less coordinates (%u) than the value specified by its header (%u).\n", locsfilepath, k, locslengths[i]);

          }

          //increment and reset
          ++i;

        }

      }

    }

  } else {

    printf("Error opening directory: %s\n", locsdirectorypath);
    exit(0);

  }

  //close directory
  closedir(locsdir);

  printf("Finished loading .locs files.\n");

  /*print locs values*/
  /*
  for (i = 0; i < locscount; ++i) {
    printf("%s\t# of Coordinate Pairs: %u\n", locsnames[i], locslengths[i]);
  }
  */
  /*
  for (i = 0; i < locscount; ++i) {

    for (k = 0; k < locslengths[i]; ++k) {

      printf("%hu:%hu\n", locs[i][0][k], locs[i][1][k]);

    }

  }
  */

  size_t cifdirectorypath_size = 5 + strlen(argv[4]) + strlen(argv[5]); //structure is PATH/C??.1 where PATH is argv4 and ?? is argv5
  char *cifdirectorypath = malloc(cifdirectorypath_size * sizeof(char));
  snprintf(cifdirectorypath, cifdirectorypath_size, "%s/C%s.1", argv[4], argv[5]);

  DIR *cifdir;
  FILE *ciffile;

  size_t channels = 4;
  size_t cifcount = 0;
  char *currentcifname = malloc(locsnames_size * sizeof(char));
  size_t ciffilepath_size = cifdirectorypath_size + 200;
  char *ciffilepath = malloc(ciffilepath_size * sizeof(char));

  int16_t ***cif;
  cif = malloc(locscount * sizeof(int16_t**));
  for (i = 0; i < locscount; ++i) {
    cif[i] = malloc(channels * sizeof(int16_t*));
  }
  u_int8_t *one = malloc(sizeof(u_int8_t));
  u_int16_t *two = malloc(sizeof(u_int16_t));
  int16_t *two_signed = malloc(sizeof(int16_t));
  u_int32_t intensitycount;
  bool ciffound;

  //open cif directory
  if (((cifdir = opendir(cifdirectorypath)) != NULL)) {

    //confirms that the cif file has the same names/values as the locs files
    while (((ent = readdir (cifdir)) != NULL)) {

      if (ent->d_type == 8) {

        filenamelength = strlen(ent->d_name);

        if (filenamelength > 4 && strcmp(ent->d_name + filenamelength - 4, ".cif") == 0) {

          ++cifcount;

          snprintf(ciffilepath, ciffilepath_size, "%s/%s", cifdirectorypath, ent->d_name);
          snprintf(currentcifname, filenamelength - 3, "%s", ent->d_name);

          ciffound = false;

          for (i = 0; i < locscount; ++i) {

            if (strcmp(currentcifname, locsnames[i]) == 0) {

              ciffound = true;
              break;

            }

          }

          if (!ciffound) {

            printf("Error: '%s' does not match any of the names of the .locs files. Please check your input.\n", currentcifname);
            printf("Full filepath: %s\n", ciffilepath);
            exit(0);

          }

          ciffile = fopen(ciffilepath, "rb");
          if (!ciffile) {
            printf("Error opening '%s'.\n", ciffilepath);
            exit(0);
          }

          //header bytes
          fread(one, sizeof(u_int8_t), 1, ciffile); //C
          fread(one, sizeof(u_int8_t), 1, ciffile); //I
          fread(one, sizeof(u_int8_t), 1, ciffile); //F
          fread(one, sizeof(u_int8_t), 1, ciffile); //version number
          fread(one, sizeof(u_int8_t), 1, ciffile); //size of intensities (bytes)
          fread(two, sizeof(u_int16_t), 1, ciffile); //first cycle number

          if (*two != (u_int16_t) strtoul(argv[5], NULL, 10)) {
            printf("ERROR: The header of cif file '%s.cif' indicated a different starting cycle number (%hu) than expected (%s).\n", currentcifname, *two, argv[5]);
            printf("Full filepath (cif): %s", ciffilepath);
            exit(0);
          }

          fread(two, sizeof(u_int16_t), 1, ciffile); //total number of cycles
          fread(four, sizeof(u_int32_t), 1, ciffile); //number of clusters

          if (*four != locslengths[i]) {

            printf("ERROR: The header of cif file '%s.cif' indicated a different number of clusters than the header of locs file '%s.locs'.\n", currentcifname, locsnames[i]);
            printf("Full filepath (cif): %s", ciffilepath);
            exit(0);

          }

          //allocate memory
          for (j = 0; j < channels; ++j) {
            cif[i][j] = malloc(((size_t) *four) * sizeof(int16_t));
          }

          intensitycount = 0;
          j = 0;
          k = 0;

          while (fread(two_signed, sizeof(int16_t), 1, ciffile) == 1 && intensitycount < channels * locslengths[i]) {
            cif[i][j][k] = *two_signed;

            ++k;

            if (k >= locslengths[i]) {

              intensitycount += k; //this is necessary to avoid the padded bytes in .cif files
              ++j;
              k = 0;

            }

          }

          fclose(ciffile);

          printf("Loaded '%s' into memory.\n", ciffilepath);

        }

      }

    }

    if (cifcount != locscount) {

      printf("ERROR: We detected %lu cif files and %lu locs files. The ratio between the two types of files should be 1:1.\n", cifcount, locscount);
      exit(0);

    }

  } else {
    printf("Error opening directory: %s\n", cifdirectorypath);
    exit(0);
  }

  //close directory
  closedir(cifdir);

  //deallocate
  free(cifdirectorypath);
  free(ciffilepath);
  free(currentcifname);

  /*to improve runtime... perhaps a sorting step?*/
  /*sort all of the locs/cif arrays simultaneously based on locs[][0]*/
  for (i = 0; i < locscount; ++i) {
    quickSortIterative(locs[i], cif[i], 0, locslengths[i] - 1);
  }


  /*begin pairing sequence to intensity*/
  size_t coords_size = 200;
  char *coordsline = malloc(coords_size * sizeof(char));
  size_t sequence_size = 200;
  char *sequenceline = malloc(sequence_size * sizeof(char));

  char *coordsdelimiter = ":";
  char *coordsdelimiteraftery = " ";
  char *readnumber;
  char *tilenumber;
  u_int16_t x;
  u_int16_t y;
  int64_t *x_indices;

  char *coordslocsname = malloc(locsnames_size * sizeof(char));

  int16_t nchar;

  bool locsfound;

  while (getline(&coordsline, &coords_size, coordsfile) != -1) {

    nchar = getline(&sequenceline, &sequence_size, sequencefile);
    if (nchar == -1) {
      printf("Error: '%s' has less sequences than '%s' has coordinates.\n", sequencefilepath, coordsfilepath);
      exit(0);
    }

    printf("%s", coordsline);
    printf("%s", sequenceline);

    strtok(coordsline, coordsdelimiter);
    strtok(NULL, coordsdelimiter);
    strtok(NULL, coordsdelimiter);
    readnumber = strtok(NULL, coordsdelimiter);
    tilenumber = strtok(NULL, coordsdelimiter);
    snprintf(coordslocsname, locsnames_size, "s_%s_%s\n", readnumber, tilenumber);
    x = (u_int16_t) strtoul(strtok(NULL, coordsdelimiter), NULL, 10);
    y = (u_int16_t) strtoul(strtok(NULL, coordsdelimiteraftery), NULL, 10);

    locsfound = false;

    for (i = 0; i < dimensions; ++i) {

      if (strcmp(coordslocsname, locsnames[i]) == 0) {

        locsfound = true;
        break;

      }

    }

    if (!locsfound) {
      printf("Unable to find specified location file: %s.locs\n", coordslocsname);
      exit(0);
    }

    locsfound = false;

    x_indices = findindices(locs[i][0], locslengths[i], x);

    if (x_indices[0] == -1 || x_indices[1] == -1) {
      printf("Error, detected that for %s where x = %hu, start: %ld\tend:%ld\n", coordslocsname, x, x_indices[0], x_indices[1]);
    }

    for (k = x_indices[0]; k < x_indices[1] + 1; ++k) {

      if (x == locs[i][0][k] && y == locs[i][1][k]) {

        printf("%hd\t%hd\t%hd\t%hd\n\n", cif[i][0][k], cif[i][1][k], cif[i][2][k], cif[i][3][k]);

        locsfound = true;
        break;
        
      }

    }

    if (!locsfound) {
      printf("ERROR: Unable to find specified coordinates File: %s\tX: %u\tY: %u\n", coordslocsname, x, y);
    }

  }

  //deallocate
  for (i = 0; i < locscount; ++i) {

    for (j = 0; j < dimensions; ++j) {

      free(locs[i][j]);

    }

    for (j = 0; j < channels; ++j) {

      free(cif[i][j]);

    }

    free(locs[i]);
    free(cif[i]);
    free(locsnames[i]);

  }

  free(cif);
  free(locs);
  free(locsnames);
  free(locslengths);
  free(one);
  free(two);
  free(two_signed);
  free(four);
  free(ffour);
  free(locsfilepath);
  free(coordsline);
  free(sequenceline);
  free(coordslocsname);

  //close files
  fclose(sequencefile);
  fclose(coordsfile);

  return 1;
}

int64_t* findindices (u_int16_t *arr1, u_int64_t length, u_int16_t target) {

  //binary search algorithm to find start and end indices of target value
  //returns -1,-1 if not found

  int64_t *x = (int64_t[2]){-1, -1};

  int64_t l = 0;
  int64_t r = length - 1;
  u_int64_t index;

  //binary search for lower index
  while (l <= r) {
    index = (l + r) / 2;
    if (arr1[index] > target) {
      r = index - 1;
    } else if (arr1[index] < target){
      l = index + 1;
    } else {
      x[0] = index;
      r = index - 1;
    }
  }

  l = 0;
  r = length - 1;

  //binary search for higher index
  while (l <= r) {
    index = (l + r) / 2;
    if (arr1[index] > target) {
      r = index - 1;
    } else if (arr1[index] < target) {
      l = index + 1;
    } else {
      x[1] = index;
      l = index + 1;
    }
  }

  return x;
}

/*modified iterative Quicksort Algorithm, copied from GeeksToGeeks and modified to work for our needs*/
void swap_int16 (int16_t* a, int16_t* b) {
    int16_t t = *a;
    *a = *b;
    *b = t;
}

void swap_uint16 (u_int16_t* a, u_int16_t* b) {
    u_int16_t t = *a;
    *a = *b;
    *b = t;
}

int32_t partition(u_int16_t** arr1, int16_t** arr2, int32_t l, int32_t h) {
    u_int16_t x = arr1[0][h];
    int32_t i = (l - 1);
    int32_t j, k;

    for (j = l; j <= h - 1; j++) {
        if (arr1[0][j] <= x) {
            i++;
            for (k = 0; k < 2; ++k) {
              swap_uint16(&arr1[k][i], &arr1[k][j]);
            }
            for (k = 0; k < 4; ++k) {
              swap_int16(&arr2[k][i], &arr2[k][j]);
            }
        }
    }
    for (k = 0; k < 2; ++k) {
      swap_uint16(&arr1[k][i + 1], &arr1[k][h]);
    }
    for (k = 0; k < 4; ++k) {
      swap_int16(&arr2[k][i + 1], &arr2[k][h]);
    }
    return (i + 1);
}

void quickSortIterative (u_int16_t** arr1, int16_t** arr2, int32_t l, int32_t h) {
    // Create an auxiliary stack
    int32_t stack[h - l + 1];

    // initialize top of stack
    int32_t top = -1;

    // push initial values of l and h to stack
    stack[++top] = l;
    stack[++top] = h;

    // Keep popping from stack while is not empty
    while (top >= 0) {
        // Pop h and l
        h = stack[top--];
        l = stack[top--];

        // Set pivot element at its correct position
        // in sorted array
        int32_t p = partition(arr1, arr2, l, h);

        // If there are elements on left side of pivot,
        // then push left side to stack
        if (p - 1 > l) {
            stack[++top] = l;
            stack[++top] = p - 1;
        }

        // If there are elements on right side of pivot,
        // then push right side to stack
        if (p + 1 < h) {
            stack[++top] = p + 1;
            stack[++top] = h;
        }
    }
}
