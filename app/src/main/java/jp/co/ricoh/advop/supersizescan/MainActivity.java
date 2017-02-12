package jp.co.ricoh.advop.supersizescan;

import android.graphics.Bitmap;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.util.ArrayList;
import java.util.regex.Pattern;

import jp.co.ricoh.advop.mini.cheetahminiutil.logic.CHolder;
import jp.co.ricoh.advop.mini.cheetahminiutil.logic.JobManager;
import jp.co.ricoh.advop.mini.cheetahminiutil.logic.JobManager.EXE_RESULT;
import jp.co.ricoh.advop.mini.cheetahminiutil.logic.JobManager.ExeCallback;
import jp.co.ricoh.advop.mini.cheetahminiutil.ssdk.function.print.attribute.HashPrintRequestAttributeSet;
import jp.co.ricoh.advop.mini.cheetahminiutil.ssdk.function.print.attribute.PrintRequestAttributeSet;
import jp.co.ricoh.advop.mini.cheetahminiutil.ssdk.function.print.attribute.standard.Copies;
import jp.co.ricoh.advop.mini.cheetahminiutil.ssdk.function.print.attribute.standard.Magnification;
import jp.co.ricoh.advop.mini.cheetahminiutil.ssdk.function.scan.attribute.ScanRequestAttributeSet;
import jp.co.ricoh.advop.mini.cheetahminiutil.ssdk.function.scan.attribute.standard.ScanResolution;
import jp.co.ricoh.advop.mini.cheetahminiutil.tzl.CancelThread;
import jp.co.ricoh.advop.mini.cheetahminiutil.tzl.RegularUtils;
import jp.co.ricoh.advop.mini.cheetahminiutil.tzl.SendOrderBroadcast;
import jp.co.ricoh.advop.mini.cheetahminiutil.tzl.mail.MailUtil;
import jp.co.ricoh.advop.mini.cheetahminiutil.ui.BaseActivity;
import jp.co.ricoh.advop.mini.cheetahminiutil.ui.dialog.BaseDialog;
import jp.co.ricoh.advop.mini.cheetahminiutil.ui.dialog.MultiButtonDialog;
import jp.co.ricoh.advop.mini.cheetahminiutil.util.BaseDialogOnClickListener;
import jp.co.ricoh.advop.mini.cheetahminiutil.util.Buzzer;
import jp.co.ricoh.advop.mini.cheetahminiutil.util.HDDUtil;
import jp.co.ricoh.advop.mini.cheetahminiutil.util.ImageUtil;
import jp.co.ricoh.advop.mini.cheetahminiutil.util.LogC;

import static android.R.string.cancel;

public class MainActivity extends BaseActivity implements View.OnClickListener {

    public static final String SUCCESS = "success";
    private ImageView mScanSecond;
    private ImageView mScanFirst;
    private Button mScan;
    private Button mCombine;
    private Button mSend;
    private TextView mIntroduction;
    private EditText mEmail;
    private ImageView mCombineImage;
    //    private String mCombineResult = CHolder.instance().getApplication().getPathPackageFolder() + "/result.jpg";
    private String mCombineResult = CHolder.instance().getApplication().getPathPackageFolder() + "/result.jpg";
    private String mScanFistResult;
    private String mScanSecondResult;
    private ImageView mSelectedImageView;
    private String test1 = CHolder.instance().getApplication().getPathPackageFolder() + "/test1_300.jpg";
    private String test2 = CHolder.instance().getApplication().getPathPackageFolder() + "/test2_300.jpg";
    private boolean landFisrt = true;
    private boolean landSecond = true;


    static {
        System.loadLibrary("native-lib");
    }

    private ScanRequestAttributeSet scanJobAttributes;
    private String mTemp = CHolder.instance().getApplication().getPathPackageFolder() + "/test_temp.jpg";
//    private CombineTask combineTask;

    Handler mHandler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
//
        initView();

        bindEventListener();
        final MultiButtonDialog processingDlg = MultiButtonDialog.createNoButtonDialog(MainActivity.this, getString(R.string.init));
        processingDlg.show();
        CHolder.instance().getJobManager().initApp(new ExeCallback() {

            @Override
            public void onComplete(EXE_RESULT ret) {
                if (ret.equals(EXE_RESULT.SUCCESSED)) {
                    new AsyncTask<Void, Void, Boolean[]>() {
                        @Override
                        protected Boolean[] doInBackground(Void... params) {
                            return SendOrderBroadcast.getsInstance().checkScanPermission(CHolder.instance().getActivity());
                        }

                        @Override
                        protected void onPostExecute(Boolean[] permission) {
                            super.onPostExecute(permission);
                            processingDlg.dismiss();
                            if (!permission[0]) {
                                Buzzer.play(Buzzer.BUZZER_NACK);
                                MultiButtonDialog.createMsgDialog(CHolder.instance().getActivity(), getString(R.string.msg_dialog_no_permission), new BaseDialogOnClickListener() {
                                    @Override
                                    public void onWork(BaseDialog dialog) {
                                        Buzzer.play();
                                        dialog.dismiss();
                                        // TODO: 1/10/2017
                                        //disable button
                                        if (!CHolder.instance().getApplication().getmSystemStateMonitor().isMachineAdmin()) {
                                            finish();
                                        } else {
                                            mScan.setEnabled(false);
                                            mCombine.setEnabled(false);
                                            mSend.setEnabled(false);
                                        }
                                    }
                                }).show();
                            }
                        }
                    }.execute();
                } else {
                    LogC.i("init error:" + ret);
                    Toast.makeText(CHolder.instance().getActivity(), "init error:" + ret, Toast.LENGTH_LONG).show();
                }
            }
        });

    }

    private void initView() {
        mScanFirst = (ImageView) findViewById(R.id.scan_first);
        mScanSecond = (ImageView) findViewById(R.id.scan_second);
        mScan = (Button) findViewById(R.id.scan);
        mCombine = (Button) findViewById(R.id.combine);
        mSend = (Button) findViewById(R.id.send);
        mEmail = (EditText) findViewById(R.id.email);
        mIntroduction = (TextView) findViewById(R.id.introduction);
        mCombineImage = (ImageView) findViewById(R.id.combine_image);

        selectImageView(mScanFirst);
        mScanFirst.setOnClickListener(this);
        mScanSecond.setOnClickListener(this);
        mScan.setOnClickListener(this);
        mCombine.setOnClickListener(this);
        mSend.setOnClickListener(this);
        scanJobAttributes = CHolder.instance().getScanManager().createScanJobAttributes();
        scanJobAttributes.add(ScanResolution.DPI_300);

        HDDUtil.deleteFile(mCombineResult);
        HDDUtil.deleteFile(test1);
        HDDUtil.deleteFile(test2);
        mCombine.setEnabled(false);
//        mSend.setEnabled(false);
    }

    private void bindEventListener() {

    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    public boolean isFirst = true;

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.scan: {
                Buzzer.play();
                final MultiButtonDialog processingDlg = MultiButtonDialog.createNoButtonDialog(MainActivity.this, getString(R.string.dlg_scaning));
                processingDlg.show();

                CHolder.instance().getJobManager().startScan(new JobManager.ExeCallbackScan() {
                    @Override
                    public void onGetFilePath(String path, final int imgRotate) {
                        if (path == null) {
                            return;
                        }
//                        if (!correctRect(path, mTemp, 0)) {
//                            LogC.e("correct rect fail");
//                        }
                        mTemp = path;

                        final Bitmap thumbnail = ImageUtil.createBitmap(mTemp, 0, 10);
                        mSelectedImageView.setImageBitmap(thumbnail);
                        mSelectedImageView.setTag("setImageBitmap");
                        if (mSelectedImageView == mScanFirst) {
                            HDDUtil.copyFile(mTemp, test1);
                            selectImageView(mScanSecond);
                            mScanFistResult = test1;
                            landFisrt = imageIsLand(thumbnail);
                            mIntroduction.setText(R.string.txt_scan_second_hint);
                        } else if (mSelectedImageView == mScanSecond) {
                            HDDUtil.copyFile(mTemp, test2);
                            selectImageView(mScanFirst);
                            mScanSecondResult = test2;
                            landSecond = imageIsLand(thumbnail);
                            if (mScanFistResult != null && mScanSecondResult != null) {
                                mIntroduction.setText(R.string.txt_combine_image_hint);
                            } else {
                                mIntroduction.setText(R.string.txt_scan_first_hint);
                            }
                        }
                        if (mScanFistResult != null && mScanSecondResult != null) {
                            mCombine.setEnabled(true);
                        }
                    }

                    @Override
                    public void onComplete(EXE_RESULT ret) {
                        processingDlg.dismiss();
                    }
                }, scanJobAttributes);
                break;
            }
            case R.id.combine: {
                if (TextUtils.isEmpty(mScanFistResult) || TextUtils.isEmpty(mScanSecondResult)) {
                    Buzzer.play(Buzzer.BUZZER_NACK);
                    MultiButtonDialog.createMsgDialog(CHolder.instance().getActivity(), getString(R.string.dlg_combine_alert), new BaseDialogOnClickListener() {
                        @Override
                        public void onWork(BaseDialog dialog) {
                            Buzzer.play();
                            dialog.dismiss();

                        }
                    }).show();
                    break;
                }
                Buzzer.play();
                final SelectedDialog selectedDialog = new SelectedDialog(this, getData());
                final CombineTask combineTask = new CombineTask();
                selectedDialog.setListener(new AdapterView.OnItemClickListener() {
                    @Override
                    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                        Buzzer.play();
                        selectedDialog.dismiss();
                        if (position == 0) {
                            combineTask.execute(0);
                        } else {
                            combineTask.execute(90);
                        }
                        selectImageView(mScanFirst);
                    }
                });
                selectedDialog.show();
                break;
            }
            case R.id.send: {
//                if (mCombineResult == null || !(new File(mCombineResult).exists())) {
//                    Buzzer.play(Buzzer.BUZZER_ACK);
//                    final MultiButtonDialog dialog = MultiButtonDialog.createMsgDialog(this, getString(R.string.dlg_sending_email_alert), new BaseDialogOnClickListener() {
//                        @Override
//                        public void onWork(BaseDialog dialog) {
//                            Buzzer.play();
//                            dialog.dismiss();
//                        }
//                    });
//                    dialog.show();
//                    break;
//                }
                String IS_EMAIL_REGEX = "^[a-zA-Z0-9!#$%&'_`/=~\\*\\+\\-\\?\\^\\{\\|\\}]+(\\.[a-zA-Z0-9!#$%&'_`/=~\\*\\+\\-\\?\\^\\{\\|\\}]+)*[a-zA-Z0-9]@[a-zA-Z0-9][a-zA-Z0-9!#$%&'_`/=~\\*\\+\\-\\?\\^\\{\\|\\}]*(\\.[a-zA-Z0-9!#$%&'_`/=~\\*\\+\\-\\?\\^\\{\\|\\}]+)*$";
                String email = mEmail.getText().toString();
                if (!Pattern.matches(IS_EMAIL_REGEX, email)) {
                    Buzzer.play(Buzzer.BUZZER_ACK);
                    final MultiButtonDialog dialog = MultiButtonDialog.createMsgDialog(this, getString(R.string.dlg_email_invalid), new BaseDialogOnClickListener() {
                        @Override
                        public void onWork(BaseDialog dialog) {
                            Buzzer.play();
                            dialog.dismiss();
                        }
                    });
                    dialog.show();
                }
                Buzzer.play();
                final MultiButtonDialog dialog = MultiButtonDialog.createNoButtonDialog(this, getString(R.string.dlg_sending_email));
                dialog.show();
                new AsyncTask<String, Void, Integer>() {

                    @Override
                    protected Integer doInBackground(String... params) {
                        MailUtil mailUtil = new MailUtil();
                        int messageResourceId = mailUtil.send(params[0], new File("/mnt/hdd/result.jpg")).getFailReason().getMessageResourceId();
                        return messageResourceId;
                    }

                    @Override
                    protected void onPostExecute(Integer messageResourceId) {
                        super.onPostExecute(messageResourceId);
                        dialog.dismiss();
                        String result;
                        if (messageResourceId == R.string.mail_sended_success) {
                            result = getString(R.string.send_success);
                        } else if (messageResourceId == R.string.mail_smtp_server_setting_invalid) {
                            result = getString(R.string.msg_dialog_mail_smtp_server_setting_invalid);
                        } else if (messageResourceId == R.string.mail_smtp_server_access_failed) {
                            result = getString(R.string.msg_dialog_mail_smtp_server_access_failed);
                        } else if (messageResourceId == R.string.mail_auth_error) {
                            result = getString(R.string.msg_dialog_mail_smtp_auth_error);
                        } else {
                            result = getString(R.string.msg_dialog_mail_smtp_server_access_failed);
                        }
                        if (messageResourceId != R.string.mail_sended_success) {
                            Buzzer.play(Buzzer.BUZZER_NACK);
                            result = getString(R.string.send_fail);
                        }
                        MultiButtonDialog.createMsgDialog(CHolder.instance().getActivity(), result, new BaseDialogOnClickListener() {
                            @Override
                            public void onWork(BaseDialog dialog) {
                                Buzzer.play();
                                dialog.dismiss();
                                mIntroduction.setText(R.string.txt_scan_first_hint);
                            }
                        }).show();
                    }
                }.execute(mEmail.getText().toString());
                selectImageView(mScanFirst);
                break;
            }
            case R.id.scan_first: {
                Buzzer.play();
                selectImageView(mScanFirst);
                mIntroduction.setText(R.string.txt_scan_first_hint);
                break;
            }
            case R.id.scan_second: {
                Buzzer.play();
                selectImageView(mScanSecond);
                mIntroduction.setText(R.string.txt_scan_second_hint);
                break;
            }
        }

    }

    private boolean imageIsLand(Bitmap thumbnail) {
        return thumbnail.getWidth() > thumbnail.getHeight();
    }

    private void selectImageView(ImageView imageView) {
        LinearLayout layout;
        if (mSelectedImageView != null) {
            layout = (LinearLayout) mSelectedImageView.getParent();
            layout.setBackgroundResource(R.drawable.rect_line_n);
        }
        mSelectedImageView = imageView;
        layout = (LinearLayout) mSelectedImageView.getParent();
        layout.setBackgroundResource(R.drawable.rect_line_w);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        HDDUtil.deleteFile(mCombineResult);
        HDDUtil.deleteFile(test1);
        HDDUtil.deleteFile(test2);
    }

    class CombineTask extends AsyncTask<Integer, Void, String> {

        public static final String COMBINE_FAIL = "combine fail";
        private MultiButtonDialog dialog;

        @Override
        protected void onPreExecute() {
            super.onPreExecute();
            dialog = MultiButtonDialog.createCancelDialog(CHolder.instance().getActivity(), getString(R.string.dlg_combineing), new BaseDialogOnClickListener() {
                @Override
                public void onWork(BaseDialog dialog) {

                    Buzzer.play();
                    if (getStatus().equals(Status.FINISHED)) {
                        dialog.dismiss();
                        return;
                    }
                    cancel(true);
                    if (isCancelled()) {
                        dialog.dismiss();
                    }
                }
            });
            dialog.show();
//            CHolder.instance().getMainUIHandler().postDelayed(new Runnable() {
//                @Override
//                public void run() {
//                    if (dialog != null && dialog.isShowing())
//                        timeOut();
//                }
//            }, 30000);
        }

        @Override
        protected void onCancelled() {
            super.onCancelled();
            if (dialog != null && dialog.isShowing())
                dialog.dismiss();
        }

        public void timeOut() {
            cancel(true);
            onPostExecute(COMBINE_FAIL);
        }

        @Override
        protected String doInBackground(Integer... params) {

//                        scale(mScanFistResult, 3, 0);
//                        scale(mScanSecondResult, 3, 0);
            if (landSecond != landFisrt) {
                return COMBINE_FAIL;
            }
            if (combine2(mScanFistResult, mScanSecondResult, mCombineResult, params[0]) != 0) {
                return COMBINE_FAIL;
            }
            return SUCCESS;
        }

        @Override
        protected void onPostExecute(String aBoolean) {
            super.onPostExecute(aBoolean);
            dialog.dismiss();
            System.gc();
            if (!aBoolean.equals(SUCCESS)) {
                Buzzer.play(Buzzer.BUZZER_NACK);
                MultiButtonDialog.createMsgDialog(CHolder.instance().getActivity(), getString(R.string.dlg_combine_fail), new BaseDialogOnClickListener() {
                    @Override
                    public void onWork(BaseDialog dialog) {
                        Buzzer.play();
                        dialog.dismiss();
                    }
                }).show();
            } else {
                MultiButtonDialog.createMsgDialog(CHolder.instance().getActivity(), getString(R.string.dlg_combine_success), new BaseDialogOnClickListener() {
                    @Override
                    public void onWork(BaseDialog dialog) {
                        Buzzer.play();
                        dialog.dismiss();
                        Bitmap thumbnail = ImageUtil.createBitmap(mCombineResult, 90, 3);
                        mCombineImage.setImageBitmap(thumbnail);
                        mIntroduction.setText(R.string.txt_send_email_hint);
                        mSend.setEnabled(true);
                    }
                }).show();
            }
        }
    }

    public ArrayList<String> getData() {
        ArrayList<String> arrayList = new ArrayList<String>();
        arrayList.add(getString(R.string.horizontal));
        arrayList.add(getString(R.string.vertical));
        return arrayList;
    }

    public synchronized native int combine2(String mScanFistResult, String mScanSecondResult, String mCombineResult, int degree);

    public native String stringFromJNI();

    public native boolean correctRect(String file, String dst, int rotation);
}
