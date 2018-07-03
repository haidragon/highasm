package com.bluelesson.a24androidno1;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;

public class MainActivity extends AppCompatActivity
    implements View.OnClickListener
{

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 1. 获取按钮对象
        Button localButton = findViewById(R.id.button);

        // 2. 设置监听器
        //localButton.setOnClickListener(this);
//        localButton.setOnClickListener(new MyOnClickListener());
        localButton.setOnClickListener(
                new View.OnClickListener() {
            @Override
            public void onClick(View argView) {
                // 1. 获取界面对象
                EditText localText = findViewById(R.id.et_password);

                // 2. 从界面对象中获取内容
                String pass = localText.getText().toString();

                // 3. 判断内容，完成验证
                if(pass.equals("15pb")){
//            // 1. 创建Toast对象
//            Toast toast = Toast.makeText(
//                   this, // this
//                   "恭喜你，成功了！", // 字符串
//                   Toast.LENGTH_SHORT // 短类型
//            );
//            // 2. 显示
//            toast.show();

                    // 1. 创建一个Intent对象，负责携带信息，传递给系统
                    Intent localIntent = new Intent();
                    // 2. 设置携带的信息
                    // 类信息
                    localIntent.setClass(
                            MainActivity.this, // 设置谁启动
                            Main2Activity.class   // 设置启动谁
                    );
                    // 设置数据
                    localIntent.putExtra("pass",pass);

                    // 3. 启动activty
                    startActivity(localIntent);
                 }
            }
        });

    }

    @Override
    public void onClick(View argView) {

    }

    public Handler mHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);

            if(msg.what == 0x111){
                TextView localTextView = findViewById(R.id.tv_1);
                localTextView.setText((String)msg.obj);
            } else if (msg.what == 0x222){

                ImageView localImageView = findViewById(R.id.iv_1);
                localImageView.setImageBitmap((Bitmap) msg.obj);
            }
        }
    };


    public void OpenURL(View view) {

        new Thread(){
            @Override
            public void run() {
                super.run();

                String url = "https://www.baidu.com";
                try {
                    // 1. 获取URL对象
                    URL localURL = new URL(url);

                    // 2. 获取连接对象
                    HttpURLConnection localConnection =
                            (HttpURLConnection) localURL.openConnection();
                    // 3. 设置连接的属性
                    localConnection.setConnectTimeout(5000);
                    localConnection.setRequestMethod("GET");

                    // 4. 获取应答内容
                    int code = localConnection.getResponseCode();
                    if(code == 200){
                        // 获取输入流
                        InputStream localStream = localConnection.getInputStream();
                        // 使用字节流对象接收输入流
                        ByteArrayOutputStream localStream1 = new ByteArrayOutputStream();
                        byte byteCode[] = new byte[1024];
                        int retSize = 0;
                        while(true){
                            retSize = localStream.read(byteCode);
                            if(retSize == -1)break;
                            localStream1.write(byteCode,0,retSize);
                        }
                        Log.d("1",localStream1.toString());
                        // 显示到界面上
                        //TextView localTextView = findViewById(R.id.tv_1);
                        //localTextView.setText(localStream1.toString());

                        Message localMessage = new Message();
                        localMessage.what = 0x111;
                        localMessage.obj = localStream1.toString();

                        mHandler.sendMessage(localMessage);

                    }

                } catch (Exception argE) {
                    argE.printStackTrace();
                }
            }
        }.start();

    }

    public void OpenJPG(View view) {

        new Thread(){
            @Override
            public void run() {
                super.run();

                String url = "http://ww3.sinaimg.cn/large/0073ob6Pgy1fswonrapfmj30yi1pcu0x.jpg";
                try {
                    // 1. 获取URL对象
                    URL localURL = new URL(url);

                    // 2. 获取连接对象
                    HttpURLConnection localConnection =
                            (HttpURLConnection) localURL.openConnection();
                    // 3. 设置连接的属性
                    localConnection.setConnectTimeout(5000);
                    localConnection.setRequestMethod("GET");

                    // 4. 获取应答内容
                    int code = localConnection.getResponseCode();
                    if(code == 200){
                        // 获取输入流
                        InputStream localStream = localConnection.getInputStream();

                        Bitmap localBitmap = BitmapFactory.decodeStream(localStream);

                        Message localMessage = new Message();
                        localMessage.what = 0x222;
                        localMessage.obj = localBitmap;

                        mHandler.sendMessage(localMessage);

                    }

                } catch (Exception argE) {
                    argE.printStackTrace();
                }
            }
        }.start();

    }

    // 创建一个内部类，MyOnClickListener
    class MyOnClickListener implements View.OnClickListener{

        @Override
        public void onClick(View argView) {

            // 1. 获取界面对象
            EditText localText = findViewById(R.id.et_password);

            // 2. 从界面对象中获取内容
            String pass = localText.getText().toString();

            // 3. 判断内容，完成验证
            if(pass.equals("15pb")){
//            // 1. 创建Toast对象
//            Toast toast = Toast.makeText(
//                   this, // this
//                   "恭喜你，成功了！", // 字符串
//                   Toast.LENGTH_SHORT // 短类型
//            );
//            // 2. 显示
//            toast.show();

                // 1. 创建一个Intent对象，负责携带信息，传递给系统
                Intent localIntent = new Intent();
                // 2. 设置携带的信息
                // 类信息
                localIntent.setClass(
                        MainActivity.this, // 设置谁启动
                        Main2Activity.class   // 设置启动谁
                );
                // 设置数据
                localIntent.putExtra("pass",pass);

                // 3. 启动activty
                startActivity(localIntent);


            }
        }
    }






    public void login(View view) {

        // 1. 获取界面对象
        EditText localText = findViewById(R.id.et_password);

        // 2. 从界面对象中获取内容
        String pass = localText.getText().toString();

        // 3. 判断内容，完成验证
        if(pass.equals("15pb")){
//            // 1. 创建Toast对象
//            Toast toast = Toast.makeText(
//                   this, // this
//                   "恭喜你，成功了！", // 字符串
//                   Toast.LENGTH_SHORT // 短类型
//            );
//            // 2. 显示
//            toast.show();

            // 1. 创建一个Intent对象，负责携带信息，传递给系统
            Intent localIntent = new Intent();
            // 2. 设置携带的信息
            // 类信息
            localIntent.setClass(this, // 设置谁启动
                    Main2Activity.class   // 设置启动谁
            );
            // 设置数据
            localIntent.putExtra("pass",pass);

            // 3. 启动activty
            startActivity(localIntent);


        }

    }
}
