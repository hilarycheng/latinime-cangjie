package com.diycircuits.cangjie;

import com.diycircuits.inputmethod.latin.R;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.util.Log;
import android.preference.PreferenceManager;

import java.util.HashMap;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.File;
import java.io.OutputStream;
import java.io.BufferedReader;
import java.io.UnsupportedEncodingException;
import com.diycircuits.cangjie.CandidateListener;
import com.diycircuits.inputmethod.keyboard.ProximityInfo;

public class Cangjie implements CandidateListener {

    public final static int QUICK     = 0;
    public final static int CANGJIE   = 1;
    public final static int STROKE    = 2;
    public final static int CANTONESE = 3;
    public final static int DAYI3     = 4;

    private Context mContext = null;
    private char mCodeInput[] = new char[64];
    private char mCodeInputNearest[][] = new char[64][6];
    private int  mCodeCount = 0;
    private char mCodeMap[]   = new char[27 * 2];
    private char mDayi3CodeMap[]   = new char[40 * 2];
    // private char mMatchChar[] = new char[21529];
    private char mPhrase[] = new char[256];
    private int  mTotalMatch = 0;
    private int  mMode = CANGJIE;
    private TableLoader mTable = TableLoader.mInstance;
    private CandidateSelect mSelect = null;
    private CandidateListener mListener = null;
    private StringBuffer mCangjieCode = new StringBuffer();
    private char[] nearestKey = new char[4];
    private char lastChineseKey = 0;
    private String mAppPath = "";

    public Cangjie(Context context) {
	mContext = context;

	ApplicationInfo appInfo = context.getApplicationInfo();
	
	for (int count = 0; count < mCodeInput.length; count++) {
	    mCodeInput[count] = 0;
	    for (int i = 0; i < mCodeInputNearest[count].length; i++) {
		mCodeInputNearest[count][i] = 0;
	    }
	}

	try {
	    mTable.setPath(appInfo.dataDir.getBytes("UTF-8"));
	} catch (UnsupportedEncodingException ex) {
	}
	mAppPath = appInfo.dataDir;

	// updateDatabase();

	mTable.initialize();

	// Log.i("Cangjie", "Input Method List " + mTable.getInputMethodCount());
	// for (int count = 0; count < mTable.getInputMethodCount(); count++) {
	//     Log.i("Cangjie", "Input Method Name " + count + " " + mTable.getInputMethodNameList()[count]);
	// }

	boolean sorting = PreferenceManager.getDefaultSharedPreferences(mContext).getBoolean("cangjie_sorting_mode", true);
	mTable.setSortingMethod(sorting ? 0 : 1);
	
	loadCangjieKey();
	loadDayi3Key();
    }
    /*
    private void updateDatabase() {
	try {
	    File db = new File(mAppPath + "/phrase.db");
	    if (!db.exists()) {
		InputStream is = mContext.getAssets().open("phrase.db");
		OutputStream os = new FileOutputStream(mAppPath + "/phrase.db");
 
		byte[] buffer = new byte[1024];
		while (is.read(buffer) > 0) {
		    os.write(buffer);
		}
		os.flush();
		os.close();
		is.close();
	    }
	} catch (Exception ex) {
	}
    }
    */

    public boolean hasMatch() {
	// if (mSelect.getState() == CandidateSelect.CHARACTER_MODE) {
	    return mTable.totalMatch() > 0;
	// } else {
	//     return mTable.getPhraseCount() > 0;
	// }
    }

    public boolean isFull() {
	// Log.i("Cangjie", "Cangjie is Full " + mCodeCount + " " + mTable.getMaxKey());
	return mCodeCount >= mTable.getMaxKey();
    }
    
    public void setCandidateSelect(CandidateSelect select) {
	mSelect = select;
	mSelect.setCandidateListener(this);
    }

    public void setCandidateListener(CandidateListener listen) {
	mListener = listen;
    }

    public void sendFirstCharacter() {
	// Log.i("Cangjie", "Send First Character 0 ");
	if (mSelect.getState() == CandidateSelect.CHARACTER_MODE) {
	// Log.i("Cangjie", "Send First Character 1 ");
	    char c = (char) 0;
	    if (mListener != null && mTable.totalMatch() > 0) {
		c = mTable.getMatchChar(0);
		if (isAutoLearning() && lastChineseKey != 0) mTable.learnPhrase(lastChineseKey, c);
		lastChineseKey = c;
		mListener.characterSelected(mTable.getMatchChar(0), 0);
	    }
	    resetState();
	// Log.i("Cangjie", "Send First Character 1-1 ");
	    if ((int) c != 0) updatePhrase(c);
	// Log.i("Cangjie", "Send First Character 1-2 ");
	} else if (mSelect.getState() == CandidateSelect.PHRASE_MODE) {
	// Log.i("Cangjie", "Send First Character 2 ");
	    char c = (char) 0;
	    if (mListener != null && mTable.getPhraseCount() > 0) {
		int len = mTable.getPhraseArray(mTable.getPhraseIndex(), mPhrase);
	// Log.i("Cangjie", "Send First Character 3 ");
		if (len > 0) {
		    mListener.phraseSelected(new String(mPhrase, 0, len), 0);
		    c = mPhrase[0];
		}
	    }
	    resetState();
	    if ((int) c != 0) updatePhrase(c);
	}
    }

    public void resetFrequency() {
	mTable.clearAllFrequency();
	// updateDatabase();
	mTable.initialize();
    }
    
    public char getFirstCharacter() {
	return mTable.getMatchChar(0);
    }

    public void saveMatch() {
	mTable.saveMatch();
    }

    private void updatePhrase(char c) {
	// Log.i("Cangjie", "Update Phrase 0");
	mTable.searchPhrase(c);
	// Log.i("Cangjie", "Update Phrase 1");
	// Log.i("Cangjie", "Phrase(Java) " +
	//       mTable.getPhraseCount() + " " + 
	//       mTable.getPhraseIndex() + " " + 
	//       mTable.getPhraseMax() + " " + c);
	// StringBuffer sb = new StringBuffer();
	// for (int count = 0; count < mTable.getPhraseCount(); count++) {
	//     sb.setLength(0);
	//     mTable.getPhrase(count + mTable.getPhraseIndex(), sb);
	//     // Log.i("Cangjie", "Phrase(Java) Phrase = " + sb.toString());
	//     mPhrase[count] = sb.toString();
	// }
	if (mTable.getPhraseCount() <= 0 || mSelect == null)
	    return;

	// Log.i("Cangjie", "Update Phrase 2");
	mSelect.updatePhrase(mTable);
	// Log.i("Cangjie", "Update Phrase 3");
    }

    public boolean isCode(int primaryCode) {
	// Log.i("Cangjie", "Is Code " + primaryCode);
	if (mMode == STROKE) {
	    // Log.i("Cangjie", "Convert isCode " + primaryCode);
	    if ((char) primaryCode == '一' ||
		(char) primaryCode == '丨' ||
		(char) primaryCode == '丿' ||
		(char) primaryCode == '丶' ||
		(char) primaryCode == '乛' ||
		(char) primaryCode == '＊') {
	    // Log.i("Cangjie", "Convert isCode OK " + primaryCode);
		return true;
	    }

	    return false;
	}

	if (isCangjieEnglishKey() || mMode == CANTONESE) {
	    if (primaryCode >= 'A' && primaryCode <= 'Z') {
		return true;
	    }
	}
	if (mMode == DAYI3) {
	// Log.i("Cangjie", "Is Code0 " + primaryCode);
	    for (int count = 0; count < mDayi3CodeMap.length; count += 2) {
		if (mDayi3CodeMap[count] == primaryCode)
		    return true;
	    }
	// Log.i("Cangjie", "Is Code1 " + primaryCode);
	    return false;
	}
	for (int count = 0; count < mCodeMap.length; count += 2) {
	    if (mCodeMap[count] == primaryCode)
		return true;
	}

	return false;
    }
    
    public void characterSelected(char c, int idx) {
	if (isAutoLearning() && lastChineseKey != 0) {
	    mTable.learnPhrase(lastChineseKey, c);
	    lastChineseKey = c;
	}
	mTable.updateFrequency(c);
	if (mListener != null) mListener.characterSelected(c, idx);
	resetState();
	updatePhrase(c);
    }
    
    public void phraseSelected(String phrase, int idx) {
	if (isAutoLearning() && lastChineseKey != 0) {
	    char c = phrase.charAt(phrase.length() - 1);
	    mTable.learnPhrase(lastChineseKey, c);
	    lastChineseKey = c;
	}
	if (idx > 0) mTable.updatePhraseFrequency(idx);
	if (mListener != null) mListener.phraseSelected(phrase, idx);
	resetState();
	updatePhrase(phrase.charAt(phrase.length() - 1));
    }

    public void resetLearning() {
	lastChineseKey = 0;
    }

    public boolean isAutoLearning() {
	return PreferenceManager.getDefaultSharedPreferences(mContext).getBoolean("auto_learning_phrase", false);
    }

    public void resetState() {
	boolean sorting = PreferenceManager.getDefaultSharedPreferences(mContext).getBoolean("cangjie_sorting_method", true);
	mTable.setSortingMethod(sorting ? 0 : 1);

	mCangjieCode.setLength(0);
	mTable.reset();
	for (int count = 0; count < mCodeInput.length; count++) {
	    mCodeInput[count] = 0;
	    for (int i = 0; i < mCodeInputNearest[count].length; i++) {
		mCodeInputNearest[count][i] = 0;
	    }
	}
	mCodeCount = 0;
	if (mSelect != null) {
	    mSelect.updateTable(null);
	    mSelect.closePopup();
	}
	String value = PreferenceManager.getDefaultSharedPreferences(mContext).getString("cangjie_mode", "0");
	if (value.compareTo("1") == 0) {
	    mTable.setInputMethod('1');
	    mMode = QUICK;
	} else if (value.compareTo("3") == 0) {
	    mTable.setInputMethod('3');
	    mTable.enableHongKongChar(true);
	    mMode = STROKE;
	} else if (value.compareTo("2") == 0) {
	    mTable.setInputMethod('2');
	    mTable.enableHongKongChar(true);
	    mMode = CANGJIE;
	} else if (value.compareTo("4") == 0) {
	    mTable.setInputMethod('4');
	    mTable.enableHongKongChar(false);
	    mMode = CANGJIE;
	} else if (value.compareTo("6") == 0) {
	    mTable.setInputMethod('6');
	    mTable.enableHongKongChar(false);
	    mMode = CANTONESE;
	} else if (value.compareTo("5") == 0) {
	    mTable.setInputMethod('5');
	    mTable.enableHongKongChar(false);
	    mMode = DAYI3;
	} else {
	    mTable.setInputMethod('0');
	    mTable.enableHongKongChar(false);
	    mMode = CANGJIE;
	}
    }
    
    private void loadCangjieKey() {
	try {
	    InputStream is = mContext.getResources().openRawResource(R.raw.cj_key);
	    InputStreamReader input = new InputStreamReader(is, "UTF-8");
	    BufferedReader reader = new BufferedReader(input);
	    String str = null;
	    int count = 0, index = 0;
	    char c = 'a';
	
	    do {
		str = reader.readLine();
		mCodeMap[count + 1] = str.charAt(0);
		mCodeMap[count    ] = str.charAt(2);
		count += 2;
	    } while (str != null && count < mCodeMap.length);
		    
	    reader.close();

	} catch (Exception ex) {
	    ex.printStackTrace();
	}
    }

    private void loadDayi3Key() {
	try {
	    InputStream is = mContext.getResources().openRawResource(R.raw.dayi3_key);
	    InputStreamReader input = new InputStreamReader(is, "UTF-8");
	    BufferedReader reader = new BufferedReader(input);
	    String str = null;
	    int count = 0, index = 0;
	
	    do {
		str = reader.readLine();
		mDayi3CodeMap[count + 1] = str.charAt(0);
		mDayi3CodeMap[count    ] = str.charAt(2);
		count += 2;
	    } while (str != null && count < mDayi3CodeMap.length);
		    
	    reader.close();

	} catch (Exception ex) {
	    ex.printStackTrace();
	}
    }

    private char convertPrimaryCode(int primaryCode) {
	if (mMode == STROKE) {
	    // Log.i("Cangjie", "Convert Primary Code " + primaryCode);
	    if ((char) primaryCode == '一')
		return '1';
	    if ((char) primaryCode == '丨')
		return '2';
	    if ((char) primaryCode == '丿')
		return '3';
	    if ((char) primaryCode == '丶')
		return '4';
	    if ((char) primaryCode == '乛')
		return '5';
	    if ((char) primaryCode == '＊')
		return '*';
	}
	if (isCangjieEnglishKey() || mMode == CANTONESE) {
	    if (primaryCode >= 'A' && primaryCode <= 'Z') {
		return (char) (primaryCode | 0x20);
	    }
	}
	if (mMode == DAYI3) {
	    for (int count = 0; count < mDayi3CodeMap.length; count += 2) {
		// Log.i("Cangjie", "Daiyi Convert " + mDayi3CodeMap[count] + " " + primaryCode + " " + mDayi3CodeMap[count + 1]);
		if (mDayi3CodeMap[count] == primaryCode)
		    return mDayi3CodeMap[count + 1];
	    }
	    return 0;
	}
	for (int count = 0; count < mCodeMap.length; count += 2) {
	    if (mCodeMap[count] == primaryCode)
		return mCodeMap[count + 1];
	}

	return 0;
    }

    private void convertPrimaryCodeNearest(char[] key, char output[]) {
	boolean changed = false;
	for (int i = 0; i < key.length; i++) {
	    changed = false;
	    if (key[i] == 0 || key[i] == -1) {
		output[i] = 0;
		continue;
	    }
	    if (isCangjieEnglishKey()) {
		if (key[i] >= 'A' && key[i] <= 'Z') {
		    output[i] = (char) (key[i] | 0x20);
		    changed = true;
		    continue;
		}
	    }
	    if (!changed) {
		if (mMode == DAYI3) {
		    for (int count = 0; count < mDayi3CodeMap.length; count += 2) {
			if (mDayi3CodeMap[count] == key[i])
			    output[i] = mDayi3CodeMap[count + 1];
			changed = true;
			break;
		    }
		} else {
		    for (int count = 0; count < mCodeMap.length; count += 2) {
			if (mCodeMap[count] == key[i]) {
			    output[i] = mCodeMap[count + 1];
			    changed = true;
			    break;
			}
		    }
		}
	    }
	    if (!changed) output[i] = 0;
	}
    }
    
    public boolean handleCharacter(ProximityInfo proximity, int x, int y, int primaryCode) {
	if (mCodeCount >= mTable.getMaxKey()) return false;
	char code = convertPrimaryCode(primaryCode);

	if (mCodeCount == 0 && code == '*') return false;

	int i = 1;
	if (code == 0) return false;

	for (int count = 0; count < nearestKey.length; count++) nearestKey[count] = 0;

	final com.diycircuits.inputmethod.keyboard.Key[] keys = proximity.getNearestKeys(x, y);
	int m = 1;
	nearestKey[0] = (char) primaryCode;
	if (primaryCode != 65290) {
	    for (int c = 0; c < keys.length; c++) {
		if (isCangjieEnglishKey() && m < nearestKey.length && keys[c].mCode != primaryCode) {
		    if (keys[c].mCode != 65290) nearestKey[m++] = (char) keys[c].mCode;
		} else {
		    if (keys[c].mCode > 255 && m < nearestKey.length &&
			keys[c].mCode != primaryCode && keys[c].mCode != 65290) nearestKey[m++] = (char) keys[c].mCode;
		}
	    }
	}
	convertPrimaryCodeNearest(nearestKey, mCodeInputNearest[mCodeCount]);
	// for (int count = 0; count < mCodeInputNearest[mCodeCount].length; count++) {
	//     Log.i("Cangjie", "Code Count " + (int) mCodeInputNearest[mCodeCount][count]);
	// }
	mCodeInput[mCodeCount] = code;
	mCodeCount++;
	if (matchCangjie()) {
	    mCangjieCode.append((char) primaryCode);
	    // Log.i("Cangjie", "Match Cangjie " + mCodeCount);
	    return true;
	}

	mCodeCount--;
	mCodeInput[mCodeCount] = 0;
	for (int count = 0; count < mCodeInputNearest[mCodeCount].length; count++)
	    mCodeInputNearest[mCodeCount][count] = 0;

	return false;
    }
    
    public void deleteLastCode() {
	if (mCodeCount <= 0) return;

	mCodeCount--;
	mCodeInput[mCodeCount] = 0;
	// Log.i("Cangjie", "Delete Last Code Cangjie " + mCodeCount);
	for (int count = 0; count < mCodeInputNearest[mCodeCount].length; count++)
	    mCodeInputNearest[mCodeCount][count] = 0;
	if (mCangjieCode.length() > 0) {
	    mCangjieCode.setLength(mCangjieCode.length() - 1);
	}

	if (mCodeCount == 0) {
	    mTable.reset();
	    mSelect.updateTable(null);
	} else { 
	    matchCangjie();
	}
    }

    public String getCangjieCode() {
	return mCangjieCode.toString();
    }

    private boolean isCangjieEnglishKey() {
	return PreferenceManager.getDefaultSharedPreferences(mContext).getBoolean("cangjie_english_key", false);
    }
    
    private boolean matchCangjie() {
	boolean autocorrection = PreferenceManager.getDefaultSharedPreferences(mContext).getBoolean("cangjie_autocorrection_mode", false);
	boolean res;

	if (mMode == STROKE || mMode == CANTONESE) {
           // StringBuffer sb = new StringBuffer();
           // for (int count = 0; count < 64; count++) {
           //  if (mCodeInput[count] >= '1' && mCodeInput[count] <= '5') {
           //      sb.append(mCodeInput[count]);
           //      sb.append(' ');
           //  } else {
           //      sb.append('0');
           //      sb.append(' ');
           //  }
           // }
           // Log.i("Cangjie", "Stroke Input " + sb.toString());

	    if (mTable.trySearchWord(mCodeInput, mCodeCount)) {
		mTable.searchWord(mCodeInput, mCodeCount);
		mSelect.updateTable(mTable);
		return true;
	    }
	    return false;
	}
	
	if (autocorrection && mMode != CANTONESE && mMode != DAYI3) {
	    res = mTable.tryMatchCangjieMore(mCodeInputNearest[0], mCodeInputNearest[1], mCodeInputNearest[2], mCodeInputNearest[3], mCodeInputNearest[4]);
	} else {
	    // Log.i("Cangjie", "Try Match  " +
	    // 	  mCodeInput[0] + " " +
	    // 	  mCodeInput[1] + " " +
	    // 	  mCodeInput[2] + " " +
	    // 	  mCodeInput[3] + " ");
	    res = mTable.tryMatchCangjie(mCodeInput[0], mCodeInput[1], mCodeInput[2], mCodeInput[3], mCodeInput[4]);
	}

	if (res) {
	    if (autocorrection && mMode != CANTONESE && mMode != DAYI3) {
		mTable.searchCangjieMore(mCodeInputNearest[0], mCodeInputNearest[1], mCodeInputNearest[2], mCodeInputNearest[3], mCodeInputNearest[4]);
	    } else {
		mTable.searchCangjie(mCodeInput[0], mCodeInput[1], mCodeInput[2], mCodeInput[3], mCodeInput[4]);
	    }
	    mSelect.updateTable(mTable);
	}
	
	return res;
    }
    
}

