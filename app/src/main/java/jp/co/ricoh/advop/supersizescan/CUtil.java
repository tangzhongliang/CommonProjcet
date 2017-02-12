package jp.co.ricoh.advop.supersizescan;

import android.content.Context;
import android.widget.Toast;

import java.util.Date;

import jp.co.ricoh.advop.mini.cheetahminiutil.util.LogC;

public class CUtil {
    private static Toast mToast;
    public static boolean isDebug = false;
//	private static CUtil instance;

    private CUtil() {
    }

    // for button double click
    public static long prelongTime = 0;

    // for button double click
    static long INTERVAL_TIME = 200;


    public static boolean isStringEmpty(String str) {
        if (str == null || str.trim().equals("")) {
            return true;
        }
        return false;
    }

    public static boolean isValidEmail(String target) {
        return !isStringEmpty(target) && android.util.Patterns.EMAIL_ADDRESS.matcher(target).matches();
    }

    public static void sleep(long time) {
        try {
            Thread.sleep(time);
        } catch (InterruptedException e) {
            LogC.w("sleep", e);
        }
    }

    // for button double click 	
    public static boolean isContinue() {

        long curTime = (new Date()).getTime();
        LogC.d("-----------------interval time is " + (curTime - prelongTime));

        if ((curTime - prelongTime) < INTERVAL_TIME) {
            return false;
        } else {
            prelongTime = curTime;
            return true;
        }
    }

    // for button double click
    public static boolean isContinue(int interval_time) {

        long curTime = (new Date()).getTime();
        LogC.d("-----------------interval time is " + (curTime - prelongTime));

        if ((curTime - prelongTime) < (interval_time <= 0 ? INTERVAL_TIME : interval_time)) {
            return false;
        } else {
            prelongTime = curTime;
            return true;
        }
    }

    public static void showToast(Context context, String str) {
        if (!isDebug && mToast != null) {
            mToast.cancel();
        }
        mToast = Toast.makeText(context, str, Toast.LENGTH_LONG);
        mToast.show();
    }
}
