#include <stdio.h>
#include <string.h>
#include "cantonese_method.h"
#include "cantonese.h"

#ifndef X86
#include <android/log.h>
#define  LOG_TAG    "Cangjie"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define  LOGE(...)
#endif

typedef struct {
  int total;
  int index[sizeof(cantonese_index)];
} CANTONESE_SORT;

static CANTONESE_SORT cantonese_sort[6];
static int _mTotalMatch = 0;

static void cantonese_init(char *path)
{
  int clear = 0;
  int count = 0;
  char key[8];

  cantonese_func.mSortingMethod = 0;
  cantonese_func.mSaved = 0;

  strncpy(cantonese_func.mPath,             path, sizeof(cantonese_func.mPath));
  strncat(cantonese_func.mPath, "/cantonese.dat", sizeof(cantonese_func.mPath));

  for (count = 0; count < sizeof(cantonese_index) / sizeof(jint); count++) {
    cantonese_index[count] = -1;
  }

  for (count = 0; count < sizeof(cantonese_sort) / sizeof(CANTONESE_SORT); count++)
    cantonese_sort[count].total = 0;
  
  memset(key, 0, 8);
  strcpy(key, "CANTO0");

  clear = 1;
  FILE *file = fopen(cantonese_func.mPath, "r");
  if (file != 0) {
    int read = fread(cantonese_func.mBuffer, 1, sizeof(cantonese_func.mBuffer), file);
    if (memcmp(cantonese_func.mBuffer, key, 8) == 0) {
      int read = fread(cantonese_frequency, 1, sizeof(cantonese_frequency), file);
      fclose(file);
      if (read == sizeof(cantonese_frequency)) clear = 0;
    }
  }
   
  if (clear != 0) {
    for (count = 0; count < sizeof(cantonese_frequency) / sizeof(jint); count++) {
      cantonese_frequency[count] = 0;
    }
  } else {
    for (count = 0; count < sizeof(cantonese_frequency) / sizeof(jint); count++) {
      if (cantonese[count][8] == 1) cantonese_frequency[count] = 0;
    }
  }
}

static int cantonese_maxKey(void)
{
  return 5;
}

static inline int cantonese_memcmp(jchar *word, jchar **key, int len)
{
  int count = 0;
  int result = 0;
  int match = 0;
  int i;

  for (count = 0; count < len; count++) {
    match = 0;
    for (i = 0; i < 6; i++) {
      if (word[count] == key[count][i] && key[count][i] != 0) {
	match++;
      }
    }
    if (match > 0) {
      result++;
    } else {
      return 1;
    }
  }

  return 0;
}

  /* char buffer[1024]; */
static jboolean cantonese_searchingMore(jchar* key0, jchar* key1, jchar* key2, jchar* key3, jchar* key4, int updateindex)
{
  jchar *src[6];
  jchar empty[6] = { 0, 0, 0, 0, 0, 0 };
  int total = sizeof(cantonese) / (sizeof(jchar) * CANTONESE_COLUMN);
  int loop  = 0;
  int found = 0;
  int ismatch = 0;
  int count0 = 0, count1 = 0, state = 0, cmatch = 0;
  int firstlen = 0, secondlen = 0, firstmatch = 0;
  int i = 0;
  int j = 0;

  src[0] = key0;
  src[1] = key1;
  src[2] = key2;
  src[3] = key3;
  src[4] = key4;
  src[5] = empty;

  found = 0;
  for (count0 = 0; count0 < 5; count0++) {
    if (src[count0][0] == '*') found++;
  }

  if (found > 1)
    return 0;

  for (count0 = 0; count0 < sizeof(cantonese_index_temp) / sizeof(jint); count0++) {
    cantonese_index_temp[count0] = 0;
  }
  for (count0 = 0; count0 < sizeof(cantonese_sort) / sizeof(CANTONESE_SORT); count0++)
    cantonese_sort[count0].total = 0;

  // Clear End Star Match
  for (count0 = 0; count0 < 5; count0++) {
    if (src[count0][0] == '*' && src[count0 + 1][0] == 0) {
      src[count0][0] = 0;
      break;
    }
  }

  state = 0; firstlen = 0; secondlen = 0; 
  for (count0 = 0; count0 < 5; count0++) {
    if (src[count0][0] == 0)
      break;
    if (src[count0][0] == '*')
      state = 1;
    if (state == 0)
      firstlen++;
    else
      secondlen++;
  }

  if (secondlen > 0)
    secondlen--;

  found = 0; state = 0;
  for (cmatch = 0; cmatch < 5; cmatch++) {
    if (!(src[0][cmatch] >= 'a' && src[0][cmatch] <= 'z'))
      continue;
    int coffset = cantonese_code_index[src[0][cmatch] - 'a'][0];
    int ccount  = cantonese_code_index[src[0][cmatch] - 'a'][1];
    /* LOGE("Cangjie Size : %d %d %d %d %c", src[0][cmatch] - 'a', src[0][cmatch], coffset, ccount, cangjie[coffset][0]); */
    /* found = 0; state = 0;  */
    for (count0 = coffset; count0 < coffset + ccount; count0++) {
      ismatch = 0;
      if (cantonese_memcmp((jchar *) cantonese[count0], src, firstlen) == 0) {
	// state = 1;
	firstmatch++;
	if (secondlen == 0 && (cantonese_func.mEnableHK != 0 || cantonese[count0][6] == 0)) {
	  ismatch = 1;
	} else {
	  if (firstlen + secondlen <= cantonese[count0][8]) {
	    if (cantonese_memcmp((jchar *) &cantonese[count0][cantonese[count0][8] - secondlen], &src[firstlen + 1], secondlen) == 0 &&
		(cantonese_func.mEnableHK != 0 || cantonese[count0][6] == 0)) {
	      ismatch = 1;
	    }
	  } else {
	    ismatch = 0;
	  }
	}
	if (ismatch) {
	  cantonese_index_temp[loop] = count0;
	  int l = cantonese[count0][8];
	  int t = cantonese_sort[l - 1].total;
	  cantonese_sort[l - 1].index[t] = count0;
	  cantonese_sort[l - 1].total++;
	  loop++;
	  /* LOGE("Matched %d = %02x %02x %02x %02x %02x = %02x %02x %02x %02x %02x", */
	  /*      firstlen, */
	  /*      src[0], */
	  /*      src[1], */
	  /*      src[2], */
	  /*      src[3], */
	  /*      src[4], */
	  /*      cangjie[count0][0], */
	  /*      cangjie[count0][1], */
	  /*      cangjie[count0][2], */
	  /*      cangjie[count0][3], */
	  /*      cangjie[count0][4]); */
	  /* LOGE("Matched %02x %02x == %02x %02x %02x %02x %02x", */
	  /*      src[0][0],  */
	  /*      src[1][0],  */
	  /*      cangjie[count0][0], */
	  /*      cangjie[count0][1], */
	  /*      cangjie[count0][2], */
	  /*      cangjie[count0][3], */
	  /*      cangjie[count0][4]); */
	}
      } else if (state == 1) {
	break;
      }
    }
  }

  _mTotalMatch = loop;
  if (loop > 0) {
    for (i = 0; i < sizeof(cantonese_sort) / sizeof(CANTONESE_SORT); i++) {
      if (cantonese_sort[i].total == 0) continue;
      int swap = 1;
      while (swap) {
	swap = 0;
	for (j = 0; j < cantonese_sort[i].total - 1; j++) {
	  int mark0 = 0;
	  int mark1 = 0;
	  int k = 0;
	  for (k = 0; k < (i + 1); k++) {
	    if (src[k][0] == 0) continue;
	    mark0 += cantonese[cantonese_sort[i].index[j]][k] - src[k][0];
	    mark1 += cantonese[cantonese_sort[i].index[j + 1]][k] - src[k][0];
	  }
	  if (mark0 < 0) mark0 = -mark0;
	  if (mark1 < 0) mark1 = -mark1;
	  if ((cantonese_frequency[cantonese_sort[i].index[j]] < cantonese_frequency[cantonese_sort[i].index[j + 1]] &&
	       (mark0 == mark1 || (mark0 != 0 && mark1 != 0)))
	      ||
	      (mark1 == 0 && mark0 > mark1)) {
	    int temp = cantonese_sort[i].index[j];
	    cantonese_sort[i].index[j] = cantonese_sort[i].index[j + 1];
	    cantonese_sort[i].index[j + 1] = temp;
	    swap = 1;
	  }
	}
      }
    }

    int copy = 0;
    for (i = 0; i < sizeof(cantonese_sort) / sizeof(CANTONESE_SORT); i++) {
      if (cantonese_sort[i].total == 0) continue;
      for (j = 0; j < cantonese_sort[i].total; j++) {
	cantonese_index_temp[copy] = cantonese_sort[i].index[j];
	copy++;
      }
    } 
  }

  if (firstmatch > 0 && secondlen > 0 && updateindex == 0)
    return 1;
  
  return (loop > 0) ? 1 : 0;
  return 1;
}

static jboolean cantonese_searching(jchar key0, jchar key1, jchar key2, jchar key3, jchar key4, int updateindex)
{
  jchar src[6];
  int total = sizeof(cantonese) / (sizeof(jchar) * CANTONESE_COLUMN);
  int loop  = 0;
  int found = 0;
  int ismatch = 0;
  int count0 = 0, count1 = 0, state = 0;
  int firstlen = 0, secondlen = 0, firstmatch = 0;
  int i = 0;

  src[0] = key0 != 0 ? (key0 | 0x40) : 0;
  src[1] = key1 != 0 ? (key1 | 0x40) : 0;
  src[2] = key2 != 0 ? (key2 | 0x40) : 0;
  src[3] = key3 != 0 ? (key3 | 0x40) : 0;
  src[4] = key4 != 0 ? (key4 | 0x40) : 0;
  src[5] = 0;

  found = 0;
  for (count0 = 0; count0 < 5; count0++) {
    if (src[count0] == '*') found++;
  }

  if (found > 1)
    return 0;

  for (count0 = 0; count0 < sizeof(cantonese_index) / sizeof(jint); count0++) {
    cantonese_index_temp[count0] = 0;
  }

  // Clear End Star Match
  for (count0 = 0; count0 < 5; count0++) {
    if (src[count0] == '*' && src[count0 + 1] == 0) {
      src[count0] = 0;
      break;
    }
  }

  state = 0; firstlen = 0; secondlen = 0; 
  for (count0 = 0; count0 < 5; count0++) {
    if (src[count0] == 0)
      break;
    if (src[count0] == '*')
      state = 1;
    if (state == 0)
      firstlen++;
    else
      secondlen++;
  }

  if (secondlen > 0)
    secondlen--;

  found = 0; state = 0;
  for (count0 = 0; count0 < total; count0++) {
    ismatch = 0;
    if (memcmp(cantonese[count0], src, firstlen * sizeof(jchar)) == 0) {
      state = 1;
      firstmatch++;
      if (secondlen == 0 && (cantonese_func.mEnableHK != 0 || cantonese[count0][6] == 0)) {
	ismatch = 1;
      } else {
	if (firstlen + secondlen <= cantonese[count0][8]) {
	  if (memcmp(&cantonese[count0][cantonese[count0][8] - secondlen], &src[firstlen + 1], secondlen * sizeof(jchar)) == 0) {
	    ismatch = 1;
	  }
	} else {
	  ismatch = 0;
	}
      }
      if (ismatch) {
	cantonese_index_temp[loop] = count0;
	loop++;
	/* LOGE("Matched %d = %02x %02x %02x %02x %02x = %02x %02x %02x %02x %02x", */
	/*      firstlen, */
	/*      src[0], */
	/*      src[1], */
	/*      src[2], */
	/*      src[3], */
	/*      src[4], */
	/*      cangjie[count0][0], */
	/*      cangjie[count0][1], */
	/*      cangjie[count0][2], */
	/*      cangjie[count0][3], */
	/*      cangjie[count0][4]); */
      }
    } else if (state == 1) {
      break;
    }
  }

  _mTotalMatch = loop;
  if (loop > 0) {
    int swap = 1;
    int start = 0;
    if (cantonese_func.mSortingMethod == 1) {
      while (swap) {
	swap = 0;
	for (i = 0; i < loop - 1; i++) {
	  if (cantonese[cantonese_index_temp[i]][8] > 1 && cantonese[cantonese_index_temp[i + 1]][8] == 1) {
	    int temp = cantonese_index_temp[i];
	    cantonese_index_temp[i] = cantonese_index_temp[i + 1];
	    cantonese_index_temp[i + 1] = temp;
	    swap = 1;
	  }
	}
      }
      for (i = 0; i < loop - 1; i++) {
	if (cantonese[cantonese_index_temp[i]][8] == 1) start = i;
	else break;
      }
      start++;
    }
    swap = 1;
    while (swap) {
      swap = 0;
      for (i = start; i < loop - 1; i++) {
	if (cantonese_func.mSortingMethod == 0) {
	  if (cantonese_frequency[cantonese_index_temp[i]] < cantonese_frequency[cantonese_index_temp[i + 1]] &&
	      cantonese[cantonese_index_temp[i]][8] >= cantonese[cantonese_index_temp[i + 1]][8]) {
	    int temp = cantonese_index_temp[i];
	    cantonese_index_temp[i] = cantonese_index_temp[i + 1];
	    cantonese_index_temp[i + 1] = temp;
	    swap = 1;
	  }
	} else if (cantonese_func.mSortingMethod == 1) {
	  if (cantonese_frequency[cantonese_index_temp[i]] < cantonese_frequency[cantonese_index_temp[i + 1]]) {
	    int temp = cantonese_index_temp[i];
	    cantonese_index_temp[i] = cantonese_index_temp[i + 1];
	    cantonese_index_temp[i + 1] = temp;
	    swap = 1;
	  }
	}
      }
    }
  }

  if (firstmatch > 0 && secondlen > 0 && updateindex == 0)
    return 1;
  
  return (loop > 0) ? 1 : 0;
}

static void cantonese_searchWord(jchar key0, jchar key1, jchar key2, jchar key3, jchar key4)
{
  cantonese_func.mTotalMatch = _mTotalMatch;
  memcpy(cantonese_index, cantonese_index_temp, _mTotalMatch * sizeof(jint));
}

static void cantonese_searchWordMore(jchar *key0, jchar *key1, jchar *key2, jchar *key3, jchar *key4)
{
  cantonese_func.mTotalMatch = _mTotalMatch;
  memcpy(cantonese_index, cantonese_index_temp, _mTotalMatch * sizeof(jint));
}

static int cmp(jchar *s, jchar *d, int n) {
  int c = 0, l = 0;
  for (c = 0; c < n; c++) {
    if (s[c] != d[c]) l++;
  }
  return l;
}

static jboolean cantonese_tryMatchWord(jchar key0, jchar key1, jchar key2, jchar key3, jchar key4)
{
  return cantonese_searching(key0, key1, key2, key3, key4, 0);
}

static jboolean cantonese_tryMatchWordMore(jchar* key0, jchar* key1, jchar* key2, jchar* key3, jchar* key4)
{
  return cantonese_searchingMore(key0, key1, key2, key3, key4, 0);
}

static int cantonese_totalMatch(void)
{
  return cantonese_func.mTotalMatch;
}

static int cantonese_updateFrequency(jchar ch)
{
  int total = sizeof(cantonese) / (sizeof(jchar) * CANTONESE_COLUMN);
  int count = 0;

  for (count = 0; count < total; count++) {
    if (cantonese[count][6] == ch) {
      cantonese_frequency[count]++;
      cantonese_func.mSaved = 1;
      return cantonese_frequency[count];
    }
  }

  return 0;
}

static void cantonese_clearFrequency(void)
{
  int count = 0;
  
  for (count = 0; count < sizeof(cantonese_frequency) / sizeof(jint); count++) {
    cantonese_frequency[count] = 0;
  }

  remove(cantonese_func.mPath);
}

static jchar cantonese_getMatchChar(int index)
{
  int total = sizeof(cantonese) / (sizeof(jchar) * CANTONESE_COLUMN);

  if (index >= total) return 0;
  if (cantonese_index[index] < 0) return 0;

  return cantonese[cantonese_index[index]][6];
}

static jint cantonese_getFrequency(int index)
{
  return cantonese_frequency[cantonese_index[index]];
}

static void cantonese_reset(void)
{
  cantonese_func.mTotalMatch = 0;
  _mTotalMatch = 0;
}

static void cantonese_saveMatch(void)
{
  char key[8];

  if (cantonese_func.mSaved == 0) return;
  cantonese_func.mSaved = 0;

  memset(key, 0, 8);
  strcpy(key, "CANGJ0");
  FILE *file = fopen(cantonese_func.mPath, "w");
  if (file != NULL) {
    fwrite(key, 1, sizeof(key), file);
    fwrite(cantonese_frequency, 1, sizeof(cantonese_frequency), file);
    fclose(file);
  }
}

static void cantonese_enableHongKongChar(jboolean hk)
{
  cantonese_func.mEnableHK = (hk != 0);
}

void cantonese_setSortingMethod(int method)
{
  cantonese_func.mSortingMethod = method;
}

struct _input_method cantonese_func =
{
  .init            = cantonese_init,
  .maxKey          = cantonese_maxKey,
  .searchWord      = cantonese_searchWord,
  .searchWordMore  = cantonese_searchWordMore,
  .searchWordArray = 0,
  .tryMatchWord    = cantonese_tryMatchWord,
  .tryMatchWordMore = cantonese_tryMatchWordMore,
  .tryMatchWordArray = 0,
  .enableHongKongChar = cantonese_enableHongKongChar,
  .totalMatch      = cantonese_totalMatch,
  .updateFrequency = cantonese_updateFrequency,
  .clearFrequency  = cantonese_clearFrequency,
  .getMatchChar    = cantonese_getMatchChar,
  .getFrequency    = cantonese_getFrequency,
  .reset           = cantonese_reset,
  .saveMatch       = cantonese_saveMatch,
  .setSortingMethod = cantonese_setSortingMethod
};

