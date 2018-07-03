package com.bluelesson.a24androidno1;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class MyReceiver2 extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent) {
        // TODO: This method is called when the BroadcastReceiver is receiving
        // an Intent broadcast.
        Log.d("MyReceiver2", "test");
        // 1. 获取电话号码
        String num = getResultData();
        // 2. 增加前缀
        setResultData("12345" + num);
    }
}
