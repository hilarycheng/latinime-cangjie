package com.diycircuits.cangjie;

import com.diycircuits.inputmethod.latin.R;

import android.content.Context;
import android.preference.PreferenceManager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.TextView;
import android.graphics.Color;
import android.util.TypedValue;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Gravity;
import java.util.ArrayList;
import java.util.List;

import it.sephiroth.android.library.widget.HorizontalListView.OnLayoutChangeListener;
import it.sephiroth.android.library.widget.HorizontalVariableListView;
import it.sephiroth.android.library.widget.HorizontalVariableListView.OnItemClickedListener;
import it.sephiroth.android.library.widget.HorizontalVariableListView.SelectionMode;

public class CandidateSelect {

    public static final int CHARACTER      = 0;
    public static final int PHRASE         = 1;
    public static final int CHARACTER_MODE = 0;
    public static final int PHRASE_MODE    = 1;

    private int mState = CHARACTER_MODE;
    private CandidateListener mListener = null;
    private Context mContext = null;
    private CandidateSelectWidget mSelectWidget = null;
    private HorizontalVariableListView mSelectList = null;
    private TableLoader mLoader = null;
    private ListAdapter mAdapter = null;
    private char[] mPhrase = null;
    private StringBuffer phraseBuffer = new StringBuffer();
    private int mFontSize = 32;
    private int mCalculatedFontSize = 32;

    public void setContext(Context context) {
	mContext = context;

	if (mAdapter == null) mAdapter = new ListAdapter(context, 0); // , mItemObject);
    }

    public void setFontSize(int size) {
	mFontSize = size;
	mCalculatedFontSize = size;

    }

    public void updateFontSize() {
	mFontSize = mCalculatedFontSize;

	int percent = PreferenceManager.getDefaultSharedPreferences(mContext).getInt("candidate_font_size",
											 1000);
	mFontSize = (mFontSize * percent) / 1000;

	if (mSelectWidget != null) mSelectWidget.updateFontSize();
    }
    
    public void setCandidateSelectWidget(CandidateSelectWidget widget) {
	mSelectWidget = widget;
    }

    private String getPhrase(int index) {
	phraseBuffer.setLength(0);

	int len = mLoader.getPhraseArray(mLoader.getPhraseIndex() + index, mPhrase);
	phraseBuffer.append(mPhrase, 0, len);

	return phraseBuffer.toString();
    }
    
    public void setCandidateList(HorizontalVariableListView list) {
	mSelectList = list;
	if (mSelectList != null) {
	    mSelectList.setDataChanged(true);
	    mSelectList.setAdapter(mAdapter);
	}

	mSelectList.setOnItemClickedListener( new OnItemClickedListener() {

		@Override
		public boolean onItemClick( AdapterView<?> parent, View view, int position, long id ) {
		    if (mState == CHARACTER_MODE) {
			if (mListener != null) mListener.characterSelected(mLoader.getMatchChar(position >> 1), position >> 1);
		    } else if (mState == PHRASE_MODE) {
			if (mListener != null) mListener.phraseSelected(getPhrase(position >> 1), mLoader.getPhraseIndex() + (position >> 1));
		    }

		    return true;
		}
	    } );
    }
    
    public static boolean isCandidatePopup(Context mContext) {
	return PreferenceManager.getDefaultSharedPreferences(mContext).getBoolean("candidate_popup", true);
    }

    public void updateTable(TableLoader loader) {
	if (isCandidatePopup(mContext)) {
	    mSelectWidget.updateTable(loader);
	} else {
	    mLoader = loader;
	    mState = CHARACTER_MODE;
	    if (mSelectList != null) {
		mSelectList.setDataChanged(true);
		mSelectList.setAdapter(mAdapter);
		mAdapter.notifyDataSetChanged();
	    }
	}
    }

    public void updatePhrase(TableLoader loader) {
	if (isCandidatePopup(mContext)) {
	    mSelectWidget.updatePhrase(loader);
	} else {
	    mLoader = loader;
	    mPhrase = new char[mLoader.getAllPhraseMax()];
	    mState = PHRASE_MODE;
	    if (mSelectList != null) {
		mSelectList.setDataChanged(true);
		mSelectList.setAdapter(mAdapter);
		mAdapter.notifyDataSetChanged();
	    }
	}
    }

    public void closePopup() {
	if (isCandidatePopup(mContext)) mSelectWidget.closePopup();
    }

    public int getState() {
	if (isCandidatePopup(mContext)) {
	    return mSelectWidget.getState();
	} else {
	    return mState;
	}
    }

    public void setCandidateListener(CandidateListener listen) {
	mListener = listen;
	if (isCandidatePopup(mContext)) mSelectWidget.setCandidateListener(listen);
    }

    class ListAdapter extends ArrayAdapter<String> implements View.OnTouchListener {

	private StringBuffer sb = new StringBuffer();
	
	public ListAdapter(Context context, int item) { //, List<String> mItemObject) {
	    super(context, R.layout.candidateitem); // , mItemObject);
	}

	public boolean onTouch(View v, MotionEvent event) {
	    if (event.getAction() == MotionEvent.ACTION_DOWN) {
		v.setBackgroundColor(0xff33B5E5);
	    } else if (event.getAction() == MotionEvent.ACTION_UP || event.getAction() == MotionEvent.ACTION_CANCEL) {
		v.setBackgroundColor(0xff272727);
	    }
	    return false;
	}

	@Override
        public int getItemViewType( int position ) {
	    return position & 1;
	}

	@Override
	public int getViewTypeCount() {
	    return 2;
	}

	@Override
	public String getItem(int pos) {
	    if (mState == CHARACTER_MODE) {
		sb.setLength(0);
		if (mLoader != null) sb.append(mLoader.getMatchChar(pos >> 1));
	    } else if (mState == PHRASE_MODE) {
		return getPhrase(pos >> 1);
	    }

	    return sb.toString();
	}

	@Override
	public long getItemId(int pos) {
	    return pos;
	}

	@Override
	public int getCount() {
	    if (mLoader != null) {
		if (mState == CHARACTER_MODE) {
		    return mLoader.totalMatch() * 2;
		} else if (mState == PHRASE_MODE) {
		    return mLoader.getPhraseCount() * 2;
		}
	    }

	    return 0;
	}

	@Override
	public View getView( int position, View convertView, ViewGroup parent ) {

	    View view;
	    int type = getItemViewType(position);
	    
	    if ( convertView == null ) {
		view = LayoutInflater.from(getContext()).inflate(type == 0 ? R.layout.candidateitem : R.layout.divider, parent, false);
		view.setLayoutParams( new HorizontalVariableListView.LayoutParams(HorizontalVariableListView.LayoutParams.WRAP_CONTENT,
										  HorizontalVariableListView.LayoutParams.WRAP_CONTENT));
	    } else {
		view = convertView;
	    }

	    if (type == 0) {
		TextView text = (TextView) view.findViewById(R.id.text);
		text.setGravity(Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL);
		text.setOnTouchListener(this);

		if (mState == CHARACTER_MODE) {
		    if (mLoader.getFrequency(position >> 1) > 0) 
			text.setTextColor(0xffff9000);
		    else
			text.setTextColor(0xff33B5E5);
		    text.setTextSize(TypedValue.COMPLEX_UNIT_PX, mFontSize);
		    sb.setLength(0);
		    if (mLoader != null) sb.append(mLoader.getMatchChar(position >> 1));
		    text.setText(sb.toString());
		} else if (mState == PHRASE_MODE) {
		    if (mLoader.getPhraseFrequency(mLoader.getPhraseIndex() + (position >> 1)) > 0) 
			text.setTextColor(0xffff9000);
		    else
			text.setTextColor(0xff33B5E5);
		    text.setTextSize(TypedValue.COMPLEX_UNIT_PX, mFontSize);
		    text.setText(getPhrase(position >> 1));
		}
	    }

	    return view;
	}

    }

}
    
