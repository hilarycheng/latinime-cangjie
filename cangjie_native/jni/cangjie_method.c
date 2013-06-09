#include <stdio.h>
#include <string.h>
#include "cangjie_method.h"

#ifdef CANGJIE3
#include "cangjie.h"
#endif
#ifdef CANGJIE5
#include "cangjie5.h"
#endif

#ifndef X86
#include <android/log.h>
#define  LOG_TAG    "Cangjie"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define  LOGE(...)
#endif

typedef struct {
  int total;
  int index[sizeof(cangjie_index)];
} CANGJIE_SORT;

static CANGJIE_SORT cangjie_sort[5];
static int _mTotalMatch = 0;

static void cangjie_init(char *path)
{
  int clear = 0;
  int count = 0;
  char key[8];

  cangjie_func.mSortingMethod = 0;
  cangjie_func.mSaved = 0;

  strncpy(cangjie_func.mPath,           path, sizeof(cangjie_func.mPath));
#ifdef CANGJIE3
  strncat(cangjie_func.mPath, "/cangjie.dat", sizeof(cangjie_func.mPath));
#endif  
#ifdef CANGJIE5
  strncat(cangjie_func.mPath, "/cangjie5.dat", sizeof(cangjie_func.mPath));
#endif  

  for (count = 0; count < sizeof(cangjie_index) / sizeof(jint); count++) {
    cangjie_index[count] = -1;
  }

  for (count = 0; count < sizeof(cangjie_sort) / sizeof(CANGJIE_SORT); count++)
    cangjie_sort[count].total = 0;
  
  memset(key, 0, 8);
  strcpy(key, "CANGJ0");

  clear = 1;
  FILE *file = fopen(cangjie_func.mPath, "r");
  if (file != 0) {
    int read = fread(cangjie_func.mBuffer, 1, sizeof(cangjie_func.mBuffer), file);
    if (memcmp(cangjie_func.mBuffer, key, 8) == 0) {
      int read = fread(cangjie_frequency, 1, sizeof(cangjie_frequency), file);
      fclose(file);
      if (read == sizeof(cangjie_frequency)) clear = 0;
    }
  }
   
  if (clear != 0) {
    for (count = 0; count < sizeof(cangjie_frequency) / sizeof(jint); count++) {
      cangjie_frequency[count] = 0;
    }
  } else {
    for (count = 0; count < sizeof(cangjie_frequency) / sizeof(jint); count++) {
      if (cangjie[count][7] == 1) cangjie_frequency[count] = 0;
    }
  }
}

static int cangjie_maxKey(void)
{
  return 5;
}

static inline int cangjie_memcmp(jchar *word, jchar **key, int len)
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
static jboolean cangjie_searchingMore(jchar* key0, jchar* key1, jchar* key2, jchar* key3, jchar* key4, int updateindex)
{
  jchar *src[6];
  jchar empty[6] = { 0, 0, 0, 0, 0, 0 };
  int total = sizeof(cangjie) / (sizeof(jchar) * CANGJIE_COLUMN);
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

  for (count0 = 0; count0 < sizeof(cangjie_index_temp) / sizeof(jint); count0++) {
    cangjie_index_temp[count0] = 0;
  }
  for (count0 = 0; count0 < sizeof(cangjie_sort) / sizeof(CANGJIE_SORT); count0++)
    cangjie_sort[count0].total = 0;

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
    int coffset = cangjie_code_index[src[0][cmatch] - 'a'][0];
    int ccount  = cangjie_code_index[src[0][cmatch] - 'a'][1];
    /* LOGE("Cangjie Size : %d %d %d %d %c", src[0][cmatch] - 'a', src[0][cmatch], coffset, ccount, cangjie[coffset][0]); */
    /* found = 0; state = 0;  */
    for (count0 = coffset; count0 < coffset + ccount; count0++) {
      ismatch = 0;
      if (cangjie_memcmp((jchar *) cangjie[count0], src, firstlen) == 0) {
	// state = 1;
	firstmatch++;
	if (secondlen == 0 && (cangjie_func.mEnableHK != 0 || cangjie[count0][6] == 0)) {
	  ismatch = 1;
	} else {
	  if (firstlen + secondlen <= cangjie[count0][7]) {
	    if (cangjie_memcmp((jchar *) &cangjie[count0][cangjie[count0][7] - secondlen], &src[firstlen + 1], secondlen) == 0 &&
		(cangjie_func.mEnableHK != 0 || cangjie[count0][6] == 0)) {
	      ismatch = 1;
	    }
	  } else {
	    ismatch = 0;
	  }
	}
	if (ismatch) {
	  cangjie_index_temp[loop] = count0;
	  int l = cangjie[count0][7];
	  int t = cangjie_sort[l - 1].total;
	  cangjie_sort[l - 1].index[t] = count0;
	  cangjie_sort[l - 1].total++;
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
    for (i = 0; i < sizeof(cangjie_sort) / sizeof(CANGJIE_SORT); i++) {
      if (cangjie_sort[i].total == 0) continue;
      int swap = 1;
      while (swap) {
	swap = 0;
	for (j = 0; j < cangjie_sort[i].total - 1; j++) {
	  int mark0 = 0;
	  int mark1 = 0;
	  int k = 0;
	  for (k = 0; k < (i + 1); k++) {
	    if (src[k][0] == 0) continue;
	    mark0 += cangjie[cangjie_sort[i].index[j]][k] - src[k][0];
	    mark1 += cangjie[cangjie_sort[i].index[j + 1]][k] - src[k][0];
	  }
	  if (mark0 < 0) mark0 = -mark0;
	  if (mark1 < 0) mark1 = -mark1;
	  if ((cangjie_frequency[cangjie_sort[i].index[j]] < cangjie_frequency[cangjie_sort[i].index[j + 1]] &&
	       (mark0 == mark1 || (mark0 != 0 && mark1 != 0)))
	      ||
	      (mark1 == 0 && mark0 > mark1)) {
	    int temp = cangjie_sort[i].index[j];
	    cangjie_sort[i].index[j] = cangjie_sort[i].index[j + 1];
	    cangjie_sort[i].index[j + 1] = temp;
	    swap = 1;
	  }
	}
      }
    }

    int copy = 0;
    for (i = 0; i < sizeof(cangjie_sort) / sizeof(CANGJIE_SORT); i++) {
      if (cangjie_sort[i].total == 0) continue;
      for (j = 0; j < cangjie_sort[i].total; j++) {
	cangjie_index_temp[copy] = cangjie_sort[i].index[j];
	copy++;
      }
    } 
  }

  if (firstmatch > 0 && secondlen > 0 && updateindex == 0)
    return 1;
  
  return (loop > 0) ? 1 : 0;
  return 1;
}

static jboolean cangjie_searching(jchar key0, jchar key1, jchar key2, jchar key3, jchar key4, int updateindex)
{
  jchar src[6];
  int total = sizeof(cangjie) / (sizeof(jchar) * CANGJIE_COLUMN);
  int loop  = 0;
  int found = 0;
  int ismatch = 0;
  int count0 = 0, count1 = 0, state = 0;
  int firstlen = 0, secondlen = 0, firstmatch = 0;
  int i = 0;

  src[0] = key0;
  src[1] = key1;
  src[2] = key2;
  src[3] = key3;
  src[4] = key4;
  src[5] = 0;

  found = 0;
  for (count0 = 0; count0 < 5; count0++) {
    if (src[count0] == '*') found++;
  }

  if (found > 1)
    return 0;

  for (count0 = 0; count0 < sizeof(cangjie_index) / sizeof(jint); count0++) {
    cangjie_index_temp[count0] = 0;
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
    if (memcmp(cangjie[count0], src, firstlen * sizeof(jchar)) == 0) {
      state = 1;
      firstmatch++;
      if (secondlen == 0 && (cangjie_func.mEnableHK != 0 || cangjie[count0][6] == 0)) {
	ismatch = 1;
      } else {
	if (firstlen + secondlen <= cangjie[count0][7]) {
	  if (memcmp(&cangjie[count0][cangjie[count0][7] - secondlen], &src[firstlen + 1], secondlen * sizeof(jchar)) == 0 &&
	      (cangjie_func.mEnableHK != 0 || cangjie[count0][6] == 0)) {
	    ismatch = 1;
	  }
	} else {
	  ismatch = 0;
	}
      }
      if (ismatch) {
	cangjie_index_temp[loop] = count0;
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
    if (cangjie_func.mSortingMethod == 1) {
      while (swap) {
	swap = 0;
	for (i = 0; i < loop - 1; i++) {
	  if (cangjie[cangjie_index_temp[i]][7] > 1 && cangjie[cangjie_index_temp[i + 1]][7] == 1) {
	    int temp = cangjie_index_temp[i];
	    cangjie_index_temp[i] = cangjie_index_temp[i + 1];
	    cangjie_index_temp[i + 1] = temp;
	    swap = 1;
	  }
	}
      }
      for (i = 0; i < loop - 1; i++) {
	if (cangjie[cangjie_index_temp[i]][7] == 1) start = i;
	else break;
      }
      start++;
    }
    swap = 1;
    while (swap) {
      swap = 0;
      for (i = start; i < loop - 1; i++) {
	if (cangjie_func.mSortingMethod == 0) {
	  if (cangjie_frequency[cangjie_index_temp[i]] < cangjie_frequency[cangjie_index_temp[i + 1]] &&
	      cangjie[cangjie_index_temp[i]][7] >= cangjie[cangjie_index_temp[i + 1]][7]) {
	    int temp = cangjie_index_temp[i];
	    cangjie_index_temp[i] = cangjie_index_temp[i + 1];
	    cangjie_index_temp[i + 1] = temp;
	    swap = 1;
	  }
	} else if (cangjie_func.mSortingMethod == 1) {
	  if (cangjie_frequency[cangjie_index_temp[i]] < cangjie_frequency[cangjie_index_temp[i + 1]]) {
	    int temp = cangjie_index_temp[i];
	    cangjie_index_temp[i] = cangjie_index_temp[i + 1];
	    cangjie_index_temp[i + 1] = temp;
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

static void cangjie_searchWord(jchar key0, jchar key1, jchar key2, jchar key3, jchar key4)
{
  cangjie_func.mTotalMatch = _mTotalMatch;
  memcpy(cangjie_index, cangjie_index_temp, _mTotalMatch * sizeof(jint));
}

static void cangjie_searchWordMore(jchar *key0, jchar *key1, jchar *key2, jchar *key3, jchar *key4)
{
  cangjie_func.mTotalMatch = _mTotalMatch;
  memcpy(cangjie_index, cangjie_index_temp, _mTotalMatch * sizeof(jint));
}

static int cmp(jchar *s, jchar *d, int n) {
  int c = 0, l = 0;
  for (c = 0; c < n; c++) {
    if (s[c] != d[c]) l++;
  }
  return l;
}

static jboolean cangjie_tryMatchWord(jchar key0, jchar key1, jchar key2, jchar key3, jchar key4)
{
  return cangjie_searching(key0, key1, key2, key3, key4, 0);
}

static jboolean cangjie_tryMatchWordMore(jchar* key0, jchar* key1, jchar* key2, jchar* key3, jchar* key4)
{
  return cangjie_searchingMore(key0, key1, key2, key3, key4, 0);
}

static int cangjie_totalMatch(void)
{
  return cangjie_func.mTotalMatch;
}

static int cangjie_updateFrequency(jchar ch)
{
  int total = sizeof(cangjie) / (sizeof(jchar) * CANGJIE_COLUMN);
  int count = 0;

  for (count = 0; count < total; count++) {
    if (cangjie[count][5] == ch) {
      cangjie_frequency[count]++;
      cangjie_func.mSaved = 1;
      return cangjie_frequency[count];
    }
  }

  return 0;
}

static void cangjie_clearFrequency(void)
{
  int count = 0;
  
  for (count = 0; count < sizeof(cangjie_frequency) / sizeof(jint); count++) {
    cangjie_frequency[count] = 0;
  }

  remove(cangjie_func.mPath);
}

static jchar cangjie_getMatchChar(int index)
{
  int total = sizeof(cangjie) / (sizeof(jchar) * CANGJIE_COLUMN);

  if (index >= total) return 0;
  if (cangjie_index[index] < 0) return 0;

  return cangjie[cangjie_index[index]][5];
}

static jint cangjie_getFrequency(int index)
{
  return cangjie_frequency[cangjie_index[index]];
}

static void cangjie_reset(void)
{
  cangjie_func.mTotalMatch = 0;
  _mTotalMatch = 0;
}

static void cangjie_saveMatch(void)
{
  char key[8];

  if (cangjie_func.mSaved == 0) return;
  cangjie_func.mSaved = 0;

  memset(key, 0, 8);
  strcpy(key, "CANGJ0");
  FILE *file = fopen(cangjie_func.mPath, "w");
  if (file != NULL) {
    fwrite(key, 1, sizeof(key), file);
    fwrite(cangjie_frequency, 1, sizeof(cangjie_frequency), file);
    fclose(file);
  }
}

static void cangjie_enableHongKongChar(jboolean hk)
{
  cangjie_func.mEnableHK = (hk != 0);
}

void cangjie_setSortingMethod(int method)
{
  cangjie_func.mSortingMethod = method;
}

#ifdef CANGJIE3
struct _input_method cangjie_func =
#endif
#ifdef CANGJIE5
struct _input_method cangjie5_func =
#endif
{
  .init            = cangjie_init,
  .maxKey          = cangjie_maxKey,
  .searchWord      = cangjie_searchWord,
  .searchWordMore  = cangjie_searchWordMore,
  .searchWordArray = 0,
  .tryMatchWord    = cangjie_tryMatchWord,
  .tryMatchWordMore = cangjie_tryMatchWordMore,
  .tryMatchWordArray = 0,
  .enableHongKongChar = cangjie_enableHongKongChar,
  .totalMatch      = cangjie_totalMatch,
  .updateFrequency = cangjie_updateFrequency,
  .clearFrequency  = cangjie_clearFrequency,
  .getMatchChar    = cangjie_getMatchChar,
  .getFrequency    = cangjie_getFrequency,
  .reset           = cangjie_reset,
  .saveMatch       = cangjie_saveMatch,
  .setSortingMethod = cangjie_setSortingMethod
};

__attribute__((constructor)) static void onDlOpen(void)
{
  LOGE(" Constructor Open ");
}

__attribute__((destructor)) static void onDlClose(void)
{
  LOGE(" Constructor Close ");
}
