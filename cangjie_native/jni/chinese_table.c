#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#define  LOG_TAG    "Cangjie"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#include "input_method.h"
#include "phrase.h"

int mTotalMatch = 0;
int mSaved = 0;
jchar mCurrentIm = -1;
jboolean mEnableHK = 0;
char data_path[1024] = "0";
char quick_data[1024] = "0";
char cangjie_data[1024] = "0";
char cangjie_hk_data[1024] = "0";
jchar keyStorage[64];

void  *inputMethodHandle = 0;
char *inputMethodList[4] = {
  "/data/data/com.diycircuits.inputmethod.latin/lib/libquick.so",
  "/data/data/com.diycircuits.inputmethod.latin/lib/libcangjie3.so",
  "/data/data/com.diycircuits.inputmethod.latin/lib/libstroke.so",
  0
};

char *inputMethodFunc[4] = {
  "quick_func",
  "cangjie_func",
  "stroke_func",
  0
};

struct _input_method *input_method = NULL;

struct _INPUT_METHOD_LIST {
  char *index;
  char *library_path;
  char *function_name;
  char *name;
} InputMethodList[16] = {
  { "1", "/data/data/com.diycircuits.inputmethod.latin/lib/libquick.so",     "quick_func",     "速成" },
  { "0", "/data/data/com.diycircuits.inputmethod.latin/lib/libcangjie3.so",  "cangjie_func",   "倉頡3" },
  { "2", "/data/data/com.diycircuits.inputmethod.latin/lib/libcangjie3.so",  "cangjie_func",   "倉頡3(香港字)" },
  { "4", "/data/data/com.diycircuits.inputmethod.latin/lib/libcangjie5.so",  "cangjie5_func",  "倉頡5" },
  { "3", "/data/data/com.diycircuits.inputmethod.latin/lib/libstroke.so",    "stroke_func",    "筆劃" },
  { "5", "/data/data/com.diycircuits.inputmethod.latin/lib/libdayi3.so",     "dayi3_func",     "大易三碼" },
  { "6", "/data/data/com.diycircuits.inputmethod.latin/lib/libcantonese.so", "cantonese_func", "廣東話拼音" },
  { 0, 0, 0}
};

int inputMethodCount = 0, inputMethodIndex = -1;
jobjectArray inputMethodNameList = NULL;
jobject _inputMethodNameList = NULL;
jobjectArray inputMethodIndexList = NULL;
jobject _inputMethodIndexList = NULL;

__attribute__((constructor)) static void onDlOpen(void)
{
  inputMethodHandle = 0;
}

static void loadInputMethod(jchar m)
{
  int count = 0; int method = -1;
  while (InputMethodList[count].index != NULL) {
    if (InputMethodList[count].index[0] == m) {
      method = count;
      break;
    } 
    count++;   
  }
  if (method == -1) return;

  inputMethodIndex = method;  

  /* LOGE("Load Input Method : %d", method); */
  
  if (inputMethodHandle == NULL) {
    inputMethodHandle = dlopen(InputMethodList[method].library_path, RTLD_NOW | RTLD_GLOBAL);
  } else {
    if (input_method != NULL) input_method->saveMatch();
    dlclose(inputMethodHandle);
    inputMethodHandle = dlopen(InputMethodList[method].library_path, RTLD_NOW | RTLD_GLOBAL);
  }
  input_method = (struct _input_method *) dlsym(inputMethodHandle, InputMethodList[method].function_name);
  input_method->init(quick_data);
  /* LOGE("Load Input Method : %d %s ", method, InputMethodList[method].name); */
}

void Java_com_diycircuits_cangjie_TableLoader_setupOnce(JNIEnv *env, jobject thiz)
{
  int count = 0;
  jstring str;

  while (InputMethodList[count].library_path != NULL) count++;
  inputMethodCount = count;

  inputMethodNameList = (*env)->NewObjectArray(env, (jsize) inputMethodCount, (jclass) (*env)->FindClass(env, "java/lang/String"), (jobject) NULL);
  inputMethodIndexList = (*env)->NewObjectArray(env, (jsize) inputMethodCount, (jclass) (*env)->FindClass(env, "java/lang/String"), (jobject) NULL);
  for (count = 0; count < inputMethodCount; count++) {
    str = (*env)->NewStringUTF(env, InputMethodList[count].name);
    (*env)->SetObjectArrayElement(env, inputMethodNameList, count, str);

    str = (*env)->NewStringUTF(env, InputMethodList[count].index);
    (*env)->SetObjectArrayElement(env, inputMethodIndexList, count, str);
  }

  _inputMethodNameList = (*env)->NewGlobalRef(env, inputMethodNameList);
  _inputMethodIndexList = (*env)->NewGlobalRef(env, inputMethodIndexList);
}

void Java_com_diycircuits_cangjie_TableLoader_setPath(JNIEnv *env, jobject thiz, jbyteArray path)
{
  jbyte *buf = (*env)->GetByteArrayElements(env, path, NULL);
  jsize len = (*env)->GetArrayLength(env, path);

  strncpy(data_path, buf, len);
  quick_data[0] = 0;
  strncat(quick_data, data_path, sizeof(quick_data));

  (*env)->ReleaseByteArrayElements(env, path, buf, 0);
}

void Java_com_diycircuits_cangjie_TableLoader_initialize(JNIEnv* env, jobject thiz)
{
  /* input_method[QUICK]->init(quick_data);   */
  /* input_method[CANGJIE]->init(quick_data); */
  /* input_method[STROKE]->init(quick_data); */
  init_phrase();
  load_phrase(quick_data);
}

void Java_com_diycircuits_cangjie_TableLoader_reset(JNIEnv* env, jobject thiz)
{
  /* input_method[QUICK]->reset(); */
  /* input_method[CANGJIE]->reset(); */
  /* input_method[STROKE]->reset(); */
  if (input_method != NULL) input_method->reset(); 
}
 
jchar Java_com_diycircuits_cangjie_TableLoader_getChar(JNIEnv* env, jobject thiz)
{
  /* int count = 0; */

  /* for (count = 0; count < sizeof(quick_index) / sizeof(jint); count++) { */
  /*   quick_index[count] = -1; */
  /* } */

  /* LOGE("Char Quick : %d", sizeof(quick) / (sizeof(jchar) * 4)); */
  /* LOGE("Char Length : %d", sizeof(jchar)); */
  /* LOGE("Char Value : %d %02X %02X %02X\n", strlen(value), value[0] & 0x00FF, value[1] & 0x00FF, value[2] & 0x00FF); */
  
  return 0;
}

jchar Java_com_diycircuits_cangjie_TableLoader_passCharArray(JNIEnv* env, jobject thiz, jcharArray arr)
{
  jchar *buf = (*env)->GetCharArrayElements(env, arr, NULL);
  jsize len = (*env)->GetArrayLength(env, arr);

  /* LOGE("Array Length : %d", len); */

  (*env)->ReleaseCharArrayElements(env, arr, buf, 0);

  return 0;
}

jint Java_com_diycircuits_cangjie_TableLoader_getMaxKey(JNIEnv* env, jobject thiz)
{
  /* return input_method[mCurrentIm]->maxKey(); */
  if (input_method == NULL) return 0;

  return input_method->maxKey();
}

void Java_com_diycircuits_cangjie_TableLoader_setInputMethod(JNIEnv* env, jobject thiz, jchar im)
{
  if (mCurrentIm != im) {
    mCurrentIm = im;
    loadInputMethod(im);
  }
}
 
void Java_com_diycircuits_cangjie_TableLoader_searchCangjie(JNIEnv* env, jobject thiz, jchar key0, jchar key1, jchar key2, jchar key3, jchar key4)
{
  /* input_method[mCurrentIm]->searchWord(key0, key1, key2, key3, key4); */
  if (input_method != NULL) input_method->searchWord(key0, key1, key2, key3, key4);
}

void Java_com_diycircuits_cangjie_TableLoader_searchCangjieMore(JNIEnv* env, jobject thiz, jcharArray key0, jcharArray key1, jcharArray key2, jcharArray key3, jcharArray key4)
{
  jchar key0array[6];
  jchar key1array[6];
  jchar key2array[6];
  jchar key3array[6];
  jchar key4array[6];

  (*env)->GetCharArrayRegion(env, key0, 0, 6, key0array);
  (*env)->GetCharArrayRegion(env, key1, 0, 6, key1array);
  (*env)->GetCharArrayRegion(env, key2, 0, 6, key2array);
  (*env)->GetCharArrayRegion(env, key3, 0, 6, key3array);
  (*env)->GetCharArrayRegion(env, key4, 0, 6, key4array);
  
  /* input_method[mCurrentIm]->searchWordMore(key0array, key1array, key2array, key3array, key4array); */
  if (input_method != NULL) input_method->searchWordMore(key0array, key1array, key2array, key3array, key4array);
}

void Java_com_diycircuits_cangjie_TableLoader_enableHongKongChar(JNIEnv* env, jobject thiz, jboolean hk)
{
  mEnableHK = hk;
  /* input_method[mCurrentIm]->enableHongKongChar(mEnableHK); */
  if (input_method != NULL) input_method->enableHongKongChar(mEnableHK);
}

jboolean Java_com_diycircuits_cangjie_TableLoader_tryMatchCangjie(JNIEnv* env, jobject thiz, jchar key0, jchar key1, jchar key2, jchar key3, jchar key4)
{
  /* LOGE("Try Match Cangjie"); */
  /* return input_method[mCurrentIm]->tryMatchWord(key0, key1, key2, key3, key4); */
  return input_method->tryMatchWord(key0, key1, key2, key3, key4);
}
 
jboolean Java_com_diycircuits_cangjie_TableLoader_trySearchWord(JNIEnv* env, jobject thiz, jcharArray key, jint len)
{
  memset(keyStorage, 0, sizeof(keyStorage));
  (*env)->GetCharArrayRegion(env, key, 0, 64, keyStorage);

  /* LOGE("Search Word : %d", inputMethodIndex); */
  
  /* if (mCurrentIm == QUICK || mCurrentIm == CANGJIE) */
  /*   return input_method[mCurrentIm]->tryMatchWord(keyStorage[0], keyStorage[1], keyStorage[2], keyStorage[3], keyStorage[4]); */
  /* else if (mCurrentIm == STROKE) */
  /*   return input_method[mCurrentIm]->tryMatchWordArray(keyStorage, len); */
  if (input_method == NULL || inputMethodIndex < 0) return 0;
  
  if (InputMethodList[inputMethodIndex].index[0] == '1' ||
      InputMethodList[inputMethodIndex].index[0] == '2' ||
      InputMethodList[inputMethodIndex].index[0] == '4' ||
      InputMethodList[inputMethodIndex].index[0] == '0')
    return input_method->tryMatchWord(keyStorage[0], keyStorage[1], keyStorage[2], keyStorage[3], keyStorage[4]);
  if (InputMethodList[inputMethodIndex].index[0] == '3' ||
      InputMethodList[inputMethodIndex].index[0] == '6')
    return input_method->tryMatchWordArray(keyStorage, len);
}

void Java_com_diycircuits_cangjie_TableLoader_searchWord(JNIEnv* env, jobject thiz, jcharArray key, jint len)
{
  memset(keyStorage, 0, sizeof(keyStorage));
  (*env)->GetCharArrayRegion(env, key, 0, 64, keyStorage);

  /* LOGE("Search Word Array : %d", inputMethodIndex); */
  
  if (input_method == NULL || inputMethodIndex < 0) return 0;

  /* if (mCurrentIm == QUICK || mCurrentIm == CANGJIE) */
  /*   input_method[mCurrentIm]->searchWord(keyStorage[0], keyStorage[1], keyStorage[2], keyStorage[3], keyStorage[4]); */
  /* else if (mCurrentIm == STROKE) */
  /*   input_method[mCurrentIm]->searchWordArray(keyStorage, len); */
  /* if (input_method != NULL) { */
  /*   if (mCurrentIm == QUICK || mCurrentIm == CANGJIE) */
  /*     input_method->searchWord(keyStorage[0], keyStorage[1], keyStorage[2], keyStorage[3], keyStorage[4]); */
  /*   else if (mCurrentIm == STROKE) */
  if (InputMethodList[inputMethodIndex].index[0] == '1' ||
      InputMethodList[inputMethodIndex].index[0] == '2' ||
      InputMethodList[inputMethodIndex].index[0] == '4' ||
      InputMethodList[inputMethodIndex].index[0] == '0')
      input_method->searchWord(keyStorage[0], keyStorage[1], keyStorage[2], keyStorage[3], keyStorage[4]); 
  if (InputMethodList[inputMethodIndex].index[0] == '3' ||
      InputMethodList[inputMethodIndex].index[0] == '6')
      input_method->searchWordArray(keyStorage, len);
}

jboolean Java_com_diycircuits_cangjie_TableLoader_tryMatchCangjieMore(JNIEnv* env, jobject thiz, jcharArray key0, jcharArray key1, jcharArray key2, jcharArray key3, jcharArray key4)
{
  jchar key0array[6];
  jchar key1array[6];
  jchar key2array[6];
  jchar key3array[6];
  jchar key4array[6];

  (*env)->GetCharArrayRegion(env, key0, 0, 6, key0array);
  (*env)->GetCharArrayRegion(env, key1, 0, 6, key1array);
  (*env)->GetCharArrayRegion(env, key2, 0, 6, key2array);
  (*env)->GetCharArrayRegion(env, key3, 0, 6, key3array);
  (*env)->GetCharArrayRegion(env, key4, 0, 6, key4array);
  
  /* return input_method[mCurrentIm]->tryMatchWordMore(key0array, key1array, key2array, key3array, key4array); */
  if (input_method == NULL) return 0;
  return input_method->tryMatchWordMore(key0array, key1array, key2array, key3array, key4array);
}
 
jint Java_com_diycircuits_cangjie_TableLoader_totalMatch(JNIEnv* env, jobject thiz)
{
  /* return input_method[mCurrentIm]->totalMatch(); */
  if (input_method == NULL) return 0;
  return input_method->totalMatch();
}
 
jchar Java_com_diycircuits_cangjie_TableLoader_getMatchChar(JNIEnv* env, jobject thiz, jint index)
{
  /* return input_method[mCurrentIm]->getMatchChar(index); */
  if (input_method == NULL) return 0;
  return input_method->getMatchChar(index);
}
 
jint Java_com_diycircuits_cangjie_TableLoader_getFrequency(JNIEnv* env, jobject thiz, jint index)
{
  /* return input_method[mCurrentIm]->getFrequency(index); */
  if (input_method == NULL) return 0;
  return input_method->getFrequency(index);
}
 
jint Java_com_diycircuits_cangjie_TableLoader_updateFrequency(JNIEnv* env, jobject thiz, jchar ch)
{
  /* return input_method[mCurrentIm]->updateFrequency(ch); */
  if (input_method == NULL) return 0;
  return input_method->updateFrequency(ch);
}
			
void Java_com_diycircuits_cangjie_TableLoader_saveMatch(JNIEnv* env, jobject thiz)
{
  /* input_method[QUICK]->saveMatch(); */
  /* input_method[CANGJIE]->saveMatch(); */
  /* input_method[STROKE]->saveMatch(); */
  if (input_method != NULL) input_method->saveMatch();
  save_phrase();
}

void Java_com_diycircuits_cangjie_TableLoader_clearAllFrequency(JNIEnv *env, jobject thiz)
{
  /* input_method[QUICK]->clearFrequency(); */
  /* input_method[CANGJIE]->clearFrequency(); */
  /* input_method[STROKE]->clearFrequency(); */
  if (input_method != NULL) input_method->clearFrequency();
  clear_phrase();
}

jint Java_com_diycircuits_cangjie_TableLoader_searchPhrase(JNIEnv *env, jobject thiz, jchar ch)
{
  /* jchar charray[16]; */
  /* (*env)->GetCharArrayRegion(env, ch, 0, 16, charray); */

  /* LOGE("Search Phrase"); */
  return search_phrase(ch);
}

jint Java_com_diycircuits_cangjie_TableLoader_getPhraseCount(JNIEnv *env, jobject thiz)
{
  /* LOGE("Get Phrase Count"); */
  return get_phrase_count();
}

jint Java_com_diycircuits_cangjie_TableLoader_getPhraseIndex(JNIEnv *env, jobject thiz)
{
  /* LOGE("Get Phrase Index"); */
  return get_phrase_index();
}

jint Java_com_diycircuits_cangjie_TableLoader_getPhraseMax(JNIEnv *env, jobject thiz)
{
  /* LOGE("Get Phrase Max"); */
  return get_phrase_max();
}

jint Java_com_diycircuits_cangjie_TableLoader_getAllPhraseMax(JNIEnv *env, jobject thiz)
{
  return MAX_PHRASE_LENGTH;
}

void Java_com_diycircuits_cangjie_TableLoader_getPhrase(JNIEnv *env, jobject thiz, jint index, jobject sb)
{
  jclass cls = (*env)->GetObjectClass(env, sb);
  jmethodID id = (*env)->GetMethodID(env, cls, "append", "(C)Ljava/lang/StringBuffer;");
  if (id != 0) {
    /* LOGE("Method ID Found"); */
    int max = get_phrase_max();
    if (max > 0) {
      int count = 0;
      jchar *p = get_phrase(index);
      for (count = 0; count < max; count++) {
	if (p[count] == 0) return;
	(*env)->CallObjectMethod(env, sb, id, p[count]);
      }
    }
  /* } else { */
  /*   LOGE("Method ID Not Found"); */
  }
}

jint Java_com_diycircuits_cangjie_TableLoader_getPhraseArray(JNIEnv *env, jobject thiz, jint index, jcharArray phrase)
{
  jint count = 0, total = 0;
  int max = get_phrase_max();
  jchar *input = get_phrase(index);
  jchar *buf = (*env)->GetCharArrayElements(env, phrase, NULL);
  jsize len = (*env)->GetArrayLength(env, phrase);

  for (count = 0; count < max; count++) {
    buf[count] = input[count];
    if (buf[count] == 0) {
      break;
    }
    total++;
  }

  (*env)->ReleaseCharArrayElements(env, phrase, buf, 0);

  return total;
}

jint Java_com_diycircuits_cangjie_TableLoader_measurePhraseRow(JNIEnv *env, jobject thiz,
							       jint width, jint charwidth, jint spacing, jint textFontSpacing, jintArray phraseRow)
{
  jint *buf = (*env)->GetIntArrayElements(env, phraseRow, NULL);
  jsize len = (*env)->GetArrayLength(env, phraseRow);

  int count = 0;
  int offset = 0;
  int row = 0;
  int plen = 0;

  for (count = 0; count < get_phrase_count(); count++) {
    if (offset + spacing + charwidth * get_phrase_length(get_phrase_index() + count) > (width - (1 * charwidth)) || count == 0) {
      /* LOGE("Phrase Row : %d = %d", row, get_phrase_index() + count); */
      buf[row] = get_phrase_index() + count;
      row++;
      offset = 0;
    }
    plen = get_phrase_length(get_phrase_index() + count);
    offset += spacing + charwidth * plen + textFontSpacing * (plen - 1);
  }

  /* if (offset > 0) row++; */
  buf[row] = get_phrase_index() + get_phrase_count();

  (*env)->ReleaseIntArrayElements(env, phraseRow, buf, 0);

  return row;
}

void Java_com_diycircuits_cangjie_TableLoader_updatePhraseFrequency(JNIEnv *env, jobject thiz, jint idx)
{
  update_phrase_frequency(idx);
}

jint Java_com_diycircuits_cangjie_TableLoader_getPhraseFrequency(JNIEnv *env, jobject thiz, jint idx)
{
  return get_phrase_frequency(idx);
}

void Java_com_diycircuits_cangjie_TableLoader_setSortingMethod(JNIEnv *env, jobject thiz, jint method)
{
  /* input_method[QUICK]->setSortingMethod(method); */
  /* input_method[CANGJIE]->setSortingMethod(method); */
  /* input_method[STROKE]->setSortingMethod(method); */
  if (input_method != NULL) input_method->setSortingMethod(method);
}

jint Java_com_diycircuits_cangjie_TableLoader_getInputMethodCount(JNIEnv *env, jobject thiz)
{
  return inputMethodCount;
}

jobject Java_com_diycircuits_cangjie_TableLoader_getInputMethodNameList(JNIEnv *env, jobject thiz)
{
  return _inputMethodNameList;
}

jobject Java_com_diycircuits_cangjie_TableLoader_getInputMethodIndexList(JNIEnv *env, jobject thiz)
{
  return _inputMethodIndexList;
}
