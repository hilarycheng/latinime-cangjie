package com.diycircuits.cangjie;

import com.diycircuits.inputmethod.latin.R;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.util.Log;
import android.preference.PreferenceManager;

import java.util.HashMap;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.UnsupportedEncodingException;
import com.diycircuits.cangjie.CandidateSelect.CandidateListener;

public class Cangjie implements CandidateListener {

    public final static int QUICK   = 0;
    public final static int CANGJIE = 1;

    private Context mContext = null;
    private char mCodeInput[] = new char[5];
    private char mCodeInputNearest[][] = new char[5][6];
    private char mKeyList[] = new char[6];
    private int  mCodeCount = 0;
    private char mCodeMap[]   = new char[27 * 2];
    private char mMatchChar[] = new char[21529];
    private int  mTotalMatch = 0;
    private int  mMode = CANGJIE;
    private TableLoader mTable = new TableLoader();
    private CandidateSelect mSelect = null;
    private CandidateListener mListener = null;
    private StringBuffer mCangjieCode = new StringBuffer();

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

	mTable.initialize();
	
	loadCangjieKey();
    }

    public boolean hasMatch() {
	return mTable.totalMatch() > 0;
    }

    public boolean isFull() {
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
	if (mListener != null && mTable.totalMatch() > 0) {
	    mListener.characterSelected(mTable.getMatchChar(0), 0);
	}
	resetState();
    }

    public void resetFrequency() {
	mTable.clearAllFrequency();
    }
    
    public char getFirstCharacter() {
	return mTable.getMatchChar(0);
    }

    public void saveMatch() {
	mTable.saveMatch();
    }
    
    public void characterSelected(char c, int idx) {
	mTable.updateFrequency(c);
	if (mListener != null) mListener.characterSelected(c, idx);
	resetState();
    }

    public void resetState() {
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
	    // mSelect.updateMatch(null, 0);
	    mSelect.updateTable(null);
	    mSelect.closePopup();
	}
	String value = PreferenceManager.getDefaultSharedPreferences(mContext).getString("cangjie_mode", "0");
	if (value.compareTo("1") == 0) {
	    mTable.setInputMethod(TableLoader.QUICK);
	    mMode = QUICK;
	} else if (value.compareTo("2") == 0) {
	    mTable.setInputMethod(TableLoader.CANGJIE);
	    mTable.enableHongKongChar(true);
	    mMode = CANGJIE;
	} else {
	    mTable.setInputMethod(TableLoader.CANGJIE);
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
    
    private char convertPrimaryCode(int primaryCode) {
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
	    for (int count = 0; count < mCodeMap.length; count += 2) {
		if (mCodeMap[count] == key[i]) {
		    output[i] = mCodeMap[count + 1];
		    changed = true;
		    break;
		}
	    }
	    if (!changed) output[i] = 0;
	}
    }
    
    public boolean handleCharacter(int primaryCode, int[] nearestKey) {
	if (mCodeCount >= mTable.getMaxKey()) return false;
	char code = convertPrimaryCode(primaryCode);
	int i = 1;
	for (int count = 0; count < mKeyList.length; count++) {
	    mKeyList[count] = 0;
	}
	if (code == 0) return false;

	mKeyList[0] = (char) primaryCode;
	for (int count = 0; count < nearestKey.length; count++) {
	    if (nearestKey[count] != primaryCode) {
		mKeyList[i] = (char) nearestKey[count];
		i++;
	    }
	}
	convertPrimaryCodeNearest(mKeyList, mCodeInputNearest[mCodeCount]);
	mCodeInput[mCodeCount] = code;

	for (int count = 0; count < mCodeInput.length; count++) {
	    if (mCodeInput[count] == '*') {
		for (int j = 1; j < mCodeInputNearest[count].length; j++)
		    mCodeInputNearest[count][j] = 0;
	    } else {
		for (int j = 1; j < mCodeInputNearest[count].length; j++)
		    if (mCodeInputNearest[count][j] == '*') mCodeInputNearest[count][j] = 0;
	    }
	}
	if (code == 'a') mCodeInputNearest[mCodeCount][1] = 's';
	if (code == 'v') {
	    mCodeInputNearest[mCodeCount][1] = 'c';
	    mCodeInputNearest[mCodeCount][2] = 'b';
	}
	if (code == 'b') mCodeInputNearest[mCodeCount][2] = 'n';
	
	if (matchCangjie()) {
	    mCangjieCode.append((char) primaryCode);
	    mCodeCount++;
	    return true;
	}
	mCodeInput[mCodeCount] = 0;
	for (int count = 0; count < mCodeInputNearest[mCodeCount].length; count++)
	    mCodeInputNearest[mCodeCount][count] = 0;

	return false;
    }
    
    public void deleteLastCode() {
	if (mCodeCount <= 0) return;

	mCodeCount--;
	mCodeInput[mCodeCount] = 0;
	for (int count = 0; count < mCodeInputNearest[mCodeCount].length; count++)
	    mCodeInputNearest[mCodeCount][count] = 0;
	if (mCangjieCode.length() > 0) {
	    mCangjieCode.setLength(mCangjieCode.length() - 1);
	}

	if (mCodeCount == 0) {
	    mTable.reset();
	    // mSelect.updateMatch(null, 0);
	    mSelect.updateTable(null);
	} else { 
	    matchCangjie();
	}
    }

    public String getCangjieCode() {
	return mCangjieCode.toString();
    }
    
    private boolean matchCangjie() {
	// boolean res = mTable.tryMatchCangjie(mCodeInput[0], mCodeInput[1], mCodeInput[2], mCodeInput[3], mCodeInput[4]);

	boolean res = mTable.tryMatchCangjieMore(mCodeInputNearest[0], mCodeInputNearest[1], mCodeInputNearest[2], mCodeInputNearest[3], mCodeInputNearest[4]);

	if (res) {
	    // mTable.searchCangjie(mCodeInput[0], mCodeInput[1], mCodeInput[2], mCodeInput[3], mCodeInput[4]);
	    mTable.searchCangjieMore(mCodeInputNearest[0], mCodeInputNearest[1], mCodeInputNearest[2], mCodeInputNearest[3], mCodeInputNearest[4]);
	    // for (int count = 0; count < mTable.totalMatch(); count++) {
	    // 	mMatchChar[count] = mTable.getMatchChar(count);
	    // }
	    // mSelect.updateMatch(mMatchChar, mTable.totalMatch());
	    mSelect.updateTable(mTable);
	}
	
	return res;
    }
    
}

