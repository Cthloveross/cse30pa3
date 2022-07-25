/*
 * CSE30 Summer Session 1 '22 HW3
 * CSE30 username: cs30su122xxx (TODO: Fill in)
 */

#include "poll_lookup.h"

/*
 * !!! DO NOT EDIT THIS FUNCTION !!!
 * main
 *
 * Arguments: argc, argv
 *
 * Operation: Main driver for the program, calls other funttions to:
 *            parse the options, allocate the hash table, load the table, print
 *out the table stats
 *            and make print population stats of the desired city/state
 * Returns:   EXIT_SUCCESS if all ok, EXIT_FAILURE otherwise
 * !!! DO NOT EDIT THIS FUNCTION !!!
 */
int main(int argc, char *argv[]) {
  node **table;
  unsigned long size = TABLE_SIZE;
  // name of csv file
  char *filename;
  int info = 0;

  // Indicates days we want stats for/to remove
  char *date = NULL;
  char *del_date = NULL;

  // Parse options
  if (!parse_opts(argc, argv, &filename, &size, &info, &date, &del_date)) {
    return EXIT_FAILURE;
  }

  // Allocate space for table
  if ((table = calloc(size, sizeof(node *))) == NULL) {
    fprintf(stderr, "%s: Unable to allocate space for hash table\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Load records from file
  if (load_table(table, size, filename)) {
    return EXIT_FAILURE;
  }

  // Delete data first
  if (del_date) {
    char *stripped_date = strip_date(del_date);
    if (stripped_date) { // no malloc fail
      delete_date(table, size, stripped_date);
      free(stripped_date);
    } else {
      return EXIT_FAILURE;
    }
  }

  // Produce data for a single date
  if (date) {
    char *stripped_date = strip_date(date);
    if (stripped_date) { // no malloc fail
      print_date_stats(table, size, stripped_date);
      free(stripped_date);
    } else {
      return EXIT_FAILURE;
    }
  }

  // Print metadata
  if (info)
    print_info(table, size);

  // Clean up
  delete_table(table, size);

  return EXIT_SUCCESS;
}

/*
 * !!! DO NOT EDIT THIS FUNCTION !!!
 * hash
 *
 * Arguments: a null terminated string
 *
 * Operation: calculates a hash value for the string
 *
 * returns:   the hash value
 * !!! DO NOT EDIT THIS FUNCTION !!!
 */
unsigned long hash(char *str) {
  unsigned long hash = 0;
  unsigned int c;
#ifdef C_HASH
  while ((c = (unsigned char)*str++) != '\0') {
    hash = c + (hash << 6) + (hash << 16) - hash;
  }
#else
  while ((c = (unsigned char)*str++) != '\0') {
    hash = hashFun((unsigned long)c, hash);
  }
#endif
  return hash;
}

/*
 * add_node
 *
 * Arguments: linked list pointer head, year, month, day, hour, pm25, temp
 */
node *add_node(node *front, int year, int month, int day, int hour, int pm25,
    int temp) {
  // TODO: Implement add_node
   node *newNode = malloc(sizeof(node));
          newNode->year = year;
          newNode->month = month;
          newNode->day = day;
          newNode->hour = hour;
          newNode->pm25 = pm25;
          newNode->temp = temp;
          newNode -> next = NULL;
          return newNode;
        if( newNode == NULL){
                return NULL;
        }

        if(front == NULL){
                front = newNode;
                return newNode;
        }
        else{
                node *lastNode = front;
                while(lastNode->next != NULL){
                        lastNode = lastNode->next;
                }
                lastNode->next = newNode;
                return front;
        }
}

/*
 * print_date_stats
 * Print the average stats for this date
 *
 * Arguments: pointer to hash table, hash table size, date as a string in the
 *form YYYY-MM-DD
 */
void print_date_stats(node **table, unsigned long size, char *datestr) {
  // TODO: Implement print_data_stats
  int hashvalue = hash(datestr)%size;
          node *front = table[hashvalue];
          int count = 0;
          int mininumpm = INT_MAX;
          int mininumtemp = INT_MAX;
          int maximumpm = INT_MIN;
          int maximumtemp = INT_MIN;
          int averagepm = 0;
          int averagetemp = 0;
          int number = 0;
          char delim[] = "-";
          char *token = strtok(datestr, delim);
         int yearcount = atoi(token);
          token = strtok(datestr,delim);
         int  monthcount =atoi( token);
          token = strtok(datestr, delim);
          int daycount = atoi(token);
         while(front != NULL){
                 if(front->year == yearcount && front->month == monthcount && front->day == daycount){
                         number++;
                         averagepm += front->pm25;
                         averagetemp += front->temp;
                         if(front->pm25 < mininumpm){
                                 mininumpm = front->pm25;
                         }
                         if(front->pm25 > maximumpm){
                                 maximumpm = front->pm25;
                            }
                         if(front->temp < mininumtemp){
                                 mininumtemp =  front->temp;
                         }
                         if(front->temp > maximumtemp){
                                 maximumtemp = front->temp;
                         }
                         front = front->next;
                 }
                else{
                        front = front->next;
                }
         }
                 if(number == 0){
                 printf("Unable to find any data for the date %s.\n", datestr);
                 }
                 else{
                         averagepm = averagepm/number;
                         averagetemp = averagetemp/number;
        printf("Minimum pm2.5: %d\tMaximum pm2.5: %d\tAverage pm2.5: %d\n",
         mininumpm, maximumpm, averagepm);
        printf("Minimum temp: %d\tMaximum temp: %d\tAverage temp: %d\n",
         mininumtemp, maximumtemp,averagetemp);
                 }
}

/*
 * load_table
 * Allocate memory for the hash table of node*s
 *
 * Arguments: pointer to hash table, hash table size, file name
 */
int load_table(node **table, unsigned long size, char *filename) {
    FILE* fp;
        char *buf;
        int count = 0;
        const char split[] = "-";
        int cols[COL_TEMP+1];
        int c = 0;
        int year;
        int  month;
        int day;
        int hour;
        int pm25;
        int temp;
        int nothing = "NA";
        int *not = nothing;
       if(!( buf = malloc(sizeof(char*) * LINE_SIZE-1))){
        perror("load_table filename open");
        return 1;
       }
       else{
            buf = malloc(sizeof(char*)*LINE_SIZE-1);
        }
           fp = fopen(filename, "r");
           if(fp == NULL){
                   perror("load_table filename open");
                   return 1;
           }
           while(fp != NULL){
           fgets(buf, LINE_SIZE-1, fp);
           char *token = strtok(buf, split);
              if(token == "NA"){
                   year = 0;
           }
           else{
                   year = atoi(token);
           }
           token = strtok(NULL, split);
           if(token == "NA"){
                   month = 0;
           }
           else{
                   month = atoi(token);
           }
           token = strtok(NULL,split);
        if(token == "NA"){
                day = 0;
        }
        else{
                day = atoi(token);
        }
        token = strtok(NULL,split);
         if(token == "NA"){
                hour = 0;
         }
        else{
                hour = atoi(token);
        }
        token = strtok(NULL,split);
        if(token == "NA"){
                pm25 = 0;
        }
        else{
                pm25 = atoi(token);
        }
        token = strtok(NULL, split);
        if(token == "NA"){
                temp = 0;
        }
        else{
                temp = atoi(token);
        }
              char datestring[11];
        strcpy(datestring, (char*)year);
        strcpy(datestring, split);
        strcpy(datestring, (char*)month);
        strcpy(datestring, split);
        strcpy(datestring, (char*)day);
        strcpy(datestring, "\0");


        unsigned long hashval = hash(datestring)%size;
        node *front = table[hashval];
        if(node_lookup(front, year, month, day, hour)==NULL){
                fprintf(stderr, "load_table duplicate entry: %d-%d-%d %d\n",year,month,day,hour);

        }
        else{
                if(add_node(front,year,month,day,hour,pm25,temp)==NULL){
                        fprintf(stderr,"load_table could not add %s\n",datestring);
                }
                add_node(front,year,month,day,hour,pm25,temp);
                }
        }
               fclose(fp);
  return 0;
}

/*
 * print_info
 *
 * Arguments: pointer to a hash table, number of elements
 */
void print_info(node **table, unsigned long size) {
  long unsigned int totalentries;
  long unsigned int chain[size];
  long unsigned int emptynumber = 0;
  long unsigned int count;
  long unsigned int longestchain = -1;
  long unsigned int shortestchain = INT_MAX;
  node *cur;
  for(long unsigned int i = 0; i< size; i++){
          count = 0;
          cur = table[i];
          if(cur == NULL){
                  emptynumber++;
          }
          while(cur != NULL){
                  cur = cur->next;
                  count++;
                  totalentries++;
          }
          chain[i] = count;
  }

  for(long unsigned int i = 0; i<size;i++){
          if(chain[i] > longestchain){
                  longestchain = chain[i];
          }
          if(chain[i] < shortestchain){
                  shortestchain = chain[i];
          }
  }
  printf("Table size: %lu\n", size);
  printf("Total entries: %lu\n", totalentries);
  printf("Longest chain: %lu\n", longestchain);
  printf("Shortest chain: %lu\n", shortestchain);
  printf("Empty buckets: %lu\n", emptynumber);
}

/*
 * delete_date
 * Delete all nodes associated with a given date of form YYYY-MM-DD
 * All leading zeros have been removed in the date string
 */
void delete_date(node **table, unsigned long size, char *datestr) {
  unsigned long hashval = hash(datestr) % size;
  node *chain = table[hashval];
  node *tmp, *prev = NULL;
  node* new_head = chain;

  const char split[] = "-";
  char *token = strtok(datestr, split);
  int cols[COL_DAY+1];
  int c = 0;
  while (token != NULL) {
    cols[c] = atoi(token);
    token = strtok(NULL, split);
    c++;
  }

  while (chain != NULL) {
    tmp = chain->next;
    // Delete if matching
    if (chain->year == cols[COL_YEAR] && chain->month == cols[COL_MONTH]
        && chain->day == cols[COL_DAY]) {
      // Only link previous if there was a previous
      if (prev) {
        prev->next = tmp;
      // No previous: this was the head, now new head is the one in front of us
      } else {
        new_head = tmp;
      }
      free(chain);
    // If not matching, don't delete and set prev as usual
    } else {
      prev = chain;
    }
    chain = tmp;
  }

  table[hashval] = new_head;
}


/*
 * delete_table
 *
 * Arguments: pointer to hash table, hash table array size
 */
void delete_table(node **table, unsigned long size) {
  unsigned int i;
  node *tmp;
  node *curr_tmp;

  for (i = 0; i < size; i++) {
    curr_tmp = table[i];
    while (curr_tmp != NULL) {
      tmp = curr_tmp->next;
      free(curr_tmp);
      curr_tmp = tmp;
    }
  }

  free(table);
}
