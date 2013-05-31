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

    public void setContext(Context context) {
	mContext = context;

	if (mAdapter == null) mAdapter = new ListAdapter(context, 0); // , mItemObject);
    }
    
    public void setCandidateSelectWidget(CandidateSelectWidget widget) {
	mSelectWidget = widget;
    }
    
    public void setCandidateList(HorizontalVariableListView list) {
	mSelectList = list;
	mSelectList.setAdapter(mAdapter);

	mSelectList.setOnItemClickedListener( new OnItemClickedListener() {

		@Override
		public boolean onItemClick( AdapterView<?> parent, View view, int position, long id ) {
		    Log.i("Cangjie", "onItemClick: " + position + " " + mLoader.getMatchChar(position >> 1));

		    return true;
		}
	    } );
    }
    
    public static boolean isCandidatePopup(Context mContext) {
	String value = PreferenceManager.getDefaultSharedPreferences(mContext).getString("cangjie_mode", "0");

	return value.compareTo("1") == 0;
    }

    public void updateTable(TableLoader loader) {
	Log.i("Cangjie", "Update Table");
	if (isCandidatePopup(mContext)) {
	Log.i("Cangjie", "Update Table 0");
	    mSelectWidget.updatePhrase(loader);
	} else {
	Log.i("Cangjie", "Update Table 1 " + mSelectList);
	    mLoader = loader;
	    mState = CHARACTER_MODE;
	    if (mSelectList != null) mSelectList.setAdapter(mAdapter);
	}
    }

    public void updatePhrase(TableLoader loader) {
	if (isCandidatePopup(mContext)) {
	    mSelectWidget.updatePhrase(loader);
	} else {
	    mLoader = loader;
	    mState = PHRASE_MODE;
	    mSelectList.setAdapter(mAdapter);
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
	    Log.i("Cangjie", "ListAdapter onTouch");
	    if (event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_MOVE) {
		v.setBackgroundColor(0xff33B5E5);
	    } else if (event.getAction() == MotionEvent.ACTION_UP || event.getAction() == MotionEvent.ACTION_CANCEL) {
		v.setBackgroundColor(0x00000000);
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
	    Log.i("Cangjie", "List Adapter Get Item " + pos);
	    sb.setLength(0);
	    if (mLoader != null) sb.append(mLoader.getMatchChar(pos));

	    return sb.toString();
	}

	@Override
	public long getItemId(int pos) {
	    Log.i("Cangjie", "List Adapter Get Item Id " + pos);
	    return pos;
	}

	@Override
	public int getCount() {
	    if (mLoader != null) {
		Log.i("Cangjie", "List Adapter Get Count " + mLoader.totalMatch());
		return mLoader.totalMatch();
	    } else
		return 0;
	}

	@Override
	public View getView( int position, View convertView, ViewGroup parent ) {

	    Log.i("Cangjie", "List Adapter Get View " + position);
	    
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
		text.setOnTouchListener(this);

		if (mLoader.getFrequency(position >> 1) > 0) 
		    text.setTextColor(0xffff9000);
		else
		    text.setTextColor(0xff33B5E5);
		text.setTextSize(TypedValue.COMPLEX_UNIT_SP, 32);
		sb.setLength(0);
		if (mLoader != null) sb.append(mLoader.getMatchChar(position >> 1));
		Log.i("Cangjie", "Get View " + sb.toString());
		text.setText(sb.toString());
	    }

	    return view;
	}

    }

}
    
