#include <stdio.h>
#include <string.h>
#include "dayi3_method.h"
#include "dayi3.h"

#ifndef X86
#include <android/log.h>
#define  LOG_TAG    "Cangjie"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define  LOGE(...)
#endif

typedef struct {
  int total;
  int index[sizeof(dayi3_index)];
} DAYI3_SORT;

static DAYI3_SORT dayi3_sort[6];
static int _mTotalMatch = 0;

static void dayi3_init(char *path)
{
  int clear = 0;
  int count = 0;
  char key[8];

  dayi3_func.mSortingMethod = 0;
  dayi3_func.mSaved = 0;

  strncpy(dayi3_func.mPath,             path, sizeof(dayi3_func.mPath));
  strncat(dayi3_func.mPath, "/dayi3.dat", sizeof(dayi3_func.mPath));

  for (count = 0; count < sizeof(dayi3_index) / sizeof(jint); count++) {
    dayi3_index[count] = -1;
  }

  for (count = 0; count < sizeof(dayi3_sort) / sizeof(DAYI3_SORT); count++)
    dayi3_sort[count].total = 0;
  
  memset(key, 0, 8);
  strcpy(key, "DAYI30");

  clear = 1;
  FILE *file = fopen(dayi3_func.mPath, "r");
  if (file != 0) {
    int read = fread(dayi3_func.mBuffer, 1, sizeof(dayi3_func.mBuffer), file);
    if (memcmp(dayi3_func.mBuffer, key, 8) == 0) {
      int read = fread(dayi3_frequency, 1, sizeof(dayi3_frequency), file);
      fclose(file);
      if (read == sizeof(dayi3_frequency)) clear = 0;
    }
  }
   
  if (clear != 0) {
    for (count = 0; count < sizeof(dayi3_frequency) / sizeof(jint); count++) {
      dayi3_frequency[count] = 0;
    }
  } else {
    for (count = 0; count < sizeof(dayi3_frequency) / sizeof(jint); count++) {
      if (dayi3[count][8] == 1) dayi3_frequency[count] = 0;
    }
  }
}

static int dayi3_maxKey(void)
{
  return 4;
}

static int dayi3_key_index(char v)
{
  char *keyMap = ",./0123456789;ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int count = 0;
  while (count < 40) {
    if (keyMap[count] == v)
      return count;
    count++;
  }

  return -1;
}

static inline int dayi3_memcmp(jchar *word, jchar **key, int len)
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
static jboolean dayi3_searchingMore(jchar* key0, jchar* key1, jchar* key2, jchar* key3, jchar* key4, int updateindex)
{
  jchar *src[6];
  jchar empty[6] = { 0, 0, 0, 0, 0, 0 };
  int total = sizeof(dayi3) / (sizeof(jchar) * DAYI3_COLUMN);
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

  LOGE("Dayi3 Key : %02X %02X %02X %02X",
       key0, key1, key2, key3);
  
  found = 0;
  for (count0 = 0; count0 < 5; count0++) {
    if (src[count0][0] == '*') found++;
  }

  if (found > 1)
    return 0;

  for (count0 = 0; count0 < sizeof(dayi3_index_temp) / sizeof(jint); count0++) {
    dayi3_index_temp[count0] = 0;
  }
  for (count0 = 0; count0 < sizeof(dayi3_sort) / sizeof(DAYI3_SORT); count0++)
    dayi3_sort[count0].total = 0;

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
    /* int coffset = dayi3_code_index[src[0][cmatch] - 'a'][0]; */
    /* int ccount  = dayi3_code_index[src[0][cmatch] - 'a'][1]; */
    int coffset = dayi3_code_index[dayi3_key_index(src[0][cmatch])][0];
    int ccount  = dayi3_code_index[dayi3_key_index(src[0][cmatch])][1];
    /* LOGE("Cangjie Size : %d %d %d %d %c", src[0][cmatch] - 'a', src[0][cmatch], coffset, ccount, cangjie[coffset][0]); */
    /* found = 0; state = 0;  */
    for (count0 = coffset; count0 < coffset + ccount; count0++) {
      ismatch = 0;
      if (dayi3_memcmp((jchar *) dayi3[count0], src, firstlen) == 0) {
	// state = 1;
	firstmatch++;
	if (secondlen == 0 && (dayi3_func.mEnableHK != 0 || dayi3[count0][6] == 0)) {
	  ismatch = 1;
	} else {
	  if (firstlen + secondlen <= dayi3[count0][8]) {
	    if (dayi3_memcmp((jchar *) &dayi3[count0][dayi3[count0][8] - secondlen], &src[firstlen + 1], secondlen) == 0 &&
		(dayi3_func.mEnableHK != 0 || dayi3[count0][6] == 0)) {
	      ismatch = 1;
	    }
	  } else {
	    ismatch = 0;
	  }
	}
	if (ismatch) {
	  dayi3_index_temp[loop] = count0;
	  int l = dayi3[count0][8];
	  int t = dayi3_sort[l - 1].total;
	  dayi3_sort[l - 1].index[t] = count0;
	  dayi3_sort[l - 1].total++;
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
    for (i = 0; i < sizeof(dayi3_sort) / sizeof(DAYI3_SORT); i++) {
      if (dayi3_sort[i].total == 0) continue;
      int swap = 1;
      while (swap) {
	swap = 0;
	for (j = 0; j < dayi3_sort[i].total - 1; j++) {
	  int mark0 = 0;
	  int mark1 = 0;
	  int k = 0;
	  for (k = 0; k < (i + 1); k++) {
	    if (src[k][0] == 0) continue;
	    mark0 += dayi3[dayi3_sort[i].index[j]][k] - src[k][0];
	    mark1 += dayi3[dayi3_sort[i].index[j + 1]][k] - src[k][0];
	  }
	  if (mark0 < 0) mark0 = -mark0;
	  if (mark1 < 0) mark1 = -mark1;
	  if ((dayi3_frequency[dayi3_sort[i].index[j]] < dayi3_frequency[dayi3_sort[i].index[j + 1]] &&
	       (mark0 == mark1 || (mark0 != 0 && mark1 != 0)))
	      ||
	      (mark1 == 0 && mark0 > mark1)) {
	    int temp = dayi3_sort[i].index[j];
	    dayi3_sort[i].index[j] = dayi3_sort[i].index[j + 1];
	    dayi3_sort[i].index[j + 1] = temp;
	    swap = 1;
	  }
	}
      }
    }

    int copy = 0;
    for (i = 0; i < sizeof(dayi3_sort) / sizeof(DAYI3_SORT); i++) {
      if (dayi3_sort[i].total == 0) continue;
      for (j = 0; j < dayi3_sort[i].total; j++) {
	dayi3_index_temp[copy] = dayi3_sort[i].index[j];
	copy++;
      }
    } 
  }

  if (firstmatch > 0 && secondlen > 0 && updateindex == 0)
    return 1;
  
  return (loop > 0) ? 1 : 0;
  return 1;
}

static jboolean dayi3_searching(jchar key0, jchar key1, jchar key2, jchar key3, jchar key4, int updateindex)
{
  jchar src[6];
  int total = sizeof(dayi3) / (sizeof(jchar) * DAYI3_COLUMN);
  int loop  = 0;
  int found = 0;
  int ismatch = 0;
  int count0 = 0, count1 = 0, state = 0;
  int firstlen = 0, secondlen = 0, firstmatch = 0;
  int i = 0;

  src[0] = key0 != 0 ? key0 : 0;
  src[1] = key1 != 0 ? key1 : 0;
  src[2] = key2 != 0 ? key2 : 0;
  src[3] = key3 != 0 ? key3 : 0;
  src[4] = key4 != 0 ? key4 : 0;
  src[5] = 0;

  LOGE("Dayi3 Key : %02X %02X %02X %02X",
       key0, key1, key2, key3);
  
  found = 0;
  for (count0 = 0; count0 < 5; count0++) {
    if (src[count0] == '*') found++;
  }

  if (found > 1)
    return 0;

  for (count0 = 0; count0 < sizeof(dayi3_index) / sizeof(jint); count0++) {
    dayi3_index_temp[count0] = 0;
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
    if (memcmp(dayi3[count0], src, firstlen * sizeof(jchar)) == 0) {
      state = 1;
      firstmatch++;
      if (secondlen == 0 && (dayi3_func.mEnableHK != 0 || dayi3[count0][6] == 0)) {
	ismatch = 1;
      } else {
	if (firstlen + secondlen <= dayi3[count0][8]) {
	  if (memcmp(&dayi3[count0][dayi3[count0][8] - secondlen], &src[firstlen + 1], secondlen * sizeof(jchar)) == 0) {
	    ismatch = 1;
	  }
	} else {
	  ismatch = 0;
	}
      }
      if (ismatch) {
	dayi3_index_temp[loop] = count0;
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
    if (dayi3_func.mSortingMethod == 1) {
      while (swap) {
	swap = 0;
	for (i = 0; i < loop - 1; i++) {
	  if (dayi3[dayi3_index_temp[i]][8] > 1 && dayi3[dayi3_index_temp[i + 1]][8] == 1) {
	    int temp = dayi3_index_temp[i];
	    dayi3_index_temp[i] = dayi3_index_temp[i + 1];
	    dayi3_index_temp[i + 1] = temp;
	    swap = 1;
	  }
	}
      }
      for (i = 0; i < loop - 1; i++) {
	if (dayi3[dayi3_index_temp[i]][8] == 1) start = i;
	else break;
      }
      start++;
    }
    swap = 1;
    while (swap) {
      swap = 0;
      for (i = start; i < loop - 1; i++) {
	if (dayi3_func.mSortingMethod == 0) {
	  if (dayi3_frequency[dayi3_index_temp[i]] < dayi3_frequency[dayi3_index_temp[i + 1]] &&
	      dayi3[dayi3_index_temp[i]][8] >= dayi3[dayi3_index_temp[i + 1]][8]) {
	    int temp = dayi3_index_temp[i];
	    dayi3_index_temp[i] = dayi3_index_temp[i + 1];
	    dayi3_index_temp[i + 1] = temp;
	    swap = 1;
	  }
	} else if (dayi3_func.mSortingMethod == 1) {
	  if (dayi3_frequency[dayi3_index_temp[i]] < dayi3_frequency[dayi3_index_temp[i + 1]]) {
	    int temp = dayi3_index_temp[i];
	    dayi3_index_temp[i] = dayi3_index_temp[i + 1];
	    dayi3_index_temp[i + 1] = temp;
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

static void dayi3_searchWord(jchar key0, jchar key1, jchar key2, jchar key3, jchar key4)
{
  dayi3_func.mTotalMatch = _mTotalMatch;
  memcpy(dayi3_index, dayi3_index_temp, _mTotalMatch * sizeof(jint));
}

static void dayi3_searchWordMore(jchar *key0, jchar *key1, jchar *key2, jchar *key3, jchar *key4)
{
  dayi3_func.mTotalMatch = _mTotalMatch;
  memcpy(dayi3_index, dayi3_index_temp, _mTotalMatch * sizeof(jint));
}

static int cmp(jchar *s, jchar *d, int n) {
  int c = 0, l = 0;
  for (c = 0; c < n; c++) {
    if (s[c] != d[c]) l++;
  }
  return l;
}

static jboolean dayi3_tryMatchWord(jchar key0, jchar key1, jchar key2, jchar key3, jchar key4)
{
  return dayi3_searching(key0, key1, key2, key3, key4, 0);
}

static jboolean dayi3_tryMatchWordMore(jchar* key0, jchar* key1, jchar* key2, jchar* key3, jchar* key4)
{
  return dayi3_searchingMore(key0, key1, key2, key3, key4, 0);
}

static int dayi3_totalMatch(void)
{
  return dayi3_func.mTotalMatch;
}

static int dayi3_updateFrequency(jchar ch)
{
  int total = sizeof(dayi3) / (sizeof(jchar) * DAYI3_COLUMN);
  int count = 0;

  for (count = 0; count < total; count++) {
    if (dayi3[count][6] == ch) {
      dayi3_frequency[count]++;
      dayi3_func.mSaved = 1;
      return dayi3_frequency[count];
    }
  }

  return 0;
}

static void dayi3_clearFrequency(void)
{
  int count = 0;
  
  for (count = 0; count < sizeof(dayi3_frequency) / sizeof(jint); count++) {
    dayi3_frequency[count] = 0;
  }

  remove(dayi3_func.mPath);
}

static jchar dayi3_getMatchChar(int index)
{
  int total = sizeof(dayi3) / (sizeof(jchar) * DAYI3_COLUMN);

  if (index >= total) return 0;
  if (dayi3_index[index] < 0) return 0;

  return dayi3[dayi3_index[index]][6];
}

static jint dayi3_getFrequency(int index)
{
  return dayi3_frequency[dayi3_index[index]];
}

static void dayi3_reset(void)
{
  dayi3_func.mTotalMatch = 0;
  _mTotalMatch = 0;
}

static void dayi3_saveMatch(void)
{
  char key[8];

  if (dayi3_func.mSaved == 0) return;
  dayi3_func.mSaved = 0;

  memset(key, 0, 8);
  strcpy(key, "DAYI30");
  FILE *file = fopen(dayi3_func.mPath, "w");
  if (file != NULL) {
    fwrite(key, 1, sizeof(key), file);
    fwrite(dayi3_frequency, 1, sizeof(dayi3_frequency), file);
    fclose(file);
  }
}

static void dayi3_enableHongKongChar(jboolean hk)
{
  dayi3_func.mEnableHK = (hk != 0);
}

void dayi3_setSortingMethod(int method)
{
  dayi3_func.mSortingMethod = method;
}

struct _input_method dayi3_func =
{
  .init            = dayi3_init,
  .maxKey          = dayi3_maxKey,
  .searchWord      = dayi3_searchWord,
  .searchWordMore  = dayi3_searchWordMore,
  .searchWordArray = 0,
  .tryMatchWord    = dayi3_tryMatchWord,
  .tryMatchWordMore = dayi3_tryMatchWordMore,
  .tryMatchWordArray = 0,
  .enableHongKongChar = dayi3_enableHongKongChar,
  .totalMatch      = dayi3_totalMatch,
  .updateFrequency = dayi3_updateFrequency,
  .clearFrequency  = dayi3_clearFrequency,
  .getMatchChar    = dayi3_getMatchChar,
  .getFrequency    = dayi3_getFrequency,
  .reset           = dayi3_reset,
  .saveMatch       = dayi3_saveMatch,
  .setSortingMethod = dayi3_setSortingMethod
};

