package com.bluelesson.a24androidno1;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;

public class Main2Activity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);

        // 获取数据
        // 1. 获取Intent对象
        Intent localIntent = getIntent();
        // 2. 取出数据
        String pass = localIntent.getStringExtra("pass");
        // 3. 显示数据
        TextView localTextView = findViewById(R.id.tv_1);
        localTextView.setText("欢迎：" + pass);

    }
}
