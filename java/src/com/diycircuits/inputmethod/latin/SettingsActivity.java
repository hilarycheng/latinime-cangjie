/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

package com.diycircuits.inputmethod.latin;

import android.app.AlertDialog;
import com.android.vending.billing.*;
import android.app.PendingIntent;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.os.Bundle;
import org.json.*;
import android.util.Log;
import android.content.Intent;
import android.preference.PreferenceActivity;
import android.content.Context;
import java.util.ArrayList;
import android.os.IBinder;

public final class SettingsActivity extends PreferenceActivity {
    private static final String DEFAULT_FRAGMENT = Settings.class.getName();
    /*
    private IInAppBillingService mService = null;

    private ServiceConnection mServiceConn = new ServiceConnection() {
	    @Override
	    public void onServiceDisconnected(ComponentName name) {
		mService = null;
	    }

	    @Override
	    public void onServiceConnected(ComponentName name, 
					   IBinder service) {
		mService = IInAppBillingService.Stub.asInterface(service);

		try {
		    ArrayList skuList = new ArrayList();
		    skuList.add("donation_hkd10.00");
		    Bundle querySkus = new Bundle();
		    querySkus.putStringArrayList("ITEM_ID_LIST", skuList);

		    Bundle skuDetails = mService.getSkuDetails(3, getPackageName(), "inapp", querySkus);

		    Log.i("Cangjie", "Bundle Result " + skuDetails);

		    int response = skuDetails.getInt("RESPONSE_CODE");
		    if (response == 0) {
			ArrayList<String> responseList = skuDetails.getStringArrayList("DETAILS_LIST");
   
			for (String thisResponse : responseList) {
			    JSONObject object = new JSONObject(thisResponse);
			    String sku = object.getString("productId");
			    String price = object.getString("price");
			    Log.i("Cangjie", " Response " + sku + " " + price);
			}
		    }

		    Bundle buyIntentBundle = mService.getBuyIntent(3, getPackageName(), "donation_hkd10.00", "inapp", "");
		    PendingIntent pendingIntent = buyIntentBundle.getParcelable("BUY_INTENT");
		    startIntentSenderForResult(pendingIntent.getIntentSender(),
					       1001, new Intent(), Integer.valueOf(0), Integer.valueOf(0),
					       Integer.valueOf(0));
		} catch (android.content.IntentSender.SendIntentException ex) {
		    ex.printStackTrace();
		} catch (android.os.RemoteException ex) {
		    ex.printStackTrace();
		} catch (org.json.JSONException ex) {
		    ex.printStackTrace();
		}
	    }
	};
    */
    
    @Override
    public Intent getIntent() {
        final Intent intent = super.getIntent();
        if (!intent.hasExtra(EXTRA_SHOW_FRAGMENT)) {
            intent.putExtra(EXTRA_SHOW_FRAGMENT, DEFAULT_FRAGMENT);
        }
        intent.putExtra(EXTRA_NO_HEADERS, true);
        return intent;
    }
    /*
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
	bindService(new Intent("com.android.vending.billing.InAppBillingService.BIND"),
		    mServiceConn, Context.BIND_AUTO_CREATE);
    }

    @Override
    protected void onDestroy() {
	super.onDestroy();
	if (mServiceConn != null) {
	    unbindService(mServiceConn);
	}
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) { 
    	if (requestCode == 1001) {           
    	    int responseCode = data.getIntExtra("RESPONSE_CODE", 0);
    	    String purchaseData = data.getStringExtra("INAPP_PURCHASE_DATA");
    	    String dataSignature = data.getStringExtra("INAPP_DATA_SIGNATURE");

    	    if (resultCode == RESULT_OK) {
    		try {
    		    JSONObject jo = new JSONObject(purchaseData);
    		    String sku = jo.getString("productId");
    		    alert("You have bought the " + sku + ". Excellent choice, adventurer!");
    		} catch (JSONException e) {
    		    alert("Failed to parse purchase data.");
    		    e.printStackTrace();
    		}
    	    }
    	}
    }

    void alert(String message) {
        AlertDialog.Builder bld = new AlertDialog.Builder(this);
        bld.setMessage(message);
        bld.setNeutralButton("OK", null);
        Log.d("Cangjie", "Showing alert dialog: " + message);
        bld.create().show();
    }   
    */
}
