package com.diycircuits.cangjie;

import it.sephiroth.android.library.widget.HorizontalListView.OnLayoutChangeListener;
import it.sephiroth.android.library.widget.HorizontalVariableListView;
import it.sephiroth.android.library.widget.HorizontalVariableListView.OnItemClickedListener;
import it.sephiroth.android.library.widget.HorizontalVariableListView.SelectionMode;
import android.preference.PreferenceManager;
import com.diycircuits.inputmethod.latin.R;
import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.widget.*;
import android.graphics.*;
import android.view.LayoutInflater;
import android.util.Log;

public class CandidateView extends LinearLayout implements View.OnClickListener {

    private final static int STARTING_FONT_SIZE = 12;
    private final static int ENDING_FONT_SIZE   = 128;

    private int width = 0;
    private int height = 0;
    private char match[] = null;
    private int total = 0;
    private CandidateSelectWidget mSelectWidget = null;
    private HorizontalVariableListView mSelectList = null;
    private PopupWindow mPopup = null;
    private Context mContext = null;
    private CandidateView mCandidate = this;
    private int mWidth = 0;
    private int mHeight = 0;
    private View mParent = null;
    private CandidateSelect mSelect = new CandidateSelect();
    private Paint paint = null;
    private float mFontSize = 50.0f;

    public CandidateView(Context context, AttributeSet attrs) {
	super(context, attrs);
	mContext = context;

	paint = new Paint();
	paint.setColor(Color.BLACK);
	paint.setAntiAlias(true);
	paint.setTextSize(mFontSize);
	paint.setStrokeWidth(0);
    }

    public void setParent(View view) {
	this.mParent = view;
    }
    
    public void setDimension(int w, int h) {
	mWidth = w;
	mHeight = h;
    }
    
    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
	super.onSizeChanged(w, h, oldw, oldh);

	mWidth = w;
	mHeight = h;

	mSelectWidget.setParentWidth(mWidth);

	for (int fontsize = STARTING_FONT_SIZE; fontsize < ENDING_FONT_SIZE; fontsize += 2) {
	    paint.setTextSize(fontsize);

	    Paint.FontMetrics metrics = paint.getFontMetrics();
	    int totalHeight = (int) (metrics.bottom - metrics.top);

	    if (totalHeight > mHeight) {
		mFontSize = (float) (fontsize - 8);
		if (mSelect != null) mSelect.setFontSize((int) mFontSize);
		break;
	    }
	}
    }

    public void setReferenceSize(int w, int h) {
	mWidth = w;
	mHeight = h;
    }

    public CandidateSelect getCandidateSelect() {
	return mSelect;
    }

    public void updateCandidate(boolean isCangjieMode) {
	if (!isCangjieMode) {
	    findViewById(R.id.arrow_left).setVisibility(View.GONE);
	    findViewById(R.id.arrow_right).setVisibility(View.GONE);
	    return;
	}
	
	if (CandidateSelect.isCandidatePopup(mContext)) {
	    mSelectWidget = (CandidateSelectWidget) findViewById(R.id.match_view);
	    mSelectWidget.setVisibility(View.VISIBLE);
	    mSelectList = (HorizontalVariableListView) findViewById(R.id.candidateList);
	    mSelectList.setVisibility(View.GONE);
         
	    if (isLeftCandidateButton()) {
		findViewById(R.id.arrow_left).setVisibility(View.VISIBLE);
		findViewById(R.id.arrow_right).setVisibility(View.GONE);
	    } else {
		findViewById(R.id.arrow_left).setVisibility(View.GONE);
		findViewById(R.id.arrow_right).setVisibility(View.VISIBLE);
	    }
	} else {
	    mSelectWidget = (CandidateSelectWidget) findViewById(R.id.match_view);
	    mSelectWidget.setVisibility(View.GONE);
	    mSelectList = (HorizontalVariableListView) findViewById(R.id.candidateList);
	    mSelectList.setVisibility(View.VISIBLE);
	    mSelectList.setSelectionMode(HorizontalVariableListView.SelectionMode.Single);
	    mSelectList.setBackgroundColor(0xff272727);
         
	    findViewById(R.id.arrow_left).setVisibility(View.GONE);
	    findViewById(R.id.arrow_right).setVisibility(View.GONE);
	}

	mSelect.setContext(mContext);
	mSelect.setCandidateSelectWidget(mSelectWidget);
	mSelect.setCandidateList(mSelectList);
	mSelect.setFontSize((int) mFontSize);
	mSelect.updateFontSize();

	findViewById(R.id.arrow_left).setOnClickListener(this);
	findViewById(R.id.arrow_right).setOnClickListener(this);
    }

    private boolean isLeftCandidateButton() {
	return PreferenceManager.getDefaultSharedPreferences(mContext).getBoolean("candidate_button", false);
    }

    public void onClick(View v) {
	synchronized(mSelectWidget) {
	    mSelectWidget.showCandidatePopup(mParent, mWidth, mHeight);
	}
    }

    @Override
    protected void onFinishInflate() {
	super.onFinishInflate();

	updateCandidate(true);
    }

}
