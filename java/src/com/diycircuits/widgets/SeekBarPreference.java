package com.diycircuits.widgets;

import android.content.Context;
import android.content.res.TypedArray;
import android.preference.Preference;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.*;
import android.view.ViewGroup;
import android.view.ViewParent;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.CheckBox;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.content.res.Configuration;
import android.preference.PreferenceManager;
import android.content.SharedPreferences;
import com.diycircuits.inputmethod.latin.R;

public class SeekBarPreference extends Preference implements OnSeekBarChangeListener, View.OnClickListener {
   
    private final String TAG = getClass().getName();
   
    private static final String ANDROIDNS="http://schemas.android.com/apk/res/android";
    private static final String FLATWORLDNS="http://flatworld.eu";
    private static final int DEFAULT_VALUE = 50;
   
    private int mMaxValue      = 800;
    private int mMinValue      = 300;
    private int mInterval      = 1;
    private int mCurrentValue;
    private int mDefaultValue  = 0;
    private boolean mDefaultChecked = true;
    
    private SeekBar mSeekBar;
    private TextView mStatusText;
    private CheckBox mDefault;
    private Context mContext = null;
    private int format = 0;
    private String mKey = "";

    public SeekBarPreference(Context context, AttributeSet attrs) {
        super(context, attrs);
	mContext = context;
        setShouldDisableView(true);
        initPreference(context, attrs);
    }

    public SeekBarPreference(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
	mContext = context;
        setShouldDisableView(true);
        initPreference(context, attrs);
    }

    public SeekBarPreference(Context context) {
	this(context, null);
    }

    private void updateValue() {
	if (getKey().compareTo("portrait_height") == 0 ||
	    getKey().compareTo("landscape_height") == 0) {
	    boolean checked = true;
	    if (mDefaultValue == 460) 
		checked = mContext.getSharedPreferences("CangjiePreference", Context.MODE_PRIVATE).getBoolean("landscape_height_default", true);
	    else
		checked = mContext.getSharedPreferences("CangjiePreference", Context.MODE_PRIVATE).getBoolean("portrait_height_default", true);

	    mDefaultChecked = checked;
	} else {
	    if (mKey == null || (mKey != null && mKey.length()  == 0)) {
		mDefaultChecked = (mCurrentValue == mDefaultValue);
	    }
	}

	if (mKey == null || (mKey != null && mKey.length()  == 0)) {
	    if (mStatusText != null) mStatusText.setEnabled(!mDefaultChecked);
	    if (mSeekBar != null) mSeekBar.setEnabled(!mDefaultChecked);
	} else {
	    if (mDefault != null) mDefault.setChecked(mDefaultChecked);
	    if (mStatusText != null) mStatusText.setEnabled(mDefaultChecked);
	    if (mSeekBar != null) mSeekBar.setEnabled(mDefaultChecked);
	}
    }
    
    private void initPreference(Context context, AttributeSet attrs) {
	setValuesFromXml(attrs);
	
	mDefaultValue = attrs.getAttributeIntValue(ANDROIDNS, "defaultValue", 0);
 
        mSeekBar = new SeekBar(context, attrs);
        mSeekBar.setMax(mMaxValue - mMinValue);
        mSeekBar.setOnSeekBarChangeListener(this);

	updateValue();
    }

    public void onClick(View v) {
	SharedPreferences.Editor edit = mContext.getSharedPreferences("CangjiePreference", Context.MODE_PRIVATE).edit();
	if (getKey().compareTo("portrait_height") == 0 ||
	    getKey().compareTo("landscape_height") == 0) {
	    if (mDefaultValue == 460) 
		edit.putBoolean("landscape_height_default", mDefault.isChecked());
	    else
		edit.putBoolean("portrait_height_default", mDefault.isChecked());	
	    edit.putInt("keyboard_height_change", 1);
	    edit.commit();
	} else {
	    if (mDefault.isChecked()) {
		mCurrentValue = mDefaultValue;
		persistInt(mCurrentValue);

		if (format == 0)
		    mStatusText.setText(String.valueOf(((float) mCurrentValue / (float) 10)) + "%");
		else
		    mStatusText.setText(String.valueOf(mCurrentValue));
		mSeekBar.setProgress(mCurrentValue - mMinValue);
	    }
	}
	if (mKey != null && mKey.length() > 0) {
	    SharedPreferences.Editor e = PreferenceManager.getDefaultSharedPreferences(mContext).edit();
	    e.putBoolean(mKey, mDefault.isChecked());
	    e.commit();
	}

	mDefaultChecked = mDefault.isChecked();
	if (mDefault != null) mDefault.setChecked(mDefaultChecked);
	if (mKey != null && mKey.length() == 0) {
	    if (mStatusText != null) mStatusText.setEnabled(!mDefaultChecked);
	    if (mSeekBar != null) mSeekBar.setEnabled(!mDefaultChecked);
	} else {
	    if (mStatusText != null) mStatusText.setEnabled(mDefaultChecked);
	    if (mSeekBar != null) mSeekBar.setEnabled(mDefaultChecked);
	}
    }

    private void setValuesFromXml(AttributeSet attrs) {
         mMaxValue = attrs.getAttributeIntValue(ANDROIDNS, "max", 100);
         mMinValue = attrs.getAttributeIntValue(FLATWORLDNS, "min", 0);
         mKey      = attrs.getAttributeValue(FLATWORLDNS, "enable_key");

	 if (mKey != null && mKey.length() > 0) {
	     boolean mDefaultEnabled = attrs.getAttributeBooleanValue(FLATWORLDNS, "default_enabled", true);
	     Log.i("Cangjie", "Default Enabled " + mDefaultEnabled);
	     mDefaultChecked = mDefaultEnabled;
	     if (mDefault != null) mDefault.setText(R.string.prefs_seek_enable);
	     if (attrs.getAttributeValue(FLATWORLDNS, "format").compareTo("int") == 0) {
		 format = 1;
	     }
	     final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(mContext);
	     Log.i("Cangjie", "Default Cursor " + prefs.getBoolean(mKey, mDefaultEnabled));
	     mDefaultChecked = prefs.getBoolean(mKey, mDefaultEnabled);
	 }

	 // Log.i("Cangjie", "Set Values " + mMaxValue + " " + mMinValue);
       
    //     try {
    //         String newInterval = attrs.getAttributeValue(FLATWORLDNS, "interval");
    //         if (newInterval != null) mInterval = Integer.parseInt(newInterval);
    //     } catch(Exception e) {
    //         Log.e(TAG, "Invalid interval value", e);
    //     }
       
    }
   
    private String getAttributeStringValue(AttributeSet attrs, String namespace, String name, String defaultValue) {
        String value = attrs.getAttributeValue(namespace, name);
        if (value == null) value = defaultValue;
        return value;
    }
   
    @Override
    protected View onCreateView(ViewGroup parent) {
        LinearLayout layout =  null;
       
        try {
            LayoutInflater mInflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            layout = (LinearLayout) mInflater.inflate(R.layout.seek_bar_preference, parent, false);
        } catch(Exception e) {
            Log.e(TAG, "Error creating seek bar preference", e);
        }
        
        return layout;
    }
   
    @Override
    public void onBindView(View view) {
        super.onBindView(view);

        try {
            // move our seekbar to the new view we've been given
            ViewParent oldContainer = mSeekBar.getParent();
            ViewGroup newContainer = (ViewGroup) view.findViewById(R.id.seekBarPrefBarContainer);
           
            if (oldContainer != newContainer) {
                // remove the seekbar from the old view
                if (oldContainer != null) {
                    ((ViewGroup) oldContainer).removeView(mSeekBar);
                }
                // remove the existing seekbar (there may not be one) and add ours
                newContainer.removeAllViews();
                newContainer.addView(mSeekBar, ViewGroup.LayoutParams.FILL_PARENT,
                        ViewGroup.LayoutParams.WRAP_CONTENT);
            }

            TextView tv = (TextView)view.findViewById(R.id.seekBarPrefValue);
            tv.setEnabled(isEnabled());
            tv = (TextView)view.findViewById(android.R.id.title);
            tv.setEnabled(isEnabled());
            tv = (TextView)view.findViewById(android.R.id.summary);
            tv.setEnabled(isEnabled());
            mSeekBar.setEnabled(isEnabled());
        } catch(Exception ex) {
            Log.e(TAG, "Error binding view", ex);
        }

        updateView(view);
    }
   
    /**
     * Update a SeekBarPreference view with our current state
     * @param view
     */
    protected void updateView(View view) {
        try {
            LinearLayout layout = (LinearLayout) view;

            mStatusText = (TextView) layout.findViewById(R.id.seekBarPrefValue);
	    if (format == 0)
		mStatusText.setText(String.valueOf(((float) mCurrentValue / (float) 10)) + "%");
	    else
		mStatusText.setText(String.valueOf(mCurrentValue));
            mStatusText.setMinimumWidth(30);
            mDefault    = (CheckBox) layout.findViewById(R.id.seekBarDefault);
	    mDefault.setChecked(mDefaultChecked);
	    mDefault.setOnClickListener(this);
	    if (mKey != null && mKey.length() > 0) mDefault.setText(R.string.prefs_seek_enable);

            mSeekBar.setProgress(mCurrentValue - mMinValue);
	    
	    updateValue();

	    if (mKey == null || (mKey != null && mKey.length()  == 0)) {
		mStatusText.setEnabled(!mDefaultChecked);
		mSeekBar.setEnabled(!mDefaultChecked);
	    } else {
		mStatusText.setEnabled(mDefaultChecked);
		mSeekBar.setEnabled(mDefaultChecked);
	    }
        } catch(Exception e) {
            Log.e(TAG, "Error updating seek bar preference", e);
        }
       
    }
   
    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        int newValue = progress + mMinValue;
       
        if (newValue > mMaxValue)
            newValue = mMaxValue;
        else if (newValue < mMinValue)
            newValue = mMinValue;
        else if (mInterval != 1 && newValue % mInterval != 0)
            newValue = Math.round(((float)newValue)/mInterval)*mInterval;  

        // change rejected, revert to the previous value
        if (!callChangeListener(newValue)) {
            seekBar.setProgress(mCurrentValue - mMinValue);
            return;
        }

	if (getKey().compareTo("portrait_height") == 0 ||
	    getKey().compareTo("landscape_height") == 0) {
	    SharedPreferences.Editor edit = mContext.getSharedPreferences("CangjiePreference", Context.MODE_PRIVATE).edit();
	    edit.putInt("keyboard_height_change", 1);
	    edit.commit();
	}

        // change accepted, store it
        mCurrentValue = newValue;
	if (format == 0)
	    mStatusText.setText(String.valueOf(((float) newValue / (float) 10)) + "%");
	else
	    mStatusText.setText(String.valueOf(mCurrentValue));
        persistInt(newValue);

	// Log.i("Cangjie", " On Progress Changed " + newValue);
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        notifyChanged();
    }

    @Override
    protected Object onGetDefaultValue(TypedArray ta, int index) {
	int orient = getContext().getResources().getConfiguration().orientation;
	int swdp   = getContext().getResources().getConfiguration().smallestScreenWidthDp;

	int defaultValue = ta.getInt(index, DEFAULT_VALUE);
	if (getKey() == null) return defaultValue;
	
	if (getKey().compareTo("portrait_height") == 0 ||
	    getKey().compareTo("landscape_height") == 0) {
	    if (defaultValue == 460) {
		if (orient ==  Configuration.ORIENTATION_LANDSCAPE) {
		    if (swdp >= 600 && swdp < 768) {
			return 460;
		    } else if (swdp >= 768) {
			return 460;
		    } else {
			return 460;
		    }
		} else {
		    if (swdp >= 600 && swdp < 768) {
			return 460;
		    } else if (swdp >= 768) {
			return 460;
		    } else {
			return 460;
		    }
		}
	    } else if (defaultValue == 618) {
		if (orient ==  Configuration.ORIENTATION_LANDSCAPE) {
		    if (swdp >= 600 && swdp < 768) {
			return 450;
		    } else if (swdp >= 768) {
			return 450;
		    } else {
			return 450;
		    }
		} else {
		    if (swdp >= 600 && swdp < 768) {
			return 350;
		    } else if (swdp >= 768) {
			return 350;
		    } else {
			return 618;
		    }
		}
	    }
	}

	return defaultValue;
    }

    @Override
    protected void onSetInitialValue(boolean restoreValue, Object defaultValue) {
	// Log.i("Cangjie", "on Set Initial Value " + defaultValue);
        if (restoreValue) {
            mCurrentValue = getPersistedInt(mCurrentValue);
        } else {
            int temp = 0;
            try {
                temp = (Integer) defaultValue;
            } catch(Exception ex) {
                Log.e(TAG, "Invalid default value: " + defaultValue.toString(), ex);
            }
           
            persistInt(temp);
            mCurrentValue = temp;
        }
       
    }
}
